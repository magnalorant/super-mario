#include "mario/core/EditorModel.hpp"

#include <algorithm>
#include <fstream>
#include <istream>
#include <ostream>
#include <string>

#include "mario/core/Camera.hpp"
#include "mario/core/Config.hpp"

namespace mario {

namespace {

constexpr std::size_t index(int value) noexcept { return static_cast<std::size_t>(value); }

}  // namespace

EditorModel::EditorModel() { reset(); }

void EditorModel::reset() {
    for (auto& row : grid_) {
        row.fill(toChar(TileCode::Empty));
    }
    selected_ = TileCode::Solid;
    cameraX_ = 0;
}

void EditorModel::load(std::istream& input) {
    for (auto& row : grid_) {
        row.fill(toChar(TileCode::Empty));
    }
    std::string line;
    for (int y = 0; y < kHeight && std::getline(input, line); ++y) {
        const int columns = std::min(static_cast<int>(line.size()), kWidth);
        for (int x = 0; x < columns; ++x) {
            const char c = line[index(x)];
            if (c != '\r' && c != '\n') {
                grid_[index(y)][index(x)] = c;
            }
        }
    }
}

bool EditorModel::loadFile(const std::filesystem::path& path) {
    reset();
    std::ifstream input(path);
    if (!input) {
        return false;
    }
    load(input);
    return true;
}

void EditorModel::save(std::ostream& output) const {
    for (const auto& row : grid_) {
        output.write(row.data(), static_cast<std::streamsize>(row.size()));
        output.put('\n');
    }
}

bool EditorModel::saveFile(const std::filesystem::path& path) const {
    std::ofstream output(path, std::ios::trunc);
    if (!output) {
        return false;
    }
    save(output);
    return static_cast<bool>(output);
}

char EditorModel::tileAt(int tileX, int tileY) const noexcept {
    if (!TileMap::inBounds(tileX, tileY)) {
        return toChar(TileCode::Empty);
    }
    return grid_[index(tileY)][index(tileX)];
}

void EditorModel::setTile(int tileX, int tileY, char symbol) noexcept {
    if (TileMap::inBounds(tileX, tileY)) {
        grid_[index(tileY)][index(tileX)] = symbol;
    }
}

bool EditorModel::paint(int screenX, int screenY) noexcept {
    return setAtScreen(screenX, screenY, toChar(selected_));
}

bool EditorModel::erase(int screenX, int screenY) noexcept {
    return setAtScreen(screenX, screenY, toChar(TileCode::Empty));
}

void EditorModel::scroll(int dx) noexcept { cameraX_ = std::clamp(cameraX_ + dx, 0, Camera::maxOffset()); }

bool EditorModel::setAtScreen(int screenX, int screenY, char symbol) noexcept {
    if (screenX < 0 || screenY < 0) {
        return false;
    }
    const int tileX = (screenX + cameraX_) / config::kTileSize;
    const int tileY = screenY / config::kTileSize;
    if (!TileMap::inBounds(tileX, tileY)) {
        return false;
    }
    grid_[index(tileY)][index(tileX)] = symbol;
    return true;
}

}  // namespace mario
