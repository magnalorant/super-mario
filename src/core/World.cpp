#include "mario/core/World.hpp"

#include <utility>

namespace mario {

namespace {

constexpr int kTile = config::kTileSize;

constexpr int tileOf(int pixel) noexcept {
    return pixel / kTile;
}

}  // namespace

World::World(EventDispatcher& events) : events_(events) {
    player_.resetForNewLevel();
}

void World::load(LevelData level) {
    tiles_ = level.tiles;
    enemies_ = std::move(level.enemies);
    powerups_ = std::move(level.powerups);
    platforms_ = std::move(level.platforms);
    flag_ = level.flag;
    fireballs_.fill(Fireball{});
    player_.resetForNewLevel();
    camera_.reset();
}

StepOutcome World::step(const PlayerInput& input) {
    const int previousY = player_.y;

    applyHorizontalMovement(input);
    resolveHorizontalTileCollisions();
    clampPlayerToWorld();

    applyGravityAndJump(input.jumpPressed);
    landOnPlatforms(previousY);
    resolveVerticalTileCollisions();

    movePlatforms();
    rideSupportingPlatform(input);
    clampPlayerToWorld();

    collectCoins();
    collectPowerups();

    moveEnemies();

    StepOutcome outcome = StepOutcome::Running;
    const StepOutcome contact = resolveEnemyContacts();
    if (contact == StepOutcome::GameOver) {
        return contact;
    }
    if (contact == StepOutcome::LifeLost) {
        outcome = contact;
    }

    updateFireballs();

    const StepOutcome fall = checkFallOutOfWorld();
    if (fall == StepOutcome::GameOver) {
        return fall;
    }
    if (fall == StepOutcome::LifeLost) {
        outcome = fall;
    }

    if (reachedFlag()) {
        publish(GameEventType::LevelCompleted, player_.score);
        return StepOutcome::LevelCompleted;
    }

    camera_.follow(player_.x);
    return outcome;
}

bool World::shootFireball() {
    if (!player_.hasFlower) {
        return false;
    }
    for (Fireball& fireball : fireballs_) {
        if (!fireball.active) {
            fireball.x = player_.x + player_.w / 2 - config::kFireballSize / 2;
            fireball.y = player_.y + player_.h / 2 - config::kFireballSize / 2;
            fireball.direction = player_.facing;
            fireball.active = true;
            publish(GameEventType::FireballShot, sign(fireball.direction));
            return true;
        }
    }
    return false;
}

void World::applyHorizontalMovement(const PlayerInput& input) noexcept {
    if (input.moveLeft) {
        player_.x -= player_.speed;
        player_.facing = Direction::Left;
    }
    if (input.moveRight) {
        player_.x += player_.speed;
        player_.facing = Direction::Right;
    }
}

void World::resolveHorizontalTileCollisions() noexcept {
    const int left = tileOf(player_.x);
    const int right = tileOf(player_.x + player_.w - 1);
    const int top = tileOf(player_.y);
    const int bottom = tileOf(player_.y + player_.h - 1);

    for (int ty = top; ty <= bottom; ++ty) {
        if (ty < 0 || ty >= TileMap::kHeight) {
            continue;
        }
        if (tiles_.isSolid(left, ty)) {
            player_.x = (left + 1) * kTile;
        }
        if (tiles_.isSolid(right, ty)) {
            player_.x = right * kTile - player_.w;
        }
    }
}

void World::clampPlayerToWorld() noexcept {
    if (player_.x < 0) {
        player_.x = 0;
    }
    if (player_.x + player_.w > config::kWorldPixelWidth) {
        player_.x = config::kWorldPixelWidth - player_.w;
    }
}

bool World::standingOnSolidTile() const noexcept {
    const int left = tileOf(player_.x);
    const int right = tileOf(player_.x + player_.w - 1);
    const int below = tileOf(player_.y + player_.h);
    for (int tx = left; tx <= right; ++tx) {
        if (tiles_.isSolid(tx, below)) {
            return true;
        }
    }
    return false;
}

void World::applyGravityAndJump(bool jumpPressed) noexcept {
    player_.onGround = standingOnSolidTile();
    if (jumpPressed && (player_.onGround || player_.onPlatform())) {
        player_.velocityY = config::kJumpStrength;
        player_.onGround = false;
        player_.supportingPlatform.reset();
    }
    if (!player_.onPlatform()) {
        player_.velocityY += config::kGravity;
    }
    player_.y += static_cast<int>(player_.velocityY);
}

void World::landOnPlatforms(int previousY) noexcept {
    for (std::size_t i = 0; i < platforms_.size(); ++i) {
        const Platform& platform = platforms_[i];
        const int feet = player_.y + player_.h;
        const bool falling = player_.velocityY >= 0.0F;
        const bool wasAbove = previousY + player_.h <= platform.y;
        const bool crossedTop = feet >= platform.y && feet < platform.y + kTile / 2;
        const bool overlapsX = player_.x + player_.w > platform.x && player_.x < platform.x + platform.w;
        if (falling && wasAbove && crossedTop && overlapsX) {
            player_.y = platform.y - player_.h;
            player_.velocityY = 0.0F;
            player_.onGround = false;
            player_.supportingPlatform = i;
            return;
        }
    }
}

void World::resolveVerticalTileCollisions() {
    if (player_.onPlatform()) {
        return;
    }
    const int left = tileOf(player_.x);
    const int right = tileOf(player_.x + player_.w - 1);
    const int top = tileOf(player_.y);
    const int bottom = tileOf(player_.y + player_.h - 1);

    for (int tx = left; tx <= right; ++tx) {
        if (tx < 0 || tx >= TileMap::kWidth) {
            continue;
        }
        if (tiles_.isSolid(tx, top) && player_.velocityY < 0.0F) {
            if (player_.hasMushroom) {
                tiles_.setSolid(tx, top, false);
                publish(GameEventType::BlockBroken, tx);
            }
            player_.y = (top + 1) * kTile;
            player_.velocityY = 0.0F;
        }
        if (tiles_.isSolid(tx, bottom) && player_.velocityY >= 0.0F) {
            player_.y = bottom * kTile - player_.h;
            player_.velocityY = 0.0F;
            player_.onGround = true;
        }
    }
}

void World::movePlatforms() {
    for (Platform& platform : platforms_) {
        platform.advance();
    }
}

void World::rideSupportingPlatform(const PlayerInput& input) {
    const std::optional<std::size_t> index = player_.supportingPlatform;
    if (!index) {
        return;
    }
    const Platform& platform = platforms_[*index];
    const int feet = player_.y + player_.h;
    const bool overlapsX = player_.x + player_.w > platform.x && player_.x < platform.x + platform.w;
    const bool onTop = feet >= platform.y && feet < platform.y + kTile / 2;
    if (!overlapsX || !onTop) {
        player_.supportingPlatform.reset();
        return;
    }
    if (!input.jumpPressed && !input.jumpHeld) {
        player_.y = platform.y - player_.h;
        player_.velocityY = 0.0F;
        player_.x += platform.carryOffsetX();
    }
}

void World::collectCoins() {
    const int tx = tileOf(player_.x + player_.w / 2);
    const int ty = tileOf(player_.y + player_.h / 2);
    if (tiles_.takeCoin(tx, ty)) {
        player_.score += config::kPointsPerCoin;
        publish(GameEventType::CoinCollected, config::kPointsPerCoin);
    }
}

void World::collectPowerups() {
    const Rect body = player_.bounds();
    for (Powerup& powerup : powerups_) {
        if (!powerup.active || !body.intersects(powerup.bounds())) {
            continue;
        }
        if (powerup.kind == PowerupKind::Mushroom) {
            player_.hasMushroom = true;
            player_.lives += config::kMushroomLivesBonus;
        } else {
            player_.hasFlower = true;
        }
        powerup.active = false;
        publish(GameEventType::PowerupCollected, powerup.kind == PowerupKind::Mushroom ? 0 : 1);
    }
}

void World::moveEnemies() noexcept {
    for (Enemy& enemy : enemies_) {
        if (!enemy.alive) {
            continue;
        }
        enemy.x += sign(enemy.direction) * config::kEnemySpeed;

        const int aheadX =
            enemy.direction == Direction::Right ? tileOf(enemy.x + enemy.w) : tileOf(enemy.x - 1);
        const int feetY = tileOf(enemy.y + enemy.h - 1);
        const int belowY = feetY + 1;

        const bool outsideMap = aheadX < 0 || aheadX >= TileMap::kWidth;
        const bool wallAhead = tiles_.isSolid(aheadX, feetY);
        const bool ledgeAhead =
            TileMap::inBounds(aheadX, belowY) && !tiles_.isSolid(aheadX, belowY) && !wallAhead;
        if (outsideMap || wallAhead || ledgeAhead) {
            enemy.direction = reversed(enemy.direction);
        }
    }
}

StepOutcome World::resolveEnemyContacts() {
    const Rect body = player_.bounds();
    for (Enemy& enemy : enemies_) {
        if (!enemy.alive || !body.intersects(enemy.bounds())) {
            continue;
        }
        const int penetration = (player_.y + player_.h) - enemy.y;
        const bool stomp = player_.velocityY > 0.0F &&
                           penetration <= player_.h / 2 + config::kStompTolerance && !player_.onPlatform();
        if (!stomp) {
            return loseLife();
        }
        enemy.alive = false;
        player_.velocityY = config::kJumpStrength / config::kStompBounceDivisor;
        player_.score += config::kPointsPerEnemyKill;
        publish(GameEventType::EnemyDefeated, config::kPointsPerEnemyKill);
    }
    return StepOutcome::Running;
}

void World::updateFireballs() {
    for (Fireball& fireball : fireballs_) {
        if (!fireball.active) {
            continue;
        }
        fireball.x += sign(fireball.direction) * config::kFireballSpeed;
        if (fireball.x < 0 || fireball.x > config::kWorldPixelWidth) {
            fireball.active = false;
            continue;
        }
        const Rect shot = fireball.bounds();
        for (Enemy& enemy : enemies_) {
            if (enemy.alive && shot.intersects(enemy.bounds())) {
                enemy.alive = false;
                fireball.active = false;
                player_.score += config::kPointsPerEnemyKill;
                publish(GameEventType::EnemyDefeated, config::kPointsPerEnemyKill);
                break;
            }
        }
    }
}

StepOutcome World::checkFallOutOfWorld() {
    if (player_.y + player_.h >= config::kWorldPixelHeight + kTile) {
        return loseLife();
    }
    return StepOutcome::Running;
}

bool World::reachedFlag() const noexcept {
    return flag_ && player_.bounds().intersects(flag_->bounds());
}

StepOutcome World::loseLife() {
    --player_.lives;
    publish(GameEventType::PlayerDied, player_.lives);
    if (player_.lives <= 0) {
        publish(GameEventType::GameOver, player_.score);
        return StepOutcome::GameOver;
    }
    player_.respawn();
    camera_.reset();
    return StepOutcome::LifeLost;
}

void World::publish(GameEventType type, int value) const {
    events_.publish(GameEvent{type, value});
}

}  // namespace mario
