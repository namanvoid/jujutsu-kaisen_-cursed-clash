#pragma once

#include "Types.hpp"
#include <raylib.h>
#include <vector>

namespace CursedClash {

constexpr int SPRITE_W = 56;
constexpr int SPRITE_H = 64;

enum class SpritePose {
    Idle0,
    Idle1,
    Walk0,
    Walk1,
    Walk2,
    Walk3,
    Jump,
    Dash,
    Sidestep,
    Block,
    Parry,
    Light1,
    Light2,
    Light3,
    Heavy,
    Special,
    Ultimate,
    Hitstun,
    KO
};

class PixelGrid {
public:
    int w = SPRITE_W;
    int h = SPRITE_H;
    std::vector<Color> data;

    PixelGrid(int width = SPRITE_W, int height = SPRITE_H);
    void set(int x, int y, Color color);
    Color get(int x, int y) const;
    void rect(int x, int y, int width, int height, Color color);
    void line(int x0, int y0, int x1, int y1, Color color, int thickness = 1);
    void applyOutline(Color outlineColor);
};

class PixelSprites {
public:
    static void init();
    static void cleanup();
    static SpritePose actionToPose(FighterAction action, float walkDist, float timeSec);
    static Texture2D getFighterTexture(FighterId id, SpritePose pose, bool whiteFlash = false);
    static PixelGrid paintFighterGrid(FighterId id, SpritePose pose);
};

} // namespace CursedClash
