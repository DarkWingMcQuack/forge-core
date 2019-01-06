#pragma once
#include <exception>

namespace buddy::wallet {

class WalletError final : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

} // namespace buddy::wallet
