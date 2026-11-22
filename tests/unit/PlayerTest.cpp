#include <gtest/gtest.h>

#include "mario/core/Entities.hpp"

namespace mario {
namespace {

TEST(PlayerTest, ResetForNewLevelRestoresInitialState) {
    Player player;
    player.score = 42;
    player.lives = 1;
    player.hasFlower = true;
    player.facing = Direction::Left;
    player.resetForNewLevel();

    EXPECT_EQ(player.x, config::kPlayerInitialX);
    EXPECT_EQ(player.y, (config::kMapHeight - config::kPlayerInitialYOffsetTiles) * config::kTileSize - player.h);
    EXPECT_EQ(player.lives, config::kInitialLives);
    EXPECT_EQ(player.score, 0);
    EXPECT_FALSE(player.hasFlower);
    EXPECT_EQ(player.facing, Direction::Right);
}

TEST(PlayerTest, RespawnKeepsScoreAndLivesButDropsPowerups) {
    Player player;
    player.resetForNewLevel();
    player.score = 9;
    player.lives = 2;
    player.hasMushroom = true;
    player.velocityY = 7.0F;
    player.supportingPlatform = 3;
    player.x = 900;

    player.respawn();

    EXPECT_EQ(player.score, 9);
    EXPECT_EQ(player.lives, 2);
    EXPECT_FALSE(player.hasMushroom);
    EXPECT_FLOAT_EQ(player.velocityY, 0.0F);
    EXPECT_FALSE(player.onPlatform());
    EXPECT_EQ(player.x, config::kPlayerInitialX);
}

TEST(DirectionTest, SignAndReverse) {
    EXPECT_EQ(sign(Direction::Left), -1);
    EXPECT_EQ(sign(Direction::Right), 1);
    EXPECT_EQ(reversed(Direction::Left), Direction::Right);
    EXPECT_EQ(reversed(Direction::Right), Direction::Left);
}

}  // namespace
}  // namespace mario
