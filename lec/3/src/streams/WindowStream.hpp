#ifndef APP_WINDOW_STREAM_H
#define APP_WINDOW_STREAM_H

#include <cassert>
#include <cmath>
#include <complex>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>

#include "../streams/Stream.hpp"

struct WindowStream : public Stream<float> {
    WindowStream(size_t size, size_t hop)
        : output_buffer(size), hop(hop), ended(false), remaining_output(0) {
    }

    std::optional<size_t> read(std::span<float> out) override {
        if (ended) {
            return std::nullopt;
        }

        if (remaining_output == 0) {
            std::memmove(output_buffer.data(), output_buffer.data() + hop,
                         (output_buffer.size() - hop) * sizeof(float));
            std::memset(output_buffer.data() + output_buffer.size() - hop, 0,
                        hop * sizeof(float));

            if (!add_window()) {
                ended = true;
                return std::nullopt;
            }
            remaining_output = hop;
        }

        size_t feed = std::min(out.size(), remaining_output);

        std::memcpy(out.data(), output_buffer.data() + hop - feed,
                    feed * sizeof(float));

        remaining_output -= feed;

        return std::make_optional(feed);
    }

    virtual bool add_window() = 0;

  protected:
    std::vector<float> output_buffer;
    size_t remaining_output;
    size_t hop;
    bool ended;
};

#endif