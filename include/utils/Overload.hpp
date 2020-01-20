#pragma once

namespace forge::utils {

template<class... Ts>
struct overload : Ts...
{
    using Ts::operator()...;
};

template<class... Ts>
overload(Ts...)->overload<Ts...>;

} // namespace forge::utils
