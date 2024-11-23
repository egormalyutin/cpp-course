#ifndef APP_MEL_FILTER_BANK_H
#define APP_MEL_FILTER_BANK_H

#include <cmath>
#include <span>
#include <vector>

struct MelFilterBank {
    MelFilterBank(size_t spectrum_length, float min_freq, float max_freq,
                  size_t sample_rate, size_t bands)
        : spectrum_length(spectrum_length), min_freq(min_freq),
          max_freq(max_freq), bands(bands), sample_rate(sample_rate),
          mat(bands * spectrum_length),
          reconstruct_mat(bands * spectrum_length) {

        prepare();
    }

    void apply(const std::span<float> input,
               std::span<float> output) const noexcept {
        for (size_t i = 0; i < bands; i++) {
            output[i] = 0.;
            for (size_t j = 0; j < spectrum_length; j++) {
                output[i] += mat[i * spectrum_length + j] * input[j];
            }
        }
    }

    void reconstruct(const std::span<float> input,
                     std::span<float> output) const noexcept {
        for (size_t i = 0; i < spectrum_length; i++) {
            output[i] = 0.;
            for (size_t j = 0; j < bands; j++) {
                output[i] += mat[j * spectrum_length + i] * input[j];
            }
        }
    }

  private:
    size_t spectrum_length;
    size_t sample_rate;
    float min_freq;
    float max_freq;
    size_t bands;

    std::vector<float> mat;
    std::vector<float> reconstruct_mat;

    static float freq2mel(float f) {
        return 1125. * std::log(1. + (f / 700.));
    }

    static float mel2freq(float m) {
        return 700. * (std::exp(m / 1125.) - 1.);
    }

    void prepare() {
        auto min_mel = freq2mel(min_freq);
        auto max_mel = freq2mel(max_freq);
        auto norm = (max_mel - min_mel) / ((float)bands - 1);
        for (size_t band = 0; band < bands; band++) {
            auto start_f = mel2freq((float)band * norm + min_mel);
            auto end_f = mel2freq((float)(band + 2) * norm + min_mel);
            size_t start = (spectrum_length + 1) * start_f / sample_rate;
            size_t end = (spectrum_length + 1) * end_f / sample_rate;
            triangle(
                std::span(mat.data() + band * spectrum_length, spectrum_length),
                start, end);
        }
    }

    static void triangle(std::span<float> arr, size_t start, size_t end) {
        triangle(std::span(arr.data() + start, end - start));
    }

    static void triangle(std::span<float> arr) {
        auto norm = (arr.size() % 2 == 0) ? ((float)arr.size() - 2.) / 2.
                                          : ((float)arr.size() - 1.) / 2.;
        for (size_t i = 0; i < arr.size() / 2; i++) {
            arr[i] = (float)i / norm;
        }
        for (size_t i = arr.size() / 2; i < arr.size(); i++) {
            arr[i] = (float)(arr.size() - i - 1) / norm;
        }
    }
};

#endif