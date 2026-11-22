#include "mario/states/Game.hpp"

#include <system_error>
#include <utility>

#include "mario/states/States.hpp"

namespace mario {

void GameState::update(Game& /*game*/, const InputState& /*held*/, int /*elapsedMs*/) {}

Game::Game(const std::filesystem::path& assetDir)
    : levelDir_(assetDir / "levels"), world_(events_), state_(std::make_unique<MenuState>()) {
    for (const char* file : {"map1.txt", "map2.txt", "map3.txt", kCustomLevelName}) {
        levels_.push_back(LevelEntry{file, levelDir_ / file});
    }
}

Game::~Game() = default;

void Game::handleEvent(const InputEvent& event) {
    state_->handleEvent(*this, event);
    applyPendingState();
}

void Game::update(const InputState& held, int elapsedMs) {
    state_->update(*this, held, elapsedMs);
    applyPendingState();
}

void Game::render(Renderer& renderer) const { state_->render(*this, renderer); }

void Game::changeState(std::unique_ptr<GameState> next) { pending_ = std::move(next); }

void Game::applyPendingState() {
    if (pending_) {
        state_ = std::move(pending_);
    }
}

bool Game::startLevel(std::size_t index) {
    if (index >= levels_.size()) {
        lastError_ = "No level with index " + std::to_string(index);
        return false;
    }
    const LevelEntry& entry = levels_[index];
    std::error_code ec;
    if (entry.path == customLevelPath() && !std::filesystem::exists(entry.path, ec)) {
        world_.load(LevelData{});
        lastError_.clear();
        return true;
    }
    try {
        world_.load(loader_.loadFile(entry.path));
    } catch (const LevelLoadError& error) {
        lastError_ = error.what();
        return false;
    }
    lastError_.clear();
    return true;
}

void Game::openEditor() { editor_.loadFile(customLevelPath()); }

bool Game::saveEditorLevel() const {
    std::error_code ec;
    std::filesystem::create_directories(levelDir_, ec);
    return editor_.saveFile(customLevelPath());
}

std::filesystem::path Game::customLevelPath() const { return levelDir_ / kCustomLevelName; }

}  // namespace mario
