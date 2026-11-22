#include "SdlRenderer.hpp"

#include <iostream>
#include <string>
#include <utility>

#include "mario/core/Config.hpp"
#include "mario/core/WorldView.hpp"

namespace mario::app {

namespace {

SDL_Rect toSdl(const Rect& r) noexcept { return SDL_Rect{r.x, r.y, r.w, r.h}; }

SDL_Color toSdl(Color c) noexcept { return SDL_Color{c.r, c.g, c.b, c.a}; }

const char* spriteFile(Sprite sprite) noexcept {
    switch (sprite) {
        case Sprite::Ground:
            return "block.png";
        case Sprite::Brick:
            return "platform.png";
        case Sprite::Coin:
            return "coin.png";
        case Sprite::Enemy:
            return "enemy.webp";
        case Sprite::Fireball:
            return "fireball.png";
        case Sprite::Mushroom:
            return "star.png";
        case Sprite::Flower:
            return "flower.png";
        case Sprite::Player:
            return "mario.webp";
        case Sprite::MovingPlatform:
            return "moving_platform.png";
    }
    return "";
}

RendererPtr createRenderer(SDL_Window& window) {
    RendererPtr renderer{SDL_CreateRenderer(&window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)};
    if (!renderer) {
        renderer.reset(SDL_CreateRenderer(&window, -1, SDL_RENDERER_SOFTWARE));
    }
    if (!renderer) {
        throw SdlError("SDL_CreateRenderer failed");
    }
    return renderer;
}

}  // namespace

SdlRenderer::SdlRenderer(SDL_Window& window, const std::filesystem::path& assetDir)
    : renderer_(createRenderer(window)), fontPath_(assetDir / "fonts" / "DejaVuSans.ttf") {
    SDL_SetRenderDrawBlendMode(renderer_.get(), SDL_BLENDMODE_BLEND);
    for (std::size_t i = 0; i < kSpriteCount; ++i) {
        const auto path = assetDir / "sprites" / spriteFile(static_cast<Sprite>(i));
        sprites_[i].reset(IMG_LoadTexture(renderer_.get(), path.string().c_str()));
        if (!sprites_[i]) {
            throw SdlError("Cannot load sprite " + path.string());
        }
    }
    if (font(kHudPointSize) == nullptr) {
        throw SdlError("Cannot open font " + fontPath_.string());
    }
}

void SdlRenderer::clear(Color color) {
    setColor(color);
    SDL_RenderClear(renderer_.get());
}

void SdlRenderer::fillRect(const Rect& area, Color color) {
    setColor(color);
    const SDL_Rect rect = toSdl(area);
    SDL_RenderFillRect(renderer_.get(), &rect);
}

void SdlRenderer::outlineRect(const Rect& area, Color color) {
    setColor(color);
    const SDL_Rect rect = toSdl(area);
    SDL_RenderDrawRect(renderer_.get(), &rect);
}

void SdlRenderer::drawSprite(Sprite sprite, const Rect& area) {
    const SDL_Rect rect = toSdl(area);
    SDL_RenderCopy(renderer_.get(), sprites_[static_cast<std::size_t>(sprite)].get(), nullptr, &rect);
}

void SdlRenderer::drawText(std::string_view text, int x, int y, Color color, int pointSize) {
    TextImage image = renderText(text, color, pointSize);
    blit(image, x, y);
}

void SdlRenderer::drawTextCentered(std::string_view text, int y, Color color, Color shadow, int pointSize,
                                   int shadowOffset) {
    TextImage back = renderText(text, shadow, pointSize);
    TextImage front = renderText(text, color, pointSize);
    const int x = (config::kWindowWidth - front.width) / 2;
    blit(back, x + shadowOffset, y + shadowOffset);
    blit(front, x, y);
}

void SdlRenderer::present() { SDL_RenderPresent(renderer_.get()); }

TTF_Font* SdlRenderer::font(int pointSize) {
    auto it = fonts_.find(pointSize);
    if (it == fonts_.end()) {
        FontPtr opened{TTF_OpenFont(fontPath_.string().c_str(), pointSize)};
        if (!opened) {
            return nullptr;
        }
        it = fonts_.emplace(pointSize, std::move(opened)).first;
    }
    return it->second.get();
}

SdlRenderer::TextImage SdlRenderer::renderText(std::string_view text, Color color, int pointSize) {
    TextImage image;
    TTF_Font* face = font(pointSize);
    if (face == nullptr || text.empty()) {
        return image;
    }
    const std::string buffer(text);
    const SurfacePtr surface{TTF_RenderUTF8_Blended(face, buffer.c_str(), toSdl(color))};
    if (!surface) {
        std::cerr << "TTF_RenderUTF8_Blended failed: " << TTF_GetError() << '\n';
        return image;
    }
    image.texture.reset(SDL_CreateTextureFromSurface(renderer_.get(), surface.get()));
    image.width = surface->w;
    image.height = surface->h;
    return image;
}

void SdlRenderer::blit(TextImage& image, int x, int y) {
    if (!image.texture) {
        return;
    }
    const SDL_Rect rect{x, y, image.width, image.height};
    SDL_RenderCopy(renderer_.get(), image.texture.get(), nullptr, &rect);
}

void SdlRenderer::setColor(Color color) { SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a); }

}  // namespace mario::app
