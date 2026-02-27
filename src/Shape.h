#ifndef SHAPE_H
#define SHAPE_H

#include <SDL_render.h>
#include <vector>
#include <optional>


using std::optional;
using namespace std;

struct Vec2 {
    float x, y;
};



class Shape {
public:
    virtual ~Shape() = default;

    virtual void update(float deltaTime) = 0;
    virtual void applyForce(float fx, float fy) = 0;
     virtual void draw(SDL_Renderer* renderer, SDL_Color color = {255, 255, 255, 255}) = 0;

    virtual float getMass() const = 0;

    virtual void checkCollision(int screenWidth, int screenHeight, const vector<shared_ptr<Shape>> &shapeList, float elasticModifier) = 0;

    virtual optional<Vec2> getMTV(const vector<Vec2>& vertsA, const vector<Vec2>& vertsB) = 0;
    
    virtual float getVx() const = 0;
    virtual void setVx(float v) = 0;

    virtual float getVy() const = 0;
    virtual void setVy(float v) = 0;

    virtual float getX() const = 0;
    virtual float getY() const = 0;

    virtual void setX(float x) = 0;
    virtual void setY(float y) = 0;

    virtual float getHeight() const = 0;
    virtual float getWidth() const = 0;

    virtual float dot(const Vec2& a, const Vec2& b) { return a.x * b.x + a.y * b.y; }

    virtual std::vector<Vec2> getVertices() const = 0;

};


struct CollisionResult {
    Vec2 mtv;                // Minimum Translation Vector
    float depth;             // Overlap amount
    Vec2 normal;             // Normal of the collision
    Shape* otherShape = nullptr; // Optional: useful for force/impulse later
};



#endif
