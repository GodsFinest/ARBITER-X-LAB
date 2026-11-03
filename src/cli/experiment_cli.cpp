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
bool write_file(const std::string& path, const std::string& content) {
    std::FILE* file = std::fopen(path.c_str(), "wb");
    if (!file) return false;
    const bool complete = std::fwrite(content.data(), 1, content.size(), file) == content.size();
    return std::fclose(file) == 0 && complete;
}

bool parse_u64_arg(const char* text, u64& value) {
    if (!text || !*text || *text == '-') return false;
    errno = 0;
    char* end = nullptr;
    const unsigned long long parsed = std::strtoull(text, &end, 10);
    if (errno == ERANGE || end == text || *end != '\0') return false;
    value = static_cast<u64>(parsed);
    return true;
}

bool parse_i64_arg(const char* text, i64& value) {
    if (!text || !*text) return false;
    errno = 0;
    char* end = nullptr;
    const long long parsed = std::strtoll(text, &end, 10);
    if (errno == ERANGE || end == text || *end != '\0') return false;
    value = static_cast<i64>(parsed);
    return true;
}

bool parse_double_arg(const char* text, double& value) {
    if (!text || !*text) return false;
    errno = 0;
    char* end = nullptr;
    const double parsed = std::strtod(text, &end);
    if (errno == ERANGE || end == text || *end != '\0' || !std::isfinite(parsed)) return false;
    value = parsed;
    return true;
}

bool parse_u64_option(int argc, char** argv, int& index, u64& value) {
    if (index + 1 >= argc || !parse_u64_arg(argv[index + 1], value)) {
        std::fprintf(stderr, "invalid numeric value for %s\n", argv[index]);
        return false;
    }
    ++index;
    return true;
}

bool parse_i64_option(int argc, char** argv, int& index, i64& value) {
    if (index + 1 >= argc || !parse_i64_arg(argv[index + 1], value)) {
        std::fprintf(stderr, "invalid numeric value for %s\n", argv[index]);
        return false;
    }
    ++index;
    return true;
}

bool millis_to_ns(u64 milliseconds, LogicalNs& nanoseconds) {
    constexpr u64 kNsPerMs = 1'000'000;
    if (milliseconds > static_cast<u64>(std::numeric_limits<LogicalNs>::max()) / kNsPerMs)
        return false;
    nanoseconds = static_cast<LogicalNs>(milliseconds * kNsPerMs);
    return true;
}

int cmd_experiment(int argc, char** argv) {
    u64 seed = 2026;
    u64 horizon_ms = 150;
    const char* name = "latency_grid";
    const char* reproduce = nullptr;
    const char* config_file = nullptr;
    for (int i = 1; i < argc; ++i) {
        if (!std::strcmp(argv[i], "--seed")) {
            if (!parse_u64_option(argc, argv, i, seed)) return 2;
        } else if (!std::strcmp(argv[i], "--ms")) {
            if (!parse_u64_option(argc, argv, i, horizon_ms)) return 2;
        }
        else if (!std::strcmp(argv[i], "--name") && i + 1 < argc) name = argv[++i];
        else if (!std::strcmp(argv[i], "--reproduce") && i + 1 < argc) reproduce = argv[++i];
        else if (!std::strcmp(argv[i], "--config") && i + 1 < argc) config_file = argv[++i];
    }
    LogicalNs horizon_ns = 0;
    if (!millis_to_ns(horizon_ms, horizon_ns)) {
        std::fprintf(stderr, "--ms is too large\n");
        return 2;
    }
    if (reproduce) {
        const ReproduceReport rep = experiment_reproduce(reproduce);
        std::printf("reproduce %s\n  variants checked : %llu\n  hash mismatches : %llu\n",
                    reproduce, (unsigned long long)rep.variants_checked,
                    (unsigned long long)rep.hash_mismatches);
        std::printf("RESULT: %s (%s)\n", rep.ok ? "EXACTLY REPRODUCED" : "REPRODUCTION FAILED",
                    rep.detail.c_str());
        return rep.ok ? 0 : 1;
    }
    std::filesystem::create_directories("artifacts");
    const std::string family = name;
    const auto run_and_write = [&](const std::string& grid_name, Experiment& exp) {
        const ExperimentResult res = exp.run();
        const std::string txt = experiment_summary(res, grid_name);
        const std::string json = experiment_to_json(res, grid_name);
        std::printf("%s", txt.c_str());
        const std::string artifact = "artifacts/experiment_" + grid_name + ".json";
        if (!write_file(artifact, json)) {
            std::fprintf(stderr, "cannot write %s\n", artifact.c_str());
            return false;
        }
        std::printf("  artifact: %s\n", artifact.c_str());
        return true;
    };
    if (config_file) {
        // file-driven experiment definition (docs/COUNTERFACTUALS.md grammar)
        auto cfg = Config::from_file(config_file);
        if (!cfg) {
            std::printf("config read failed: %s\n", to_cstr(cfg.status.code));
            return 1;
        }
        // the file's [experiment] section owns seed/horizon; explicit CLI
        // flags are ignored in this mode, say so instead of silently
        // surprising the user
        if (seed != 2026 || horizon_ms != 150)
            std::printf("  note: --seed/--ms are ignored with --config (the file's [experiment] section wins)\n");
        const std::string stem = std::filesystem::path(config_file).stem().string();
        auto exp = Experiment::from_config(cfg.value, stem);
        if (exp) {
            // validate every variant up front: a config error must be a
            // reported CLI error, never a mid-grid construction failure
            for (const auto& v : exp.value.variants())
                if (const std::string invalid = v.cfg.validate(); !invalid.empty()) {
                    std::printf("invalid variant '%s': %s\n", v.name.c_str(), invalid.c_str());
                    return 2;
                }
        }
        if (!exp) {
            std::printf("experiment definition invalid: %s (ctx %llu)\n",
                        to_cstr(exp.status.code), (unsigned long long)exp.status.context);
            return 1;
        }
        if (!run_and_write(stem, exp.value)) return 1;
        std::printf("  reproduce: arbiter-lab experiment --reproduce artifacts/experiment_%s.json\n", stem.c_str());
        return 0;
    }
    if (family == "latency_sweep") {
        // Eight-point sweep: where does decision latency start to cost fills?
        for (u64 i = 0; i < 8; ++i) {
            const i64 lat = 100 * static_cast<i64>(1 << i);  // 100ns..12.8us
            WorldConfig base;
            base.journal_events = false;
            base.horizon_ns = horizon_ns;
            base.warmup_ns = 2'000'000;
            Experiment exp(base);
            exp.set_seed(seed);
            exp.set_name("latency_sweep_" + std::to_string(lat));
            exp.add_variant("baseline_800ns", {{"algo_latency_ns", "800"}});
            exp.add_variant("sweep", {{"algo_latency_ns", std::to_string(lat)}});
            if (!run_and_write("latency_sweep_" + std::to_string(lat), exp)) return 1;
        }
        std::printf("  reproduce: arbiter-lab experiment --reproduce artifacts/experiment_latency_sweep_3200.json\n");
        return 0;
    }
    if (family == "network_grid") {
        // jitter/loss cross-grid on the MD channel (the feed-handler path)
        const std::pair<const char*, std::vector<std::pair<std::string, std::string>>> mutators[] = {
            {"baseline", {}},
            {"jitter_500", {{"md_channel.jitter_ns", "500"}}},
            {"jitter_2000", {{"md_channel.jitter_ns", "2000"}}},
            {"loss_1pct", {{"md_channel.loss_rate", "0.01"}}},
            {"loss_2pct", {{"md_channel.loss_rate", "0.02"}}},
            {"jitter_2000_loss_1pct", {{"md_channel.jitter_ns", "2000"}, {"md_channel.loss_rate", "0.01"}}},
        };
        WorldConfig base;
        base.journal_events = false;
        base.horizon_ns = horizon_ns;
        base.warmup_ns = 2'000'000;
        Experiment exp(base);
        exp.set_seed(seed);
        exp.set_name("network_grid");
        for (const auto& [nm, ovr] : mutators) exp.add_variant(nm, ovr);
        if (!run_and_write("network_grid", exp)) return 1;
        std::printf("  reproduce: arbiter-lab experiment --reproduce artifacts/experiment_network_grid.json\n");
        return 0;
    }
    // default: the classic latency grid
    WorldConfig base;
    base.journal_events = false;
    base.horizon_ns = horizon_ns;
    base.warmup_ns = 2'000'000;
    Experiment exp(base);
    exp.set_seed(seed);
    exp.set_name(name);
    exp.add_variant("baseline_800ns", {{"algo_latency_ns", "800"}});
    exp.add_variant("fast_300ns", {{"algo_latency_ns", "300"}});
    exp.add_variant("slow_3000ns", {{"algo_latency_ns", "3000"}});
    exp.add_variant("jittery_md", {{"md_channel.jitter_ns", "1500"}});
    exp.add_variant("lossy_md_1pct", {{"md_channel.loss_rate", "0.01"}});
    if (!run_and_write(name, exp)) return 1;
    std::printf("  reproduce: arbiter-lab experiment --reproduce artifacts/experiment_%s.json\n", name);
    return 0;
}

