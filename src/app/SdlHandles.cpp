#include "SdlHandles.hpp"

namespace mario::app {

SdlSystem::SdlSystem() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw SdlError("SDL_Init failed");
    }
    if (TTF_Init() != 0) {
        SDL_Quit();
        throw SdlError("TTF_Init failed");
    }
    constexpr auto png = static_cast<unsigned>(IMG_INIT_PNG);
    constexpr auto webp = static_cast<unsigned>(IMG_INIT_WEBP);
    const auto loaded = static_cast<unsigned>(IMG_Init(static_cast<int>(png | webp)));
    if ((loaded & png) == 0U) {
        TTF_Quit();
        SDL_Quit();
        throw SdlError("IMG_Init failed");
    }
}

SdlSystem::~SdlSystem() {
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

}  // namespace mario::app
