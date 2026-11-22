#include "SdlInput.hpp"

namespace mario::app {

Key translateKey(SDL_Keycode code) noexcept {
    switch (code) {
        case SDLK_UP:
            return Key::Up;
        case SDLK_DOWN:
            return Key::Down;
        case SDLK_LEFT:
            return Key::Left;
        case SDLK_RIGHT:
            return Key::Right;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            return Key::Enter;
        case SDLK_SPACE:
            return Key::Space;
        case SDLK_ESCAPE:
            return Key::Escape;
        case SDLK_p:
            return Key::P;
        case SDLK_m:
            return Key::M;
        case SDLK_s:
            return Key::S;
        case SDLK_LSHIFT:
            return Key::LeftShift;
        case SDLK_0:
            return Key::Digit0;
        case SDLK_1:
            return Key::Digit1;
        case SDLK_2:
            return Key::Digit2;
        case SDLK_3:
            return Key::Digit3;
        case SDLK_4:
            return Key::Digit4;
        case SDLK_5:
            return Key::Digit5;
        case SDLK_6:
            return Key::Digit6;
        case SDLK_7:
            return Key::Digit7;
        case SDLK_8:
            return Key::Digit8;
        default:
            return Key::Unknown;
    }
}

std::optional<InputEvent> translateEvent(const SDL_Event& event) noexcept {
    if (event.type == SDL_KEYDOWN) {
        const Key key = translateKey(event.key.keysym.sym);
        if (key == Key::Unknown) {
            return std::nullopt;
        }
        return KeyPressed{key};
    }
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        MouseButton button = MouseButton::Other;
        if (event.button.button == SDL_BUTTON_LEFT) {
            button = MouseButton::Left;
        } else if (event.button.button == SDL_BUTTON_RIGHT) {
            button = MouseButton::Right;
        }
        return MousePressed{event.button.x, event.button.y, button};
    }
    return std::nullopt;
}

InputState readInputState() noexcept {
    const Uint8* keys = SDL_GetKeyboardState(nullptr);
    return InputState{keys[SDL_SCANCODE_LEFT] != 0, keys[SDL_SCANCODE_RIGHT] != 0, keys[SDL_SCANCODE_UP] != 0,
                      keys[SDL_SCANCODE_SPACE] != 0};
}

}  // namespace mario::app
