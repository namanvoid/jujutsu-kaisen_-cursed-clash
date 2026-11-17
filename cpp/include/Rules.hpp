#pragma once

#include "Types.hpp"
#include <raylib.h>

namespace CursedClash {

struct HitResolution {
    bool hit = false;
    bool blocked = false;
    bool parried = false;
    bool infinityBlocked = false;
    int damage = 0;
    float hitstun = 0.0f;
    float knockbackX = 0.0f;
    float knockbackY = 0.0f;
    float hitstop = 0.0f;
    int ceGainAttacker = 0;
    int ceGainDefender = 0;
};

class Rules {
public:
    static float computeComboMultiplier(int comboCount);
    static float computeDynamicHitstun(float baseHitstun, int comboCount);
    static void computeDynamicCamera(float pX, float eX, float pY, float eY, Camera2D& cam, float dt);
    static float getDifficultyDamageMultiplier(Difficulty diff);
    static float clampStageX(float x);
    static int computeSukunaPhase(int hp, int maxHp);
    static bool canChainAttack(FighterId id, FighterAction current, FighterAction next);
    static bool getPlayerMoveSpec(FighterId id, FighterAction action, MoveSpec& outSpec);
    static MoveSpec getEnemyMoveSpec(FighterId id, FighterAction action);
    static HitResolution resolveHit(
        const FighterState& attacker,
        const FighterState& defender,
        const MoveSpec& move,
        float difficultyMultiplier = 1.0f
    );
};

} // namespace CursedClash
