#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
BUILD_DIR=${ARBITER_LAB_BUILD_DIR:-build-repro}
cmake --fresh -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release \
  -DARBITER_WARNINGS_AS_ERRORS=ON "-DARBITER_X_SOURCE_DIR=${ARBITER_X_SOURCE_DIR:-}"
cmake --build "$BUILD_DIR" --parallel "${CMAKE_BUILD_PARALLEL_LEVEL:-4}"
ctest --test-dir "$BUILD_DIR" --output-on-failure
for experiment in EXP-001 EXP-002 EXP-003; do
  "$BUILD_DIR/arbiter-lab" research reproduce "$experiment"
done
"$BUILD_DIR/arbiter-lab" research validate --csv data/golden/imbalance_case_001.csv
mkdir -p artifacts
"$BUILD_DIR/arbiter-lab" research features --csv data/golden/imbalance_case_001.csv --out artifacts/features.csv
"$BUILD_DIR/arbiter-lab" experiment --config configs/latency_grid.axexp
"$BUILD_DIR/arbiter-lab" experiment --reproduce artifacts/experiment_latency_grid.json
if command -v python3 >/dev/null 2>&1; then
  python3 tools/research/reference_features.py --compare "$BUILD_DIR/arbiter-lab"
  python3 tools/research/analyze.py experiments/EXP-001
fi
"$BUILD_DIR/arbiter_lab_bench" --quick
