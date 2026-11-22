#pragma once

/**
 * @file EntityFactory.hpp
 * @brief Factory turning level file characters into level content.
 */

#include <functional>
#include <unordered_map>

#include "mario/core/LevelData.hpp"
#include "mario/core/TileCode.hpp"

namespace mario {

/**
 * @brief Registry based factory that spawns level content for a tile code.
 *
 * Each tile code maps to a creator callback. The loader only knows how to read characters;
 * what a character produces is decided here, so new tile types are added by registering a
 * creator instead of extending a switch statement in the parser.
 */
class EntityFactory {
public:
    /**
     * @brief Callback that adds content for one tile.
     * @param level Level being built.
     * @param tileX Column of the tile.
     * @param tileY Row of the tile.
     */
    using Creator = std::function<void(LevelData& level, int tileX, int tileY)>;

    /// @brief Creates a factory with no registered creators.
    EntityFactory() = default;

    /// @return A factory configured with every tile code of the game.
    [[nodiscard]] static EntityFactory withDefaultCreators();

    /**
     * @brief Registers or replaces the creator for a tile code.
     * @param code Tile code to handle.
     * @param creator Callback invoked for each occurrence of the code.
     */
    void registerCreator(TileCode code, Creator creator);

    /// @return True if a creator is registered for the code.
    [[nodiscard]] bool canCreate(TileCode code) const;

    /**
     * @brief Spawns content for one tile.
     * @return False if the character is unknown or has no registered creator.
     */
    bool create(char symbol, LevelData& level, int tileX, int tileY) const;

private:
    std::unordered_map<TileCode, Creator> creators_;
};

}  // namespace mario
