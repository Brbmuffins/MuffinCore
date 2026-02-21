#ifndef _REAL_PLAYER_BOT_FACTORY_H
#define _REAL_PLAYER_BOT_FACTORY_H

#include "Player.h"
#include "ObjectMgr.h"
#include "World.h"
#include "BotSpawner.h"

namespace BotAI {

class RealPlayerBotFactory {
public:
    static RealPlayerBotFactory* instance();
    
    Player* CreateRealBot(Player* owner, BotProfile profile);
    
private:
    RealPlayerBotFactory() = default;
    static RealPlayerBotFactory* instance_;
    
    Player* CreatePlayerObject(Player* owner, BotProfile profile);
    void SetupRaceAndClass(Player* bot, BotProfile profile);
    void LearnClassSpells(Player* bot, uint8 level);
    void EquipGear(Player* bot, BotRole role, uint8 level);
    void SetupStats(Player* bot, BotRole role);
    void SetupTalents(Player* bot, BotRole role);
    void AddToWorld(Player* bot, Player* owner);
    
    ObjectGuid GenerateBotGuid();
    uint32 GetNextBotId();
    
    void LearnWarriorSpells(Player* bot, BotRole role);
    void LearnPaladinSpells(Player* bot, BotRole role);
    void LearnPriestSpells(Player* bot, BotRole role);
    void LearnMageSpells(Player* bot, BotRole role);
    void LearnWarlockSpells(Player* bot, BotRole role);
    void LearnRogueSpells(Player* bot, BotRole role);
    void LearnHunterSpells(Player* bot, BotRole role);
    void LearnDruidSpells(Player* bot, BotRole role);
    void LearnShamanSpells(Player* bot, BotRole role);
    void LearnDeathKnightSpells(Player* bot, BotRole role);
    
    void EquipTankGear(Player* bot, uint8 level);
    void EquipHealerGear(Player* bot, uint8 level);
    void EquipDpsGear(Player* bot, uint8 level);
    
    uint32 nextBotId_ = 1000000;
};

} // namespace BotAI

#endif
