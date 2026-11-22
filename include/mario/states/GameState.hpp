#pragma once

/**
 * @file GameState.hpp
 * @brief Base class of the screens the game can be in.
 */

#include <string_view>

#include "mario/core/Input.hpp"
#include "mario/core/Renderer.hpp"

namespace mario {

class Game;

/**
 * @brief State of the State pattern implemented by Game.
 *
 * Every screen (menu, level selection, gameplay, pause, game over, level complete, editor)
 * handles input, updates and draws itself. Game forwards calls to the active state, which
 * replaces the large switch statements of the original main loop. A state requests a
 * transition with Game::changeState(); the switch happens after the current call returns.
 */
class GameState {
public:
    GameState() = default;
    virtual ~GameState() = default;
    GameState(const GameState&) = delete;
    GameState& operator=(const GameState&) = delete;
    GameState(GameState&&) = delete;
    GameState& operator=(GameState&&) = delete;

    /// @return Short identifier used in logs and tests.
    [[nodiscard]] virtual std::string_view name() const noexcept = 0;

    /// @brief Reacts to a key or mouse press.
    virtual void handleEvent(Game& game, const InputEvent& event) = 0;

    /**
     * @brief Advances time-dependent logic.
     * @param game Context.
     * @param held Keys currently held.
     * @param elapsedMs Milliseconds since the previous update.
     */
    virtual void update(Game& game, const InputState& held, int elapsedMs);

    /// @brief Draws the screen.
    virtual void render(const Game& game, Renderer& renderer) const = 0;
};

}  // namespace mario
