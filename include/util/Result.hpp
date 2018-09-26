#pragma once

#include <optional>
#include <variant>
#include <vector>
#include <type_traits>

namespace buddy::util {

template<class T, class Err>
class Result;


// clang-format off
template<class T>
struct is_result : std::false_type{};

template<class T, class Err>
struct is_result<Result<T, Err>> : std::true_type{};

// clang-format on

template<class T, class Err>
class Result
{
public:
    constexpr Result(T value)
        : _var(std::move(value)) {}
    constexpr Result(Err error)
        : _var(std::move(error)) {}

    using result_type = T;
    using error_type = Err;

    constexpr explicit operator bool() const
    {
        return _var.type() == typeid(T);
    }

    constexpr auto get_value() const
        -> const T&
    {
        return std::get<T>(_var);
    }

    constexpr auto get_value()
        -> T&
    {
        return std::get<T>(_var);
    }

    constexpr auto value_or(T value) const
        -> T
    {
        return static_cast<bool>(*this) ? get_value() : value;
    }

    constexpr auto has_value() const
        -> bool
    {
        return static_cast<bool>(*this);
    }


    constexpr auto has_error() const
        -> bool
    {
        return !static_cast<bool>(*this);
    }

    constexpr auto get_error() const
        -> const Err&
    {
        return std::get<Err>(_var);
    }

    constexpr auto get_error()
        -> Err&
    {
        return std::get<Err>(_var);
    }

    template<class Func>
    constexpr auto map(Func&& func) const
    {
        using FuncRet = std::result_of_t<Func(T)>;
        using ReturnType = Result<FuncRet, Err>;

        return static_cast<bool>(*this)
            ? ReturnType{func(get_value())}
            : ReturnType{get_error()};
    }

    template<class Func>
    constexpr auto flat_map(Func&& func) const
    {
        using FuncRet = std::result_of_t<Func(T)>;

        static_assert(is_result<FuncRet>::value,
                      "return of flatmap function must be a result");

        static_assert(std::is_same_v<typename FuncRet::error_type, Err>,
                      "error type of flatmat return musst be same as the original error type");

        return static_cast<bool>(*this)
            ? func(get_value())
            : FuncRet{get_error()};
    }

    template<class Func>
    constexpr auto map_error(Func&& func) const
    {
        using FuncRet = std::result_of_t<Func(Err)>;
        using ReturnType = Result<T, FuncRet>;

        return static_cast<bool>(*this)
            ? ReturnType{get_value()}
            : ReturnType{func(get_error())};
    }

    template<class Func>
    constexpr auto flat_map_error(Func&& func) const
    {
        using FuncRet = std::result_of_t<Func(Err)>;

        static_assert(is_result<FuncRet>::value,
                      "return of flatmap function must be a result");

        static_assert(std::is_same_v<typename FuncRet::value_type, T>,
                      "error type of flatmat return musst be same as the original error type");

        return static_cast<bool>(*this)
            ? FuncRet{get_value()}
            : func(get_error());
    }

    template<class Func>
    constexpr auto change_value(Func&& func)
        -> Result<T, Err>&
    {
        using FuncRet = std::result_of_t<Func(T)>;

        static_assert(std::is_void_v<FuncRet>,
                      "return of change_value function must be void");

        if(*this) {
            func(get_value());
        }

        return *this;
    }

    template<class Func>
    constexpr auto change_error(Func&& func)
        -> Result<T, Err>&
    {
        using FuncRet = std::result_of_t<Func(Err)>;

        static_assert(std::is_void_v<FuncRet>,
                      "return of change_value function must be void");

        if(*this) {
            func(get_error());
        }

        return *this;
    }

    //this serves as finally block
    //to execute functions which in a maner of
    //java finally blocks
    template<class Func>
    constexpr auto finally(Func&& func) const
        -> const Result<T, Err>&
    {

        using FuncRet = std::result_of_t<Func()>;
        static_assert(std::is_void_v<FuncRet>,
                      "return of change_value function must be void");
        func();
        return *this;
    }

private:
    std::variant<T, Err> _var;
};



template<class Err>
class Result<void, Err>
{
public:
    constexpr Result()
        : _var() {}
    constexpr Result(Err error)
        : _var(std::move(error)) {}

    using result_type = void;
    using error_type = Err;

    constexpr explicit operator bool() const
    {
        return !static_cast<bool>(_var);
    }

    constexpr auto get_error() const
        -> const Err&
    {
        return std::get<Err>(_var);
    }

    constexpr auto get_error()
        -> Err&
    {
        return std::get<Err>(_var);
    }

    constexpr auto has_value() const
        -> bool
    {
        return static_cast<bool>(*this);
    }

    constexpr auto has_error() const
        -> bool
    {
        return !static_cast<bool>(*this);
    }

    template<class Func>
    constexpr auto map(Func&& func) const
    {

        using FuncRet = std::result_of_t<Func(void)>;
        using ReturnType = Result<FuncRet, Err>;

        if constexpr(std::is_void_v<FuncRet>) {

            return static_cast<bool>(*this)
                   ? func(),
                   ReturnType{}
                   : ReturnType{get_error()};

        } else {
            return static_cast<bool>(*this)
                ? ReturnType{func()}
                : ReturnType{get_error()};
        }
    }

    template<class Func>
    constexpr auto flat_map(Func&& func) const
    {
        using FuncRet = std::result_of_t<Func(void)>;

        static_assert(is_result<FuncRet>::value,
                      "return of flatmap function must be a result");

        static_assert(std::is_same_v<typename FuncRet::error_type, Err>,
                      "error type of flatmat return musst be same as the original error type");

        return static_cast<bool>(*this)
            ? func()
            : FuncRet{get_error()};
    }

    template<class Func>
    constexpr auto map_error(Func&& func) const
    {
        using FuncRet = std::result_of_t<Func(Err)>;
        using ReturnType = Result<void, FuncRet>;

        return static_cast<bool>(*this)
            ? ReturnType{}
            : ReturnType{func(get_error())};
    }

    template<class Func>
    constexpr auto flat_map_error(Func&& func) const
    {
        using FuncRet = std::result_of_t<Func(Err)>;

        static_assert(is_result<FuncRet>::value,
                      "return of flatmap function must be a result");

        static_assert(std::is_void_v<typename FuncRet::result_type>,
                      "value type of flatmap return musst be same as the original value type");

        return static_cast<bool>(*this)
            ? FuncRet{}
            : func(get_error());
    }


    //this serves as finally block
    //to execute functions which in a maner of
    //java finally blocks
    template<class Func>
    constexpr auto finally(Func&& func) const
        -> const Result<void, Err>&
    {

        using FuncRet = std::result_of_t<Func()>;
        static_assert(std::is_void_v<FuncRet>,
                      "return of change_value function must be void");
        func();
        return *this;
    }

private:
    std::optional<Err> _var;
};

template<class E, class Func, class... Params>
auto Try(Func&& f, Params&&... x)
    -> Result<decltype(f(std::forward<Params>(x)...)), E>
{
    try {
        return f(std::forward<Params>(x)...);
    } catch(E const& e) {
        return e;
    }
}

template<class E, class T>
auto flatten(Result<Result<T, E>, E>&& result)
    -> Result<T, E>
{
    if(result) {
        return result.get_value();
    }

    return result.get_error();
}

template<class E>
auto flatten(Result<Result<void, E>, E>&& result)
    -> Result<void, E>
{
    if(result) {
        return {};
    }

    return result.get_error();
}

template<class E, class T>
auto collect(std::vector<Result<T, E>>&& vec)
    -> Result<std::vector<T>, E>
{
    std::vector<T> ret_vec;
    ret_vec.resize(vec.size());

    for(auto&& res : vec) {
        if(!res) {
            return res.get_error();
        }

        auto value = std::move(res.get_value());
        ret_vec.push_back(std::move(value));
    }

    return ret_vec;
}

template<class E>
auto collect(std::vector<Result<void, E>>&& vec)
    -> Result<void, E>
{
    for(auto&& res : vec) {
        if(!res) {
            return res.get_error();
        }
    }

    return {};
}


template<class T, class F>
auto traverse(const std::vector<T>& vec, F&& func)
    //enable if the result of F(T) is not a Result<void,...>
    -> std::enable_if_t<!std::is_void<typename std::result_of_t<F(T)>::result_type>::value,
                        Result<std::vector<typename std::result_of_t<F(T)>::result_type>,
                               typename std::result_of_t<F(T)>::error_type>>
{
    using FuncRet = std::result_of_t<F(T)>;

    static_assert(is_result<FuncRet>::value,
                  "return of the traversing function must be a result");


    std::vector<typename FuncRet::result_type> ret_vec;

    for(const auto& elem : vec) {
        auto res = func(elem);

        if(!res) {
            return res.get_error();
        }

        ret_vec.push_back(std::move(res.get_value()));
    }

    return ret_vec;
}


//overload for functions returning Result<void,...>
//this function then returns Result<void, ...> also
template<class T, class F>
auto traverse(const std::vector<T>& vec, F&& func)
    //enable if the result of F(T) is a Result<void,...>
    -> std::enable_if_t<std::is_void<typename std::result_of_t<F(T)>::result_type>::value,
                        Result<void,
                               typename std::result_of_t<F(T)>::error_type>>
{
    using FuncRet = std::result_of_t<F(T)>;

    static_assert(is_result<FuncRet>::value,
                  "return of the traversing function must be a result");

    for(auto&& elem : vec) {
        auto res = func(elem);

        if(!res) {
            return res.get_error();
        }
    }

    return {};
}


template<class T, class V, class E>
auto combine(Result<T, E>&& first, Result<V, E>&& second)
    -> Result<std::pair<T, V>, E>
{
    if(first.has_value()) {
        if(second.has_value()) {
            return std::pair{std::move(first.get_value()),
                             std::move(second.get_value())};
        }

        return second.get_error();
    }

    return first.get_error();
}

} // namespace kyle::util
