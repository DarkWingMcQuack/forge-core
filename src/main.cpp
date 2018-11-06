#include <chrono>
#include <core/Operation.hpp>
#include <cxxopts.hpp>
#include <daemon/OdinDaemon.hpp>
#include <env/LoggingSetup.hpp>
#include <env/ProgramOptions.hpp>
#include <fmt/core.h>
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <lookup/LookupManager.hpp>
#include <thread>
#include <utilxx/Opt.hpp>
#include <utilxx/Result.hpp>

using buddy::daemon::OdinDaemon;
using buddy::lookup::LookupManager;
using buddy::daemon::ReadOnlyDaemonBase;
using buddy::daemon::make_daemon;
using buddy::daemon::Coin;
using buddy::env::initConsoleLogger;
using buddy::env::initFileLogger;
using buddy::env::parseOptions;

auto main(int argc, char* argv[]) -> int
{
    using namespace std::chrono_literals;

    auto params = parseOptions(argc, argv);

    if(params.getLogFolder()) {
        initFileLogger(argv[0],
                       params.getLogFolder().getValue());
    } else {
        initConsoleLogger();
    }


    //get a daemon
    auto daemon = make_daemon(params.getCoinHost(),
                              params.getCoinUser(),
                              params.getCoinPassword(),
                              params.getCoinPort(),
                              Coin::Odin);

    LookupManager manager{std::move(daemon)};

    while(true) {

        //update to newest block
        auto res = manager.updateLookup();
        //report errors

        res.onError([](auto&& error) {
               auto error_msg = std::visit(
                   [](auto&& er) {
                       return er.what();
                   },
                   error);
               LOG(WARNING) << error_msg;
           })
            .onValue([&](auto&& added) {
                if(!added) {
                    return;
                }

                auto valid_res = manager.lookupIsValid();

                valid_res.onError([](auto&& error) {
                             LOG(WARNING) << error.what();
                         })
                    .onValue([](auto&& value) {
                        LOG_IF(DEBUG, value) << "lookup is valid";
                        LOG_IF(WARNING, !value) << "lookup is invalid";
                    });
            });

        //do this every 30 seconds
        std::this_thread::sleep_for(30s);
    }
}
