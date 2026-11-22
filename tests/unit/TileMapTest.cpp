#include <gtest/gtest.h>

#include "mario/core/TileCode.hpp"
#include "mario/core/TileMap.hpp"

namespace mario {
namespace {

TEST(TileMapTest, StartsEmpty) {
    const TileMap map;
    EXPECT_EQ(map.solidCount(), 0);
    EXPECT_EQ(map.coinCount(), 0);
    EXPECT_FALSE(map.isSolid(0, 0));
}

TEST(TileMapTest, SetAndQuerySolid) {
    TileMap map;
    map.setSolid(3, 4, true);
    EXPECT_TRUE(map.isSolid(3, 4));
    EXPECT_FALSE(map.isSolid(4, 3));
    map.setSolid(3, 4, false);
    EXPECT_FALSE(map.isSolid(3, 4));
}

TEST(TileMapTest, OutOfBoundsIsAirAndWritesAreIgnored) {
    TileMap map;
    map.setSolid(-1, 0, true);
    map.setSolid(TileMap::kWidth, 0, true);
    map.setSolid(0, TileMap::kHeight, true);
    map.setCoin(0, -1, true);
    EXPECT_EQ(map.solidCount(), 0);
    EXPECT_EQ(map.coinCount(), 0);
    EXPECT_FALSE(map.isSolid(-1, 0));
    EXPECT_FALSE(map.hasCoin(TileMap::kWidth, TileMap::kHeight));
}

TEST(TileMapTest, TakeCoinRemovesItOnce) {
    TileMap map;
    map.setCoin(7, 2, true);
    EXPECT_EQ(map.coinCount(), 1);
    EXPECT_TRUE(map.takeCoin(7, 2));
    EXPECT_FALSE(map.takeCoin(7, 2));
    EXPECT_EQ(map.coinCount(), 0);
}

TEST(TileMapTest, BoundsCheck) {
    EXPECT_TRUE(TileMap::inBounds(0, 0));
    EXPECT_TRUE(TileMap::inBounds(TileMap::kWidth - 1, TileMap::kHeight - 1));
    EXPECT_FALSE(TileMap::inBounds(TileMap::kWidth, 0));
    EXPECT_FALSE(TileMap::inBounds(0, -1));
}

TEST(TileCodeTest, RoundTripsThroughCharacters) {
    for (char c = '0'; c <= '8'; ++c) {
        const auto code = tileCodeFromChar(c);
        ASSERT_TRUE(code.has_value());
        EXPECT_EQ(toChar(*code), c);
        EXPECT_NE(tileName(*code), "Unknown");
    }
}

TEST(TileCodeTest, RejectsUnknownCharacters) {
    EXPECT_FALSE(tileCodeFromChar('9').has_value());
    EXPECT_FALSE(tileCodeFromChar('x').has_value());
    EXPECT_FALSE(tileCodeFromChar(' ').has_value());
}

}  // namespace
}  // namespace mario
