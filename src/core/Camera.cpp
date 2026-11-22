#include "mario/core/Camera.hpp"

#include <algorithm>

namespace mario {

void Camera::follow(int targetX) noexcept {
    constexpr int leadIn = config::kWindowWidth / 3;
    offsetX_ = std::clamp(targetX - leadIn, 0, maxOffset());
}

}  // namespace mario
