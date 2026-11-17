#pragma once

#include "Types.hpp"

namespace CursedClash {

struct AiDecision {
    int moveDir = 0; // -1, 0, 1
    bool wantsBlock = false;
    bool wantsParry = false;
    bool wantsJump = false;
    bool wantsDash = false;
    bool wantsSidestep = false;
    bool hasTriggerAction = false;
    FighterAction triggerAction = FighterAction::Idle;
};

class Ai {
public:
    static AiDecision decideVillainAi(
        FighterState& enemy,
        const FighterState& player,
        float dt,
        Difficulty difficulty,
        bool activeThreatClose
    );
};

} // namespace CursedClash
