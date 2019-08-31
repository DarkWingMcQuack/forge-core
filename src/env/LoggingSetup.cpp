#include <env/LoggingSetup.hpp>
#include <fmt/core.h>
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>

namespace {

struct ColorCoutSink
{
    // Linux xterm color
    // http://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal

    enum FgColor { YELLOW = 33,
                    RED = 31,
                    GREEN = 32,
                    WHITE = 97 };

    FgColor getColor(const LEVELS level) const
    {
        if(level.value == WARNING.value) {
            return YELLOW;
        }
        if(level.value == INFO.value) {
            return GREEN;
        }
        if(g3::internal::wasFatal(level)) {
            return RED;
        }

        return WHITE;
    }

    void receiveLogMessage(g3::LogMessageMover log_um_entry)
    {
        auto level = log_um_entry.get()._level;
        auto color = getColor(level);

        fmt::print("\033[{}m{}\033[m",
                   color,
                   log_um_entry.get().toString());
    }
};

} // namespace

auto forge::env::initFileLogger(std::string_view log_preview,
                                std::string_view log_folder)
    -> void
{
    static auto worker = g3::LogWorker::createLogWorker();
    worker->addDefaultLogger(log_preview.data(),
                             log_folder.data());
    // logger is initialized
    g3::initializeLogging(worker.get());
}

auto forge::env::initConsoleLogger()
    -> void
{
    static auto worker = g3::LogWorker::createLogWorker();

    auto sink_handle = worker->addSink(std::make_unique<ColorCoutSink>(),
                                      &ColorCoutSink::receiveLogMessage);
    // logger is initialized
    g3::initializeLogging(worker.get());
}
