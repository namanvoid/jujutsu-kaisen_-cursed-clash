#pragma once

#include "Types.hpp"
#include <map>
#include <string>
#include <vector>

namespace CursedClash {

struct CharacterProfile {
    FighterId id;
    std::string name;
    std::string title;
    int maxHp;
    float speed;
    float jumpForce;
    Color accentColor;
    std::string styleSummary;
    std::vector<std::string> signatureMoves;
    std::string domainName;
    DomainTheme domainTheme;
    std::string introQuote;
    std::string victoryQuote;
};

struct ArenaProfile {
    ArenaId id;
    std::string name;
    std::string subtitle;
    Color skyTop;
    Color skyBottom;
    Color accentColor;
    std::string musicTheme;
};

struct StoryStage {
    int stageNumber;
    FighterId opponentId;
    ArenaId arenaId;
    std::string preFightLine;
};

struct VictoryQuoteKit {
    std::string defaultQuote;
    std::string domainFinishQuote;
    std::string flawlessWinQuote;
    std::map<FighterId, std::string> vsRivals;
};

class Roster {
public:
    static void init();
    static const CharacterProfile& getProfile(FighterId id);
    static const ArenaProfile& getArena(ArenaId id);
    static const std::vector<SkillSlotMeta>& getSkills(FighterId id);
    static const VictoryQuoteKit& getVictoryKit(FighterId id);
    static const std::vector<FighterId>& getPlayableList();
    static const std::vector<FighterId>& getSorcererList();
    static const std::vector<FighterId>& getVillainList();
    static const std::vector<ArenaId>& getArenaList();
    static const std::vector<StoryStage>& getStoryStages();

    static std::pair<std::string, std::string> selectVictoryQuote(
        FighterId winnerId,
        FighterId loserId,
        bool wasDomainFinish,
        bool wasFlawless
    );
};

} // namespace CursedClash
