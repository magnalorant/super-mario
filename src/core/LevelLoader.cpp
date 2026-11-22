#include "mario/core/LevelLoader.hpp"

#include <algorithm>
#include <fstream>
#include <istream>
#include <sstream>
#include <utility>

namespace mario {

LevelLoader::LevelLoader() : factory_(EntityFactory::withDefaultCreators()) {}

LevelLoader::LevelLoader(EntityFactory factory) : factory_(std::move(factory)) {}

LevelData LevelLoader::parse(std::istream& input) const {
    LevelData level;
    std::string line;
    for (int y = 0; y < TileMap::kHeight && std::getline(input, line); ++y) {
        const int columns = std::min(static_cast<int>(line.size()), TileMap::kWidth);
        for (int x = 0; x < columns; ++x) {
            factory_.create(line[static_cast<std::size_t>(x)], level, x, y);
        }
    }
    return level;
}

LevelData LevelLoader::parseString(const std::string& text) const {
    std::istringstream input(text);
    return parse(input);
}

LevelData LevelLoader::loadFile(const std::filesystem::path& path) const {
    std::ifstream input(path);
    if (!input) {
        throw LevelLoadError("Cannot open level file: " + path.string());
    }
    return parse(input);
}

}  // namespace mario
