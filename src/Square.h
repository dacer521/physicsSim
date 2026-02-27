
#ifndef SQUARE_H
#define SQUARE_H

#include "Shape.h"
#include <SDL.h>
#include <vector>
#include <optional>

using namespace std;

class Square : public Shape {
public:
    Square(float x, float y, float w, float h, float vx, float vy, float mass);

    

    void update(float deltaTime) override;
    void applyForce(float fx, float fy) override;
    void draw(SDL_Renderer* renderer, SDL_Color color) override;

    void checkCollision(int screenWidth, int screenHeight, const std::vector<std::shared_ptr<Shape>> &shapeList, float elasticModifier) override;
    std::optional<Vec2> getMTV(const std::vector<Vec2>& vertsA, const std::vector<Vec2>& vertsB) override;

    float getVx() const override;
    void setVx(float v) override;

    float getVy() const override;
    void setVy(float v) override;

    float getMass() const override;

    void setX(float x) override;
    void setY(float y) override;

    std::vector<Vec2> getVertices() const override;


    float getX() const override;
    float getY() const override;
    float getHeight() const override;
    float getWidth() const override;

    float x, y;
    float width, height;
    float vx, vy;
    float mass;
};

#endif
