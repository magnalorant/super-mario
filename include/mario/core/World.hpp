#pragma once

/**
 * @file World.hpp
 * @brief Gameplay simulation of a single level.
 */

#include <array>
#include <optional>
#include <vector>

#include "mario/core/Camera.hpp"
#include "mario/core/Entities.hpp"
#include "mario/core/EventDispatcher.hpp"
#include "mario/core/LevelData.hpp"
#include "mario/core/Platform.hpp"
#include "mario/core/TileMap.hpp"

namespace mario {

/**
 * @brief Player intent for one simulation step, independent of any input library.
 */
struct PlayerInput {
    bool moveLeft{false};     ///< Left is held.
    bool moveRight{false};    ///< Right is held.
    bool jumpPressed{false};  ///< Jump was pressed during this frame.
    bool jumpHeld{false};     ///< A jump key is currently held.
};

/// @brief Result of advancing the simulation by one frame.
enum class StepOutcome {
    Running,         ///< Nothing special happened.
    LifeLost,        ///< The player lost a life and has been respawned.
    GameOver,        ///< The player lost the last life.
    LevelCompleted,  ///< The player touched the goal flag.
};

/**
 * @brief Owns all level state and advances it frame by frame.
 *
 * The simulation is deterministic and has no dependency on SDL, so it can be driven by unit
 * tests with scripted input. Gameplay events are published through an injected
 * EventDispatcher.
 */
class World {
public:
    /// @brief Fixed-size pool of fireballs.
    using FireballPool = std::array<Fireball, config::kMaxFireballs>;

    /**
     * @brief Creates an empty world.
     * @param events Dispatcher receiving gameplay events. Must outlive the world.
     */
    explicit World(EventDispatcher& events);

    /// @brief Replaces the current level and resets the player.
    void load(LevelData level);

    /**
     * @brief Advances the simulation by one frame.
     * @param input Player intent for this frame.
     */
    StepOutcome step(const PlayerInput& input);

    /**
     * @brief Launches a fireball in the facing direction.
     * @return False if the player has no flower or every fireball is in flight.
     */
    bool shootFireball();

    /// @name Accessors
    /// @{
    [[nodiscard]] const Player& player() const noexcept { return player_; }
    [[nodiscard]] Player& player() noexcept { return player_; }
    [[nodiscard]] const TileMap& tiles() const noexcept { return tiles_; }
    [[nodiscard]] const std::vector<Enemy>& enemies() const noexcept { return enemies_; }
    [[nodiscard]] std::vector<Enemy>& enemies() noexcept { return enemies_; }
    [[nodiscard]] const std::vector<Powerup>& powerups() const noexcept { return powerups_; }
    [[nodiscard]] const std::vector<Platform>& platforms() const noexcept { return platforms_; }
    [[nodiscard]] const FireballPool& fireballs() const noexcept { return fireballs_; }
    [[nodiscard]] const std::optional<Flag>& flag() const noexcept { return flag_; }
    [[nodiscard]] const Camera& camera() const noexcept { return camera_; }
    /// @}

private:
    void applyHorizontalMovement(const PlayerInput& input) noexcept;
    void resolveHorizontalTileCollisions() noexcept;
    void clampPlayerToWorld() noexcept;
    [[nodiscard]] bool standingOnSolidTile() const noexcept;
    void applyGravityAndJump(bool jumpPressed) noexcept;
    void landOnPlatforms(int previousY) noexcept;
    void resolveVerticalTileCollisions();
    void movePlatforms();
    void rideSupportingPlatform(const PlayerInput& input);
    void collectCoins();
    void collectPowerups();
    void moveEnemies() noexcept;
    StepOutcome resolveEnemyContacts();
    void updateFireballs();
    StepOutcome checkFallOutOfWorld();
    [[nodiscard]] bool reachedFlag() const noexcept;
    StepOutcome loseLife();
    void publish(GameEventType type, int value) const;

    EventDispatcher& events_;
    TileMap tiles_;
    Player player_;
    std::vector<Enemy> enemies_;
    std::vector<Powerup> powerups_;
    std::vector<Platform> platforms_;
    FireballPool fireballs_;
    std::optional<Flag> flag_;
    Camera camera_;
};

}  // namespace mario
