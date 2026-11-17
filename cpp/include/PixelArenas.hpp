#pragma once

#include "Types.hpp"
#include <raylib.h>

namespace CursedClash {

class PixelArenas {
public:
    static void init();
    static void cleanup();
    static Texture2D getArenaTexture(ArenaId id);
    static void drawArenaAtmosphere(ArenaId arenaId, float timeSec, DomainTheme domainActive);
    static void drawTorii(int x, int y, float scale, Color color, Color dark);
    static void ditherRect(int x, int y, int w, int h, Color c1, Color c2);
};

} // namespace CursedClash
