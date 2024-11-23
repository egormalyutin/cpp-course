#ifndef APP_MUTE_STREAM_H
#define APP_MUTE_STREAM_H

#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>

#include "../../streams/Stream.hpp"

template <IsStream<float> S> class MuteStream : public Stream<float> {
  public:
    MuteStream(S &&stream, size_t start, size_t end)
        : stream(std::move(stream)), start(start), end(end), offset(0) {
    }

    std::optional<size_t> read(std::span<float> out) override {
        auto r = stream.read(out);

        if (!r.has_value()) {
            return std::nullopt;
        }

        for (size_t i = 0; i < *r; i++) {
            if (offset + i >= start && offset + i < end) {
                out[i] = 0.;
            }
        }

        offset += *r;
        return r;
    }

    std::optional<size_t> length() const override {
        return stream.length();
    }

    StreamBox<float> clone() const override {
        return box_stream<MuteStream<S>>(*this);
    }

  private:
    S stream;
    size_t start;
    size_t end;
    size_t offset;
};

#endif