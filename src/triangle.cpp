#include "Triangle.h"
#include "Square.h"
#include "Circle.h"
#include "Shape.h"

#include <algorithm>
#include <array>
#include <vector>
#include <iostream>
#include <limits>

static bool pointInTriangle(const Vec2& p, const Vec2& a, const Vec2& b, const Vec2& c) {
    auto sign = [](const Vec2& p1, const Vec2& p2, const Vec2& p3) {
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    };

    float d1 = sign(p, a, b);
    float d2 = sign(p, b, c);
    float d3 = sign(p, c, a);

    bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);
    return !(hasNeg && hasPos);
}

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
    SDL_Vertex verts[3];
    for (int i = 0; i < 3; ++i) {
        verts[i].position = SDL_FPoint{ vertices[i].x, vertices[i].y };
        verts[i].color = SDL_FColor{
        color.r / 255.0f,
        color.g / 255.0f,
        color.b / 255.0f,
        color.a / 255.0f 
};

        verts[i].tex_coord = SDL_FPoint{0, 0};
    }

    SDL_RenderGeometry(renderer, nullptr, verts, 3, nullptr, 0);
}


void Triangle::draw(SDL_Renderer* renderer, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    drawTriangle(renderer, color);
}

std::optional<Vec2> Triangle::getMTV(const std::vector<Vec2>& vertsA, const std::vector<Vec2>& vertsB) {
    float minOverlap = std::numeric_limits<float>::max();
    Vec2 mtvAxis;

    std::vector<Vec2> axes;

    // Get normals from triangle edges
    for (int i = 0; i < vertsA.size(); ++i) {
        Vec2 edge = {
            vertsA[(i + 1) % vertsA.size()].x - vertsA[i].x,
            vertsA[(i + 1) % vertsA.size()].y - vertsA[i].y
        };
        Vec2 normal = {-edge.y, edge.x};
        float length = std::sqrt(normal.x * normal.x + normal.y * normal.y);
        if (length > 0.001f)
            axes.push_back({normal.x / length, normal.y / length});
    }

    // Get normals from other shape edges
    for (int i = 0; i < vertsB.size(); ++i) {
        Vec2 edge = {
            vertsB[(i + 1) % vertsB.size()].x - vertsB[i].x,
            vertsB[(i + 1) % vertsB.size()].y - vertsB[i].y
        };
        Vec2 normal = {-edge.y, edge.x};
        float length = std::sqrt(normal.x * normal.x + normal.y * normal.y);
        if (length > 0.001f)
            axes.push_back({normal.x / length, normal.y / length});
    }

    for (const Vec2& axis : axes) {
        float minA = vertsA[0].x * axis.x + vertsA[0].y * axis.y;
        float maxA = minA;
        for (const Vec2& v : vertsA) {
            float proj = v.x * axis.x + v.y * axis.y;
            minA = std::min(minA, proj);
            maxA = std::max(maxA, proj);
        }

        float minB = vertsB[0].x * axis.x + vertsB[0].y * axis.y;
        float maxB = minB;
        for (const Vec2& v : vertsB) {
            float proj = v.x * axis.x + v.y * axis.y;
            minB = std::min(minB, proj);
            maxB = std::max(maxB, proj);
        }

        if (maxA < minB || maxB < minA)
            return std::nullopt;

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
void Triangle::checkCollision(int screenWidth,
                              int screenHeight,
                              const std::vector<std::shared_ptr<Shape>> &shapeList,
                              float elasticModifier)
{
    // 1) Boundary checks (unchanged)
    float minX = std::min({ vertices[0].x, vertices[1].x, vertices[2].x });
    float maxX = std::max({ vertices[0].x, vertices[1].x, vertices[2].x });
    float minY = std::min({ vertices[0].y, vertices[1].y, vertices[2].y });
    float maxY = std::max({ vertices[0].y, vertices[1].y, vertices[2].y });

    if (maxY >= screenHeight) {
        float dy = screenHeight - maxY;
        for (auto &v : vertices) v.y += dy;
        vy = -std::abs(vy) * elasticModifier;
    }
    if (minY <= 0) {
        float dy = -minY;
        for (auto &v : vertices) v.y += dy;
        vy = std::abs(vy) * elasticModifier;
    }
    if (maxX >= screenWidth) {
        float dx = screenWidth - maxX;
        for (auto &v : vertices) v.x += dx;
        vx = -std::abs(vx) * elasticModifier;
    }
    if (minX <= 0) {
        float dx = -minX;
        for (auto &v : vertices) v.x += dx;
        vx = std::abs(vx) * elasticModifier;
    }

    // 2) Shape–shape collisions
    for (auto &shapePtr : shapeList) {
        if (shapePtr.get() == this) continue;

        // --- Circle vs Triangle (edge-projection) ---

            if (auto *c = dynamic_cast<Circle*>(shapePtr.get())) {
                Vec2 circleCenter{ c->getX(), c->getY() };
                float radius = c->getRadius();
                
                // Find closest point on triangle to circle center
                float minDistSq = std::numeric_limits<float>::max();
                Vec2 closestPoint;
                
                // Check each edge of the triangle
                for (int i = 0; i < 3; ++i) {
                    Vec2 A = vertices[i];
                    Vec2 B = vertices[(i + 1) % 3];
                    
                    // Vector from A to B (edge)
                    Vec2 edge{ B.x - A.x, B.y - A.y };
                    // Vector from A to circle center
                    Vec2 toCircle{ circleCenter.x - A.x, circleCenter.y - A.y };
                    
                    // Project circle center onto edge
                    float edgeLengthSq = edge.x * edge.x + edge.y * edge.y;
                    float t = 0.0f;
                    if (edgeLengthSq > 0.0001f) {
                        t = (edge.x * toCircle.x + edge.y * toCircle.y) / edgeLengthSq;
                        t = std::clamp(t, 0.0f, 1.0f);  // Keep on edge segment
                    }
                    
                    // Closest point on this edge
                    Vec2 pointOnEdge{ A.x + t * edge.x, A.y + t * edge.y };
                    
                    // Distance squared from circle center to this point
                    float dx = circleCenter.x - pointOnEdge.x;
                    float dy = circleCenter.y - pointOnEdge.y;
                    float distSq = dx * dx + dy * dy;
                    
                    if (distSq < minDistSq) {
                        minDistSq = distSq;
                        closestPoint = pointOnEdge;
                    }
                }
                
                float dist = std::sqrt(minDistSq);
                bool centerInside = pointInTriangle(circleCenter, vertices[0], vertices[1], vertices[2]);
                
                // Check collision (including containment)
                if (dist < radius || centerInside) {
                    float overlap = radius - dist;
                    if (centerInside && dist >= radius) {
                        overlap = radius + dist;
                    }
                    
                    // Normal points FROM closest point TO circle center
                    float nx = 1.0f;
                    float ny = 0.0f;
                    if (dist > 0.001f) {
                        nx = (circleCenter.x - closestPoint.x) / dist;
                        ny = (circleCenter.y - closestPoint.y) / dist;
                    } else {
                        Vec2 centroid{
                            (vertices[0].x + vertices[1].x + vertices[2].x) / 3.0f,
                            (vertices[0].y + vertices[1].y + vertices[2].y) / 3.0f
                        };
                        float cdx = circleCenter.x - centroid.x;
                        float cdy = circleCenter.y - centroid.y;
                        float clen = std::sqrt(cdx * cdx + cdy * cdy);
                        if (clen > 0.001f) {
                            nx = cdx / clen;
                            ny = cdy / clen;
                        }
                    }
                    
                    // FIXED: Use direct vertex manipulation like boundary collision
                    float separation = overlap;
                    float triangleSeparation = separation * 0.5f;
                    float circleSeparation = separation * 0.5f;
                    
                    // Move triangle vertices directly (like boundary collision does)
                    for (auto &v : vertices) {
                        v.x -= nx * triangleSeparation;
                        v.y -= ny * triangleSeparation;
                    }
                    
                    // Move circle away from triangle
                    c->setX(circleCenter.x + nx * circleSeparation);
                    c->setY(circleCenter.y + ny * circleSeparation);
                    
                    // Velocity collision response (same as Square)
                    float relVelX = vx - c->getVx();
                    float relVelY = vy - c->getVy();
                    float velAlongNormal = relVelX * nx + relVelY * ny;
                    
                    if (velAlongNormal < 0) {  // Moving towards collision
                        float impulse = -(1 + elasticModifier) * velAlongNormal;
                        impulse /= (1.0f/mass + 1.0f/c->getMass());
                        
                        vx += impulse * nx / mass;
                        vy += impulse * ny / mass;
                        c->setVx(c->getVx() - impulse * nx / c->getMass());
                        c->setVy(c->getVy() - impulse * ny / c->getMass());
                    }
                }
            }  else {    
            if (this > shapePtr.get()) continue; // resolve each polygon pair once
            auto myVerts    = getVertices();
            auto otherVerts = shapePtr->getVertices();
            auto mtvOpt     = getMTV(myVerts, otherVerts);

            if (!mtvOpt) continue;
            Vec2 mtv = *mtvOpt;
            float len = std::sqrt(mtv.x*mtv.x + mtv.y*mtv.y);
            if (len <= 0.001f) continue;

            Vec2 normal{ mtv.x/len, mtv.y/len };
            float overlap = len;
            Vec2 sepVec{ normal.x*(overlap*0.5f),
                         normal.y*(overlap*0.5f) };

            // move triangle
            Vec2 triOld{ getX(), getY() };
            setX(triOld.x + sepVec.x);
            setY(triOld.y + sepVec.y);

            // move other shape
            Vec2 othOld{ shapePtr->getX(), shapePtr->getY() };
            shapePtr->setX(othOld.x - sepVec.x);
            shapePtr->setY(othOld.y - sepVec.y);

            // velocity impulse
            float relVx = vx - shapePtr->getVx();
            float relVy = vy - shapePtr->getVy();
            float velAlong = relVx*normal.x + relVy*normal.y;
            if (velAlong < 0) {
                float j = -(1 + elasticModifier)*velAlong;
                j /= (1.0f/mass + 1.0f/shapePtr->getMass());
                vx += j*normal.x/mass;
                vy += j*normal.y/mass;
                shapePtr->setVx(shapePtr->getVx() - j*normal.x/shapePtr->getMass());
                shapePtr->setVy(shapePtr->getVy() - j*normal.y/shapePtr->getMass());
            }
        }
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



void Triangle::setX(float x1) {
    float dx = x1 - getX();
    for (int i = 0; i < 3; ++i) vertices[i].x += dx;

}

void Triangle::setY(float y1) {
    float dy = y1 - getY();
    for (int i = 0; i < 3; ++i) vertices[i].y += dy;

}

float Triangle::getMass() const {return mass;}
