#pragma once

/**
 * @file States.hpp
 * @brief Concrete game screens.
 */

#include <array>
#include <cstddef>
#include <string>
#include <string_view>

#include "mario/states/GameState.hpp"

namespace mario {

/**
 * @brief Title screen with the main menu.
 */
class MenuState final : public GameState {
public:
    /// @brief Menu entries in display order.
    static constexpr std::array<std::string_view, 3> kOptions{"New Game", "Level Editor", "Quit"};

    [[nodiscard]] std::string_view name() const noexcept override { return "Menu"; }
    void handleEvent(Game& game, const InputEvent& event) override;
    void render(const Game& game, Renderer& renderer) const override;

    /// @return Index of the highlighted entry.
    [[nodiscard]] std::size_t selected() const noexcept { return selected_; }

private:
    std::size_t selected_{0};
};

/**
 * @brief Lets the player choose which level file to play.
 */
class LevelSelectState final : public GameState {
public:
    [[nodiscard]] std::string_view name() const noexcept override { return "LevelSelect"; }
    void handleEvent(Game& game, const InputEvent& event) override;
    void render(const Game& game, Renderer& renderer) const override;

    /// @return Index of the highlighted level.
    [[nodiscard]] std::size_t selected() const noexcept { return selected_; }

private:
    std::size_t selected_{0};
    std::string error_;
};

/**
 * @brief Active gameplay.
 */
class PlayingState final : public GameState {
public:
    [[nodiscard]] std::string_view name() const noexcept override { return "Playing"; }
    void handleEvent(Game& game, const InputEvent& event) override;
    void update(Game& game, const InputState& held, int elapsedMs) override;
    void render(const Game& game, Renderer& renderer) const override;

    /// @return Remaining freeze time after losing a life.
    [[nodiscard]] int freezeMs() const noexcept { return freezeMs_; }

private:
    bool jumpQueued_{false};
    int freezeMs_{0};
};

/**
 * @brief Gameplay is suspended and an overlay is shown.
 */
class PausedState final : public GameState {
public:
    [[nodiscard]] std::string_view name() const noexcept override { return "Paused"; }
    void handleEvent(Game& game, const InputEvent& event) override;
    void render(const Game& game, Renderer& renderer) const override;
};

/**
 * @brief Shown after the last life is lost.
 */
class GameOverState final : public GameState {
public:
    [[nodiscard]] std::string_view name() const noexcept override { return "GameOver"; }
    void handleEvent(Game& game, const InputEvent& event) override;
    void render(const Game& game, Renderer& renderer) const override;
};

/**
 * @brief Congratulation screen shown for a fixed time after reaching the flag.
 */
class LevelCompleteState final : public GameState {
public:
    [[nodiscard]] std::string_view name() const noexcept override { return "LevelComplete"; }
    void handleEvent(Game& game, const InputEvent& event) override;
    void update(Game& game, const InputState& held, int elapsedMs) override;
    void render(const Game& game, Renderer& renderer) const override;

private:
    int elapsedMs_{0};
};

/**
 * @brief Tile based level editor writing the custom level file.
 */
class EditorState final : public GameState {
public:
    [[nodiscard]] std::string_view name() const noexcept override { return "Editor"; }
    void handleEvent(Game& game, const InputEvent& event) override;
    void render(const Game& game, Renderer& renderer) const override;

    /// @return Feedback of the last save attempt, empty before the first one.
    [[nodiscard]] const std::string& status() const noexcept { return status_; }

private:
    std::string status_;
};

}  // namespace mario
