# General World experiments

These grids use the public core World and shared override parser. They vary
total latency/fees/faults; fixed-total allocation remains in core.

## Prior latency experiments (superseded as evidence for H0-allocation)

The supporting latency grid/sweep (`arbiter-lab experiment`, `configs/*.axexp`)
vary TOTAL latency. They answer "does
the total matter", which is a different question from the flagship's "does
placement matter at a fixed total". They remain valid infrastructure
validation and are kept as supporting records; they must NOT be cited as
evidence for or against allocation non-fungibility.

Classic measured results (seed 2026, re-baselined after the engine's
transport/risk-ledger repairs; regenerate with
`arbiter-lab experiment --seed 2026 --ms 40`, never hand-edit):

| variant | events | trades | MM fills | MM PnL (ticks*qty) | first divergent checkpoint |
|---|---|---|---|---|---|
| baseline_800ns | 14183 | 1014 | 14 | 150 | identical |
| fast_300ns | 14182 | 1014 | 14 | 150 | @checkpoint(event 512) |
| slow_3000ns | 14178 | 1022 | 11 | 125 | @checkpoint(event 512) |
| jittery_md | 14354 | 1014 | 14 | 150 | @checkpoint(event 256) |
| lossy_md_1pct | 14227 | 1014 | 14 | 150 | @checkpoint(event 256) |

Reading: state divergence does not imply economic divergence. The reported
value is the first differing hash CHECKPOINT (an upper bound on where the
histories split), not the exact causal event.

## Experiment configuration

Experiments are defined in two equivalent ways:

1. **File-driven** (preferred): a `.axexp` file using the sectioned
   `key = value` grammar, run with `arbiter-lab experiment --config FILE`.
   Unknown keys are LOUD failures, never ignored. The same override-table
   mechanism covers the new research variables
   (`sim.taker_self_excitation`, `bots_observe_shadow`, `stage_tracing`,
   `momentum.id_base`).

2. **Built-in families**: `arbiter-lab experiment --name
   latency_grid|latency_sweep|network_grid`.

Every artifact records the full definition (seed, horizon,
definition_hash, per-variant override sets) so `arbiter-lab experiment
--reproduce ARTIFACT.json` re-runs the exact grid and verifies every hash
bit-for-bit when compiler and platform provenance match.
