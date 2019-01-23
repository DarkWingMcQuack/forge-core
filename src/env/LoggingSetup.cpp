#include <env/LoggingSetup.hpp>
#include <fmt/core.h>
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>

namespace {

struct ColorCoutSink
{
    // Linux xterm color
    // http://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal

    enum FG_Color { YELLOW = 33,
                    RED = 31,
                    GREEN = 32,
                    WHITE = 97 };

    FG_Color GetColor(const LEVELS level) const
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

    void ReceiveLogMessage(g3::LogMessageMover logEntry)
    {
        auto level = logEntry.get()._level;
        auto color = GetColor(level);

        fmt::print("\033[{}m{}\033[m",
                   color,
                   logEntry.get().toString());
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

    auto sinkHandle = worker->addSink(std::make_unique<ColorCoutSink>(),
                                      &ColorCoutSink::ReceiveLogMessage);
    // logger is initialized
    g3::initializeLogging(worker.get());
}
