#pragma once

/**
 * @file Entities.hpp
 * @brief Plain data types for the dynamic objects living in a level.
 */

#include <cstddef>
#include <optional>

#include "mario/core/Config.hpp"
#include "mario/core/Geometry.hpp"

namespace mario {

/// @brief Horizontal facing or travel direction.
enum class Direction : int {
    Left = -1,  ///< Towards smaller x.
    Right = 1,  ///< Towards larger x.
};

/// @return +1 for Right and -1 for Left.
[[nodiscard]] constexpr int sign(Direction d) noexcept { return static_cast<int>(d); }

/// @return The opposite direction.
[[nodiscard]] constexpr Direction reversed(Direction d) noexcept {
    return d == Direction::Left ? Direction::Right : Direction::Left;
}

/**
 * @brief State of the player character.
 */
struct Player {
    int x{0};                     ///< Left edge in world pixels.
    int y{0};                     ///< Top edge in world pixels.
    int w{config::kTileSize};     ///< Width in pixels.
    int h{config::kTileSize};     ///< Height in pixels.
    float velocityY{0.0F};        ///< Vertical velocity, positive is downwards.
    int speed{config::kPlayerSpeed};  ///< Horizontal speed in pixels per frame.
    int lives{config::kInitialLives};  ///< Remaining lives.
    int score{0};                 ///< Collected points.
    bool hasMushroom{false};      ///< Can break blocks from below.
    bool hasFlower{false};        ///< Can shoot fireballs.
    bool onGround{false};         ///< Standing on a solid tile.
    Direction facing{Direction::Right};  ///< Direction used for fireballs.
    std::optional<std::size_t> supportingPlatform;  ///< Index of the platform carrying the player.

    /// @return Collision rectangle in world coordinates.
    [[nodiscard]] Rect bounds() const noexcept { return {x, y, w, h}; }

    /// @return True while the player rides a moving platform.
    [[nodiscard]] bool onPlatform() const noexcept { return supportingPlatform.has_value(); }

    /// @brief Moves the player back to the spawn point and clears power-ups and momentum.
    void respawn() noexcept;

    /// @brief Restores the full initial state used at the start of a level.
    void resetForNewLevel() noexcept;
};

/**
 * @brief Walking enemy that turns around at walls and ledges.
 */
struct Enemy {
    int x{0};                           ///< Left edge in world pixels.
    int y{0};                           ///< Top edge in world pixels.
    int w{config::kTileSize};           ///< Width in pixels.
    int h{config::kTileSize};           ///< Height in pixels.
    Direction direction{Direction::Right};  ///< Current walking direction.
    bool alive{true};                   ///< False after being defeated.

    /// @return Collision rectangle in world coordinates.
    [[nodiscard]] Rect bounds() const noexcept { return {x, y, w, h}; }
};

/// @brief Kind of a collectable power-up.
enum class PowerupKind {
    Mushroom,  ///< Extra life and block breaking.
    Flower,    ///< Fireball shooting.
};

/**
 * @brief Collectable power-up occupying one tile.
 */
struct Powerup {
    int x{0};                              ///< Left edge in world pixels.
    int y{0};                              ///< Top edge in world pixels.
    PowerupKind kind{PowerupKind::Mushroom};  ///< Effect applied on pickup.
    bool active{true};                     ///< False after being collected.

    /// @return Collision rectangle in world coordinates.
    [[nodiscard]] Rect bounds() const noexcept { return {x, y, config::kTileSize, config::kTileSize}; }
};

/**
 * @brief Projectile shot by a player holding a flower.
 */
struct Fireball {
    int x{0};                           ///< Left edge in world pixels.
    int y{0};                           ///< Top edge in world pixels.
    Direction direction{Direction::Right};  ///< Flight direction.
    bool active{false};                 ///< True while in flight.

    /// @return Collision rectangle in world coordinates.
    [[nodiscard]] Rect bounds() const noexcept {
        return {x, y, config::kFireballSize, config::kFireballSize};
    }
};

/**
 * @brief Goal of a level. Touching it completes the level.
 */
struct Flag {
    int x{0};                  ///< Left edge in world pixels.
    int y{0};                  ///< Top edge in world pixels.
    int w{config::kTileSize};  ///< Width in pixels.
    int h{config::kTileSize};  ///< Height in pixels.

    /// @return Collision rectangle in world coordinates.
    [[nodiscard]] Rect bounds() const noexcept { return {x, y, w, h}; }
};

}  // namespace mario
