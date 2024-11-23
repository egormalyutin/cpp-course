#include "../src/register.hpp"

#include <cstdint>
#include <fstream>
#include <gtest/gtest.h>

inline uint32_t rotl32(uint32_t x, int32_t bits) {
    return x << bits | x >> (32 - bits);
}

uint32_t simple_hash(const uint8_t *data, size_t size) {
    uint32_t result = 0x55555555;
    for (size_t i = 0; i < size; i++) {
        result ^= data[i];
        result = rotl32(result, 5);
    }
    return result;
}

void _write_to(const std::string &path, const std::vector<char> &data) {
    std::ofstream stream;
    stream.exceptions(std::ofstream::failbit);
    stream.open(path);
    stream.write(data.data(), data.size());
}

TEST(Tests, Test1) {
    auto app = create_app();

    auto config = "";
    std::vector<std::string> in_paths{"../sounds/track1.wav"};

    auto data = app.run_collect(config, in_paths);

    auto hash = simple_hash((uint8_t *)data.data(), data.size());
    ASSERT_EQ(hash, 1534047269);
}

TEST(Tests, Test2) {
    auto app = create_app();

    auto config = "mix $2 10";
    std::vector<std::string> in_paths{"../sounds/track1.wav",
                                      "../sounds/track2.wav"};

    auto data = app.run_collect(config, in_paths);
    auto hash = simple_hash((uint8_t *)data.data(), data.size());
    ASSERT_EQ(hash, 70551530);
}

TEST(Tests, Test3) {
    auto app = create_app();

    auto config = "mute 1 2\n"
                  "vocoder $2";
    std::vector<std::string> in_paths{"../sounds/voice.wav",
                                      "../sounds/gangstaparadise.wav"};

    auto data = app.run_collect(config, in_paths);
    auto hash = simple_hash((uint8_t *)data.data(), data.size());
    ASSERT_EQ(hash, 3495596404);
}

TEST(Tests, Test4) {
    auto app = create_app();

    auto config = "# ...\n"
                  "mute 10 20 # mute\n"
                  "resample 50%\n\n\n";
    std::vector<std::string> in_paths{"../sounds/track2.wav",
                                      "../sounds/track3.wav"};

    auto data = app.run_collect(config, in_paths);
    auto hash = simple_hash((uint8_t *)data.data(), data.size());
    ASSERT_EQ(hash, 3777795033);
}
