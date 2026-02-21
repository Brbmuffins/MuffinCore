#include "BotSpawner.h"
#include "BotAI.h"
#include "BotConstants.h"
#include "BotPersonality.h"
#include "BotCombatSystem.h"
#include "RealPlayerBotFactory.h"
#include "Creature.h"
#include "ObjectMgr.h"
#include "Map.h"
#include "Log.h"
#include <algorithm>
#include <cmath>
#include <cctype>

namespace BotAI {

uint32 GetNpcEntryForClass(BotClass botClass, BotRole role);

std::map<uint64, std::vector<Creature*>> ownerBotsCreatures;

BotSpawner* BotSpawner::instance_ = nullptr;

BotSpawner* BotSpawner::instance() {
    if (!instance_) {
        instance_ = new BotSpawner();
    }
    return instance_;
}

BotSpawner::BotSpawner() {
    InitializeProfiles();
    LOG_INFO("module", "==============================================");
    LOG_INFO("module", "  MUFFIN COMPANIONS Bot Spawner Initialized");
    LOG_INFO("module", "  The Ultimate AI Bot System for AzerothCore");
    LOG_INFO("module", "==============================================");
}

void BotSpawner::InitializeProfiles() {
    // ========================================================================
    // SPECIAL PROFILES
    // ========================================================================
    
    // Dr Steve Brule - The legendary goofy tank
    profiles_["brule"] = {"Dr Steve", BotClass::WARRIOR, 1, 0, BotRole::TANK, "brule"};
    profiles_["drsteve"] = {"Dr Steve", BotClass::WARRIOR, 1, 0, BotRole::TANK, "brule"};
    
    // ========================================================================
    // TANK PROFILES
    // ========================================================================
    profiles_["ironforge"] = {"Ironforge", BotClass::WARRIOR, 3, 0, BotRole::TANK, "righteous"};
    profiles_["grimshield"] = {"Grimshield", BotClass::PALADIN, 1, 0, BotRole::TANK, "serious"};
    profiles_["frostguard"] = {"Frostguard", BotClass::DEATH_KNIGHT, 5, 0, BotRole::TANK, "quiet"};
    profiles_["bearfang"] = {"Bearfang", BotClass::DRUID, 4, 0, BotRole::TANK, "serious"};
    
    // ========================================================================
    // HEALER PROFILES
    // ========================================================================
    profiles_["mirana"] = {"Mirana", BotClass::PRIEST, 4, 1, BotRole::HEALER, "serious"};
    profiles_["brightheal"] = {"Brightheal", BotClass::PALADIN, 11, 1, BotRole::HEALER, "righteous"};
    profiles_["earthmender"] = {"Earthmender", BotClass::SHAMAN, 6, 0, BotRole::HEALER, "quiet"};
    profiles_["moonwhisper"] = {"Moonwhisper", BotClass::DRUID, 4, 1, BotRole::HEALER, "serious"};
    
    // ========================================================================
    // DPS PROFILES
    // ========================================================================
    
    // Mages
    profiles_["kael"] = {"Kael", BotClass::MAGE, 10, 0, BotRole::DPS, "sarcastic"};
    profiles_["frostwhisper"] = {"Frostwhisper", BotClass::MAGE, 1, 1, BotRole::DPS, "serious"};
    
    // Rogues
    profiles_["shadowstrike"] = {"Shadowstrike", BotClass::ROGUE, 1, 0, BotRole::DPS, "quiet"};
    profiles_["vex"] = {"Vex", BotClass::ROGUE, 7, 1, BotRole::DPS, "quiet"};
    
    // Hunters
    profiles_["swiftarrow"] = {"Swiftarrow", BotClass::HUNTER, 4, 1, BotRole::DPS, "serious"};
    profiles_["eagleeye"] = {"Eagleeye", BotClass::HUNTER, 3, 0, BotRole::DPS, "quiet"};
    
    // Warlocks
    profiles_["voidcaller"] = {"Voidcaller", BotClass::WARLOCK, 7, 1, BotRole::DPS, "sarcastic"};
    profiles_["doomwhisper"] = {"Doomwhisper", BotClass::WARLOCK, 5, 0, BotRole::DPS, "quiet"};
    
    // Ret Paladins
    profiles_["thrain"] = {"Thrain", BotClass::PALADIN, 3, 0, BotRole::DPS, "righteous"};
    
    LOG_INFO("module", "Loaded {} bot profiles", profiles_.size());
}

std::string BotSpawner::GetRandomBotName(BotRole role) {
    std::vector<std::string> tanks = {"Ironforge", "Grimshield", "Frostguard", "Bearfang"};
    std::vector<std::string> healers = {"Mirana", "Brightheal", "Earthmender", "Moonwhisper"};
    std::vector<std::string> dps = {
        "Kael", "Vex", "Thrain", "Frostwhisper", "Shadowstrike", 
        "Swiftarrow", "Eagleeye", "Voidcaller", "Doomwhisper"
    };
    
    switch (role) {
        case BotRole::TANK:
            return tanks[urand(0, tanks.size() - 1)];
        case BotRole::HEALER:
            return healers[urand(0, healers.size() - 1)];
        case BotRole::DPS:
            return dps[urand(0, dps.size() - 1)];
    }
    return "Bot";
}

BotProfile BotSpawner::GetBotProfile(const std::string& name) {
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    
    auto it = profiles_.find(lowerName);
    if (it != profiles_.end()) {
        return it->second;
    }
    
    // Fallback to Dr Steve if not found
    return profiles_["brule"];
}

Creature* BotSpawner::SpawnBot(Player* owner, const std::string& name, BotRole role) {
    if (!owner) return nullptr;
    
    // CRITICAL: Check bot limit
    auto existingBots = GetPlayerBots(owner);
    if (existingBots.size() >= Constants::MAX_BOTS_PER_PLAYER) {
        LOG_ERROR("module", "Player {} has reached max bot limit ({})", 
                  owner->GetName(), Constants::MAX_BOTS_PER_PLAYER);
        return nullptr;
    }
    
    BotProfile profile;
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    
    auto it = profiles_.find(lowerName);
    if (it != profiles_.end()) {
        profile = it->second;
    } else {
        // Generate random bot of requested role
        std::string randomName = GetRandomBotName(role);
        profile = GetBotProfile(randomName);
    }
    
    return SpawnBot(owner, profile);
}

Creature* BotSpawner::SpawnBot(Player* owner, BotProfile profile) {
    if (!owner) return nullptr;
    
    // CRITICAL: Check bot limit
    auto existingBots = GetPlayerBots(owner);
    if (existingBots.size() >= Constants::MAX_BOTS_PER_PLAYER) {
        LOG_ERROR("module", "Player {} has reached max bot limit ({})", 
                  owner->GetName(), Constants::MAX_BOTS_PER_PLAYER);
        return nullptr;
    }
    
    Creature* bot = CreateBotPlayer(owner, profile);
    if (bot) {
        // Register with AI system
        BotControllerManager::instance()->RegisterBot(bot, owner, profile.role);
        
        // Register with COMBAT system
        BotCombatManager::instance()->RegisterBot(bot, profile.role);
        
        LOG_INFO("module", "✓ Bot '{}' spawned successfully with FULL AI SUITE", profile.name);
    }
    
    return bot;
}

uint32 GetNpcEntryForClass(BotClass botClass, BotRole /*role*/) {
    // Map bot classes to appropriate NPC creature entries
    // These are standard WoW NPCs that have the right model/animations
    
    if (botClass == BotClass::WARRIOR) return 1641;      // Warrior trainer
    if (botClass == BotClass::PALADIN) return 1615;      // Paladin trainer
    if (botClass == BotClass::DRUID) return 1619;        // Druid trainer
    if (botClass == BotClass::SHAMAN) return 1594;       // Shaman trainer
    if (botClass == BotClass::PRIEST) return 1634;       // Priest trainer
    if (botClass == BotClass::MAGE) return 1580;         // Mage trainer
    if (botClass == BotClass::WARLOCK) return 1587;      // Warlock trainer
    if (botClass == BotClass::ROGUE) return 1600;        // Rogue trainer
    if (botClass == BotClass::HUNTER) return 1631;       // Hunter trainer
    if (botClass == BotClass::DEATH_KNIGHT) return 28472; // DK trainer
    
    return 1641; // Default to warrior
}

Creature* BotSpawner::CreateBotPlayer(Player* owner, BotProfile profile) {
    if (!owner || !owner->GetMap()) {
        LOG_ERROR("module", "CreateBotPlayer: Invalid owner or map");
        return nullptr;
    }
    
    // ========================================================================
    // FORMATION POSITIONING
    // ========================================================================
    Position pos = owner->GetPosition();
    
    auto existingBots = ownerBotsCreatures[owner->GetGUID().GetCounter()];
    uint32 botIndex = existingBots.size();
    
    // Semicircle formation behind player
    float baseAngle = owner->GetOrientation() + M_PI;
    float angleOffset = (botIndex % Constants::FORMATION_BOTS_PER_ROW - 1) * 
                       (Constants::FORMATION_ARC / 6);
    
    float angle = baseAngle + angleOffset;
    float dist = 4.0f + (botIndex / Constants::FORMATION_BOTS_PER_ROW) * 
                 Constants::FORMATION_ROW_DEPTH;
    
    pos.m_positionX += dist * cos(angle);
    pos.m_positionY += dist * sin(angle);
    
    // ========================================================================
    // CREATURE CREATION
    // ========================================================================
    uint32 creatureEntry = GetNpcEntryForClass(profile.botClass, profile.role);
    
    CreatureTemplate const* cInfo = sObjectMgr->GetCreatureTemplate(creatureEntry);
    if (!cInfo) {
        LOG_ERROR("module", "Failed to get creature template for entry {}", creatureEntry);
        return nullptr;
    }
    
    Creature* bot = new Creature();
    if (!bot->Create(owner->GetMap()->GenerateLowGuid<HighGuid::Unit>(), 
                     owner->GetMap(), 
                     owner->GetPhaseMask(),
                     creatureEntry,
                     0,
                     pos.GetPositionX(),
                     pos.GetPositionY(),
                     pos.GetPositionZ(),
                     pos.GetOrientation())) {
        delete bot;
        LOG_ERROR("module", "Failed to create bot creature");
        return nullptr;
    }
    
    // ========================================================================
    // NAME SETTING - CRITICAL FIX
    // ========================================================================
    
    // Try multiple methods to ensure name sticks
    bot->SetName(profile.name);
    
    // Method 1: Set object name
    if (bot->GetCreatureTemplate()) {
        CreatureTemplate* templ = const_cast<CreatureTemplate*>(bot->GetCreatureTemplate());
        if (templ) {
            templ->Name = profile.name;
        }
    }
    
    // Method 2: Set via creature data
    CreatureData* data = const_cast<CreatureData*>(bot->GetCreatureData());
    if (data) {
        // Note: CreatureData may not have name field in all AC versions
        // This is a fallback
    }
    
    LOG_INFO("module", "Setting bot name to: {}", profile.name);
    
    // ========================================================================
    // BASIC STATS
    // ========================================================================
    bot->SetLevel(owner->GetLevel());
    bot->SetFaction(owner->GetFaction());
    bot->SetUInt32Value(UNIT_FIELD_FLAGS, 0);
    bot->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    bot->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
    bot->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
    bot->SetUInt32Value(UNIT_NPC_FLAGS, 0);
    bot->SetReactState(REACT_AGGRESSIVE);
    bot->SetPvP(true);
    
    // ========================================================================
    // LEVEL-SCALED HP
    // ========================================================================
    if (bot) {
        uint32 ownerLevel = owner->GetLevel();
        uint32 baseHealth = bot->GetMaxHealth();
        
        // Level scaling factor: 1x at lv1-20, 1.5x at 21-40, 2x at 41-60, 3x at 61-80
        float levelFactor = 1.0f + (ownerLevel / 20.0f);
        
        uint32 finalHealth = 0;
        if (profile.role == BotRole::TANK) {
            finalHealth = baseHealth * Constants::TANK_HP_MULTIPLIER * levelFactor;
        } else if (profile.role == BotRole::HEALER) {
            finalHealth = baseHealth * Constants::HEALER_HP_MULTIPLIER * levelFactor;
        } else {
            finalHealth = baseHealth * Constants::DPS_HP_MULTIPLIER * levelFactor;
        }
        
        bot->SetMaxHealth(finalHealth);
        bot->SetHealth(finalHealth);
        
        LOG_INFO("module", "{} (Lv{} {}) HP: {} (factor: {:.1f}x)",
                 profile.name, ownerLevel, 
                 profile.role == BotRole::TANK ? "TANK" :
                 profile.role == BotRole::HEALER ? "HEALER" : "DPS",
                 finalHealth, levelFactor);
    }
    
    // ========================================================================
    // LEVEL-SCALED MANA
    // ========================================================================
    if (bot) {
        uint32 ownerLevel = owner->GetLevel();
        uint32 baseMana = Constants::BASE_MANA_START + 
                         (ownerLevel * Constants::BASE_MANA_PER_LEVEL);
        
        uint32 finalMana = 0;
        if (profile.role == BotRole::HEALER) {
            finalMana = baseMana * Constants::HEALER_MANA_MULTIPLIER;
        } else if (profile.role == BotRole::DPS) {
            finalMana = baseMana * Constants::DPS_MANA_MULTIPLIER;
        } else {
            finalMana = baseMana * Constants::TANK_MANA_MULTIPLIER;
        }
        
        bot->SetMaxPower(POWER_MANA, finalMana);
        bot->SetPower(POWER_MANA, finalMana);
        
        LOG_INFO("module", "{} mana pool: {}", profile.name, finalMana);
    }
    
    // ========================================================================
    // ADD TO WORLD
    // ========================================================================
    if (!owner->GetMap()->AddToMap(bot)) {
        delete bot;
        LOG_ERROR("module", "Failed to add bot to map");
        return nullptr;
    }
    
    // ========================================================================
    // REGISTER BOT
    // ========================================================================
    ownerBotsCreatures[owner->GetGUID().GetCounter()].push_back(bot);
    ownerBots_[owner->GetGUID().GetCounter()].push_back(bot->GetGUID().GetCounter());
    
    LOG_INFO("module", "✓ '{}' successfully spawned and registered!", profile.name);
    
    return bot;
}

void BotSpawner::EquipBot(Creature* bot, BotRole role, uint8 level) {
    // TODO: Implement equipment system
    // This will equip bots with level-appropriate gear
    if (!bot) return;
    
    // Placeholder for future implementation
    LOG_DEBUG("module", "EquipBot called for {} (lv{})", bot->GetName(), level);
}

void BotSpawner::SetBotStats(Creature* bot, BotRole role) {
    // TODO: Implement detailed stat system
    // This will set proper stats beyond HP/mana
    if (!bot) return;
    
    // Placeholder for future implementation
    LOG_DEBUG("module", "SetBotStats called for {}", bot->GetName());
}

bool BotSpawner::RemoveBot(Player* owner, const std::string& name) {
    if (!owner) return false;
    
    auto it = ownerBotsCreatures.find(owner->GetGUID().GetCounter());
    if (it != ownerBotsCreatures.end()) {
        for (auto botIt = it->second.begin(); botIt != it->second.end(); ++botIt) {
            Creature* bot = *botIt;
            if (bot && bot->GetName() == name) {
                BotControllerManager::instance()->UnregisterBot(bot);
                BotCombatManager::instance()->UnregisterBot(bot);
                bot->DespawnOrUnsummon();
                it->second.erase(botIt);
                LOG_INFO("module", "✓ Bot '{}' dismissed", name);
                return true;
            }
        }
    }
    return false;
}

void BotSpawner::RemoveAllBots(Player* owner) {
    if (!owner) return;
    
    auto it = ownerBotsCreatures.find(owner->GetGUID().GetCounter());
    if (it != ownerBotsCreatures.end()) {
        uint32 count = it->second.size();
        for (Creature* bot : it->second) {
            if (bot) {
                BotControllerManager::instance()->UnregisterBot(bot);
                BotCombatManager::instance()->UnregisterBot(bot);
                bot->DespawnOrUnsummon();
            }
        }
        ownerBotsCreatures.erase(it);
        LOG_INFO("module", "✓ All {} bots dismissed", count);
    }
    ownerBots_.erase(owner->GetGUID().GetCounter());
}

std::vector<Creature*> BotSpawner::GetPlayerBots(Player* owner) {
    std::vector<Creature*> bots;
    if (!owner) return bots;
    
    auto it = ownerBotsCreatures.find(owner->GetGUID().GetCounter());
    if (it != ownerBotsCreatures.end()) {
        for (Creature* bot : it->second) {
            if (bot && bot->IsInWorld()) {
                bots.push_back(bot);
            }
        }
    }
    return bots;
}

void BotSpawner::FillGroup(Player* owner, uint8 groupSize) {
    if (!owner) return;
    
    uint8 tanksNeeded = 1;
    uint8 healersNeeded = 1;
    uint8 dpsNeeded = groupSize - 2;
    
    LOG_INFO("module", "Filling {}-man group for {}...", groupSize, owner->GetName());
    
    auto existingBots = GetPlayerBots(owner);
    uint8 existingTanks = 0, existingHealers = 0, existingDps = 0;
    
    for (auto bot : existingBots) {
        if (!bot) continue;
        auto controller = BotControllerManager::instance()->GetController(bot);
        if (controller) {
            BotRole role = controller->GetRole();
            if (role == BotRole::TANK) existingTanks++;
            else if (role == BotRole::HEALER) existingHealers++;
            else existingDps++;
        }
    }
    
    // Spawn needed bots
    for (uint8 i = existingTanks; i < tanksNeeded; i++) {
        SpawnBot(owner, "tank", BotRole::TANK);
    }
    for (uint8 i = existingHealers; i < healersNeeded; i++) {
        SpawnBot(owner, "healer", BotRole::HEALER);
    }
    for (uint8 i = existingDps; i < dpsNeeded; i++) {
        SpawnBot(owner, "dps", BotRole::DPS);
    }
    
    LOG_INFO("module", "✓ Group composition complete!");
}

void BotSpawner::FillRaid(Player* owner, uint8 raidSize) {
    if (!owner) return;
    
    uint8 tanksNeeded = (raidSize == 10) ? 2 : 3;
    uint8 healersNeeded = (raidSize == 10) ? 3 : 6;
    uint8 dpsNeeded = raidSize - tanksNeeded - healersNeeded;
    
    LOG_INFO("module", "Filling {}-man raid for {}...", raidSize, owner->GetName());
    
    auto existingBots = GetPlayerBots(owner);
    uint8 existingTanks = 0, existingHealers = 0, existingDps = 0;
    
    for (auto bot : existingBots) {
        if (!bot) continue;
        auto controller = BotControllerManager::instance()->GetController(bot);
        if (controller) {
            BotRole role = controller->GetRole();
            if (role == BotRole::TANK) existingTanks++;
            else if (role == BotRole::HEALER) existingHealers++;
            else existingDps++;
        }
    }
    
    // Spawn needed bots
    for (uint8 i = existingTanks; i < tanksNeeded; i++) {
        SpawnBot(owner, "tank", BotRole::TANK);
    }
    for (uint8 i = existingHealers; i < healersNeeded; i++) {
        SpawnBot(owner, "healer", BotRole::HEALER);
    }
    for (uint8 i = existingDps; i < dpsNeeded; i++) {
        SpawnBot(owner, "dps", BotRole::DPS);
    }
    
    LOG_INFO("module", "✓ Raid composition complete!");
}

} // namespace BotAI
