#pragma once
#include <exception>

namespace buddy::daemon {

class DaemonError final : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

} // namespace buddy::daemon
