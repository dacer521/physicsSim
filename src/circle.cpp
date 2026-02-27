#include "Circle.h"
#include <SDL_render.h>
#include <cmath>
#include <vector>
#include <algorithm>

#include "Square.h"
#include "Shape.h"
#include "Triangle.h"

using namespace std;

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

std::optional<Vec2> Circle::getMTV(const std::vector<Vec2>& vertsA, const std::vector<Vec2>& vertsB) {
    if (vertsA.empty()) return std::nullopt;

    Vec2 center = vertsA[0];
    float minOverlap = std::numeric_limits<float>::max();
    Vec2 smallestAxis;

    for (int i = 0; i < vertsB.size(); ++i) {
        Vec2 edge = {
            vertsB[(i + 1) % vertsB.size()].x - vertsB[i].x,
            vertsB[(i + 1) % vertsB.size()].y - vertsB[i].y
        };
        Vec2 axis = {-edge.y, edge.x};
        float len = std::sqrt(axis.x * axis.x + axis.y * axis.y);
        axis = {axis.x / len, axis.y / len};

        float minB = INFINITY, maxB = -INFINITY;
        for (const auto& v : vertsB) {
            float proj = v.x * axis.x + v.y * axis.y;
            minB = std::min(minB, proj);
            maxB = std::max(maxB, proj);
        }

        float centerProj = center.x * axis.x + center.y * axis.y;
        float minA = centerProj - r;
        float maxA = centerProj + r;

        float overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap < 0) return std::nullopt;
        if (overlap < minOverlap) {
            minOverlap = overlap;
            smallestAxis = axis;
        }
    }

    return Vec2{smallestAxis.x * minOverlap, smallestAxis.y * minOverlap};
}

float Circle::getRadius() {
    return r;
}
void Circle::checkCollision(int screenWidth, int screenHeight, const std::vector<std::shared_ptr<Shape>> &shapeList, float elasticModifier) {
    // Boundary collision detection
    if (y + r >= screenHeight) {
        y = screenHeight - r;
        vy = -abs(vy) * elasticModifier;
    }
    
    if (y - r <= 0) {
        y = r;
        vy = abs(vy) * elasticModifier;
    }
    
    if (x + r >= screenWidth) {
        x = screenWidth - r;
        vx = -abs(vx) * elasticModifier;
    }
    
    if (x - r <= 0) {
        x = r;
        vx = abs(vx) * elasticModifier;
    }

    // Shape-to-shape collision (circle-only to avoid double-resolution)
    for (auto& shape : shapeList) {
        if (shape.get() == this) continue;

        if (Circle* c = dynamic_cast<Circle*>(shape.get())) {
            if (this > c) continue; // resolve each pair once
            float dx = x - c->getX();
            float dy = y - c->getY();
            float dist = std::sqrt(dx * dx + dy * dy);
            float totalRadius = r + c->getRadius();

            if (dist < totalRadius) {
                float overlap = totalRadius - dist;

                // Normalize direction (fallback to avoid division by zero)
                float nx = 1.0f;
                float ny = 0.0f;
                if (dist > 0.001f) {
                    nx = dx / dist;
                    ny = dy / dist;
                }

                // Separate objects by the overlap (no extra buffer)
                float separation = overlap * 0.5f;
                x += nx * separation;
                y += ny * separation;
                c->setX(c->getX() - nx * separation);
                c->setY(c->getY() - ny * separation);
                
                // Calculate relative velocity
                float rvx = vx - c->getVx();
                float rvy = vy - c->getVy();
                
                // Calculate relative velocity along normal
                float velAlongNormal = rvx * nx + rvy * ny;
                
                // Don't resolve if velocities are separating
                if (velAlongNormal > 0) continue;
                
                // Calculate impulse scalar
                float impulse = -(1 + elasticModifier) * velAlongNormal;
                impulse /= (1.0f/mass + 1.0f/c->mass);
                
                // Apply impulse
                vx += impulse * nx / mass;
                vy += impulse * ny / mass;
                c->setVx(c->getVx() - impulse * nx / c->mass);
                c->setVy(c->getVy() - impulse * ny / c->mass);
            }
        }
    }
}

void Circle::setX(float x1) {
    x = x1;
}

void Circle::setY(float y1) {
    y = y1;
}

void Circle::setVx(float v) { vx = v; }
void Circle::setVy(float v) { vy = v; }

float Circle::getVx() const { return vx; }
float Circle::getVy() const { return vy; }

float Circle::getX() const { return x; }
float Circle::getY() const { return y; }

float Circle::getWidth() const { return r * 2; }
float Circle::getHeight() const { return r * 2; }

std::vector<Vec2> Circle::getVertices() const {
    // For SAT compatibility, just return 1 point or approximate polygon if needed
    return { {x, y} };
}

float Circle::getMass() const {return mass;}
