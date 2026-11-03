#include "arbiter/build_info.hpp"
#include "arbiter_build_info_config.hpp"
#include <cstdio>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <map>
#include <set>
#include <exception>
#include <stdexcept>
#include <string>
int cmd_research(int, char**);
int cmd_experiment(int, char**);
namespace {
bool validate_options(int argc,char**argv,const std::string& command) {
    std::set<std::string> values, flags;
    int start=2;
    if(command=="experiment") values={"--seed","--ms","--name","--config","--reproduce"};
    else {
        if(argc<3) return true;
        const std::string sub=argv[2]; start=3;
        if(sub=="validate") values={"--csv"};
        else if(sub=="features" || sub=="evaluate" || sub=="backtest") {
            values={"--csv","--seed","--events"}; flags={"--synthetic","--biased"};
            if(sub=="features") values.insert("--out");
            else { values.insert("--latency-ns"); values.insert("--fee"); }
            if(sub=="backtest") { values.insert("--threshold"); values.insert("--queue"); }
        } else if(sub=="run") values={"--exp","--csv","--seed","--latency-ns","--fee","--threshold","--out"};
        else if(sub=="reproduce") values={"--artifact-dir"};
        else return true;
        if((sub=="run" || sub=="reproduce") && start<argc && argv[start][0]!='-') ++start;
    }
    for(int i=start;i<argc;++i) {
        const std::string key=argv[i];
        if(flags.contains(key)) continue;
        if(!values.contains(key) || i+1>=argc) { std::fprintf(stderr,"Unknown or incomplete option: %s\n",key.c_str()); return false; }
        const char* raw=argv[++i];
        if(key=="--queue" && std::strcmp(raw,"all")) { std::fputs("--queue supports all (the three-model comparison)\n",stderr); return false; }
        if(key=="--seed" || key=="--events" || key=="--ms" || key=="--latency-ns" || key=="--fee") {
            errno=0; char* end=nullptr;
            const auto n=std::strtoull(raw,&end,10);
            if(!*raw || *raw=='-' || errno==ERANGE || end==raw || *end ||
               (key=="--events" && n==0) ||
               ((key=="--ms" || key=="--latency-ns" || key=="--fee") && n>0x7fffffffffffffffull)) {
                std::fprintf(stderr,"Invalid numeric value for %s\n",key.c_str()); return false;
            }
        }
        if(key=="--threshold") { errno=0; char* end=nullptr; const double n=std::strtod(raw,&end);
            if(errno==ERANGE || end==raw || *end || !std::isfinite(n) || n<0) return false;
        }
    }
    return true;
}
}
int main(int argc, char** argv) {
    const std::string command = argc > 1 ? argv[1] : "--help";
    if(command=="--version") {
        std::printf("ARBITER-X-LAB %s %s (%s)\nARBITER-X %s %s (%s)\n",
            AX_BUILD_VERSION, AX_BUILD_REVISION, AX_BUILD_SOURCE_STATE,
            arbiter::build::version(), arbiter::build::revision(), arbiter::build::source_state());
        std::printf("Tested core: %s\n", AX_TESTED_CORE_REVISION);
        if(std::string(arbiter::build::revision())!=AX_TESTED_CORE_REVISION)
            std::puts("Core revision differs from tested pin; verify numerical reproduction for this build.");
        return 0;
    }
    if(command=="--help" || command=="help" || command=="-h") {
        std::puts("arbiter-lab research <validate|features|evaluate|backtest|run|reproduce> [options]\narbiter-lab experiment [--config F|--reproduce F|--seed S --ms MS]\narbiter-lab --version");
        return 0;
    }
    if(!validate_options(argc,argv,command)) return 2;
    try {
        if(command=="research") return cmd_research(argc-1, argv+1);
        if(command=="experiment") return cmd_experiment(argc-1, argv+1);
    } catch(const std::invalid_argument& e) { std::fprintf(stderr,"error: %s\n",e.what()); return 2; }
    catch(const std::exception& e) { std::fprintf(stderr,"error: %s\n",e.what()); return 1; }
    std::fprintf(stderr,"unknown command: %s\n",command.c_str());
    return 2;
}
