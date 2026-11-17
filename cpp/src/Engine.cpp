#include "Engine.hpp"
#include "Roster.hpp"
#include "Rules.hpp"
#include "Ai.hpp"
#include "Audio.hpp"
#include <cmath>
#include <algorithm>
#include <cstdlib>

namespace CursedClash {

static float randF() {
    return (float)rand() / (float)RAND_MAX;
}

static FighterState createFighter(FighterId id, bool isPlayer, float startX) {
    const auto& prof = Roster::getProfile(id);
    FighterState f;
    f.id = id;
    f.name = prof.name;
    f.title = prof.title;
    f.isPlayer = isPlayer;
    f.x = startX;
    f.y = GROUND_Y;
    f.facing = isPlayer ? 1 : -1;
    f.hp = prof.maxHp;
    f.maxHp = prof.maxHp;
    f.ghostHp = (float)prof.maxHp;
    f.ce = 25.0f;
    f.maxCe = 100.0f;
    return f;
}

Engine::Engine() {
    Roster::init();
    camera.offset = Vector2{ (float)WORLD_WIDTH * 0.5f, (float)WORLD_HEIGHT * 0.5f };
    camera.target = Vector2{ (float)WORLD_WIDTH * 0.5f, (float)WORLD_HEIGHT * 0.5f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    startTitleBackdrop(FighterId::Gojo);
}

void Engine::initFighters(FighterId pId, FighterId eId, bool resetPositions) {
    playerId = pId;
    enemyId = eId;
    player = createFighter(pId, true, resetPositions ? 140.0f : player.x);
    enemy = createFighter(eId, false, resetPositions ? 340.0f : enemy.x);
}

void Engine::startTitleBackdrop(FighterId heroId) {
    screenState = GameScreenState::Title;
    arenaId = ArenaId::Shibuya;
    initFighters(heroId, FighterId::Jogo, true);
    player.ce = 100.0f;
    enemy.ce = 100.0f;
}

void Engine::setTitleHero(FighterId heroId) {
    playerId = heroId;
    player = createFighter(heroId, true, 140.0f);
}

void Engine::resetRound(bool isNewMatch) {
    const auto& pProf = Roster::getProfile(playerId);
    const auto& eProf = Roster::getProfile(enemyId);

    player.x = 140.0f;
    player.y = GROUND_Y;
    player.vx = 0.0f;
    player.vy = 0.0f;
    player.facing = 1;
    player.hp = pProf.maxHp;
    player.ghostHp = (float)pProf.maxHp;
    player.action = FighterAction::Idle;
    player.actionTimer = 0.0f;
    player.hitstunTimer = 0.0f;
    player.invulnTimer = 0.0f;
    player.comboCount = 0;
    player.infinityTimer = 0.0f;
    player.buffTimer = 0.0f;

    enemy.x = 340.0f;
    enemy.y = GROUND_Y;
    enemy.vx = 0.0f;
    enemy.vy = 0.0f;
    enemy.facing = -1;
    enemy.hp = eProf.maxHp;
    enemy.ghostHp = (float)eProf.maxHp;
    enemy.action = FighterAction::Idle;
    enemy.actionTimer = 0.0f;
    enemy.hitstunTimer = 0.0f;
    enemy.invulnTimer = 0.0f;
    enemy.comboCount = 0;
    enemy.infinityTimer = 0.0f;
    enemy.buffTimer = 0.0f;
    enemy.bossPhase = 1;

    if (isNewMatch) {
        player.ce = 25.0f;
        enemy.ce = 25.0f;
        playerRounds = 0;
        enemyRounds = 0;
        roundNumber = 1;
        score = 0;
        maxCombo = 0;
        damageDealt = 0;
        matchTime = 0.0f;
        winner = 0;
        victoryQuote = "";
        victoryQuoteTag = "";
    }

    roundPhase = RoundPhase::Intro;
    phaseTimer = 1.6f;
    roundTimer = ROUND_DURATION;
    bannerText = "ROUND " + std::to_string(roundNumber);
    bannerSubtext = "CURSED CLASH";
    domainActive = DomainTheme::None;
    domainTimer = 0.0f;
    domainBanner = "";
    domainCinematicTimer = 0.0f;
    hitstopTimer = 0.0f;

    projectiles.clear();
    hazards.clear();
    particles.clear();
    afterimages.clear();
    callouts.clear();

    Audio::playSfx(SoundEffect::RoundStart);
}

void Engine::startStoryMode(int stageIdx, FighterId heroId) {
    mode = GameMode::Story;
    stageIndex = stageIdx;
    playerId = heroId;

    const auto& stages = Roster::getStoryStages();
    if (stageIndex >= (int)stages.size()) stageIndex = 0;
    const auto& stg = stages[stageIndex];
    enemyId = stg.opponentId;
    arenaId = stg.arenaId;

    screenState = GameScreenState::Playing;
    storyComplete = false;
    initFighters(playerId, enemyId, true);
    resetRound(true);
}

void Engine::startPracticeMode(FighterId heroId, FighterId opponentId, ArenaId arena) {
    mode = GameMode::Practice;
    playerId = heroId;
    enemyId = opponentId;
    arenaId = arena;

    screenState = GameScreenState::Playing;
    storyComplete = false;
    initFighters(playerId, enemyId, true);
    resetRound(true);
}

void Engine::setPaused(bool paused) {
    if (screenState == GameScreenState::Playing && paused) {
        screenState = GameScreenState::Paused;
    } else if (screenState == GameScreenState::Paused && !paused) {
        screenState = GameScreenState::Playing;
    }
}

void Engine::retryMatch() {
    screenState = GameScreenState::Playing;
    resetRound(true);
}

void Engine::advanceStoryStage() {
    const auto& stages = Roster::getStoryStages();
    if (stageIndex + 1 < (int)stages.size()) {
        startStoryMode(stageIndex + 1, playerId);
    } else {
        storyComplete = true;
        screenState = GameScreenState::Title;
    }
}

void Engine::step(float rawDt, const InputState& input) {
    float dt = std::min(rawDt, 0.05f);
    timeSec += dt;

    trauma = std::max(0.0f, trauma - dt * 2.2f);
    screenFlashAlpha = std::max(0.0f, screenFlashAlpha - dt * 2.8f);

    if (screenState == GameScreenState::Title) {
        player.y = GROUND_Y;
        enemy.y = GROUND_Y;
        updateParticles(dt);
        return;
    }

    if (screenState != GameScreenState::Playing) {
        return;
    }

    // Dynamic camera
    Rules::computeDynamicCamera(player.x, enemy.x, player.y, enemy.y, camera, dt);

    if (domainCinematicTimer > 0.0f) {
        domainCinematicTimer = std::max(0.0f, domainCinematicTimer - dt);
        updateParticles(dt);
        return;
    }

    if (hitstopTimer > 0.0f) {
        hitstopTimer = std::max(0.0f, hitstopTimer - dt);
        player.flashTimer = std::max(0.0f, player.flashTimer - dt);
        enemy.flashTimer = std::max(0.0f, enemy.flashTimer - dt);
        return;
    }

    if (roundPhase == RoundPhase::Intro) {
        phaseTimer -= dt;
        if (phaseTimer < 0.55f) {
            bannerText = "FIGHT!";
            bannerSubtext = "";
        }
        if (phaseTimer <= 0.0f) {
            roundPhase = RoundPhase::Fight;
            bannerText = "";
            bannerSubtext = "";
        }
        updateParticles(dt);
        return;
    }

    if (roundPhase == RoundPhase::RoundEnd) {
        phaseTimer -= dt;
        updatePhysicsOnly(player, dt);
        updatePhysicsOnly(enemy, dt);
        updateParticles(dt);
        updateCallouts(dt);
        if (phaseTimer <= 0.0f) {
            advanceAfterRound();
        }
        return;
    }

    if (roundPhase == RoundPhase::MatchEnd) {
        updateParticles(dt);
        return;
    }

    // Active Fight
    matchTime += dt;
    if (mode != GameMode::Practice) {
        roundTimer = std::max(0.0f, roundTimer - dt);
    }

    // Passive CE regen
    float ceRegen = (mode == GameMode::Practice && settings.practiceAutoCe) ? 28.0f : 4.2f;
    player.ce = std::min(player.maxCe, player.ce + ceRegen * dt);
    enemy.ce = std::min(enemy.maxCe, enemy.ce + ((enemy.id == FighterId::Sukuna) ? 7.5f : 5.2f) * dt);

    // Sukuna boss phase
    if (enemy.id == FighterId::Sukuna) {
        enemy.bossPhase = Rules::computeSukunaPhase(enemy.hp, enemy.maxHp);
    }

    // Domain timer
    if (domainTimer > 0.0f) {
        domainTimer -= dt;
        if (domainTimer <= 0.0f) {
            domainActive = DomainTheme::None;
            domainBanner = "";
        }
    }

    // Sukuna Phase 3 slashes
    if (enemy.id == FighterId::Sukuna && enemy.bossPhase == 3 && domainActive == DomainTheme::MalevolentShrineTheme) {
        shrineSlashTimer -= dt;
        if (shrineSlashTimer <= 0.0f) {
            shrineSlashTimer = 1.65f;
            GroundHazard h;
            h.active = true;
            h.isPlayerOwner = false;
            h.kind = HazardKind::ShrineSlash;
            h.x = Rules::clampStageX(player.x + (randF() - 0.5f) * 32.0f);
            h.width = 34.0f;
            h.telegraphTimer = 0.52f;
            h.activeTimer = 0.22f;
            h.damage = (int)(18 * Rules::getDifficultyDamageMultiplier(settings.difficulty));
            h.knockbackX = 85.0f;
            h.knockbackY = -95.0f;
            spawnHazard(h);
        }
    }

    // Facing directions
    if (player.action == FighterAction::Idle || player.action == FighterAction::Walk) {
        player.facing = (enemy.x >= player.x) ? 1 : -1;
    }
    if (enemy.action == FighterAction::Idle || enemy.action == FighterAction::Walk) {
        enemy.facing = (player.x >= enemy.x) ? 1 : -1;
    }

    // Input & AI
    handlePlayerInput(input, dt);

    if (mode != GameMode::Practice || settings.practiceAiActive) {
        bool threatClose = false;
        for (const auto& p : projectiles) {
            if (p.active && p.isPlayerOwner && std::abs(p.x - enemy.x) < 90.0f) {
                threatClose = true; break;
            }
        }
        AiDecision ai = Ai::decideVillainAi(enemy, player, dt, settings.difficulty, threatClose);
        applyAiInput(enemy, ai);
    }

    updateFighter(player, enemy, dt);
    updateFighter(enemy, player, dt);
    resolvePushbox();

    updateProjectiles(dt);
    updateHazards(dt);
    updateParticles(dt);
    updateCallouts(dt);

    // Check round over
    if (player.hp <= 0 || enemy.hp <= 0 || (mode != GameMode::Practice && roundTimer <= 0.0f)) {
        roundPhase = RoundPhase::RoundEnd;
        phaseTimer = 2.4f;

        if (player.hp <= 0 && enemy.hp <= 0) {
            bannerText = "DOUBLE KO";
        } else if (player.hp <= 0) {
            bannerText = "K.O.";
            bannerSubtext = enemy.name + " WINS";
            enemyRounds++;
            player.action = FighterAction::KO;
            Audio::playSfx(SoundEffect::KO);
        } else if (enemy.hp <= 0) {
            bannerText = "K.O.";
            bannerSubtext = player.name + " WINS";
            playerRounds++;
            enemy.action = FighterAction::KO;
            Audio::playSfx(SoundEffect::KO);
        } else {
            // Time out
            bannerText = "TIME UP";
            if (player.hp > enemy.hp) { playerRounds++; bannerSubtext = player.name + " WINS"; }
            else if (enemy.hp > player.hp) { enemyRounds++; bannerSubtext = enemy.name + " WINS"; }
            else bannerSubtext = "DRAW";
        }
    }
}

void Engine::advanceAfterRound() {
    int targetRounds = 2;
    if (playerRounds >= targetRounds) {
        roundPhase = RoundPhase::MatchEnd;
        winner = 1;
        screenState = GameScreenState::GameOver;
        bool flawless = (player.hp == player.maxHp);
        auto quote = Roster::selectVictoryQuote(playerId, enemyId, lastHitWasDomain, flawless);
        victoryQuote = quote.first;
        victoryQuoteTag = quote.second;
        Audio::playSfx(SoundEffect::Victory);
    } else if (enemyRounds >= targetRounds) {
        roundPhase = RoundPhase::MatchEnd;
        winner = 2;
        screenState = GameScreenState::GameOver;
        bool flawless = (enemy.hp == enemy.maxHp);
        auto quote = Roster::selectVictoryQuote(enemyId, playerId, lastHitWasDomain, flawless);
        victoryQuote = quote.first;
        victoryQuoteTag = quote.second;
        Audio::playSfx(SoundEffect::Defeat);
    } else {
        roundNumber++;
        resetRound(false);
    }
}

void Engine::handlePlayerInput(const InputState& input, float dt) {
    auto& p = player;
    bool airborne = (p.y < GROUND_Y - 1.0f);
    int moveDir = input.left ? -1 : (input.right ? 1 : 0);

    if (p.stunLockedTimer > 0.0f || p.hitstunTimer > 0.0f || p.action == FighterAction::KO) {
        return;
    }

    if (p.bufferTimer > 0.0f) {
        p.bufferTimer = std::max(0.0f, p.bufferTimer - dt);
        if (p.bufferTimer <= 0.0f) p.hasBufferedAction = false;
    }

    // Parry
    if (input.justParry && (p.action == FighterAction::Idle || p.action == FighterAction::Walk ||
                            p.action == FighterAction::Block || p.action == FighterAction::Jump)) {
        p.action = FighterAction::Parry;
        p.actionTimer = 0.0f;
        p.parryWindowTimer = 0.24f;
        if (!airborne) p.vx = 0.0f;
        Audio::playSfx(SoundEffect::Block);
        return;
    }

    auto canCast = [&](FighterAction act) {
        MoveSpec s;
        return Rules::getPlayerMoveSpec(p.id, act, s) && p.ce >= s.ceCost;
    };

    bool hasRequested = false;
    FighterAction requested = FighterAction::Idle;

    if (input.justSkill5 && canCast(FighterAction::Skill5Domain)) {
        hasRequested = true; requested = FighterAction::Skill5Domain;
    } else if (input.justSkill3 && canCast(FighterAction::Skill3)) {
        hasRequested = true; requested = FighterAction::Skill3;
    } else if (input.justSkill2 && canCast(FighterAction::Skill2)) {
        hasRequested = true; requested = FighterAction::Skill2;
    } else if (input.justSkill1 && canCast(FighterAction::Skill1)) {
        hasRequested = true; requested = FighterAction::Skill1;
    } else if (input.justSkill4 && canCast(FighterAction::Skill4) && !(p.id == FighterId::Gojo && p.infinityTimer > 0.2f)) {
        hasRequested = true; requested = FighterAction::Skill4;
    } else if (input.justHeavy) {
        hasRequested = true; requested = FighterAction::Heavy;
    } else if (input.justLight) {
        hasRequested = true;
        if (p.action == FighterAction::Light1) requested = FighterAction::Light2;
        else if (p.action == FighterAction::Light2) requested = FighterAction::Light3;
        else requested = FighterAction::Light1;
    }

    if (hasRequested) {
        if (p.action == FighterAction::Idle || p.action == FighterAction::Walk ||
            p.action == FighterAction::Jump || p.action == FighterAction::Block) {
            startAction(p, requested);
            return;
        } else if (Rules::canChainAttack(p.id, p.action, requested)) {
            MoveSpec spec;
            if (Rules::getPlayerMoveSpec(p.id, p.action, spec) && p.actionTimer >= spec.startup + spec.active * 0.5f) {
                startAction(p, requested);
                return;
            }
            p.hasBufferedAction = true;
            p.bufferedAction = requested;
            p.bufferTimer = 0.14f;
        } else {
            p.hasBufferedAction = true;
            p.bufferedAction = requested;
            p.bufferTimer = 0.14f;
        }
    }

    if ((p.action == FighterAction::Idle || p.action == FighterAction::Walk || p.action == FighterAction::Jump) && p.hasBufferedAction) {
        FighterAction act = p.bufferedAction;
        p.hasBufferedAction = false;
        p.bufferTimer = 0.0f;
        startAction(p, act);
        return;
    }

    const auto& prof = Roster::getProfile(p.id);

    if (airborne && p.action != FighterAction::Dash) {
        if (moveDir != 0) p.vx = moveDir * prof.speed * 0.96f;
        else p.vx *= std::pow(0.08f, dt);
    }

    bool canMove = (p.action == FighterAction::Idle || p.action == FighterAction::Walk ||
                    p.action == FighterAction::Jump || p.action == FighterAction::Block);
    if (!canMove) return;

    // Sidestep
    if (input.justDown && !airborne) {
        p.action = FighterAction::Sidestep;
        p.actionTimer = 0.0f;
        p.invulnTimer = 0.24f;
        p.vx = 0.0f;
        Audio::playSfx(SoundEffect::Dash);
        spawnAfterimage(p, prof.accentColor);
        return;
    }

    // Dash
    if (input.justDash) {
        int dir = input.left ? -1 : (input.right ? 1 : p.facing);
        p.action = FighterAction::Dash;
        p.actionTimer = 0.0f;
        p.invulnTimer = 0.16f;
        p.vx = dir * 290.0f;
        Audio::playSfx(SoundEffect::Dash);
        spawnAfterimage(p, prof.accentColor);
        return;
    }

    // Block
    if (input.block && !airborne) {
        if (p.action != FighterAction::Block) {
            p.action = FighterAction::Block;
            p.blockWindowTimer = 0.16f;
        }
        p.vx = 0.0f;
        return;
    } else if (p.action == FighterAction::Block) {
        p.action = FighterAction::Idle;
    }

    // Jump
    if (input.justUp && !airborne) {
        p.vy = prof.jumpForce;
        p.action = FighterAction::Jump;
        p.vx = moveDir * prof.speed;
        Audio::playSfx(SoundEffect::Jump);
        return;
    }

    // Ground movement
    if (airborne) {
        p.vx = (moveDir != 0) ? moveDir * prof.speed * 0.96f : 0.0f;
    } else if (moveDir != 0) {
        p.vx = moveDir * prof.speed;
        p.action = FighterAction::Walk;
        p.walkDist += std::abs(p.vx * dt);
    } else {
        p.vx = 0.0f;
        if (p.action == FighterAction::Walk) p.action = FighterAction::Idle;
    }
}

void Engine::applyAiInput(FighterState& enemyState, const AiDecision& ai) {
    if (ai.hasTriggerAction) {
        startAction(enemyState, ai.triggerAction);
        return;
    }

    const auto& prof = Roster::getProfile(enemyState.id);
    bool airborne = (enemyState.y < GROUND_Y - 1.0f);

    if (ai.wantsParry && !airborne) {
        enemyState.action = FighterAction::Parry;
        enemyState.actionTimer = 0.0f;
        enemyState.parryWindowTimer = 0.24f;
        enemyState.vx = 0.0f;
        Audio::playSfx(SoundEffect::Block);
        return;
    }

    if (ai.wantsSidestep && !airborne) {
        enemyState.action = FighterAction::Sidestep;
        enemyState.actionTimer = 0.0f;
        enemyState.invulnTimer = 0.24f;
        enemyState.vx = 0.0f;
        Audio::playSfx(SoundEffect::Dash);
        spawnAfterimage(enemyState, prof.accentColor);
        return;
    }

    if (ai.wantsDash) {
        enemyState.action = FighterAction::Dash;
        enemyState.actionTimer = 0.0f;
        enemyState.invulnTimer = 0.16f;
        enemyState.vx = enemyState.facing * 290.0f;
        Audio::playSfx(SoundEffect::Dash);
        spawnAfterimage(enemyState, prof.accentColor);
        return;
    }

    if (ai.wantsBlock && !airborne) {
        enemyState.action = FighterAction::Block;
        enemyState.vx = 0.0f;
        return;
    }

    if (ai.moveDir != 0 && !airborne) {
        enemyState.action = FighterAction::Walk;
        enemyState.vx = ai.moveDir * prof.speed;
        enemyState.walkDist += std::abs(enemyState.vx * 0.016f);
    } else if (!airborne && enemyState.action == FighterAction::Walk) {
        enemyState.action = FighterAction::Idle;
        enemyState.vx = 0.0f;
    }
}

void Engine::updateFighter(FighterState& f, FighterState& other, float dt) {
    f.flashTimer = std::max(0.0f, f.flashTimer - dt);
    f.parryFlashTimer = std::max(0.0f, f.parryFlashTimer - dt);
    f.invulnTimer = std::max(0.0f, f.invulnTimer - dt);
    f.blockWindowTimer = std::max(0.0f, f.blockWindowTimer - dt);
    f.parryWindowTimer = std::max(0.0f, f.parryWindowTimer - dt);
    f.infinityTimer = std::max(0.0f, f.infinityTimer - dt);
    f.buffTimer = std::max(0.0f, f.buffTimer - dt);
    f.stunLockedTimer = std::max(0.0f, f.stunLockedTimer - dt);

    if (f.ghostHp > (float)f.hp) {
        f.ghostHp = std::max((float)f.hp, f.ghostHp - 65.0f * dt);
    }

    if (f.comboResetTimer > 0.0f) {
        f.comboResetTimer -= dt;
        if (f.comboResetTimer <= 0.0f) f.comboCount = 0;
    }

    if (f.stunLockedTimer > 0.0f) {
        f.vx *= std::pow(0.05f, dt);
        updatePhysicsOnly(f, dt);
        return;
    }

    if (f.hitstunTimer > 0.0f) {
        f.hitstunTimer -= dt;
        f.vx *= std::pow(0.08f, dt);
        updatePhysicsOnly(f, dt);
        if (f.hitstunTimer <= 0.0f && f.hp > 0) {
            f.action = (f.y < GROUND_Y - 1.0f) ? FighterAction::Jump : FighterAction::Idle;
        }
        return;
    }

    if (f.action == FighterAction::Dash) {
        f.actionTimer += dt;
        if (f.actionTimer >= 0.22f) {
            f.vx = 0.0f;
            f.action = (f.y < GROUND_Y - 1.0f) ? FighterAction::Jump : FighterAction::Idle;
        }
    } else if (f.action == FighterAction::Sidestep) {
        f.actionTimer += dt;
        if (f.actionTimer >= 0.28f) f.action = FighterAction::Idle;
    } else if (f.action == FighterAction::Parry) {
        f.actionTimer += dt;
        if (f.actionTimer >= 0.30f) {
            f.parryWindowTimer = 0.0f;
            f.action = (f.y < GROUND_Y - 1.0f) ? FighterAction::Jump : FighterAction::Idle;
        }
    } else if (f.action != FighterAction::Idle && f.action != FighterAction::Walk &&
               f.action != FighterAction::Jump && f.action != FighterAction::Block &&
               f.action != FighterAction::KO && f.action != FighterAction::Victory) {
        MoveSpec spec;
        bool found = f.isPlayer ? Rules::getPlayerMoveSpec(f.id, f.action, spec)
                                : (spec = Rules::getEnemyMoveSpec(f.id, f.action), true);
        if (found) {
            f.actionTimer += dt;
            if (!spec.isProjectile && f.y >= GROUND_Y - 1.0f &&
                f.actionTimer >= spec.startup * 0.7f && f.actionTimer <= spec.startup + spec.active) {
                if (!f.actionHitApplied) {
                    f.x = Rules::clampStageX(f.x + f.facing * 65.0f * dt);
                }
            }
            if (!f.actionHitApplied && f.actionTimer >= spec.startup) {
                f.actionHitApplied = true;
                executeMoveActiveFrame(f, other, spec);
            }
            if (f.actionTimer >= spec.startup + spec.active + spec.recovery) {
                f.superArmorActive = false;
                f.action = (f.y < GROUND_Y - 1.0f) ? FighterAction::Jump : FighterAction::Idle;
            }
        } else {
            f.action = FighterAction::Idle;
        }
    }

    updatePhysicsOnly(f, dt);
}

void Engine::updatePhysicsOnly(FighterState& f, float dt) {
    if (f.y < GROUND_Y) {
        f.vy += 850.0f * dt;
    }
    f.x += f.vx * dt;
    f.y += f.vy * dt;

    if (f.y >= GROUND_Y) {
        f.y = GROUND_Y;
        f.vy = 0.0f;
        if (f.action == FighterAction::Jump) {
            f.action = FighterAction::Idle;
        }
    }
    f.x = Rules::clampStageX(f.x);
}

void Engine::resolvePushbox() {
    float dist = std::abs(player.x - enemy.x);
    float minDist = 24.0f;
    if (dist < minDist) {
        float push = (minDist - dist) * 0.5f;
        if (player.x <= enemy.x) {
            player.x = Rules::clampStageX(player.x - push);
            enemy.x = Rules::clampStageX(enemy.x + push);
        } else {
            player.x = Rules::clampStageX(player.x + push);
            enemy.x = Rules::clampStageX(enemy.x - push);
        }
    }
}

void Engine::startAction(FighterState& f, FighterAction action) {
    MoveSpec spec;
    bool found = f.isPlayer ? Rules::getPlayerMoveSpec(f.id, action, spec)
                            : (spec = Rules::getEnemyMoveSpec(f.id, action), true);
    if (!found) return;
    if (f.ce < (float)spec.ceCost) return;

    f.ce -= (float)spec.ceCost;
    f.action = action;
    f.actionTimer = 0.0f;
    f.actionHitApplied = false;
    f.superArmorActive = spec.superArmor;
    if (f.y >= GROUND_Y - 1.0f) f.vx = 0.0f;

    const auto& prof = Roster::getProfile(f.id);

    if (action == FighterAction::Skill5Domain || action == FighterAction::EnemyUltimate) {
        domainActive = prof.domainTheme;
        domainTimer = 4.2f;
        domainBanner = "DOMAIN EXPANSION: " + prof.domainName;
        domainCinematicMax = 0.85f;
        domainCinematicTimer = 0.85f;
        domainCasterId = f.id;
        domainTitle = prof.domainName;
        screenFlashColor = prof.accentColor;
        screenFlashAlpha = 0.65f;
        trauma = 0.85f;
        Audio::playSfx((f.id == FighterId::Gojo) ? SoundEffect::Void :
                       (f.id == FighterId::Sukuna) ? SoundEffect::Shrine : SoundEffect::Domain);
        spawnBurst(f.x, f.y - 26.0f, 32, prof.accentColor, 195.0f);
        return;
    }

    if (action == FighterAction::Skill4) {
        if (f.id == FighterId::Gojo) {
            f.infinityTimer = 3.6f;
            Audio::playSfx(SoundEffect::Infinity);
            spawnBurst(f.x, f.y - 24.0f, 14, hexToColor("#38bdf8"), 110.0f);
            spawnCallout("INFINITY", f.x, f.y - 52.0f, hexToColor("#38bdf8"));
        } else if (f.id == FighterId::Yuta) {
            f.hp = std::min(f.maxHp, f.hp + 48);
            Audio::playSfx(SoundEffect::Heal);
            spawnBurst(f.x, f.y - 26.0f, 16, hexToColor("#34d399"), 100.0f);
            spawnCallout("RCT +48 HP", f.x, f.y - 54.0f, hexToColor("#34d399"));
        } else {
            f.buffTimer = 5.0f;
            spawnCallout("BUFF ACTIVE", f.x, f.y - 54.0f, prof.accentColor);
        }
    }
}

void Engine::executeMoveActiveFrame(FighterState& f, FighterState& target, const MoveSpec& spec) {
    float diffMul = f.isPlayer ? 1.0f : Rules::getDifficultyDamageMultiplier(settings.difficulty);

    // Projectiles
    if (spec.isProjectile) {
        Projectile p;
        p.active = true;
        p.isPlayerOwner = f.isPlayer;
        p.x = f.x + f.facing * 24.0f;
        p.y = f.y - 24.0f;
        p.vx = f.facing * 280.0f;
        p.vy = 0.0f;
        p.radius = 12.0f;
        p.damage = (int)(spec.damage * diffMul);
        p.chipDamage = (int)(spec.chipDamage * diffMul);
        p.knockbackX = spec.knockbackX;
        p.knockbackY = spec.knockbackY;
        p.life = 1.3f;
        p.maxLife = 1.3f;
        p.piercesInfinity = spec.piercesInfinity;

        if (f.id == FighterId::Gojo) {
            if (spec.id == FighterAction::Skill1) {
                p.kind = ProjectileKind::Blue;
                p.vx = f.facing * 140.0f;
                p.pullsEnemy = true;
                p.hitsRemaining = 3;
            } else if (spec.id == FighterAction::Skill2) {
                p.kind = ProjectileKind::Red;
            } else {
                p.kind = ProjectileKind::Purple;
                p.radius = 18.0f;
            }
        } else if (f.id == FighterId::Sukuna) {
            p.kind = (spec.id == FighterAction::Skill3) ? ProjectileKind::SukunaFlame : ProjectileKind::SukunaDismantle;
        } else {
            p.kind = ProjectileKind::Red;
        }
        spawnProjectile(p);
        return;
    }

    // Melee hitbox check
    float dx = (target.x - f.x) * f.facing;
    float dy = std::abs(target.y - f.y);
    if (dx >= -14.0f && dx <= spec.range && dy <= spec.heightRange) {
        applyHitToTarget(f, target, spec, diffMul);
    }
}

void Engine::applyHitToTarget(FighterState& attacker, FighterState& defender, const MoveSpec& spec, float diffMul) {
    HitResolution res = Rules::resolveHit(attacker, defender, spec, diffMul);
    if (!res.hit) return;

    if (res.infinityBlocked) {
        hitstopTimer = res.hitstop;
        spawnBurst(defender.x - attacker.facing * 12.0f, defender.y - 24.0f, 10, hexToColor("#38bdf8"), 90.0f);
        spawnCallout("INFINITY", defender.x, defender.y - 52.0f, hexToColor("#38bdf8"));
        Audio::playSfx(SoundEffect::Infinity);
        defender.ce = std::min(defender.maxCe, defender.ce + (float)res.ceGainDefender);
        return;
    }

    if (res.parried) {
        defender.parryFlashTimer = 0.28f;
        defender.parryWindowTimer = 0.0f;
        defender.ce = std::min(defender.maxCe, defender.ce + (float)res.ceGainDefender);
        attacker.hitstunTimer = 0.55f;
        attacker.action = FighterAction::Hitstun;
        attacker.vx = -attacker.facing * 105.0f;
        hitstopTimer = res.hitstop;
        spawnBurst(defender.x + defender.facing * 10.0f, defender.y - 26.0f, 18, hexToColor("#fde047"), 155.0f);
        spawnCallout("PARRY!", defender.x, defender.y - 56.0f, hexToColor("#fde047"));
        Audio::playSfx(SoundEffect::Parry);
        return;
    }

    defender.hp = std::max(0, defender.hp - res.damage);
    if (res.damage > 0) {
        lastHitWasDomain = (spec.id == FighterAction::Skill5Domain || spec.id == FighterAction::EnemyUltimate);
    }
    attacker.ce = std::min(attacker.maxCe, attacker.ce + (float)res.ceGainAttacker);
    defender.ce = std::min(defender.maxCe, defender.ce + (float)res.ceGainDefender);

    if (attacker.isPlayer) {
        damageDealt += res.damage;
        score += res.damage * 10 * std::max(1, attacker.comboCount);
    }

    if (res.blocked) {
        defender.vx = res.knockbackX;
        hitstopTimer = res.hitstop;
        spawnBurst(defender.x, defender.y - 24.0f, 8, hexToColor("#94a3b8"), 80.0f);
        Audio::playSfx(SoundEffect::Block);
        return;
    }

    defender.flashTimer = 0.07f;
    if (res.hitstun > 0.0f) {
        defender.hitstunTimer = res.hitstun;
        defender.action = FighterAction::Hitstun;
    }
    defender.vx = res.knockbackX;
    if (res.knockbackY < -10.0f) {
        defender.vy = res.knockbackY;
    }
    hitstopTimer = res.hitstop;

    attacker.comboCount += 1;
    attacker.comboResetTimer = 1.65f;
    if (attacker.isPlayer && attacker.comboCount > maxCombo) {
        maxCombo = attacker.comboCount;
    }

    bool isHeavy = (spec.damage >= 24);
    trauma = std::max(trauma, isHeavy ? 0.42f : 0.16f);
    Audio::playSfx(isHeavy ? SoundEffect::HeavyHit : SoundEffect::LightHit);
    Color accent = Roster::getProfile(attacker.id).accentColor;
    spawnBurst(defender.x, defender.y - 24.0f, isHeavy ? 20 : 9, accent, isHeavy ? 170.0f : 105.0f);

    if (attacker.comboCount > 1) {
        spawnCallout(std::to_string(attacker.comboCount) + " HITS", defender.x, defender.y - 48.0f, accent);
    }
}

void Engine::updateProjectiles(float dt) {
    for (auto& p : projectiles) {
        if (!p.active) continue;
        p.life -= dt;
        if (p.life <= 0.0f) { p.active = false; continue; }
        p.x += p.vx * dt;
        p.y += p.vy * dt;

        FighterState& target = p.isPlayerOwner ? enemy : player;
        float dist = std::hypot(p.x - target.x, p.y - (target.y - 24.0f));
        if (dist <= p.radius + 14.0f) {
            MoveSpec spec;
            spec.id = FighterAction::Light1;
            spec.damage = p.damage;
            spec.chipDamage = p.chipDamage;
            spec.knockbackX = p.knockbackX;
            spec.knockbackY = p.knockbackY;
            spec.hitstop = 0.05f;
            spec.hitstun = 0.35f;
            spec.piercesInfinity = p.piercesInfinity;

            FighterState& attacker = p.isPlayerOwner ? player : enemy;
            applyHitToTarget(attacker, target, spec, 1.0f);
            p.hitsRemaining--;
            if (p.hitsRemaining <= 0) p.active = false;
        }
    }
}

void Engine::updateHazards(float dt) {
    for (auto& h : hazards) {
        if (!h.active) continue;
        if (h.telegraphTimer > 0.0f) {
            h.telegraphTimer -= dt;
            continue;
        }
        h.activeTimer -= dt;
        if (h.activeTimer <= 0.0f) { h.active = false; continue; }

        if (!h.hitApplied) {
            FighterState& target = h.isPlayerOwner ? enemy : player;
            if (std::abs(target.x - h.x) <= h.width * 0.5f && target.y >= GROUND_Y - 30.0f) {
                h.hitApplied = true;
                MoveSpec spec;
                spec.damage = h.damage;
                spec.chipDamage = h.damage / 3;
                spec.knockbackX = h.knockbackX;
                spec.knockbackY = h.knockbackY;
                spec.hitstop = 0.06f;
                spec.hitstun = 0.40f;
                FighterState& attacker = h.isPlayerOwner ? player : enemy;
                applyHitToTarget(attacker, target, spec, 1.0f);
            }
        }
    }
}

void Engine::updateParticles(float dt) {
    for (auto& pt : particles) {
        if (!pt.active) continue;
        pt.life -= dt;
        if (pt.life <= 0.0f) { pt.active = false; continue; }
        pt.vy += pt.gravity * dt;
        pt.x += pt.vx * dt;
        pt.y += pt.vy * dt;
    }
}

void Engine::updateCallouts(float dt) {
    for (auto& c : callouts) {
        if (!c.active) continue;
        c.life -= dt;
        if (c.life <= 0.0f) { c.active = false; continue; }
        c.y += c.vy * dt;
    }
}

void Engine::spawnAfterimage(const FighterState& f, Color tint) {
    Afterimage ai;
    ai.active = true;
    ai.fighterId = f.id;
    ai.action = f.action;
    ai.x = f.x;
    ai.y = f.y;
    ai.facing = f.facing;
    ai.alpha = 0.7f;
    ai.tint = tint;
    afterimages.push_back(ai);
}

void Engine::spawnBurst(float x, float y, int count, Color color, float speed) {
    for (int i = 0; i < count; ++i) {
        Particle p;
        p.active = true;
        p.x = x;
        p.y = y;
        float angle = randF() * 6.28318f;
        float spd = speed * (0.35f + randF() * 0.65f);
        p.vx = std::cos(angle) * spd;
        p.vy = std::sin(angle) * spd;
        p.size = 2.0f + randF() * 2.0f;
        p.color = color;
        p.life = 0.25f + randF() * 0.35f;
        p.maxLife = p.life;
        p.gravity = 120.0f;
        particles.push_back(p);
    }
}

void Engine::spawnCallout(const std::string& text, float x, float y, Color color) {
    FloatingCallout c;
    c.active = true;
    c.text = text;
    c.x = x;
    c.y = y;
    c.vy = -30.0f;
    c.color = color;
    c.life = 0.85f;
    c.maxLife = 0.85f;
    callouts.push_back(c);
}

void Engine::spawnHazard(const GroundHazard& h) {
    hazards.push_back(h);
}

void Engine::spawnProjectile(const Projectile& p) {
    projectiles.push_back(p);
}

HudSnapshot Engine::getHudSnapshot() const {
    HudSnapshot s;
    s.screenState = screenState;
    s.mode = mode;
    s.stageIndex = stageIndex;
    s.totalStages = (int)Roster::getStoryStages().size();
    s.arenaId = arenaId;
    s.arenaName = Roster::getArena(arenaId).name;
    s.roundNumber = roundNumber;
    s.roundPhase = roundPhase;
    s.roundTimer = roundTimer;
    s.bannerText = bannerText;
    s.bannerSubtext = bannerSubtext;

    s.playerId = player.id;
    s.playerName = player.name;
    s.playerHp = player.hp;
    s.playerMaxHp = player.maxHp;
    s.playerGhostHp = player.ghostHp;
    s.playerCe = player.ce;
    s.playerInfinity = player.infinityTimer;
    s.playerBuffTimer = player.buffTimer;
    s.playerRounds = playerRounds;
    s.playerCombo = player.comboCount;
    s.playerComboMultiplier = Rules::computeComboMultiplier(player.comboCount);
    s.playerSkills = Roster::getSkills(player.id);

    s.enemyId = enemy.id;
    s.enemyName = enemy.name;
    s.enemyTitle = enemy.title;
    s.enemyHp = enemy.hp;
    s.enemyMaxHp = enemy.maxHp;
    s.enemyGhostHp = enemy.ghostHp;
    s.enemyCe = enemy.ce;
    s.enemyRounds = enemyRounds;
    s.enemyCombo = enemy.comboCount;
    s.enemyBossPhase = enemy.bossPhase;

    s.score = score;
    s.maxCombo = maxCombo;
    s.damageDealt = damageDealt;
    s.matchTime = matchTime;
    s.domainActive = domainActive;
    s.domainBanner = domainBanner;
    s.cameraZoom = camera.zoom;
    s.winner = winner;
    s.storyComplete = storyComplete;
    s.victoryQuote = victoryQuote;
    s.victoryQuoteTag = victoryQuoteTag;
    return s;
}

} // namespace CursedClash
