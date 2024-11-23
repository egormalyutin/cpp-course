#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <fstream>
#include <initializer_list>
#include <ios>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

#include <bitset>
#include <ctime>
#include <vector>

#include "BitArray.h"

std::vector<bool> random_vec(std::size_t n) {
    std::vector<bool> v(n);
    for (std::size_t i = 0; i < n; i++) {
        v[i] = std::rand() & 1;
    }
    return v;
}

void test() {
    std::vector<std::size_t> test_sizes{0, 1, 2, 10, 16, 52, 100, 2048};
    // std::vector<std::size_t> test_sizes{2048};

    // Test empty constructor
    {
        BitArray arr;
        assert(arr.data == NULL);
        assert(arr.len == 0);
        assert(arr.cap == 0);
    }

    // Test vector initializer
    for (auto size : test_sizes) {
        auto vec = random_vec(size);

        BitArray arr(vec);
        assert(arr.len == vec.size());
        assert(arr.cap == vec.size());

        for (std::size_t i = 0; i < vec.size(); i++) {
            assert(arr[i] == vec[i]);
            const BitArray &a = arr;
            assert(a[i] == vec[i]);
        }
    }

    // Test initializer list initializer
    {
        BitArray a0{};
        assert(a0.len == 0);
        assert(a0.cap == 0);

        BitArray a1{1};
        assert(a1.len == 1);
        assert(a1.cap == 1);
    }

    // Test length initializer
    for (auto size : test_sizes) {
        BitArray a0(size);
        assert(a0.len == size);
        assert(a0.cap == size);

        for (std::size_t i = 0; i < size; i++) {
            assert(a0[i] == 0);
        }

        BitArray a1(size, 4);
        assert(a1.len == size);
        assert(a1.cap == size);

        for (std::size_t i = 0; i < size; i++) {
            assert(a1[i] == (i == 2));
        }
    }

    // Test copy constructor
    for (auto size : test_sizes) {
        BitArray a0(random_vec(size));
        BitArray a1(a0);

        assert(a0 == a1);

        if (size != 0) {
            a0[0] = !a0[0];
            assert(a0 != a1);
        }
    }

    // Test swap
    for (auto size : test_sizes) {
        auto v0 = random_vec(size);
        auto v1 = random_vec(size);

        BitArray a0(v0), a1(v1);

        std::swap(a0, a1);

        assert(a0 == v1);
        assert(a1 == v0);
    }

    // Test &=
    for (auto size : test_sizes) {
        auto v0 = random_vec(size);
        auto v1 = random_vec(size);

        BitArray a0(v0), a1(v1);
        a0 = a1;

        assert(a0 == v1);

        a0 = a0;
        assert(a0 == v1);

        if (size != 0) {
            a0[0] = !a0[0];
            assert(a0 != a1);
        }
    }

    // Test &&=
    for (auto size : test_sizes) {
        auto v0 = random_vec(size);
        auto v1 = random_vec(size);

        BitArray a0(v0), a1(v1);
        a0 = std::move(a1);

        assert(a0 == BitArray(v1));
        assert(a1.len == 0);
        assert(a1.cap == 0);
        assert(a1.data == NULL);
    }

    // Test move constructor
    for (auto size : test_sizes) {
        auto v0 = random_vec(size);

        BitArray a0(v0);
        BitArray a1(std::move(a0));

        assert(a1 == BitArray(v0));
        assert(a0.len == 0);
        assert(a0.cap == 0);
        assert(a0.data == NULL);
    }

    // Test clear
    for (auto size : test_sizes) {
        BitArray a(random_vec(size));
        a.clear();
        assert(a.len == 0);
    }

    // Test push
    for (auto size : test_sizes) {
        auto v = random_vec(size);

        BitArray a;

        for (std::size_t i = 0; i < v.size(); i++) {
            a.push_back(v[i]);
        }

        assert(a == v);
    }

    // Test &=
    for (auto size : test_sizes) {
        auto v0 = random_vec(size);
        auto v1 = random_vec(size);

        BitArray a0(v0);
        BitArray a1(v1);

        a0 &= a1;

        for (std::size_t i = 0; i < size; i++) {
            assert(a0[i] == (v0[i] & v1[i]));
        }
    }

    // Test |=
    for (auto size : test_sizes) {
        auto v0 = random_vec(size);
        auto v1 = random_vec(size);

        BitArray a0(v0);
        BitArray a1(v1);

        a0 |= a1;

        for (std::size_t i = 0; i < size; i++) {
            assert(a0[i] == (v0[i] | v1[i]));
        }
    }

    // Test ^=
    for (auto size : test_sizes) {
        auto v0 = random_vec(size);
        auto v1 = random_vec(size);

        BitArray a0(v0);
        BitArray a1(v1);

        a0 ^= a1;

        for (std::size_t i = 0; i < size; i++) {
            assert(a0[i] == (v0[i] ^ v1[i]));
        }
    }

    // Test set()
    for (auto size : test_sizes) {
        auto v = random_vec(size);
        BitArray a(v);
        a.set();
        assert(a == std::vector<bool>(size, true));
    }

    // Test reset()
    for (auto size : test_sizes) {
        auto v = random_vec(size);
        BitArray a(v);
        a.reset();
        assert(a == std::vector<bool>(size, false));
    }

    // Test reset(n)
    for (auto size : test_sizes) {
        if (size == 0) {
            continue;
        }

        std::size_t i = std::rand() % size;
        auto v = random_vec(size);
        BitArray a(v);
        v[i] = false;
        a.reset(i);
        assert(a == v);
    }

    // Test any()
    for (auto size : test_sizes) {
        if (size == 0) {
            continue;
        }

        std::size_t i = std::rand() % size;
        BitArray a(size);
        assert(!a.any());
        a[i] = true;
        assert(a.any());
    }

    // Test none()
    for (auto size : test_sizes) {
        if (size == 0) {
            continue;
        }

        std::size_t i = std::rand() % size;
        BitArray a(size);
        assert(a.none());
        a[i] = true;
        assert(!a.none());
    }

    // Test ~
    for (auto size : test_sizes) {
        auto v = random_vec(size);
        BitArray a(v);
        auto r = ~a;

        for (std::size_t i = 0; i < size; i++) {
            assert(r[i] == !v[i]);
        }
    }

    // Test count()
    for (auto size : test_sizes) {
        auto v = random_vec(size);
        BitArray a(v);

        std::size_t count = 0;
        for (std::size_t i = 0; i < size; i++) {
            count += v[i];
        }

        assert(count == a.count());
    }

    // Test size()
    for (auto size : test_sizes) {
        auto v = random_vec(size);
        BitArray a(v);

        assert(a.size() == size);
    }

    // Test empty()
    for (auto size : test_sizes) {
        if (size == 0) {
            continue;
        }

        auto v = random_vec(size);
        BitArray a(v);

        assert(!a.empty());
        a.clear();
        assert(a.empty());
    }

    // Test safe access
    for (auto size : test_sizes) {
        auto v = random_vec(size);
        BitArray a(v);

        for (std::size_t i = 0; i < size; i++) {
            assert(a.at(i) == v[i]);
        }

        try {
            bool r = a.at(size);
            assert(false);
        } catch (std::out_of_range &e) {
        }
    }

    // Test const safe access
    for (auto size : test_sizes) {
        auto v = random_vec(size);
        BitArray aw(v);
        auto &a = aw;

        for (std::size_t i = 0; i < size; i++) {
            assert(a.at(i) == v[i]);
        }

        try {
            bool r = a.at(size);
            assert(false);
        } catch (std::out_of_range &e) {
        }
    }

    // Test const access
    for (auto size : test_sizes) {
        auto v = random_vec(size);
        BitArray aw(v);
        auto &a = aw;

        for (std::size_t i = 0; i < size; i++) {
            assert(a[i] == v[i]);
        }
    }

    // Test >>=
    {
        auto v = random_vec(2048);
        BitArray a(v);
        std::bitset<2048> b;
        for (std::size_t i = 0; i < 2048; i++) {
            b[i] = v[i];
        }
        b <<= 3;
        a >>= 3;
        for (std::size_t i = 0; i < 2048; i++) {
            assert(a[i] == b[i]);
        }
    }

    // Test <<=
    {
        auto v = random_vec(2048);
        BitArray a(v);
        std::bitset<2048> b;
        for (std::size_t i = 0; i < 2048; i++) {
            b[i] = v[i];
        }
        b >>= 3;
        a <<= 3;
        for (std::size_t i = 0; i < 2048; i++) {
            assert(a[i] == b[i]);
        }
    }
}

int main(int argc, char **argv) {
    std::srand(std::time(nullptr));
    test();

    // auto v = random_vec(2048);
    // BitArray a(v);
    // std::cout << a << std::endl;
}
