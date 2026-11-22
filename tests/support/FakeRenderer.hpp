#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include "mario/core/Renderer.hpp"

namespace mario::test {

class FakeRenderer final : public Renderer {
public:
    void clear(Color color) override {
        ++clears;
        lastClear = color;
    }
    void fillRect(const Rect& area, Color /*color*/) override { fills.push_back(area); }
    void outlineRect(const Rect& /*area*/, Color /*color*/) override { ++outlines; }
    void drawSprite(Sprite sprite, const Rect& area) override {
        ++sprites[sprite];
        spriteRects.emplace_back(sprite, area);
    }
    void drawText(std::string_view text, int /*x*/, int /*y*/, Color /*color*/, int /*pointSize*/) override {
        texts.emplace_back(text);
    }
    void drawTextCentered(std::string_view text, int /*y*/, Color /*color*/, Color /*shadow*/,
                          int /*pointSize*/, int /*shadowOffset*/) override {
        texts.emplace_back(text);
    }

    [[nodiscard]] bool hasText(std::string_view needle) const {
        return std::any_of(texts.begin(), texts.end(),
                           [needle](const std::string& t) { return t.find(needle) != std::string::npos; });
    }

    [[nodiscard]] int spriteCount(Sprite sprite) const {
        const auto it = sprites.find(sprite);
        return it == sprites.end() ? 0 : it->second;
    }

    int clears{0};
    Color lastClear{};
    int outlines{0};
    std::vector<Rect> fills;
    std::map<Sprite, int> sprites;
    std::vector<std::pair<Sprite, Rect>> spriteRects;
    std::vector<std::string> texts;
};

}  // namespace mario::test
