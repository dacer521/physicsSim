#include <SDL.h>
#include <SDL_render.h>
#include <SDL_keyboard.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <memory>

#include "Shape.h"
#include "Square.h"
#include "Circle.h"
#include "Triangle.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        cerr << "SDL_Init Error: " << SDL_GetError() << endl;
        return 1;
    }

    int width = 640;
    int height = 480;

    SDL_Window* window = SDL_CreateWindow("Physics Sim", width, height, 0);
    if (!window) {
        cerr << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    vector<shared_ptr<Shape>> shapeList = {
        make_shared<Square>(270.0f, 100.0f, 50.0f, 50.0f, 0.0f, -1.5f, 1.0f),
        make_shared<Square>(320.0f, 114.0f, 50.0f, 50.0f, -5.0f, -1.5f, 1.0f),
        make_shared<Triangle>(Vec2{300.0f, 300.0f}, Vec2{340.0f, 300.0f}, Vec2{320.0f, 260.0f}, 1.2f, 0.0f, -1.0f, SDL_Color{255, 255, 255, 255}),
        make_shared<Triangle>(Vec2{200.0f, 200.0f}, Vec2{240.0f, 200.0f}, Vec2{220.0f, 160.0f}, 1.2f, 3.0f, -1.0f, SDL_Color{255, 255, 255, 255})
    };

    bool running = true;
    int FPS = 60;
    float FRAME_TARGET_TIME = 1000.0f / FPS;
    float last_frame_time = 0;

    while (running) {
        float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;
        last_frame_time = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Update all shapes and check bounds
        for (auto& shape : shapeList) {
            shape->update(delta_time);
            shape->setVy(shape->getVy() + 1.0f);
            shape->checkCollision(width, height);
        }

        // Determine collision status for each shape
        vector<bool> isColliding(shapeList.size(), false);
        for (size_t i = 0; i < shapeList.size(); ++i) {
            for (size_t j = 0; j < shapeList.size(); ++j) {
                if (i != j && shapeList[i]->collideShape(*shapeList[j])) {
                    isColliding[i] = true;
                    isColliding[j] = true;
                }
            }
        }

        // Draw all shapes based on collision status
        for (size_t i = 0; i < shapeList.size(); ++i) {
            SDL_Color color = isColliding[i] ? SDL_Color{255, 255, 255, 255} : SDL_Color{255, 0, 0, 255};
            shapeList[i]->draw(renderer, color);
        }

        SDL_RenderPresent(renderer);

        float frame_time = SDL_GetTicks() - last_frame_time;
        if (frame_time < FRAME_TARGET_TIME) {
            SDL_Delay(FRAME_TARGET_TIME - frame_time);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}