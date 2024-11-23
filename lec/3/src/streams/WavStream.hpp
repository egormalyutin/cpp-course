#ifndef APP_WAV_STREAM_H
#define APP_WAV_STREAM_H

#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>

#include "../streams/Stream.hpp"

struct WavHeader {
    char chunk_id[4];
    std::uint32_t chunk_size;
    char format[4];
    char subchunk_1_id[4];
    std::uint32_t subchunk_1_size;
    std::uint16_t audio_format;
    std::uint16_t num_channels;
    std::uint32_t sample_rate;
    std::uint32_t byte_rate;
    std::uint16_t block_align;
    std::uint16_t bits_per_sample;
};

template <IsStream<char> S> class WavStream : public Stream<float> {
  public:
    WavStream(S &&stream)
        : stream(std::move(stream)), ready(false), len(0), ended(false),
          buffer(44100) {
    }

    std::optional<size_t> read(std::span<float> out) override {
        if (!ready) {
            prepare();
        }

        if (ended) {
            return std::nullopt;
        }

        size_t target_size = std::min(out.size(), buffer.size());

        auto r =
            stream.read_full(std::span((char *)buffer.data(), target_size * 2));
        if (r < target_size * 2) {
            ended = true;
        }

        for (size_t i = 0; i < r / 2; i++) {
            out[i] = buffer[i];
        }

        return std::make_optional(r / 2);
    }

    std::optional<size_t> length() const override {
        return std::make_optional(len);
    }

    StreamBox<float> clone() const override {
        return box_stream<WavStream<S>>(*this);
    }

  private:
    void prepare() {
        // read header
        WavHeader header;
        auto r = stream.read(std::span((char *)&header, sizeof(header)));
        if (r < sizeof(header)) {
            throw std::runtime_error("incorrect wav file");
        }

        auto correct = std::memcmp(header.chunk_id, "RIFF", 4) == 0 &&
                       std::memcmp(header.format, "WAVE", 4) == 0 &&
                       std::memcmp(header.subchunk_1_id, "fmt ", 4) == 0 &&
                       header.audio_format == 1 && header.num_channels == 1 &&
                       header.sample_rate == 44100 &&
                       header.byte_rate == 88200 && header.block_align == 2 &&
                       header.bits_per_sample == 16;

        if (!correct) {
            throw std::runtime_error("unsupported file format");
        }

        // skip metadata
        while (1) {
            char id[4];
            r = stream.read(id);
            if (r < sizeof(id)) {
                throw std::runtime_error("incorrect wav file");
            }

            std::uint32_t size;
            r = stream.read(std::span((char *)&size, 4));
            if (r < sizeof(size)) {
                throw std::runtime_error("incorrect wav file");
            }

            if (std::memcmp(id, "data", 4)) {
                r = stream.skip(size);
                if (r < sizeof(size)) {
                    throw std::runtime_error("incorrect wav file");
                }
            } else {
                if (size % 2 == 1) {
                    throw std::runtime_error("incorrect wav file");
                }

                len = size / 2;
                break;
            }
        }

        ready = true;
    }

    S stream;
    bool ready;
    size_t len;
    bool ended;
    std::vector<int16_t> buffer;
};

#endif