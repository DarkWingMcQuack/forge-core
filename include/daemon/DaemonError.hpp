#pragma once

#include <stdexcept>

namespace forge::daemon {

class DaemonError final : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

} // namespace forge::daemon
