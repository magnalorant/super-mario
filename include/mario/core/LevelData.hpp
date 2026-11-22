#pragma once

/**
 * @file LevelData.hpp
 * @brief Everything parsed from a level file, before the simulation starts.
 */

#include <optional>
#include <vector>

#include "mario/core/Entities.hpp"
#include "mario/core/Platform.hpp"
#include "mario/core/TileMap.hpp"

namespace mario {

/**
 * @brief Initial content of a level.
 */
struct LevelData {
    TileMap tiles;                   ///< Solid blocks and coins.
    std::vector<Enemy> enemies;      ///< Enemy spawn states.
    std::vector<Powerup> powerups;   ///< Power-ups waiting to be collected.
    std::vector<Platform> platforms;  ///< Moving platforms.
    std::optional<Flag> flag;        ///< Goal of the level, if any.
};

}  // namespace mario
