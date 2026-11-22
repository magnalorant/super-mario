#include "mario/core/EntityFactory.hpp"

#include <utility>

namespace mario {

namespace {

constexpr int toPixels(int tile) noexcept {
    return tile * config::kTileSize;
}

template <typename T>
bool belowLimit(const std::vector<T>& items, int limit) {
    return items.size() < static_cast<std::size_t>(limit);
}

}  // namespace

EntityFactory EntityFactory::withDefaultCreators() {
    EntityFactory factory;

    factory.registerCreator(TileCode::Solid,
                            [](LevelData& level, int tx, int ty) { level.tiles.setSolid(tx, ty, true); });

    factory.registerCreator(TileCode::Coin,
                            [](LevelData& level, int tx, int ty) { level.tiles.setCoin(tx, ty, true); });

    factory.registerCreator(TileCode::Enemy, [](LevelData& level, int tx, int ty) {
        if (belowLimit(level.enemies, config::kMaxEnemies)) {
            Enemy enemy;
            enemy.x = toPixels(tx);
            enemy.y = toPixels(ty);
            level.enemies.push_back(enemy);
        }
    });

    factory.registerCreator(TileCode::Flag, [](LevelData& level, int tx, int ty) {
        Flag flag;
        flag.x = toPixels(tx);
        flag.y = toPixels(ty);
        level.flag = flag;
    });

    const auto addPowerup = [](PowerupKind kind) {
        return [kind](LevelData& level, int tx, int ty) {
            if (belowLimit(level.powerups, config::kMaxPowerups)) {
                level.powerups.push_back(Powerup{toPixels(tx), toPixels(ty), kind, true});
            }
        };
    };
    factory.registerCreator(TileCode::Mushroom, addPowerup(PowerupKind::Mushroom));
    factory.registerCreator(TileCode::Flower, addPowerup(PowerupKind::Flower));

    factory.registerCreator(TileCode::HorizontalPlatform, [](LevelData& level, int tx, int ty) {
        if (belowLimit(level.platforms, config::kMaxPlatforms)) {
            level.platforms.push_back(makeHorizontalPlatform(toPixels(tx), toPixels(ty)));
        }
    });
    factory.registerCreator(TileCode::VerticalPlatform, [](LevelData& level, int tx, int ty) {
        if (belowLimit(level.platforms, config::kMaxPlatforms)) {
            level.platforms.push_back(makeVerticalPlatform(toPixels(tx), toPixels(ty)));
        }
    });

    return factory;
}

void EntityFactory::registerCreator(TileCode code, Creator creator) {
    creators_[code] = std::move(creator);
}

bool EntityFactory::canCreate(TileCode code) const {
    return creators_.find(code) != creators_.end();
}

bool EntityFactory::create(char symbol, LevelData& level, int tileX, int tileY) const {
    const auto code = tileCodeFromChar(symbol);
    if (!code) {
        return false;
    }
    const auto it = creators_.find(*code);
    if (it == creators_.end()) {
        return false;
    }
    it->second(level, tileX, tileY);
    return true;
}

}  // namespace mario
