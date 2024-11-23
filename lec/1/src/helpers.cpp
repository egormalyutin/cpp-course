#include <bit>
#include <cstddef>
#include <cstdint>
#include <iostream>

size_t ceil_div(size_t n, size_t m) {
    return (n + m - 1) / m;
}

std::uint64_t set_bit(std::uint64_t data, std::size_t n, bool value) {
    if (value) {
        return data | ((std::uint64_t)1 << n);
    } else {
        return data & ~((std::uint64_t)1 << n);
    }
}

bool get_bit(std::uint64_t data, std::size_t n) {
    return !!(data & ((std::uint64_t)1 << n));
}

void set_bit_arr(std::uint64_t *data, std::size_t n, bool value) {
    data[n / 64] = set_bit(data[n / 64], n % 64, value);
}

bool get_bit_arr(std::uint64_t *data, std::size_t n) {
    return get_bit(data[n / 64], n % 64);
}

bool any_arr(std::uint64_t *data, std::size_t len) {
    for (std::size_t i = 0; i < len; i++) {
        if (get_bit_arr(data, i)) {
            return true;
        }
    }
    return false;
}

std::size_t arr_popcnt(std::uint64_t *arr, std::size_t len) {
    std::size_t n = 0;
    for (std::size_t i = 0; i < len; i++) {
        n += get_bit_arr(arr, i);
    }
    return n;
}

std::size_t arr_eq(std::uint64_t *a, std::uint64_t *b, std::size_t len) {
    std::size_t n = 0;
    for (std::size_t i = 0; i < len; i++) {
        if (get_bit_arr(a, i) != get_bit_arr(b, i)) {
            return false;
        }
    }
    return true;
}

void set_bit_range(std::uint64_t *data, std::size_t offset, std::size_t len,
                   bool value) {
    for (std::size_t i = 0; i < len; i++) {
        set_bit_arr(data, offset + i, value);
    }
}