# Research reproduction

## Research manifests (`ARBITERX/RESEARCH-MANIFEST/3`, `.../RESULT/1`)

Each `experiments/EXP-*/` holds `manifest.json` (run/dataset/config/seed/
feature/model/code identity plus Git/compiler/platform provenance),
`results.json` (metrics/counters/findings + `result_hash`), and `report.md`.
The manifest records `git_commit` AND a `git_dirty` marker: when the working
tree had uncommitted modifications at generation time, the commit hash alone
does NOT identify the generating code, and the marker says so explicitly (an
artifact never implies that an older commit contains newer working-tree
code). The deterministic manifest hash excludes informational build
provenance (including the dirty marker), so a documentation-only commit does
not invalidate an otherwise identical result.
Doubles are hashed through a stable 17-digit decimal representation. `arbiter-lab
research reproduce EXP-00X` uses exact `result_hash` equality when compiler and
platform provenance match. Across a different compiler or platform, bitwise
floating-point equality is not claimed: metric names and order must match,
integer counters remain exact, and floating metrics must satisfy
`|stored-rebuilt| <= 1e-9 * max(1, |stored|, |rebuilt|)`. The command prints a
distinct cross-platform result, reports the first numeric difference even when
it is tolerated, and names the first field outside that contract.

## What to do when reproduction fails

1. Compare `manifest_hash` first (input divergence) vs `result_hash` (same-build
   code drift) or the first field-level mismatch (cross-platform drift).
2. Compare `git rev-parse` with manifest `git_commit` (+ `git_dirty`), then
   compiler/platform provenance.
3. `experiment --reproduce` names the first mismatching variant for World-leg
   artifacts. Never "fix" an artifact by hand, regenerate from the pipeline.
