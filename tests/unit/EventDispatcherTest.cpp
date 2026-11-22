#include <gtest/gtest.h>

#include <vector>

#include "mario/core/EventDispatcher.hpp"

namespace mario {
namespace {

TEST(EventDispatcherTest, DeliversEventsToAllListenersInOrder) {
    EventDispatcher dispatcher;
    std::vector<int> calls;
    dispatcher.subscribe([&](const GameEvent& e) { calls.push_back(e.value); });
    dispatcher.subscribe([&](const GameEvent& e) { calls.push_back(e.value * 10); });

    dispatcher.publish({GameEventType::CoinCollected, 3});

    EXPECT_EQ(calls, (std::vector<int>{3, 30}));
}

TEST(EventDispatcherTest, UnsubscribedListenerIsNotCalled) {
    EventDispatcher dispatcher;
    int count = 0;
    const auto id = dispatcher.subscribe([&](const GameEvent&) { ++count; });
    EXPECT_TRUE(dispatcher.unsubscribe(id));
    EXPECT_FALSE(dispatcher.unsubscribe(id));

    dispatcher.publish({GameEventType::GameOver, 0});

    EXPECT_EQ(count, 0);
    EXPECT_EQ(dispatcher.listenerCount(), 0U);
}

TEST(EventDispatcherTest, ListenerMayUnsubscribeItselfDuringPublish) {
    EventDispatcher dispatcher;
    int count = 0;
    EventDispatcher::SubscriptionId id = 0;
    id = dispatcher.subscribe([&](const GameEvent&) {
        ++count;
        dispatcher.unsubscribe(id);
    });

    dispatcher.publish({GameEventType::PlayerDied, 1});
    dispatcher.publish({GameEventType::PlayerDied, 0});

    EXPECT_EQ(count, 1);
}

TEST(EventDispatcherTest, PublishWithoutListenersIsHarmless) {
    const EventDispatcher dispatcher;
    EXPECT_NO_THROW(dispatcher.publish({GameEventType::LevelCompleted, 7}));
}

TEST(EventDispatcherTest, EveryEventTypeHasAName) {
    for (const auto type :
         {GameEventType::CoinCollected, GameEventType::EnemyDefeated, GameEventType::PowerupCollected,
          GameEventType::BlockBroken, GameEventType::FireballShot, GameEventType::PlayerDied,
          GameEventType::GameOver, GameEventType::LevelCompleted}) {
        EXPECT_NE(toString(type), "Unknown");
    }
}

}  // namespace
}  // namespace mario
