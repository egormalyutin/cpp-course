#ifndef APP_WINDOWER_H
#define APP_WINDOWER_H

#include <cassert>
#include <cmath>
#include <cstring>
#include <vector>

#include "../streams/Stream.hpp"

template <typename T> struct Windower {
    Windower(size_t size) : buffer(size) {
    }

    template <IsStream<T> S> size_t read_from(S &stream, size_t n) {
        std::memmove(buffer.data(), buffer.data() + n,
                     (buffer.size() - n) * sizeof(float));
        size_t r =
            stream.read_full(std::span(buffer.data() + buffer.size() - n, n));
        return r;
    }

    std::vector<T> buffer;
};

#endif