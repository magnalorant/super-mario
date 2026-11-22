#include <gtest/gtest.h>

#include <vector>

#include "mario/core/World.hpp"
#include "support/LevelBuilder.hpp"

namespace mario {
namespace {

constexpr int kTile = config::kTileSize;
constexpr int kGroundTop = (TileMap::kHeight - 2) * kTile;

class WorldPhysicsTest : public ::testing::Test {
protected:
    void load(const test::LevelBuilder& builder) { world.load(builder.build()); }

    StepOutcome run(int frames, const PlayerInput& input = {}) {
        StepOutcome last = StepOutcome::Running;
        for (int i = 0; i < frames; ++i) {
            last = world.step(input);
        }
        return last;
    }

    void settle() { run(60); }

    EventDispatcher events;
    World world{events};
    std::vector<GameEvent> received;

    void SetUp() override {
        events.subscribe([this](const GameEvent& e) { received.push_back(e); });
    }
};

TEST_F(WorldPhysicsTest, PlayerFallsAndLandsOnGround) {
    load(test::LevelBuilder().ground());
    settle();
    for (int i = 0; i < 10; ++i) {
        world.step({});
        EXPECT_EQ(world.player().y, kGroundTop - world.player().h);
        EXPECT_LT(world.player().velocityY, 1.0F);
    }
}

TEST_F(WorldPhysicsTest, WalksRightAndLeft) {
    load(test::LevelBuilder().ground());
    settle();
    const int start = world.player().x;

    run(10, PlayerInput{false, true, false, false});
    EXPECT_EQ(world.player().x, start + 10 * config::kPlayerSpeed);
    EXPECT_EQ(world.player().facing, Direction::Right);

    run(4, PlayerInput{true, false, false, false});
    EXPECT_EQ(world.player().x, start + 6 * config::kPlayerSpeed);
    EXPECT_EQ(world.player().facing, Direction::Left);
}

TEST_F(WorldPhysicsTest, CannotLeaveTheLeftEdge) {
    load(test::LevelBuilder().ground());
    run(100, PlayerInput{true, false, false, false});
    EXPECT_EQ(world.player().x, 0);
}

TEST_F(WorldPhysicsTest, WallStopsHorizontalMovement) {
    load(test::LevelBuilder().ground().put(5, 7, '1').put(5, 6, '1'));
    settle();
    run(40, PlayerInput{false, true, false, false});
    EXPECT_EQ(world.player().x, 5 * kTile - world.player().w);
}

TEST_F(WorldPhysicsTest, JumpRequiresGround) {
    load(test::LevelBuilder().ground());
    settle();
    const int groundY = world.player().y;

    world.step(PlayerInput{false, false, true, true});
    EXPECT_LT(world.player().y, groundY);
    EXPECT_LT(world.player().velocityY, 0.0F);

    run(5);
    const float midAir = world.player().velocityY;
    world.step(PlayerInput{false, false, true, true});
    EXPECT_FLOAT_EQ(world.player().velocityY, midAir + config::kGravity);

    settle();
    EXPECT_EQ(world.player().y, groundY);
}

TEST_F(WorldPhysicsTest, HeadBumpStopsJumpAndKeepsBlockWithoutMushroom) {
    load(test::LevelBuilder().ground().put(3, 5, '1'));
    settle();
    world.step(PlayerInput{false, false, true, true});
    run(3);
    EXPECT_EQ(world.player().y, 6 * kTile);
    EXPECT_TRUE(world.tiles().isSolid(3, 5));
}

TEST_F(WorldPhysicsTest, MushroomBreaksBlocksFromBelow) {
    load(test::LevelBuilder().ground().put(3, 5, '1'));
    settle();
    world.player().hasMushroom = true;
    world.step(PlayerInput{false, false, true, true});
    run(3);
    EXPECT_FALSE(world.tiles().isSolid(3, 5));
    ASSERT_FALSE(received.empty());
    EXPECT_EQ(received.back().type, GameEventType::BlockBroken);
    EXPECT_EQ(received.back().value, 3);
}

TEST_F(WorldPhysicsTest, FallingIntoAPitCostsALife) {
    load(test::LevelBuilder().ground().pit(0, 10));
    const int lives = world.player().lives;
    StepOutcome outcome = StepOutcome::Running;
    for (int i = 0; i < 200 && outcome == StepOutcome::Running; ++i) {
        outcome = world.step({});
    }
    EXPECT_EQ(outcome, StepOutcome::LifeLost);
    EXPECT_EQ(world.player().lives, lives - 1);
    EXPECT_EQ(world.player().x, config::kPlayerInitialX);
}

TEST_F(WorldPhysicsTest, LosingTheLastLifeIsGameOver) {
    load(test::LevelBuilder());
    world.player().lives = 1;
    StepOutcome outcome = StepOutcome::Running;
    for (int i = 0; i < 200 && outcome == StepOutcome::Running; ++i) {
        outcome = world.step({});
    }
    EXPECT_EQ(outcome, StepOutcome::GameOver);
    EXPECT_EQ(world.player().lives, 0);
    ASSERT_GE(received.size(), 2U);
    EXPECT_EQ(received[received.size() - 2].type, GameEventType::PlayerDied);
    EXPECT_EQ(received.back().type, GameEventType::GameOver);
}

TEST_F(WorldPhysicsTest, CameraFollowsThePlayer) {
    load(test::LevelBuilder().ground());
    settle();
    run(200, PlayerInput{false, true, false, false});
    EXPECT_EQ(world.camera().x(), world.player().x - config::kWindowWidth / 3);
}

TEST_F(WorldPhysicsTest, PlayerLandsOnPlatformAndIsCarried) {
    load(test::LevelBuilder().put(2, 7, '7'));
    world.step({});
    run(30);
    ASSERT_TRUE(world.player().onPlatform());
    const Platform& platform = world.platforms()[0];
    EXPECT_EQ(world.player().y, platform.y - world.player().h);

    const int before = world.player().x;
    world.step({});
    EXPECT_EQ(world.player().x, before + platform.carryOffsetX());
}

TEST_F(WorldPhysicsTest, JumpingOffAPlatformDetachesThePlayer) {
    load(test::LevelBuilder().put(2, 7, '7'));
    run(30);
    ASSERT_TRUE(world.player().onPlatform());
    world.step(PlayerInput{false, false, true, true});
    EXPECT_FALSE(world.player().onPlatform());
    EXPECT_LT(world.player().velocityY, 0.0F);
}

}  // namespace
}  // namespace mario
