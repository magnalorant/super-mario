#include <gtest/gtest.h>

#include "mario/core/Platform.hpp"

namespace mario {
namespace {

constexpr int kSpeed = config::kPlatformSpeed;
constexpr int kRange = config::kPlatformRange;

TEST(PlatformTest, FactoriesSelectStrategyAndInitialDirection) {
    const Platform h = makeHorizontalPlatform(80, 120);
    const Platform v = makeVerticalPlatform(80, 120);
    ASSERT_NE(h.motion, nullptr);
    ASSERT_NE(v.motion, nullptr);
    EXPECT_EQ(h.motion->axis(), PlatformAxis::Horizontal);
    EXPECT_EQ(v.motion->axis(), PlatformAxis::Vertical);
    EXPECT_EQ(h.direction, 1);
    EXPECT_EQ(v.direction, -1);
    EXPECT_EQ(h.startX, 80);
    EXPECT_EQ(v.startY, 120);
}

TEST(PlatformTest, HorizontalPlatformMovesAndCarriesRider) {
    Platform p = makeHorizontalPlatform(0, 0);
    p.advance();
    EXPECT_EQ(p.x, kSpeed);
    EXPECT_EQ(p.y, 0);
    EXPECT_EQ(p.carryOffsetX(), kSpeed);
}

TEST(PlatformTest, HorizontalPlatformBouncesAtRangeEnd) {
    Platform p = makeHorizontalPlatform(0, 0);
    for (int i = 0; i < kRange / kSpeed + 1; ++i) {
        p.advance();
    }
    EXPECT_EQ(p.x, kRange);
    EXPECT_EQ(p.direction, -1);
    EXPECT_EQ(p.carryOffsetX(), -kSpeed);
}

TEST(PlatformTest, HorizontalPlatformStaysWithinRangeForever) {
    Platform p = makeHorizontalPlatform(100, 0);
    for (int i = 0; i < 1000; ++i) {
        p.advance();
        ASSERT_GE(p.x, p.startX);
        ASSERT_LE(p.x, p.startX + p.range);
    }
}

TEST(PlatformTest, VerticalPlatformRisesFirstAndNeverCarriesHorizontally) {
    Platform p = makeVerticalPlatform(0, 300);
    p.advance();
    EXPECT_EQ(p.y, 300 - kSpeed);
    EXPECT_EQ(p.x, 0);
    EXPECT_EQ(p.carryOffsetX(), 0);
}

TEST(PlatformTest, VerticalPlatformBouncesAtBothEnds) {
    Platform p = makeVerticalPlatform(0, 300);
    bool reachedTop = false;
    bool returnedToStart = false;
    for (int i = 0; i < 1000; ++i) {
        p.advance();
        ASSERT_GE(p.y, p.startY - p.range);
        ASSERT_LE(p.y, p.startY);
        if (p.y == p.startY - p.range) {
            reachedTop = true;
        }
        if (reachedTop && p.y == p.startY) {
            returnedToStart = true;
        }
    }
    EXPECT_TRUE(reachedTop);
    EXPECT_TRUE(returnedToStart);
}

}  // namespace
}  // namespace mario
