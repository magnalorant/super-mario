#pragma once

#include <string>
#include <vector>

#include "mario/core/LevelLoader.hpp"
#include "mario/core/TileMap.hpp"

namespace mario::test {

class LevelBuilder {
public:
    LevelBuilder() : rows_(static_cast<std::size_t>(TileMap::kHeight), std::string(TileMap::kWidth, '0')) {}

    LevelBuilder& ground() {
        fillRow(TileMap::kHeight - 2, '1');
        fillRow(TileMap::kHeight - 1, '1');
        return *this;
    }

    LevelBuilder& fillRow(int ty, char symbol) {
        rows_[static_cast<std::size_t>(ty)].assign(TileMap::kWidth, symbol);
        return *this;
    }

    LevelBuilder& put(int tx, int ty, char symbol) {
        rows_[static_cast<std::size_t>(ty)][static_cast<std::size_t>(tx)] = symbol;
        return *this;
    }

    LevelBuilder& pit(int fromX, int toX) {
        for (int tx = fromX; tx <= toX; ++tx) {
            put(tx, TileMap::kHeight - 2, '0');
            put(tx, TileMap::kHeight - 1, '0');
        }
        return *this;
    }

    [[nodiscard]] std::string text() const {
        std::string out;
        for (const auto& row : rows_) {
            out += row;
            out += '\n';
        }
        return out;
    }

    [[nodiscard]] LevelData build() const { return LevelLoader().parseString(text()); }

private:
    std::vector<std::string> rows_;
};

}  // namespace mario::test
