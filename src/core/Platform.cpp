#include "mario/core/Platform.hpp"

namespace mario {

void HorizontalMotion::advance(Platform& platform) const {
    platform.x += platform.direction * config::kPlatformSpeed;
    const int maxX = platform.startX + platform.range;
    if (platform.x > maxX || platform.x < platform.startX) {
        platform.direction = -platform.direction;
        if (platform.x > maxX) {
            platform.x = maxX;
        }
        if (platform.x < platform.startX) {
            platform.x = platform.startX;
        }
    }
}

int HorizontalMotion::carryOffsetX(const Platform& platform) const {
    return platform.direction * config::kPlatformSpeed;
}

void VerticalMotion::advance(Platform& platform) const {
    platform.y += platform.direction * config::kPlatformSpeed;
    const int topY = platform.startY - platform.range;
    if (platform.direction < 0 && platform.y < topY) {
        platform.y = topY;
        platform.direction = 1;
    } else if (platform.direction > 0 && platform.y > platform.startY) {
        platform.y = platform.startY;
        platform.direction = -1;
    }
}

int VerticalMotion::carryOffsetX(const Platform& /*platform*/) const {
    return 0;
}

namespace {

const std::shared_ptr<const PlatformMotion>& horizontalMotion() {
    static const std::shared_ptr<const PlatformMotion> instance = std::make_shared<HorizontalMotion>();
    return instance;
}

const std::shared_ptr<const PlatformMotion>& verticalMotion() {
    static const std::shared_ptr<const PlatformMotion> instance = std::make_shared<VerticalMotion>();
    return instance;
}

}  // namespace

Platform makeHorizontalPlatform(int x, int y) {
    Platform platform;
    platform.x = x;
    platform.y = y;
    platform.startX = x;
    platform.startY = y;
    platform.direction = 1;
    platform.motion = horizontalMotion();
    return platform;
}

Platform makeVerticalPlatform(int x, int y) {
    Platform platform;
    platform.x = x;
    platform.y = y;
    platform.startX = x;
    platform.startY = y;
    platform.direction = -1;
    platform.motion = verticalMotion();
    return platform;
}

}  // namespace mario
