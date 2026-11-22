#include <gtest/gtest.h>

#include <filesystem>
#include <string>

#include "mario/core/LevelLoader.hpp"

namespace mario {
namespace {

constexpr int kTile = config::kTileSize;

TEST(LevelLoaderTest, ParsesEveryTileCode) {
    const LevelData level = LevelLoader().parseString("012345678\n");

    EXPECT_FALSE(level.tiles.isSolid(0, 0));
    EXPECT_TRUE(level.tiles.isSolid(1, 0));
    EXPECT_TRUE(level.tiles.hasCoin(2, 0));

    ASSERT_EQ(level.enemies.size(), 1U);
    EXPECT_EQ(level.enemies[0].x, 3 * kTile);
    EXPECT_EQ(level.enemies[0].y, 0);
    EXPECT_TRUE(level.enemies[0].alive);
    EXPECT_EQ(level.enemies[0].direction, Direction::Right);

    ASSERT_TRUE(level.flag.has_value());
    EXPECT_EQ(level.flag->x, 4 * kTile);

    ASSERT_EQ(level.powerups.size(), 2U);
    EXPECT_EQ(level.powerups[0].kind, PowerupKind::Mushroom);
    EXPECT_EQ(level.powerups[1].kind, PowerupKind::Flower);
    EXPECT_EQ(level.powerups[1].x, 6 * kTile);

    ASSERT_EQ(level.platforms.size(), 2U);
    EXPECT_EQ(level.platforms[0].motion->axis(), PlatformAxis::Horizontal);
    EXPECT_EQ(level.platforms[1].motion->axis(), PlatformAxis::Vertical);
    EXPECT_EQ(level.platforms[1].startX, 8 * kTile);
}

TEST(LevelLoaderTest, RowsMapToTileY) {
    const LevelData level = LevelLoader().parseString("0\n0\n01\n");
    EXPECT_TRUE(level.tiles.isSolid(1, 2));
    EXPECT_EQ(level.tiles.solidCount(), 1);
}

TEST(LevelLoaderTest, AcceptsWindowsLineEndings) {
    const LevelData level = LevelLoader().parseString("1\r\n2\r\n");
    EXPECT_TRUE(level.tiles.isSolid(0, 0));
    EXPECT_TRUE(level.tiles.hasCoin(0, 1));
}

TEST(LevelLoaderTest, IgnoresUnknownCharacters) {
    const LevelData level = LevelLoader().parseString("x9 #1\n");
    EXPECT_EQ(level.tiles.solidCount(), 1);
    EXPECT_TRUE(level.tiles.isSolid(4, 0));
    EXPECT_TRUE(level.enemies.empty());
}

TEST(LevelLoaderTest, TruncatesLongLinesAndExtraRows) {
    std::string text;
    for (int y = 0; y < TileMap::kHeight + 3; ++y) {
        text += std::string(TileMap::kWidth + 20, '1') + "\n";
    }
    const LevelData level = LevelLoader().parseString(text);
    EXPECT_EQ(level.tiles.solidCount(), TileMap::kWidth * TileMap::kHeight);
}

TEST(LevelLoaderTest, EmptyInputGivesEmptyLevel) {
    const LevelData level = LevelLoader().parseString("");
    EXPECT_EQ(level.tiles.solidCount(), 0);
    EXPECT_FALSE(level.flag.has_value());
}

TEST(LevelLoaderTest, LastFlagWins) {
    const LevelData level = LevelLoader().parseString("4004\n");
    ASSERT_TRUE(level.flag.has_value());
    EXPECT_EQ(level.flag->x, 3 * kTile);
}

TEST(LevelLoaderTest, EntityCountsAreCapped) {
    const std::string row(TileMap::kWidth, '3');
    std::string text;
    for (int i = 0; i < 2; ++i) {
        text += row + "\n";
    }
    const LevelData level = LevelLoader().parseString(text);
    EXPECT_EQ(static_cast<int>(level.enemies.size()), config::kMaxEnemies);

    const LevelData platforms = LevelLoader().parseString(std::string(40, '7'));
    EXPECT_EQ(static_cast<int>(platforms.platforms.size()), config::kMaxPlatforms);

    const LevelData powerups = LevelLoader().parseString(std::string(40, '5'));
    EXPECT_EQ(static_cast<int>(powerups.powerups.size()), config::kMaxPowerups);
}

TEST(LevelLoaderTest, MissingFileThrows) {
    const LevelLoader loader;
    EXPECT_THROW((void)loader.loadFile("this/file/does/not/exist.txt"), LevelLoadError);
}

TEST(EntityFactoryTest, CustomCreatorsReplaceDefaults) {
    EntityFactory factory = EntityFactory::withDefaultCreators();
    factory.registerCreator(TileCode::Coin,
                            [](LevelData& level, int tx, int ty) { level.tiles.setSolid(tx, ty, true); });

    const LevelData level = LevelLoader(factory).parseString("2\n");
    EXPECT_TRUE(level.tiles.isSolid(0, 0));
    EXPECT_FALSE(level.tiles.hasCoin(0, 0));
}

TEST(EntityFactoryTest, EmptyFactoryCreatesNothing) {
    const EntityFactory factory;
    LevelData level;
    EXPECT_FALSE(factory.canCreate(TileCode::Solid));
    EXPECT_FALSE(factory.create('1', level, 0, 0));
    EXPECT_FALSE(factory.create('z', level, 0, 0));
    EXPECT_EQ(level.tiles.solidCount(), 0);
}

TEST(EntityFactoryTest, DefaultFactoryHandlesAllCodesButEmpty) {
    const EntityFactory factory = EntityFactory::withDefaultCreators();
    EXPECT_FALSE(factory.canCreate(TileCode::Empty));
    for (char c = '1'; c <= '8'; ++c) {
        EXPECT_TRUE(factory.canCreate(*tileCodeFromChar(c))) << c;
    }
}

}  // namespace
}  // namespace mario
