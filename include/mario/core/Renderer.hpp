#pragma once

/**
 * @file Renderer.hpp
 * @brief Drawing interface the game core renders through.
 */

#include <cstdint>
#include <string_view>

#include "mario/core/Geometry.hpp"

namespace mario {

/// @brief RGBA color.
struct Color {
    std::uint8_t r{0};    ///< Red channel.
    std::uint8_t g{0};    ///< Green channel.
    std::uint8_t b{0};    ///< Blue channel.
    std::uint8_t a{255};  ///< Alpha channel, 255 is opaque.
};

/// @brief Colors used by the user interface.
namespace palette {
inline constexpr Color kWhite{255, 255, 255, 255};          ///< Regular text.
inline constexpr Color kGold{255, 220, 40, 255};            ///< Highlighted menu entries.
inline constexpr Color kYellow{255, 255, 0, 255};           ///< Flag and editor highlights.
inline constexpr Color kRed{220, 50, 50, 255};              ///< Game over title and errors.
inline constexpr Color kShadow{40, 40, 40, 255};            ///< Default text shadow.
inline constexpr Color kDeepShadow{30, 30, 30, 255};        ///< Game over text shadow.
inline constexpr Color kBlueShadow{30, 30, 70, 255};        ///< Level select text shadow.
inline constexpr Color kSky{107, 140, 255, 255};            ///< Game background.
inline constexpr Color kEditorBackground{30, 30, 30, 255};  ///< Editor background.
inline constexpr Color kEditorEmpty{80, 80, 80, 255};       ///< Empty editor cell.
inline constexpr Color kEditorGrid{120, 120, 120, 255};     ///< Editor grid lines.
inline constexpr Color kFlagPole{180, 180, 180, 255};       ///< Goal flag pole.
}  // namespace palette

/// @brief Images the renderer knows how to draw.
enum class Sprite {
    Ground,          ///< Bottom ground blocks.
    Brick,           ///< Floating blocks.
    Coin,            ///< Coin.
    Enemy,           ///< Walking enemy.
    Fireball,        ///< Fireball.
    Mushroom,        ///< Mushroom power-up.
    Flower,          ///< Flower power-up.
    Player,          ///< Player character.
    MovingPlatform,  ///< Moving platform.
};

/**
 * @brief Abstract drawing target.
 *
 * Game states and views draw through this interface only. The SDL frontend provides an
 * adapter implementation, while tests use a recording fake, so presentation logic can be
 * verified without opening a window. Blending with the alpha channel is always enabled.
 */
class Renderer {
public:
    Renderer() = default;
    virtual ~Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    /// @brief Fills the whole target with a color.
    virtual void clear(Color color) = 0;

    /// @brief Fills a rectangle given in screen coordinates.
    virtual void fillRect(const Rect& area, Color color) = 0;

    /// @brief Draws the outline of a rectangle given in screen coordinates.
    virtual void outlineRect(const Rect& area, Color color) = 0;

    /// @brief Draws a sprite stretched to a rectangle given in screen coordinates.
    virtual void drawSprite(Sprite sprite, const Rect& area) = 0;

    /**
     * @brief Draws text with its top left corner at (x, y).
     * @param text UTF-8 text.
     * @param x Left edge.
     * @param y Top edge.
     * @param color Text color.
     * @param pointSize Font size in points.
     */
    virtual void drawText(std::string_view text, int x, int y, Color color, int pointSize) = 0;

    /**
     * @brief Draws horizontally centered text with a drop shadow.
     * @param text UTF-8 text.
     * @param y Top edge.
     * @param color Text color.
     * @param shadow Shadow color.
     * @param pointSize Font size in points.
     * @param shadowOffset Shadow displacement in pixels on both axes.
     */
    virtual void drawTextCentered(std::string_view text, int y, Color color, Color shadow, int pointSize,
                                  int shadowOffset) = 0;
};

}  // namespace mario
