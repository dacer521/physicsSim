#ifndef CIRCLE_H
#define CIRCLE_H

#include "Shape.h"
#include <SDL.h>
#include <vector>

class Circle : public Shape {
public:
    Circle(float x, float y, float r, float vx, float vy, float mass);

    void update(float deltaTime) override;
    void applyForce(float fx, float fy) override;
    void draw(SDL_Renderer* renderer, SDL_Color color) override;

    void checkCollision(int screenWidth, int screenHeight) override;

    float getVx() const override;
    void setVx(float v) override;

    float getVy() const override;
    void setVy(float v) override;

    float getX() const override;
    float getY() const override;
    float getHeight() const override;
    float getWidth() const override;

    std::vector<Vec2> getVertices() const override;
    bool collideShape(Shape& other) override;

private:
    float x, y;
    float r;
    float vx, vy;
    float mass;
};

#endif