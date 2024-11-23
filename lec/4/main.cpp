#include "CSVParser.hpp"

#include <fstream>
#include <iostream>
#include <tuple>

template <typename Ch, typename Tr, typename... Args>
auto &operator<<(std::basic_ostream<Ch, Tr> &os, std::tuple<Args...> const &t);

template <typename Os, typename Tuple, std::size_t N> struct TuplePrinter {
    static void print(Os &os, const Tuple &t) {
        TuplePrinter<Os, Tuple, N - 1>::print(os, t);
        os << ", " << std::get<N>(t);
    }
};

template <typename Os, typename Tuple> struct TuplePrinter<Os, Tuple, 0> {
    static void print(Os &os, const Tuple &t) {
        os << std::get<0>(t);
    }
};

template <typename Ch, typename Tr, typename... Args>
auto &operator<<(std::basic_ostream<Ch, Tr> &os, std::tuple<Args...> const &t) {
    os << "std::tuple(";
    TuplePrinter<typeof(os), typeof(t), sizeof...(Args) - 1>::print(os, t);
    os << ")";
    return os;
}

// template <> void CSVParser::read_part(CSVParser::Context &ctx, float &f) {
//     ctx.skip_indents();
// }

int main() {
    std::ifstream fs("../1.csv");
    try {
        CSVParser::Parser<std::string, float, std::string> parser(fs);
        for (auto r : parser) {
            std::cout << r << std::endl;
        }
    } catch (CSVParser::Exception &e) {
        std::cout << e.describe() << std::endl;
    }
}