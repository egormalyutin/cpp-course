#ifndef APP_MIX_STREAM_H
#define APP_MIX_STREAM_H

#include <cstdint>
#include <cstring>
#include <optional>
#include <stdexcept>
#include <vector>

#include "../../streams/Stream.hpp"

template <IsStream<float> A, IsStream<float> B>
class MixStream : public Stream<float> {
  public:
    MixStream(A &&a, B &&b, size_t insert_at = 0)
        : a(std::move(a)), b(std::move(b)), b_buffer(44100), a_ended(false),
          b_ended(false), insert_at_remaining(insert_at) {
    }

    std::optional<size_t> read(std::span<float> out) override {
        if (insert_at_remaining != 0) {
            auto r_max = std::min(insert_at_remaining, out.size());
            auto r = a.read(std::span(out.begin(), r_max));
            if (r.has_value()) {
                insert_at_remaining -= *r;
            }
            return r;
        }

        if (a_ended) {
            return std::nullopt;
        }

        auto ar = a.read_full(out);
        if (ar < out.size()) {
            a_ended = true;
        }

        if (b_ended) {
            return std::make_optional(ar);
        }

        auto br = b.read_full(std::span(b_buffer.data(), ar));
        if (br < ar) {
            b_ended = true;
        }

        for (size_t i = 0; i < br; i++) {
            out[i] = (out[i] + b_buffer[i]) * 0.5;
        }

        return std::make_optional(ar);
    }

    std::optional<size_t> length() const override {
        return a.length();
    }

    StreamBox<float> clone() const override {
        return box_stream<MixStream<A, B>>(*this);
    }

  private:
    A a;
    bool a_ended;

    B b;
    bool b_ended;
    std::vector<float> b_buffer;

    size_t insert_at_remaining;
};

#endif