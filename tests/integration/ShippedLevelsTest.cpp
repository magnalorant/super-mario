#include <gtest/gtest.h>

#include <filesystem>
#include <string>

#include "mario/core/LevelLoader.hpp"
#include "mario/core/WorldView.hpp"
#include "mario/states/Game.hpp"
#include "support/FakeRenderer.hpp"

namespace mario {
namespace {

const std::filesystem::path kAssetDir{MARIO_ASSET_DIR};

class ShippedLevelsTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ShippedLevelsTest, LoadsWithGoalAndSafeSpawn) {
    const auto path = kAssetDir / "levels" / GetParam();
    ASSERT_TRUE(std::filesystem::exists(path)) << path;

    const LevelData level = LevelLoader().loadFile(path);
    EXPECT_TRUE(level.flag.has_value());
    EXPECT_GT(level.tiles.solidCount(), 0);

    const int spawnColumn = config::kPlayerInitialX / config::kTileSize;
    bool groundBelowSpawn = false;
    for (int ty = config::kMapHeight - config::kPlayerInitialYOffsetTiles; ty < config::kMapHeight; ++ty) {
        groundBelowSpawn = groundBelowSpawn || level.tiles.isSolid(spawnColumn, ty);
    }
    EXPECT_TRUE(groundBelowSpawn) << "the player would fall into a pit right after spawning";
}

TEST_P(ShippedLevelsTest, SurvivesALongScriptedRun) {
    Game game(kAssetDir);
    const auto& levels = game.levels();
    std::size_t index = 0;
    while (index < levels.size() && levels[index].name != GetParam()) {
        ++index;
    }
    ASSERT_LT(index, levels.size());
    ASSERT_TRUE(game.startLevel(index)) << game.lastError();

    World& world = game.world();
    test::FakeRenderer renderer;
    for (int frame = 0; frame < 3000; ++frame) {
        const PlayerInput input{false, true, frame % 40 == 0, frame % 40 < 10};
        const StepOutcome outcome = world.step(input);
        if (frame % 7 == 0) {
            world.shootFireball();
        }
        ASSERT_GE(world.player().x, 0);
        ASSERT_LE(world.player().x + world.player().w, config::kWorldPixelWidth);
        if (outcome == StepOutcome::GameOver || outcome == StepOutcome::LevelCompleted) {
            break;
        }
    }
    renderWorld(world, renderer);
    EXPECT_GE(renderer.clears, 1);
}

INSTANTIATE_TEST_SUITE_P(Assets, ShippedLevelsTest,
                         ::testing::Values("map1.txt", "map2.txt", "map3.txt", "custom_map.txt"),
                         [](const ::testing::TestParamInfo<std::string>& param) {
                             return param.param.substr(0, param.param.find('.'));
                         });

}  // namespace
}  // namespace mario
