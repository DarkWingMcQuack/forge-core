#pragma once

#include <functional>
#include <optional>
#include <type_traits>
#include <variant>
#include <vector>

namespace buddy::util {

template<class T>
class Opt;

// clang-format off
template<class T>
struct is_opt : std::false_type{};

template<class T>
struct is_opt<Opt<T>> : std::true_type{};
// clang-format on

template<class T>
class Opt
{
public:
    constexpr Opt(T value)
        : _value(std::move(value)) {}

    constexpr Opt(std::nullopt_t)
        : _value() {}

    constexpr Opt()
        : Opt(std::nullopt) {}

    using value_type = T;


    constexpr explicit operator bool() const
    {
        return static_cast<bool>(_value);
    }

    constexpr auto getValue() const& -> const T&
    {
        return _value.value();
    }

    constexpr auto getValue() & -> T&
    {
        return _value.value();
    }

    constexpr auto getValue() && -> T&&
    {
        return std::move(_value.value());
    }

    template<class U>
    constexpr auto valueOr(U&& value) & -> T
    {
        // clang-format off
        static_assert(std::is_copy_constructible<T>::value  &&
                      std::is_convertible<U&&, T>::value,
                      "T must be copy constructible and convertible from U");
        // clang-format on

        return hasValue()
            ? getValue()
            : static_cast<T>(std::forward<U>(value));
    }

    template<class U>
    constexpr auto valueOr(U&& value) const& -> T
    {
        // clang-format off
        static_assert(std::is_copy_constructible<T>::value  &&
                      std::is_convertible<U&&, T>::value,
                      "T must be copy constructible and convertible from U");
        // clang-format on

        return hasValue()
            ? getValue()
            : static_cast<T>(std::forward<U>(value));
    }

    template<class U>
    constexpr auto valueOr(U&& value) && -> T
    {
        // clang-format off
        static_assert(std::is_move_constructible<T>::value &&
                      std::is_convertible<U&&, T>::value,
                      "T must be move constructible and convertible from U");
        // clang-format on

        return hasValue()
            ? getValue()
            : static_cast<T>(std::forward<U>(value));
    }

    constexpr auto hasValue() const
        -> bool
    {
        return static_cast<bool>(*this);
    }

    template<class Func>
    constexpr auto flatMap(Func&& f) &
    {
        using Result = std::invoke_result_t<Func, T&>;
        static_assert(is_opt<Result>::value,
                      "F must return an optional");
        return hasValue()
            ? std::invoke(std::forward<Func>(f), getValue())
            : Result{std::nullopt};
    }

    template<class Func>
    constexpr auto flatMap(Func&& f) const&
    {
        using Result = std::invoke_result_t<Func, const T&>;
        static_assert(is_opt<Result>::value,
                      "F must return an optional");
        return hasValue()
            ? std::invoke(std::forward<Func>(f), getValue())
            : Result{std::nullopt};
    }

    template<class Func>
    constexpr auto flatMap(Func&& f) &&
    {
        using Result = std::invoke_result_t<Func, T&&>;
        static_assert(is_opt<Result>::value,
                      "F must return an optional");
        return hasValue()
            ? std::invoke(std::forward<Func>(f), getValue())
            : Result{std::nullopt};
    }

    template<class Func>
    constexpr auto map(Func&& f) &
    {
        using Result = std::invoke_result_t<Func, T&>;
        return hasValue()
            ? Opt<Result>{std::invoke(std::forward<Func>(f), getValue())}
            : Opt<Result>{std::nullopt};
    }

    template<class Func>
    constexpr auto map(Func&& f) const&
    {
        using Result = std::invoke_result_t<Func, const T&>;
        return hasValue()
            ? Opt<Result>{std::invoke(std::forward<Func>(f), getValue())}
            : Opt<Result>{std::nullopt};
    }

    template<class Func>
    constexpr auto map(Func&& f) &&
    {
        using Result = std::invoke_result_t<Func, T&&>;
        return hasValue()
            ? Opt<Result>{std::invoke(std::forward<Func>(f), getValue())}
            : Opt<Result>{std::nullopt};
    }

    constexpr auto flatten() &&
    {
        if constexpr(is_opt<T>::value) {
            return getValue();
        } else {
            return *this;
        }
    }

    constexpr auto flatten() &
    {
        if constexpr(is_opt<T>::value) {
            return getValue();
        } else {
            return *this;
        }
    }

    constexpr auto flatten() const&
    {
        if constexpr(is_opt<T>::value) {
            return getValue();
        } else {
            return *this;
        }
    }

private:
    std::optional<T> _value;
};

template<class T>
constexpr auto collect(std::vector<Opt<T>>&& vec)
    -> Opt<std::vector<T>>
{
    std::vector<T> ret_vec;
    for(auto&& elem : vec) {
        if(!elem) {
            return std::nullopt;
        }

        ret_vec.emplace_back(std::move(elem.getValue()));
    }

    return ret_vec;
}

template<class T, class F>
constexpr auto traverse(std::vector<T>&& vec, F&& f)
    -> Opt<std::vector<typename std::invoke_result_t<F, T&&>::value_type>>
{
    using invoke_res = typename std::invoke_result_t<F, T&&>;

    static_assert(is_opt<invoke_res>::value,
                  "function musst return an opt to be useable with traverse");

    std::vector<typename invoke_res::value_type> ret_vec;
    for(auto&& elem : vec) {
        auto result = std::invoke(std::forward<F>(f),
                                  std::move(elem));

        if(!result) {
            return std::nullopt;
        }

        ret_vec.emplace_back(std::move(result.getValue()));
    }

    return ret_vec;
}

template<class T>
constexpr auto flatten(Opt<Opt<T>>&& opt)
    -> Opt<T>
{
    if(opt) {
        return opt.getValue();
    }

    return std::nullopt;
}

template<class T, class U>
constexpr auto combine(Opt<T>&& first, Opt<U>&& second)
    -> Opt<std::pair<T, U>>
{
    if(first) {
        if(second) {
            return std::pair{std::move(first.getValue()),
                             std::move(second.getValue())};
        }
    }

    return std::nullopt;
}

} // namespace buddy::util
