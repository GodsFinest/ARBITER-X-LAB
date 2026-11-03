# Research extension verification

Supported core 1.1.x/API 1; the tested core revision is recorded in CORE_REVISION.
C++23, Apple M1/Apple Clang 17, Release -Werror. Python helpers use the standard
library; optional plots require matplotlib.

29 C++ cases (26 quant,3 grid) and 11 CTest entries passed through source and
installed providers. The installed package was relocated and the core source
checkout hidden; all three original EXP artifacts reproduced exactly. The final
shared-override direct calls were verified again through both providers and
ASan/UBSan. The combined core/LAB sanitizer profile passed 32 entries; final LAB
rechecks passed 11. Core's docs/VALIDATION.md describes verification coverage.

The original EXP-001/002/003 records remain byte-identical to the original
monolith. Normalized-data/manifest/result identity tags are unchanged. Quality
reporting now retains parser rejections and stream validation is idempotent;
regressions failed before the fix and pass afterward. Python independently
compares selected actual C++ export columns, not complete models or backtests.

Core 17,281 source lines; LAB 6,616, using tracked C++/Python/shell physical lines,
including comments/blanks/tests. Data/model/backtest semantics remain approximate;
EXP-002 retains its separate actual World latency pair. Original negative/null
findings are preserved. The thin calibration CLI wrapper was retired; Python
CSV summaries and core simstats retain the useful diagnostic behavior.

GitHub Actions checks Clang 18 source integration, Apple Clang installed-package
integration, and ASan/UBSan. The pinned core revision is fetched explicitly;
provenance metadata does not enter numerical identity hashes.
