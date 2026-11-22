#include <gtest/gtest.h>

#include <filesystem>
#include <sstream>
#include <string>

#include "mario/core/EditorModel.hpp"
#include "mario/core/LevelLoader.hpp"

namespace mario {
namespace {

constexpr int kTile = config::kTileSize;

TEST(EditorModelTest, StartsEmptyWithWallSelected) {
    const EditorModel editor;
    EXPECT_EQ(editor.selected(), TileCode::Solid);
    EXPECT_EQ(editor.cameraX(), 0);
    for (int y = 0; y < EditorModel::kHeight; ++y) {
        for (int x = 0; x < EditorModel::kWidth; ++x) {
            ASSERT_EQ(editor.tileAt(x, y), '0');
        }
    }
}

TEST(EditorModelTest, PaintAndEraseUseScreenCoordinates) {
    EditorModel editor;
    editor.select(TileCode::Coin);
    EXPECT_TRUE(editor.paint(2 * kTile + 5, 3 * kTile + 39));
    EXPECT_EQ(editor.tileAt(2, 3), '2');
    EXPECT_TRUE(editor.erase(2 * kTile, 3 * kTile));
    EXPECT_EQ(editor.tileAt(2, 3), '0');
}

TEST(EditorModelTest, PaintTakesCameraIntoAccount) {
    EditorModel editor;
    editor.scroll(config::kEditorCameraSpeed * 4);
    editor.paint(0, 0);
    EXPECT_EQ(editor.tileAt(config::kEditorCameraSpeed * 4 / kTile, 0), '1');
}

TEST(EditorModelTest, PaintOutsideTheGridIsRejected) {
    EditorModel editor;
    EXPECT_FALSE(editor.paint(-1, 0));
    EXPECT_FALSE(editor.paint(0, EditorModel::kHeight * kTile));
}

TEST(EditorModelTest, ScrollIsClampedToTheLevel) {
    EditorModel editor;
    editor.scroll(-100);
    EXPECT_EQ(editor.cameraX(), 0);
    editor.scroll(1'000'000);
    EXPECT_EQ(editor.cameraX(), config::kWorldPixelWidth - config::kWindowWidth);
}

TEST(EditorModelTest, SaveAndLoadRoundTrip) {
    EditorModel editor;
    editor.setTile(0, 0, '4');
    editor.setTile(140, 9, '8');
    editor.setTile(70, 5, '3');

    std::stringstream buffer;
    editor.save(buffer);

    EditorModel copy;
    copy.load(buffer);
    for (int y = 0; y < EditorModel::kHeight; ++y) {
        for (int x = 0; x < EditorModel::kWidth; ++x) {
            ASSERT_EQ(copy.tileAt(x, y), editor.tileAt(x, y)) << x << "," << y;
        }
    }
}

TEST(EditorModelTest, SavedFileIsAValidLevel) {
    EditorModel editor;
    editor.setTile(10, 9, '1');
    editor.setTile(11, 8, '3');
    editor.setTile(12, 8, '4');

    std::stringstream buffer;
    editor.save(buffer);
    const LevelData level = LevelLoader().parse(buffer);

    EXPECT_TRUE(level.tiles.isSolid(10, 9));
    EXPECT_EQ(level.enemies.size(), 1U);
    ASSERT_TRUE(level.flag.has_value());
    EXPECT_EQ(level.flag->x, 12 * kTile);
}

TEST(EditorModelTest, SavedRowsHaveFixedWidth) {
    const EditorModel editor;
    std::stringstream buffer;
    editor.save(buffer);
    std::string line;
    int rows = 0;
    while (std::getline(buffer, line)) {
        EXPECT_EQ(static_cast<int>(line.size()), EditorModel::kWidth);
        ++rows;
    }
    EXPECT_EQ(rows, EditorModel::kHeight);
}

TEST(EditorModelTest, FileRoundTrip) {
    const auto path = std::filesystem::temp_directory_path() / "mario_editor_roundtrip.txt";
    EditorModel editor;
    editor.setTile(5, 5, '6');
    ASSERT_TRUE(editor.saveFile(path));

    EditorModel loaded;
    ASSERT_TRUE(loaded.loadFile(path));
    EXPECT_EQ(loaded.tileAt(5, 5), '6');
    std::filesystem::remove(path);
}

TEST(EditorModelTest, MissingFileGivesEmptyGrid) {
    EditorModel editor;
    editor.setTile(1, 1, '1');
    EXPECT_FALSE(editor.loadFile("no/such/editor/file.txt"));
    EXPECT_EQ(editor.tileAt(1, 1), '0');
}

}  // namespace
}  // namespace mario
