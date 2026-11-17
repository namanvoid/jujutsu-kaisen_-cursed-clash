#include "Rules.hpp"
#include "Roster.hpp"
#include <algorithm>
#include <cmath>
#include <map>

namespace CursedClash {

float Rules::computeComboMultiplier(int comboCount) {
    if (comboCount <= 1) return 1.0f;
    return std::min(1.65f, 1.0f + (comboCount - 1) * 0.08f);
}

float Rules::computeDynamicHitstun(float baseHitstun, int comboCount) {
    if (baseHitstun <= 0.0f) return 0.0f;
    if (comboCount <= 4) {
        return baseHitstun * (1.0f + std::max(0, comboCount - 1) * 0.04f);
    }
    float decay = std::max(0.65f, 1.12f - (comboCount - 4) * 0.06f);
    return baseHitstun * decay;
}

void Rules::computeDynamicCamera(float pX, float eX, float pY, float eY, Camera2D& cam, float dt) {
    float dist = std::abs(pX - eX);
    float t = std::clamp((230.0f - dist) / (230.0f - 85.0f), 0.0f, 1.0f);
    float targetZoom = 1.0f + t * 0.26f;

    float midX = (pX + eX) * 0.5f;
    float midY = std::min(GROUND_Y - 22.0f, (pY + eY) * 0.5f - 26.0f);

    float halfW = (WORLD_WIDTH * 0.5f) / targetZoom;
    float halfH = (WORLD_HEIGHT * 0.5f) / targetZoom;
    float targetX = std::clamp(midX, halfW, WORLD_WIDTH - halfW);
    float targetY = std::clamp(midY + 18.0f, halfH, WORLD_HEIGHT - halfH);

    float smooth = std::min(1.0f, dt * 6.5f);
    cam.zoom += (targetZoom - cam.zoom) * smooth;
    cam.target.x += (targetX - cam.target.x) * smooth;
    cam.target.y += (targetY - cam.target.y) * smooth;
}

float Rules::getDifficultyDamageMultiplier(Difficulty diff) {
    switch (diff) {
        case Difficulty::Grade1: return 0.75f;
        case Difficulty::Special: return 1.0f;
        case Difficulty::King: return 1.35f;
    }
    return 1.0f;
}

float Rules::clampStageX(float x) {
    return std::clamp(x, STAGE_LEFT + 8.0f, STAGE_RIGHT - 8.0f);
}

int Rules::computeSukunaPhase(int hp, int maxHp) {
    float ratio = (float)hp / (float)maxHp;
    if (ratio > 0.65f) return 1;
    if (ratio > 0.30f) return 2;
    return 3;
}

static MoveSpec getBasicMove(FighterAction action, FighterId id) {
    MoveSpec m;
    m.id = action;
    if (action == FighterAction::Light1) {
        m.name = "Quick Strike";
        m.startup = 0.06f; m.active = 0.08f; m.recovery = 0.14f;
        m.damage = 11; m.chipDamage = 2; m.ceCost = 0; m.ceGainOnHit = 6;
        m.range = 38.0f; m.heightRange = 32.0f;
        m.hitstun = 0.24f; m.blockstun = 0.12f;
        m.knockbackX = 26.0f; m.knockbackY = -18.0f; m.hitstop = 0.03f;
        m.canChainInto = { FighterAction::Light2, FighterAction::Heavy, FighterAction::Skill1, FighterAction::Skill2, FighterAction::Skill3 };
    } else if (action == FighterAction::Light2) {
        m.name = "Combo Follow-Up";
        m.startup = 0.07f; m.active = 0.09f; m.recovery = 0.16f;
        m.damage = 14; m.chipDamage = 3; m.ceCost = 0; m.ceGainOnHit = 8;
        m.range = 42.0f; m.heightRange = 32.0f;
        m.hitstun = 0.28f; m.blockstun = 0.14f;
        m.knockbackX = 38.0f; m.knockbackY = -30.0f; m.hitstop = 0.035f;
        m.canChainInto = { FighterAction::Light3, FighterAction::Heavy, FighterAction::Skill1, FighterAction::Skill2, FighterAction::Skill3, FighterAction::Skill5Domain };
    } else if (action == FighterAction::Light3) {
        m.name = "Cursed Finisher";
        m.startup = 0.09f; m.active = 0.10f; m.recovery = 0.22f;
        m.damage = 20; m.chipDamage = 4; m.ceCost = 0; m.ceGainOnHit = 10;
        m.range = 46.0f; m.heightRange = 36.0f;
        m.hitstun = 0.36f; m.blockstun = 0.18f;
        m.knockbackX = 105.0f; m.knockbackY = -78.0f; m.hitstop = 0.05f;
        m.canChainInto = { FighterAction::Skill1, FighterAction::Skill2, FighterAction::Skill3, FighterAction::Skill5Domain };
    } else { // Heavy
        m.name = "Heavy Launcher";
        m.startup = 0.13f; m.active = 0.11f; m.recovery = 0.24f;
        m.damage = 25; m.chipDamage = 6; m.ceCost = 0; m.ceGainOnHit = 12;
        m.range = 50.0f; m.heightRange = 38.0f;
        m.hitstun = 0.42f; m.blockstun = 0.20f;
        m.knockbackX = 135.0f; m.knockbackY = -102.0f; m.hitstop = 0.06f;
        m.canChainInto = { FighterAction::Skill1, FighterAction::Skill2, FighterAction::Skill3, FighterAction::Skill5Domain };
    }
    if (id == FighterId::Toji) {
        m.startup = std::max(0.04f, m.startup - 0.015f);
        m.piercesInfinity = true;
    }
    return m;
}

bool Rules::canChainAttack(FighterId id, FighterAction current, FighterAction next) {
    (void)id;
    if (current == FighterAction::Light1) {
        return next == FighterAction::Light2 || next == FighterAction::Heavy ||
               next == FighterAction::Skill1 || next == FighterAction::Skill2 || next == FighterAction::Skill3;
    }
    if (current == FighterAction::Light2) {
        return next == FighterAction::Light3 || next == FighterAction::Heavy ||
               next == FighterAction::Skill1 || next == FighterAction::Skill2 || next == FighterAction::Skill3 || next == FighterAction::Skill5Domain;
    }
    if (current == FighterAction::Light3 || current == FighterAction::Heavy) {
        return next == FighterAction::Skill1 || next == FighterAction::Skill2 || next == FighterAction::Skill3 || next == FighterAction::Skill5Domain;
    }
    return false;
}

bool Rules::getPlayerMoveSpec(FighterId id, FighterAction action, MoveSpec& outSpec) {
    if (action == FighterAction::Light1 || action == FighterAction::Light2 ||
        action == FighterAction::Light3 || action == FighterAction::Heavy) {
        outSpec = getBasicMove(action, id);
        return true;
    }

    // Match each skill action for the fighter
    outSpec.id = action;
    outSpec.piercesInfinity = (id == FighterId::Toji);

    if (id == FighterId::Gojo) {
        if (action == FighterAction::Skill1) {
            outSpec.name = "Lapse: Blue";
            outSpec.startup = 0.13f; outSpec.active = 0.16f; outSpec.recovery = 0.20f;
            outSpec.damage = 16; outSpec.chipDamage = 4; outSpec.ceCost = 25; outSpec.ceGainOnHit = 0;
            outSpec.range = 150.0f; outSpec.heightRange = 60.0f;
            outSpec.hitstun = 0.36f; outSpec.blockstun = 0.20f;
            outSpec.knockbackX = -75.0f; outSpec.knockbackY = -40.0f; outSpec.hitstop = 0.04f;
            outSpec.isProjectile = true;
            return true;
        } else if (action == FighterAction::Skill2) {
            outSpec.name = "Reversal: Red";
            outSpec.startup = 0.16f; outSpec.active = 0.14f; outSpec.recovery = 0.24f;
            outSpec.damage = 40; outSpec.chipDamage = 10; outSpec.ceCost = 35; outSpec.ceGainOnHit = 0;
            outSpec.range = 185.0f; outSpec.heightRange = 50.0f;
            outSpec.hitstun = 0.45f; outSpec.blockstun = 0.24f;
            outSpec.knockbackX = 205.0f; outSpec.knockbackY = -130.0f; outSpec.hitstop = 0.065f;
            outSpec.isProjectile = true;
            return true;
        } else if (action == FighterAction::Skill3) {
            outSpec.name = "Hollow Purple";
            outSpec.startup = 0.30f; outSpec.active = 0.20f; outSpec.recovery = 0.32f;
            outSpec.damage = 80; outSpec.chipDamage = 24; outSpec.ceCost = 65; outSpec.ceGainOnHit = 0;
            outSpec.range = 340.0f; outSpec.heightRange = 72.0f;
            outSpec.hitstun = 0.58f; outSpec.blockstun = 0.30f;
            outSpec.knockbackX = 235.0f; outSpec.knockbackY = -155.0f; outSpec.hitstop = 0.08f;
            outSpec.isProjectile = true;
            return true;
        } else if (action == FighterAction::Skill4) {
            outSpec.name = "Infinity";
            outSpec.startup = 0.08f; outSpec.active = 0.16f; outSpec.recovery = 0.12f;
            outSpec.damage = 0; outSpec.chipDamage = 0; outSpec.ceCost = 30; outSpec.ceGainOnHit = 0;
            outSpec.range = 0; outSpec.heightRange = 0; outSpec.hitstun = 0; outSpec.blockstun = 0;
            outSpec.knockbackX = 0; outSpec.knockbackY = 0; outSpec.hitstop = 0.02f;
            return true;
        } else if (action == FighterAction::Skill5Domain) {
            outSpec.name = "Unlimited Void";
            outSpec.startup = 0.24f; outSpec.active = 0.55f; outSpec.recovery = 0.30f;
            outSpec.damage = 145; outSpec.chipDamage = 75; outSpec.ceCost = 100; outSpec.ceGainOnHit = 0;
            outSpec.range = 460.0f; outSpec.heightRange = 270.0f;
            outSpec.hitstun = 1.80f; outSpec.blockstun = 0.45f;
            outSpec.knockbackX = 205.0f; outSpec.knockbackY = -155.0f; outSpec.hitstop = 0.10f;
            outSpec.isUnblockable = true;
            return true;
        }
    } else if (id == FighterId::Sukuna) {
        if (action == FighterAction::Skill1) {
            outSpec.name = "Dismantle";
            outSpec.startup = 0.14f; outSpec.active = 0.12f; outSpec.recovery = 0.20f;
            outSpec.damage = 26; outSpec.chipDamage = 6; outSpec.ceCost = 25; outSpec.ceGainOnHit = 0;
            outSpec.range = 240.0f; outSpec.heightRange = 45.0f;
            outSpec.hitstun = 0.34f; outSpec.blockstun = 0.18f;
            outSpec.knockbackX = 130.0f; outSpec.knockbackY = -60.0f; outSpec.hitstop = 0.05f;
            outSpec.isProjectile = true;
            return true;
        } else if (action == FighterAction::Skill2) {
            outSpec.name = "Cleave";
            outSpec.startup = 0.16f; outSpec.active = 0.14f; outSpec.recovery = 0.22f;
            outSpec.damage = 44; outSpec.chipDamage = 12; outSpec.ceCost = 35; outSpec.ceGainOnHit = 0;
            outSpec.range = 65.0f; outSpec.heightRange = 45.0f;
            outSpec.hitstun = 0.44f; outSpec.blockstun = 0.22f;
            outSpec.knockbackX = 190.0f; outSpec.knockbackY = -110.0f; outSpec.hitstop = 0.065f;
            return true;
        } else if (action == FighterAction::Skill3) {
            outSpec.name = "Divine Flame (Fuga)";
            outSpec.startup = 0.32f; outSpec.active = 0.22f; outSpec.recovery = 0.34f;
            outSpec.damage = 85; outSpec.chipDamage = 26; outSpec.ceCost = 65; outSpec.ceGainOnHit = 0;
            outSpec.range = 350.0f; outSpec.heightRange = 75.0f;
            outSpec.hitstun = 0.60f; outSpec.blockstun = 0.32f;
            outSpec.knockbackX = 240.0f; outSpec.knockbackY = -160.0f; outSpec.hitstop = 0.085f;
            outSpec.isProjectile = true;
            return true;
        } else if (action == FighterAction::Skill4) {
            outSpec.name = "World-Cutting Slash";
            outSpec.startup = 0.22f; outSpec.active = 0.14f; outSpec.recovery = 0.26f;
            outSpec.damage = 55; outSpec.chipDamage = 16; outSpec.ceCost = 45; outSpec.ceGainOnHit = 0;
            outSpec.range = 280.0f; outSpec.heightRange = 55.0f;
            outSpec.hitstun = 0.50f; outSpec.blockstun = 0.26f;
            outSpec.knockbackX = 210.0f; outSpec.knockbackY = -125.0f; outSpec.hitstop = 0.075f;
            outSpec.piercesInfinity = true;
            outSpec.isUnblockable = true;
            return true;
        } else if (action == FighterAction::Skill5Domain) {
            outSpec.name = "Malevolent Shrine";
            outSpec.startup = 0.26f; outSpec.active = 0.60f; outSpec.recovery = 0.32f;
            outSpec.damage = 150; outSpec.chipDamage = 75; outSpec.ceCost = 100; outSpec.ceGainOnHit = 0;
            outSpec.range = 460.0f; outSpec.heightRange = 270.0f;
            outSpec.hitstun = 1.80f; outSpec.blockstun = 0.45f;
            outSpec.knockbackX = 210.0f; outSpec.knockbackY = -160.0f; outSpec.hitstop = 0.10f;
            outSpec.isUnblockable = true;
            return true;
        }
    }

    // Fallback generic skill stats for all characters
    if (action == FighterAction::Skill1) {
        outSpec.name = "Special Strike";
        outSpec.startup = 0.14f; outSpec.active = 0.14f; outSpec.recovery = 0.20f;
        outSpec.damage = 26; outSpec.chipDamage = 6; outSpec.ceCost = 25; outSpec.ceGainOnHit = 0;
        outSpec.range = 95.0f; outSpec.heightRange = 45.0f;
        outSpec.hitstun = 0.36f; outSpec.blockstun = 0.18f;
        outSpec.knockbackX = 115.0f; outSpec.knockbackY = -70.0f; outSpec.hitstop = 0.05f;
        return true;
    } else if (action == FighterAction::Skill2) {
        outSpec.name = "Technique Blast";
        outSpec.startup = 0.16f; outSpec.active = 0.15f; outSpec.recovery = 0.22f;
        outSpec.damage = 36; outSpec.chipDamage = 9; outSpec.ceCost = 35; outSpec.ceGainOnHit = 0;
        outSpec.range = 140.0f; outSpec.heightRange = 50.0f;
        outSpec.hitstun = 0.42f; outSpec.blockstun = 0.22f;
        outSpec.knockbackX = 150.0f; outSpec.knockbackY = -100.0f; outSpec.hitstop = 0.06f;
        return true;
    } else if (action == FighterAction::Skill3) {
        outSpec.name = "Maximum Output";
        outSpec.startup = 0.28f; outSpec.active = 0.20f; outSpec.recovery = 0.30f;
        outSpec.damage = 70; outSpec.chipDamage = 20; outSpec.ceCost = 60; outSpec.ceGainOnHit = 0;
        outSpec.range = 300.0f; outSpec.heightRange = 65.0f;
        outSpec.hitstun = 0.54f; outSpec.blockstun = 0.28f;
        outSpec.knockbackX = 220.0f; outSpec.knockbackY = -140.0f; outSpec.hitstop = 0.075f;
        return true;
    } else if (action == FighterAction::Skill4) {
        outSpec.name = "Stance / Buff";
        outSpec.startup = 0.10f; outSpec.active = 0.14f; outSpec.recovery = 0.14f;
        outSpec.damage = 0; outSpec.chipDamage = 0; outSpec.ceCost = 25; outSpec.ceGainOnHit = 0;
        outSpec.range = 0; outSpec.heightRange = 0; outSpec.hitstun = 0; outSpec.blockstun = 0;
        outSpec.knockbackX = 0; outSpec.knockbackY = 0; outSpec.hitstop = 0.02f;
        return true;
    } else if (action == FighterAction::Skill5Domain) {
        outSpec.name = "Domain Expansion";
        outSpec.startup = 0.25f; outSpec.active = 0.55f; outSpec.recovery = 0.32f;
        outSpec.damage = 135; outSpec.chipDamage = 65; outSpec.ceCost = 100; outSpec.ceGainOnHit = 0;
        outSpec.range = 460.0f; outSpec.heightRange = 270.0f;
        outSpec.hitstun = 1.70f; outSpec.blockstun = 0.45f;
        outSpec.knockbackX = 200.0f; outSpec.knockbackY = -150.0f; outSpec.hitstop = 0.095f;
        outSpec.isUnblockable = true;
        return true;
    }

    return false;
}

MoveSpec Rules::getEnemyMoveSpec(FighterId id, FighterAction action) {
    MoveSpec spec;
    if (getPlayerMoveSpec(id, action, spec)) {
        return spec;
    }

    if (action == FighterAction::EnemySpecial1) {
        spec.id = action;
        spec.name = "Enemy Special 1";
        spec.startup = 0.28f; spec.active = 0.14f; spec.recovery = 0.28f;
        spec.damage = 24; spec.chipDamage = 6; spec.ceCost = 25; spec.ceGainOnHit = 0;
        spec.range = 160.0f; spec.heightRange = 45.0f;
        spec.hitstun = 0.34f; spec.blockstun = 0.18f;
        spec.knockbackX = 120.0f; spec.knockbackY = -65.0f; spec.hitstop = 0.05f;
        spec.piercesInfinity = (id == FighterId::Toji);
        return spec;
    }
    if (action == FighterAction::EnemySpecial2) {
        spec.id = action;
        spec.name = "Enemy Special 2";
        spec.startup = 0.36f; spec.active = 0.16f; spec.recovery = 0.30f;
        spec.damage = 34; spec.chipDamage = 9; spec.ceCost = 35; spec.ceGainOnHit = 0;
        spec.range = 180.0f; spec.heightRange = 55.0f;
        spec.hitstun = 0.44f; spec.blockstun = 0.22f;
        spec.knockbackX = 150.0f; spec.knockbackY = -110.0f; spec.hitstop = 0.065f;
        spec.piercesInfinity = (id == FighterId::Toji);
        return spec;
    }
    if (action == FighterAction::EnemyUltimate) {
        spec.id = action;
        spec.name = "Domain Expansion";
        spec.startup = 0.46f; spec.active = 0.28f; spec.recovery = 0.36f;
        spec.damage = (id == FighterId::Sukuna ? 50 : 40);
        spec.chipDamage = 16; spec.ceCost = 65; spec.ceGainOnHit = 0;
        spec.range = 250.0f; spec.heightRange = 85.0f;
        spec.hitstun = 0.52f; spec.blockstun = 0.25f;
        spec.knockbackX = 175.0f; spec.knockbackY = -130.0f; spec.hitstop = 0.075f;
        spec.piercesInfinity = (id == FighterId::Toji);
        return spec;
    }

    return getBasicMove(FighterAction::Light1, id);
}

HitResolution Rules::resolveHit(
    const FighterState& attacker,
    const FighterState& defender,
    const MoveSpec& move,
    float difficultyMultiplier
) {
    HitResolution res;
    if (defender.invulnTimer > 0.0f && !move.isUnblockable) {
        return res;
    }

    int nextCombo = attacker.comboCount + 1;
    float comboMul = computeComboMultiplier(nextCombo);
    float attackerBuffMul = attacker.buffTimer > 0.0f ? 1.15f : 1.0f;
    float defenderArmorMul = (defender.buffTimer > 0.0f && (defender.id == FighterId::Panda || defender.id == FighterId::Hanami)) ? 0.65f : 1.0f;

    int rawDamage = std::round(move.damage * difficultyMultiplier * comboMul * attackerBuffMul * defenderArmorMul);
    int rawChip = std::round(move.chipDamage * difficultyMultiplier * defenderArmorMul);

    // Parry stance or Just-Block
    if ((defender.parryWindowTimer > 0.0f || (defender.action == FighterAction::Block && defender.blockWindowTimer > 0.0f)) && !move.isUnblockable) {
        res.hit = true;
        res.blocked = true;
        res.parried = true;
        res.damage = 0;
        res.hitstun = 0.0f;
        res.knockbackX = attacker.facing * 50.0f;
        res.knockbackY = 0.0f;
        res.hitstop = 0.075f;
        res.ceGainAttacker = 0;
        res.ceGainDefender = defender.parryWindowTimer > 0.0f ? 22 : 18;
        return res;
    }

    // Gojo's Infinity
    if (defender.id == FighterId::Gojo && defender.infinityTimer > 0.0f && !move.piercesInfinity && !move.isUnblockable) {
        res.hit = true;
        res.blocked = true;
        res.infinityBlocked = true;
        res.damage = 0;
        res.hitstun = 0.0f;
        res.knockbackX = attacker.facing * 20.0f;
        res.knockbackY = 0.0f;
        res.hitstop = 0.025f;
        res.ceGainAttacker = 0;
        res.ceGainDefender = 4;
        return res;
    }

    // Normal Block
    bool isBlocking = (defender.action == FighterAction::Block) && !move.isUnblockable;
    if (isBlocking) {
        res.hit = true;
        res.blocked = true;
        res.damage = rawChip;
        res.hitstun = move.blockstun;
        res.knockbackX = attacker.facing * move.knockbackX * 0.45f;
        res.knockbackY = 0.0f;
        res.hitstop = move.hitstop * 0.65f;
        res.ceGainAttacker = std::round(move.ceGainOnHit * 0.35f);
        res.ceGainDefender = 4;
        return res;
    }

    // Super Armor
    if (defender.superArmorActive && !move.isUnblockable) {
        res.hit = true;
        res.damage = std::round(rawDamage * 0.7f);
        res.hitstun = 0.0f;
        res.knockbackX = 0.0f;
        res.knockbackY = 0.0f;
        res.hitstop = move.hitstop * 0.5f;
        res.ceGainAttacker = move.ceGainOnHit;
        res.ceGainDefender = 6;
        return res;
    }

    // Clean Hit
    res.hit = true;
    res.damage = rawDamage;
    res.hitstun = computeDynamicHitstun(move.hitstun, nextCombo);
    res.knockbackX = attacker.facing * move.knockbackX;
    res.knockbackY = move.knockbackY;
    res.hitstop = move.hitstop;
    res.ceGainAttacker = move.ceGainOnHit;
    res.ceGainDefender = 8;
    return res;
}

} // namespace CursedClash
