#include <raylib.h>
#include "Types.hpp"
#include "Roster.hpp"
#include "Rules.hpp"
#include "PixelSprites.hpp"
#include "PixelArenas.hpp"
#include "Audio.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "UI.hpp"
#include <iostream>
#include <cstring>

using namespace CursedClash;

int main(int argc, char* argv[]) {
    // Check if running in headless test mode
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--test") == 0 || std::strcmp(argv[i], "-t") == 0) {
            std::cout << "[Test Mode] Initializing Cursed Clash Engine simulation..." << std::endl;
            Roster::init();
            Engine testEngine;
            testEngine.startStoryMode(0, FighterId::Gojo);
            InputState testInput;
            testInput.justLight = true;
            for (int step = 0; step < 120; ++step) {
                testEngine.step(1.0f / 60.0f, testInput);
                testInput.justLight = (step % 20 == 0);
                testInput.justSkill1 = (step == 30);
            }
            std::cout << "[Test Mode] Successfully simulated 120 frames! Player HP: "
                      << testEngine.player.hp << ", Enemy HP: " << testEngine.enemy.hp << std::endl;
            return 0;
        }
    }

    const int windowWidth = 960;
    const int windowHeight = 540;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(windowWidth, windowHeight, "Jujutsu Kaisen: Cursed Clash");

    if (!IsWindowReady()) {
        std::cerr << "Note: Window could not be initialized in this environment (e.g. headless/no display)." << std::endl;
        std::cerr << "Run with '--test' to verify engine logic headlessly." << std::endl;
        return 0;
    }

    SetTargetFPS(60);

    Audio::init();
    Roster::init();
    PixelSprites::init();
    PixelArenas::init();
    Renderer::init(WORLD_WIDTH, WORLD_HEIGHT);
    UI::init();

    Engine engine;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        int screenW = GetScreenWidth();
        int screenH = GetScreenHeight();

        // Pause toggle
        if (IsKeyPressed(KEY_ESCAPE) && engine.screenState == GameScreenState::Playing) {
            engine.setPaused(true);
        }

        // Input Gathering
        InputState input;
        input.left = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
        input.right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
        input.up = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
        input.down = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
        input.block = IsKeyDown(KEY_L) || IsKeyDown(KEY_RIGHT_SHIFT);

        input.justUp = IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP);
        input.justDown = IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN);
        input.justDash = IsKeyPressed(KEY_SPACE);
        input.justLight = IsKeyPressed(KEY_J);
        input.justHeavy = IsKeyPressed(KEY_K);
        input.justBlock = IsKeyPressed(KEY_L) || IsKeyPressed(KEY_RIGHT_SHIFT);
        input.justParry = IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_F);

        input.justSkill1 = IsKeyPressed(KEY_U);
        input.justSkill2 = IsKeyPressed(KEY_I);
        input.justSkill3 = IsKeyPressed(KEY_O);
        input.justSkill4 = IsKeyPressed(KEY_H);
        input.justSkill5 = IsKeyPressed(KEY_Y);

        // Game Simulation Step
        engine.step(dt, input);

        // Render World Texture (480x270 virtual pixel canvas)
        Renderer::renderWorld(engine);

        // Draw to Window Screen with HUD & UI
        BeginDrawing();
        ClearBackground(BLACK);

        Renderer::drawWorldToScreen(screenW, screenH);
        UI::draw(engine, screenW, screenH);

        EndDrawing();
    }

    Renderer::cleanup();
    PixelArenas::cleanup();
    PixelSprites::cleanup();
    Audio::cleanup();
    CloseWindow();

    return 0;
}
