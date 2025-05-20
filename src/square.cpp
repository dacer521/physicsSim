#include "Square.h"

Square::Square(float x, float y, float w, float h, float vx, float vy, float mass)
    : x(x), y(y), width(w), height(h), vx(vx), vy(vy), mass(mass) {}

void Square::update(float deltaTime) {
    x += vx * deltaTime;
    y += vy * deltaTime;
}

void Square::applyForce(float fx, float fy) {
    float ax = fx / mass;
    float ay = fy / mass;
    vx += ax;
    vy += ay;
}

SDL_FRect Square::toFRect() const {
    return SDL_FRect{ x, y, width, height };
}
