#include "Time.hpp"

int main() {
    Time t;
    t.set_hours(1);
    Time t2 = t;
    t2.set_hours(2);
    std::cout << t2 << std::endl;
    {
        t2.set_hours(12);
        std::cout << t2 << std::endl;
        Time t2(22, 22, 22);
        std::cout << t2 << std::endl;
    }
    std::cout << t2 << std::endl;
    Time t3(3, 3, 3);
    t3 = t;
    t3.set_hours(3);
    std::cout << t3 << std::endl;

    // auto c = Time(-1, -50, 644);
    // std::cout << "c" << c << std::endl;
}