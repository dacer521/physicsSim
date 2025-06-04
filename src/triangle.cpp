#include "Triangle.h"
#include "Square.h"
#include "Circle.h"

#include <algorithm>
#include <array>
#include <vector>
#include <iostream>
#include <limits>

Triangle::Triangle(Vec2 v1, Vec2 v2, Vec2 v3, float m, float vx, float vy, SDL_Color c)
    : vx(vx), vy(vy), mass(m), color(c)
{
    vertices[0] = v1;
    vertices[1] = v2;
    vertices[2] = v3;
}

void Triangle::applyForce(float fx, float fy) {
    vx += fx / mass;
    vy += fy / mass;
}

void Triangle::update(float deltaTime) {
    for (int i = 0; i < 3; ++i) {
        vertices[i].x += vx * deltaTime;
        vertices[i].y += vy * deltaTime;
    }
}

void Triangle::drawTriangle(SDL_Renderer* renderer, SDL_Color color) const {
    auto edgeFunction = [](const Vec2& a, const Vec2& b, const Vec2& c) {
        return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
    };

    float minX = std::min({ vertices[0].x, vertices[1].x, vertices[2].x });
    float maxX = std::max({ vertices[0].x, vertices[1].x, vertices[2].x });
    float minY = std::min({ vertices[0].y, vertices[1].y, vertices[2].y });
    float maxY = std::max({ vertices[0].y, vertices[1].y, vertices[2].y });

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    for (int y = (int)minY; y <= (int)maxY; ++y) {
        for (int x = (int)minX; x <= (int)maxX; ++x) {
            Vec2 p = { (float)x + 0.5f, (float)y + 0.5f };
            float w0 = edgeFunction(vertices[1], vertices[2], p);
            float w1 = edgeFunction(vertices[2], vertices[0], p);
            float w2 = edgeFunction(vertices[0], vertices[1], p);

            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                SDL_RenderPoint(renderer, x, y);
            }
        }
    }
}

void Triangle::draw(SDL_Renderer* renderer, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    drawTriangle(renderer, color);
}

void Triangle::checkCollision(int screenWidth, int screenHeight) {
    float minX = std::min({vertices[0].x, vertices[1].x, vertices[2].x});
    float maxX = std::max({vertices[0].x, vertices[1].x, vertices[2].x});
    float minY = std::min({vertices[0].y, vertices[1].y, vertices[2].y});
    float maxY = std::max({vertices[0].y, vertices[1].y, vertices[2].y});

    if (maxY >= screenHeight) {
        float offset = screenHeight - maxY;
        for (auto& v : vertices) v.y += offset;
        vy = -abs(vy);
    }

    if (minY <= 0) {
        float offset = -minY;
        for (auto& v : vertices) v.y += offset;
        vy = abs(vy);
    }

    if (maxX >= screenWidth) {
        float offset = screenWidth - maxX;
        for (auto& v : vertices) v.x += offset;
        vx = -abs(vx);
    }

    if (minX <= 0) {
        float offset = -minX;
        for (auto& v : vertices) v.x += offset;
        vx = abs(vx);
    }
}

float Triangle::getVy() const {
    return vy;
}

void Triangle::setVy(float v) {
    vy = v; 
}

float Triangle::getVx() const { 
    return vx; 
}

void Triangle::setVx(float v) { 
    vx = v; 
}

float Triangle::getX() const {
    return getV1().x;
}

float Triangle::getY() const {
    return getV1().y;
}

Vec2 Triangle::getV1() const {
    return vertices[0];
}

Vec2 Triangle::getV2() const {
    return vertices[1];
}

Vec2 Triangle::getV3() const {
    return vertices[2];
}

std::vector<Vec2> Triangle::getVertices() const {
    std::vector<Vec2> triangleVerts;
    triangleVerts.push_back(vertices[0]);
    triangleVerts.push_back(vertices[1]);
    triangleVerts.push_back(vertices[2]);
    return triangleVerts;
}

float Triangle::getWidth() const { 
    float minX = std::min({ vertices[0].x, vertices[1].x, vertices[2].x });
    float maxX = std::max({ vertices[0].x, vertices[1].x, vertices[2].x });
    return maxX - minX;
}

float Triangle::getHeight() const { 
    float minY = std::min({ vertices[0].y, vertices[1].y, vertices[2].y });
    float maxY = std::max({ vertices[0].y, vertices[1].y, vertices[2].y });
    return maxY - minY;
}

bool Triangle::collideShape(Shape &shape) {
    std::vector<Vec2> axes;
    std::vector<Vec2> triangleVerts = this->getVertices();
    
    if (Square* s = dynamic_cast<Square*>(&shape)) {
        std::vector<Vec2> squareVerts = s->getVertices();

        // Triangle edge normals
        for (int i = 0; i < triangleVerts.size(); i++) {
            Vec2 edge = {
                triangleVerts[(i + 1) % 3].x - triangleVerts[i].x,
                triangleVerts[(i + 1) % 3].y - triangleVerts[i].y
            };
            Vec2 normal = {-edge.y, edge.x};
            float length = std::sqrt(normal.x * normal.x + normal.y * normal.y);
            if (length == 0) continue;
            
            Vec2 normalized = {normal.x / length, normal.y / length};
            axes.push_back(normalized);
        }

        // Square edge normals
        for (int i = 0; i < squareVerts.size(); i++) {
            Vec2 edge = {
                squareVerts[(i + 1) % 4].x - squareVerts[i].x,
                squareVerts[(i + 1) % 4].y - squareVerts[i].y
            };
            Vec2 normal = {-edge.y, edge.x};
            float length = std::sqrt(normal.x * normal.x + normal.y * normal.y);
            if (length == 0) continue;
            
            Vec2 normalized = {normal.x / length, normal.y / length};
            axes.push_back(normalized);
        }

        // SAT axis testing
        for (Vec2 axis : axes) {
            float triMin = std::numeric_limits<float>::infinity();
            float triMax = -std::numeric_limits<float>::infinity();
            float sqMin = std::numeric_limits<float>::infinity();
            float sqMax = -std::numeric_limits<float>::infinity();

            for (Vec2 vertex : triangleVerts) {
                float projection = dot(vertex, axis);
                triMin = std::min(triMin, projection);
                triMax = std::max(triMax, projection);
            }

            for (Vec2 vertex : squareVerts) {
                float projection = dot(vertex, axis);
                sqMin = std::min(sqMin, projection);
                sqMax = std::max(sqMax, projection);
            }

            if (triMax < sqMin || sqMax < triMin) {
                return false; // Separating axis found
            }
        }

        return true; // No separating axis → collision
    }

    else if (Triangle* t = dynamic_cast<Triangle*>(&shape)) {
        std::vector<Vec2> otherVerts = t->getVertices();

        // Other triangle edge normals
        for (int i = 0; i < otherVerts.size(); i++) {
            Vec2 edge = {
                otherVerts[(i + 1) % 3].x - otherVerts[i].x,
                otherVerts[(i + 1) % 3].y - otherVerts[i].y};
            Vec2 normal = {-edge.y, edge.x};
            
            float length = sqrt(normal.x * normal.x + normal.y * normal.y);
            if (length == 0) continue;

            Vec2 normalized = {normal.x / length, normal.y / length};
            axes.push_back(normalized);
        }       

        // This triangle edge normals - FIXED: use triangleVerts, not otherVerts
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
        
        // SAT projection testing
        for (Vec2 axis : axes) {
            float otherMin = std::numeric_limits<float>::infinity();
            float otherMax = -std::numeric_limits<float>::infinity();
            float thisMin = std::numeric_limits<float>::infinity();
            float thisMax = -std::numeric_limits<float>::infinity();

            for (Vec2 vertex : otherVerts) {
                float projection = dot(vertex, axis);

                if (projection < otherMin) {
                    otherMin = projection;
                }
                
                if (projection > otherMax) {
                    otherMax = projection;
                }
            }

            for (Vec2 vertex : triangleVerts) {
                float projection = dot(vertex, axis);

                if (projection < thisMin) {
                    thisMin = projection;
                }
                
                if (projection > thisMax) {
                    thisMax = projection;
                }
            }

            if (otherMax < thisMin || thisMax < otherMin) {
                return false;
            }
        }

        return true;
    }

    else if (Circle* c = dynamic_cast<Circle*>(&shape)) {
        std::vector<Vec2> verts = getVertices();

        for (int i = 0; i < verts.size(); i++) {
            Vec2 start = verts[i];
            Vec2 end = verts[(i + 1) % 3];

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
    
    return false; // Not a supported shape type
}