# Research tooling (Python side of the quant workflow)

C++ owns determinism (matching, replay, execution, latency, network).
Python owns analysis (statistics, plots, model prototyping). The boundary is
the versioned CSV schemas (`mktdata_v1`, `features_v2`); Python never drives
the matching engine.

Scripts (all stdlib-only unless noted):

- `reference_features.py` — independent OFI/imbalance/microprice implementation
  over `data/golden/*.csv`. The C++-vs-Python differential: `arbiter-lab research
  features --csv data/golden/imbalance_case_001.csv` must agree with this
  script's hand-verified values. Run in CI (`scripts/reproduce_all.sh`).
- `analyze.py` — tables from `experiments/EXP-*/results.json`; `--plot` writes
  decay PNGs (needs matplotlib, optional).
- `calibrate.py` — distributional summaries of a real CSV for simulator
  calibration (compare manually with core `arbiter simstats`).

No third-party package is required. `analyze.py --plot` optionally uses
matplotlib. Run the actual export comparison with:

```sh
python3 tools/research/reference_features.py --compare ./build/arbiter-lab
```

This independently checks selected feature columns, not the complete model or
backtesting pipeline.
