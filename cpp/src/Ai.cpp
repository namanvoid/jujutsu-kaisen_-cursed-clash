#include "Ai.hpp"
#include "Rules.hpp"
#include <cmath>
#include <cstdlib>
#include <algorithm>

namespace CursedClash {

static float randFloat() {
    return (float)rand() / (float)RAND_MAX;
}

AiDecision Ai::decideVillainAi(
    FighterState& enemy,
    const FighterState& player,
    float dt,
    Difficulty difficulty,
    bool activeThreatClose
) {
    AiDecision out;

    if (enemy.stunLockedTimer > 0.0f || enemy.hitstunTimer > 0.0f ||
        enemy.action == FighterAction::KO || enemy.action == FighterAction::Knockdown) {
        return out;
    }

    float dx = player.x - enemy.x;
    float absDx = std::abs(dx);
    int toward = (dx >= 0.0f) ? 1 : -1;
    int away = -toward;

    enemy.aiCooldown = std::max(0.0f, enemy.aiCooldown - dt);
    enemy.aiStateTimer = std::max(0.0f, enemy.aiStateTimer - dt);

    float reactionRate = (difficulty == Difficulty::King) ? 0.90f :
                         (difficulty == Difficulty::Special) ? 0.65f : 0.40f;

    bool isIdleOrWalk = (enemy.action == FighterAction::Idle ||
                         enemy.action == FighterAction::Walk ||
                         enemy.action == FighterAction::Block);

    // Reactive defense
    bool playerAttacking = (player.action == FighterAction::Light1 ||
                            player.action == FighterAction::Light2 ||
                            player.action == FighterAction::Light3 ||
                            player.action == FighterAction::Heavy ||
                            player.action == FighterAction::Skill1 ||
                            player.action == FighterAction::Skill2 ||
                            player.action == FighterAction::Skill3);

    if (isIdleOrWalk && (activeThreatClose || (playerAttacking && absDx < 105.0f))) {
        if (enemy.aiStateTimer <= 0.0f) {
            float roll = randFloat();
            if ((enemy.id == FighterId::Toji || enemy.id == FighterId::Kashimo) && roll < reactionRate * 0.5f) {
                out.wantsSidestep = true;
                enemy.aiStateTimer = 0.55f;
                return out;
            }
            if ((enemy.id == FighterId::Sukuna || enemy.id == FighterId::Kenjaku || enemy.id == FighterId::Toji) &&
                roll < reactionRate * 0.22f) {
                out.wantsParry = true;
                enemy.aiStateTimer = 0.70f;
                return out;
            }
            if (roll < reactionRate * 0.6f) {
                enemy.aiIntent = "guard";
                enemy.aiStateTimer = 0.36f;
            } else if (enemy.id == FighterId::Hanami && roll < 0.85f) {
                out.hasTriggerAction = true;
                out.triggerAction = FighterAction::Heavy;
                enemy.aiCooldown = 0.68f;
                return out;
            }
        }
    }

    if (enemy.aiIntent == "guard" && enemy.aiStateTimer > 0.0f) {
        out.wantsBlock = true;
        return out;
    }

    if (!isIdleOrWalk) {
        return out;
    }

    float aggressionMul = (difficulty == Difficulty::King) ? 0.74f :
                          (difficulty == Difficulty::Special) ? 1.0f : 1.35f;

    MoveSpec ult = Rules::getEnemyMoveSpec(enemy.id, FighterAction::EnemyUltimate);

    // Domain Expansion trigger
    if (enemy.id != FighterId::Sukuna && enemy.hp < enemy.maxHp * 0.65f &&
        enemy.ce >= (float)ult.ceCost && enemy.aiCooldown <= 0.0f && randFloat() < 0.25f) {
        out.hasTriggerAction = true;
        out.triggerAction = FighterAction::EnemyUltimate;
        enemy.aiCooldown = 2.0f * aggressionMul;
        return out;
    }

    // Sukuna Boss AI
    if (enemy.id == FighterId::Sukuna) {
        if (enemy.bossPhase == 3 && enemy.ce >= (float)ult.ceCost && enemy.aiCooldown <= 0.0f && randFloat() < 0.35f) {
            out.hasTriggerAction = true;
            out.triggerAction = FighterAction::EnemyUltimate;
            enemy.aiCooldown = 2.4f;
            return out;
        }

        if (absDx < 65.0f) {
            if (enemy.aiCooldown <= 0.0f) {
                float r = randFloat();
                out.hasTriggerAction = true;
                if (r < 0.45f) {
                    out.triggerAction = FighterAction::Light1;
                } else if (r < 0.80f) {
                    out.triggerAction = FighterAction::EnemySpecial2; // Cleave
                } else {
                    out.triggerAction = FighterAction::Heavy;
                }
                enemy.aiCooldown = (enemy.bossPhase >= 2 ? 0.35f : 0.50f) * aggressionMul;
            } else {
                out.wantsBlock = randFloat() < 0.35f;
            }
            return out;
        }

        if (absDx >= 65.0f && absDx < 170.0f) {
            if (enemy.aiCooldown <= 0.0f && randFloat() < 0.55f) {
                out.hasTriggerAction = true;
                out.triggerAction = (randFloat() < 0.6f) ? FighterAction::EnemySpecial1 : FighterAction::EnemySpecial2;
                enemy.aiCooldown = 0.6f * aggressionMul;
            } else {
                out.moveDir = toward;
            }
            return out;
        }

        // Far distance
        if (enemy.aiCooldown <= 0.0f && randFloat() < 0.65f) {
            out.hasTriggerAction = true;
            out.triggerAction = FighterAction::EnemySpecial1; // Dismantle
            enemy.aiCooldown = 0.7f * aggressionMul;
        } else {
            out.moveDir = toward;
            if (randFloat() < 0.15f) out.wantsDash = true;
        }
        return out;
    }

    // General villain AI
    if (absDx < 58.0f) {
        if (enemy.aiCooldown <= 0.0f) {
            out.hasTriggerAction = true;
            out.triggerAction = (randFloat() < 0.6f) ? FighterAction::Light1 : FighterAction::Heavy;
            enemy.aiCooldown = 0.50f * aggressionMul;
        } else {
            out.moveDir = (randFloat() < 0.3f) ? away : 0;
            out.wantsBlock = (randFloat() < 0.3f);
        }
        return out;
    }

    if (absDx >= 58.0f && absDx < 150.0f) {
        if (enemy.aiCooldown <= 0.0f && randFloat() < 0.50f) {
            out.hasTriggerAction = true;
            out.triggerAction = FighterAction::EnemySpecial1;
            enemy.aiCooldown = 0.75f * aggressionMul;
        } else {
            out.moveDir = toward;
        }
        return out;
    }

    // Far distance
    if (enemy.aiCooldown <= 0.0f && randFloat() < 0.40f) {
        out.hasTriggerAction = true;
        out.triggerAction = (randFloat() < 0.6f) ? FighterAction::EnemySpecial1 : FighterAction::EnemySpecial2;
        enemy.aiCooldown = 0.90f * aggressionMul;
    } else {
        out.moveDir = toward;
        if (randFloat() < 0.12f) out.wantsDash = true;
    }

    return out;
}

} // namespace CursedClash
