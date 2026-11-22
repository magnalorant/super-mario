#pragma once

/**
 * @file SdlHandles.hpp
 * @brief RAII ownership of SDL resources.
 */

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <memory>
#include <stdexcept>
#include <string>

namespace mario::app {

/// @brief Raised when an SDL call fails. The message includes the SDL error string.
class SdlError : public std::runtime_error {
public:
    /// @param what Description of the failed operation.
    explicit SdlError(const std::string& what) : std::runtime_error(what + ": " + SDL_GetError()) {}
};

/// @brief Releases SDL objects with the matching destroy function.
struct SdlDeleter {
    /// @brief Destroys a window.
    void operator()(SDL_Window* window) const noexcept { SDL_DestroyWindow(window); }
    /// @brief Destroys a renderer.
    void operator()(SDL_Renderer* renderer) const noexcept { SDL_DestroyRenderer(renderer); }
    /// @brief Destroys a texture.
    void operator()(SDL_Texture* texture) const noexcept { SDL_DestroyTexture(texture); }
    /// @brief Frees a surface.
    void operator()(SDL_Surface* surface) const noexcept { SDL_FreeSurface(surface); }
    /// @brief Closes a font.
    void operator()(TTF_Font* font) const noexcept { TTF_CloseFont(font); }
};

using WindowPtr = std::unique_ptr<SDL_Window, SdlDeleter>;      ///< Owning window handle.
using RendererPtr = std::unique_ptr<SDL_Renderer, SdlDeleter>;  ///< Owning renderer handle.
using TexturePtr = std::unique_ptr<SDL_Texture, SdlDeleter>;    ///< Owning texture handle.
using SurfacePtr = std::unique_ptr<SDL_Surface, SdlDeleter>;    ///< Owning surface handle.
using FontPtr = std::unique_ptr<TTF_Font, SdlDeleter>;          ///< Owning font handle.

/**
 * @brief Initialises SDL, SDL_image and SDL_ttf for the lifetime of the object.
 *
 * Replaces the hand written cleanup sequences of the C version: every subsystem that was
 * started is shut down in reverse order, also when construction fails half way.
 */
class SdlSystem {
public:
    /// @throws SdlError if a subsystem cannot be initialised.
    SdlSystem();
    ~SdlSystem();
    SdlSystem(const SdlSystem&) = delete;
    SdlSystem& operator=(const SdlSystem&) = delete;
    SdlSystem(SdlSystem&&) = delete;
    SdlSystem& operator=(SdlSystem&&) = delete;
};

}  // namespace mario::app
