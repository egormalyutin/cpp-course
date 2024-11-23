#include <iostream>
#include <set>

int main() {
    std::set<char> result;

    bool first = true;
    std::string str;
    while (std::cin >> str) {
        if (first) {
            for (char ch : str) {
                result.insert(ch);
            }
            first = false;
        } else {
            std::set<char> intersection;
            for (char ch : str) {
                if (result.contains(ch)) {
                    intersection.insert(ch);
                }
            }
            result = std::move(intersection);
        }
    }

    for (char ch : result) {
        std::cout << ch;
    }

    std::cout << std::endl;
}