#include "Application.hpp"

#include <iostream>
#include <memory>
#include <utility>

#include "SdlHandles.hpp"
#include "SdlInput.hpp"
#include "SdlRenderer.hpp"
#include "mario/core/Config.hpp"
#include "mario/states/Game.hpp"

namespace mario::app {

namespace {

struct SdlFree {
    void operator()(char* text) const noexcept { SDL_free(text); }
};

}  // namespace

Application::Application(CommandLineOptions options) : options_(std::move(options)) {}

std::filesystem::path Application::resolveAssetDir() const {
    if (options_.assetDir) {
        return *options_.assetDir;
    }
    const std::unique_ptr<char, SdlFree> base{SDL_GetBasePath()};
    if (base) {
        return std::filesystem::path(base.get()) / "assets";
    }
    return "assets";
}

int Application::run() {
    const SdlSystem sdl;
    const std::filesystem::path assetDir = resolveAssetDir();

    const WindowPtr window{SDL_CreateWindow("Super Mario", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                            config::kWindowWidth, config::kWindowHeight, SDL_WINDOW_SHOWN)};
    if (!window) {
        throw SdlError("SDL_CreateWindow failed");
    }
    SdlRenderer renderer(*window, assetDir);
    Game game(assetDir);

    if (options_.verbose) {
        std::clog << "Assets: " << assetDir.string() << '\n';
        game.events().subscribe([](const GameEvent& event) {
            std::clog << "[event] " << toString(event.type) << ' ' << event.value << '\n';
        });
    }

    Uint32 previous = SDL_GetTicks();
    int frame = 0;
    while (game.running()) {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                game.requestQuit();
            } else if (const auto input = translateEvent(event)) {
                game.handleEvent(*input);
            }
        }

        const Uint32 now = SDL_GetTicks();
        game.update(readInputState(), static_cast<int>(now - previous));
        previous = now;

        game.render(renderer);
        renderer.present();

        ++frame;
        if (options_.frameLimit && frame >= *options_.frameLimit) {
            game.requestQuit();
        }
        SDL_Delay(config::kFrameDelayMs);
    }
    return 0;
}

}  // namespace mario::app
