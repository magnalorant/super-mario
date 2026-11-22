#pragma once

/**
 * @file SdlRenderer.hpp
 * @brief SDL2 implementation of the core Renderer interface.
 */

#include <array>
#include <filesystem>
#include <string_view>
#include <unordered_map>

#include "SdlHandles.hpp"
#include "mario/core/Renderer.hpp"

namespace mario::app {

/**
 * @brief Adapter from the core Renderer interface to SDL_Renderer, SDL_image and SDL_ttf.
 *
 * Sprites are loaded once at construction. Fonts are opened lazily and cached per point size,
 * whereas the C version opened and closed font files on every frame.
 */
class SdlRenderer final : public Renderer {
public:
    /**
     * @brief Creates a renderer for a window and loads all sprites.
     * @param window Target window, must outlive the renderer.
     * @param assetDir Directory containing sprites/ and fonts/.
     * @throws SdlError if the renderer or a sprite cannot be created.
     */
    SdlRenderer(SDL_Window& window, const std::filesystem::path& assetDir);

    void clear(Color color) override;
    void fillRect(const Rect& area, Color color) override;
    void outlineRect(const Rect& area, Color color) override;
    void drawSprite(Sprite sprite, const Rect& area) override;
    void drawText(std::string_view text, int x, int y, Color color, int pointSize) override;
    void drawTextCentered(std::string_view text, int y, Color color, Color shadow, int pointSize,
                          int shadowOffset) override;

    /// @brief Shows everything drawn since the last call.
    void present();

private:
    static constexpr std::size_t kSpriteCount = static_cast<std::size_t>(Sprite::MovingPlatform) + 1;

    struct TextImage {
        TexturePtr texture;
        int width{0};
        int height{0};
    };

    [[nodiscard]] TTF_Font* font(int pointSize);
    [[nodiscard]] TextImage renderText(std::string_view text, Color color, int pointSize);
    void blit(TextImage& image, int x, int y);
    void setColor(Color color);

    RendererPtr renderer_;
    std::array<TexturePtr, kSpriteCount> sprites_;
    std::filesystem::path fontPath_;
    std::unordered_map<int, FontPtr> fonts_;
};

}  // namespace mario::app
