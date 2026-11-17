#pragma once

#include "Engine.hpp"
#include <raylib.h>

namespace CursedClash {

class UI {
public:
    static void init();
    static void draw(Engine& engine, int screenW, int screenH);

private:
    static void drawTitleScreen(Engine& engine, int screenW, int screenH);
    static void drawCombatHud(const Engine& engine, int screenW, int screenH);
    static void drawPauseModal(Engine& engine, int screenW, int screenH);
    static void drawMatchResult(Engine& engine, int screenW, int screenH);
    static void drawSkillBar(const Engine& engine, int screenW, int screenH);
};

} // namespace CursedClash
