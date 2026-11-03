# Source provenance

Extracted from https://github.com/GodsFinest/ARBITER-X at
65b9fa7803c5b888cdea777295683a7c06ef14f6 (MIT). Existing notices retained.
Core preparation extracts the shared override and build metadata utilities.
Namespaces/includes/build/CLI are adapted; historical results are unchanged.
The original archive remains available in core history.

| Original path | LAB path | Original Git blob |
|---|---|---|
| include/arbiter/data.hpp | include/arbiter_lab/data.hpp | `9a6a1d0e5085d240e75e4d077660de1ed5731072` |
| src/data/data.cpp | src/data/data.cpp | `460abc237cf8c343fe7324041a7d595627d565d6` |
| include/arbiter/features.hpp | include/arbiter_lab/features.hpp | `640f0ddcd20190857d6bb95eb686066678d13369` |
| src/research/features.cpp | src/research/features.cpp | `1a0e5ff1a5efc8526b9e4893a538ee29096cc0e8` |
| include/arbiter/labels.hpp | include/arbiter_lab/labels.hpp | `a52d6cd4fd07a47f2cfd8b5dd34d0ce245936943` |
| src/research/labels.cpp | src/research/labels.cpp | `46509f14b7ab5f33ed52d79d0bd4d8933b554edd` |
| include/arbiter/quant_model.hpp | include/arbiter_lab/quant_model.hpp | `87c12f9530f8c016cec86c18a668fca664ca738d` |
| src/research/quant_model.cpp | src/research/quant_model.cpp | `ea63684058b2d09cab314437128403d07a3355d4` |
| include/arbiter/execution_research.hpp | include/arbiter_lab/execution_research.hpp | `ec7bfc2033f20b2d2311893e84970a72d111eb77` |
| src/research/execution_research.cpp | src/research/execution_research.cpp | `44037e5342133223fed3288062bd85e100d13735` |
| include/arbiter/pipeline.hpp | include/arbiter_lab/pipeline.hpp | `7cef808f4ac109435ed4382d2443cd7e7d7c30d8` |
| src/research/pipeline.cpp | src/research/pipeline.cpp | `5a278b961dc81e70417d141b5f2864019cb7285a` |
| include/arbiter/research.hpp | include/arbiter_lab/research.hpp | `bf35dc36ef1855eb895be9057b9967fe1b590f10` |
| src/research/research.cpp | src/research/research.cpp | `b58ee531181e0fff07d7cc0dfe5047e8b8a051a7` |
| include/arbiter/experiment.hpp | include/arbiter_lab/experiment.hpp | `020104157e8ee4c7081ce4ed90d4752ddec1b419` |
| src/counterfactual/experiment.cpp | src/experiment/experiment.cpp | `0f6be8ada0a08956adb6ab510b529f6083aeb558` |
| src/cli/research_cli.cpp | src/cli/research_cli.cpp | `903bab3b5c98b94e5e03d92540f733d64b8f1b39` |
| tests/test_quant.cpp | tests/test_quant.cpp | `212c7877e19db5c2cac92653a65a6294e162998f` |
| tests/testfw.hpp | tests/testfw.hpp | `6cc74e1fa5428844bfcfa2f69734fe099e191dd0` |
| LICENSE | LICENSE | `14b28c3572ab893c5c1f15adb3da71fb97d6b049` |
| .gitignore | .gitignore | `1ed4b01666d32a05e1363afe093c772c1ed64c4b` |
| configs/fee_grid.axexp | configs/fee_grid.axexp | `03126f63f606a4aacf02557e979c8b570cdb813f` |
| configs/latency_grid.axexp | configs/latency_grid.axexp | `c34b67123ccea62426a0212fbb3ae7fad8448df5` |
| data/README.md | data/README.md | `e15347451514cba6d7cca1f78de2e507dea2538b` |
| data/golden/README.md | data/golden/README.md | `478e06c1774ee034ae0c3e1e2992cab243c1d48b` |
| data/golden/imbalance_case_001.csv | data/golden/imbalance_case_001.csv | `404f01ec567cb87c1da0bab189a794fc648e8d8b` |
| data/golden/ofi_case_001.csv | data/golden/ofi_case_001.csv | `8aeae53a164bcfd2143ea3ff708258636e816b34` |
| data/golden/simple_book_001.csv | data/golden/simple_book_001.csv | `0b573027633df1dbdc71f9cabd766562ebfcc376` |
| docs/DATA.md | docs/DATA.md | `52288b188bd01062066247bd6e7d9b8e29e8567c` |
| docs/RESEARCH.md | docs/RESEARCH.md | `efae874d02f50094014db5d0cf4064c88aae45bd` |
| experiments/EXP-001/README.md | experiments/EXP-001/README.md | `8ecca4a23636e15fabdf5bd5314eeb307c6be283` |
| experiments/EXP-001/manifest.json | experiments/EXP-001/manifest.json | `46a3e402aa40e2f85c4c0d6333bd939bcdb14008` |
| experiments/EXP-001/report.md | experiments/EXP-001/report.md | `5e9f4455ddfdf3013c7169ce0160cfa2e1278249` |
| experiments/EXP-001/results.json | experiments/EXP-001/results.json | `75b4b2eb4008e3040cde991f2ef1bca321903a44` |
| experiments/EXP-002/README.md | experiments/EXP-002/README.md | `8e44fced402e97f68baa07f14167e26154f0abef` |
| experiments/EXP-002/manifest.json | experiments/EXP-002/manifest.json | `8675839f17d213c4bc3752561da64d1c7a382996` |
| experiments/EXP-002/report.md | experiments/EXP-002/report.md | `dde5d7ca5ebc70519407b6f74dbe1552b7388108` |
| experiments/EXP-002/results.json | experiments/EXP-002/results.json | `c33f8818341144642332c7ccad1b790b9166c5b7` |
| experiments/EXP-003/README.md | experiments/EXP-003/README.md | `01622f324588d61f53d760e73a0af9712bdb3f74` |
| experiments/EXP-003/manifest.json | experiments/EXP-003/manifest.json | `574383cd204c68a885ed70123b863f050db003f4` |
| experiments/EXP-003/report.md | experiments/EXP-003/report.md | `87490d3f16db74a49bb1843d8595f843c969210e` |
| experiments/EXP-003/results.json | experiments/EXP-003/results.json | `bfefda19d41335257541abe6e65f11f153346622` |
| tools/research/README.md | tools/research/README.md | `7fb89b5df9c008f9fd399484d993f75c8b432741` |
| tools/research/analyze.py | tools/research/analyze.py | `da34bae8d632750e02a1c91f61d109a3570fc99a` |
| tools/research/calibrate.py | tools/research/calibrate.py | `2bef658c746bae28dfe878cb3cee1bcd4524ae2f` |
| tools/research/reference_features.py | tools/research/reference_features.py | `c6bf82789ad8509da87a02d21de3e05560a39b59` |
| src/cli/main.cpp (relevant sections) | src/cli/experiment_cli.cpp | `9c70f906089205fb7c6cbc2e3fc6fee89d62e61b` |
| tests/test_world.cpp (relevant sections) | tests/test_experiment.cpp | `48342718be9404d24b2d57444b3c728e75c6b354` |
| benchmarks/bench_main.cpp (relevant sections) | benchmarks/bench_main.cpp | `d09d0714481dd37555b9472243a2a0ddebff483b` |
| docs/COUNTERFACTUALS.md (relevant sections) | docs/COUNTERFACTUALS.md | `25ed7aaeab0d75596f73f243b06b351adc6907fb` |
| docs/EXPERIMENTS.md (relevant sections) | docs/EXPERIMENTS.md | `90a2afae9a7076cbeb16c87b88f6012cf6c5d456` |
| docs/REPRODUCIBILITY.md (relevant sections) | docs/REPRODUCIBILITY.md | `dfc9e70e8b3157963d6a172fcf44281eed77dd0a` |

The small CMake provenance generator is shared with core.
Quality reporting and modeling/utility descriptions are adapted for LAB.
Original EXP records remain byte-identical to the monolith.
