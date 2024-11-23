#ifndef APP_BUFFERED_STREAM_H
#define APP_BUFFERED_STREAM_H

#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>

#include "../streams/Stream.hpp"

template <typename T, IsStream<T> S> struct BufferedStream : public Stream<T> {
    BufferedStream(S &&stream, size_t buffer_size)
        : stream(std::move(stream)), buffer(buffer_size), buffer_len(0),
          buffer_offset(0), ended(false) {
    }

    std::optional<size_t> read(std::span<T> out) override {
        if (ended) {
            return std::nullopt;
        }

        if (buffer_offset == buffer_len) {
            auto r = stream.read_full(buffer);
            if (r == 0) {
                ended = true;
                return std::nullopt;
            }

            buffer_len = r;
            buffer_offset = 0;
        }

        auto r = std::min(buffer_len - buffer_offset, out.size());
        buffer_offset += r;

        std::memcpy(out.data(), buffer.data() + buffer_offset, r * sizeof(T));

        return std::make_optional(r);
    }

    std::optional<size_t> length() const override {
        return stream.length();
    }

    StreamBox<T> clone() const override {
        return box_stream<BufferedStream<T, S>>(*this);
    }

  private:
    S stream;
    std::vector<T> buffer;
    size_t buffer_len;
    size_t buffer_offset;
    bool ended;
};

#endif