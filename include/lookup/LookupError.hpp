#pragma once

#include <stdexcept>

namespace forge::lookup {

class LookupError final : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

} // namespace forge::lookup
