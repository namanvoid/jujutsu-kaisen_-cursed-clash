#include "Renderer.hpp"
#include "PixelSprites.hpp"
#include "PixelArenas.hpp"
#include <cmath>
#include <algorithm>

namespace CursedClash {

static RenderTexture2D target{};
static bool initialized = false;

void Renderer::init(int virtualW, int virtualH) {
    if (initialized) return;
    target = LoadRenderTexture(virtualW, virtualH);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
    initialized = true;
}

void Renderer::cleanup() {
    if (initialized && target.id > 0) {
        UnloadRenderTexture(target);
        initialized = false;
    }
}

RenderTexture2D Renderer::getTarget() {
    return target;
}

void Renderer::drawShadow(const FighterState& f) {
    float heightRatio = std::max(0.35f, 1.0f - (GROUND_Y - f.y) / 140.0f);
    int w = std::round(28.0f * heightRatio);
    DrawRectangle((int)f.x - w / 2, (int)GROUND_Y - 1, w, 4, Color{ 0, 0, 0, 120 });
}

void Renderer::drawFighter(const FighterState& f, float timeSec) {
    SpritePose pose = PixelSprites::actionToPose(f.action, f.walkDist, timeSec);
    Texture2D sp = PixelSprites::getFighterTexture(f.id, pose, f.flashTimer > 0.0f);

    Rectangle src = { 0.0f, 0.0f, (float)sp.width * f.facing, (float)sp.height };
    Rectangle dst = { f.x, f.y, (float)sp.width * f.scaleX, (float)sp.height * f.scaleY };
    Vector2 origin = { 25.0f * f.scaleX, 59.0f * f.scaleY };

    // Boss / Buff aura
    if ((f.id == FighterId::Sukuna && f.bossPhase >= 2 && f.action != FighterAction::KO) || f.buffTimer > 0.0f) {
        Color aura = (f.id == FighterId::Panda) ? Color{245, 158, 11, 55} :
                     (f.bossPhase == 3) ? Color{220, 38, 38, 70} : Color{239, 68, 68, 45};
        DrawRectangle((int)f.x - 16, (int)f.y - 56, 32, 56, aura);
    }

    DrawTexturePro(sp, src, dst, origin, 0.0f, WHITE);

    // Gojo Infinity Barrier
    if (f.id == FighterId::Gojo && f.infinityTimer > 0.0f) {
        float pulse = 25.0f + std::sin(timeSec * 10.0f) * 2.0f;
        int cx = (int)f.x;
        int cy = (int)f.y - 28;
        DrawCircleLines(cx, cy, pulse, hexToColor("#38bdf8"));
        DrawCircle(cx, cy, pulse, Color{56, 189, 248, 30});
    }

    // Parry stance arc
    if (f.action == FighterAction::Parry && f.parryWindowTimer > 0.0f) {
        DrawCircleLines((int)f.x + f.facing * 8, (int)f.y - 28, 19.0f, hexToColor("#fde047"));
    }

    // Just-Parry ring
    if (f.parryFlashTimer > 0.0f) {
        float r = (0.28f - f.parryFlashTimer) * 95.0f + 10.0f;
        DrawCircleLines((int)f.x, (int)f.y - 28, r, WHITE);
    }
}

void Renderer::drawProjectiles(const std::vector<Projectile>& projectiles, float timeSec) {
    (void)timeSec;
    for (const auto& p : projectiles) {
        if (!p.active) continue;
        int x = (int)p.x;
        int y = (int)p.y;
        int r = (int)p.radius;

        switch (p.kind) {
            case ProjectileKind::Blue:
                DrawCircle(x, y, r + 5, Color{14, 165, 233, 90});
                DrawCircle(x, y, r, hexToColor("#0284c7"));
                DrawCircle(x, y, std::max(3, r / 2), hexToColor("#e0f2fe"));
                break;
            case ProjectileKind::Red:
                DrawCircle(x, y, r + 5, Color{239, 68, 68, 100});
                DrawCircle(x, y, r, hexToColor("#dc2626"));
                DrawCircle(x, y, std::max(3, r / 2), hexToColor("#fef2f2"));
                break;
            case ProjectileKind::Purple:
            case ProjectileKind::PureLove:
                DrawCircle(x, y, r + 7, Color{168, 85, 247, 90});
                DrawCircle(x, y, r, hexToColor("#7e22ce"));
                DrawCircle(x, y, std::max(5, r / 2), hexToColor("#f3e8ff"));
                break;
            case ProjectileKind::SukunaDismantle:
                DrawLineEx(Vector2{(float)x - 8, (float)y - r}, Vector2{(float)x + 6, (float)y}, 3.0f, hexToColor("#dc2626"));
                DrawLineEx(Vector2{(float)x + 6, (float)y}, Vector2{(float)x - 8, (float)y + r}, 3.0f, hexToColor("#dc2626"));
                DrawLine(x - 6, y - r + 2, x + 4, y, WHITE);
                DrawLine(x + 4, y, x - 6, y + r - 2, WHITE);
                break;
            case ProjectileKind::SukunaFlame:
                DrawCircle(x, y, r + 4, Color{249, 115, 22, 100});
                DrawCircle(x, y, r, hexToColor("#ea580c"));
                DrawCircle(x, y, std::max(3, r / 2), hexToColor("#fde047"));
                break;
            default:
                DrawCircle(x, y, r, hexToColor("#dc2626"));
                break;
        }
    }
}

void Renderer::drawHazards(const std::vector<GroundHazard>& hazards, float timeSec) {
    for (const auto& h : hazards) {
        if (!h.active) continue;
        int x = (int)h.x;
        int halfW = (int)h.width / 2;

        if (h.telegraphTimer > 0.0f) {
            bool blink = ((int)(timeSec * 18.0f) % 2 == 0);
            Color blinkColor = blink ? Color{239, 68, 68, 140} : Color{250, 204, 21, 110};
            DrawRectangle(x - halfW, (int)GROUND_Y - 3, (int)h.width, 5, blinkColor);
            continue;
        }

        switch (h.kind) {
            case HazardKind::JogoGeyser:
                DrawRectangle(x - halfW, (int)GROUND_Y - 62, (int)h.width, 62, hexToColor("#ea580c"));
                DrawRectangle(x - halfW / 2, (int)GROUND_Y - 58, std::max(4, halfW), 58, hexToColor("#fde047"));
                break;
            case HazardKind::HanamiRoots:
                DrawRectangle(x - halfW, (int)GROUND_Y - 46, (int)h.width, 46, hexToColor("#3f2e21"));
                DrawRectangle(x - 3, (int)GROUND_Y - 54, 6, 12, hexToColor("#22c55e"));
                break;
            case HazardKind::ShrineSlash:
                DrawLine(x - halfW, (int)GROUND_Y - 64, x + halfW, (int)GROUND_Y - 4, hexToColor("#ef4444"));
                DrawLine(x + halfW, (int)GROUND_Y - 64, x - halfW, (int)GROUND_Y - 4, hexToColor("#ef4444"));
                break;
            default:
                DrawRectangle(x - halfW, (int)GROUND_Y - 50, (int)h.width, 50, hexToColor("#581c87"));
                break;
        }
    }
}

void Renderer::renderWorld(const Engine& engine) {
    if (!initialized) init();

    BeginTextureMode(target);
    ClearBackground(BLACK);

    BeginMode2D(engine.camera);

    Texture2D arenaTex = PixelArenas::getArenaTexture(engine.arenaId);
    DrawTexture(arenaTex, 0, 0, WHITE);
    PixelArenas::drawArenaAtmosphere(engine.arenaId, engine.timeSec, engine.domainActive);

    drawShadow(engine.player);
    drawShadow(engine.enemy);

    for (const auto& ai : engine.afterimages) {
        if (!ai.active) continue;
        SpritePose pose = PixelSprites::actionToPose(ai.action, 0, engine.timeSec);
        Texture2D sp = PixelSprites::getFighterTexture(ai.fighterId, pose, false);
        Rectangle src = { 0.0f, 0.0f, (float)sp.width * ai.facing, (float)sp.height };
        Rectangle dst = { ai.x, ai.y, (float)sp.width, (float)sp.height };
        Vector2 origin = { 25.0f, 59.0f };
        DrawTexturePro(sp, src, dst, origin, 0.0f, ColorAlpha(ai.tint, ai.alpha));
    }

    drawFighter(engine.enemy, engine.timeSec);
    drawFighter(engine.player, engine.timeSec);

    drawHazards(engine.hazards, engine.timeSec);
    drawProjectiles(engine.projectiles, engine.timeSec);

    for (const auto& pt : engine.particles) {
        if (!pt.active) continue;
        int s = std::max(1, (int)pt.size);
        DrawRectangle((int)pt.x - s / 2, (int)pt.y - s / 2, s, s, pt.color);
    }

    for (const auto& c : engine.callouts) {
        if (!c.active) continue;
        int textW = MeasureText(c.text.c_str(), 10);
        DrawText(c.text.c_str(), (int)c.x - textW / 2 + 1, (int)c.y + 1, 10, BLACK);
        DrawText(c.text.c_str(), (int)c.x - textW / 2, (int)c.y, 10, c.color);
    }

    EndMode2D();

    if (engine.screenFlashAlpha > 0.01f) {
        DrawRectangle(0, 0, WORLD_WIDTH, WORLD_HEIGHT, ColorAlpha(engine.screenFlashColor, std::min(0.85f, engine.screenFlashAlpha)));
    }

    EndTextureMode();
}

void Renderer::drawWorldToScreen(int screenW, int screenH) {
    if (!initialized) return;
    float scale = std::min((float)screenW / WORLD_WIDTH, (float)screenH / WORLD_HEIGHT);
    Rectangle src = { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height };
    Rectangle dst = {
        (screenW - WORLD_WIDTH * scale) * 0.5f,
        (screenH - WORLD_HEIGHT * scale) * 0.5f,
        WORLD_WIDTH * scale,
        WORLD_HEIGHT * scale
    };
    DrawTexturePro(target.texture, src, dst, { 0.0f, 0.0f }, 0.0f, WHITE);
}

} // namespace CursedClash
