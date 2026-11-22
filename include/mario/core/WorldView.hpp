#pragma once

/**
 * @file WorldView.hpp
 * @brief Draws a World and its heads-up display through the Renderer interface.
 */

#include <string>

#include "mario/core/Renderer.hpp"
#include "mario/core/World.hpp"

namespace mario {

/// @brief Point size of the heads-up display text.
inline constexpr int kHudPointSize = 20;

/**
 * @brief Formats the heads-up display line.
 * @return Text such as "Score: 12   Lives: 3".
 */
[[nodiscard]] std::string hudText(const Player& player);

/**
 * @brief Draws the level, all entities and the heads-up display.
 *
 * Objects outside the visible part of the level are skipped.
 */
void renderWorld(const World& world, Renderer& renderer);

/**
 * @brief Darkens the whole screen.
 * @param renderer Target.
 * @param alpha Opacity of the black overlay.
 */
void renderDimOverlay(Renderer& renderer, std::uint8_t alpha);

}  // namespace mario
