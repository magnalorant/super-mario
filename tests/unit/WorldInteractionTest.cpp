#include <gtest/gtest.h>

#include <algorithm>
#include <functional>
#include <vector>

#include "mario/core/World.hpp"
#include "support/LevelBuilder.hpp"

namespace mario {
namespace {

constexpr int kTile = config::kTileSize;
constexpr int kFeetRow = TileMap::kHeight - 3;

class WorldInteractionTest : public ::testing::Test {
protected:
    void SetUp() override {
        events.subscribe([this](const GameEvent& e) { received.push_back(e); });
    }

    void load(const test::LevelBuilder& builder) { world.load(builder.build()); }

    StepOutcome runUntil(int maxFrames, const PlayerInput& input, const std::function<bool()>& done) {
        StepOutcome last = StepOutcome::Running;
        for (int i = 0; i < maxFrames && !done(); ++i) {
            last = world.step(input);
        }
        return last;
    }

    [[nodiscard]] int count(GameEventType type) const {
        return static_cast<int>(std::count_if(received.begin(), received.end(),
                                              [type](const GameEvent& e) { return e.type == type; }));
    }

    EventDispatcher events;
    World world{events};
    std::vector<GameEvent> received;
};

TEST_F(WorldInteractionTest, CollectingACoinAddsScore) {
    load(test::LevelBuilder().ground().put(5, kFeetRow, '2'));
    runUntil(60, PlayerInput{false, true, false, false}, [&] { return world.player().score > 0; });
    EXPECT_EQ(world.player().score, config::kPointsPerCoin);
    EXPECT_FALSE(world.tiles().hasCoin(5, kFeetRow));
    EXPECT_EQ(count(GameEventType::CoinCollected), 1);
}

TEST_F(WorldInteractionTest, MushroomGivesExtraLife) {
    load(test::LevelBuilder().ground().put(4, kFeetRow, '5'));
    runUntil(60, PlayerInput{false, true, false, false}, [&] { return world.player().hasMushroom; });
    EXPECT_TRUE(world.player().hasMushroom);
    EXPECT_EQ(world.player().lives, config::kInitialLives + config::kMushroomLivesBonus);
    EXPECT_FALSE(world.powerups()[0].active);
    EXPECT_EQ(count(GameEventType::PowerupCollected), 1);
}

TEST_F(WorldInteractionTest, FlowerEnablesFireballs) {
    load(test::LevelBuilder().ground().put(4, kFeetRow, '6'));
    EXPECT_FALSE(world.shootFireball());
    runUntil(60, PlayerInput{false, true, false, false}, [&] { return world.player().hasFlower; });
    ASSERT_TRUE(world.player().hasFlower);
    EXPECT_TRUE(world.shootFireball());
    EXPECT_EQ(count(GameEventType::FireballShot), 1);
}

TEST_F(WorldInteractionTest, FireballPoolIsLimited) {
    load(test::LevelBuilder().ground());
    world.player().hasFlower = true;
    int shots = 0;
    for (int i = 0; i < config::kMaxFireballs + 5; ++i) {
        shots += world.shootFireball() ? 1 : 0;
    }
    EXPECT_EQ(shots, config::kMaxFireballs);
}

TEST_F(WorldInteractionTest, FireballLeavesTheWorld) {
    load(test::LevelBuilder().ground());
    world.player().hasFlower = true;
    world.player().facing = Direction::Left;
    ASSERT_TRUE(world.shootFireball());
    runUntil(100, {}, [] { return false; });
    EXPECT_TRUE(std::none_of(world.fireballs().begin(), world.fireballs().end(),
                             [](const Fireball& f) { return f.active; }));
}

TEST_F(WorldInteractionTest, FireballDefeatsEnemy) {
    load(test::LevelBuilder().ground().put(12, kFeetRow, '3'));
    runUntil(30, {}, [] { return false; });
    world.player().hasFlower = true;
    ASSERT_TRUE(world.shootFireball());
    runUntil(200, {}, [&] { return !world.enemies()[0].alive; });
    EXPECT_FALSE(world.enemies()[0].alive);
    EXPECT_EQ(world.player().score, config::kPointsPerEnemyKill);
    EXPECT_EQ(count(GameEventType::EnemyDefeated), 1);
}

TEST_F(WorldInteractionTest, WalkingIntoAnEnemyCostsALife) {
    load(test::LevelBuilder().ground().put(6, kFeetRow, '3'));
    world.enemies()[0].direction = Direction::Left;
    const StepOutcome outcome =
        runUntil(120, {}, [&] { return world.player().lives < config::kInitialLives; });
    EXPECT_EQ(outcome, StepOutcome::LifeLost);
    EXPECT_EQ(world.player().lives, config::kInitialLives - 1);
    EXPECT_TRUE(world.enemies()[0].alive);
    EXPECT_EQ(count(GameEventType::PlayerDied), 1);
}

TEST_F(WorldInteractionTest, StompingAnEnemyDefeatsItAndBounces) {
    load(test::LevelBuilder().ground().put(6, kFeetRow, '3'));
    Player& player = world.player();
    player.x = 6 * kTile;
    player.y = 4 * kTile;
    player.velocityY = 6.0F;
    runUntil(30, {}, [&] { return !world.enemies()[0].alive; });
    EXPECT_FALSE(world.enemies()[0].alive);
    EXPECT_EQ(player.lives, config::kInitialLives);
    EXPECT_EQ(player.score, config::kPointsPerEnemyKill);
}

TEST_F(WorldInteractionTest, EnemyTurnsAtWalls) {
    load(test::LevelBuilder().ground().put(20, kFeetRow, '3').put(22, kFeetRow, '1'));
    world.player().x = 0;
    runUntil(40, {}, [] { return false; });
    EXPECT_EQ(world.enemies()[0].direction, Direction::Left);
    EXPECT_LT(world.enemies()[0].x + world.enemies()[0].w, 22 * kTile + 1);
}

TEST_F(WorldInteractionTest, EnemyTurnsAtLedges) {
    load(test::LevelBuilder().ground().pit(23, 30).put(20, kFeetRow, '3'));
    for (int i = 0; i < 200; ++i) {
        world.step({});
        ASSERT_LE(world.enemies()[0].x, 23 * kTile) << "enemy walked over the ledge at frame " << i;
    }
}

TEST_F(WorldInteractionTest, TouchingTheFlagCompletesTheLevel) {
    load(test::LevelBuilder().ground().put(6, kFeetRow, '4'));
    const StepOutcome outcome = runUntil(120, PlayerInput{false, true, false, false},
                                         [&] { return count(GameEventType::LevelCompleted) > 0; });
    EXPECT_EQ(outcome, StepOutcome::LevelCompleted);
}

TEST_F(WorldInteractionTest, LoadResetsPreviousProgress) {
    load(test::LevelBuilder().ground());
    world.player().score = 50;
    world.player().hasFlower = true;
    ASSERT_TRUE(world.shootFireball());
    load(test::LevelBuilder().ground());
    EXPECT_EQ(world.player().score, 0);
    EXPECT_FALSE(world.player().hasFlower);
    EXPECT_EQ(world.camera().x(), 0);
    EXPECT_TRUE(std::none_of(world.fireballs().begin(), world.fireballs().end(),
                             [](const Fireball& f) { return f.active; }));
}

}  // namespace
}  // namespace mario
