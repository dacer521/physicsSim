#pragma once
#include <SDL3/SDL.h>

struct Vec2 {
    float x, y;
};

class Triangle {
public:

    Vec2 vertices[3];
    float vx, vy;
    float mass;
    SDL_Color color;


    Triangle(Vec2 v1, Vec2 v2, Vec2 v3, float mass, float vx, float vy, SDL_Color color);

    void update(float deltaTime);
    void applyForce(float fx, float fy);
    void drawTriangle(SDL_Renderer* renderer) const;

    
};