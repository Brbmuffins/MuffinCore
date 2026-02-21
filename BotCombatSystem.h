#ifndef _BOT_COMBAT_SYSTEM_H
#define _BOT_COMBAT_SYSTEM_H

#include "Creature.h"
#include "Player.h"
#include "Unit.h"
#include "Spell.h"
#include "SpellMgr.h"
#include "BotSpawner.h"
#include <map>
#include <vector>

namespace BotAI {

// ============================================================================
// SPELL CONSTANTS
// ============================================================================

namespace Spells {
    namespace Warrior {
        const uint32 TAUNT = 355;
        const uint32 SUNDER_ARMOR = 7386;
        const uint32 HEROIC_STRIKE = 78;
        const uint32 THUNDER_CLAP = 6343;
        const uint32 BLOODRAGE = 2687;
        const uint32 SHIELD_BASH = 72;
        const uint32 BATTLE_SHOUT = 6673;
    }
    
    namespace Priest {
        const uint32 FLASH_HEAL = 2061;
        const uint32 RENEW = 139;
        const uint32 POWER_WORD_SHIELD = 17;
        const uint32 SHADOW_WORD_PAIN = 589;
        const uint32 SMITE = 585;
        const uint32 POWER_WORD_FORTITUDE = 1243;
        const uint32 DIVINE_SPIRIT = 14752;
        const uint32 DISPEL_MAGIC = 527;
    }
    
    namespace Mage {
        const uint32 FIREBALL = 133;
        const uint32 FIRE_BLAST = 2136;
        const uint32 ARCANE_MISSILES = 5143;
        const uint32 COUNTERSPELL = 2139;
        const uint32 ARCANE_INTELLECT = 1459;
    }
    
    namespace Paladin {
        // Tank
        const uint32 RIGHTEOUS_FURY = 25780;
        const uint32 HOLY_SHIELD = 20925;
        const uint32 AVENGERS_SHIELD = 31935;
        const uint32 HAMMER_OF_RIGHTEOUSNESS = 53595;
        const uint32 SHIELD_OF_RIGHTEOUSNESS = 61411;
        const uint32 CONSECRATION = 26573;
        // Healer
        const uint32 FLASH_OF_LIGHT = 19750;
        const uint32 HOLY_LIGHT = 635;
        const uint32 HOLY_SHOCK = 20473;
        const uint32 BEACON_OF_LIGHT = 53563;
        // Buffs
        const uint32 BLESSING_OF_KINGS = 20217;
        const uint32 BLESSING_OF_MIGHT = 19740;
    }
    
    namespace DeathKnight {
        // Tank
        const uint32 DEATH_GRIP = 49576;
        const uint32 ICEBOUND_FORTITUDE = 48792;
        const uint32 HEART_STRIKE = 55050;
        const uint32 RUNE_STRIKE = 56815;
        const uint32 DEATH_AND_DECAY = 43265;
        const uint32 BLOOD_STRIKE = 45902;
        const uint32 PLAGUE_STRIKE = 45462;
        const uint32 STRANGULATE = 47476;
    }
    
    namespace Shaman {
        // Healer
        const uint32 HEALING_WAVE = 331;
        const uint32 LESSER_HEALING_WAVE = 8004;
        const uint32 CHAIN_HEAL = 1064;
        const uint32 RIPTIDE = 61295;
        const uint32 EARTH_SHIELD = 974;
        // DPS
        const uint32 LIGHTNING_BOLT = 403;
        const uint32 CHAIN_LIGHTNING = 421;
        const uint32 WIND_SHEAR = 57994;
    }
    
    namespace Druid {
        // Healer
        const uint32 HEALING_TOUCH = 5185;
        const uint32 REJUVENATION = 774;
        const uint32 REGROWTH = 8936;
        const uint32 SWIFTMEND = 18562;
        const uint32 WILD_GROWTH = 53251;
        const uint32 NOURISH = 50464;
        // Forms
        const uint32 BEAR_FORM = 5487;
        const uint32 CAT_FORM = 768;
        // Buffs
        const uint32 MARK_OF_THE_WILD = 1126;
    }
    
    namespace Rogue {
        // DPS
        const uint32 SINISTER_STRIKE = 1752;
        const uint32 EVISCERATE = 2098;
        const uint32 SLICE_AND_DICE = 5171;
        const uint32 RUPTURE = 1943;
        const uint32 BACKSTAB = 53;
        const uint32 MUTILATE = 1329;
        const uint32 ENVENOM = 32645;
        const uint32 FAN_OF_KNIVES = 51723;
        const uint32 KICK = 1766;
    }
    
    namespace Hunter {
        // DPS
        const uint32 STEADY_SHOT = 56641;
        const uint32 ARCANE_SHOT = 3044;
        const uint32 MULTI_SHOT = 2643;
        const uint32 SERPENT_STING = 1978;
        const uint32 AIMED_SHOT = 19434;
        const uint32 KILL_SHOT = 53351;
        const uint32 EXPLOSIVE_SHOT = 53301;
        const uint32 SILENCING_SHOT = 34490;
    }
    
    namespace Warlock {
        // DPS
        const uint32 SHADOW_BOLT = 686;
        const uint32 CORRUPTION = 172;
        const uint32 CURSE_OF_AGONY = 980;
        const uint32 IMMOLATE = 348;
        const uint32 INCINERATE = 29722;
        const uint32 CONFLAGRATE = 17962;
        const uint32 CHAOS_BOLT = 50796;
        const uint32 HAUNT = 48181;
    }
} // namespace Spells

// ============================================================================
// COMBAT BEHAVIOR BASE CLASS
// ============================================================================

class CombatBehavior {
public:
    CombatBehavior(Creature* bot, BotRole role);
    virtual ~CombatBehavior() = default;
    
    virtual void Update(uint32 diff);
    virtual void ExecuteRotation();
    
    // Spell casting
    bool CanCastSpell(uint32 spellId);
    void CastSpell(Unit* target, uint32 spellId);
    void DirectHeal(Unit* target, uint32 amount);
    void DirectDamage(Unit* target, uint32 amount);
    
    // Target management
    Unit* SelectTarget();
    Player* GetOwner();
    
    // Positioning
    void MaintainRangedDistance(float minDist, float maxDist, float idealDist);
    
    // Advanced features
    bool TryInterrupt();
    void CheckBuffs();
    void HandleResurrection(uint32 diff);
    
    // Getters
    Creature* GetBot() const { return bot_; }
    BotRole GetRole() const { return role_; }
    
protected:
    Creature* bot_;
    BotRole role_;
    Unit* currentTarget_;
    uint32 globalCooldown_;
    uint32 rotationTimer_;
    uint32 lastInterruptTime_;
    uint32 lastBuffTime_;
    uint32 resurrectionTimer_;
    
    float GetManaPercent();
    float GetHealthPercent();
    bool IsInMeleeRange(Unit* target);
    
    virtual void ApplyClassBuffs(Player* owner, const std::vector<Creature*>& allBots);
};

// ============================================================================
// TANK COMBAT
// ============================================================================

class TankCombat : public CombatBehavior {
public:
    TankCombat(Creature* bot) : CombatBehavior(bot, BotRole::TANK) {}
    void ExecuteRotation() override;
    void PositionForMelee();
};

// ============================================================================
// HEALER COMBAT
// ============================================================================

class HealerCombat : public CombatBehavior {
public:
    HealerCombat(Creature* bot) : CombatBehavior(bot, BotRole::HEALER) {}
    void ExecuteRotation() override;
    void PositionForRanged();
    Unit* FindHealTarget();
};

// ============================================================================
// DPS COMBAT
// ============================================================================

class DpsCombat : public CombatBehavior {
public:
    DpsCombat(Creature* bot) : CombatBehavior(bot, BotRole::DPS) {}
    void ExecuteRotation() override;
    void PositionForRanged();
};

// ============================================================================
// COMBAT MANAGER
// ============================================================================

class BotCombatManager {
public:
    static BotCombatManager* instance();
    
    void RegisterBot(Creature* bot, BotRole role);
    void UnregisterBot(Creature* bot);
    void UpdateAllCombat(uint32 diff);
    CombatBehavior* GetBehavior(Creature* bot);
    
private:
    BotCombatManager() = default;
    static BotCombatManager* instance_;
    std::map<uint64, CombatBehavior*> behaviors_;
};

} // namespace BotAI

#endif // _BOT_COMBAT_SYSTEM_H
