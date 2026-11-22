#pragma once

/**
 * @file Config.hpp
 * @brief Compile-time tuning constants shared by the game core and the SDL frontend.
 */

namespace mario::config {

inline constexpr int kWindowWidth = 800;   ///< Window width in pixels.
inline constexpr int kWindowHeight = 600;  ///< Window height in pixels.
inline constexpr int kTileSize = 40;       ///< Edge length of one map tile in pixels.
inline constexpr int kMapWidth = 141;      ///< Number of tile columns in a level.
inline constexpr int kMapHeight = 10;      ///< Number of tile rows in a level.

inline constexpr int kMaxEnemies = 64;    ///< Upper bound of enemies spawned from one level file.
inline constexpr int kMaxPowerups = 32;   ///< Upper bound of power-ups spawned from one level file.
inline constexpr int kMaxFireballs = 20;  ///< Number of fireballs that can be in flight at once.
inline constexpr int kMaxPlatforms = 16;  ///< Upper bound of moving platforms spawned from one level file.

inline constexpr int kPlayerSpeed = 5;              ///< Horizontal player speed in pixels per frame.
inline constexpr int kEnemySpeed = 2;               ///< Horizontal enemy speed in pixels per frame.
inline constexpr int kPlatformSpeed = 2;            ///< Moving platform speed in pixels per frame.
inline constexpr int kPlatformRange = kTileSize * 6;  ///< Travel distance of a moving platform in pixels.
inline constexpr int kFireballSpeed = 8;            ///< Fireball speed in pixels per frame.
inline constexpr int kFireballSize = 12;            ///< Fireball edge length in pixels.

inline constexpr float kGravity = 0.5F;        ///< Downward acceleration in pixels per frame squared.
inline constexpr float kJumpStrength = -12.0F;  ///< Initial vertical velocity of a jump.
inline constexpr float kStompBounceDivisor = 1.5F;  ///< Jump strength divisor applied after stomping an enemy.

inline constexpr int kPlayerInitialX = 100;           ///< Spawn x coordinate of the player.
inline constexpr int kPlayerInitialYOffsetTiles = 3;  ///< Spawn height measured in tiles from the bottom.
inline constexpr int kInitialLives = 3;               ///< Lives at the start of a level.
inline constexpr int kPointsPerCoin = 1;              ///< Score awarded for a coin.
inline constexpr int kPointsPerEnemyKill = 5;         ///< Score awarded for defeating an enemy.
inline constexpr int kMushroomLivesBonus = 1;         ///< Extra lives granted by a mushroom.
inline constexpr int kStompTolerance = 5;             ///< Extra pixels accepted when detecting a stomp.

inline constexpr int kLevelCompleteDelayMs = 2500;  ///< Duration of the level complete screen.
inline constexpr int kRespawnDelayMs = 500;         ///< Freeze time after losing a life.
inline constexpr int kFrameDelayMs = 16;            ///< Target frame time of the main loop.

inline constexpr int kPoleHeightTiles = 4;      ///< Height of the goal flag pole in tiles.
inline constexpr int kEditorCameraSpeed = 20;  ///< Editor camera scroll step in pixels.

inline constexpr int kWorldPixelWidth = kMapWidth * kTileSize;    ///< Level width in pixels.
inline constexpr int kWorldPixelHeight = kMapHeight * kTileSize;  ///< Level height in pixels.
inline constexpr int kRenderOffsetY = kWindowHeight - kWorldPixelHeight;  ///< Vertical offset of the level on screen.

}  // namespace mario::config
