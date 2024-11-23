#include "../src/Time.hpp"

#include <gtest/gtest.h>

TEST(Tests, TestConstructors) {
    auto a = Time();
    EXPECT_EQ(a, Time(0, 0, 0));

    auto b = Time(4, 50, 3);
    EXPECT_EQ(b, Time(4, 50, 3));
    EXPECT_EQ(b.get_hours(), 4);
    EXPECT_EQ(b.get_minutes(), 50);
    EXPECT_EQ(b.get_seconds(), 3);

    auto c = Time(-1, -50, 643);
    EXPECT_EQ(c, Time(22, 20, 43));
}

TEST(Tests, Test2) {
    auto b = Time(4, 50, 3);
    auto c = Time(-1, -50, 643);
    EXPECT_EQ(b + c, Time(23, 10, 26));
    EXPECT_EQ(b - c, Time(8, 29, 42));
}

TEST(Tests, Test3) {
    auto b = Time(49, -5, 4);
    EXPECT_EQ(b.to_seconds(), 3304);
    b += 10;
    EXPECT_EQ(b.to_seconds(), 3314);
    b -= 400;
    EXPECT_EQ(b.to_seconds(), 2914);
}

TEST(Tests, Test4) {
    auto b = Time(49, -5, 4);
    EXPECT_EQ(b.to_seconds(), 3304);
    b += 10;
    EXPECT_EQ(b.to_seconds(), 2914);
}