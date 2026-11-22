# Design notes {#design}

## Goal

Port an existing single-file C platformer (`main.c`, SDL2) to C++17 without changing how the
game plays, and use the port to apply production practices: a modular structure, an
automated build, tests, static and dynamic analysis, documentation and version control.

## Requirements

Functional requirements carried over from the C version:

| ID | Requirement | Verified by |
| --- | --- | --- |
| F1 | Load levels from 10 x 141 character text files | `LevelLoaderTest`, `ShippedLevelsTest` |
| F2 | Player walks, jumps, collides with blocks and is clamped to the level | `WorldPhysicsTest` |
| F3 | Enemies walk, turn at walls and ledges, die when stomped or burnt | `WorldInteractionTest` |
| F4 | Coins, mushroom and flower power-ups, fireballs | `WorldInteractionTest` |
| F5 | Moving platforms carry the player | `PlatformTest`, `WorldPhysicsTest` |
| F6 | Lives, respawn, game over and level completion via the flag | `WorldPhysicsTest`, `GameStateTest` |
| F7 | Menu, level selection, pause, game over and congratulation screens | `GameStateTest` |
| F8 | Level editor with placing, erasing, scrolling and saving | `EditorModelTest`, `GameStateTest` |

Non-functional requirements:

- the game logic builds and is testable without SDL2 or a display
- a missing or malformed level never terminates the program
- no manual resource management: every SDL object is released by RAII
- reproducible out-of-source builds with CMake presets on Ubuntu 24.04 LTS

## Architecture

```
            +--------------------------- super_mario (executable) ---------------------------+
            |  main.cpp -> Application (composition root)                                    |
            |     SdlSystem / unique_ptr deleters (RAII)                                      |
            |     SdlRenderer  : Renderer   (Adapter)                                         |
            |     SdlInput     -> InputEvent / InputState                                     |
            +----------------------------------------|---------------------------------------+
                                                     | depends on interfaces only
            +--------------------------- mario_core (static library) ------------------------+
            |  Game (State context) --owns--> GameState subclasses                           |
            |      |                                                                          |
            |      +-- World (simulation) -- Player, Enemy, Powerup, Fireball, Flag           |
            |      |       |                  Platform -- PlatformMotion (Strategy)           |
            |      |       +-- TileMap, Camera                                                |
            |      |       +-- EventDispatcher (Observer)                                     |
            |      +-- LevelLoader -- EntityFactory (Factory)                                 |
            |      +-- EditorModel                                                            |
            |  Renderer / Input interfaces, renderWorld()                                     |
            +--------------------------------------------------------------------------------+
```

The dependency only points from the frontend to the core. The core never includes an SDL header,
so the unit tests link against `mario_core` alone and draw into a recording `FakeRenderer`.

## Design patterns

| Pattern | Where | Problem it solves | Trade-off |
| --- | --- | --- | --- |
| State | `Game`, `GameState`, `States.hpp` | `main.c` had a `switch (game_state)` for input and another for rendering; every new screen touched both. Each screen is now one class with its own data (menu selection, freeze timer). | More classes; transitions are deferred to avoid destroying the running state. |
| Strategy | `PlatformMotion`, `HorizontalMotion`, `VerticalMotion` | Platforms differed only by a type code checked in the update loop. The motion rule is now injected. | Virtual call per platform per frame, negligible for at most 16 platforms. |
| Factory | `EntityFactory` | The loader had an if/else chain per character. Creators are registered per tile code, so the parser does not change when a tile type is added; tests can swap creators. | `std::function` indirection during loading only. |
| Observer | `EventDispatcher`, `GameEvent` | Gameplay code should not know about logging (and later sound or statistics). `World` publishes events, the frontend subscribes a logger in verbose mode. | Listeners must not outlive captured objects; ids allow unsubscribing. |
| Adapter | `SdlRenderer` implements `Renderer` | The core needs to draw without depending on SDL. | One extra interface. |
| RAII | `SdlSystem`, `WindowPtr`, `TexturePtr`, `FontPtr` | The C code had several hand written cleanup chains and opened fonts every frame. | None worth noting. |
| Dependency injection | `World(EventDispatcher&)`, `Game::render(Renderer&)`, `LevelLoader(EntityFactory)` | Collaborators are passed in, which is what makes the fakes in the tests possible. | Callers must keep injected objects alive (documented). |

Patterns that were considered and left out: a Command layer for input (the key bindings are
simple and fixed) and an entity-component system (the number of entity types is small and stable).

## Notable changes compared to the C version

- `exit(1)` on a missing level file became `LevelLoadError`, shown on the level select screen;
  `map3.txt` was listed but did not exist, the unused `map.txt` now fills that slot
- blocking `SDL_Delay` calls for respawn and level completion became timers in the states
- fonts are cached per size instead of being opened and closed every frame
- global mutable state (map arrays, counters, textures, renderer) is gone; data is owned by
  `World`, `EditorModel` and `SdlRenderer`
- two sprites were WebP files named `.png`; they carry the correct extension now
- user interface texts are in English
- the physics (speeds, gravity, collision order) is intentionally unchanged, including the
  0 / 0.5 velocity oscillation of a player standing on the ground

## Verification and its limits

- Unit tests cover the core library: 94% of lines and 99% of functions according to gcovr
  (`coverage` preset). Branch coverage is lower (about 70%), mainly defensive bounds checks.
- Integration tests load every shipped level, check that it has a goal and ground under the
  spawn point, and run a 3000 frame scripted session per level.
- Smoke tests start the real executable with the SDL dummy video driver, so sprite and font
  loading and the main loop are exercised in CI without a display.
- The whole suite runs under AddressSanitizer and UndefinedBehaviorSanitizer.
- Not covered automatically: how the game looks and feels. Rendering through SDL is checked by
  the smoke test only for crashes; visual correctness was checked manually.
- Level completability is not verified; the tests only prove that levels load and can be played
  without crashing.
