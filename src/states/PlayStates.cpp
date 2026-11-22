#include <memory>

#include "mario/core/Config.hpp"
#include "mario/core/WorldView.hpp"
#include "mario/states/Game.hpp"
#include "mario/states/States.hpp"

namespace mario {

namespace {

constexpr int kWidth = config::kWindowWidth;
constexpr int kHeight = config::kWindowHeight;
constexpr std::uint8_t kOverlayAlpha = 180;

}  // namespace

void PlayingState::handleEvent(Game& game, const InputEvent& event) {
    if (isAnyKey(event, Key::P, Key::Escape)) {
        game.changeState(std::make_unique<PausedState>());
    } else if (isKey(event, Key::LeftShift)) {
        game.world().shootFireball();
    } else if (isAnyKey(event, Key::Up, Key::Space)) {
        jumpQueued_ = true;
    }
}

void PlayingState::update(Game& game, const InputState& held, int elapsedMs) {
    if (freezeMs_ > 0) {
        freezeMs_ -= elapsedMs;
        jumpQueued_ = false;
        return;
    }

    const PlayerInput input{held.left, held.right, jumpQueued_, held.up || held.space};
    jumpQueued_ = false;

    switch (game.world().step(input)) {
        case StepOutcome::Running:
            break;
        case StepOutcome::LifeLost:
            freezeMs_ = config::kRespawnDelayMs;
            break;
        case StepOutcome::GameOver:
            game.changeState(std::make_unique<GameOverState>());
            break;
        case StepOutcome::LevelCompleted:
            game.changeState(std::make_unique<LevelCompleteState>());
            break;
    }
}

void PlayingState::render(const Game& game, Renderer& renderer) const { renderWorld(game.world(), renderer); }

void PausedState::handleEvent(Game& game, const InputEvent& event) {
    if (isAnyKey(event, Key::P, Key::Escape)) {
        game.changeState(std::make_unique<PlayingState>());
    } else if (isKey(event, Key::M)) {
        game.changeState(std::make_unique<MenuState>());
    }
}

void PausedState::render(const Game& game, Renderer& renderer) const {
    renderWorld(game.world(), renderer);
    renderDimOverlay(renderer, kOverlayAlpha);

    int y = kHeight / 3;
    renderer.drawTextCentered("PAUSED", y, palette::kWhite, palette::kShadow, 60, 3);
    y += 100;
    renderer.drawTextCentered("P or ESC - resume", y, palette::kWhite, palette::kShadow, 36, 3);
    y += 56;
    renderer.drawTextCentered("M - menu", y, palette::kWhite, palette::kShadow, 36, 3);
}

void GameOverState::handleEvent(Game& game, const InputEvent& event) {
    if (isAnyKey(event, Key::Enter, Key::Space)) {
        game.changeState(std::make_unique<MenuState>());
    }
}

void GameOverState::render(const Game& game, Renderer& renderer) const {
    renderWorld(game.world(), renderer);
    renderDimOverlay(renderer, kOverlayAlpha);

    int y = kHeight / 2 - 80;
    renderer.drawTextCentered("GAME OVER", y, palette::kRed, palette::kDeepShadow, 72, 4);
    y += 112;
    renderer.drawTextCentered("ENTER - menu", y, palette::kWhite, palette::kDeepShadow, 40, 3);
}

void LevelCompleteState::handleEvent(Game& /*game*/, const InputEvent& /*event*/) {}

void LevelCompleteState::update(Game& game, const InputState& /*held*/, int elapsedMs) {
    elapsedMs_ += elapsedMs;
    if (elapsedMs_ >= config::kLevelCompleteDelayMs) {
        game.changeState(std::make_unique<MenuState>());
    }
}

void LevelCompleteState::render(const Game& game, Renderer& renderer) const {
    renderWorld(game.world(), renderer);
    renderer.fillRect({kWidth / 2 - 200, kHeight / 2 - 60, 400, 120}, Color{0, 0, 0, 200});
    renderer.drawTextCentered("Congratulations!", kHeight / 2 - 28, palette::kYellow, palette::kShadow, 48, 2);
}

}  // namespace mario
