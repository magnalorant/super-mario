#pragma once

/**
 * @file Game.hpp
 * @brief Top level game object owning the simulation and the active screen.
 */

#include <cstddef>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "mario/core/EditorModel.hpp"
#include "mario/core/EventDispatcher.hpp"
#include "mario/core/Input.hpp"
#include "mario/core/LevelLoader.hpp"
#include "mario/core/Renderer.hpp"
#include "mario/core/World.hpp"
#include "mario/states/GameState.hpp"

namespace mario {

/**
 * @brief A playable level shown in the level selection.
 */
struct LevelEntry {
    std::string name;             ///< Display name, the file name.
    std::filesystem::path path;   ///< Location of the level file.
};

/**
 * @brief Context of the State pattern and owner of all game data.
 *
 * Game does not know about SDL. The frontend feeds it InputEvent and InputState values,
 * calls update() once per frame and lets it draw through a Renderer.
 */
class Game {
public:
    /// @brief File name of the level written by the editor.
    static constexpr const char* kCustomLevelName = "custom_map.txt";

    /**
     * @brief Creates a game that reads levels from assetDir/levels.
     * @param assetDir Directory containing the levels folder.
     */
    explicit Game(const std::filesystem::path& assetDir);

    ~Game();
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    Game(Game&&) = delete;
    Game& operator=(Game&&) = delete;

    /// @brief Forwards an input event to the active state.
    void handleEvent(const InputEvent& event);

    /**
     * @brief Advances the active state.
     * @param held Keys currently held.
     * @param elapsedMs Milliseconds since the previous update.
     */
    void update(const InputState& held, int elapsedMs);

    /// @brief Draws the active state.
    void render(Renderer& renderer) const;

    /// @brief Schedules a state change, applied after the current call into the state returns.
    void changeState(std::unique_ptr<GameState> next);

    /// @return The active state.
    [[nodiscard]] const GameState& state() const noexcept { return *state_; }

    /// @return False once the player chose to quit.
    [[nodiscard]] bool running() const noexcept { return running_; }

    /// @brief Ends the main loop.
    void requestQuit() noexcept { running_ = false; }

    /**
     * @brief Loads a level from the level list and resets the player.
     * @param index Position in levels().
     * @return False if the level could not be loaded; see lastError().
     */
    bool startLevel(std::size_t index);

    /// @brief Loads the custom level into the editor.
    void openEditor();

    /**
     * @brief Writes the editor content to the custom level file.
     * @return False if writing failed.
     */
    [[nodiscard]] bool saveEditorLevel() const;

    /// @return The playable levels in display order.
    [[nodiscard]] const std::vector<LevelEntry>& levels() const noexcept { return levels_; }

    /// @return Message describing the last failed operation.
    [[nodiscard]] const std::string& lastError() const noexcept { return lastError_; }

    /// @return Location of the level written by the editor.
    [[nodiscard]] std::filesystem::path customLevelPath() const;

    /// @name Owned subsystems
    /// @{
    [[nodiscard]] World& world() noexcept { return world_; }
    [[nodiscard]] const World& world() const noexcept { return world_; }
    [[nodiscard]] EditorModel& editor() noexcept { return editor_; }
    [[nodiscard]] const EditorModel& editor() const noexcept { return editor_; }
    [[nodiscard]] EventDispatcher& events() noexcept { return events_; }
    /// @}

private:
    void applyPendingState();

    std::filesystem::path levelDir_;
    EventDispatcher events_;
    World world_;
    EditorModel editor_;
    LevelLoader loader_;
    std::vector<LevelEntry> levels_;
    std::unique_ptr<GameState> state_;
    std::unique_ptr<GameState> pending_;
    std::string lastError_;
    bool running_{true};
};

}  // namespace mario
