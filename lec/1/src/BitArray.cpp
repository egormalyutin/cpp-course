#include <algorithm>
#include <assert.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <ostream>
#include <stdexcept>

#include "BitArray.h"
#include "helpers.cpp"

// Private constructor
BitArray::BitArray(std::size_t _cap, std::size_t len, bool fill)
    : len(len), data(nullptr) {
    set_cap(_cap);

    if (fill) {
        for (std::size_t i = 0; i < chunks(len); i++) {
            data[i] = 0;
        }
    }
}

// Default constructor
BitArray::BitArray() {
    data = nullptr;
    len = 0;
    cap = 0;
}

// Initialize with given length
BitArray::BitArray(std::size_t len, std::uint64_t value)
    : BitArray(len, len, true) {
    data[0] = value;
}

// Initialize with init list
BitArray::BitArray(const std::initializer_list<bool> list)
    : BitArray(list.size(), list.size(), false) {
    std::size_t i = 0;
    for (auto iter = list.begin(); iter < list.end(); iter++, i++) {
        (*this)[i] = *iter;
    }
}

// Initialize with vector
BitArray::BitArray(const std::vector<bool> &vec)
    : BitArray(vec.size(), vec.size(), false) {
    for (std::size_t i = 0; i < vec.size(); i++) {
        (*this)[i] = vec[i];
    }
}

// Copy constructor
BitArray::BitArray(const BitArray &b) {
    len = b.len;
    cap = b.len;
    data = (std::uint64_t *)malloc(chunks(len) * 8);
    memcpy(data, b.data, chunks(len) * 8);
}

// Move constructor
BitArray::BitArray(BitArray &&b) {
    len = b.len;
    cap = b.cap;
    data = b.data;
    b.len = 0;
    b.cap = 0;
    b.data = nullptr;
}

// Destructor
BitArray::~BitArray() {
    if (data != nullptr) {
        free(data);
        data = nullptr;
    }
    len = 0;
    cap = 0;
}

// Set cap
void BitArray::set_cap(std::size_t new_cap) {
    cap = new_cap;
    if (cap == 0 && data != nullptr) {
        free(data);
        data = nullptr;
        return;
    }

    // malloc и realloc используются в реализации, т. к. с new нельзя
    // динамически менять размер выделенной памяти (можно только выделить новый
    // кусок и скопировать туда старые значения). В данном случае так делать
    // можно, т.к. данные в массиве очевидно trivially relocatable и move
    // constructor вызывать при их перемещении не надо. Я не стал использовать
    // std::vector, потому что я захотел его сделать сам :)
    if (data == nullptr) {
        data = (std::uint64_t *)malloc(chunks(cap) * 8);
    } else {
        data = (std::uint64_t *)realloc(data, chunks(cap) * 8);
    }
}

// Swap
void BitArray::swap(BitArray &b) {
    std::swap(len, b.len);
    std::swap(cap, b.cap);
    std::swap(data, b.data);
}

// Copy
BitArray &BitArray::operator=(const BitArray &b) {
    if (data == b.data) {
        return *this;
    }

    if (data != nullptr) {
        free(data);
    }

    len = b.len;
    cap = b.len;
    data = (std::uint64_t *)malloc(chunks(cap) * 8);
    memcpy(data, b.data, chunks(cap) * 8);
    return *this;
}

// move
BitArray &BitArray::operator=(BitArray &&b) {
    if (data == b.data) {
        return *this;
    }

    if (data != nullptr) {
        free(data);
    }

    len = b.len;
    cap = b.cap;
    data = b.data;
    b.len = 0;
    b.cap = 0;
    b.data = nullptr;

    return *this;
}

// Grow array so that capacity >= least_cap
void BitArray::grow_at_least(std::size_t least_cap) {
    if (least_cap <= cap) {
        return;
    }

    std::size_t new_cap = cap < 2 ? 2 : cap;
    while (new_cap < len + 1) {
        new_cap = new_cap + new_cap / 2;
    }

    set_cap(new_cap);
}

// Resize
void BitArray::resize(std::size_t length, bool value) {
    size_t prev_len = len;

    set_cap(length);
    len = length;

    if (prev_len < len) {
        set_range(prev_len, len - prev_len, value);
    }
}

// Clear
void BitArray::clear() {
    len = 0;
}

// Push
void BitArray::push_back(bool bit) {
    grow_at_least(len + 1);
    len++;
    set_single(len - 1, bit);
}

BitArray &BitArray::operator&=(const BitArray &b) {
    assert(len == b.len);
    for (std::size_t i = 0; i < chunks(cap); i++) {
        data[i] &= b.data[i];
    }
    return *this;
}

BitArray &BitArray::operator|=(const BitArray &b) {
    assert(len == b.len);
    for (std::size_t i = 0; i < chunks(cap); i++) {
        data[i] |= b.data[i];
    }
    return *this;
}

BitArray &BitArray::operator^=(const BitArray &b) {
    assert(len == b.len);
    for (std::size_t i = 0; i < chunks(cap); i++) {
        data[i] ^= b.data[i];
    }
    return *this;
}

BitArray &BitArray::operator<<=(int n) {
    if (n == 0) {
        return *this;
    }

    for (std::size_t i = n; i < len; i++) {
        (*this)[i - n] = (bool)(*this)[i];
    }
    for (std::size_t i = len - n; i < len; i++) {
        (*this)[i] = 0;
    }

    return *this;
}

BitArray &BitArray::operator>>=(int n) {
    if (n == 0) {
        return *this;
    }

    for (std::size_t i = len - n + 1; i > 0; i--) {
        (*this)[i + n - 1] = (bool)(*this)[i - 1];
    }

    for (std::size_t i = 0; i < n; i++) {
        (*this)[i] = 0;
    }

    return *this;
}

BitArray BitArray::operator<<(int n) const {
    BitArray arr(*this);
    arr <<= n;
    return arr;
}

BitArray BitArray::operator>>(int n) const {
    BitArray arr(*this);
    arr >>= n;
    return arr;
}

BitArray &BitArray::set() {
    for (std::size_t i = 0; i < chunks(len); i++) {
        data[i] = ~0;
    }
    return *this;
}

BitArray &BitArray::reset(std::size_t n) {
    set_single(n, false);
    return *this;
}

BitArray &BitArray::reset() {
    for (std::size_t i = 0; i < chunks(len); i++) {
        data[i] = 0;
    }
    return *this;
}

bool BitArray::any() const {
    return any_arr(data, len);
}

bool BitArray::none() const {
    return !any();
}

BitArray BitArray::operator~() const {
    BitArray arr(len, len, false);
    for (std::size_t i = 0; i < chunks(len); i++) {
        arr.data[i] = ~data[i];
    }
    return arr;
}

std::size_t BitArray::count() const {
    return arr_popcnt(data, len);
}

std::size_t BitArray::size() const {
    return len;
}

bool BitArray::empty() const {
    return len == 0;
}

std::string BitArray::to_string() const {
    std::string str(len, '0');
    for (std::size_t i = 0; i < len; i++) {
        if (get_single(i)) {
            str[i] = '1';
        }
    }
    return str;
}

// mutable reference
BitArray::Reference BitArray::operator[](std::size_t i) {
    return BitArray::Reference(*this, i);
}

BitArray::Reference::Reference(BitArray &array, std::size_t index)
    : array(array), index(index) {
}

bool BitArray::Reference::operator=(bool value) {
    array.set_single(index, value);
    return value;
}

BitArray::Reference::operator bool() const {
    return array.get_single(index);
}

// const access
bool BitArray::operator[](std::size_t i) const {
    return get_single(i);
}

// safe mutable reference
BitArray::SafeReference BitArray::at(std::size_t i) {
    return BitArray::SafeReference(*this, i);
}

BitArray::SafeReference::SafeReference(BitArray &array, std::size_t index)
    : array(array), index(index) {
}

bool BitArray::SafeReference::operator=(bool value) {
    if (index >= array.len) {
        throw std::out_of_range("invalid index");
    }
    array.set_single(index, value);
    return value;
}

BitArray::SafeReference::operator bool() const {
    if (index >= array.len) {
        throw std::out_of_range("invalid index");
    }
    return array.get_single(index);
}

// safe const access
bool BitArray::at(std::size_t i) const {
    if (i >= len) {
        throw std::out_of_range("invalid index");
    }
    return get_single(i);
}

// internal set
void BitArray::set_single(std::size_t n, bool val) {
    set_bit_arr(data, n, val);
}

// internal get
bool BitArray::get_single(std::size_t i) const {
    return get_bit_arr(data, i);
}

bool operator==(const BitArray &a, const BitArray &b) {
    if (a.size() != b.size()) {
        return false;
    }
    return arr_eq(a.data, b.data, a.len);
}

bool operator!=(const BitArray &a, const BitArray &b) {
    return !(a == b);
}

BitArray operator&(const BitArray &b1, const BitArray &b2) {
    BitArray arr(b1);
    arr &= b2;
    return arr;
}

BitArray operator|(const BitArray &b1, const BitArray &b2) {
    BitArray arr(b1);
    arr |= b2;
    return arr;
}

BitArray operator^(const BitArray &b1, const BitArray &b2) {
    BitArray arr(b1);
    arr ^= b2;
    return arr;
}

void BitArray::set_range(std::size_t offset, std::size_t len, bool value) {
    for (std::size_t i = 0; i < len; i++) {
        (*this)[offset + i] = value;
    }
}

std::ostream &operator<<(std::ostream &stream, const BitArray &arr) {
    for (std::size_t i = 0; i < arr.len; i++) {
        stream << arr[i];
    }
    return stream;
}
