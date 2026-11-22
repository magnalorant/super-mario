#pragma once

/**
 * @file Geometry.hpp
 * @brief Integer axis-aligned rectangle used for collision detection and rendering.
 */

namespace mario {

/**
 * @brief Axis-aligned rectangle with integer coordinates.
 *
 * The rectangle covers the half-open ranges [x, x + w) and [y, y + h), which matches the
 * semantics of SDL_HasIntersection without depending on SDL.
 */
struct Rect {
    int x{0};  ///< Left edge.
    int y{0};  ///< Top edge.
    int w{0};  ///< Width.
    int h{0};  ///< Height.

    /// @return True if the rectangle has no area.
    [[nodiscard]] constexpr bool empty() const noexcept { return w <= 0 || h <= 0; }

    /// @return The first x coordinate to the right of the rectangle.
    [[nodiscard]] constexpr int right() const noexcept { return x + w; }

    /// @return The first y coordinate below the rectangle.
    [[nodiscard]] constexpr int bottom() const noexcept { return y + h; }

    /**
     * @brief Tests whether two rectangles overlap.
     * @param other Rectangle to test against.
     * @return True if both rectangles are non-empty and share at least one pixel.
     */
    [[nodiscard]] constexpr bool intersects(const Rect& other) const noexcept {
        if (empty() || other.empty()) {
            return false;
        }
        return x < other.right() && other.x < right() && y < other.bottom() && other.y < bottom();
    }

    /**
     * @brief Returns a copy moved by the given offset.
     * @param dx Horizontal offset.
     * @param dy Vertical offset.
     */
    [[nodiscard]] constexpr Rect translated(int dx, int dy) const noexcept { return {x + dx, y + dy, w, h}; }

    /// @return True if the rectangle overlaps the horizontal screen span [0, screenWidth].
    [[nodiscard]] constexpr bool visibleHorizontally(int screenWidth) const noexcept {
        return right() >= 0 && x <= screenWidth;
    }
};

/// @return True if both rectangles have identical position and size.
[[nodiscard]] constexpr bool operator==(const Rect& a, const Rect& b) noexcept {
    return a.x == b.x && a.y == b.y && a.w == b.w && a.h == b.h;
}

/// @return True if the rectangles differ in position or size.
[[nodiscard]] constexpr bool operator!=(const Rect& a, const Rect& b) noexcept { return !(a == b); }

}  // namespace mario
