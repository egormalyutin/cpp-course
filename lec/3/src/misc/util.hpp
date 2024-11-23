#ifndef APP_UTIL_HPP
#define APP_UTIL_HPP

#include <sstream>
#include <tuple>

template <typename T> struct _ArgType;

template <typename ClassType, typename Result, typename... Args>
struct _ArgType<Result (ClassType::*)(Args...)> {
    template <std::size_t i> struct arg {
        typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
    };
};

template <std::size_t i, typename F>
using ArgType = typename _ArgType<F>::template arg<i>::type;

template <typename Tuple, std::size_t N> struct FormatPrinter {
    static void print(std::stringstream &os, const Tuple &t) {
        FormatPrinter<Tuple, N - 1>::print(os, t);
        os << std::get<N>(t);
    }
};

template <typename Tuple> struct FormatPrinter<Tuple, 0> {
    static void print(std::stringstream &os, const Tuple &t) {
        os << std::get<0>(t);
    }
};

template <typename... Args> static std::string format(Args... args) {
    std::stringstream ss;
    auto tpl = std::make_tuple(args...);
    FormatPrinter<decltype(tpl), sizeof...(Args) - 1>::print(ss, tpl);
    return ss.str();
}

#endif