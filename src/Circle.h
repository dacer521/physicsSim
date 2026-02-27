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

    void checkCollision(int screenWidth, int screenHeight, const std::vector<std::shared_ptr<Shape>> &shapeList, float elasticModifier) override;


    float getVx() const override;
    void setVx(float v) override;

    float getVy() const override;
    void setVy(float v) override;

    float getX() const override;
    float getY() const override;
    float getHeight() const override;
    float getWidth() const override;

    float getRadius();

    std::vector<Vec2> getVertices() const override;



    void setX(float x1) override;
    void setY(float y1) override;

    std::optional<Vec2> getMTV(const std::vector<Vec2>& vertsA, const std::vector<Vec2>& vertsB) override; 

    float getMass() const override;

private:
    float x, y;
    float r;
    float vx, vy;
    float mass;
};

#endif
