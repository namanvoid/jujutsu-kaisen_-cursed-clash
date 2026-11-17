#pragma once

#include "Engine.hpp"
#include <raylib.h>

namespace CursedClash {

class Renderer {
public:
    static void init(int virtualW = WORLD_WIDTH, int virtualH = WORLD_HEIGHT);
    static void cleanup();
    static void renderWorld(const Engine& engine);
    static void drawWorldToScreen(int screenW, int screenH);
    static RenderTexture2D getTarget();

private:
    static void drawShadow(const FighterState& f);
    static void drawFighter(const FighterState& f, float timeSec);
    static void drawProjectiles(const std::vector<Projectile>& projectiles, float timeSec);
    static void drawHazards(const std::vector<GroundHazard>& hazards, float timeSec);
};

} // namespace CursedClash
