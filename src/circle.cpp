#include "Circle.h"
#include <SDL_render.h>
#include <cmath>
#include <vector>

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

void Circle::draw(SDL_Renderer* renderer ,SDL_Color color) {
    // Draw circle centered at (x, y)
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int w = 0; w < r * 2; w++) {
        for (int h = 0; h < r * 2; h++) {
            int dx = r - w;
            int dy = r - h;
            if ((dx * dx + dy * dy) <= (r * r)) {
                SDL_RenderPoint(renderer, static_cast<int>(x - r + w), static_cast<int>(y - r + h));
            }
        }
    }
}

void Circle::checkCollision(int screenWidth, int screenHeight) {
    if (y + r >= screenHeight) {
        y = screenHeight - r;
        vy = -abs(vy);
    }

    if (y - r <= 0) {
        y = r;
        vy = abs(vy);
    }

    if (x + r >= screenWidth) {
        x = screenWidth - r;
        vx = -abs(vx);
    }

    if (x - r <= 0) {
        x = r;
        vx = abs(vx);
    }
}

float Circle::getVx() const { return vx; }
void Circle::setVx(float v) { vx = v; }

float Circle::getVy() const { return vy; }
void Circle::setVy(float v) { vy = v; }

// Return center coordinates for physics calculations  
float Circle::getX() const { return x; }
float Circle::getY() const { return y; }

float Circle::getHeight() const { return r * 2; }
float Circle::getWidth() const { return r * 2; }

std::vector<Vec2> Circle::getVertices() const {
    // Simplified approximation: bounding box
    return {
        {x - r, y - r},
        {x + r, y - r},
        {x + r, y + r},
        {x - r, y + r}
    };
}

bool Circle::collideShape(Shape& other) {
    // TODO: implement actual collision
    return false;
}