#include <gtest/gtest.h>

#include "mario/core/WorldView.hpp"
#include "support/FakeRenderer.hpp"
#include "support/LevelBuilder.hpp"

namespace mario {
namespace {

class WorldViewTest : public ::testing::Test {
protected:
    EventDispatcher events;
    World world{events};
    test::FakeRenderer renderer;
};

TEST_F(WorldViewTest, HudShowsScoreAndLives) {
    Player player;
    player.score = 17;
    player.lives = 2;
    EXPECT_EQ(hudText(player), "Score: 17   Lives: 2");
}

TEST_F(WorldViewTest, DrawsOnlyVisibleTiles) {
    world.load(test::LevelBuilder().ground().build());
    renderWorld(world, renderer);

    const int visibleColumns = config::kWindowWidth / config::kTileSize + 1;
    EXPECT_EQ(renderer.spriteCount(Sprite::Ground), 2 * visibleColumns);
    EXPECT_EQ(renderer.spriteCount(Sprite::Player), 1);
    EXPECT_EQ(renderer.clears, 1);
    EXPECT_TRUE(renderer.hasText("Score: 0"));
}

TEST_F(WorldViewTest, GroundRowsAndFloatingBlocksUseDifferentSprites) {
    world.load(test::LevelBuilder().put(3, 4, '1').put(3, 9, '1').build());
    renderWorld(world, renderer);
    EXPECT_EQ(renderer.spriteCount(Sprite::Brick), 1);
    EXPECT_EQ(renderer.spriteCount(Sprite::Ground), 1);
}

TEST_F(WorldViewTest, EntitiesAreShiftedByTheRenderOffset) {
    world.load(test::LevelBuilder().put(5, 2, '3').put(6, 2, '5').put(7, 2, '6').put(8, 2, '7').build());
    renderWorld(world, renderer);

    EXPECT_EQ(renderer.spriteCount(Sprite::Enemy), 1);
    EXPECT_EQ(renderer.spriteCount(Sprite::Mushroom), 1);
    EXPECT_EQ(renderer.spriteCount(Sprite::Flower), 1);
    EXPECT_EQ(renderer.spriteCount(Sprite::MovingPlatform), 1);

    for (const auto& [sprite, rect] : renderer.spriteRects) {
        if (sprite == Sprite::Mushroom) {
            EXPECT_EQ(rect.x, 6 * config::kTileSize);
            EXPECT_EQ(rect.y, 2 * config::kTileSize + config::kRenderOffsetY);
        }
    }
}

TEST_F(WorldViewTest, FlagIsDrawnAsPoleAndCloth) {
    world.load(test::LevelBuilder().put(4, 7, '4').build());
    renderWorld(world, renderer);
    EXPECT_EQ(renderer.fills.size(), 2U);
}

TEST_F(WorldViewTest, DimOverlayCoversTheScreen) {
    renderDimOverlay(renderer, 128);
    ASSERT_EQ(renderer.fills.size(), 1U);
    EXPECT_EQ(renderer.fills[0], (Rect{0, 0, config::kWindowWidth, config::kWindowHeight}));
}

}  // namespace
}  // namespace mario
