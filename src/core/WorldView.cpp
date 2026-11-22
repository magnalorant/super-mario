#include "mario/core/WorldView.hpp"

namespace mario {

namespace {

constexpr int kTile = config::kTileSize;

Rect toScreen(const Rect& world, int cameraX) noexcept {
    return world.translated(-cameraX, config::kRenderOffsetY);
}

void drawIfVisible(Renderer& renderer, Sprite sprite, const Rect& screen) {
    if (screen.visibleHorizontally(config::kWindowWidth)) {
        renderer.drawSprite(sprite, screen);
    }
}

void renderTiles(const World& world, Renderer& renderer, int cameraX) {
    const TileMap& tiles = world.tiles();
    for (int ty = 0; ty < TileMap::kHeight; ++ty) {
        for (int tx = 0; tx < TileMap::kWidth; ++tx) {
            const Rect cell = toScreen({tx * kTile, ty * kTile, kTile, kTile}, cameraX);
            if (tiles.isSolid(tx, ty)) {
                const Sprite sprite = ty >= TileMap::kHeight - 2 ? Sprite::Ground : Sprite::Brick;
                drawIfVisible(renderer, sprite, cell);
            }
        }
    }
    for (int ty = 0; ty < TileMap::kHeight; ++ty) {
        for (int tx = 0; tx < TileMap::kWidth; ++tx) {
            if (tiles.hasCoin(tx, ty)) {
                drawIfVisible(renderer, Sprite::Coin,
                              toScreen({tx * kTile, ty * kTile, kTile, kTile}, cameraX));
            }
        }
    }
}

void renderFlag(const Flag& flag, Renderer& renderer, int cameraX) {
    constexpr int poleHeight = config::kPoleHeightTiles * kTile;
    const int top = flag.y + config::kRenderOffsetY - (poleHeight - kTile);
    const Rect pole{flag.x - cameraX + kTile / 2 - 2, top, 4, poleHeight};
    const Rect cloth{flag.x - cameraX + kTile / 2 + 2, top, kTile, kTile / 2};
    if (pole.visibleHorizontally(config::kWindowWidth)) {
        renderer.fillRect(pole, palette::kFlagPole);
        renderer.fillRect(cloth, palette::kYellow);
    }
}

}  // namespace

std::string hudText(const Player& player) {
    return "Score: " + std::to_string(player.score) + "   Lives: " + std::to_string(player.lives);
}

void renderWorld(const World& world, Renderer& renderer) {
    const int cameraX = world.camera().x();
    renderer.clear(palette::kSky);

    renderTiles(world, renderer, cameraX);

    for (const Powerup& powerup : world.powerups()) {
        if (powerup.active) {
            const Sprite sprite = powerup.kind == PowerupKind::Mushroom ? Sprite::Mushroom : Sprite::Flower;
            drawIfVisible(renderer, sprite, toScreen(powerup.bounds(), cameraX));
        }
    }
    for (const Enemy& enemy : world.enemies()) {
        if (enemy.alive) {
            drawIfVisible(renderer, Sprite::Enemy, toScreen(enemy.bounds(), cameraX));
        }
    }
    for (const Fireball& fireball : world.fireballs()) {
        if (fireball.active) {
            drawIfVisible(renderer, Sprite::Fireball, toScreen(fireball.bounds(), cameraX));
        }
    }
    if (world.flag()) {
        renderFlag(*world.flag(), renderer, cameraX);
    }
    for (const Platform& platform : world.platforms()) {
        drawIfVisible(renderer, Sprite::MovingPlatform, toScreen(platform.bounds(), cameraX));
    }
    drawIfVisible(renderer, Sprite::Player, toScreen(world.player().bounds(), cameraX));

    renderer.drawText(hudText(world.player()), 20, 20, palette::kWhite, kHudPointSize);
}

void renderDimOverlay(Renderer& renderer, std::uint8_t alpha) {
    renderer.fillRect({0, 0, config::kWindowWidth, config::kWindowHeight}, Color{0, 0, 0, alpha});
}

}  // namespace mario
