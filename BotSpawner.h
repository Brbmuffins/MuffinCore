#ifndef _BOT_SPAWNER_H
#define _BOT_SPAWNER_H
#include "BotAI.h"

#include "Player.h"
#include "Creature.h"
#include <string>
#include <map>
#include <vector>

namespace BotAI {

enum class BotClass {
    WARRIOR = 1,
    PALADIN = 2,
    HUNTER = 3,
    ROGUE = 4,
    PRIEST = 5,
    DEATH_KNIGHT = 6,
    SHAMAN = 7,
    MAGE = 8,
    WARLOCK = 9,
    DRUID = 11
};


struct BotProfile {
    std::string name;
    BotAI::BotClass botClass;
    uint8 race;
    uint8 gender;
    BotAI::BotRole role;
    std::string personality;
};

struct BotProfileExtended {
    std::string name;
    BotAI::BotClass botClass;
    uint8 race;
    uint8 gender;
    BotAI::BotRole role;
    std::string personality;
    uint8 faction;
};

class BotSpawner {
public:
    static BotSpawner* instance();
    
    Creature* SpawnBot(Player* owner, const std::string& name, BotRole role);
    Creature* SpawnBot(Player* owner, BotProfile profile);
    
    bool RemoveBot(Player* owner, const std::string& name);
    void RemoveAllBots(Player* owner);
    std::vector<Creature*> GetPlayerBots(Player* owner);
    
    void FillGroup(Player* owner, uint8 groupSize = 5);
    void FillRaid(Player* owner, uint8 raidSize = 25);
    
    std::string GetRandomBotName(BotRole role);
    BotProfile GetBotProfile(const std::string& name);
    
private:
    BotSpawner();
    void InitializeProfiles();
    
    Creature* CreateBotPlayer(Player* owner, BotProfile profile);
    void EquipBot(Creature* bot, BotRole role, uint8 level);
    void SetBotStats(Creature* bot, BotRole role);
    
    std::map<std::string, BotProfile> profiles_;
    std::map<uint64, std::vector<uint64>> ownerBots_;
    
    static BotSpawner* instance_;
};

} // namespace BotAI

#endif
