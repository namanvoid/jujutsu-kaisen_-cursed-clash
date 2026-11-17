#include "UI.hpp"
#include "Roster.hpp"
#include "PixelSprites.hpp"
#include "Audio.hpp"
#include <cmath>
#include <algorithm>
#include <string>

namespace CursedClash {

static int selectedHeroIdx = 0;
static int selectedEnemyIdx = 4; // Jogo
static int selectedArenaIdx = 0;

void UI::init() {}

void UI::draw(Engine& engine, int screenW, int screenH) {
    if (engine.screenState == GameScreenState::Title) {
        drawTitleScreen(engine, screenW, screenH);
    } else if (engine.screenState == GameScreenState::Playing) {
        drawCombatHud(engine, screenW, screenH);
    } else if (engine.screenState == GameScreenState::Paused) {
        drawCombatHud(engine, screenW, screenH);
        drawPauseModal(engine, screenW, screenH);
    } else if (engine.screenState == GameScreenState::GameOver) {
        drawCombatHud(engine, screenW, screenH);
        drawMatchResult(engine, screenW, screenH);
    }
}

void UI::drawTitleScreen(Engine& engine, int screenW, int screenH) {
    const auto& playables = Roster::getPlayableList();
    const auto& arenas = Roster::getArenaList();

    // Inputs on title screen
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        selectedHeroIdx = (selectedHeroIdx - 1 + (int)playables.size()) % (int)playables.size();
        Audio::playSfx(SoundEffect::UiClick);
        engine.setTitleHero(playables[selectedHeroIdx]);
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        selectedHeroIdx = (selectedHeroIdx + 1) % (int)playables.size();
        Audio::playSfx(SoundEffect::UiClick);
        engine.setTitleHero(playables[selectedHeroIdx]);
    }
    if (IsKeyPressed(KEY_TAB)) {
        selectedArenaIdx = (selectedArenaIdx + 1) % (int)arenas.size();
        Audio::playSfx(SoundEffect::UiClick);
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ONE)) {
        Audio::playSfx(SoundEffect::UiClick);
        engine.startStoryMode(0, playables[selectedHeroIdx]);
        return;
    }
    if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_TWO)) {
        Audio::playSfx(SoundEffect::UiClick);
        engine.startPracticeMode(playables[selectedHeroIdx], playables[selectedEnemyIdx], arenas[selectedArenaIdx]);
        return;
    }

    FighterId currentHero = playables[selectedHeroIdx];
    const auto& prof = Roster::getProfile(currentHero);

    // Dim overlay
    DrawRectangle(0, 0, screenW, screenH, Color{ 5, 6, 15, 180 });

    // Title text
    const char* title = "JUJUTSU KAISEN: CURSED CLASH";
    int titleW = MeasureText(title, 28);
    DrawText(title, (screenW - titleW) / 2 + 2, 38, 28, BLACK);
    DrawText(title, (screenW - titleW) / 2, 36, 28, hexToColor("#38bdf8"));

    const char* sub = "2D PIXEL FIGHTING GAME — C++ & RAYLIB PORT";
    int subW = MeasureText(sub, 14);
    DrawText(sub, (screenW - subW) / 2, 72, 14, hexToColor("#94a3b8"));

    // Character Card Frame
    int cardW = 460;
    int cardH = 240;
    int cardX = (screenW - cardW) / 2;
    int cardY = 100;

    DrawRectangle(cardX, cardY, cardW, cardH, Color{ 15, 18, 32, 230 });
    DrawRectangleLines(cardX, cardY, cardW, cardH, prof.accentColor);

    // Sprite preview
    Texture2D sp = PixelSprites::getFighterTexture(currentHero, SpritePose::Idle0, false);
    Rectangle src = { 0, 0, (float)sp.width, (float)sp.height };
    Rectangle dst = { (float)cardX + 25.0f, (float)cardY + 30.0f, (float)sp.width * 2.5f, (float)sp.height * 2.5f };
    DrawTexturePro(sp, src, dst, { 0, 0 }, 0.0f, WHITE);

    // Character info
    int textX = cardX + 175;
    DrawText(prof.name.c_str(), textX, cardY + 20, 22, WHITE);
    DrawText(prof.title.c_str(), textX, cardY + 48, 12, prof.accentColor);

    DrawText(("Max HP: " + std::to_string(prof.maxHp) + "   Speed: " + std::to_string((int)prof.speed)).c_str(), textX, cardY + 70, 12, hexToColor("#cbd5e1"));
    DrawText(("Domain: " + prof.domainName).c_str(), textX, cardY + 90, 12, hexToColor("#fde047"));

    DrawText("Style:", textX, cardY + 115, 12, hexToColor("#94a3b8"));
    DrawText(prof.styleSummary.c_str(), textX, cardY + 130, 11, WHITE);

    DrawText("Quote:", textX, cardY + 165, 12, hexToColor("#94a3b8"));
    DrawText(("\"" + prof.introQuote + "\"").c_str(), textX, cardY + 180, 11, hexToColor("#f1f5f9"));

    // Arrow navigation helpers
    DrawText("< [A / LEFT]", cardX - 110, cardY + 105, 14, prof.accentColor);
    DrawText("[D / RIGHT] >", cardX + cardW + 15, cardY + 105, 14, prof.accentColor);

    // Menu options at bottom
    int menuY = cardY + cardH + 25;
    const char* opt1 = "[ENTER] START STORY MODE (10 STAGES)";
    const char* opt2 = "[P] FREE PRACTICE MODE";
    const char* opt3 = "[TAB] SELECT ARENA: ";

    DrawRectangle((screenW - 400) / 2, menuY, 400, 32, Color{ 24, 30, 52, 220 });
    DrawRectangleLines((screenW - 400) / 2, menuY, 400, 32, hexToColor("#38bdf8"));
    int opt1W = MeasureText(opt1, 14);
    DrawText(opt1, (screenW - opt1W) / 2, menuY + 9, 14, WHITE);

    DrawRectangle((screenW - 400) / 2, menuY + 40, 400, 30, Color{ 18, 22, 38, 220 });
    int opt2W = MeasureText(opt2, 13);
    DrawText(opt2, (screenW - opt2W) / 2, menuY + 48, 13, hexToColor("#cbd5e1"));

    std::string arenaText = std::string(opt3) + Roster::getArena(arenas[selectedArenaIdx]).name;
    int opt3W = MeasureText(arenaText.c_str(), 12);
    DrawText(arenaText.c_str(), (screenW - opt3W) / 2, menuY + 80, 12, hexToColor("#94a3b8"));

    const char* ctrlHelp = "CONTROLS: WASD / Arrows to Move | Space: Dash | S: Dodge | L: Block | Q/F: Parry | J: Light | K: Heavy | U, I, O, H, Y: Skills & Domain";
    int ctrlW = MeasureText(ctrlHelp, 10);
    DrawText(ctrlHelp, (screenW - ctrlW) / 2, screenH - 22, 10, hexToColor("#64748b"));
}

void UI::drawCombatHud(const Engine& engine, int screenW, int screenH) {
    HudSnapshot s = engine.getHudSnapshot();

    int barW = std::min(360, screenW / 2 - 80);
    int barH = 18;
    int barY = 22;

    // Player 1 HP Bar (Left)
    int pBarX = 40;
    DrawRectangle(pBarX - 2, barY - 2, barW + 4, barH + 4, Color{ 10, 10, 18, 220 });
    DrawRectangleLines(pBarX - 2, barY - 2, barW + 4, barH + 4, hexToColor("#334155"));

    float pGhostRatio = std::clamp(s.playerGhostHp / (float)s.playerMaxHp, 0.0f, 1.0f);
    DrawRectangle(pBarX, barY, (int)(barW * pGhostRatio), barH, hexToColor("#ef4444"));

    float pRatio = std::clamp((float)s.playerHp / (float)s.playerMaxHp, 0.0f, 1.0f);
    Color pHpColor = (pRatio > 0.5f) ? hexToColor("#22c55e") : (pRatio > 0.25f) ? hexToColor("#eab308") : hexToColor("#dc2626");
    DrawRectangle(pBarX, barY, (int)(barW * pRatio), barH, pHpColor);

    // Player name & rounds
    DrawText(s.playerName.c_str(), pBarX, barY - 16, 12, WHITE);
    for (int r = 0; r < 2; ++r) {
        Color rc = (r < s.playerRounds) ? hexToColor("#f59e0b") : hexToColor("#334155");
        DrawCircle(pBarX + barW - 24 + r * 16, barY - 9, 5, rc);
    }

    // Player CE Bar
    int ceW = (int)(barW * 0.75f);
    int ceH = 8;
    int ceY = barY + barH + 3;
    DrawRectangle(pBarX, ceY, ceW, ceH, Color{ 15, 23, 42, 200 });
    float pCeRatio = std::clamp(s.playerCe / 100.0f, 0.0f, 1.0f);
    DrawRectangle(pBarX, ceY, (int)(ceW * pCeRatio), ceH, hexToColor("#38bdf8"));
    if (s.playerInfinity > 0.0f) {
        DrawText("INFINITY ACTIVE", pBarX + ceW + 8, ceY - 1, 10, hexToColor("#38bdf8"));
    }

    // Enemy HP Bar (Right)
    int eBarX = screenW - 40 - barW;
    DrawRectangle(eBarX - 2, barY - 2, barW + 4, barH + 4, Color{ 10, 10, 18, 220 });
    DrawRectangleLines(eBarX - 2, barY - 2, barW + 4, barH + 4, hexToColor("#334155"));

    float eGhostRatio = std::clamp(s.enemyGhostHp / (float)s.enemyMaxHp, 0.0f, 1.0f);
    int eGhostW = (int)(barW * eGhostRatio);
    DrawRectangle(eBarX + (barW - eGhostW), barY, eGhostW, barH, hexToColor("#ef4444"));

    float eRatio = std::clamp((float)s.enemyHp / (float)s.enemyMaxHp, 0.0f, 1.0f);
    int eFillW = (int)(barW * eRatio);
    Color eHpColor = (eRatio > 0.5f) ? hexToColor("#22c55e") : (eRatio > 0.25f) ? hexToColor("#eab308") : hexToColor("#dc2626");
    DrawRectangle(eBarX + (barW - eFillW), barY, eFillW, barH, eHpColor);

    // Enemy name & rounds
    int eNameW = MeasureText(s.enemyName.c_str(), 12);
    DrawText(s.enemyName.c_str(), eBarX + barW - eNameW, barY - 16, 12, WHITE);
    for (int r = 0; r < 2; ++r) {
        Color rc = (r < s.enemyRounds) ? hexToColor("#f59e0b") : hexToColor("#334155");
        DrawCircle(eBarX + 10 + r * 16, barY - 9, 5, rc);
    }

    // Enemy CE Bar
    int eCeW = (int)(barW * 0.75f);
    int eCeX = eBarX + (barW - eCeW);
    DrawRectangle(eCeX, ceY, eCeW, ceH, Color{ 15, 23, 42, 200 });
    float eCeRatio = std::clamp(s.enemyCe / 100.0f, 0.0f, 1.0f);
    int eCeFillW = (int)(eCeW * eCeRatio);
    DrawRectangle(eCeX + (eCeW - eCeFillW), ceY, eCeFillW, ceH, hexToColor("#ef4444"));

    // Center Timer
    std::string timeStr = (s.mode == GameMode::Practice) ? "INF" : std::to_string((int)std::ceil(s.roundTimer));
    int timeW = MeasureText(timeStr.c_str(), 24);
    DrawRectangle(screenW / 2 - 28, 14, 56, 32, Color{ 10, 12, 20, 220 });
    DrawRectangleLines(screenW / 2 - 28, 14, 56, 32, hexToColor("#f59e0b"));
    DrawText(timeStr.c_str(), (screenW - timeW) / 2, 18, 24, WHITE);

    // Combo Counter
    if (s.playerCombo > 1) {
        std::string comboStr = std::to_string(s.playerCombo) + " HITS";
        DrawText(comboStr.c_str(), 45, 95, 20, hexToColor("#fbbf24"));
        char mulStr[32];
        std::snprintf(mulStr, sizeof(mulStr), "x%.2f DMG", s.playerComboMultiplier);
        DrawText(mulStr, 45, 118, 12, hexToColor("#fde047"));
    }

    // Domain Expansion Banner
    if (!s.domainBanner.empty()) {
        DrawRectangle(0, 58, screenW, 26, Color{ 10, 5, 20, 230 });
        int bannerW = MeasureText(s.domainBanner.c_str(), 14);
        DrawText(s.domainBanner.c_str(), (screenW - bannerW) / 2, 64, 14, hexToColor("#fde047"));
    }

    // Round / Fight Banner
    if (!s.bannerText.empty()) {
        int bW = MeasureText(s.bannerText.c_str(), 36);
        DrawText(s.bannerText.c_str(), (screenW - bW) / 2 + 2, screenH / 2 - 40 + 2, 36, BLACK);
        DrawText(s.bannerText.c_str(), (screenW - bW) / 2, screenH / 2 - 40, 36, hexToColor("#f59e0b"));

        if (!s.bannerSubtext.empty()) {
            int subW = MeasureText(s.bannerSubtext.c_str(), 16);
            DrawText(s.bannerSubtext.c_str(), (screenW - subW) / 2, screenH / 2 + 5, 16, WHITE);
        }
    }

    // Bottom Skill Cards
    drawSkillBar(engine, screenW, screenH);
}

void UI::drawSkillBar(const Engine& engine, int screenW, int screenH) {
    const auto& skills = Roster::getSkills(engine.player.id);
    int totalCards = (int)skills.size();
    int cardW = 92;
    int cardH = 38;
    int gap = 8;
    int startX = (screenW - (totalCards * cardW + (totalCards - 1) * gap)) / 2;
    int cardY = screenH - cardH - 12;

    for (int i = 0; i < totalCards; ++i) {
        const auto& sk = skills[i];
        int x = startX + i * (cardW + gap);
        bool canCast = (engine.player.ce >= (float)sk.cost);

        DrawRectangle(x, cardY, cardW, cardH, canCast ? Color{ 15, 20, 36, 220 } : Color{ 10, 10, 15, 200 });
        DrawRectangleLines(x, cardY, cardW, cardH, canCast ? sk.color : Color{ 50, 55, 70, 180 });

        std::string key = "[" + sk.keyLabel + "]";
        DrawText(key.c_str(), x + 6, cardY + 4, 10, canCast ? hexToColor("#38bdf8") : Color{ 120, 130, 150, 180 });

        std::string cost = std::to_string(sk.cost) + " CE";
        int costW = MeasureText(cost.c_str(), 10);
        DrawText(cost.c_str(), x + cardW - costW - 6, cardY + 4, 10, canCast ? hexToColor("#fbbf24") : Color{ 120, 130, 150, 180 });

        DrawText(sk.shortName.c_str(), x + 6, cardY + 18, 11, canCast ? WHITE : Color{ 120, 130, 150, 180 });
    }
}

void UI::drawPauseModal(Engine& engine, int screenW, int screenH) {
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)) {
        Audio::playSfx(SoundEffect::UiClick);
        engine.setPaused(false);
        return;
    }
    if (IsKeyPressed(KEY_R)) {
        Audio::playSfx(SoundEffect::UiClick);
        engine.retryMatch();
        return;
    }
    if (IsKeyPressed(KEY_Q)) {
        Audio::playSfx(SoundEffect::UiClick);
        engine.startTitleBackdrop();
        return;
    }

    DrawRectangle(0, 0, screenW, screenH, Color{ 0, 0, 0, 190 });

    int boxW = 340;
    int boxH = 210;
    int boxX = (screenW - boxW) / 2;
    int boxY = (screenH - boxH) / 2;

    DrawRectangle(boxX, boxY, boxW, boxH, Color{ 18, 22, 38, 245 });
    DrawRectangleLines(boxX, boxY, boxW, boxH, hexToColor("#38bdf8"));

    const char* txt = "PAUSED";
    int txtW = MeasureText(txt, 22);
    DrawText(txt, (screenW - txtW) / 2, boxY + 20, 22, WHITE);

    DrawText("[ESC / P] Resume", boxX + 60, boxY + 68, 14, hexToColor("#cbd5e1"));
    DrawText("[R] Restart Match", boxX + 60, boxY + 104, 14, hexToColor("#cbd5e1"));
    DrawText("[Q] Exit to Title", boxX + 60, boxY + 140, 14, hexToColor("#cbd5e1"));
}

void UI::drawMatchResult(Engine& engine, int screenW, int screenH) {
    HudSnapshot s = engine.getHudSnapshot();

    if (IsKeyPressed(KEY_ENTER)) {
        Audio::playSfx(SoundEffect::UiClick);
        if (s.mode == GameMode::Story && s.winner == 1) {
            engine.advanceStoryStage();
        } else {
            engine.retryMatch();
        }
        return;
    }
    if (IsKeyPressed(KEY_R)) {
        Audio::playSfx(SoundEffect::UiClick);
        engine.retryMatch();
        return;
    }
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_SPACE)) {
        Audio::playSfx(SoundEffect::UiClick);
        engine.startTitleBackdrop();
        return;
    }

    DrawRectangle(0, 0, screenW, screenH, Color{ 0, 0, 0, 210 });

    int boxW = 480;
    int boxH = 260;
    int boxX = (screenW - boxW) / 2;
    int boxY = (screenH - boxH) / 2;

    DrawRectangle(boxX, boxY, boxW, boxH, Color{ 16, 18, 30, 245 });
    DrawRectangleLines(boxX, boxY, boxW, boxH, (s.winner == 1) ? hexToColor("#f59e0b") : hexToColor("#dc2626"));

    bool won = (s.winner == 1);
    const char* header = won ? "VICTORY" : "DEFEAT";
    int hW = MeasureText(header, 32);
    DrawText(header, (screenW - hW) / 2, boxY + 18, 32, won ? hexToColor("#f59e0b") : hexToColor("#ef4444"));

    if (!s.victoryQuoteTag.empty()) {
        int tagW = MeasureText(s.victoryQuoteTag.c_str(), 11);
        DrawText(s.victoryQuoteTag.c_str(), (screenW - tagW) / 2, boxY + 54, 11, hexToColor("#38bdf8"));
    }

    // Stats
    std::string scoreStr = "Score: " + std::to_string(s.score) + "   Max Combo: " + std::to_string(s.maxCombo) + " hits";
    DrawText(scoreStr.c_str(), boxX + 35, boxY + 76, 12, hexToColor("#cbd5e1"));

    std::string dmgStr = "Damage Dealt: " + std::to_string(s.damageDealt) + "   Match Time: " + std::to_string((int)s.matchTime) + "s";
    DrawText(dmgStr.c_str(), boxX + 35, boxY + 96, 12, hexToColor("#cbd5e1"));

    // Quote Box
    DrawRectangle(boxX + 30, boxY + 124, boxW - 60, 54, Color{ 24, 28, 48, 200 });
    DrawRectangleLines(boxX + 30, boxY + 124, boxW - 60, 54, hexToColor("#334155"));
    DrawText(("\"" + s.victoryQuote + "\"").c_str(), boxX + 40, boxY + 140, 11, WHITE);

    // Buttons
    std::string nextText = (s.mode == GameMode::Story && won) ? "[ENTER] Next Stage" : "[ENTER / R] Rematch";
    DrawText(nextText.c_str(), boxX + 50, boxY + 200, 13, hexToColor("#f59e0b"));
    DrawText("[SPACE / ESC] Title Screen", boxX + 270, boxY + 200, 13, hexToColor("#94a3b8"));
}

} // namespace CursedClash
