# Research pipeline

LAB provides historical-data research and optional World experiment grids. It is designed to reject weak claims rather than optimize a backtest.

## Data contract

`mktdata_v1` CSV input is validated, normalized, and hashed before use. The
repository includes small hand-checkable fixtures; external datasets must be
provided by the user and are never bundled. Sequence gaps and session
boundaries are preserved rather than interpolated.

## Evaluation flow

```text
normalized L2 events
  -> as-of features
  -> forward labels
  -> chronological splits
  -> deterministic baseline/logistic/stump models
  -> dependence-aware statistics
  -> latency- and queue-aware execution
  -> fees, slippage, markouts, and net economics
  -> hashed manifest, result, and report
```

Features include multi-level imbalance, microprice, order-flow imbalance,
signed flow and realized volatility. Leakage tests mutate future
data and require earlier features to remain unchanged. Labels cover multiple
forward horizons; the previous-return baseline uses only backward information.

Chronological train/validation/test splits and a sealed held-out tail prevent
random-shuffle leakage. Reported uncertainty includes HAC intervals for
overlapping samples, non-overlapping estimates, permutation nulls, and
multiple-testing correction where applicable.

## Execution economics

L2 data cannot reveal exact queue position. Every study therefore evaluates
conservative, neutral, and aggressive queue assumptions and reports the spread
between them. Decisions execute against the book at `signal_time + latency`,
then account for spread, fee, slippage, missed fills, and markout. Gross and net
results remain separate.

## Falsification rules

- A confidence interval spanning zero is inconclusive, not evidence of alpha.
- A result that fails on the sealed tail is reported as unstable.
- Simulation findings are properties of the configured model, not claims about
  a specific venue or real market.
- Historical fills are modeled. Core World subexperiments use actual exchange
  matching and authoritative accounting; their outcomes are reported separately.
- Negative and uncertain findings remain in generated reports.

The Python scripts in `tools/research/` independently check selected features,
summarize result JSON, and support calibration. The deterministic engine and
artifact hashes remain owned by C++.

The historical study currently uses fixed queue inputs (ahead=50 and
traded-through=60) and interpolated mid-price execution assumptions. These are
sensitivity-model choices, not inferred venue queue positions. EXP-002 retains
a separate true World latency pair; it does not turn the historical backtest
into an exchange-matching simulation.
