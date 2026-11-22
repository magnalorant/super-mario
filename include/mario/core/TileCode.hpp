#pragma once

/**
 * @file TileCode.hpp
 * @brief Characters used in level text files and their meaning.
 */

#include <optional>
#include <string_view>

namespace mario {

/**
 * @brief One cell of a level file.
 *
 * Level files are plain text with one character per tile. The enumerator values are the
 * characters written to disk, so a TileCode can be cast to char and back.
 */
enum class TileCode : char {
    Empty = '0',               ///< Air.
    Solid = '1',               ///< Wall or ground block.
    Coin = '2',                ///< Collectable coin.
    Enemy = '3',               ///< Walking enemy spawn point.
    Flag = '4',                ///< Goal flag that finishes the level.
    Mushroom = '5',            ///< Power-up granting an extra life and block breaking.
    Flower = '6',              ///< Power-up granting fireballs.
    HorizontalPlatform = '7',  ///< Platform moving left and right.
    VerticalPlatform = '8',    ///< Platform moving up and down.
};

/// @return The character stored in level files for the given code.
[[nodiscard]] constexpr char toChar(TileCode code) noexcept { return static_cast<char>(code); }

/**
 * @brief Converts a level file character into a tile code.
 * @param c Character read from a level file.
 * @return The matching code, or std::nullopt if the character has no meaning.
 */
[[nodiscard]] constexpr std::optional<TileCode> tileCodeFromChar(char c) noexcept {
    if (c >= '0' && c <= '8') {
        return static_cast<TileCode>(c);
    }
    return std::nullopt;
}

/// @return A short human readable name used by the editor palette.
[[nodiscard]] constexpr std::string_view tileName(TileCode code) noexcept {
    switch (code) {
        case TileCode::Empty:
            return "Empty";
        case TileCode::Solid:
            return "Wall";
        case TileCode::Coin:
            return "Coin";
        case TileCode::Enemy:
            return "Enemy";
        case TileCode::Flag:
            return "Flag";
        case TileCode::Mushroom:
            return "Mushroom";
        case TileCode::Flower:
            return "Flower";
        case TileCode::HorizontalPlatform:
            return "Horiz. Platform";
        case TileCode::VerticalPlatform:
            return "Vert. Platform";
    }
    return "Unknown";
}

}  // namespace mario
