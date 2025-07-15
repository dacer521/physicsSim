#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_events.h>
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>

#include "Shape.h"
#include "Square.h"
#include "Circle.h"
#include "Triangle.h"

using namespace std;

int main(int argc, char* argv[]) {
    // Initialize SDL with error checking
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        cerr << "SDL_Init Error: " << SDL_GetError() << endl;
        return 1;
    }

    // Create window with proper SDL3 syntax
    SDL_Window* window = SDL_CreateWindow("Physics Sim", 640, 480, SDL_WINDOW_RESIZABLE);
    if (!window) {
        cerr << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    // Create renderer with proper SDL3 syntax
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    int width = 640, height = 480;
    vector<shared_ptr<Shape>> shapeList = {
        make_shared<Square>(270.0f, 100.0f, 50.0f, 50.0f, 4.0f, 1.5f, 1.0f),
        make_shared<Triangle>(Vec2{100,100}, Vec2{150,50}, Vec2{200,100}, 1.2f, 1.0f, -1.0f, SDL_Color{255,0,0,255}),
        make_shared<Circle>(200.0f, 100.0f, 30.0f, 3.0f, -2.0f, 1.0f)
    };

    bool running = true;
    const int FPS = 60;
    const float FRAME_TARGET_TIME = 1000.0f / FPS;
    Uint64 last_ticks = SDL_GetTicks();

    while (running) {
        Uint64 current_ticks = SDL_GetTicks();
        float delta_time = (current_ticks - last_ticks) / 1000.0f;
        last_ticks = current_ticks;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Update physics
        for (auto& shape : shapeList) {
            shape->update(delta_time);
            shape->setVy(shape->getVy() + 981.0f * delta_time); 
        }

        // Check collisions
        for (auto& shape : shapeList) {
            shape->checkCollision(width, height, shapeList, 0.9f);
        }

        // Draw shapes with colors
        for (auto& shape : shapeList) {
            if (dynamic_cast<Triangle*>(shape.get()))
                shape->draw(renderer, SDL_Color{255, 0, 0, 255});
            else if (dynamic_cast<Square*>(shape.get()))
                shape->draw(renderer, SDL_Color{0, 255, 0, 255});
            else if (dynamic_cast<Circle*>(shape.get()))
                shape->draw(renderer, SDL_Color{0, 0, 255, 255});
}

        SDL_RenderPresent(renderer);

        // Frame rate limiting
        float frame_time = SDL_GetTicks() - current_ticks;
        if (frame_time < FRAME_TARGET_TIME) {
            SDL_Delay((Uint32)(FRAME_TARGET_TIME - frame_time));
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}