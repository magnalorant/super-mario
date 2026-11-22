#pragma once

/**
 * @file SdlInput.hpp
 * @brief Translation of SDL input into the core input types.
 */

#include <SDL.h>

#include <optional>

#include "mario/core/Input.hpp"

namespace mario::app {

/// @return The core key bound to an SDL key code, or Key::Unknown.
[[nodiscard]] Key translateKey(SDL_Keycode code) noexcept;

/**
 * @brief Converts an SDL event into a core input event.
 * @return The event, or std::nullopt for events the game does not handle.
 */
[[nodiscard]] std::optional<InputEvent> translateEvent(const SDL_Event& event) noexcept;

/// @return The movement keys currently held down.
[[nodiscard]] InputState readInputState() noexcept;

}  // namespace mario::app
