#include "PixelArenas.hpp"
#include <map>
#include <cmath>
#include <vector>

namespace CursedClash {

static std::map<ArenaId, Texture2D> arenaTextures;

void PixelArenas::ditherRect(int x, int y, int w, int h, Color c1, Color c2) {
    DrawRectangle(x, y, w, h, c1);
    for (int py = y; py < y + h; py += 2) {
        for (int px = x + (py % 4 == 0 ? 0 : 1); px < x + w; px += 2) {
            DrawRectangle(px, py, 1, 1, c2);
        }
    }
}

void PixelArenas::drawTorii(int x, int y, float scale, Color color, Color dark) {
    int w = std::round(44.0f * scale);
    int h = std::round(52.0f * scale);
    int pillarW = std::max(2, (int)std::round(4.0f * scale));
    DrawRectangle(x - (int)(w * 0.34f), y - h, pillarW, h, dark);
    DrawRectangle(x + (int)(w * 0.24f), y - h, pillarW, h, dark);
    DrawRectangle(x - (int)(w * 0.52f), y - h - 4, w + 4, std::max(3, (int)std::round(5.0f * scale)), color);
    DrawRectangle(x - (int)(w * 0.44f), y - h + 6, (int)(w * 0.88f), std::max(2, (int)std::round(3.0f * scale)), color);
}

static Image bakeJujutsuHigh() {
    Image img = GenImageColor(WORLD_WIDTH, WORLD_HEIGHT, hexToColor("#13162b"));
    // Dusk sky gradient
    for (int y = 0; y < (int)GROUND_Y; ++y) {
        float t = (float)y / GROUND_Y;
        Color c = (t < 0.55f) ? hexToColor("#3b2848") : hexToColor("#7a404c");
        ImageDrawRectangle(&img, 0, y, WORLD_WIDTH, 1, c);
    }
    // Sun / Moon halo
    ImageDrawRectangle(&img, 342, 36, 40, 40, Color{251, 191, 36, 50});
    ImageDrawRectangle(&img, 348, 42, 28, 28, hexToColor("#f59e0b"));
    ImageDrawRectangle(&img, 352, 46, 20, 20, hexToColor("#fde68a"));

    // Clouds
    ImageDrawRectangle(&img, 46, 48, 68, 8, hexToColor("#2d2142"));
    ImageDrawRectangle(&img, 212, 32, 74, 7, hexToColor("#2d2142"));

    // Mountains
    for (int x = 0; x < WORLD_WIDTH; x += 4) {
        int mh = 18 + (int)(std::sin(x * 0.025f) * 14.0f + std::cos(x * 0.06f) * 6.0f);
        ImageDrawRectangle(&img, x, (int)GROUND_Y - 58 - mh, 4, mh + 60, hexToColor("#221a35"));
    }

    // Traditional temple buildings
    ImageDrawRectangle(&img, 42, 134, 128, 98, hexToColor("#161224"));
    ImageDrawRectangle(&img, 310, 140, 126, 92, hexToColor("#161224"));
    for (int step = 0; step < 5; ++step) {
        Color tc = (step % 2 == 0) ? hexToColor("#0d0b18") : hexToColor("#1b172e");
        ImageDrawRectangle(&img, 24 + step * 8, 128 - step * 5, 164 - step * 16, 6, tc);
        ImageDrawRectangle(&img, 296 + step * 8, 134 - step * 5, 154 - step * 16, 6, tc);
    }

    // Warm shoji lit windows
    for (int wx = 54; wx < 154; wx += 22) {
        ImageDrawRectangle(&img, wx, 152, 12, 22, hexToColor("#fbbf24"));
        ImageDrawRectangle(&img, wx + 5, 152, 2, 22, hexToColor("#141122"));
        ImageDrawRectangle(&img, wx, 162, 12, 2, hexToColor("#141122"));
    }
    for (int wx = 324; wx < 420; wx += 22) {
        ImageDrawRectangle(&img, wx, 158, 12, 20, hexToColor("#fbbf24"));
        ImageDrawRectangle(&img, wx + 5, 158, 2, 20, hexToColor("#141122"));
        ImageDrawRectangle(&img, wx, 167, 12, 2, hexToColor("#141122"));
    }

    // Courtyard floor
    ImageDrawRectangle(&img, 0, (int)GROUND_Y, WORLD_WIDTH, WORLD_HEIGHT - (int)GROUND_Y, hexToColor("#2c2738"));
    ImageDrawRectangle(&img, 0, (int)GROUND_Y, WORLD_WIDTH, 3, hexToColor("#403952"));

    // Lanterns
    for (int lx : {178, 302}) {
        ImageDrawRectangle(&img, lx - 4, (int)GROUND_Y - 28, 8, 28, hexToColor("#3f3b4d"));
        ImageDrawRectangle(&img, lx - 7, (int)GROUND_Y - 30, 14, 4, hexToColor("#3f3b4d"));
        ImageDrawRectangle(&img, lx - 2, (int)GROUND_Y - 24, 4, 6, hexToColor("#fde047"));
    }

    return img;
}

static Image bakeAbandonedSchool() {
    Image img = GenImageColor(WORLD_WIDTH, WORLD_HEIGHT, hexToColor("#090d16"));
    for (int y = 0; y < (int)GROUND_Y; ++y) {
        ImageDrawRectangle(&img, 0, y, WORLD_WIDTH, 1, hexToColor("#182133"));
    }
    for (int wx : {44, 146, 268, 370}) {
        ImageDrawRectangle(&img, wx, 42, 64, 92, hexToColor("#2a3754"));
        ImageDrawRectangle(&img, wx + 4, 46, 56, 84, hexToColor("#0f1626"));
        ImageDrawRectangle(&img, wx + 8, 48, 22, 180, Color{147, 197, 253, 20});
        ImageDrawRectangle(&img, wx + 31, 46, 2, 84, hexToColor("#1a2234"));
        ImageDrawRectangle(&img, wx + 4, 86, 56, 2, hexToColor("#1a2234"));
    }
    // Moon
    ImageDrawRectangle(&img, 162, 56, 16, 16, hexToColor("#e2e8f0"));
    // Pillars & Talismans
    for (int px : {120, 240, 352}) {
        ImageDrawRectangle(&img, px - 8, 32, 16, (int)GROUND_Y - 32, hexToColor("#111622"));
        ImageDrawRectangle(&img, px - 2, 145, 4, 14, hexToColor("#991b1b"));
        ImageDrawRectangle(&img, px - 1, 147, 2, 10, hexToColor("#fbbf24"));
    }
    // Floor
    ImageDrawRectangle(&img, 0, (int)GROUND_Y, WORLD_WIDTH, WORLD_HEIGHT - (int)GROUND_Y, hexToColor("#1c2331"));
    ImageDrawRectangle(&img, 0, (int)GROUND_Y, WORLD_WIDTH, 2, hexToColor("#2d374c"));
    return img;
}

static Image bakeCursedForest() {
    Image img = GenImageColor(WORLD_WIDTH, WORLD_HEIGHT, hexToColor("#06110e"));
    for (int y = 0; y < (int)GROUND_Y; ++y) {
        ImageDrawRectangle(&img, 0, y, WORLD_WIDTH, 1, hexToColor("#1a3a2d"));
    }
    // Trees
    for (int tx = 14; tx < WORLD_WIDTH; tx += 38) {
        ImageDrawRectangle(&img, tx, 20, 14, (int)GROUND_Y - 20, hexToColor("#091813"));
    }
    // Trunks
    for (int tx : {22, 134, 290, 424}) {
        ImageDrawRectangle(&img, tx, 0, 26, (int)GROUND_Y, hexToColor("#1b1615"));
        ImageDrawRectangle(&img, tx + 4, 0, 8, (int)GROUND_Y, hexToColor("#2c2422"));
        ImageDrawRectangle(&img, tx + 2, (int)GROUND_Y - 38, 6, 12, hexToColor("#22c55e"));
    }
    // Canopy
    ImageDrawRectangle(&img, 0, 0, WORLD_WIDTH, 34, hexToColor("#0d261c"));
    // Floor
    ImageDrawRectangle(&img, 0, (int)GROUND_Y, WORLD_WIDTH, WORLD_HEIGHT - (int)GROUND_Y, hexToColor("#172e22"));
    ImageDrawRectangle(&img, 0, (int)GROUND_Y, WORLD_WIDTH, 2, hexToColor("#22c55e"));
    return img;
}

static Image bakeShibuya() {
    Image img = GenImageColor(WORLD_WIDTH, WORLD_HEIGHT, hexToColor("#05060f"));
    for (int y = 0; y < (int)GROUND_Y; ++y) {
        ImageDrawRectangle(&img, 0, y, WORLD_WIDTH, 1, hexToColor("#231934"));
    }
    // Skyscrapers
    struct Building { int x, w, h; Color c; };
    std::vector<Building> bldgs = {
        {0, 68, 175, hexToColor("#0d101f")},
        {64, 72, 198, hexToColor("#111528")},
        {140, 84, 156, hexToColor("#0b0e1a")},
        {230, 66, 205, hexToColor("#13172b")},
        {300, 88, 170, hexToColor("#0d101f")},
        {392, 88, 190, hexToColor("#111528")}
    };
    for (const auto& b : bldgs) {
        ImageDrawRectangle(&img, b.x, (int)GROUND_Y - b.h, b.w, b.h, b.c);
        for (int wy = (int)GROUND_Y - b.h + 12; wy < (int)GROUND_Y - 40; wy += 12) {
            for (int wx = b.x + 8; wx < b.x + b.w - 10; wx += 10) {
                if ((wx + wy) % 3 != 0) {
                    Color wc = ((wx * wy) % 5 == 0) ? hexToColor("#38bdf8") : hexToColor("#fef08a");
                    ImageDrawRectangle(&img, wx, wy, 4, 6, wc);
                }
            }
        }
    }
    // Neon signs
    ImageDrawRectangle(&img, 74, 86, 52, 26, hexToColor("#ec4899"));
    ImageDrawRectangle(&img, 77, 89, 46, 20, hexToColor("#090912"));
    ImageDrawRectangle(&img, 314, 98, 60, 24, hexToColor("#06b6d4"));
    ImageDrawRectangle(&img, 317, 101, 54, 18, hexToColor("#090912"));

    // Floor
    ImageDrawRectangle(&img, 0, (int)GROUND_Y, WORLD_WIDTH, WORLD_HEIGHT - (int)GROUND_Y, hexToColor("#141722"));
    ImageDrawRectangle(&img, 0, (int)GROUND_Y, WORLD_WIDTH, 2, hexToColor("#334155"));
    for (int sx = 24; sx < WORLD_WIDTH; sx += 56) {
        ImageDrawRectangle(&img, sx, (int)GROUND_Y + 8, 26, 14, hexToColor("#475569"));
    }
    return img;
}

static Image bakeMalevolentShrine() {
    Image img = GenImageColor(WORLD_WIDTH, WORLD_HEIGHT, hexToColor("#090204"));
    for (int y = 0; y < (int)GROUND_Y; ++y) {
        ImageDrawRectangle(&img, 0, y, WORLD_WIDTH, 1, hexToColor("#580a13"));
    }
    // Blood Moon
    ImageDrawRectangle(&img, 196, 18, 88, 88, hexToColor("#dc2626"));
    ImageDrawRectangle(&img, 204, 26, 72, 72, hexToColor("#ef4444"));

    // Shrine structure
    ImageDrawRectangle(&img, 156, (int)GROUND_Y - 38, 168, 38, hexToColor("#120608"));
    ImageDrawRectangle(&img, 174, (int)GROUND_Y - 52, 132, 14, hexToColor("#120608"));
    ImageDrawRectangle(&img, 184, (int)GROUND_Y - 122, 14, 70, hexToColor("#120608"));
    ImageDrawRectangle(&img, 282, (int)GROUND_Y - 122, 14, 70, hexToColor("#120608"));

    // Curved Pagoda Roofs
    ImageDrawRectangle(&img, 142, (int)GROUND_Y - 134, 196, 14, hexToColor("#1c080c"));
    ImageDrawRectangle(&img, 160, (int)GROUND_Y - 150, 160, 16, hexToColor("#1c080c"));
    ImageDrawRectangle(&img, 186, (int)GROUND_Y - 166, 108, 16, hexToColor("#1c080c"));
    ImageDrawRectangle(&img, 228, (int)GROUND_Y - 184, 24, 18, hexToColor("#1c080c"));

    // Skulls & Inner Maw
    ImageDrawRectangle(&img, 232, (int)GROUND_Y - 142, 16, 14, hexToColor("#d6cfc7"));
    ImageDrawRectangle(&img, 216, (int)GROUND_Y - 112, 48, 60, hexToColor("#991b1b"));
    ImageDrawRectangle(&img, 226, (int)GROUND_Y - 98, 28, 46, hexToColor("#dc2626"));

    // Floor
    ImageDrawRectangle(&img, 0, (int)GROUND_Y, WORLD_WIDTH, WORLD_HEIGHT - (int)GROUND_Y, hexToColor("#26060b"));
    ImageDrawRectangle(&img, 0, (int)GROUND_Y, WORLD_WIDTH, 2, hexToColor("#dc2626"));
    return img;
}

void PixelArenas::init() {}

void PixelArenas::cleanup() {
    for (auto& pair : arenaTextures) {
        if (pair.second.id > 0) UnloadTexture(pair.second);
    }
    arenaTextures.clear();
}

Texture2D PixelArenas::getArenaTexture(ArenaId id) {
    auto it = arenaTextures.find(id);
    if (it != arenaTextures.end()) {
        return it->second;
    }
    Image img;
    switch (id) {
        case ArenaId::JujutsuHigh: img = bakeJujutsuHigh(); break;
        case ArenaId::AbandonedSchool: img = bakeAbandonedSchool(); break;
        case ArenaId::CursedForest: img = bakeCursedForest(); break;
        case ArenaId::Shibuya: img = bakeShibuya(); break;
        case ArenaId::MalevolentShrine: img = bakeMalevolentShrine(); break;
    }
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    arenaTextures[id] = tex;
    return tex;
}

void PixelArenas::drawArenaAtmosphere(ArenaId arenaId, float timeSec, DomainTheme domainActive) {
    if (domainActive == DomainTheme::UnlimitedVoid) {
        DrawRectangle(0, 0, WORLD_WIDTH, (int)GROUND_Y, hexToColor("#040512"));
        for (int i = 0; i < 6; ++i) {
            float r = std::fmod(timeSec * 55.0f + i * 42.0f, 240.0f) + 12.0f;
            Color ringColor = (i % 2 == 0) ? Color{56, 189, 248, 100} : Color{248, 250, 252, 70};
            DrawCircleLines(240, 118, r, ringColor);
        }
        for (int s = 0; s < 45; ++s) {
            int sx = (s * 73 + (int)(timeSec * 18.0f)) % WORLD_WIDTH;
            int sy = (s * 41 + s * 17) % ((int)GROUND_Y - 10);
            DrawRectangle(sx, sy, 2, 2, (s % 2 == 0) ? hexToColor("#38bdf8") : WHITE);
        }
        return;
    }

    if (domainActive == DomainTheme::IronMountain) {
        DrawRectangle(0, 0, WORLD_WIDTH, (int)GROUND_Y, Color{120, 20, 5, 80});
        for (int e = 0; e < 35; ++e) {
            int ex = (e * 67 + (int)(timeSec * 25.0f)) % WORLD_WIDTH;
            int ey = (int)GROUND_Y - ((int)(timeSec * 60.0f + e * 37.0f) % (int)GROUND_Y);
            DrawRectangle(ex, ey, 2, 2, hexToColor("#fb923c"));
        }
        return;
    }

    if (domainActive == DomainTheme::MalevolentShrineTheme) {
        DrawRectangle(0, 0, WORLD_WIDTH, (int)GROUND_Y, Color{180, 10, 15, 60});
        for (int sl = 0; sl < 5; ++sl) {
            int sx = (sl * 115 + (int)(timeSec * 320.0f)) % WORLD_WIDTH;
            DrawLine(sx - 35, 20, sx + 35, (int)GROUND_Y, Color{239, 68, 68, 180});
        }
        return;
    }

    // Ambient arena particles
    if (arenaId == ArenaId::CursedForest) {
        for (int i = 0; i < 18; ++i) {
            int fx = (i * 47 + (int)(timeSec * 22.0f)) % WORLD_WIDTH;
            int fy = ((int)(timeSec * 35.0f + i * 29.0f)) % (int)GROUND_Y;
            DrawRectangle(fx, fy, 2, 2, Color{74, 222, 128, 120});
        }
    } else if (arenaId == ArenaId::JujutsuHigh) {
        drawTorii(240, (int)GROUND_Y - 12, 1.35f, hexToColor("#991b1b"), hexToColor("#450a0a"));
    }
}

} // namespace CursedClash
