#include "PixelSprites.hpp"
#include <map>
#include <cmath>
#include <algorithm>

namespace CursedClash {

struct Palette {
    Color outline;
    Color skin;
    Color skinShadow;
    Color hair;
    Color hairHighlight;
    Color outfitMain;
    Color outfitShadow;
    Color outfitTrim;
    Color pants;
    Color pantsShadow;
    Color boots;
    Color accent;
    Color eye;
};

static std::map<FighterId, Palette> palettes;

PixelGrid::PixelGrid(int width, int height) : w(width), h(height), data(width * height, BLANK) {}

void PixelGrid::set(int x, int y, Color color) {
    if (x < 0 || x >= w || y < 0 || y >= h) return;
    data[y * w + x] = color;
}

Color PixelGrid::get(int x, int y) const {
    if (x < 0 || x >= w || y < 0 || y >= h) return BLANK;
    return data[y * w + x];
}

void PixelGrid::rect(int x, int y, int width, int height, Color color) {
    for (int dy = 0; dy < height; ++dy) {
        for (int dx = 0; dx < width; ++dx) {
            set(x + dx, y + dy, color);
        }
    }
}

void PixelGrid::line(int x0, int y0, int x1, int y1, Color color, int thickness) {
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        if (thickness > 1) {
            int off = thickness / 2;
            rect(x0 - off, y0 - off, thickness, thickness, color);
        } else {
            set(x0, y0, color);
        }
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
    }
}

void PixelGrid::applyOutline(Color outlineColor) {
    std::vector<Color> copy = data;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (copy[y * w + x].a != 0) continue;
            bool hasNeighbor =
                (x > 0 && copy[y * w + (x - 1)].a != 0) ||
                (x < w - 1 && copy[y * w + (x + 1)].a != 0) ||
                (y > 0 && copy[(y - 1) * w + x].a != 0) ||
                (y < h - 1 && copy[(y + 1) * w + x].a != 0);
            if (hasNeighbor) {
                data[y * w + x] = outlineColor;
            }
        }
    }
}

static void initPalettes() {
    if (!palettes.empty()) return;
    palettes[FighterId::Gojo] = {
        hexToColor("#090812"), hexToColor("#f6d5c3"), hexToColor("#d9a892"),
        hexToColor("#f1f5f9"), hexToColor("#ffffff"), hexToColor("#181e38"),
        hexToColor("#0d1021"), hexToColor("#38bdf8"), hexToColor("#12162b"),
        hexToColor("#090b16"), hexToColor("#1e293b"), hexToColor("#38bdf8"), hexToColor("#06b6d4")
    };
    palettes[FighterId::Yuta] = {
        hexToColor("#0a0814"), hexToColor("#f5d2c1"), hexToColor("#d4a18c"),
        hexToColor("#171923"), hexToColor("#2d3748"), hexToColor("#f1f5f9"),
        hexToColor("#cbd5e1"), hexToColor("#ec4899"), hexToColor("#121420"),
        hexToColor("#080910"), hexToColor("#1e293b"), hexToColor("#ec4899"), hexToColor("#334155")
    };
    palettes[FighterId::Megumi] = {
        hexToColor("#080914"), hexToColor("#f3d0be"), hexToColor("#cf9e89"),
        hexToColor("#161f38"), hexToColor("#2c3d66"), hexToColor("#1b2036"),
        hexToColor("#0f1222"), hexToColor("#818cf8"), hexToColor("#15192b"),
        hexToColor("#0a0c17"), hexToColor("#1e2436"), hexToColor("#6366f1"), hexToColor("#38bdf8")
    };
    palettes[FighterId::Panda] = {
        hexToColor("#0b0908"), hexToColor("#f8fafc"), hexToColor("#cbd5e1"),
        hexToColor("#18181b"), hexToColor("#3f3f46"), hexToColor("#f8fafc"),
        hexToColor("#cbd5e1"), hexToColor("#f59e0b"), hexToColor("#18181b"),
        hexToColor("#09090b"), hexToColor("#27272a"), hexToColor("#f59e0b"), hexToColor("#18181b")
    };
    palettes[FighterId::Jogo] = {
        hexToColor("#120806"), hexToColor("#8a8296"), hexToColor("#5e576b"),
        hexToColor("#4a4147"), hexToColor("#fb923c"), hexToColor("#3b332f"),
        hexToColor("#231d1a"), hexToColor("#f97316"), hexToColor("#282220"),
        hexToColor("#181412"), hexToColor("#1c1917"), hexToColor("#f97316"), hexToColor("#fde047")
    };
    palettes[FighterId::Mahito] = {
        hexToColor("#0c0814"), hexToColor("#c4c9de"), hexToColor("#939ab8"),
        hexToColor("#8fa4c9"), hexToColor("#cbd5e1"), hexToColor("#231c35"),
        hexToColor("#151022"), hexToColor("#a855f7"), hexToColor("#1a1528"),
        hexToColor("#0f0c18"), hexToColor("#1e1b2e"), hexToColor("#a855f7"), hexToColor("#93c5fd")
    };
    palettes[FighterId::Hanami] = {
        hexToColor("#07110b"), hexToColor("#6b656e"), hexToColor("#4a454d"),
        hexToColor("#e5e2dc"), hexToColor("#ffffff"), hexToColor("#2a332b"),
        hexToColor("#1a201b"), hexToColor("#22c55e"), hexToColor("#232b24"),
        hexToColor("#141915"), hexToColor("#2d241e"), hexToColor("#22c55e"), hexToColor("#dc2626")
    };
    palettes[FighterId::Choso] = {
        hexToColor("#120709"), hexToColor("#edd0c4"), hexToColor("#c7a294"),
        hexToColor("#1e1a24"), hexToColor("#3b3347"), hexToColor("#52404d"),
        hexToColor("#33252f"), hexToColor("#dc2626"), hexToColor("#3a2b35"),
        hexToColor("#241a20"), hexToColor("#231f20"), hexToColor("#ef4444"), hexToColor("#ef4444")
    };
    palettes[FighterId::Geto] = {
        hexToColor("#0c0816"), hexToColor("#f2cfc0"), hexToColor("#caa08e"),
        hexToColor("#181822"), hexToColor("#3b3b52"), hexToColor("#272038"),
        hexToColor("#161124"), hexToColor("#eab308"), hexToColor("#1b1628"),
        hexToColor("#0f0c18"), hexToColor("#f8fafc"), hexToColor("#8b5cf6"), hexToColor("#334155")
    };
    palettes[FighterId::Kenjaku] = {
        hexToColor("#0d0714"), hexToColor("#f1cebe"), hexToColor("#c89e8c"),
        hexToColor("#161620"), hexToColor("#37374d"), hexToColor("#1f192e"),
        hexToColor("#120e1c"), hexToColor("#c084fc"), hexToColor("#161121"),
        hexToColor("#0b0812"), hexToColor("#e2e8f0"), hexToColor("#c084fc"), hexToColor("#7e22ce")
    };
    palettes[FighterId::Uraume] = {
        hexToColor("#09111a"), hexToColor("#f8e2da"), hexToColor("#d8b4a8"),
        hexToColor("#f1f5f9"), hexToColor("#f43f5e"), hexToColor("#e2e8f0"),
        hexToColor("#94a3b8"), hexToColor("#38bdf8"), hexToColor("#9f1239"),
        hexToColor("#6b0b25"), hexToColor("#f8fafc"), hexToColor("#67e8f9"), hexToColor("#ef4444")
    };
    palettes[FighterId::Kashimo] = {
        hexToColor("#06111a"), hexToColor("#f0ccb8"), hexToColor("#c99f89"),
        hexToColor("#67e8f9"), hexToColor("#cffafe"), hexToColor("#f8fafc"),
        hexToColor("#cbd5e1"), hexToColor("#06b6d4"), hexToColor("#1e293b"),
        hexToColor("#0f172a"), hexToColor("#0f172a"), hexToColor("#38bdf8"), hexToColor("#22d3ee")
    };
    palettes[FighterId::Toji] = {
        hexToColor("#090b10"), hexToColor("#e5bfa8"), hexToColor("#bc9178"),
        hexToColor("#18181f"), hexToColor("#3f3f4e"), hexToColor("#1f242d"),
        hexToColor("#12151c"), hexToColor("#cbd5e1"), hexToColor("#e2e8f0"),
        hexToColor("#94a3b8"), hexToColor("#18181b"), hexToColor("#94a3b8"), hexToColor("#64748b")
    };
    palettes[FighterId::Sukuna] = {
        hexToColor("#120406"), hexToColor("#f2cbbd"), hexToColor("#c99b8d"),
        hexToColor("#f48fb1"), hexToColor("#f8bbd0"), hexToColor("#ebe3d5"),
        hexToColor("#bfb39f"), hexToColor("#dc2626"), hexToColor("#1a1721"),
        hexToColor("#0d0b12"), hexToColor("#2a1f1d"), hexToColor("#dc2626"), hexToColor("#ef4444")
    };
}

struct PoseOffsets {
    int bodyDx = 0, bodyDy = 0;
    int headDx = 0, headDy = 0;
    int legFrontDx = 5, legFrontDy = 0;
    int legBackDx = -5, legBackDy = 0;
    int armFrontX = 7, armFrontY = 28;
    int armBackX = -6, armBackY = 29;
    bool kickOut = false;
    bool punchOut = false;
    bool weaponSlash = false;
    bool domainSeal = false;
    bool knockedDown = false;
    bool guarding = false;
    bool parrying = false;
};

static PoseOffsets getPoseOffsets(SpritePose pose) {
    PoseOffsets o;
    switch (pose) {
        case SpritePose::Idle0:
            o.legFrontDx = 5; o.legBackDx = -5;
            o.armFrontX = 7; o.armFrontY = 28; o.armBackX = -6; o.armBackY = 29;
            break;
        case SpritePose::Idle1:
            o.bodyDy = 1; o.headDy = 1;
            o.legFrontDx = 5; o.legBackDx = -5;
            o.armFrontX = 8; o.armFrontY = 29; o.armBackX = -5; o.armBackY = 30;
            break;
        case SpritePose::Walk0:
            o.bodyDx = 1; o.headDx = 2;
            o.legFrontDx = 8; o.legFrontDy = -1; o.legBackDx = -6;
            o.armFrontX = 9; o.armFrontY = 27; o.armBackX = -7; o.armBackY = 29;
            break;
        case SpritePose::Walk1:
            o.bodyDx = 1; o.bodyDy = 1; o.headDx = 2; o.headDy = 1;
            o.legFrontDx = 3; o.legBackDx = -2; o.legBackDy = -1;
            o.armFrontX = 5; o.armFrontY = 29; o.armBackX = -5; o.armBackY = 28;
            break;
        case SpritePose::Walk2:
            o.bodyDx = 1; o.headDx = 2;
            o.legFrontDx = -5; o.legBackDx = 8; o.legBackDy = -1;
            o.armFrontX = -5; o.armFrontY = 29; o.armBackX = 9; o.armBackY = 27;
            break;
        case SpritePose::Walk3:
            o.bodyDx = 1; o.bodyDy = 1; o.headDx = 2; o.headDy = 1;
            o.legFrontDx = -2; o.legFrontDy = -1; o.legBackDx = 3;
            o.armFrontX = 4; o.armFrontY = 28; o.armBackX = -4; o.armBackY = 29;
            break;
        case SpritePose::Jump:
            o.bodyDy = -4; o.headDx = 1; o.headDy = -4;
            o.legFrontDx = 6; o.legFrontDy = -6; o.legBackDx = -5; o.legBackDy = -4;
            o.armFrontX = 8; o.armFrontY = 20; o.armBackX = -7; o.armBackY = 21;
            break;
        case SpritePose::Dash:
            o.bodyDx = 4; o.bodyDy = 2; o.headDx = 6; o.headDy = 2;
            o.legFrontDx = 9; o.legBackDx = -9; o.legBackDy = -2;
            o.armFrontX = 10; o.armFrontY = 27; o.armBackX = -10; o.armBackY = 26;
            break;
        case SpritePose::Sidestep:
            o.bodyDx = -2; o.bodyDy = 1; o.headDx = -2; o.headDy = 1;
            o.legFrontDx = 3; o.legFrontDy = -2; o.legBackDx = -3;
            o.armFrontX = 5; o.armFrontY = 24; o.armBackX = -5; o.armBackY = 26;
            break;
        case SpritePose::Block:
            o.bodyDx = -1; o.bodyDy = 2; o.headDx = -1; o.headDy = 2;
            o.legFrontDx = 5; o.legBackDx = -6;
            o.armFrontX = 7; o.armFrontY = 21; o.armBackX = 4; o.armBackY = 25;
            o.guarding = true;
            break;
        case SpritePose::Parry:
            o.bodyDx = 1; o.bodyDy = 1; o.headDx = 1; o.headDy = 1;
            o.legFrontDx = 6; o.legBackDx = -5;
            o.armFrontX = 10; o.armFrontY = 21; o.armBackX = 6; o.armBackY = 26;
            o.parrying = true;
            break;
        case SpritePose::Light1:
            o.bodyDx = 2; o.headDx = 3;
            o.legFrontDx = 6; o.legBackDx = -6;
            o.armFrontX = 17; o.armFrontY = 23; o.armBackX = -4; o.armBackY = 27;
            o.punchOut = true;
            break;
        case SpritePose::Light2:
            o.bodyDx = 4; o.bodyDy = 1; o.headDx = 4; o.headDy = 1;
            o.legFrontDx = 7; o.legBackDx = -6;
            o.armFrontX = 18; o.armFrontY = 21; o.armBackX = -2; o.armBackY = 28;
            o.punchOut = true;
            break;
        case SpritePose::Light3:
            o.bodyDx = 5; o.bodyDy = 1; o.headDx = 5; o.headDy = 1;
            o.legFrontDx = 8; o.legBackDx = -7;
            o.armFrontX = 20; o.armFrontY = 22; o.armBackX = 13; o.armBackY = 25;
            o.punchOut = true;
            break;
        case SpritePose::Heavy:
            o.bodyDx = 2; o.bodyDy = -1; o.headDx = 1; o.headDy = -1;
            o.legFrontDx = 18; o.legFrontDy = -13; o.legBackDx = -5;
            o.armFrontX = 9; o.armFrontY = 22; o.armBackX = -7; o.armBackY = 24;
            o.kickOut = true;
            break;
        case SpritePose::Special:
            o.bodyDx = 3; o.bodyDy = 1; o.headDx = 3; o.headDy = 1;
            o.legFrontDx = 7; o.legBackDx = -6;
            o.armFrontX = 18; o.armFrontY = 23; o.armBackX = -5; o.armBackY = 26;
            o.weaponSlash = true;
            break;
        case SpritePose::Ultimate:
            o.bodyDy = -2; o.headDy = -2;
            o.legFrontDx = 4; o.legFrontDy = -1; o.legBackDx = -4; o.legBackDy = -1;
            o.armFrontX = 6; o.armFrontY = 18; o.armBackX = -5; o.armBackY = 20;
            o.domainSeal = true;
            break;
        case SpritePose::Hitstun:
            o.bodyDx = -4; o.bodyDy = 1; o.headDx = -5; o.headDy = 1;
            o.legFrontDx = 4; o.legBackDx = -6;
            o.armFrontX = 4; o.armFrontY = 30; o.armBackX = -7; o.armBackY = 28;
            break;
        case SpritePose::KO:
            o.bodyDy = 18; o.headDx = -12; o.headDy = 26;
            o.legFrontDx = 12; o.legBackDx = 7;
            o.armFrontX = 6; o.armFrontY = 52; o.armBackX = -6; o.armBackY = 54;
            o.knockedDown = true;
            break;
    }
    return o;
}

PixelGrid PixelSprites::paintFighterGrid(FighterId id, SpritePose pose) {
    initPalettes();
    const auto& p = palettes[id];
    PixelGrid g(SPRITE_W, SPRITE_H);
    PoseOffsets o = getPoseOffsets(pose);
    int cx = 25;

    if (o.knockedDown) {
        g.rect(cx - 15, 50, 28, 7, p.outfitMain);
        g.rect(cx - 12, 53, 20, 4, p.outfitShadow);
        g.rect(cx + 6, 51, 12, 5, p.pants);
        g.rect(cx + 17, 51, 4, 5, p.boots);
        g.rect(cx - 21, 47, 9, 9, p.skin);
        g.rect(cx - 22, 44, 10, 5, p.hair);
        if (id == FighterId::Gojo) g.rect(cx - 21, 49, 8, 3, hexToColor("#0b0c15"));
        g.applyOutline(p.outline);
        return g;
    }

    int bulk = (id == FighterId::Hanami || id == FighterId::Panda) ? 3 : (id == FighterId::Sukuna ? 1 : 0);
    int hipY = 38 + o.bodyDy;
    int torsoY = 23 + o.bodyDy;
    int headY = 9 + o.headDy;
    int hx = cx + o.headDx;
    int bx = cx + o.bodyDx;

    // Back leg
    int blx = cx + o.legBackDx;
    int bly = 56 + o.legBackDy;
    g.line(bx - 3, hipY + 2, blx, bly - 5, p.pantsShadow, 5);
    g.rect(blx - 3, bly - 3, 6, 4, p.boots);

    // Back arm
    g.line(bx - 5 - bulk, torsoY + 3, cx + o.armBackX, o.armBackY, p.outfitShadow, 4);
    g.rect(cx + o.armBackX - 2, o.armBackY - 1, 4, 4, (id == FighterId::Panda) ? hexToColor("#18181b") : p.skinShadow);

    // Torso
    int torsoW = 12 + bulk * 2;
    int leftTorso = bx - torsoW / 2;
    g.rect(leftTorso, torsoY, torsoW, 16, p.outfitMain);
    g.rect(leftTorso, torsoY + 2, 4, 14, p.outfitShadow);
    g.rect(leftTorso + torsoW - 3, torsoY + 1, 2, 13, p.outfitMain);

    // Neck
    g.rect(bx - 2, torsoY - 2, 4, 3, p.skinShadow);

    // Specific outfit details
    if (id == FighterId::Gojo) {
        g.rect(bx - 6, torsoY - 3, 12, 4, p.outfitMain);
        g.set(bx + 2, torsoY + 4, hexToColor("#fbbf24"));
        g.set(bx + 2, torsoY + 9, hexToColor("#fbbf24"));
        g.rect(leftTorso, torsoY + 13, torsoW, 2, hexToColor("#090b16"));
    } else if (id == FighterId::Yuta) {
        g.rect(bx - 6, torsoY - 2, 12, 3, p.outfitShadow);
        g.rect(bx - 1, torsoY + 2, 3, 12, hexToColor("#0f172a"));
        g.line(bx - 10, hipY + 4, bx + 4, torsoY + 11, hexToColor("#1e293b"), 2);
        g.set(bx - 10, hipY + 4, hexToColor("#ec4899"));
    } else if (id == FighterId::Panda) {
        g.rect(leftTorso, torsoY, torsoW, 5, hexToColor("#18181b"));
        g.rect(leftTorso - 1, torsoY + 1, torsoW + 2, 3, hexToColor("#18181b"));
        g.rect(bx - 3, torsoY + 6, 7, 7, WHITE);
    } else if (id == FighterId::Sukuna) {
        g.rect(bx - 3, torsoY + 1, 7, 8, p.skin);
        g.set(bx - 2, torsoY + 3, hexToColor("#111827"));
        g.set(bx + 2, torsoY + 4, hexToColor("#111827"));
        g.rect(leftTorso, torsoY + 11, torsoW, 3, hexToColor("#18181b"));
        g.rect(leftTorso, torsoY + 14, torsoW, 2, p.outfitTrim);
    }

    // Front leg
    if (o.kickOut) {
        g.line(bx + 3, hipY + 1, cx + o.legFrontDx, hipY - 9, p.pants, 5);
        g.rect(cx + o.legFrontDx - 1, hipY - 11, 6, 5, p.boots);
        g.rect(cx + o.legFrontDx + 3, hipY - 12, 5, 4, p.accent);
    } else {
        int flx = cx + o.legFrontDx;
        int fly = 56 + o.legFrontDy;
        g.line(bx + 3, hipY + 1, flx, fly - 5, p.pants, 5);
        g.rect(flx - 2, fly - 3, 6, 4, p.boots);
    }

    // Head base
    int headW = (id == FighterId::Hanami || id == FighterId::Panda) ? 13 : 11;
    int headH = (id == FighterId::Jogo) ? 13 : 12;
    g.rect(hx - 5, headY, headW, headH, p.skin);
    g.rect(hx - 5, headY + 7, 3, 5, p.skinShadow);

    // Head details
    if (id == FighterId::Gojo) {
        g.rect(hx - 5, headY + 4, 12, 4, hexToColor("#09090f"));
        g.rect(hx - 4, headY + 5, 10, 1, hexToColor("#1e293b"));
        if (pose == SpritePose::Special || pose == SpritePose::Ultimate || pose == SpritePose::Parry) {
            g.rect(hx + 2, headY + 5, 3, 2, hexToColor("#38bdf8"));
            g.set(hx + 3, headY + 5, WHITE);
        }
        g.rect(hx - 6, headY - 4, 13, 6, p.hair);
        g.set(hx - 5, headY - 6, p.hairHighlight);
        g.set(hx - 1, headY - 7, p.hairHighlight);
        g.set(hx + 3, headY - 6, p.hairHighlight);
        g.set(hx + 6, headY - 5, p.hair);
    } else if (id == FighterId::Panda) {
        g.rect(hx - 6, headY - 3, 4, 4, hexToColor("#18181b"));
        g.rect(hx + 4, headY - 3, 4, 4, hexToColor("#18181b"));
        g.rect(hx - 1, headY + 3, 3, 3, hexToColor("#18181b"));
        g.rect(hx + 3, headY + 3, 3, 3, hexToColor("#18181b"));
        g.set(hx + 1, headY + 4, WHITE);
        g.set(hx + 4, headY + 4, WHITE);
        g.rect(hx + 1, headY + 7, 3, 2, hexToColor("#18181b"));
    } else if (id == FighterId::Sukuna) {
        g.rect(hx - 6, headY - 4, 13, 6, p.hair);
        g.set(hx - 5, headY - 6, p.hairHighlight);
        g.set(hx, headY - 7, p.hairHighlight);
        g.set(hx + 5, headY - 6, p.hair);
        g.set(hx + 1, headY + 3, hexToColor("#dc2626"));
        g.set(hx + 4, headY + 3, hexToColor("#dc2626"));
        g.set(hx + 1, headY + 5, hexToColor("#ef4444"));
        g.set(hx + 4, headY + 5, hexToColor("#ef4444"));
        g.rect(hx - 2, headY + 1, 3, 1, hexToColor("#18181b"));
        g.rect(hx - 4, headY + 7, 4, 2, hexToColor("#18181b"));
    } else {
        g.rect(hx - 6, headY - 3, 13, 6, p.hair);
        g.set(hx + 2, headY + 5, p.eye);
        g.set(hx + 4, headY + 5, p.eye);
    }

    // Front arm & weapons
    int armBaseX = bx + 4;
    int armBaseY = torsoY + 3;
    int fax = cx + o.armFrontX;
    int fay = o.armFrontY;

    if (o.domainSeal) {
        g.line(armBaseX, armBaseY, fax, fay, p.outfitMain, 4);
        g.rect(fax - 1, fay - 5, 4, 6, (id == FighterId::Panda) ? hexToColor("#18181b") : p.skin);
        g.rect(fax - 3, fay - 8, 7, 4, p.accent);
    } else if (o.parrying) {
        g.line(armBaseX, armBaseY, fax, fay, p.outfitMain, 4);
        g.rect(fax - 1, fay - 2, 4, 4, (id == FighterId::Panda) ? hexToColor("#18181b") : p.skin);
        g.line(fax + 3, fay - 9, fax + 6, fay + 7, hexToColor("#fde047"), 2);
        g.line(fax + 4, fay - 7, fax + 7, fay + 5, WHITE, 1);
    } else if (o.punchOut) {
        g.line(armBaseX, armBaseY, fax, fay, p.outfitMain, 4);
        g.rect(fax - 1, fay - 2, 6, 5, (id == FighterId::Panda) ? hexToColor("#18181b") : p.skin);
        if (id == FighterId::Yuta) {
            g.line(fax + 2, fay, fax + 14, fay - 5, hexToColor("#e2e8f0"), 2);
            g.line(fax + 4, fay + 1, fax + 15, fay - 4, p.accent, 1);
        } else {
            g.rect(fax + 4, fay - 2, 4, 5, p.accent);
        }
    } else if (o.weaponSlash) {
        g.line(armBaseX, armBaseY, fax, fay, p.outfitMain, 4);
        if (id == FighterId::Toji || id == FighterId::Yuta) {
            g.line(fax, fay, fax + 16, fay - 6, hexToColor("#f8fafc"), 2);
        } else if (id == FighterId::Sukuna) {
            g.rect(fax - 1, fay - 2, 5, 5, p.skin);
            g.line(fax + 3, fay - 7, fax + 14, fay + 5, hexToColor("#ef4444"), 2);
        } else {
            g.rect(fax - 1, fay - 2, 5, 5, (id == FighterId::Panda) ? hexToColor("#18181b") : p.skin);
            g.rect(fax + 4, fay - 3, 5, 6, p.accent);
        }
    } else {
        g.line(armBaseX, armBaseY, fax, fay, p.outfitMain, 4);
        g.rect(fax - 1, fay - 1, 4, 4, (id == FighterId::Panda) ? hexToColor("#18181b") : p.skin);
    }

    if (o.guarding) {
        g.rect(bx + 5, torsoY - 4, 3, 18, p.accent);
    }

    g.applyOutline(p.outline);
    return g;
}

SpritePose PixelSprites::actionToPose(FighterAction action, float walkDist, float timeSec) {
    switch (action) {
        case FighterAction::Idle:
            return ((int)std::floor(timeSec * 3.2f) % 2 == 0) ? SpritePose::Idle0 : SpritePose::Idle1;
        case FighterAction::Walk: {
            int idx = (int)std::floor(walkDist / 7.0f) % 4;
            if (idx == 0) return SpritePose::Walk0;
            if (idx == 1) return SpritePose::Walk1;
            if (idx == 2) return SpritePose::Walk2;
            return SpritePose::Walk3;
        }
        case FighterAction::Jump: return SpritePose::Jump;
        case FighterAction::Dash: return SpritePose::Dash;
        case FighterAction::Sidestep: return SpritePose::Sidestep;
        case FighterAction::Block: return SpritePose::Block;
        case FighterAction::Parry: return SpritePose::Parry;
        case FighterAction::Light1: return SpritePose::Light1;
        case FighterAction::Light2: return SpritePose::Light2;
        case FighterAction::Light3: return SpritePose::Light3;
        case FighterAction::Heavy: return SpritePose::Heavy;
        case FighterAction::Skill1:
        case FighterAction::Skill2:
        case FighterAction::Skill3:
        case FighterAction::EnemySpecial1:
        case FighterAction::EnemySpecial2:
        case FighterAction::EnemySpecial3:
            return SpritePose::Special;
        case FighterAction::Skill4:
        case FighterAction::Skill5Domain:
        case FighterAction::EnemyUltimate:
        case FighterAction::Victory:
            return SpritePose::Ultimate;
        case FighterAction::Hitstun:
            return SpritePose::Hitstun;
        case FighterAction::Knockdown:
        case FighterAction::KO:
            return SpritePose::KO;
    }
    return SpritePose::Idle0;
}

static std::map<std::string, Texture2D> normalCache;
static std::map<std::string, Texture2D> flashCache;

void PixelSprites::init() {
    initPalettes();
}

void PixelSprites::cleanup() {
    for (auto& pair : normalCache) {
        if (pair.second.id > 0) UnloadTexture(pair.second);
    }
    normalCache.clear();
    for (auto& pair : flashCache) {
        if (pair.second.id > 0) UnloadTexture(pair.second);
    }
    flashCache.clear();
}

Texture2D PixelSprites::getFighterTexture(FighterId id, SpritePose pose, bool whiteFlash) {
    std::string key = std::to_string((int)id) + ":" + std::to_string((int)pose);
    auto& cache = whiteFlash ? flashCache : normalCache;
    auto it = cache.find(key);
    if (it != cache.end()) {
        return it->second;
    }

    PixelGrid grid = paintFighterGrid(id, pose);
    Image img = GenImageColor(grid.w, grid.h, BLANK);
    for (int y = 0; y < grid.h; ++y) {
        for (int x = 0; x < grid.w; ++x) {
            Color c = grid.get(x, y);
            if (c.a > 0) {
                ImageDrawPixel(&img, x, y, whiteFlash ? WHITE : c);
            }
        }
    }
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    cache[key] = tex;
    return tex;
}

} // namespace CursedClash
