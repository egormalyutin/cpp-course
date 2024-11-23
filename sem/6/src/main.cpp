#include "Time.hpp"
#include <list>
#include <memory>
#include <stdexcept>
#include <vector>

int main() {
    auto h = new Time(5, 6, 6);
    delete h;

    auto hs = new Time[10];
    delete[] hs;

    auto u = std::make_unique<Time>(1, 2, 3);
    auto s = std::make_shared<Time>(1, 2, 3);

    std::vector<Time> v(100);
    std::list<Time> l(100);

    try {
        u->break_everything();
        std::cout << "expected error???" << std::endl;
    } catch (std::runtime_error &err) {
        std::cout << "err: " << err.what() << std::endl;
    }
}