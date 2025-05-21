#include <SDL.h>
#include <SDL_render.h>
#include <SDL_keyboard.h>
#include <iostream>
#include <math.h>

#include "Square.h"
#include "Circle.h"
#include "Triangle.h"

using namespace std;

void handleMovement(const SDL_KeyboardEvent &keyEvent, float &x, float &y);

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    

    int height = 640;
    int width = 480;

    SDL_Window* window = SDL_CreateWindow("Physics Sim", height, width, 0);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    else {
        
        SDL_GetWindowSizeInPixels(window, &width, &height);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    

    Square player(100.0f, 100.0f, 50.0f, 50.0f, -2.0f, -1.5f, 1.0f); // x, y, w, h, vx, vy, mass

    Circle player2(200.0f, 300.0f, 40.0f, 0.0f, 0.0f, 1.0f); //x y radius vx vy mass

    Triangle player3({350.0f, 0.0f}, {300.0f, 30.0f}, {400.0f, 60.0f}, 1.0f, 0.0f, 0.0f, {0, 255, 0, 255}); // v1 {x, y} v2 v3 mass vx vy mass color

    bool running = true;
    float delta_time;
    int FPS = 60;
    float FRAME_TARGET_TIME = (1000 / FPS);
    float last_frame_time = 0;
    
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }
        


    

    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);

    // Only delay if we are too fast to update this frame
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }
    // Get a delta time factor converted to seconds to be used to update my objects
    delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;

    last_frame_time = SDL_GetTicks();

    

    // Update position
    player.update(1.0f); // deltaTime = 1 for now

    player.vy++;
    
    // Bounce logic
    if (player.y <= 0 && player.vy < 1) {
        player.vy *= -1;
    }

    if (player.y + player.height >= height && player.vy > 1) {
        player.vy *= -1;
    }

    if (player.x <= 0 && player.vx < 1) {
        player.vx *= -1;
    }

    if (player.x + player.width >= width && player.vx > 1) {
        player.vx *= -1;
    }
    
    player.update(1.0f); // assuming deltaTime = 1 for simplicity


    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_FRect rect = player.toFRect(); 
    SDL_RenderFillRect(renderer, &rect);
    
    player3.drawTriangle(renderer);

    player2.draw_circle(renderer, player2.x, player2.y, player2.r, {0, 255, 0, 255});

    SDL_RenderPresent(renderer);
    SDL_Delay(16); // ~60 FPS
}

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}



