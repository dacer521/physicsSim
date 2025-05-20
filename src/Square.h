#pragma once
#include <SDL3/SDL.h>

class Square {
public:
    float x, y;        // Top-left corner
    float width, height;
    float vx, vy;      // Velocity components
    float mass;

    
    Square(float x, float y, float w, float h, float vx = 0, float vy = 0, float mass = 1.0f);

    void update(float deltaTime);
    void applyForce(float fx, float fy);
    SDL_FRect toFRect() const;
};
