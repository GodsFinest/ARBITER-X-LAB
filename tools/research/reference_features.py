#!/usr/bin/env python3
"""Reference (stdlib-only) feature implementation for differential testing.

Recomputes imbalance / mid / microprice / OFI from a golden market-data CSV
using the documented L2 level-set semantics (docs/DATA.md) and checks the
hand-verified expectations. This is the Python side of the C++-vs-Python
differential: `arbiter research features` must agree with this script.

Usage: python3 tools/research/reference_features.py [csv ...]
Exit 0 iff all expectations hold. Zero third-party dependencies.
"""
import csv
import math
import sys

GOLDEN_DIR = "data/golden"


def load(path):
    evs = []
    with open(path) as f:
        for row in csv.DictReader(filter(lambda r: not r.startswith("#"), f)):
            evs.append(row)
    return evs


def book_state(evs):
    bids, asks = {}, {}
    for e in evs:
        if e["type"] in ("depth_l2", "quote_l1"):
            (bids if e["side"] == "bid" else asks)[int(e["price"])] = int(e["qty"])
        elif e["type"] == "trade":
            px, q = int(e["price"]), int(e["qty"])
            book = asks if e["side"] == "buy" else bids
            if px in book:
                book[px] = max(0, book[px] - q)
    # cross-clear (same rule as pipeline::build_series)
    while bids and asks and max(bids) >= min(asks):
        # remove crossed opposite levels (asks <= best bid)
        bb = max(bids)
        asks = {p: q for p, q in asks.items() if p > bb}
        if bids and asks and max(bids) >= min(asks):
            ba = min(asks)
            bids = {p: q for p, q in bids.items() if p < ba}
        else:
            break
    return bids, asks


def top_qty(book, n, reverse):
    return sum(q for _, q in sorted(book.items(), reverse=reverse)[:n])


def imbalance(bids, asks, k):
    b = top_qty(bids, k, True)
    a = top_qty(asks, k, False)
    return 0.0 if b + a == 0 else (b - a) / (b + a)


def check_imbalance_case():
    evs = load(f"{GOLDEN_DIR}/imbalance_case_001.csv")
    bids, asks = book_state(evs[:4])  # pre-trade book (the trade row consumes 10 lots)
    bb, ba = max(bids), min(asks)
    mid = (bb + ba) / 2
    assert abs(mid - 1500.5) < 1e-12, mid
    assert abs(imbalance(bids, asks, 1) - 1 / 3) < 1e-12
    assert abs(imbalance(bids, asks, 2) - (-50 / 350)) < 1e-12
    bq = bids[bb]
    aq = asks[ba]
    mp = (ba * bq + bb * aq) / (bq + aq)
    assert abs(mp - (1501 * 100 + 1500 * 50) / 150) < 1e-9, mp
    print("imbalance_case_001 OK  mid=1500.5 imb1=0.3333 imb2=-0.14286 microprice=1500.6667")


def check_ofi_case():
    evs = load(f"{GOLDEN_DIR}/ofi_case_001.csv")
    # Cont OFI over successive top-of-book states
    prev = None
    total = 0
    bids, asks = {}, {}
    for e in evs:
        (bids if e["side"] == "bid" else asks)[int(e["price"])] = int(e["qty"])
        if not bids or not asks:
            continue
        cur = (max(bids), bids[max(bids)], min(asks), asks[min(asks)])
        if prev is not None:
            pb, pbq, pa, paq = prev
            cb, cbq, ca, caq = cur
            en = 0
            if cb > pb:
                en += cbq
            elif cb == pb:
                en += cbq - pbq
            else:
                en -= pbq
            if ca < pa:
                en -= caq
            elif ca == pa:
                en -= caq - paq
            else:
                en += paq
            total += en
        prev = cur
    assert total == 80, total
    print("ofi_case_001 OK  lifetime OFI=+80")


def compare_cpp(executable):
    """Compare independent per-event calculations with the real C++ CSV export."""
    import subprocess
    import tempfile
    from pathlib import Path
    for fixture in ("imbalance_case_001.csv", "ofi_case_001.csv"):
        source = f"{GOLDEN_DIR}/{fixture}"
        events = load(source)
        expected, history, previous = [], [], None
        for index, event in enumerate(events):
            bids, asks = book_state(events[:index + 1])
            bb, ba = max(bids, default=0), min(asks, default=0)
            bq, aq = bids.get(bb, 0), asks.get(ba, 0)
            current = bb, bq, ba, aq
            contribution = 0
            if previous is not None:
                pb, pbq, pa, paq = previous
                contribution += bq if bb > pb else (-pbq if bb < pb else bq - pbq)
                contribution += -aq if ba < pa else (paq if ba > pa else paq - aq)
            previous = current
            ts = int(event["exchange_ts_ns"])
            history.append((ts, contribution))
            if not bb or not ba:
                continue
            mid = (bb + ba) / 2
            row = {"ts_ns": ts, "mid": mid, "spread": ba - bb,
                   "micro_dev": (ba*bq + bb*aq)/(bq+aq) - mid,
                   "ofi_10ms": sum(v for t, v in history if ts-10_000_000 < t <= ts)}
            for depth in (1, 2, 5, 10, 20):
                row[f"imb_{depth}"] = imbalance(bids, asks, depth)
            expected.append(row)
        with tempfile.TemporaryDirectory(prefix="arbiter-python-diff-") as directory:
            output = Path(directory) / "features.csv"
            subprocess.run([executable, "research", "features", "--csv", source,
                            "--out", str(output)], check=True, capture_output=True)
            actual = load(output)
        assert len(expected) == len(actual), (fixture, len(expected), len(actual))
        for index, (want, got) in enumerate(zip(expected, actual)):
            for field, value in want.items():
                assert math.isclose(float(got[field]), value, rel_tol=1e-8, abs_tol=1e-8), (fixture, index, field, value, got[field])
        print(f"C++/Python export comparison: {fixture} {len(expected)} rows OK")


def main():
    check_imbalance_case()
    check_ofi_case()
    if len(sys.argv) > 1:
        if len(sys.argv) != 3 or sys.argv[1] != "--compare":
            raise SystemExit("usage: reference_features.py [--compare ARBITER_LAB_EXECUTABLE]")
        compare_cpp(sys.argv[2])
    print("reference_features: all requested checks passed")


if __name__ == "__main__":
    main()
