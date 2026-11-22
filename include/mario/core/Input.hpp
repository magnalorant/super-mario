#pragma once

/**
 * @file Input.hpp
 * @brief Library independent description of user input.
 */

#include <optional>
#include <variant>

#include "mario/core/TileCode.hpp"

namespace mario {

/// @brief Keys the game reacts to.
enum class Key {
    Unknown,    ///< Any key without a binding.
    Up,         ///< Arrow up.
    Down,       ///< Arrow down.
    Left,       ///< Arrow left.
    Right,      ///< Arrow right.
    Enter,      ///< Return or keypad enter.
    Space,      ///< Space bar.
    Escape,     ///< Escape.
    P,          ///< Letter P.
    M,          ///< Letter M.
    S,          ///< Letter S.
    LeftShift,  ///< Left shift.
    Digit0,     ///< Number row 0.
    Digit1,     ///< Number row 1.
    Digit2,     ///< Number row 2.
    Digit3,     ///< Number row 3.
    Digit4,     ///< Number row 4.
    Digit5,     ///< Number row 5.
    Digit6,     ///< Number row 6.
    Digit7,     ///< Number row 7.
    Digit8,     ///< Number row 8.
};

/// @brief Mouse buttons the editor reacts to.
enum class MouseButton {
    Left,   ///< Primary button.
    Right,  ///< Secondary button.
    Other,  ///< Any other button.
};

/// @brief A key went down.
struct KeyPressed {
    Key key{Key::Unknown};  ///< The key.
};

/// @brief A mouse button went down.
struct MousePressed {
    int x{0};                              ///< Window x coordinate.
    int y{0};                              ///< Window y coordinate.
    MouseButton button{MouseButton::Left};  ///< The button.
};

/// @brief Discrete input event delivered to the active game state.
using InputEvent = std::variant<KeyPressed, MousePressed>;

/**
 * @brief Keys held down at the moment of the update.
 */
struct InputState {
    bool left{false};   ///< Arrow left is held.
    bool right{false};  ///< Arrow right is held.
    bool up{false};     ///< Arrow up is held.
    bool space{false};  ///< Space is held.
};

/**
 * @brief Maps a number key to the editor tile it selects.
 * @return The tile code, or std::nullopt for keys that are not digits 0 to 8.
 */
[[nodiscard]] constexpr std::optional<TileCode> tileForKey(Key key) noexcept {
    if (key < Key::Digit0 || key > Key::Digit8) {
        return std::nullopt;
    }
    const int offset = static_cast<int>(key) - static_cast<int>(Key::Digit0);
    return tileCodeFromChar(static_cast<char>('0' + offset));
}

/// @return True if the event is a press of the given key.
[[nodiscard]] inline bool isKey(const InputEvent& event, Key key) noexcept {
    const auto* pressed = std::get_if<KeyPressed>(&event);
    return pressed != nullptr && pressed->key == key;
}

/// @return True if the event is a press of either key.
[[nodiscard]] inline bool isAnyKey(const InputEvent& event, Key first, Key second) noexcept {
    return isKey(event, first) || isKey(event, second);
}

}  // namespace mario
