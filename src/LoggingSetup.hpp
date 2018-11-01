#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <iostream>

namespace buddy::env {

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
        if(level.value == DEBUG.value) {
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

        std::cout << "\033[" << color << "m" << logEntry.get().toString() << "\033[m";
    }
};

auto setupFileLogger(std::string_view log_preview,
                     std::string_view log_folder)
{
    auto worker = g3::LogWorker::createLogWorker();
    worker->addDefaultLogger(log_preview.data(),
                             log_folder.data());
    // logger is initialized
    g3::initializeLogging(worker.get());

    return worker;
}

auto setupConsoleLogger()
{
    auto worker = g3::LogWorker::createLogWorker();

    auto sinkHandle = worker->addSink(std::make_unique<ColorCoutSink>(),
                                      &ColorCoutSink::ReceiveLogMessage);
    // logger is initialized
    g3::initializeLogging(worker.get());
    return worker;
}

} // namespace buddy::env
