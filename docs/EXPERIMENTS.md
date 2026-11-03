# Supporting experiments

## Supporting world experiments

`latency_grid`, `latency_sweep`, and `network_grid` validate divergence,
network recovery, and experiment plumbing while varying total latency or
transport conditions. `configs/fee_grid.axexp` checks the exact fee-to-filled-lot
accounting relationship. These are supporting tests, not evidence for the
fixed-total allocation hypothesis.

Run a file-defined grid and reproduce its output with:

```sh
./build/arbiter-lab experiment --config configs/fee_grid.axexp
./build/arbiter-lab experiment --reproduce artifacts/experiment_fee_grid.json
```

## Supporting research studies

- `EXP-001`: whether book imbalance predicts monetizable short-horizon moves.
- `EXP-002`: how execution latency erodes a fixed prediction stream.
- `EXP-003`: whether deeper-book features add value beyond top-of-book input.

Each tracked experiment directory contains a manifest, machine-readable result,
and generated report. The result can be rebuilt from the manifest:

```sh
./build/arbiter-lab research reproduce EXP-001
./build/arbiter-lab research reproduce EXP-002
./build/arbiter-lab research reproduce EXP-003
```

See [RESEARCH.md](RESEARCH.md) for the evaluation contract and
[REPRODUCIBILITY.md](REPRODUCIBILITY.md) for artifact guarantees.
