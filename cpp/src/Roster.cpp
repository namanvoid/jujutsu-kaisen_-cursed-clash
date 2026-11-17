#include "Roster.hpp"
#include <unordered_map>

namespace CursedClash {

std::string fighterIdToString(FighterId id) {
    switch (id) {
        case FighterId::Gojo: return "gojo";
        case FighterId::Yuta: return "yuta";
        case FighterId::Megumi: return "megumi";
        case FighterId::Panda: return "panda";
        case FighterId::Jogo: return "jogo";
        case FighterId::Mahito: return "mahito";
        case FighterId::Hanami: return "hanami";
        case FighterId::Choso: return "choso";
        case FighterId::Geto: return "geto";
        case FighterId::Kenjaku: return "kenjaku";
        case FighterId::Uraume: return "uraume";
        case FighterId::Kashimo: return "kashimo";
        case FighterId::Toji: return "toji";
        case FighterId::Sukuna: return "sukuna";
    }
    return "gojo";
}

FighterId stringToFighterId(const std::string& str) {
    if (str == "yuta") return FighterId::Yuta;
    if (str == "megumi") return FighterId::Megumi;
    if (str == "panda") return FighterId::Panda;
    if (str == "jogo") return FighterId::Jogo;
    if (str == "mahito") return FighterId::Mahito;
    if (str == "hanami") return FighterId::Hanami;
    if (str == "choso") return FighterId::Choso;
    if (str == "geto") return FighterId::Geto;
    if (str == "kenjaku") return FighterId::Kenjaku;
    if (str == "uraume") return FighterId::Uraume;
    if (str == "kashimo") return FighterId::Kashimo;
    if (str == "toji") return FighterId::Toji;
    if (str == "sukuna") return FighterId::Sukuna;
    return FighterId::Gojo;
}

std::string arenaIdToString(ArenaId id) {
    switch (id) {
        case ArenaId::JujutsuHigh: return "jujutsu_high";
        case ArenaId::AbandonedSchool: return "abandoned_school";
        case ArenaId::CursedForest: return "cursed_forest";
        case ArenaId::Shibuya: return "shibuya";
        case ArenaId::MalevolentShrine: return "malevolent_shrine";
    }
    return "jujutsu_high";
}

ArenaId stringToArenaId(const std::string& str) {
    if (str == "abandoned_school") return ArenaId::AbandonedSchool;
    if (str == "cursed_forest") return ArenaId::CursedForest;
    if (str == "shibuya") return ArenaId::Shibuya;
    if (str == "malevolent_shrine") return ArenaId::MalevolentShrine;
    return ArenaId::JujutsuHigh;
}

Color hexToColor(const std::string& hex, unsigned char alpha) {
    if (hex.empty() || hex[0] != '#') return WHITE;
    std::string s = hex.substr(1);
    if (s.length() == 3) {
        s = std::string() + s[0] + s[0] + s[1] + s[1] + s[2] + s[2];
    }
    if (s.length() != 6) return WHITE;
    unsigned int val = std::stoul(s, nullptr, 16);
    return Color{
        static_cast<unsigned char>((val >> 16) & 0xFF),
        static_cast<unsigned char>((val >> 8) & 0xFF),
        static_cast<unsigned char>(val & 0xFF),
        alpha
    };
}

static std::map<FighterId, CharacterProfile> profiles;
static std::map<ArenaId, ArenaProfile> arenas;
static std::map<FighterId, std::vector<SkillSlotMeta>> skills;
static std::map<FighterId, VictoryQuoteKit> victoryKits;
static std::vector<FighterId> sorcerers;
static std::vector<FighterId> villains;
static std::vector<FighterId> playables;
static std::vector<ArenaId> arenaList;
static std::vector<StoryStage> storyStages;
static bool initialized = false;

void Roster::init() {
    if (initialized) return;
    initialized = true;

    sorcerers = { FighterId::Gojo, FighterId::Yuta, FighterId::Megumi, FighterId::Panda };
    villains = {
        FighterId::Jogo, FighterId::Mahito, FighterId::Hanami, FighterId::Choso,
        FighterId::Geto, FighterId::Uraume, FighterId::Kashimo, FighterId::Toji,
        FighterId::Kenjaku, FighterId::Sukuna
    };
    playables = sorcerers;
    playables.insert(playables.end(), villains.begin(), villains.end());

    arenaList = {
        ArenaId::JujutsuHigh,
        ArenaId::AbandonedSchool,
        ArenaId::CursedForest,
        ArenaId::Shibuya,
        ArenaId::MalevolentShrine
    };

    // Profiles
    profiles[FighterId::Gojo] = {
        FighterId::Gojo, "Satoru Gojo", "The Strongest Jujutsu Sorcerer",
        300, 152.0f, -292.0f, hexToColor("#38bdf8"),
        "Limitless Cursed Technique, Infinity shield & Six Eyes control.",
        {"Lapse: Blue", "Reversal: Red", "Hollow Purple", "Infinity", "Unlimited Void"},
        "UNLIMITED VOID", DomainTheme::UnlimitedVoid,
        "Don't worry. I'm the strongest.",
        "Throughout Heaven and Earth, I alone am the honored one."
    };

    profiles[FighterId::Yuta] = {
        FighterId::Yuta, "Yuta Okkotsu", "Special Grade Prodigy",
        295, 158.0f, -294.0f, hexToColor("#ec4899"),
        "Katana mastery, Rika manifestation, Cursed Speech & RCT healing.",
        {"Rika Claw", "Cursed Speech", "Pure Love Beam", "RCT Heal", "Authentic Mutual Love"},
        "AUTHENTIC MUTUAL LOVE", DomainTheme::MutualLove,
        "Rika, lend me your strength—just for a moment!",
        "I will protect my friends, no matter the curse."
    };

    profiles[FighterId::Megumi] = {
        FighterId::Megumi, "Megumi Fushiguro", "Ten Shadows Tactician",
        285, 162.0f, -296.0f, hexToColor("#6366f1"),
        "Ten Shadows shikigami combos, agile Rabbit feints & shadow spires.",
        {"Divine Dogs", "Nue Lightning", "Max Elephant", "Rabbit Escape", "Chimera Shadow Garden"},
        "CHIMERA SHADOW GARDEN", DomainTheme::ChimeraShadow,
        "I will surpass my limits right here.",
        "With this treasure, the shadows answer."
    };

    profiles[FighterId::Panda] = {
        FighterId::Panda, "Panda", "Abrupt-Mutated Cursed Corpse",
        345, 136.0f, -274.0f, hexToColor("#f59e0b"),
        "Armored brawler with unblockable Drumming Beat & 3-Core transformations.",
        {"Drumming Beat", "Gorilla Smash", "Triceratops Charge", "Core Guard", "Three-Core Sanctuary"},
        "THREE-CORE SANCTUARY", DomainTheme::BeastSanctuary,
        "Panda is not a panda! Let's brawl!",
        "Three cores beat as one!"
    };

    profiles[FighterId::Jogo] = {
        FighterId::Jogo, "Jogo", "Disaster Curse of Flame",
        245, 130.0f, -260.0f, hexToColor("#f97316"),
        "Ranged fireballs, ember swarms, magma geysers & Iron Mountain domain.",
        {"Disaster Flame", "Volcanic Geyser", "Ember Swarm", "Magma Burst", "Coffin of Iron Mountain"},
        "COFFIN OF THE IRON MOUNTAIN", DomainTheme::IronMountain,
        "Ashes are all that will remain of you!",
        "True curses are born from human fear!"
    };

    profiles[FighterId::Mahito] = {
        FighterId::Mahito, "Mahito", "Patchwork Curse of Humanity",
        270, 152.0f, -278.0f, hexToColor("#a855f7"),
        "Unpredictable blade-arm pokes, transfigured spikes & Perfection domain.",
        {"Blade-Arm Extend", "Soul Spike", "Body Repel", "Polymorphic Rush", "Self-Embodiment of Perfection"},
        "SELF-EMBODIMENT OF PERFECTION", DomainTheme::Perfection,
        "Let me see the shape of your soul!",
        "Humans and sorcerers break just the same!"
    };

    profiles[FighterId::Hanami] = {
        FighterId::Hanami, "Hanami", "Disaster Curse of the Forest",
        340, 106.0f, -242.0f, hexToColor("#22c55e"),
        "Super-armor heavy slams, cursed roots, flower beam & forest domain.",
        {"Cursed Roots", "Wood Slam", "Flower Beam", "Seed Trap", "Shining Sea of Branches"},
        "SHINING SEA OF BRANCHES", DomainTheme::ForestBranches,
        "The earth cries out for your silence.",
        "Return to the soil."
    };

    profiles[FighterId::Choso] = {
        FighterId::Choso, "Choso", "Death Painting Womb",
        280, 140.0f, -268.0f, hexToColor("#ef4444"),
        "Piercing Blood lasers, Supernova orbs, Crimson Scale & Blood Eclipse.",
        {"Piercing Blood", "Supernova Orbs", "Crimson Edge", "Flowing Red Scale", "Blood Eclipse"},
        "BLOOD ECLIPSE", DomainTheme::BloodEclipse,
        "For my brothers, I will pierce right through you.",
        "A big brother never falters."
    };

    profiles[FighterId::Geto] = {
        FighterId::Geto, "Suguru Geto", "Master of Cursed Spirit Manipulation",
        300, 148.0f, -282.0f, hexToColor("#8b5cf6"),
        "Playful Cloud combos, centipede swarms & Maximum: Uzumaki vortex.",
        {"Spirit Swarm", "Playful Cloud", "Squid Barrier", "Centipede Surge", "Maximum: Uzumaki"},
        "WOMB PROFUSION", DomainTheme::WombProfusion,
        "Only sorcerers deserve to shape this world.",
        "Another curse added to my arsenal."
    };

    profiles[FighterId::Uraume] = {
        FighterId::Uraume, "Uraume", "The Frozen Star",
        285, 146.0f, -275.0f, hexToColor("#67e8f9"),
        "Ice Formation Frost Calm waves, icicle pillars & freezing Blizzard domain.",
        {"Frost Calm", "Icefall Pillars", "Frozen Lance", "Mist Guard", "Absolute Zero"},
        "ABSOLUTE ZERO SANCTUARY", DomainTheme::AbsoluteZero,
        "Prepare to shatter before Lord Sukuna's blade.",
        "Cold silence is your reward."
    };

    profiles[FighterId::Kashimo] = {
        FighterId::Kashimo, "Hajime Kashimo", "God of Lightning",
        305, 172.0f, -292.0f, hexToColor("#38bdf8"),
        "Electrified staff rushdown, sure-hit lightning bolts & Thunderclap domain.",
        {"Lightning Discharge", "Staff Blitz", "Thunder Dive", "Amber Surge", "Thunderclap Arena"},
        "THUNDERCLAP ARENA", DomainTheme::Thunderclap,
        "Turn up the volume! This is a funeral for the living!",
        "Show me someone stronger!"
    };

    profiles[FighterId::Toji] = {
        FighterId::Toji, "Toji Fushiguro", "The Sorcerer Killer",
        305, 180.0f, -298.0f, hexToColor("#94a3b8"),
        "Blistering Heavenly Restriction speed & Inverted Spear piercing Infinity.",
        {"Inverted Spear Blitz", "Chain Whirlwind", "Playful Cloud Smash", "Shadow Step", "Heavenly Zero"},
        "HEAVENLY RESTRICTION ZERO", DomainTheme::HeavenlyZero,
        "You got sloppy relying on cursed techniques.",
        "Zero cursed energy beats the elite every time."
    };

    profiles[FighterId::Kenjaku] = {
        FighterId::Kenjaku, "Kenjaku", "Thousand-Year Curse Mastermind",
        335, 154.0f, -282.0f, hexToColor("#c084fc"),
        "Anti-Gravity System crushes, Mini-Uzumaki & Open Domain Womb Profusion.",
        {"Mini-Uzumaki", "Anti-Gravity Crush", "Spirit Volley", "Barrier Counter", "Womb Profusion"},
        "WOMB PROFUSION", DomainTheme::WombProfusion,
        "A thousand years of evolution converges right now.",
        "How delightfully predictable."
    };

    profiles[FighterId::Sukuna] = {
        FighterId::Sukuna, "Ryomen Sukuna", "The King of Curses",
        460, 164.0f, -286.0f, hexToColor("#dc2626"),
        "3-Phase Final Boss: Dismantle, Cleave, Divine Flame & Malevolent Shrine.",
        {"Dismantle", "Cleave", "Divine Flame (Fuga)", "World-Cutting Slash", "Malevolent Shrine"},
        "MALEVOLENT SHRINE", DomainTheme::MalevolentShrineTheme,
        "Know your place, fool. Show me what you can do.",
        "Stand proud. You were strong."
    };

    // Arenas
    arenas[ArenaId::JujutsuHigh] = {
        ArenaId::JujutsuHigh, "Jujutsu High", "Tokyo Metropolitan Courtyard",
        hexToColor("#161b33"), hexToColor("#473154"), hexToColor("#f59e0b"), "jujutsu"
    };
    arenas[ArenaId::AbandonedSchool] = {
        ArenaId::AbandonedSchool, "Abandoned School", "Cursed Corridor Ruins",
        hexToColor("#0b101b"), hexToColor("#1c263b"), hexToColor("#a855f7"), "school"
    };
    arenas[ArenaId::CursedForest] = {
        ArenaId::CursedForest, "Cursed Forest", "Kyoto Goodwill Woods",
        hexToColor("#071411"), hexToColor("#122b23"), hexToColor("#22c55e"), "forest"
    };
    arenas[ArenaId::Shibuya] = {
        ArenaId::Shibuya, "Shibuya Station", "October 31 Midnight Crossing",
        hexToColor("#070914"), hexToColor("#171a32"), hexToColor("#ec4899"), "shibuya"
    };
    arenas[ArenaId::MalevolentShrine] = {
        ArenaId::MalevolentShrine, "Malevolent Shrine", "Innate Domain of the King",
        hexToColor("#160406"), hexToColor("#3c080e"), hexToColor("#ef4444"), "shrine"
    };

    // Story stages
    storyStages = {
        {1, FighterId::Jogo, ArenaId::JujutsuHigh, "Stage 1 — Volcanic ambush outside Tokyo Jujutsu High."},
        {2, FighterId::Mahito, ArenaId::AbandonedSchool, "Stage 2 — Idle Transfiguration echoes through the ruined school."},
        {3, FighterId::Hanami, ArenaId::CursedForest, "Stage 3 — Ancient cursed roots awaken in the forest depths."},
        {4, FighterId::Choso, ArenaId::Shibuya, "Stage 4 — Blood Manipulation clashes beneath Shibuya neon."},
        {5, FighterId::Geto, ArenaId::JujutsuHigh, "Stage 5 — Suguru Geto unleashes Cursed Spirit Manipulation."},
        {6, FighterId::Uraume, ArenaId::AbandonedSchool, "Stage 6 — Uraume freezes the night with Absolute Zero."},
        {7, FighterId::Kashimo, ArenaId::Shibuya, "Stage 7 — The God of Lightning challenges you in Shibuya."},
        {8, FighterId::Toji, ArenaId::CursedForest, "Stage 8 — The Sorcerer Killer strikes with Inverted Spear of Heaven."},
        {9, FighterId::Kenjaku, ArenaId::Shibuya, "Stage 9 — Kenjaku opens Womb Profusion in the Shibuya ruins."},
        {10, FighterId::Sukuna, ArenaId::MalevolentShrine, "Final Battle — The King of Curses awaits inside Malevolent Shrine."}
    };

    // Skills
    skills[FighterId::Gojo] = {
        {1, FighterAction::Skill1, "U", "Lapse: Blue", "BLUE", 25, hexToColor("#0ea5e9"), "Gravity orb pulling enemies inward"},
        {2, FighterAction::Skill2, "I", "Reversal: Red", "RED", 35, hexToColor("#ef4444"), "High-knockback repulsion blast"},
        {3, FighterAction::Skill3, "O", "Hollow Purple", "PURPLE", 65, hexToColor("#a855f7"), "Massive void annihilation sphere"},
        {4, FighterAction::Skill4, "H", "Infinity", "INFINITY", 30, hexToColor("#22d3ee"), "Spatial shield deflecting normal hits"},
        {5, FighterAction::Skill5Domain, "Y", "Unlimited Void", "VOID", 100, hexToColor("#38bdf8"), "Domain Expansion: Absolute stun & collapse"}
    };

    skills[FighterId::Yuta] = {
        {1, FighterAction::Skill1, "U", "Rika Claw Strike", "RIKA CLAW", 25, hexToColor("#ec4899"), "Manifested Rika crushing slash"},
        {2, FighterAction::Skill2, "I", "Cursed Speech", "DONT MOVE", 35, hexToColor("#c084fc"), "Sonic command stunning the target"},
        {3, FighterAction::Skill3, "O", "Pure Love Beam", "LOVE BEAM", 65, hexToColor("#f43f5e"), "Colossal cursed beam blast"},
        {4, FighterAction::Skill4, "H", "RCT Healing", "HEAL", 30, hexToColor("#34d399"), "Reverse Cursed Technique recovers 48 HP"},
        {5, FighterAction::Skill5Domain, "Y", "Authentic Mutual Love", "MUTUAL LOVE", 100, hexToColor("#ec4899"), "Domain Expansion: Katana storm & Rika barrage"}
    };

    skills[FighterId::Megumi] = {
        {1, FighterAction::Skill1, "U", "Divine Dog Lunge", "DIVINE DOG", 25, hexToColor("#38bdf8"), "Twin shadow hounds rend the target"},
        {2, FighterAction::Skill2, "I", "Nue Thunder", "NUE", 35, hexToColor("#facc15"), "Lightning swoop shocking opponent"},
        {3, FighterAction::Skill3, "O", "Max Elephant", "ELEPHANT", 60, hexToColor("#3b82f6"), "Pressurized torrent crushing opponent"},
        {4, FighterAction::Skill4, "H", "Rabbit Escape", "RABBITS", 25, hexToColor("#cbd5e1"), "Shadow decoy dash with brief invulnerability"},
        {5, FighterAction::Skill5Domain, "Y", "Chimera Shadow Garden", "SHADOWS", 100, hexToColor("#818cf8"), "Domain Expansion: Shadow spire barrage"}
    };

    skills[FighterId::Panda] = {
        {1, FighterAction::Skill1, "U", "Drumming Beat", "DRUM BEAT", 25, hexToColor("#f59e0b"), "Unblockable internal shockwave palm"},
        {2, FighterAction::Skill2, "I", "Gorilla Smash", "GORILLA", 35, hexToColor("#ef4444"), "Armored heavy overhead seismic smash"},
        {3, FighterAction::Skill3, "O", "Triceratops Charge", "TRICERA", 60, hexToColor("#fb923c"), "Unstoppable horned charge across arena"},
        {4, FighterAction::Skill4, "H", "Cursed Core Guard", "CORE GUARD", 25, hexToColor("#a3e635"), "Steel-fur buff reducing damage & boosting hits"},
        {5, FighterAction::Skill5Domain, "Y", "Three-Core Sanctuary", "3-CORE DOM", 100, hexToColor("#fbbf24"), "Domain Expansion: All three cores erupt"}
    };

    skills[FighterId::Jogo] = {
        {1, FighterAction::Skill1, "U", "Disaster Flame", "FLAME", 25, hexToColor("#f97316"), "High-heat cursed fireball projectile"},
        {2, FighterAction::Skill2, "I", "Volcanic Geyser", "GEYSER", 35, hexToColor("#fb923c"), "Lava pillar erupting beneath opponent"},
        {3, FighterAction::Skill3, "O", "Ember Swarm", "SWARM", 60, hexToColor("#ef4444"), "Volley of 3 blazing ember projectiles"},
        {4, FighterAction::Skill4, "H", "Magma Burst", "MAGMA", 30, hexToColor("#fbbf24"), "Point-blank volcanic ring with brief invuln"},
        {5, FighterAction::Skill5Domain, "Y", "Coffin of Iron Mountain", "IRON MTN", 100, hexToColor("#ea580c"), "Domain Expansion: Volcanic eruption barrage"}
    };

    skills[FighterId::Mahito] = {
        {1, FighterAction::Skill1, "U", "Blade-Arm Extend", "BLADE ARM", 25, hexToColor("#a855f7"), "Long-reach transfigured limb slash"},
        {2, FighterAction::Skill2, "I", "Soul Spike", "SPIKE", 35, hexToColor("#c084fc"), "Transfigured spike erupting from floor"},
        {3, FighterAction::Skill3, "O", "Body Repel", "BODY REPEL", 60, hexToColor("#d946ef"), "Colossal soul-warped energy battering wave"},
        {4, FighterAction::Skill4, "H", "Polymorphic Shift", "SHIFT", 25, hexToColor("#e879f9"), "Evasive soul-morph dash with invulnerability"},
        {5, FighterAction::Skill5Domain, "Y", "Self-Embodiment of Perfection", "PERFECTION", 100, hexToColor("#c084fc"), "Domain Expansion: Soul lock & spike eruption"}
    };

    skills[FighterId::Hanami] = {
        {1, FighterAction::Skill1, "U", "Cursed Roots", "ROOTS", 25, hexToColor("#22c55e"), "Armored cursed roots erupting at target"},
        {2, FighterAction::Skill2, "I", "Wood Slam", "WOOD SLAM", 35, hexToColor("#16a34a"), "Super-armored forest earthshaker slam"},
        {3, FighterAction::Skill3, "O", "Cursed Flower Beam", "FLOWER BM", 60, hexToColor("#86efac"), "Solar-charged cursed energy forest beam"},
        {4, FighterAction::Skill4, "H", "Bark Armor & Seeds", "BARK ARMOR", 25, hexToColor("#4ade80"), "Hardens bark (+15% dmg, -35% incoming dmg)"},
        {5, FighterAction::Skill5Domain, "Y", "Shining Sea of Branches", "BRANCHES", 100, hexToColor("#22c55e"), "Domain Expansion: Solar roots & flower storm"}
    };

    skills[FighterId::Choso] = {
        {1, FighterAction::Skill1, "U", "Piercing Blood", "PIERCING", 25, hexToColor("#ef4444"), "Supersonic pressurized crimson blood beam"},
        {2, FighterAction::Skill2, "I", "Supernova Orbs", "SUPERNOVA", 35, hexToColor("#f87171"), "Twin bursting blood spheres covering arcs"},
        {3, FighterAction::Skill3, "O", "Crimson Edge", "CRIMSON", 60, hexToColor("#dc2626"), "Triple Piercing Blood meteor salvo"},
        {4, FighterAction::Skill4, "H", "Flowing Red Scale", "RED SCALE", 30, hexToColor("#fb7185"), "Blood doping surge boosting attack power"},
        {5, FighterAction::Skill5Domain, "Y", "Blood Eclipse", "ECLIPSE", 100, hexToColor("#dc2626"), "Domain Expansion: Crimson vortex barrage"}
    };

    skills[FighterId::Geto] = {
        {1, FighterAction::Skill1, "U", "Cursed Spirit Swarm", "SWARM", 25, hexToColor("#8b5cf6"), "Releases flying cursed spirit orb wave"},
        {2, FighterAction::Skill2, "I", "Playful Cloud", "PLAYFUL C", 35, hexToColor("#a78bfa"), "Three-section staff heavy combo strike"},
        {3, FighterAction::Skill3, "O", "Centipede Surge", "CENTIPEDE", 60, hexToColor("#7c3aed"), "Twin special-grade spirit torrent"},
        {4, FighterAction::Skill4, "H", "Squid Barrier", "BARRIER", 25, hexToColor("#c4b5fd"), "Summons cursed shield for brief invuln"},
        {5, FighterAction::Skill5Domain, "Y", "Maximum: Uzumaki", "UZUMAKI", 100, hexToColor("#a855f7"), "Domain Expansion: Combined spirit spiral"}
    };

    skills[FighterId::Uraume] = {
        {1, FighterAction::Skill1, "U", "Frost Calm", "FROST CALM", 25, hexToColor("#67e8f9"), "Freezing mist wave chilling the target"},
        {2, FighterAction::Skill2, "I", "Icefall Pillars", "ICEFALL", 35, hexToColor("#38bdf8"), "Jagged icicle spire erupting underfoot"},
        {3, FighterAction::Skill3, "O", "Frozen Lance", "ICE LANCE", 60, hexToColor("#bae6fd"), "Piercing glacier spear breaking guards"},
        {4, FighterAction::Skill4, "H", "Mist Guard", "MIST GUARD", 25, hexToColor("#a5f3fc"), "Sub-zero frost veil granting brief invuln"},
        {5, FighterAction::Skill5Domain, "Y", "Absolute Zero Sanctuary", "ABS ZERO", 100, hexToColor("#22d3ee"), "Domain Expansion: Deep freeze & ice pillars"}
    };

    skills[FighterId::Kashimo] = {
        {1, FighterAction::Skill1, "U", "Lightning Discharge", "DISCHARGE", 25, hexToColor("#38bdf8"), "Sure-hit electric charge bolt"},
        {2, FighterAction::Skill2, "I", "Staff Blitz", "STAFF BLZ", 35, hexToColor("#22d3ee"), "High-speed electrified Nyoi staff lunge"},
        {3, FighterAction::Skill3, "O", "Sky Thunder Dive", "THUNDER", 60, hexToColor("#facc15"), "Calls down sky lightning bolt at target"},
        {4, FighterAction::Skill4, "H", "Mythical Beast Amber", "AMBER", 30, hexToColor("#38bdf8"), "Overcharges cursed electricity (+15% dmg)"},
        {5, FighterAction::Skill5Domain, "Y", "Thunderclap Arena", "THUNDERCLP", 100, hexToColor("#22d3ee"), "Domain Expansion: Relentless sky bolts"}
    };

    skills[FighterId::Toji] = {
        {1, FighterAction::Skill1, "U", "Inverted Spear Blitz", "INV SPEAR", 25, hexToColor("#94a3b8"), "Pierces Infinity & lunges through defenses"},
        {2, FighterAction::Skill2, "I", "Chain Whirlwind", "CHAIN WHRL", 35, hexToColor("#cbd5e1"), "Thousand-Mile Chain sweeping slash"},
        {3, FighterAction::Skill3, "O", "Playful Cloud Smash", "CLOUD SMSH", 60, hexToColor("#f8fafc"), "Armored cursed-tool finisher piercing Infinity"},
        {4, FighterAction::Skill4, "H", "Shadow Step", "SHADOW STP", 25, hexToColor("#64748b"), "Zero-CE flash step behind enemy guard"},
        {5, FighterAction::Skill5Domain, "Y", "Heavenly Restriction Zero", "HEAVEN ZRO", 100, hexToColor("#e2e8f0"), "Ultimate Rush: Pierces Infinity & barriers"}
    };

    skills[FighterId::Kenjaku] = {
        {1, FighterAction::Skill1, "U", "Mini-Uzumaki", "MINI-UZU", 25, hexToColor("#c084fc"), "Compact condensed cursed spiral blast"},
        {2, FighterAction::Skill2, "I", "Anti-Gravity Crush", "ANTI-GRAV", 35, hexToColor("#a855f7"), "Crushes target with intense gravity well"},
        {3, FighterAction::Skill3, "O", "Special Grade Volley", "VOLLEY", 60, hexToColor("#d8b4fe"), "Unleashes multi-spirit + Uzumaki barrage"},
        {4, FighterAction::Skill4, "H", "Barrier Counter", "BARRIER", 30, hexToColor("#818cf8"), "Ancient barrier ward granting invuln & buff"},
        {5, FighterAction::Skill5Domain, "Y", "Womb Profusion", "WOMB PROF", 100, hexToColor("#c084fc"), "Open Domain: Gravity pillars & spirit spire"}
    };

    skills[FighterId::Sukuna] = {
        {1, FighterAction::Skill1, "U", "Dismantle", "DISMANTLE", 25, hexToColor("#ef4444"), "Twin flying invisible slash waves"},
        {2, FighterAction::Skill2, "I", "Cleave", "CLEAVE", 35, hexToColor("#dc2626"), "Devastating point-blank adaptive slash"},
        {3, FighterAction::Skill3, "O", "Divine Flame (Fuga)", "FUGA FLAME", 65, hexToColor("#f97316"), "Incinerating cursed fire arrow explosion"},
        {4, FighterAction::Skill4, "H", "World-Cutting Slash", "WORLD SLSH", 45, hexToColor("#fca5a5"), "Unblockable slash piercing Infinity"},
        {5, FighterAction::Skill5Domain, "Y", "Malevolent Shrine", "SHRINE", 100, hexToColor("#dc2626"), "Open Domain: Relentless Cleave & Dismantle"}
    };

    // Victory quote kits
    victoryKits[FighterId::Gojo] = {
        profiles[FighterId::Gojo].victoryQuote,
        "Inside Unlimited Void, all information flows infinitely into nothingness.",
        "You couldn't even scratch my Infinity. Not even once.",
        {
            {FighterId::Sukuna, "You were magnificent, Ryomen Sukuna. I will remember you."},
            {FighterId::Jogo, "You did great, Jogo! Too bad you fought me."},
            {FighterId::Toji, "You should have aimed for the head, Toji."}
        }
    };

    victoryKits[FighterId::Sukuna] = {
        profiles[FighterId::Sukuna].victoryQuote,
        "Inside Malevolent Shrine, nothing escapes Cleave and Dismantle.",
        "Is that really all? Know your place, fool.",
        {
            {FighterId::Gojo, "I will never forget you for as long as I live, Satoru Gojo."},
            {FighterId::Jogo, "Stand proud, Jogo. You were strong."},
            {FighterId::Yuta, "Not bad, brat—your resolve almost entertained me."}
        }
    };
}

const CharacterProfile& Roster::getProfile(FighterId id) {
    init();
    return profiles[id];
}

const ArenaProfile& Roster::getArena(ArenaId id) {
    init();
    return arenas[id];
}

const std::vector<SkillSlotMeta>& Roster::getSkills(FighterId id) {
    init();
    return skills[id];
}

const VictoryQuoteKit& Roster::getVictoryKit(FighterId id) {
    init();
    return victoryKits[id];
}

const std::vector<FighterId>& Roster::getPlayableList() {
    init();
    return playables;
}

const std::vector<FighterId>& Roster::getSorcererList() {
    init();
    return sorcerers;
}

const std::vector<FighterId>& Roster::getVillainList() {
    init();
    return villains;
}

const std::vector<ArenaId>& Roster::getArenaList() {
    init();
    return arenaList;
}

const std::vector<StoryStage>& Roster::getStoryStages() {
    init();
    return storyStages;
}

std::pair<std::string, std::string> Roster::selectVictoryQuote(
    FighterId winnerId,
    FighterId loserId,
    bool wasDomainFinish,
    bool wasFlawless
) {
    init();
    const auto& kit = getVictoryKit(winnerId);
    if (wasFlawless && !kit.flawlessWinQuote.empty()) {
        return {kit.flawlessWinQuote, "FLAWLESS VICTORY"};
    }
    if (wasDomainFinish && !kit.domainFinishQuote.empty()) {
        return {kit.domainFinishQuote, "DOMAIN FINISH"};
    }
    auto it = kit.vsRivals.find(loserId);
    if (it != kit.vsRivals.end()) {
        return {it->second, "RIVALRY CLASH"};
    }
    if (!kit.defaultQuote.empty()) {
        return {kit.defaultQuote, "VICTORY QUOTE"};
    }
    return {getProfile(winnerId).victoryQuote, "VICTORY"};
}

} // namespace CursedClash
