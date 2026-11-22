#pragma once

/**
 * @file EventDispatcher.hpp
 * @brief Observer pattern used to report gameplay events without coupling to their consumers.
 */

#include <cstddef>
#include <functional>
#include <string_view>
#include <utility>
#include <vector>

namespace mario {

/// @brief Kinds of events raised by the simulation.
enum class GameEventType {
    CoinCollected,     ///< A coin was picked up. Value: points awarded.
    EnemyDefeated,     ///< An enemy was stomped or burnt. Value: points awarded.
    PowerupCollected,  ///< A mushroom or flower was picked up. Value: 0 mushroom, 1 flower.
    BlockBroken,       ///< A block was destroyed from below. Value: tile column.
    FireballShot,      ///< A fireball was launched. Value: direction sign.
    PlayerDied,        ///< The player lost a life. Value: lives left.
    GameOver,          ///< The player has no lives left. Value: final score.
    LevelCompleted,    ///< The goal flag was reached. Value: final score.
};

/// @return Stable name of the event type, used in logs.
[[nodiscard]] std::string_view toString(GameEventType type) noexcept;

/**
 * @brief A single notification published by the simulation.
 */
struct GameEvent {
    GameEventType type{GameEventType::CoinCollected};  ///< What happened.
    int value{0};  ///< Event specific payload, see GameEventType.
};

/**
 * @brief Subject of the Observer pattern.
 *
 * The world publishes events, while logging, sound or statistics code subscribes to them.
 * Neither side knows about the other, which keeps the simulation free of presentation code.
 * Listeners may subscribe or unsubscribe from within a callback; such changes take effect
 * from the next publish call.
 */
class EventDispatcher {
public:
    /// @brief Callback invoked for every published event.
    using Listener = std::function<void(const GameEvent&)>;
    /// @brief Handle used to cancel a subscription.
    using SubscriptionId = std::size_t;

    /**
     * @brief Adds a listener.
     * @return Identifier that can be passed to unsubscribe().
     */
    SubscriptionId subscribe(Listener listener);

    /**
     * @brief Removes a listener.
     * @return False if the identifier is unknown.
     */
    bool unsubscribe(SubscriptionId id);

    /// @brief Delivers an event to every listener in subscription order.
    void publish(const GameEvent& event) const;

    /// @return Number of active listeners.
    [[nodiscard]] std::size_t listenerCount() const noexcept { return listeners_.size(); }

private:
    std::vector<std::pair<SubscriptionId, Listener>> listeners_;
    SubscriptionId nextId_{1};
};

}  // namespace mario
