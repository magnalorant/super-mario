#pragma once

/**
 * @file Camera.hpp
 * @brief Horizontal scrolling camera.
 */

#include "mario/core/Config.hpp"

namespace mario {

/**
 * @brief Side-scrolling camera that keeps the player in the left third of the screen.
 */
class Camera {
public:
    /// @return The largest valid camera offset for a level of the given pixel width.
    [[nodiscard]] static constexpr int maxOffset(int worldWidth = config::kWorldPixelWidth,
                                                 int viewWidth = config::kWindowWidth) noexcept {
        return worldWidth > viewWidth ? worldWidth - viewWidth : 0;
    }

    /**
     * @brief Moves the camera so that the target stays one third into the view.
     * @param targetX World x coordinate to follow.
     */
    void follow(int targetX) noexcept;

    /// @brief Scrolls back to the start of the level.
    void reset() noexcept { offsetX_ = 0; }

    /// @return Horizontal world offset of the left screen edge.
    [[nodiscard]] int x() const noexcept { return offsetX_; }

private:
    int offsetX_{0};
};

}  // namespace mario
