#pragma once

/**
 * @file LevelLoader.hpp
 * @brief Parser for the plain text level format.
 */

#include <filesystem>
#include <iosfwd>
#include <stdexcept>
#include <string>

#include "mario/core/EntityFactory.hpp"
#include "mario/core/LevelData.hpp"

namespace mario {

/// @brief Raised when a level file cannot be opened.
class LevelLoadError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

/**
 * @brief Reads levels written as TileMap::kHeight lines of TileMap::kWidth characters.
 *
 * Shorter lines and missing lines are treated as empty air, longer lines are truncated and
 * characters without a meaning are ignored, so hand-edited files never crash the game.
 * Windows line endings are accepted.
 */
class LevelLoader {
public:
    /// @brief Creates a loader using the default entity factory.
    LevelLoader();

    /// @brief Creates a loader that delegates tile creation to the given factory.
    explicit LevelLoader(EntityFactory factory);

    /**
     * @brief Parses a level from a stream.
     * @param input Stream positioned at the first row.
     */
    [[nodiscard]] LevelData parse(std::istream& input) const;

    /**
     * @brief Parses a level from a string.
     * @param text Complete level text.
     */
    [[nodiscard]] LevelData parseString(const std::string& text) const;

    /**
     * @brief Loads a level from disk.
     * @param path File to read.
     * @throws LevelLoadError if the file cannot be opened.
     */
    [[nodiscard]] LevelData loadFile(const std::filesystem::path& path) const;

private:
    EntityFactory factory_;
};

}  // namespace mario
