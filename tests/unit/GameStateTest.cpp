#include <gtest/gtest.h>

#include <filesystem>
#include <memory>

#include "mario/states/Game.hpp"
#include "mario/states/States.hpp"
#include "support/FakeRenderer.hpp"
#include "support/LevelBuilder.hpp"
#include "support/TempAssetDir.hpp"

namespace mario {
namespace {

constexpr int kFrameMs = config::kFrameDelayMs;
constexpr std::size_t kMap1 = 0;
constexpr std::size_t kCustom = 3;

class GameStateTest : public ::testing::Test {
protected:
    void press(Key key) { game.handleEvent(KeyPressed{key}); }

    void click(int x, int y, MouseButton button) { game.handleEvent(MousePressed{x, y, button}); }

    void frames(int count, const InputState& held = {}) {
        for (int i = 0; i < count; ++i) {
            game.update(held, kFrameMs);
        }
    }

    void openLevelSelect() {
        press(Key::Enter);
        ASSERT_EQ(game.state().name(), "LevelSelect");
    }

    void selectLevel(std::size_t index) {
        openLevelSelect();
        for (std::size_t i = 0; i < index; ++i) {
            press(Key::Down);
        }
        press(Key::Enter);
    }

    test::TempAssetDir assets;
    Game game{assets.path()};
    test::FakeRenderer renderer;
};

TEST_F(GameStateTest, StartsInTheMenu) {
    EXPECT_EQ(game.state().name(), "Menu");
    EXPECT_TRUE(game.running());
    EXPECT_EQ(game.levels().size(), 4U);
    EXPECT_EQ(game.levels().back().name, Game::kCustomLevelName);
}

TEST_F(GameStateTest, MenuSelectionWrapsAround) {
    const auto& menu = dynamic_cast<const MenuState&>(game.state());
    press(Key::Up);
    EXPECT_EQ(menu.selected(), MenuState::kOptions.size() - 1);
    press(Key::Down);
    EXPECT_EQ(menu.selected(), 0U);
}

TEST_F(GameStateTest, QuitMenuEntryStopsTheGame) {
    press(Key::Up);
    press(Key::Enter);
    EXPECT_FALSE(game.running());
}

TEST_F(GameStateTest, MenuRendersAllOptions) {
    game.render(renderer);
    for (const auto option : MenuState::kOptions) {
        EXPECT_TRUE(renderer.hasText(option)) << option;
    }
    EXPECT_GT(renderer.spriteCount(Sprite::Ground), 0);
}

TEST_F(GameStateTest, EscapeLeavesLevelSelect) {
    openLevelSelect();
    press(Key::Escape);
    EXPECT_EQ(game.state().name(), "Menu");
}

TEST_F(GameStateTest, LevelSelectListsLevelFiles) {
    openLevelSelect();
    game.render(renderer);
    EXPECT_TRUE(renderer.hasText("> map1.txt"));
    EXPECT_TRUE(renderer.hasText("custom_map.txt"));
}

TEST_F(GameStateTest, MissingLevelShowsAnErrorInsteadOfExiting) {
    selectLevel(kMap1);
    EXPECT_EQ(game.state().name(), "LevelSelect");
    EXPECT_FALSE(game.lastError().empty());
    game.render(renderer);
    EXPECT_TRUE(renderer.hasText("map1.txt"));
    EXPECT_TRUE(renderer.hasText("Cannot open level file"));
}

TEST_F(GameStateTest, MissingCustomLevelStartsAnEmptyLevel) {
    selectLevel(kCustom);
    EXPECT_EQ(game.state().name(), "Playing");
    EXPECT_EQ(game.world().tiles().solidCount(), 0);
}

TEST_F(GameStateTest, ExistingLevelIsLoaded) {
    assets.writeLevel("map1.txt", test::LevelBuilder().ground().text());
    selectLevel(kMap1);
    ASSERT_EQ(game.state().name(), "Playing");
    EXPECT_EQ(game.world().tiles().solidCount(), 2 * TileMap::kWidth);
    game.render(renderer);
    EXPECT_TRUE(renderer.hasText("Score: 0   Lives: 3"));
}

TEST_F(GameStateTest, PauseAndResume) {
    assets.writeLevel("map1.txt", test::LevelBuilder().ground().text());
    selectLevel(kMap1);
    press(Key::P);
    EXPECT_EQ(game.state().name(), "Paused");

    const int y = game.world().player().y;
    frames(30);
    EXPECT_EQ(game.world().player().y, y);

    press(Key::Escape);
    EXPECT_EQ(game.state().name(), "Playing");
    press(Key::P);
    press(Key::M);
    EXPECT_EQ(game.state().name(), "Menu");
}

TEST_F(GameStateTest, JumpKeyMakesThePlayerJump) {
    assets.writeLevel("map1.txt", test::LevelBuilder().ground().text());
    selectLevel(kMap1);
    frames(60);
    const int groundY = game.world().player().y;
    press(Key::Space);
    frames(1, InputState{false, false, false, true});
    EXPECT_LT(game.world().player().y, groundY);
}

TEST_F(GameStateTest, LosingAllLivesLeadsToGameOverAndBackToMenu) {
    selectLevel(kCustom);
    for (int i = 0; i < 2000 && game.state().name() == "Playing"; ++i) {
        frames(1);
    }
    EXPECT_EQ(game.state().name(), "GameOver");
    game.render(renderer);
    EXPECT_TRUE(renderer.hasText("GAME OVER"));
    press(Key::Enter);
    EXPECT_EQ(game.state().name(), "Menu");
}

TEST_F(GameStateTest, RespawnFreezesTheGameBriefly) {
    selectLevel(kCustom);
    for (int i = 0; i < 500 && game.world().player().lives == config::kInitialLives; ++i) {
        frames(1);
    }
    const auto& playing = dynamic_cast<const PlayingState&>(game.state());
    EXPECT_GT(playing.freezeMs(), 0);
    const int y = game.world().player().y;
    frames(config::kRespawnDelayMs / kFrameMs - 1);
    EXPECT_EQ(game.world().player().y, y);
}

TEST_F(GameStateTest, ReachingTheFlagShowsCongratulationsThenMenu) {
    assets.writeLevel("map1.txt", test::LevelBuilder().ground().put(4, 7, '4').text());
    selectLevel(kMap1);
    for (int i = 0; i < 60 && game.state().name() == "Playing"; ++i) {
        frames(1, InputState{false, true, false, false});
    }
    ASSERT_EQ(game.state().name(), "LevelComplete");
    game.render(renderer);
    EXPECT_TRUE(renderer.hasText("Congratulations!"));

    frames(config::kLevelCompleteDelayMs / kFrameMs - 1);
    EXPECT_EQ(game.state().name(), "LevelComplete");
    frames(2);
    EXPECT_EQ(game.state().name(), "Menu");
}

TEST_F(GameStateTest, EditorPaintsSavesAndReturnsToMenu) {
    press(Key::Down);
    press(Key::Enter);
    ASSERT_EQ(game.state().name(), "Editor");

    press(Key::Digit3);
    EXPECT_EQ(game.editor().selected(), TileCode::Enemy);
    click(5, 5, MouseButton::Left);
    EXPECT_EQ(game.editor().tileAt(0, 0), '3');
    click(45, 5, MouseButton::Left);
    click(45, 5, MouseButton::Right);
    EXPECT_EQ(game.editor().tileAt(1, 0), '0');

    press(Key::Right);
    EXPECT_EQ(game.editor().cameraX(), config::kEditorCameraSpeed);

    press(Key::S);
    EXPECT_EQ(dynamic_cast<const EditorState&>(game.state()).status(), "Saved");
    EXPECT_TRUE(std::filesystem::exists(game.customLevelPath()));

    game.render(renderer);
    EXPECT_TRUE(renderer.hasText("Palette:"));
    EXPECT_TRUE(renderer.hasText("Selected: 3 (Enemy)"));

    press(Key::M);
    EXPECT_EQ(game.state().name(), "Menu");
}

TEST_F(GameStateTest, EditorLevelIsPlayable) {
    press(Key::Down);
    press(Key::Enter);
    press(Key::Digit1);
    for (int x = 0; x < config::kWindowWidth; x += config::kTileSize) {
        click(x, 9 * config::kTileSize, MouseButton::Left);
    }
    press(Key::S);
    press(Key::Escape);

    selectLevel(kCustom);
    ASSERT_EQ(game.state().name(), "Playing");
    EXPECT_EQ(game.world().tiles().solidCount(), config::kWindowWidth / config::kTileSize);
}

TEST_F(GameStateTest, EditorReopensWithSavedContent) {
    assets.writeLevel(Game::kCustomLevelName, "4\n");
    press(Key::Down);
    press(Key::Enter);
    EXPECT_EQ(game.editor().tileAt(0, 0), '4');
}

}  // namespace
}  // namespace mario
