#ifndef APP_RESAMPLE_STREAM_H
#define APP_RESAMPLE_STREAM_H

#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <variant>
#include <vector>

#include "../../streams/BufferedStream.hpp"
#include "../../streams/Stream.hpp"

template <IsStream<float> S> struct DownsampleStream : public Stream<float> {
    DownsampleStream(S &&stream, float factor)
        : stream(std::move(stream), 44100), factor(factor), offset(0) {
    }

    std::optional<size_t> read(std::span<float> out) override {
        if (ended) {
            return std::nullopt;
        }

        for (size_t i = 0; i < out.size(); i++) {
            auto r = next();
            if (!r.has_value()) {
                ended = true;
                return std::make_optional(i);
            }

            out[i] = *r;
        }

        return std::make_optional(out.size());
    }

    std::optional<size_t> length() const override {
        auto r = stream.length();
        if (r.has_value()) {
            return std::make_optional((size_t)((float)*r / factor));
        } else {
            return std::nullopt;
        }
    }

    StreamBox<float> clone() const override {
        return box_stream<DownsampleStream<S>>(*this);
    }

  private:
    std::optional<float> next() {
        size_t start = (float)offset / factor;
        size_t end = (float)(offset + 1) / factor;
        size_t len = end - start;

        float sum = 0.;
        for (size_t i = 0; i < len; i++) {
            auto r = stream.read_single();
            if (!r.has_value()) {
                return std::nullopt;
            }
            sum += *r;
        }

        offset++;
        return std::make_optional(sum / (float)len);
    }

    BufferedStream<float, S> stream;
    size_t offset;
    float factor;
    bool ended;
};

template <IsStream<float> S> struct UpsampleStream : public Stream<float> {
    UpsampleStream(S &&stream, float factor)
        : stream(std::move(stream), 44100), factor(factor), offset(0) {
    }

    std::optional<size_t> read(std::span<float> out) override {
        if (ended) {
            return std::nullopt;
        }

        for (size_t i = 0; i < out.size(); i++) {
            auto r = next();
            if (!r.has_value()) {
                ended = true;
                return std::make_optional(i);
            }

            out[i] = *r;
        }

        return std::make_optional(out.size());
    }

    std::optional<size_t> length() const override {
        auto r = stream.length();
        if (r.has_value()) {
            return std::make_optional((size_t)((float)*r / factor));
        } else {
            return std::nullopt;
        }
    }

    StreamBox<float> clone() const override {
        return box_stream<UpsampleStream<S>>(*this);
    }

  private:
    std::optional<float> next() {
        if (repeat == 0) {
            size_t start = (float)offset * factor;
            size_t end = (float)(offset + 1) * factor;
            repeat = end - start;
            auto cur = stream.read_single();
            if (!cur.has_value()) {
                return std::nullopt;
            }
            current = *cur;
        }

        repeat--;
        offset++;
        return std::make_optional(current);
    }

    BufferedStream<float, S> stream;
    size_t offset;
    size_t repeat;
    float current;
    float factor;
    bool ended;
};

template <IsStream<float> S> struct ResampleStream : public Stream<float> {
    ResampleStream(S &&input, float factor) : stream() {
        if (factor > 1.) {
            stream = std::move(
                box_stream<UpsampleStream<S>>(std::move(input), factor));
        } else {
            stream = std::move(
                box_stream<DownsampleStream<S>>(std::move(input), factor));
        }
    }

    std::optional<size_t> read(std::span<float> out) override {
        return stream.read(out);
    }

    std::optional<size_t> length() const override {
        return stream.length();
    }

    StreamBox<float> clone() const override {
        return stream.clone();
    }

  private:
    StreamBox<float> stream;
};

#endif