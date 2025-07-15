#include "Triangle.h"
#include "Square.h"
#include "Circle.h"
#include "Shape.h"

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
                              std::vector<std::shared_ptr<Shape>> shapeList,
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
            Vec2 center{ c->getX(), c->getY() };
            float radius = c->getRadius();

            float bestDistSq = std::numeric_limits<float>::max();
            Vec2 bestNormal{1,0};

            // find closest point on each triangle edge
            for (int i = 0; i < 3; ++i) {
                Vec2 A = vertices[i];
                Vec2 B = vertices[(i+1)%3];
                Vec2 AB{ B.x - A.x, B.y - A.y };
                Vec2 AC{ center.x - A.x, center.y - A.y };

                float t = (AB.x*AC.x + AB.y*AC.y) / (AB.x*AB.x + AB.y*AB.y);
                t = std::clamp(t, 0.0f, 1.0f);

                Vec2 proj{ A.x + t*AB.x, A.y + t*AB.y };
                float dx = center.x - proj.x, dy = center.y - proj.y;
                float distSq = dx*dx + dy*dy;

                if (distSq < bestDistSq) {
                    bestDistSq = distSq;
                    float d = std::sqrt(distSq);
                    if (d > 0.0001f) bestNormal = { dx/d, dy/d };
                    else            bestNormal = {1,0};
                }
            }

            float dist = std::sqrt(bestDistSq);
            float overlap = radius - dist;
            if (overlap > 0.001f) {
                // separate by half each
                float sep = overlap + 2.0f;
                Vec2 sepVec{ bestNormal.x*(sep*0.5f),
                             bestNormal.y*(sep*0.5f) };

                // move triangle
                Vec2 triOld{ getX(), getY() };
                setX(triOld.x + sepVec.x);
                setY(triOld.y + sepVec.y);

                // move circle
                Vec2 cirOld{ c->getX(), c->getY() };
                c->setX(cirOld.x - sepVec.x);
                c->setY(cirOld.y - sepVec.y);

                // velocity impulse
                float relVx = vx - c->getVx();
                float relVy = vy - c->getVy();
                float velAlong = relVx*bestNormal.x + relVy*bestNormal.y;
                if (velAlong < 0) {
                    float j = -(1 + elasticModifier)*velAlong;
                    j /= (1.0f/mass + 1.0f/c->getMass());
                    vx += j*bestNormal.x/mass;
                    vy += j*bestNormal.y/mass;
                    c->setVx(c->getVx() - j*bestNormal.x/c->getMass());
                    c->setVy(c->getVy() - j*bestNormal.y/c->getMass());
                }
            }

        // --- Polygon vs Triangle (SAT) ---
        } else {
            auto myVerts    = getVertices();
            auto otherVerts = shapePtr->getVertices();
            auto mtvOpt     = getMTV(myVerts, otherVerts);

            if (!mtvOpt) continue;
            Vec2 mtv = *mtvOpt;
            float len = std::sqrt(mtv.x*mtv.x + mtv.y*mtv.y);
            if (len <= 0.001f) continue;

            Vec2 normal{ mtv.x/len, mtv.y/len };
            float overlap = len + 2.0f;
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