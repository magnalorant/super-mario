#pragma once

/**
 * @file Platform.hpp
 * @brief Moving platforms and their interchangeable motion strategies.
 */

#include <memory>

#include "mario/core/Config.hpp"
#include "mario/core/Geometry.hpp"

namespace mario {

struct Platform;

/// @brief Axis a platform travels along.
enum class PlatformAxis {
    Horizontal,  ///< Moves between startX and startX + range.
    Vertical,    ///< Moves between startY - range and startY.
};

/**
 * @brief Strategy interface describing how a platform moves.
 *
 * Platforms differ only in their movement rule, so the rule is injected as a stateless
 * strategy object instead of being selected by a type switch in the update loop. New motion
 * types can be added without touching Platform or World.
 */
class PlatformMotion {
public:
    PlatformMotion() = default;
    virtual ~PlatformMotion() = default;
    PlatformMotion(const PlatformMotion&) = delete;
    PlatformMotion& operator=(const PlatformMotion&) = delete;
    PlatformMotion(PlatformMotion&&) = delete;
    PlatformMotion& operator=(PlatformMotion&&) = delete;

    /// @brief Moves the platform by one frame and bounces it at the ends of its range.
    virtual void advance(Platform& platform) const = 0;

    /// @return Horizontal distance a rider is carried during the current frame.
    [[nodiscard]] virtual int carryOffsetX(const Platform& platform) const = 0;

    /// @return The axis this strategy moves along.
    [[nodiscard]] virtual PlatformAxis axis() const noexcept = 0;
};

/// @brief Moves a platform left and right.
class HorizontalMotion final : public PlatformMotion {
public:
    void advance(Platform& platform) const override;
    [[nodiscard]] int carryOffsetX(const Platform& platform) const override;
    [[nodiscard]] PlatformAxis axis() const noexcept override { return PlatformAxis::Horizontal; }
};

/// @brief Moves a platform up and down.
class VerticalMotion final : public PlatformMotion {
public:
    void advance(Platform& platform) const override;
    [[nodiscard]] int carryOffsetX(const Platform& platform) const override;
    [[nodiscard]] PlatformAxis axis() const noexcept override { return PlatformAxis::Vertical; }
};

/**
 * @brief A platform the player can ride.
 */
struct Platform {
    int x{0};                        ///< Current left edge.
    int y{0};                        ///< Current top edge.
    int w{config::kTileSize};        ///< Width in pixels.
    int h{config::kTileSize};        ///< Height in pixels.
    int direction{1};                ///< +1 or -1 along the motion axis.
    int range{config::kPlatformRange};  ///< Travel distance in pixels.
    int startX{0};                   ///< Anchor x coordinate.
    int startY{0};                   ///< Anchor y coordinate.
    std::shared_ptr<const PlatformMotion> motion;  ///< Movement strategy, shared between platforms.

    /// @return Collision rectangle in world coordinates.
    [[nodiscard]] Rect bounds() const noexcept { return {x, y, w, h}; }

    /// @brief Moves the platform by one frame using its strategy.
    void advance() { motion->advance(*this); }

    /// @return Horizontal distance a rider is carried this frame.
    [[nodiscard]] int carryOffsetX() const { return motion->carryOffsetX(*this); }
};

/**
 * @brief Creates a platform moving to the right first.
 * @param x Spawn x coordinate in pixels.
 * @param y Spawn y coordinate in pixels.
 */
[[nodiscard]] Platform makeHorizontalPlatform(int x, int y);

/**
 * @brief Creates a platform moving upwards first.
 * @param x Spawn x coordinate in pixels.
 * @param y Spawn y coordinate in pixels.
 */
[[nodiscard]] Platform makeVerticalPlatform(int x, int y);

}  // namespace mario
