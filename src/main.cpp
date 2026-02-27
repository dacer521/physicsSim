#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_events.h>
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <random>
#include <string>
#include <unordered_map>

#include "Shape.h"
#include "Square.h"
#include "Circle.h"
#include "Triangle.h"

using namespace std;

enum class GameState {
    Select,
    Play,
    Dead
};

enum class ShapeType {
    Triangle,
    Circle,
    Square
};

static shared_ptr<Shape> makeTriangle(float cx, float cy, float size, float vx, float vy, float mass, SDL_Color color) {
    Vec2 v1{cx, cy - size};
    Vec2 v2{cx - size, cy + size};
    Vec2 v3{cx + size, cy + size};
    return make_shared<Triangle>(v1, v2, v3, mass, vx, vy, color);
}

static shared_ptr<Shape> makeSquare(float x, float y, float size, float vx, float vy, float mass) {
    return make_shared<Square>(x, y, size, size, vx, vy, mass);
}

static shared_ptr<Shape> makeCircle(float x, float y, float radius, float vx, float vy, float mass) {
    return make_shared<Circle>(x, y, radius, vx, vy, mass);
}

static shared_ptr<Shape> makeShape(ShapeType type, float x, float y, float size, float vx, float vy, float mass) {
    switch (type) {
        case ShapeType::Triangle:
            return makeTriangle(x, y, size, vx, vy, mass, SDL_Color{255, 0, 0, 255});
        case ShapeType::Circle:
            return makeCircle(x, y, size, vx, vy, mass);
        case ShapeType::Square:
        default:
            return makeSquare(x - size, y - size, size * 2.0f, vx, vy, mass);
    }
}

static const unordered_map<char, array<uint8_t, 7>> kFont = {
    {'A',{0x0E,0x11,0x11,0x1F,0x11,0x11,0x11}},
    {'B',{0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E}},
    {'C',{0x0E,0x11,0x10,0x10,0x10,0x11,0x0E}},
    {'D',{0x1E,0x11,0x11,0x11,0x11,0x11,0x1E}},
    {'E',{0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F}},
    {'F',{0x1F,0x10,0x10,0x1E,0x10,0x10,0x10}},
    {'G',{0x0E,0x11,0x10,0x17,0x11,0x11,0x0E}},
    {'H',{0x11,0x11,0x11,0x1F,0x11,0x11,0x11}},
    {'I',{0x0E,0x04,0x04,0x04,0x04,0x04,0x0E}},
    {'J',{0x07,0x02,0x02,0x02,0x12,0x12,0x0C}},
    {'K',{0x11,0x12,0x14,0x18,0x14,0x12,0x11}},
    {'L',{0x10,0x10,0x10,0x10,0x10,0x10,0x1F}},
    {'M',{0x11,0x1B,0x15,0x15,0x11,0x11,0x11}},
    {'N',{0x11,0x19,0x15,0x13,0x11,0x11,0x11}},
    {'O',{0x0E,0x11,0x11,0x11,0x11,0x11,0x0E}},
    {'P',{0x1E,0x11,0x11,0x1E,0x10,0x10,0x10}},
    {'Q',{0x0E,0x11,0x11,0x11,0x15,0x12,0x0D}},
    {'R',{0x1E,0x11,0x11,0x1E,0x14,0x12,0x11}},
    {'S',{0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E}},
    {'T',{0x1F,0x04,0x04,0x04,0x04,0x04,0x04}},
    {'U',{0x11,0x11,0x11,0x11,0x11,0x11,0x0E}},
    {'V',{0x11,0x11,0x11,0x11,0x11,0x0A,0x04}},
    {'W',{0x11,0x11,0x11,0x15,0x15,0x15,0x0A}},
    {'X',{0x11,0x11,0x0A,0x04,0x0A,0x11,0x11}},
    {'Y',{0x11,0x11,0x0A,0x04,0x04,0x04,0x04}},
    {'Z',{0x1F,0x01,0x02,0x04,0x08,0x10,0x1F}},
    {'0',{0x0E,0x11,0x13,0x15,0x19,0x11,0x0E}},
    {'1',{0x04,0x0C,0x04,0x04,0x04,0x04,0x0E}},
    {'2',{0x0E,0x11,0x01,0x02,0x04,0x08,0x1F}},
    {'3',{0x1F,0x02,0x04,0x02,0x01,0x11,0x0E}},
    {'4',{0x02,0x06,0x0A,0x12,0x1F,0x02,0x02}},
    {'5',{0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E}},
    {'6',{0x06,0x08,0x10,0x1E,0x11,0x11,0x0E}},
    {'7',{0x1F,0x01,0x02,0x04,0x08,0x08,0x08}},
    {'8',{0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E}},
    {'9',{0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C}},
    {':',{0x00,0x04,0x04,0x00,0x04,0x04,0x00}},
    {'.',{0x00,0x00,0x00,0x00,0x00,0x0C,0x0C}},
    {'-',{0x00,0x00,0x00,0x1F,0x00,0x00,0x00}},
    {'/',{0x01,0x02,0x04,0x08,0x10,0x00,0x00}},
    {' ',{0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
};

static void drawText(SDL_Renderer* renderer, float x, float y, int scale, SDL_Color color, const string& text) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    float cursorX = x;
    for (char c : text) {
        char up = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        auto it = kFont.find(up);
        if (it == kFont.end()) {
            cursorX += (6 * scale);
            continue;
        }
        const auto& glyph = it->second;
        for (int row = 0; row < 7; ++row) {
            for (int col = 0; col < 5; ++col) {
                if (glyph[row] & (1 << (4 - col))) {
                    SDL_FRect pixel{cursorX + col * scale, y + row * scale, (float)scale, (float)scale};
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }
        }
        cursorX += (6 * scale);
    }
}

static void drawSelectionScreen(SDL_Renderer* renderer, int width, int height, int selectedIndex) {
    float spacing = width / 4.0f;
    float centerY = height * 0.5f;
    float size = std::min(width, height) * 0.08f;

    float x1 = spacing;
    float x2 = spacing * 2.0f;
    float x3 = spacing * 3.0f;

    Triangle tri({x1, centerY - size}, {x1 - size, centerY + size}, {x1 + size, centerY + size}, 1.0f, 0.0f, 0.0f, SDL_Color{255, 0, 0, 255});
    Circle circ(x2, centerY, size, 0.0f, 0.0f, 1.0f);
    Square sq(x3 - size, centerY - size, size * 2.0f, size * 2.0f, 0.0f, 0.0f, 1.0f);

    tri.draw(renderer, SDL_Color{255, 0, 0, 255});
    circ.draw(renderer, SDL_Color{0, 0, 255, 255});
    sq.draw(renderer, SDL_Color{0, 255, 0, 255});

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_FRect highlight{};
    if (selectedIndex == 0) {
        highlight = SDL_FRect{ x1 - size * 1.3f, centerY - size * 1.3f, size * 2.6f, size * 2.6f };
    } else if (selectedIndex == 1) {
        highlight = SDL_FRect{ x2 - size * 1.3f, centerY - size * 1.3f, size * 2.6f, size * 2.6f };
    } else {
        highlight = SDL_FRect{ x3 - size * 1.3f, centerY - size * 1.3f, size * 2.6f, size * 2.6f };
    }
    SDL_RenderRect(renderer, &highlight);

    int scale = 2;
    float textY = height * 0.12f;
    drawText(renderer, width * 0.5f - 120, textY, scale, SDL_Color{255,255,255,255}, "SELECT A SHAPE");
    drawText(renderer, width * 0.5f - 170, textY + 20, scale, SDL_Color{200,200,200,255}, "A/D OR LEFT/RIGHT");
    drawText(renderer, width * 0.5f - 170, textY + 38, scale, SDL_Color{200,200,200,255}, "ENTER OR SPACE TO START");
    drawText(renderer, width * 0.5f - 170, textY + 56, scale, SDL_Color{200,200,200,255}, "1 TRI 2 CIRC 3 SQR");

    drawText(renderer, width * 0.5f - 150, height * 0.78f, scale, SDL_Color{200,200,200,255}, "MOVE: WASD OR ARROWS");
    drawText(renderer, width * 0.5f - 150, height * 0.78f + 18, scale, SDL_Color{200,200,200,255}, "AVOID ALL ENEMIES");
}

static bool pointInTriangle2D(const Vec2& p, const Vec2& a, const Vec2& b, const Vec2& c) {
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

static bool shapesOverlap(Shape* a, Shape* b) {
    if (!a || !b) return false;

    if (auto* ca = dynamic_cast<Circle*>(a)) {
        if (auto* cb = dynamic_cast<Circle*>(b)) {
            float dx = ca->getX() - cb->getX();
            float dy = ca->getY() - cb->getY();
            float distSq = dx * dx + dy * dy;
            float r = ca->getRadius() + cb->getRadius();
            return distSq <= r * r;
        }
        if (auto* sb = dynamic_cast<Square*>(b)) {
            float closestX = std::max(sb->getX(), std::min(ca->getX(), sb->getX() + sb->getWidth()));
            float closestY = std::max(sb->getY(), std::min(ca->getY(), sb->getY() + sb->getHeight()));
            float dx = ca->getX() - closestX;
            float dy = ca->getY() - closestY;
            float distSq = dx * dx + dy * dy;
            float r = ca->getRadius();
            return distSq <= r * r;
        }
        if (auto* tb = dynamic_cast<Triangle*>(b)) {
            auto verts = tb->getVertices();
            Vec2 center{ ca->getX(), ca->getY() };
            float radius = ca->getRadius();

            float minDistSq = std::numeric_limits<float>::max();
            Vec2 closest{};
            for (int i = 0; i < 3; ++i) {
                Vec2 A = verts[i];
                Vec2 B = verts[(i + 1) % 3];
                Vec2 edge{ B.x - A.x, B.y - A.y };
                Vec2 toCenter{ center.x - A.x, center.y - A.y };
                float edgeLenSq = edge.x * edge.x + edge.y * edge.y;
                float t = 0.0f;
                if (edgeLenSq > 0.0001f) {
                    t = (edge.x * toCenter.x + edge.y * toCenter.y) / edgeLenSq;
                    t = std::clamp(t, 0.0f, 1.0f);
                }
                Vec2 pointOnEdge{ A.x + t * edge.x, A.y + t * edge.y };
                float dx = center.x - pointOnEdge.x;
                float dy = center.y - pointOnEdge.y;
                float distSq = dx * dx + dy * dy;
                if (distSq < minDistSq) {
                    minDistSq = distSq;
                    closest = pointOnEdge;
                }
            }

            bool inside = pointInTriangle2D(center, verts[0], verts[1], verts[2]);
            return inside || (minDistSq <= radius * radius);
        }

        vector<Vec2> cVerts = ca->getVertices();
        vector<Vec2> oVerts = b->getVertices();
        return b->getMTV(cVerts, oVerts).has_value();
    }

    if (auto* cb = dynamic_cast<Circle*>(b)) {
        return shapesOverlap(b, a);
    }

    vector<Vec2> aVerts = a->getVertices();
    vector<Vec2> bVerts = b->getVertices();
    return a->getMTV(aVerts, bVerts).has_value();
}

static void drawDeathScreen(SDL_Renderer* renderer, int width, int height, float timeSurvived) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    int scale = 3;
    drawText(renderer, width * 0.5f - 60, height * 0.4f, scale, SDL_Color{255, 50, 50, 255}, "YOU DIED");

    char buf[64];
    snprintf(buf, sizeof(buf), "TIME: %.2F S", timeSurvived);
    drawText(renderer, width * 0.5f - 80, height * 0.5f, 2, SDL_Color{255,255,255,255}, buf);
    drawText(renderer, width * 0.5f - 100, height * 0.62f, 2, SDL_Color{200,200,200,255}, "R TO RESTART");
    drawText(renderer, width * 0.5f - 100, height * 0.62f + 18, 2, SDL_Color{200,200,200,255}, "ESC TO QUIT");
}

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        cerr << "SDL_Init Error: " << SDL_GetError() << endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Physics Sim", 640, 480, SDL_WINDOW_RESIZABLE);
    if (!window) {
        cerr << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    int width = 640, height = 480;
    GameState state = GameState::Select;
    int selectedIndex = 0;
    vector<ShapeType> options = {ShapeType::Triangle, ShapeType::Circle, ShapeType::Square};

    vector<shared_ptr<Shape>> shapeList;
    vector<shared_ptr<Shape>> npcs;
    shared_ptr<Shape> player;

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<float> dirDist(-1.0f, 1.0f);
    std::uniform_real_distribution<float> speedDist(200.0f, 320.0f);
    std::uniform_int_distribution<int> typeDist(0, 2);

    float elapsed = 0.0f;
    float nextSpawnTime = 60.0f;
    int spawnCount = 1;

    float timeSurvived = 0.0f;

    bool running = true;
    const int FPS = 60;
    const float FRAME_TARGET_TIME = 1000.0f / FPS;
    Uint64 last_ticks = SDL_GetTicks();

    cout << "Select shape: 1=Triangle 2=Circle 3=Square (or A/D and Enter)." << endl;

    while (running) {
        Uint64 current_ticks = SDL_GetTicks();
        float delta_time = (current_ticks - last_ticks) / 1000.0f;
        if (delta_time > 0.05f) delta_time = 0.05f;
        last_ticks = current_ticks;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_KEY_DOWN && state == GameState::Select) {
                SDL_Scancode sc = event.key.scancode;
                if (sc == SDL_SCANCODE_LEFT || sc == SDL_SCANCODE_A) {
                    selectedIndex = (selectedIndex + 2) % 3;
                } else if (sc == SDL_SCANCODE_RIGHT || sc == SDL_SCANCODE_D) {
                    selectedIndex = (selectedIndex + 1) % 3;
                } else if (sc == SDL_SCANCODE_1) {
                    selectedIndex = 0;
                    sc = SDL_SCANCODE_RETURN;
                } else if (sc == SDL_SCANCODE_2) {
                    selectedIndex = 1;
                    sc = SDL_SCANCODE_RETURN;
                } else if (sc == SDL_SCANCODE_3) {
                    selectedIndex = 2;
                    sc = SDL_SCANCODE_RETURN;
                }

                if (sc == SDL_SCANCODE_RETURN || sc == SDL_SCANCODE_SPACE) {
                    float startX = width * 0.25f;
                    float startY = height * 0.5f;
                    float size = std::min(width, height) * 0.06f;

                    player = makeShape(options[selectedIndex], startX, startY, size, 0.0f, 0.0f, 1.0f);

                    ShapeType npcType = static_cast<ShapeType>(typeDist(rng));
                    float npcX = width * 0.75f;
                    float npcY = height * 0.5f;
                    float dx = dirDist(rng);
                    float dy = dirDist(rng);
                    float mag = std::sqrt(dx * dx + dy * dy);
                    if (mag < 0.001f) mag = 1.0f;
                    dx /= mag;
                    dy /= mag;
                    float speed = speedDist(rng);
                    auto npc = makeShape(npcType, npcX, npcY, size, dx * speed, dy * speed, 1.0f);

                    shapeList.clear();
                    npcs.clear();
                    shapeList.push_back(player);
                    shapeList.push_back(npc);
                    npcs.push_back(npc);

                    elapsed = 0.0f;
                    nextSpawnTime = 60.0f;
                    spawnCount = 1;
                    timeSurvived = 0.0f;

                    state = GameState::Play;
                }
            } else if (event.type == SDL_EVENT_KEY_DOWN && state == GameState::Dead) {
                SDL_Scancode sc = event.key.scancode;
                if (sc == SDL_SCANCODE_R) {
                    state = GameState::Select;
                } else if (sc == SDL_SCANCODE_ESCAPE) {
                    running = false;
                }
            }
        }

        SDL_GetWindowSize(window, &width, &height);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (state == GameState::Select) {
            drawSelectionScreen(renderer, width, height, selectedIndex);
        } else if (state == GameState::Dead) {
            drawDeathScreen(renderer, width, height, timeSurvived);
        } else {
            elapsed += delta_time;
            timeSurvived = elapsed;

            if (elapsed >= nextSpawnTime) {
                float size = std::min(width, height) * 0.06f;
                for (int i = 0; i < spawnCount; ++i) {
                    ShapeType npcType = static_cast<ShapeType>(typeDist(rng));
                    float npcX = width * 0.7f + (i * 20.0f);
                    float npcY = height * 0.3f + (i * 30.0f);
                    float dx = dirDist(rng);
                    float dy = dirDist(rng);
                    float mag = std::sqrt(dx * dx + dy * dy);
                    if (mag < 0.001f) mag = 1.0f;
                    dx /= mag;
                    dy /= mag;
                    float speed = speedDist(rng);
                    auto npc = makeShape(npcType, npcX, npcY, size, dx * speed, dy * speed, 1.0f);
                    shapeList.push_back(npc);
                    npcs.push_back(npc);
                }
                spawnCount += 1;
                nextSpawnTime += 60.0f;
            }

            const bool* keys = SDL_GetKeyboardState(nullptr);
            float playerSpeed = 220.0f;
            float vx = 0.0f;
            float vy = 0.0f;
            if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) vy -= playerSpeed;
            if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) vy += playerSpeed;
            if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT]) vx -= playerSpeed;
            if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) vx += playerSpeed;

            if (player) {
                player->setVx(vx);
                player->setVy(vy);
            }

            for (auto& shape : shapeList) {
                shape->update(delta_time);
            }

            bool hit = false;
            for (auto& npc : npcs) {
                if (shapesOverlap(player.get(), npc.get())) {
                    hit = true;
                    break;
                }
            }

            if (hit) {
                state = GameState::Dead;
            } else {
                for (auto& shape : shapeList) {
                    shape->checkCollision(width, height, shapeList, 0.9f);
                }

                for (auto& npc : npcs) {
                    auto verts = npc->getVertices();
                    float minX = verts[0].x, maxX = verts[0].x;
                    float minY = verts[0].y, maxY = verts[0].y;
                    for (const auto& v : verts) {
                        minX = std::min(minX, v.x);
                        maxX = std::max(maxX, v.x);
                        minY = std::min(minY, v.y);
                        maxY = std::max(maxY, v.y);
                    }
                    bool hitWall = (minX <= 0.5f) || (maxX >= width - 0.5f) || (minY <= 0.5f) || (maxY >= height - 0.5f);
                    if (hitWall) {
                        float vxn = npc->getVx();
                        float vyn = npc->getVy();
                        float speed = std::sqrt(vxn * vxn + vyn * vyn);
                        if (speed < 0.001f) speed = speedDist(rng);
                        float jitter = 0.25f;
                        float dx = vxn / speed + dirDist(rng) * jitter;
                        float dy = vyn / speed + dirDist(rng) * jitter;
                        float mag = std::sqrt(dx * dx + dy * dy);
                        if (mag < 0.001f) mag = 1.0f;
                        dx /= mag;
                        dy /= mag;
                        npc->setVx(dx * speed);
                        npc->setVy(dy * speed);
                    }
                }

                for (auto& shape : shapeList) {
                    if (dynamic_cast<Triangle*>(shape.get()))
                        shape->draw(renderer, SDL_Color{255, 0, 0, 255});
                    else if (dynamic_cast<Square*>(shape.get()))
                        shape->draw(renderer, SDL_Color{0, 255, 0, 255});
                    else if (dynamic_cast<Circle*>(shape.get()))
                        shape->draw(renderer, SDL_Color{0, 0, 255, 255});
                }
            }
        }

        SDL_RenderPresent(renderer);

        float frame_time = SDL_GetTicks() - current_ticks;
        if (frame_time < FRAME_TARGET_TIME) {
            SDL_Delay((Uint32)(FRAME_TARGET_TIME - frame_time));
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
