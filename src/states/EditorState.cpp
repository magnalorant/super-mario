#include <memory>
#include <string>
#include <variant>

#include "mario/core/Config.hpp"
#include "mario/states/Game.hpp"
#include "mario/states/States.hpp"

namespace mario {

namespace {

constexpr int kWidth = config::kWindowWidth;
constexpr int kHeight = config::kWindowHeight;
constexpr int kTile = config::kTileSize;
constexpr int kTextSize = 20;

void drawCell(Renderer& renderer, char symbol, int tileY, const Rect& cell) {
    switch (symbol) {
        case '0':
            renderer.fillRect(cell, palette::kEditorEmpty);
            break;
        case '1':
            renderer.drawSprite(tileY >= EditorModel::kHeight - 2 ? Sprite::Ground : Sprite::Brick, cell);
            break;
        case '2':
            renderer.drawSprite(Sprite::Coin, cell);
            break;
        case '3':
            renderer.drawSprite(Sprite::Enemy, cell);
            break;
        case '4':
            renderer.fillRect(cell, palette::kYellow);
            break;
        case '5':
            renderer.drawSprite(Sprite::Mushroom, cell);
            break;
        case '6':
            renderer.drawSprite(Sprite::Flower, cell);
            break;
        case '7':
        case '8':
            renderer.drawSprite(Sprite::MovingPlatform, cell);
            break;
        default:
            renderer.fillRect(cell, Color{100, 100, 100, 255});
            break;
    }
    renderer.outlineRect(cell, palette::kEditorGrid);
}

void drawGrid(const EditorModel& editor, Renderer& renderer) {
    for (int ty = 0; ty < EditorModel::kHeight; ++ty) {
        for (int tx = 0; tx < EditorModel::kWidth; ++tx) {
            const Rect cell{tx * kTile - editor.cameraX(), ty * kTile, kTile, kTile};
            if (cell.visibleHorizontally(kWidth)) {
                drawCell(renderer, editor.tileAt(tx, ty), ty, cell);
            }
        }
    }
}

void drawHelp(const EditorModel& editor, const std::string& status, Renderer& renderer) {
    const Color panel{0, 0, 0, 180};
    renderer.fillRect({0, kHeight - 125, kWidth, 125}, panel);
    renderer.fillRect({kWidth - 210, 0, 210, 255}, panel);

    const std::string selected = std::string("Selected: ") + toChar(editor.selected()) + " (" +
                                 std::string(tileName(editor.selected())) + ")";
    renderer.drawText("Arrows: move camera", 10, kHeight - 115, palette::kWhite, kTextSize);
    renderer.drawText(selected, 10, kHeight - 90, palette::kYellow, kTextSize);
    renderer.drawText("Left click: place, Right click: erase", 10, kHeight - 65, palette::kWhite, kTextSize);
    renderer.drawText("S: save (custom_map.txt), M/ESC: menu", 10, kHeight - 40, palette::kWhite, kTextSize);
    if (!status.empty()) {
        renderer.drawText(status, 450, kHeight - 115, palette::kYellow, kTextSize);
    }

    constexpr int paletteX = kWidth - 200;
    int y = 10;
    renderer.drawText("Palette:", paletteX, y, palette::kYellow, kTextSize);
    for (char c = '0'; c <= '8'; ++c) {
        y += 25;
        const std::string entry = std::string(1, c) + ": " + std::string(tileName(static_cast<TileCode>(c)));
        renderer.drawText(entry, paletteX, y, palette::kWhite, kTextSize);
    }
}

}  // namespace

void EditorState::handleEvent(Game& game, const InputEvent& event) {
    EditorModel& editor = game.editor();

    if (const auto* mouse = std::get_if<MousePressed>(&event)) {
        if (mouse->button == MouseButton::Left) {
            editor.paint(mouse->x, mouse->y);
        } else if (mouse->button == MouseButton::Right) {
            editor.erase(mouse->x, mouse->y);
        }
        return;
    }

    const Key key = std::get<KeyPressed>(event).key;
    if (const auto tile = tileForKey(key)) {
        editor.select(*tile);
        return;
    }
    switch (key) {
        case Key::Escape:
        case Key::M:
            game.changeState(std::make_unique<MenuState>());
            break;
        case Key::S:
            status_ = game.saveEditorLevel() ? "Saved" : "Save failed";
            break;
        case Key::Left:
            editor.scroll(-config::kEditorCameraSpeed);
            break;
        case Key::Right:
            editor.scroll(config::kEditorCameraSpeed);
            break;
        default:
            break;
    }
}

void EditorState::render(const Game& game, Renderer& renderer) const {
    renderer.clear(palette::kEditorBackground);
    drawGrid(game.editor(), renderer);
    drawHelp(game.editor(), status_, renderer);
}

}  // namespace mario
