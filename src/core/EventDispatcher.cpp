#include "mario/core/EventDispatcher.hpp"

#include <algorithm>

namespace mario {

std::string_view toString(GameEventType type) noexcept {
    switch (type) {
        case GameEventType::CoinCollected:
            return "CoinCollected";
        case GameEventType::EnemyDefeated:
            return "EnemyDefeated";
        case GameEventType::PowerupCollected:
            return "PowerupCollected";
        case GameEventType::BlockBroken:
            return "BlockBroken";
        case GameEventType::FireballShot:
            return "FireballShot";
        case GameEventType::PlayerDied:
            return "PlayerDied";
        case GameEventType::GameOver:
            return "GameOver";
        case GameEventType::LevelCompleted:
            return "LevelCompleted";
    }
    return "Unknown";
}

EventDispatcher::SubscriptionId EventDispatcher::subscribe(Listener listener) {
    const SubscriptionId id = nextId_++;
    listeners_.emplace_back(id, std::move(listener));
    return id;
}

bool EventDispatcher::unsubscribe(SubscriptionId id) {
    const auto it = std::find_if(listeners_.begin(), listeners_.end(),
                                 [id](const auto& entry) { return entry.first == id; });
    if (it == listeners_.end()) {
        return false;
    }
    listeners_.erase(it);
    return true;
}

void EventDispatcher::publish(const GameEvent& event) const {
    const auto snapshot = listeners_;
    for (const auto& entry : snapshot) {
        entry.second(event);
    }
}

}  // namespace mario
