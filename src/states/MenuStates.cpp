#include <memory>

#include "mario/core/Config.hpp"
#include "mario/states/Game.hpp"
#include "mario/states/States.hpp"

namespace mario {

namespace {

constexpr int kWidth = config::kWindowWidth;
constexpr int kHeight = config::kWindowHeight;

std::size_t wrapStep(std::size_t value, std::size_t count, bool forward) noexcept {
    return forward ? (value + 1) % count : (value + count - 1) % count;
}

}  // namespace

void MenuState::handleEvent(Game& game, const InputEvent& event) {
    if (isKey(event, Key::Up)) {
        selected_ = wrapStep(selected_, kOptions.size(), false);
    } else if (isKey(event, Key::Down)) {
        selected_ = wrapStep(selected_, kOptions.size(), true);
    } else if (isAnyKey(event, Key::Enter, Key::Space)) {
        switch (selected_) {
            case 0:
                game.changeState(std::make_unique<LevelSelectState>());
                break;
            case 1:
                game.openEditor();
                game.changeState(std::make_unique<EditorState>());
                break;
            default:
                game.requestQuit();
                break;
        }
    }
}

void MenuState::render(const Game& /*game*/, Renderer& renderer) const {
    renderer.clear(palette::kSky);
    for (int y = 0; y < kHeight; y += config::kTileSize) {
        for (int x = 0; x < kWidth; x += config::kTileSize) {
            renderer.drawSprite(Sprite::Ground, {x, y, config::kTileSize, config::kTileSize});
        }
    }
    renderer.fillRect({kWidth / 2 - 260, kHeight / 4 - 40, 520, 320}, Color{0, 0, 0, 180});
    renderer.drawTextCentered("SUPER MARIO", kHeight / 4 - 42, palette::kGold, palette::kShadow, 72, 4);

    constexpr int firstY = kHeight / 2 - 50;
    constexpr int spacing = 60;
    for (std::size_t i = 0; i < kOptions.size(); ++i) {
        const Color color = i == selected_ ? palette::kGold : palette::kWhite;
        renderer.drawTextCentered(kOptions[i], firstY + static_cast<int>(i) * spacing, color,
                                  palette::kShadow, 38, 3);
    }
}

void LevelSelectState::handleEvent(Game& game, const InputEvent& event) {
    const std::size_t count = game.levels().size();
    if (isAnyKey(event, Key::Left, Key::Up)) {
        selected_ = wrapStep(selected_, count, false);
    } else if (isAnyKey(event, Key::Right, Key::Down)) {
        selected_ = wrapStep(selected_, count, true);
    } else if (isAnyKey(event, Key::Enter, Key::Space)) {
        if (game.startLevel(selected_)) {
            game.changeState(std::make_unique<PlayingState>());
        } else {
            error_ = game.lastError();
        }
    } else if (isKey(event, Key::Escape)) {
        game.changeState(std::make_unique<MenuState>());
    }
}

void LevelSelectState::render(const Game& game, Renderer& renderer) const {
    renderer.clear(palette::kSky);
    renderer.fillRect({kWidth / 2 - 250, kHeight / 4 - 30, 500, kHeight / 2 + 90}, Color{0, 0, 0, 100});

    int y = kHeight / 4;
    renderer.drawTextCentered("Select a level", y, palette::kWhite, palette::kBlueShadow, 48, 3);
    y += 80;

    const auto& levels = game.levels();
    for (std::size_t i = 0; i < levels.size(); ++i) {
        const bool active = i == selected_;
        const std::string label = (active ? "> " : "  ") + levels[i].name;
        renderer.drawTextCentered(label, y, active ? palette::kGold : palette::kWhite, palette::kBlueShadow,
                                  32, 2);
        y += 47;
    }

    y += 10;
    renderer.drawTextCentered("ESC - back", y, palette::kWhite, palette::kBlueShadow, 28, 2);

    if (!error_.empty()) {
        renderer.drawTextCentered(error_, y + 40, palette::kRed, palette::kShadow, 18, 1);
    }
}

}  // namespace mario
