// Triangle.h
#pragma once
#include <SDL3/SDL.h>
#include <cmath>
#include "Shape.h"
#include <array>
#include <vector>
#include <optional>



class Triangle : public Shape {
public:
    std::array<Vec2, 3> vertices;
    float vx, vy;
    float mass;
    SDL_Color color;

    Triangle(Vec2 v1, Vec2 v2, Vec2 v3, float mass, float vx, float vy, SDL_Color color);

    void update(float deltaTime) override;
    void applyForce(float fx, float fy) override;
    void drawTriangle(SDL_Renderer* renderer, SDL_Color color) const;
    void draw(SDL_Renderer* renderer, SDL_Color color) override;
    void checkCollision(int screenWidth, int screenHeight, std::vector<std::shared_ptr<Shape>> shapeList, float elasticModifier) override;

    float getVy() const override;
    void setVy(float v) override;

    float getVx() const override;
    void setVx(float v) override;

    float getMass() const override;

    // Return centroid X and Y for correct separation
    float getX() const override;
    float getY() const override;

    Vec2 getV1() const;
    Vec2 getV2() const;
    Vec2 getV3() const;

    std::vector<Vec2> getVertices() const override;

    float getWidth() const override;
    float getHeight() const override;

    void setX(float x1) override;
    void setY(float y1) override;

    std::optional<Vec2> getMTV(const std::vector<Vec2>& vertsA, const std::vector<Vec2>& vertsB) override;
};
