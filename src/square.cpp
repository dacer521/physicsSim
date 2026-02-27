#include <SDL_render.h>
#include <cmath>
#include <array>
#include <vector>
#include <limits>
#include <iostream>
#include <optional>

#include "Square.h"
#include "Triangle.h"
#include "Circle.h"
#include "Shape.h"

using namespace std;

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

void Square::draw(SDL_Renderer* renderer, SDL_Color color)  {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_FRect rect = { x, y, width, height };
    SDL_RenderFillRect(renderer, &rect);
}


void Square::checkCollision(int screenWidth, int screenHeight, const vector<shared_ptr<Shape>> &shapeList, float elasticModifier) {
    // Boundary collision
    if (y + height >= screenHeight) {
        y = screenHeight - height;
        vy = -abs(vy) * elasticModifier;
    }

    if (y <= 0) {
        y = 0;
        vy = abs(vy) * elasticModifier;
    }

    if (x + width >= screenWidth) {
        x = screenWidth - width;
        vx = -abs(vx) * elasticModifier;
    }

    if (x <= 0) {
        x = 0;
        vx = abs(vx) * elasticModifier;
    }

    // Shape-to-shape collision - FIXED
    for (auto& shape : shapeList) {
        if (shape.get() == this) continue;

        if (Circle* c = dynamic_cast<Circle*>(shape.get())) {
            // Circle-Square collision
            float squareCenterX = x + width / 2;
            float squareCenterY = y + height / 2;
            float dx = squareCenterX - c->getX();
            float dy = squareCenterY - c->getY();
            
            // Find closest point on square to circle center
            float closestX = std::max(x, std::min(c->getX(), x + width));
            float closestY = std::max(y, std::min(c->getY(), y + height));

            dx = c->getX() - closestX;
            dy = c->getY() - closestY;
            float dist = std::sqrt(dx * dx + dy * dy);

            if (dist < c->getRadius()) {
                float overlap = c->getRadius() - dist;
                float nx = 1.0f;
                float ny = 0.0f;

                if (dist > 0.001f) {
                    nx = dx / dist;
                    ny = dy / dist;
                } else {
                    // Circle center inside square: push out along nearest face
                    float toLeft = c->getX() - x;
                    float toRight = x + width - c->getX();
                    float toTop = c->getY() - y;
                    float toBottom = y + height - c->getY();
                    if (toLeft <= toRight && toLeft <= toTop && toLeft <= toBottom) {
                        nx = 1.0f; ny = 0.0f; dist = toLeft;
                    } else if (toRight <= toTop && toRight <= toBottom) {
                        nx = -1.0f; ny = 0.0f; dist = toRight;
                    } else if (toTop <= toBottom) {
                        nx = 0.0f; ny = 1.0f; dist = toTop;
                    } else {
                        nx = 0.0f; ny = -1.0f; dist = toBottom;
                    }
                    if (dist >= c->getRadius()) {
                        overlap = c->getRadius() + dist;
                    } else {
                        overlap = c->getRadius() - dist;
                    }
                }

                // Separate by overlap (no extra buffer)
                float separation = overlap * 0.5f;
                x -= nx * separation;
                y -= ny * separation;
                c->setX(c->getX() + nx * separation);
                c->setY(c->getY() + ny * separation);
                
                // Velocity reflection
                float relVelX = vx - c->getVx();
                float relVelY = vy - c->getVy();
                float velAlongNormal = relVelX * (-nx) + relVelY * (-ny);
                
                if (velAlongNormal > 0) continue;  // Separating
                
                float impulse = -(1 + elasticModifier) * velAlongNormal;
                impulse /= (1.0f/mass + 1.0f/c->getMass());
                
                vx += impulse * (-nx) / mass;
                vy += impulse * (-ny) / mass;
                c->setVx(c->getVx() - impulse * (-nx) / c->getMass());
                c->setVy(c->getVy() - impulse * (-ny) / c->getMass());
            }
        } else {
            if (this > shape.get()) continue; // resolve each polygon pair once
            // SAT collision with other polygons - FIXED
            vector<Vec2> vertsA = getVertices();
            vector<Vec2> vertsB = shape->getVertices();
            optional<Vec2> mtv = getMTV(vertsA, vertsB);
            
            if (mtv.has_value()) {
                float mtvLength = std::sqrt(mtv->x * mtv->x + mtv->y * mtv->y);
                if (mtvLength > 0.001f) {
                    Vec2 normalizedMtv = {mtv->x / mtvLength, mtv->y / mtvLength};
                    float separationDistance = mtvLength;
                    
                    // Complete separation
                    x += normalizedMtv.x * separationDistance * 0.5f;
                    y += normalizedMtv.y * separationDistance * 0.5f;
                    shape->setX(shape->getX() - normalizedMtv.x * separationDistance * 0.5f);
                    shape->setY(shape->getY() - normalizedMtv.y * separationDistance * 0.5f);
                    
                    // Velocity collision response
                    float relVelX = vx - shape->getVx();
                    float relVelY = vy - shape->getVy();
                    float velAlongNormal = relVelX * normalizedMtv.x + relVelY * normalizedMtv.y;
                    
                    if (velAlongNormal > 0) continue;  // Separating
                    
                    float impulse = -(1 + elasticModifier) * velAlongNormal;
                    impulse /= (1.0f/mass + 1.0f/shape->getMass());  // Assuming mass exists
                    
                    vx += impulse * normalizedMtv.x / mass;
                    vy += impulse * normalizedMtv.y / mass;
                    shape->setVx(shape->getVx() - impulse * normalizedMtv.x / shape->getMass());
                    shape->setVy(shape->getVy() - impulse * normalizedMtv.y / shape->getMass());
                }
            }
        }
    }
}

std::vector<Vec2> Square::getVertices() const {
    std::vector<Vec2> verts;

    verts.push_back({getX(), getY()});
    verts.push_back({getX() + getWidth(), getY()});
    verts.push_back({getX() + getWidth(), getY() + getHeight()});
    verts.push_back({getX(), getY() + getHeight()});

    return verts;
}
std::optional<Vec2> Square::getMTV(const std::vector<Vec2>& vertsA, const std::vector<Vec2>& vertsB) {
    float minOverlap = std::numeric_limits<float>::infinity();
    Vec2 mtvAxis;

    std::vector<Vec2> axes;
    for (int i = 0; i < vertsA.size(); ++i) {
        Vec2 edge = {vertsA[(i + 1) % vertsA.size()].x - vertsA[i].x, vertsA[(i + 1) % vertsA.size()].y - vertsA[i].y};
        Vec2 normal = {-edge.y, edge.x};
        float length = std::sqrt(normal.x * normal.x + normal.y * normal.y);
        axes.push_back({normal.x / length, normal.y / length});
    }
    for (int i = 0; i < vertsB.size(); ++i) {
        Vec2 edge = {vertsB[(i + 1) % vertsB.size()].x - vertsB[i].x, vertsB[(i + 1) % vertsB.size()].y - vertsB[i].y};
        Vec2 normal = {-edge.y, edge.x};
        float length = std::sqrt(normal.x * normal.x + normal.y * normal.y);
        axes.push_back({normal.x / length, normal.y / length});
    }

    for (const auto& axis : axes) {
        float minA = vertsA[0].x * axis.x + vertsA[0].y * axis.y;
        float maxA = minA;
        for (const auto& v : vertsA) {
            float proj = v.x * axis.x + v.y * axis.y;
            minA = std::min(minA, proj);
            maxA = std::max(maxA, proj);
        }

        float minB = vertsB[0].x * axis.x + vertsB[0].y * axis.y;
        float maxB = minB;
        for (const auto& v : vertsB) {
            float proj = v.x * axis.x + v.y * axis.y;
            minB = std::min(minB, proj);
            maxB = std::max(maxB, proj);
        }

        if (maxA < minB || maxB < minA) return std::nullopt;

        float overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap < minOverlap) {
            minOverlap = overlap;
            mtvAxis = axis;
            float centerA = (minA + maxA) / 2;
            float centerB = (minB + maxB) / 2;
            if (centerA > centerB) {
                mtvAxis.x = -mtvAxis.x;
                mtvAxis.y = -mtvAxis.y;
            }
        }
    }

    return Vec2{mtvAxis.x * minOverlap, mtvAxis.y * minOverlap};
}

float Square::getVx() const { return vx; }
void Square::setVx(float v) { vx = v; }

float Square::getVy() const { return vy; }
void Square::setVy(float v) { vy = v; }

float Square::getX() const { return x; }
float Square::getY() const { return y; }

float Square::getHeight() const { return height; }
float Square::getWidth() const { return width; }

void Square::setX(float x1) {x = x1;}
void Square::setY(float y1) {y = y1;}

float Square::getMass() const {return mass;}
