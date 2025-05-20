#include <SDL.h>
#include <SDL_keyboard.h>

void handleMovement(const SDL_KeyboardEvent &keyEvent, float &x, float &y) {
    switch (keyEvent.scancode) {
        case SDL_SCANCODE_W:
        case SDL_SCANCODE_UP:
            y -= 5;
            break;
        case SDL_SCANCODE_S:
        case SDL_SCANCODE_DOWN:
            y += 5;
            break;
        case SDL_SCANCODE_A:
        case SDL_SCANCODE_LEFT:
            x -= 5;
            break;
        case SDL_SCANCODE_D:
        case SDL_SCANCODE_RIGHT:
            x += 5;
            break;
        default:
            break;
    }
}