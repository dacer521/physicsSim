#include "Triangle.h"
#include <algorithm>

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

void Triangle::drawTriangle(SDL_Renderer* renderer) const {
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

void Triangle::draw(SDL_Renderer* renderer) const {
    drawTriangle(renderer);
}