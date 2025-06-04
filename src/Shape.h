#ifndef SHAPE_H
#define SHAPE_H

#include <SDL_render.h>
#include <vector>

struct Vec2 {
    float x, y;
};

class Shape {
public:
    virtual ~Shape() = default;

    virtual void update(float deltaTime) = 0;
    virtual void applyForce(float fx, float fy) = 0;
     virtual void draw(SDL_Renderer* renderer, SDL_Color color = {255, 255, 255, 255}) = 0;

    virtual void checkCollision(int screenWidth, int screenHeight) = 0;

    virtual bool collideShape(Shape &shape) = 0;

    virtual float getVx() const = 0;
    virtual void setVx(float v) = 0;

    virtual float getVy() const = 0;
    virtual void setVy(float v) = 0;

    virtual float getX() const = 0;
    virtual float getY() const = 0;

    virtual float getHeight() const = 0;
    virtual float getWidth() const = 0;

    virtual float dot(const Vec2& a, const Vec2& b) { return a.x * b.x + a.y * b.y; }

    virtual std::vector<Vec2> getVertices() const = 0;

};

#endif