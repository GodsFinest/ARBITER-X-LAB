# ARBITER-X-LAB

Optional historical-data research and general experiment grids for
[ARBITER-X](https://github.com/GodsFinest/ARBITER-X). C++23, CMake 3.24+;
core 1.1.x / API generation 1. The tested core revision is in `CORE_REVISION`.
Core runs independently. LAB links its public library and never copies its engine.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DARBITER_X_SOURCE_DIR=/absolute/path/to/arbiter-x
cmake --build build --parallel 4
ctest --test-dir build --output-on-failure
./build/arbiter-lab --version
./build/arbiter-lab research validate --csv data/golden/imbalance_case_001.csv
mkdir -p artifacts
./build/arbiter-lab research features --csv data/golden/imbalance_case_001.csv --out artifacts/features.csv
./build/arbiter-lab research run EXP-001 --out artifacts/EXP-001
./build/arbiter-lab research reproduce EXP-001 --artifact-dir artifacts/EXP-001
./build/arbiter-lab experiment --config configs/latency_grid.axexp
```

For an installed core, replace `ARBITER_X_SOURCE_DIR` with
`CMAKE_PREFIX_PATH=/absolute/install/prefix`. Use the same compiler, standard
library, architecture and compatible build configuration. No binary ABI promise
across toolchains. No dependencies are fetched automatically.

Run `research reproduce EXP-001`, `EXP-002`, and `EXP-003` from this checkout
for the original committed records. Reproduction never overwrites them.
Matching compiler/platform uses exact hashes; other fingerprints use the existing
1e-9 relative floating tolerance and exact integer counters, explicitly reported.
CSV manifests retain literal input paths; those inputs must remain available.

The library provides ingestion, logged normalization, as-of features, forward
labels, chronological/held-out evaluation, logistic and stump models, statistical
checks, three queue assumptions and cost accounting. Python tools require Python 3.8+ and use the standard
library; optional plots require matplotlib. See docs/DATA.md and docs/RESEARCH.md.

Historical fills use an L2 approximation, including assumed queue positions and
interpolated prices. They are not exchange-matching fills. EXP-002 also runs a
separate actual `World` latency pair through core. Synthetic controls contain
known injected alpha; none establish real-market trading profitability.

EXP-001/002/003 retain original negative/null findings. General latency sweeps
vary total latency and are not evidence for the fixed-total allocation study,
which stays in core. `tools/research/calibrate.py` summarizes CSV distributions;
it does not fit simulator parameters automatically. Use core `simstats` for
world diagnostics.

See ORIGIN.md for source mapping, baseline revision and MIT attribution.

The published `research run` synthetic design uses generator seed 2026, 6000
generation steps and bias 0.6. Its `--seed` changes the study seed, not that
fixed generator design; manifests record both. Generic feature/evaluation
commands expose their generator seed separately through their own `--seed`.
