#pragma once
#include <SDL3/SDL.h>
#include "Shape.h"

class Square : public Shape {
public:
    float x, y;
    float width, height;
    float vx, vy;
    float mass;

    Square(float x, float y, float w, float h, float vx = 0, float vy = 0, float mass = 1.0f);

    void update(float deltaTime) override;
    void applyForce(float fx, float fy);
    SDL_FRect toFRect() const;
    void draw(SDL_Renderer* renderer) const override;
};