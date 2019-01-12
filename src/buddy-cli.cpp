#include <CLI/CLI.hpp>
#include <chrono>
#include <core/Operation.hpp>
#include <cxxopts.hpp>
#include <env/LoggingSetup.hpp>
#include <env/ProgramOptions.hpp>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <g3log/g3log.hpp>
#include <thread>
#include <utilxx/Opt.hpp>
#include <utilxx/Result.hpp>
#include <wallet/ReadWriteWallet.hpp>

using buddy::core::stringToByteVec;
using buddy::core::toHexString;
using buddy::core::Coin;
using buddy::env::initConsoleLogger;
using buddy::env::initFileLogger;
using buddy::env::parseOptions;
using buddy::env::ProgramOptions;

auto main(int argc, char* argv[]) -> int
{
    auto params = parseOptions(argc, argv);
}
