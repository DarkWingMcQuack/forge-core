#pragma once
#include <exception>

namespace forge::wallet {

class WalletError final : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

} // namespace forge::wallet
