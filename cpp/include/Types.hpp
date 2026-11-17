#pragma once

#include <string>
#include <vector>
#include <raylib.h>

namespace CursedClash {

constexpr int WORLD_WIDTH = 480;
constexpr int WORLD_HEIGHT = 270;
constexpr float GROUND_Y = 232.0f;
constexpr float STAGE_LEFT = 24.0f;
constexpr float STAGE_RIGHT = 456.0f;
constexpr float ROUND_DURATION = 99.0f;

enum class GameScreenState {
    Title,
    Playing,
    Paused,
    GameOver
};

enum class GameMode {
    Story,
    Practice
};

enum class Difficulty {
    Grade1,
    Special,
    King
};

enum class FighterId {
    Gojo,
    Yuta,
    Megumi,
    Panda,
    Jogo,
    Mahito,
    Hanami,
    Choso,
    Geto,
    Kenjaku,
    Uraume,
    Kashimo,
    Toji,
    Sukuna
};

enum class ArenaId {
    JujutsuHigh,
    AbandonedSchool,
    CursedForest,
    Shibuya,
    MalevolentShrine
};

enum class DomainTheme {
    None,
    UnlimitedVoid,
    MutualLove,
    ChimeraShadow,
    BeastSanctuary,
    IronMountain,
    Perfection,
    ForestBranches,
    BloodEclipse,
    WombProfusion,
    AbsoluteZero,
    Thunderclap,
    HeavenlyZero,
    MalevolentShrineTheme
};

enum class FighterAction {
    Idle,
    Walk,
    Jump,
    Dash,
    Sidestep,
    Block,
    Parry,
    Light1,
    Light2,
    Light3,
    Heavy,
    Skill1,
    Skill2,
    Skill3,
    Skill4,
    Skill5Domain,
    EnemySpecial1,
    EnemySpecial2,
    EnemySpecial3,
    EnemyUltimate,
    Hitstun,
    Knockdown,
    Victory,
    KO
};

enum class RoundPhase {
    Intro,
    Fight,
    RoundEnd,
    PhaseTransition,
    MatchEnd
};

enum class ProjectileKind {
    Blue,
    Red,
    Purple,
    RikaClaw,
    CursedSpeech,
    PureLove,
    DivineDog,
    NueBolt,
    ElephantWave,
    DrummingWave,
    JogoFireball,
    ChosoPiercing,
    ChosoOrb,
    HanamiBeam,
    GetoSwarm,
    KenjakuUzumaki,
    UraumeIce,
    KashimoBolt,
    SukunaDismantle,
    SukunaFlame
};

enum class HazardKind {
    JogoGeyser,
    HanamiRoots,
    ShrineSlash,
    MahitoSpike,
    KatanaRain,
    ShadowSpire,
    IcePillar,
    GravityCrush,
    LightningStrike,
    BeastPillar
};

enum class SoundEffect {
    LightHit,
    HeavyHit,
    Block,
    Parry,
    Dash,
    Jump,
    Blue,
    Red,
    Purple,
    Infinity,
    Void,
    Katana,
    Heal,
    Ice,
    Lightning,
    Drum,
    Fire,
    Blood,
    Blade,
    Roots,
    Slash,
    Shrine,
    Domain,
    RoundStart,
    KO,
    Victory,
    Defeat,
    UiClick
};

struct SkillSlotMeta {
    int slot;
    FighterAction action;
    std::string keyLabel;
    std::string name;
    std::string shortName;
    int cost;
    Color color;
    std::string description;
};

struct MoveSpec {
    FighterAction id;
    std::string name;
    float startup;
    float active;
    float recovery;
    int damage;
    int chipDamage;
    int ceCost;
    int ceGainOnHit;
    float range;
    float heightRange;
    float hitstun;
    float blockstun;
    float knockbackX;
    float knockbackY;
    float hitstop;
    bool isProjectile = false;
    bool isUnblockable = false;
    bool piercesInfinity = false;
    bool superArmor = false;
    std::vector<FighterAction> canChainInto;
};

struct FighterState {
    FighterId id;
    std::string name;
    std::string title;
    bool isPlayer = false;
    float x = 0;
    float y = 0;
    float vx = 0;
    float vy = 0;
    int facing = 1; // 1 or -1
    int hp = 100;
    int maxHp = 100;
    float ghostHp = 100.0f;
    float ce = 0.0f;
    float maxCe = 100.0f;
    FighterAction action = FighterAction::Idle;
    float actionTimer = 0.0f;
    bool actionHitApplied = false;
    float hitstunTimer = 0.0f;
    float invulnTimer = 0.0f;
    float blockWindowTimer = 0.0f;
    float parryWindowTimer = 0.0f;
    float infinityTimer = 0.0f;
    float buffTimer = 0.0f;
    float flashTimer = 0.0f;
    float parryFlashTimer = 0.0f;
    int comboCount = 0;
    float comboResetTimer = 0.0f;
    bool hasBufferedAction = false;
    FighterAction bufferedAction = FighterAction::Idle;
    float bufferTimer = 0.0f;
    float walkDist = 0.0f;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    int bossPhase = 1;
    bool superArmorActive = false;
    float stunLockedTimer = 0.0f;
    float aiCooldown = 0.0f;
    float aiStateTimer = 0.0f;
    std::string aiIntent = "approach";
};

struct Projectile {
    bool active = false;
    bool isPlayerOwner = true;
    ProjectileKind kind;
    float x = 0;
    float y = 0;
    float vx = 0;
    float vy = 0;
    float radius = 10.0f;
    int damage = 10;
    int chipDamage = 2;
    float knockbackX = 0;
    float knockbackY = 0;
    float life = 1.0f;
    float maxLife = 1.0f;
    float hitTickTimer = 0.0f;
    bool piercesInfinity = false;
    bool pullsEnemy = false;
    bool stunsTarget = false;
    bool unblockable = false;
    int hitsRemaining = 1;
};

struct GroundHazard {
    bool active = false;
    bool isPlayerOwner = true;
    HazardKind kind;
    float x = 0;
    float y = 0;
    float width = 30.0f;
    float telegraphTimer = 0.0f;
    float activeTimer = 0.0f;
    int damage = 10;
    float knockbackX = 0;
    float knockbackY = 0;
    bool hitApplied = false;
};

struct Particle {
    bool active = false;
    float x = 0;
    float y = 0;
    float vx = 0;
    float vy = 0;
    float size = 2.0f;
    Color color = WHITE;
    float life = 0.5f;
    float maxLife = 0.5f;
    float gravity = 0.0f;
    bool glow = false;
};

struct Afterimage {
    bool active = false;
    FighterId fighterId = FighterId::Gojo;
    FighterAction action = FighterAction::Idle;
    float x = 0;
    float y = 0;
    int facing = 1;
    float alpha = 0.7f;
    Color tint = WHITE;
};

struct FloatingCallout {
    bool active = false;
    std::string text;
    float x = 0;
    float y = 0;
    float vy = -25.0f;
    Color color = WHITE;
    float life = 0.8f;
    float maxLife = 0.8f;
};

struct InputState {
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
    bool block = false;
    bool justUp = false;
    bool justDown = false;
    bool justDash = false;
    bool justLight = false;
    bool justHeavy = false;
    bool justBlock = false;
    bool justParry = false;
    bool justSkill1 = false;
    bool justSkill2 = false;
    bool justSkill3 = false;
    bool justSkill4 = false;
    bool justSkill5 = false;
};

struct GameSettings {
    Difficulty difficulty = Difficulty::Special;
    float musicVolume = 0.5f;
    float sfxVolume = 0.7f;
    bool screenShake = true;
    bool practiceAutoCe = false;
    bool practiceAiActive = true;
};

struct HudSnapshot {
    GameScreenState screenState;
    GameMode mode;
    int stageIndex;
    int totalStages;
    ArenaId arenaId;
    std::string arenaName;
    int roundNumber;
    RoundPhase roundPhase;
    float roundTimer;
    std::string bannerText;
    std::string bannerSubtext;
    FighterId playerId;
    std::string playerName;
    int playerHp;
    int playerMaxHp;
    float playerGhostHp;
    float playerCe;
    float playerInfinity;
    float playerBuffTimer;
    int playerRounds;
    int playerCombo;
    float playerComboMultiplier;
    std::vector<SkillSlotMeta> playerSkills;
    FighterId enemyId;
    std::string enemyName;
    std::string enemyTitle;
    int enemyHp;
    int enemyMaxHp;
    float enemyGhostHp;
    float enemyCe;
    int enemyRounds;
    int enemyCombo;
    int enemyBossPhase;
    int score;
    int maxCombo;
    int damageDealt;
    float matchTime;
    DomainTheme domainActive;
    std::string domainBanner;
    float cameraZoom;
    int winner; // 0 = none, 1 = player, 2 = enemy
    bool storyComplete;
    std::string victoryQuote;
    std::string victoryQuoteTag;
};

// Helper conversion functions
std::string fighterIdToString(FighterId id);
FighterId stringToFighterId(const std::string& str);
std::string arenaIdToString(ArenaId id);
ArenaId stringToArenaId(const std::string& str);
Color hexToColor(const std::string& hex, unsigned char alpha = 255);

} // namespace CursedClash
