#include "mario/core/TileMap.hpp"

#include <algorithm>

namespace mario {

bool TileMap::isSolid(int tileX, int tileY) const noexcept {
    return inBounds(tileX, tileY) && solid_[row(tileY)][col(tileX)];
}

void TileMap::setSolid(int tileX, int tileY, bool solid) noexcept {
    if (inBounds(tileX, tileY)) {
        solid_[row(tileY)][col(tileX)] = solid;
    }
}

bool TileMap::hasCoin(int tileX, int tileY) const noexcept {
    return inBounds(tileX, tileY) && coins_[row(tileY)][col(tileX)];
}

void TileMap::setCoin(int tileX, int tileY, bool present) noexcept {
    if (inBounds(tileX, tileY)) {
        coins_[row(tileY)][col(tileX)] = present;
    }
}

bool TileMap::takeCoin(int tileX, int tileY) noexcept {
    if (!hasCoin(tileX, tileY)) {
        return false;
    }
    coins_[row(tileY)][col(tileX)] = false;
    return true;
}

int TileMap::coinCount() const noexcept {
    return countSet(coins_);
}

int TileMap::solidCount() const noexcept {
    return countSet(solid_);
}

int TileMap::countSet(const Grid& grid) noexcept {
    int total = 0;
    for (const auto& line : grid) {
        total += static_cast<int>(std::count(line.begin(), line.end(), true));
    }
    return total;
}

}  // namespace mario
