#include "mario/core/Entities.hpp"

namespace mario {

void Player::respawn() noexcept {
    x = config::kPlayerInitialX;
    y = (config::kMapHeight - config::kPlayerInitialYOffsetTiles) * config::kTileSize - h;
    velocityY = 0.0F;
    hasMushroom = false;
    hasFlower = false;
    onGround = false;
    supportingPlatform.reset();
}

void Player::resetForNewLevel() noexcept {
    w = config::kTileSize;
    h = config::kTileSize;
    speed = config::kPlayerSpeed;
    lives = config::kInitialLives;
    score = 0;
    facing = Direction::Right;
    respawn();
}

}  // namespace mario
