#include "Circle.h"
#include <SDL_render.h>
#include <cmath>

Circle::Circle(float x, float y, float r, float vx, float vy, float mass) //x y radius vx vy mass
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


void Circle::draw_circle(SDL_Renderer* renderer, int center_x, int center_y, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    for (int y = -radius; y <= radius; y++) {
        int dx = static_cast<int>(sqrt(radius * radius - y * y));
        SDL_RenderLine(renderer, center_x - dx, center_y + y, center_x + dx, center_y + y);
    }
}
