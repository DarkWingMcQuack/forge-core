#pragma once

#include <string_view>

namespace forge::env {

auto initFileLogger(std::string_view log_preview,
                    std::string_view log_folder)
    -> void;

auto initConsoleLogger()
    -> void;

} // namespace forge::env
