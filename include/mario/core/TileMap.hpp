#pragma once

/**
 * @file TileMap.hpp
 * @brief Static part of a level: solid blocks and coins.
 */

#include <array>
#include <cstddef>

#include "mario/core/Config.hpp"

namespace mario {

/**
 * @brief Fixed-size grid storing solid tiles and coins.
 *
 * All accessors accept arbitrary coordinates. Coordinates outside the grid are treated as
 * empty air, which keeps collision code free of repeated bounds checks.
 */
class TileMap {
public:
    static constexpr int kWidth = config::kMapWidth;    ///< Number of columns.
    static constexpr int kHeight = config::kMapHeight;  ///< Number of rows.

    /// @return True if (tileX, tileY) lies inside the grid.
    [[nodiscard]] static constexpr bool inBounds(int tileX, int tileY) noexcept {
        return tileX >= 0 && tileX < kWidth && tileY >= 0 && tileY < kHeight;
    }

    /// @return True if the tile blocks movement.
    [[nodiscard]] bool isSolid(int tileX, int tileY) const noexcept;

    /// @brief Marks a tile as solid or empty. Out-of-range coordinates are ignored.
    void setSolid(int tileX, int tileY, bool solid) noexcept;

    /// @return True if the tile currently holds a coin.
    [[nodiscard]] bool hasCoin(int tileX, int tileY) const noexcept;

    /// @brief Places or removes a coin. Out-of-range coordinates are ignored.
    void setCoin(int tileX, int tileY, bool present) noexcept;

    /**
     * @brief Removes the coin at the given tile.
     * @return True if a coin was present and has been removed.
     */
    bool takeCoin(int tileX, int tileY) noexcept;

    /// @return Number of coins left in the grid.
    [[nodiscard]] int coinCount() const noexcept;

    /// @return Number of solid tiles in the grid.
    [[nodiscard]] int solidCount() const noexcept;

private:
    using Grid = std::array<std::array<bool, kWidth>, kHeight>;

    static std::size_t row(int tileY) noexcept { return static_cast<std::size_t>(tileY); }
    static std::size_t col(int tileX) noexcept { return static_cast<std::size_t>(tileX); }
    static int countSet(const Grid& grid) noexcept;

    Grid solid_{};
    Grid coins_{};
};

}  // namespace mario
