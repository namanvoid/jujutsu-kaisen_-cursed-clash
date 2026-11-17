#pragma once

#include "Types.hpp"
#include <raylib.h>
#include <vector>
#include <string>

namespace CursedClash {

class Engine {
public:
    GameScreenState screenState = GameScreenState::Title;
    GameMode mode = GameMode::Story;
    int stageIndex = 0;
    ArenaId arenaId = ArenaId::Shibuya;
    FighterId playerId = FighterId::Gojo;
    FighterId enemyId = FighterId::Jogo;
    GameSettings settings;

    int roundNumber = 1;
    RoundPhase roundPhase = RoundPhase::Intro;
    float phaseTimer = 1.6f;
    float roundTimer = ROUND_DURATION;
    int playerRounds = 0;
    int enemyRounds = 0;
    int score = 0;
    int maxCombo = 0;
    int damageDealt = 0;
    float matchTime = 0.0f;
    int winner = 0; // 0 = none, 1 = player, 2 = enemy
    bool storyComplete = false;
    std::string victoryQuote = "";
    std::string victoryQuoteTag = "";
    std::string bannerText = "ROUND 1";
    std::string bannerSubtext = "CURSED CLASH";

    DomainTheme domainActive = DomainTheme::None;
    float domainTimer = 0.0f;
    std::string domainBanner = "";
    float domainCinematicTimer = 0.0f;
    float domainCinematicMax = 0.95f;
    FighterId domainCasterId = FighterId::Gojo;
    std::string domainTitle = "";

    float timeSec = 0.0f;
    Camera2D camera{};
    float trauma = 0.0f;
    float screenFlashAlpha = 0.0f;
    Color screenFlashColor = WHITE;
    float hitstopTimer = 0.0f;
    float afterimageSpawnTimer = 0.0f;
    float shrineSlashTimer = 0.0f;
    bool lastHitWasDomain = false;

    FighterState player;
    FighterState enemy;
    std::vector<Projectile> projectiles;
    std::vector<GroundHazard> hazards;
    std::vector<Particle> particles;
    std::vector<Afterimage> afterimages;
    std::vector<FloatingCallout> callouts;

    Engine();
    void startTitleBackdrop(FighterId heroId = FighterId::Gojo);
    void setTitleHero(FighterId heroId);
    void startStoryMode(int stageIdx = 0, FighterId heroId = FighterId::Gojo);
    void startPracticeMode(FighterId heroId, FighterId opponentId, ArenaId arena);
    void setPaused(bool paused);
    void advanceStoryStage();
    void retryMatch();
    void step(float rawDt, const InputState& input);

    HudSnapshot getHudSnapshot() const;

private:
    void initFighters(FighterId pId, FighterId eId, bool resetPositions = true);
    void resetRound(bool isNewMatch);
    void advanceAfterRound();
    void handlePlayerInput(const InputState& input, float dt);
    void applyAiInput(FighterState& enemyState, const struct AiDecision& ai);
    void updateFighter(FighterState& f, FighterState& other, float dt);
    void updatePhysicsOnly(FighterState& f, float dt);
    void resolvePushbox();
    void startAction(FighterState& f, FighterAction action);
    void executeMoveActiveFrame(FighterState& f, FighterState& target, const MoveSpec& spec);
    void applyHitToTarget(FighterState& attacker, FighterState& defender, const MoveSpec& spec, float diffMul);
    void updateProjectiles(float dt);
    void updateHazards(float dt);
    void updateParticles(float dt);
    void updateCallouts(float dt);
    void spawnAfterimage(const FighterState& f, Color tint);
    void spawnBurst(float x, float y, int count, Color color, float speed);
    void spawnCallout(const std::string& text, float x, float y, Color color);
    void spawnHazard(const GroundHazard& h);
    void spawnProjectile(const Projectile& p);
};

} // namespace CursedClash
