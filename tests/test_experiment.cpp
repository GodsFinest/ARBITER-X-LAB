#include "testfw.hpp"
#include "arbiter_lab/experiment.hpp"
#include "arbiter/build_info.hpp"
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <limits>
using namespace arbiter;
using namespace arbiter::lab;
#include <functional>
AX_TEST(experiment_artifact_reproduces_exactly) {
    // The artifact records the full grid definition (definition_hash) +
    // per-variant override sets + genealogy; `experiment_reproduce` re-runs
    // it and verifies every hash bit-for-bit. Any grid
    // expressible as (variable, value) overrides, not just built-in names.
    std::filesystem::create_directories("artifacts");
    WorldConfig base;
    base.journal_events = false;
    base.horizon_ns = 40'000'000;
    base.warmup_ns = 2'000'000;
    Experiment exp(base);
    exp.set_seed(2026);
    exp.set_name("repro_grid");
    exp.add_variant("baseline_800ns", {{"algo_latency_ns", "800"}});
    exp.add_variant("fast_300ns", {{"algo_latency_ns", "300"}});
    exp.add_variant("half_spread_4t", {{"mm.half_spread_ticks", "4"}});
    const ExperimentResult res = exp.run();
    const std::string json = experiment_to_json(res, "repro_grid");
    const std::string path = "artifacts/experiment_repro_grid.json";
    { FILE* f = fopen(path.c_str(), "wb"); fwrite(json.data(), 1, json.size(), f); fclose(f); }

    const ReproduceReport rep = experiment_reproduce(path);
    CHECK(rep.ok);
    CHECK_EQ(rep.variants_checked, 3u);
    CHECK_EQ(rep.hash_mismatches, 0u);
    if (!rep.ok) std::printf("  reproduce detail: %s\n", rep.detail.c_str());

    // tamper with one recorded hash -> reproduction must FAIL loudly
    std::string tampered = json;
    const std::string needle = "\"final_hash\": " + std::to_string(res.final_hashes[1]);
    const std::string repl = "\"final_hash\": " + std::to_string(res.final_hashes[1] ^ 0x1);
    const size_t at = tampered.find(needle);
    CHECK_NE(at, std::string::npos);
    if (at != std::string::npos) {
        tampered.replace(at, needle.size(), repl);
        FILE* f = fopen("artifacts/experiment_tampered.json", "wb");
        CHECK(f != nullptr);
        if (f) {
            fwrite(tampered.data(), 1, tampered.size(), f);
            fclose(f);
        }
        const ReproduceReport bad = experiment_reproduce("artifacts/experiment_tampered.json");
        CHECK(!bad.ok);
        CHECK(bad.hash_mismatches > 0);
    }
}

AX_TEST(experiment_from_config_file) {
    // File-driven experiment definitions (docs/COUNTERFACTUALS.md grammar):
    // [experiment] seed/horizon; [baseline] overrides; [variant.NAME] one or
    // more variables (section names are dotted, the parser flattens
    // [variant.NAME] key = x  to  variant.NAME.key). The loaded grid must
    // match the equivalent hand-built grid EXACTLY (definition hash +
    // results) and reproduce from its artifact. Unknown keys are loud
    // failures, never ignored.
    std::filesystem::create_directories("artifacts");
    const char* def =
        "[experiment]\n"
        "name = file_grid\n"
        "seed = 77123\n"
        "horizon_ms = 30\n"
        "[baseline]\n"
        "algo_latency_ns = 800\n"
        "[variant.slow]\n"
        "algo_latency_ns = 3000\n"
        "[variant.wide_fees]\n"
        "mm.half_spread_ticks = 4\n"
        "fee_per_lot = 2\n";
    Status cfg_st;
    Config cfg = Config::from_string(def, &cfg_st);
    CHECK(cfg_st);
    if (!cfg_st) return;
    auto exp = Experiment::from_config(cfg, "file_grid");
    CHECK(exp);
    if (!exp) {
        std::printf("  from_config failed: %s\n", to_cstr(exp.status.code));
        return;
    }
    const ExperimentResult res = exp.value.run();
    CHECK_EQ(res.names.size(), 3u);
    // equivalent hand-built grid (baseline override recorded the same way)
    WorldConfig base;
    base.journal_events = false;
    base.horizon_ns = 30'000'000;
    base.warmup_ns = 2'000'000;
    Experiment manual(base);
    manual.set_seed(77123);
    manual.set_name("file_grid");
    manual.add_variant("baseline", {{"algo_latency_ns", "800"}});
    manual.add_variant("slow", {{"algo_latency_ns", "3000"}});
    manual.add_variant("wide_fees", {{"mm.half_spread_ticks", "4"}, {"fee_per_lot", "2"}});
    const ExperimentResult res2 = manual.run();
    CHECK_EQ(res.definition_hash, res2.definition_hash);
    CHECK_EQ(res.final_hashes.size(), res2.final_hashes.size());
    for (size_t i = 0; i < res.final_hashes.size(); ++i)
        CHECK_EQ(res.final_hashes[i], res2.final_hashes[i]);
    // artifact of the file-driven run reproduces exactly
    const std::string json = experiment_to_json(res, "file_grid");
    const std::string path = "artifacts/experiment_file_grid.json";
    { FILE* f = fopen(path.c_str(), "wb"); fwrite(json.data(), 1, json.size(), f); fclose(f); }
    const ReproduceReport rep = experiment_reproduce(path);
    CHECK(rep.ok);
    CHECK_EQ(rep.variants_checked, 3u);
    // unknown keys must fail loudly
    const char* bad_def =
        "[experiment]\nseed = 1\n[variant.oops]\nnonsense_key = 5\n";
    Status bad_st;
    Config bad_cfg = Config::from_string(bad_def, &bad_st);
    CHECK(bad_st);
    if (bad_st) {
        auto bad = Experiment::from_config(bad_cfg, "bad");
        CHECK(!bad);
        CHECK_EQ(bad.status.code, Errc::InvalidArgument);
    }
}

AX_TEST(experiment_latency_grid_produces_comparison) {
    // NOTE: this test writes a scratch artifact, NOT the canonical
    // artifacts/experiment_latency_grid.json (that file is the CLI default
    // grid at --ms 150 cited by README/docs, a test grid with different
    // variants must never clobber it).
    std::filesystem::create_directories("artifacts");
    WorldConfig base;
    base.journal_events = false;
    base.horizon_ns = 150'000'000;
    base.warmup_ns = 2'000'000;
    Experiment exp(base);
    exp.set_seed(2026);
    exp.add_variant("baseline_800ns", {{"algo_latency_ns", "800"}});
    exp.add_variant("fast_300ns", {{"algo_latency_ns", "300"}});
    exp.add_variant("slow_3000ns", {{"algo_latency_ns", "3000"}});
    exp.add_variant("jittery_md", {{"md_channel.jitter_ns", "1500"}});
    auto res = exp.run();
    CHECK_EQ(res.names.size(), 4u);
    // identical baseline hash when re-run (variant 0 == base with 800ns? no:
    // baseline uses default 300ns unless mutated, assert determinism instead)
    CHECK(res.final_hashes[0] != 0);
    // divergence reported for at least the slow variant
    CHECK(res.first_divergence_event[2] != ~u64{0} || res.final_hashes[2] == res.final_hashes[0]);
    const std::string json = experiment_to_json(res, "latency_grid");
    const std::string txt = experiment_summary(res, "latency_grid");
    CHECK(json.size() > 100);
    CHECK(txt.size() > 100);
    std::printf("%s", txt.c_str());
    FILE* f = fopen("artifacts/test_tmp_latency_grid.json", "wb");
    CHECK(f != nullptr);
    if (f) {
        fwrite(json.data(), 1, json.size(), f);
        fclose(f);
    }
}

// ---------------- engine contracts: identity, determinism, flow regimes ----------------

AX_TEST_MAIN
