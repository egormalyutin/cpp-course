#ifndef APP_VOCODER_STREAM_H
#define APP_VOCODER_STREAM_H

#include <cassert>
#include <cmath>
#include <complex>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>

#include "../../streams/Stream.hpp"

#include "../../streams/WindowStream.hpp"
#include "../../streams/Windower.hpp"

#include "../../misc/MelFilterBank.hpp"
#include "../../misc/fft.hpp"

template <IsStream<float> A, IsStream<float> B>
class VocoderStream : public WindowStream {
  public:
    VocoderStream(A &&a, B &&b, size_t window_size, size_t window_hop,
                  size_t bands = 40)
        : a(std::move(a)), b(std::move(b)),
          WindowStream(window_size, window_hop), a_windower(window_size),
          b_windower(window_size), win_f(window_size), a_fft(window_size),
          b_fft(window_size), abs_a(window_size / 2), abs_b(window_size / 2),
          bank(window_size / 2, 0., window_size / 2., window_size, bands),
          env_fft_a(bands), env_fft_b(bands) {
        for (size_t i = 0; i < window_size; i++) {
            float p = std::sin(M_PI * (float)i / (float)window_size);
            win_f[i] = p * p;
        }
    }

    bool add_window() override {
        auto ra = a_windower.read_from(a, hop);
        auto rb = b_windower.read_from(b, hop);
        if (ra < hop || rb < hop) {
            return false;
        }

        for (size_t i = 0; i < output_buffer.size(); i++) {
            a_fft[i] = a_windower.buffer[i] * win_f[i];
            b_fft[i] = b_windower.buffer[i] * win_f[i];
        }

        fft2(a_fft, false);
        fft2(b_fft, false);

        for (size_t i = 0; i < win_f.size() / 2; i++) {
            abs_a[i] = std::abs(a_fft[i]) * std::abs(a_fft[i]);
            abs_b[i] = std::abs(b_fft[i]) * std::abs(b_fft[i]);
        }

        size_t bands = env_fft_a.size();

        bank.apply(abs_a, env_fft_a);
        for (size_t i = 0; i < bands; i++) {
            env_fft_a[i] = std::sqrt(env_fft_a[i]);
        }
        bank.reconstruct(env_fft_a, abs_a);

        bank.apply(abs_b, env_fft_b);
        for (size_t i = 0; i < bands; i++) {
            env_fft_b[i] = std::sqrt(env_fft_b[i]);
        }
        bank.reconstruct(env_fft_b, abs_b);

        for (size_t i = 0; i < a_fft.size() / 2; i++) {
            if (abs_b[i] == 0.) {
                a_fft[i] = b_fft[i];
            } else {
                a_fft[i] = (b_fft[i] * abs_a[i] / abs_b[i]) * (float)2.;
            }
        }

        for (size_t i = a_fft.size() / 2; i < a_fft.size(); i++) {
            a_fft[i] = std::conj(a_fft[a_fft.size() - i]);
        }

        fft2(a_fft, true);

        for (size_t i = 0; i < output_buffer.size(); i++) {
            output_buffer[i] += a_fft[i].real();
        }

        return true;
    }

    std::optional<size_t> length() const override {
        return a.length();
    }

    StreamBox<float> clone() const override {
        return box_stream<VocoderStream<A, B>>(*this);
    }

  private:
    A a;
    B b;

    std::vector<float> win_f;

    Windower<float> a_windower;
    Windower<float> b_windower;

    std::vector<std::complex<float>> a_fft;
    std::vector<std::complex<float>> b_fft;

    std::vector<float> abs_a;
    std::vector<float> env_fft_a;
    std::vector<float> abs_b;
    std::vector<float> env_fft_b;

    MelFilterBank bank;
};

#endif