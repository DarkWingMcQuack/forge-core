#pragma once

#include <stdexcept>

namespace forge::client {

class ClientError final : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

} // namespace forge::client
