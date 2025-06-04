#include <SDL_render.h>
#include <cmath>
#include <array>
#include <vector>
#include <limits>
#include <iostream>

#include "Square.h"
#include "Triangle.h"
#include "Circle.h"

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

void Square::checkCollision(int screenWidth, int screenHeight) {
    if (y + height >= screenHeight) {
        y = screenHeight - height;
        vy = -abs(vy);
    }

    if (y <= 0) {
        y = 0;
        vy = abs(vy);
    }

    if (x + width >= screenWidth) {
        x = screenWidth - width;
        vx = -abs(vx);
    }

    if (x <= 0) {
        x = 0;
        vx = abs(vx);
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

bool Square::collideShape(Shape &shape) {
    std::vector<Vec2> axes;

    std::vector<Vec2> squareVerts = getVertices();
    
    if (Triangle* t = dynamic_cast<Triangle*>(&shape)) {
        std::vector<Vec2> triangleVerts = t->getVertices();

        // Triangle edge normals
        for (int i = 0; i < triangleVerts.size(); i++) {
            Vec2 edge = {
                triangleVerts[(i + 1) % 3].x - triangleVerts[i].x,
                triangleVerts[(i + 1) % 3].y - triangleVerts[i].y};
            Vec2 normal = {-edge.y, edge.x};
            
            float length = sqrt(normal.x * normal.x + normal.y * normal.y);
            if (length == 0) continue;

            Vec2 normalized = {normal.x / length, normal.y / length};
            axes.push_back(normalized);
        }       

        // Square edge normals - FIXED: use squareVerts, not triangleVerts
        for (int i = 0; i < squareVerts.size(); i++) {
            Vec2 edge = {
                squareVerts[(i + 1) % 4].x - squareVerts[i].x,
                squareVerts[(i + 1) % 4].y - squareVerts[i].y};

            Vec2 normal = {-edge.y, edge.x};
            
            float length = sqrt(normal.x * normal.x + normal.y * normal.y);
            if (length == 0) continue;

            Vec2 normalized = {normal.x / length, normal.y / length};
            axes.push_back(normalized);
        }
        
        // SAT projection testing
        for (Vec2 axis : axes) {
            float triMin = std::numeric_limits<float>::infinity();
            float triMax = -std::numeric_limits<float>::infinity();
            float sqMin = std::numeric_limits<float>::infinity();
            float sqMax = -std::numeric_limits<float>::infinity();

            for (Vec2 vertex : triangleVerts) {
                float projection = dot(vertex, axis);

                if (projection < triMin) {
                    triMin = projection;
                }
                
                if (projection > triMax) {
                    triMax = projection;
                }
            }

            for (Vec2 vertex : squareVerts) {
                float projection = dot(vertex, axis);

                if (projection < sqMin) {
                    sqMin = projection;
                }
                
                if (projection > sqMax) {
                    sqMax = projection;
                }
            }

            if (triMax < sqMin || sqMax < triMin) {
                return false;
            }
        }

        return true; // No separating axis found
    }

    else if (Square* s = dynamic_cast<Square*>(&shape)) {
        std::vector<Vec2> otherVerts = s->getVertices();

        // Other square edge normals
        for (int i = 0; i < otherVerts.size(); i++) {
            Vec2 edge = {
                otherVerts[(i + 1) % 4].x - otherVerts[i].x,
                otherVerts[(i + 1) % 4].y - otherVerts[i].y};
            Vec2 normal = {-edge.y, edge.x};
            
            float length = sqrt(normal.x * normal.x + normal.y * normal.y);
            if (length == 0) continue;

            Vec2 normalized = {normal.x / length, normal.y / length};
            axes.push_back(normalized);
        }       

        // This square edge normals - FIXED: use squareVerts, not otherVerts
        for (int i = 0; i < squareVerts.size(); i++) {
            Vec2 edge = {
                squareVerts[(i + 1) % 4].x - squareVerts[i].x,
                squareVerts[(i + 1) % 4].y - squareVerts[i].y};

            Vec2 normal = {-edge.y, edge.x};
            
            float length = sqrt(normal.x * normal.x + normal.y * normal.y);
            if (length == 0) continue;

            Vec2 normalized = {normal.x / length, normal.y / length};
            axes.push_back(normalized);
        }
        
        // SAT projection testing
        for (Vec2 axis : axes) {
            float otherMin = std::numeric_limits<float>::infinity();
            float otherMax = -std::numeric_limits<float>::infinity();
            float sqMin = std::numeric_limits<float>::infinity();
            float sqMax = -std::numeric_limits<float>::infinity();

            for (Vec2 vertex : otherVerts) {
                float projection = dot(vertex, axis);

                if (projection < otherMin) {
                    otherMin = projection;
                }
                
                if (projection > otherMax) {
                    otherMax = projection;
                }
            }

            for (Vec2 vertex : squareVerts) {
                float projection = dot(vertex, axis);

                if (projection < sqMin) {
                    sqMin = projection;
                }
                
                if (projection > sqMax) {
                    sqMax = projection;
                }
            }

            if (otherMax < sqMin || sqMax < otherMin) {
                return false;
            }
        }

        return true; // FIXED: Added missing return statement
    }

    else if (Circle* c = dynamic_cast<Circle*>(&shape)) {
        std::vector<Vec2> verts = getVertices();

        for (int i = 0; i < verts.size(); i++) {
            Vec2 start = verts[i];
            Vec2 end = verts[(i + 1) % 4];

            float dx = end.x - start.x;
            float dy = end.y - start.y;

            //pixel density
            float steps = std::max(std::abs(dx), std::abs(dy));
            if (steps == 0) continue;

            for (int j = 0; j < steps; j++) {
                float t_param = static_cast<float>(j) / steps; //normalized interpolation parameter. Controls how far between start and end I am. 0 is start point 1 is end point.

                float pointX = start.x + t_param * dx;
                float pointY = start.y + t_param * dy;

                float circleVal = pow((pointX - c->getX()), 2) + pow((pointY - c->getY()), 2);

                if (circleVal <= pow(c->getRadius(), 2)) {
                    return true;
                }
            }
        }

        return false;
    }

    return false;
}

float Square::getVx() const { return vx; }
void Square::setVx(float v) { vx = v; }

float Square::getVy() const { return vy; }
void Square::setVy(float v) { vy = v; }

float Square::getX() const { return x; }
float Square::getY() const { return y; }

float Square::getHeight() const { return height; }
float Square::getWidth() const { return width; }