#include "Circle.h"
#include <SDL_render.h>
#include <cmath>

Circle::Circle(float x, float y, float r, float vx, float vy, float mass)
    : x(x), y(y), r(r), vx(vx), vy(vy), mass(mass) {}

void Circle::update(float deltaTime) {
    x += vx * deltaTime;
    y += vy * deltaTime;
}

void Circle::applyForce(float fx, float fy) {
    float ax = fx / mass;
    float ay = fy / mass;
    vx += ax;
    vy += ay;
}

void Circle::draw_circle(SDL_Renderer* renderer, int cx, int cy, int radius, SDL_Color color) const {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            if (dx*dx + dy*dy <= radius*radius)
                SDL_RenderPoint(renderer, cx + dx, cy + dy);
        }
    }
}

void Circle::draw(SDL_Renderer* renderer) const {
    SDL_Color magenta = {255, 0, 255, 255};
    draw_circle(renderer, static_cast<int>(x), static_cast<int>(y), static_cast<int>(r), magenta);
}