#include <gtest/gtest.h>

#include "mario/core/Camera.hpp"

namespace mario {
namespace {

constexpr int kLeadIn = config::kWindowWidth / 3;

TEST(CameraTest, StaysAtStartNearTheBeginning) {
    Camera camera;
    camera.follow(0);
    EXPECT_EQ(camera.x(), 0);
    camera.follow(kLeadIn);
    EXPECT_EQ(camera.x(), 0);
}

TEST(CameraTest, KeepsTargetOneThirdIntoTheView) {
    Camera camera;
    camera.follow(1000);
    EXPECT_EQ(camera.x(), 1000 - kLeadIn);
}

TEST(CameraTest, StopsAtTheEndOfTheLevel) {
    Camera camera;
    camera.follow(config::kWorldPixelWidth);
    EXPECT_EQ(camera.x(), config::kWorldPixelWidth - config::kWindowWidth);
    EXPECT_EQ(camera.x(), Camera::maxOffset());
}

TEST(CameraTest, ResetReturnsToStart) {
    Camera camera;
    camera.follow(3000);
    camera.reset();
    EXPECT_EQ(camera.x(), 0);
}

TEST(CameraTest, NarrowWorldNeverScrolls) {
    EXPECT_EQ(Camera::maxOffset(500, 800), 0);
    EXPECT_EQ(Camera::maxOffset(800, 800), 0);
    EXPECT_EQ(Camera::maxOffset(1000, 800), 200);
}

}  // namespace
}  // namespace mario
