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
    constexpr int imageFormats = IMG_INIT_PNG | IMG_INIT_WEBP;
    if ((IMG_Init(imageFormats) & IMG_INIT_PNG) == 0) {
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
