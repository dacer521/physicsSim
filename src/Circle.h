#pragma once
#include <SDL3/SDL.h>
#include "Shape.h"

class Circle : public Shape {
public:
    float x, y;
    float r;
    float vx, vy;
    float mass;

    Circle(float x, float y, float r, float vx = 0, float vy = 0, float mass = 1.0f);

    void update(float deltaTime) override;
    void applyForce(float fx, float fy);
    void draw_circle(SDL_Renderer*, int center_x, int center_y, int radius, SDL_Color color) const;
    void draw(SDL_Renderer* renderer) const override;
};