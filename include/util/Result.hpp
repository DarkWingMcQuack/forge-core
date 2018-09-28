#pragma once

#include <functional>
#include <optional>
#include <type_traits>
#include <variant>
#include <vector>

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

    constexpr auto getValue() const& -> const T&
    {
        return std::get<T>(_var);
    }

    constexpr auto getValue() & -> T&
    {
        return std::get<T>(_var);
    }

    constexpr auto getValue() && -> T&&
    {
        return std::get<T>(std::move(_var));
    }

    constexpr auto valueOr(T value) const
        -> T
    {
        return static_cast<bool>(*this) ? getValue() : value;
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

    constexpr auto hasError() const
        -> bool
    {
        return !static_cast<bool>(*this);
    }

    constexpr auto getError() const& -> const Err&
    {
        return std::get<Err>(_var);
    }

    constexpr auto getError() & -> Err&
    {
        return std::get<Err>(_var);
    }

    constexpr auto getError() && -> Err&&
    {
        return std::get<Err>(std::move(_var));
    }

    template<class Func>
    constexpr auto map(Func&& f) &
    {
        using FuncRet = std::invoke_result_t<Func, T>;
        using ReturnType = Result<FuncRet, Err>;

        return hasValue()
            ? ReturnType{std::invoke(std::forward<Func>(f), getValue())}
            : ReturnType{getError()};
    }

    template<class Func>
    constexpr auto map(Func&& f) const&
    {
        using FuncRet = std::invoke_result_t<Func, T>;
        using ReturnType = Result<FuncRet, Err>;

        return hasValue()
            ? ReturnType{std::invoke(std::forward<Func>(f), getValue())}
            : ReturnType{getError()};
    }

    template<class Func>
    constexpr auto map(Func&& f) &&
    {
        using FuncRet = std::invoke_result_t<Func, T>;
        using ReturnType = Result<FuncRet, Err>;

        return hasValue()
            ? ReturnType{std::invoke(std::forward<Func>(f), getValue())}
            : ReturnType{getError()};
    }

    template<class Func>
    constexpr auto flatMap(Func&& f) &
    {
        using result = std::invoke_result_t<Func, T&>;
        static_assert(is_result<result>::value,
                      "F must return a result");

        static_assert(std::is_same_v<typename result::error_type, Err>,
                      "error type of flatmat return musst be same as the original error type");

        return hasValue()
            ? std::invoke(std::forward<Func>(f), getValue())
            : Result{getError()};
    }

    template<class Func>
    constexpr auto flatMap(Func&& f) const&
    {
        using result = std::invoke_result_t<Func, const T&>;
        static_assert(is_result<result>::value,
                      "F must return a result");

        static_assert(std::is_same_v<typename result::error_type, Err>,
                      "error type of flatmat return musst be same as the original error type");

        return hasValue()
            ? std::invoke(std::forward<Func>(f), getValue())
            : Result{getError()};
    }

    template<class Func>
    constexpr auto flatMap(Func&& f) &&
    {
        using result = std::invoke_result_t<Func, T&&>;
        static_assert(is_result<result>::value,
                      "F must return a result");

        static_assert(std::is_same_v<typename result::error_type, Err>,
                      "error type of flatmat return musst be same as the original error type");

        return hasValue()
            ? std::invoke(std::forward<Func>(f), getValue())
            : Result{getError()};
    }

    template<class Func>
    constexpr auto mapError(Func&& f) const&
    {
        using result = std::invoke_result_t<Func, const Err&>;
        static_assert(is_result<result>::value,
                      "F must return a result");

        static_assert(std::is_same_v<typename result::value_type, Err>,
                      "value type of flatmat return musst be same as the original value type");

        return hasValue()
            ? std::invoke(std::forward<Func>(f), getError())
            : Result{getValue()};
    }

    template<class Func>
    constexpr auto mapError(Func&& f) &
    {
        using result = std::invoke_result_t<Func, Err&>;
        static_assert(is_result<result>::value,
                      "F must return a result");

        static_assert(std::is_same_v<typename result::value_type, Err>,
                      "value type of flatmat return musst be same as the original value type");

        return hasValue()
            ? std::invoke(std::forward<Func>(f), getError())
            : Result{getValue()};
    }

    template<class Func>
    constexpr auto mapError(Func&& f) &&
    {
        using result = std::invoke_result_t<Func, Err&&>;
        static_assert(is_result<result>::value,
                      "F must return a result");

        static_assert(std::is_same_v<typename result::value_type, Err>,
                      "value type of flatmat return musst be same as the original value type");

        return hasValue()
            ? std::invoke(std::forward<Func>(f), getError())
            : Result{getValue()};
    }

    template<class Func>
    constexpr auto flatMapError(Func&& f) &
    {
        using result = std::invoke_result_t<Func, Err&>;
        static_assert(is_result<result>::value,
                      "F must return a result");

        static_assert(std::is_same_v<typename result::value_type, Err>,
                      "value type of flatmat return musst be same as the original value type");

        return hasValue()
            ? std::invoke(std::forward<Func>(f), getError())
            : Result{getValue()};
    }

    template<class Func>
    constexpr auto flatMapError(Func&& f) const&
    {
        using result = std::invoke_result_t<Func, const Err&>;
        static_assert(is_result<result>::value,
                      "F must return a result");

        static_assert(std::is_same_v<typename result::value_type, Err>,
                      "value type of flatmat return musst be same as the original value type");

        return hasValue()
            ? std::invoke(std::forward<Func>(f), getError())
            : Result{getValue()};
    }

    template<class Func>
    constexpr auto flatMapError(Func&& f) &&
    {
        using result = std::invoke_result_t<Func, Err&&>;
        static_assert(is_result<result>::value,
                      "F must return a result");

        static_assert(std::is_same_v<typename result::value_type, Err>,
                      "value type of flatmat return musst be same as the original value type");

        return hasValue()
            ? std::invoke(std::forward<Func>(f), getError())
            : Result{getValue()};
    }

    template<class Func>
    constexpr auto changeValue(Func&& func) & -> Result<T, Err>&
    {
        using FuncRet = std::invoke_result_t<Func, T&>;

        static_assert(std::is_void_v<FuncRet>,
                      "return of changeValue function must be void");

        if(*this) {
            func(getValue());
        }

        return *this;
    }

    template<class Func>
    constexpr auto changeValue(Func&& func) && -> Result<T, Err>&&
    {
        using FuncRet = std::invoke_result_t<Func, T&&>;

        static_assert(std::is_void_v<FuncRet>,
                      "return of changeValue function must be void");

        if(*this) {
            func(getValue());
        }

        return *this;
    }

    template<class Func>
    constexpr auto changeError(Func&& func) & -> Result<T, Err>&
    {
        using FuncRet = std::invoke_result_t<Func, Err&&>;

        static_assert(std::is_void_v<FuncRet>,
                      "return of changeValue function must be void");

        if(*this) {
            func(getError());
        }

        return *this;
    }

    template<class Func>
    constexpr auto changeError(Func&& func) && -> Result<T, Err>&&
    {
        using FuncRet = std::invoke_result_t<Func, Err&&>;

        static_assert(std::is_void_v<FuncRet>,
                      "return of changeValue function must be void");

        if(*this) {
            func(getError());
        }

        return *this;
    }

    //this serves as finally block
    //to execute functions which in a maner of
    //java finally blocks
    template<class Func>
    constexpr auto finally(Func&& func) const&
    {
        using FuncRet = std::invoke_result_t<Func, void>;

        static_assert(std::is_void_v<FuncRet>,
                      "return of changeValue function must be void");
        func();
        return *this;
    }

    template<class Func>
    constexpr auto finally(Func&& func) &
    {
        using FuncRet = std::invoke_result_t<Func, void>;

        static_assert(std::is_void_v<FuncRet>,
                      "return of changeValue function must be void");
        func();
        return *this;
    }

    template<class Func>
    constexpr auto finally(Func&& func) &&
    {

        using FuncRet = std::invoke_result_t<Func, void>;

        static_assert(std::is_void_v<FuncRet>,
                      "return of changeValue function must be void");
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

    constexpr auto getError() const& -> const Err&
    {
        return std::get<Err>(_var);
    }

    constexpr auto getError() & -> Err&
    {
        return std::get<Err>(_var);
    }

    constexpr auto getError() && -> Err&&
    {
        return std::get<Err>(std::move(_var));
    }

    constexpr auto hasValue() const
        -> bool
    {
        return static_cast<bool>(*this);
    }

    constexpr auto hasError() const
        -> bool
    {
        return !static_cast<bool>(*this);
    }

    template<class Func>
    constexpr auto map(Func&& f) &
    {
        using FuncRet = std::invoke_result_t<Func, void>;
        using ReturnType = Result<FuncRet, Err>;

        if constexpr(std::is_void_v<FuncRet>) {

            return hasValue()
                   ? f(),
                   ReturnType{}
                   : ReturnType{getError()};
        }

        return hasValue()
            ? ReturnType{std::invoke(std::forward<Func>(f))}
            : ReturnType{getError()};
    }

    template<class Func>
    constexpr auto map(Func&& f) const&
    {
        using FuncRet = std::invoke_result_t<Func, void>;
        using ReturnType = Result<FuncRet, Err>;

        if constexpr(std::is_void_v<FuncRet>) {

            return hasValue()
                   ? f(),
                   ReturnType{}
                   : ReturnType{getError()};
        }

        return hasValue()
            ? ReturnType{std::invoke(std::forward<Func>(f))}
            : ReturnType{getError()};
    }

    template<class Func>
    constexpr auto map(Func&& f) &&
    {
        using FuncRet = std::invoke_result_t<Func, void>;
        using ReturnType = Result<FuncRet, Err>;

        if constexpr(std::is_void_v<FuncRet>) {

            return hasValue()
                   ? f(),
                   ReturnType{}
                   : ReturnType{getError()};
        }

        return hasValue()
            ? ReturnType{std::invoke(std::forward<Func>(f))}
            : ReturnType{getError()};
    }

    template<class Func>
    constexpr auto flatMap(Func&& f) &
    {
        using result = std::invoke_result_t<Func, void>;
        static_assert(is_result<result>::value,
                      "F must return a result");

        static_assert(std::is_same_v<typename result::error_type, Err>,
                      "error type of flatmat return musst be same as the original error type");

        return hasValue()
            ? std::invoke(std::forward<Func>(f))
            : Result{getError()};
    }

    template<class Func>
    constexpr auto flatMap(Func&& f) const&
    {
        using result = std::invoke_result_t<Func, void>;
        static_assert(is_result<result>::value,
                      "F must return a result");

        static_assert(std::is_same_v<typename result::error_type, Err>,
                      "error type of flatmat return musst be same as the original error type");

        return hasValue()
            ? std::invoke(std::forward<Func>(f))
            : Result{getError()};
    }

    template<class Func>
    constexpr auto flatMap(Func&& f) &&
    {
        using result = std::invoke_result_t<Func, void>;
        static_assert(is_result<result>::value,
                      "F must return a result");

        static_assert(std::is_same_v<typename result::error_type, Err>,
                      "error type of flatmat return musst be same as the original error type");

        return hasValue()
            ? std::invoke(std::forward<Func>(f))
            : Result{getError()};
    }

    template<class Func>
    constexpr auto flatMapError(Func&& f) &
    {
        using result = std::invoke_result_t<Func, Err>;
        static_assert(is_result<result>::value,
                      "F must return a result");

        static_assert(std::is_same_v<typename result::value_type, Err>,
                      "value type of flatmat return musst be same as the original value type");

        return hasValue()
            ? std::invoke(std::forward<Func>(f))
            : Result{};
    }

    template<class Func>
    constexpr auto flatMapError(Func&& f) const&
    {
        using result = std::invoke_result_t<Func, Err>;
        static_assert(is_result<result>::value,
                      "F must return a result");

        static_assert(std::is_same_v<typename result::value_type, Err>,
                      "value type of flatmat return musst be same as the original value type");

        return hasValue()
            ? std::invoke(std::forward<Func>(f))
            : Result{};
    }

    template<class Func>
    constexpr auto flatMapError(Func&& f) &&
    {
        using result = std::invoke_result_t<Func, Err>;
        static_assert(is_result<result>::value,
                      "F must return a result");

        static_assert(std::is_same_v<typename result::value_type, Err>,
                      "value type of flatmat return musst be same as the original value type");

        return hasValue()
            ? std::invoke(std::forward<Func>(f))
            : Result{};
    }

    //this serves as finally block
    //to execute functions which in a maner of
    //java finally blocks
    template<class Func>
    constexpr auto finally(Func&& func) const&
    {
        using FuncRet = std::invoke_result_t<Func, void>;

        static_assert(std::is_void_v<FuncRet>,
                      "return of changeValue function must be void");
        func();
        return *this;
    }

    template<class Func>
    constexpr auto finally(Func&& func) &
    {
        using FuncRet = std::invoke_result_t<Func, void>;

        static_assert(std::is_void_v<FuncRet>,
                      "return of changeValue function must be void");
        func();
        return *this;
    }

    template<class Func>
    constexpr auto finally(Func&& func) &&
    {

        using FuncRet = std::invoke_result_t<Func, void>;

        static_assert(std::is_void_v<FuncRet>,
                      "return of changeValue function must be void");
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
        return result.getValue();
    }

    return result.getError();
}

template<class E>
auto flatten(Result<Result<void, E>, E>&& result)
    -> Result<void, E>
{
    if(result) {
        return {};
    }

    return result.getError();
}

template<class E, class T>
auto collect(std::vector<Result<T, E>>&& vec)
    -> Result<std::vector<T>, E>
{
    std::vector<T> ret_vec;
    ret_vec.resize(vec.size());

    for(auto&& res : vec) {
        if(!res) {
            return res.getError();
        }

        auto value = std::move(res.getValue());
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
            return res.getError();
        }
    }

    return {};
}


template<class T, class F>
auto traverse(const std::vector<T>& vec, F&& func)
    //enable if the result of F(T) is not a Result<void,...>
    -> std::enable_if_t<!std::is_void<typename std::invoke_result_t<F, T>::result_type>::value,
                        Result<std::vector<typename std::invoke_result_t<F, T>::result_type>,
                               typename std::invoke_result_t<F, T>::error_type>>
{
    using FuncRet = std::invoke_result_t<F, T>;

    static_assert(is_result<FuncRet>::value,
                  "return of the traversing function must be a result");


    std::vector<typename FuncRet::result_type> ret_vec;

    for(const auto& elem : vec) {
        auto res = func(elem);

        if(!res) {
            return res.getError();
        }

        ret_vec.push_back(std::move(res.getValue()));
    }

    return ret_vec;
}


//overload for functions returning Result<void,...>
//this function then returns Result<void, ...> also
template<class T, class F>
auto traverse(const std::vector<T>& vec, F&& func)
    //enable if the result of F(T) is a Result<void,...>
    -> std::enable_if_t<std::is_void<typename std::invoke_result_t<F, T>::result_type>::value,
                        Result<void,
                               typename std::invoke_result_t<F, T>::error_type>>
{
    using FuncRet = std::invoke_result_t<F, T>;

    static_assert(is_result<FuncRet>::value,
                  "return of the traversing function must be a result");

    for(auto&& elem : vec) {
        auto res = func(elem);

        if(!res) {
            return res.getError();
        }
    }

    return {};
}


template<class T, class V, class E>
auto combine(Result<T, E>&& first, Result<V, E>&& second)
    -> Result<std::pair<T, V>, E>
{
    if(first.hasValue()) {
        if(second.hasValue()) {
            return std::pair{std::move(first.getValue()),
                             std::move(second.getValue())};
        }

        return second.getError();
    }

    return first.getError();
}

} // namespace buddy::util
