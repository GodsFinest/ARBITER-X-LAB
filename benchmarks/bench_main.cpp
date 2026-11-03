#include "arbiter_lab/pipeline.hpp"
#include "arbiter_lab/features.hpp"
#include "arbiter/histogram.hpp"
#include "arbiter/build_info.hpp"
#include <chrono>
#include <cstdio>
#include <cstring>
using namespace arbiter;
using namespace arbiter::lab;
bool g_quick=false;
u64 now_ns(){return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();}
void bench_research() {
    // Research-layer throughput (deterministic synthetic input; wall-clock).
    // Never confuse these with trading performance, they measure analysis
    // cost per event, the latency-budget input for §70 (decay vs compute).
    std::printf("=== research layer (synthetic L2, requested events=%zu) ===\n",
                g_quick ? size_t{500} : size_t{20'000});
    pipeline::SyntheticConfig sc;
    sc.seed = 2026;
    sc.n_events = g_quick ? 500 : 20000;
    auto evs = pipeline::synthesize(sc);
    u64 t0 = now_ns();
    auto series = pipeline::build_series(evs);
    u64 t1 = now_ns();
    // feature pass (as-of, feat_v1)
    features::OfiAccumulator ofi;
    features::FlowWindows flow;
    features::MidSeries mids;
    Histogram fh(1u << 21);
    for (const auto& s : series) {
        features::BookDelta d{s.ladder.bids.empty() ? 0 : s.ladder.bids.front().price,
                              s.ladder.asks.empty() ? 0 : s.ladder.asks.front().price,
                              s.ladder.bids.empty() ? 0 : s.ladder.bids.front().total_qty,
                              s.ladder.asks.empty() ? 0 : s.ladder.asks.front().total_qty};
        ofi.observe(s.ts_ns, d);
        if (s.mid > 0) mids.observe(s.ts_ns, s.mid);
        if (s.mid <= 0 || s.spread_rel < 0) continue;
        const u64 a = now_ns();
        volatile double sink =
            features::compute_v1(s.ladder, ofi, flow, mids, s.ts_ns).get("imb_1");
        (void)sink;
        fh.record(static_cast<double>(now_ns() - a));
    }
    pipeline::StudyConfig cfg;
    cfg.experiment_id = "BENCH";
    cfg.seed = 2026;
    const u64 t3 = now_ns();
    pipeline::StudyResult st = pipeline::run_study(series, cfg);
    const u64 t4 = now_ns();
    (void)st;
    const auto fs = fh.summarize();
    std::printf("    series build : %llu events -> %llu samples in %.1fms (%.0f events/s)\n",
                (unsigned long long)evs.size(), (unsigned long long)series.size(),
                (t1 - t0) / 1e6, evs.size() / ((t1 - t0) * 1e-9));
    std::printf("    feature v1   : p50=%.0fns p99=%.0fns (%.0f features/s)\n", fs.p50, fs.p99,
                1e9 / (fs.mean > 0 ? fs.mean : 1));
    std::printf("    full study   : %.1fms end-to-end (features+labels+models+execution)\n",
                (t4 - t3) / 1e6);
}

int main(int argc,char**argv){
 for(int i=1;i<argc;++i) {
  if(!std::strcmp(argv[i],"--quick")) g_quick=true;
  else if(!std::strcmp(argv[i],"--help")) { std::puts("arbiter_lab_bench [--quick]"); return 0; }
  else return 2;
 }
 std::printf("compiler=%s platform=%s\n",build::compiler_id().c_str(),build::platform_id().c_str());
 bench_research(); return 0;
}
