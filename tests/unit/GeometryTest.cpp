#include <gtest/gtest.h>

#include "mario/core/Geometry.hpp"

namespace mario {
namespace {

TEST(RectTest, OverlappingRectanglesIntersect) {
    constexpr Rect a{0, 0, 10, 10};
    constexpr Rect b{5, 5, 10, 10};
    EXPECT_TRUE(a.intersects(b));
    EXPECT_TRUE(b.intersects(a));
}

TEST(RectTest, TouchingEdgesDoNotIntersect) {
    constexpr Rect a{0, 0, 10, 10};
    EXPECT_FALSE(a.intersects(Rect{10, 0, 10, 10}));
    EXPECT_FALSE(a.intersects(Rect{0, 10, 10, 10}));
}

TEST(RectTest, EmptyRectangleNeverIntersects) {
    constexpr Rect a{0, 0, 10, 10};
    EXPECT_FALSE(a.intersects(Rect{2, 2, 0, 5}));
    EXPECT_FALSE(Rect{}.intersects(a));
}

TEST(RectTest, ContainedRectangleIntersects) {
    constexpr Rect outer{0, 0, 100, 100};
    EXPECT_TRUE(outer.intersects(Rect{40, 40, 1, 1}));
}

TEST(RectTest, TranslatedKeepsSize) {
    constexpr Rect moved = Rect{1, 2, 3, 4}.translated(10, -2);
    EXPECT_EQ(moved, (Rect{11, 0, 3, 4}));
}

TEST(RectTest, HorizontalVisibility) {
    EXPECT_TRUE((Rect{-5, 0, 10, 10}.visibleHorizontally(800)));
    EXPECT_TRUE((Rect{800, 0, 10, 10}.visibleHorizontally(800)));
    EXPECT_FALSE((Rect{-20, 0, 10, 10}.visibleHorizontally(800)));
    EXPECT_FALSE((Rect{801, 0, 10, 10}.visibleHorizontally(800)));
}

TEST(RectTest, IntersectionIsUsableAtCompileTime) {
    static_assert(Rect{0, 0, 2, 2}.intersects(Rect{1, 1, 2, 2}));
    static_assert(!Rect{0, 0, 2, 2}.intersects(Rect{2, 2, 2, 2}));
    SUCCEED();
}

}  // namespace
}  // namespace mario
