#pragma once
#include <SDL3/SDL.h>

class Shape {
public:
    virtual void update(float deltaTime) = 0;
    virtual void draw(SDL_Renderer* renderer) const = 0;
    virtual ~Shape() = default;
};