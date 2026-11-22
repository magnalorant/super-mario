#pragma once

/**
 * @file EditorModel.hpp
 * @brief Data and editing operations of the in-game level editor.
 */

#include <array>
#include <cstddef>
#include <filesystem>
#include <iosfwd>

#include "mario/core/TileCode.hpp"
#include "mario/core/TileMap.hpp"

namespace mario {

/**
 * @brief Character grid edited by the level editor.
 *
 * The editor works on raw level characters instead of a parsed LevelData, so saving writes
 * exactly what the user placed. Screen coordinates are converted to tiles using the editor
 * camera, which scrolls horizontally.
 */
class EditorModel {
public:
    static constexpr int kWidth = TileMap::kWidth;    ///< Number of columns.
    static constexpr int kHeight = TileMap::kHeight;  ///< Number of rows.

    /// @brief Creates an empty grid with walls selected.
    EditorModel();

    /// @brief Clears the grid, resets the camera and selects walls.
    void reset();

    /**
     * @brief Replaces the grid with level text. Missing cells become empty.
     * @param input Stream in the level file format.
     */
    void load(std::istream& input);

    /**
     * @brief Resets the editor and loads a level file if it exists.
     * @return False if the file could not be opened; the grid is then empty.
     */
    bool loadFile(const std::filesystem::path& path);

    /// @brief Writes the grid in the level file format.
    void save(std::ostream& output) const;

    /**
     * @brief Writes the grid to disk.
     * @return False if the file could not be written.
     */
    [[nodiscard]] bool saveFile(const std::filesystem::path& path) const;

    /// @return Character at a tile, or TileCode::Empty outside the grid.
    [[nodiscard]] char tileAt(int tileX, int tileY) const noexcept;

    /// @brief Sets a tile character. Out-of-range coordinates are ignored.
    void setTile(int tileX, int tileY, char symbol) noexcept;

    /// @brief Chooses the tile placed by paint().
    void select(TileCode code) noexcept { selected_ = code; }

    /// @return The tile placed by paint().
    [[nodiscard]] TileCode selected() const noexcept { return selected_; }

    /**
     * @brief Places the selected tile under a screen position.
     * @return True if the position maps to a tile of the grid.
     */
    bool paint(int screenX, int screenY) noexcept;

    /**
     * @brief Clears the tile under a screen position.
     * @return True if the position maps to a tile of the grid.
     */
    bool erase(int screenX, int screenY) noexcept;

    /// @brief Scrolls the camera by dx pixels, clamped to the level.
    void scroll(int dx) noexcept;

    /// @return Horizontal camera offset in pixels.
    [[nodiscard]] int cameraX() const noexcept { return cameraX_; }

private:
    using Grid = std::array<std::array<char, kWidth>, kHeight>;

    bool setAtScreen(int screenX, int screenY, char symbol) noexcept;

    Grid grid_{};
    TileCode selected_{TileCode::Solid};
    int cameraX_{0};
};

}  // namespace mario
