#pragma once
#include <SDL3/SDL.h>

class Circle {
public:
    float x, y;        // Top-left corner
    float r;
    float vx, vy;      // Velocity components
    float mass;

    
    Circle(float x, float y, float r, float vx = 0, float vy = 0, float mass = 1.0f);

    void update(float deltaTime);
    void applyForce(float fx, float fy);
    void draw_circle(SDL_Renderer*, int center_x, int center_y, int radius, SDL_Color color);
};
