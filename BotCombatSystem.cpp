#include "BotCombatSystem.h"
#include "BotAI.h"
#include "BotConstants.h"
#include "Creature.h"
#include "Player.h"
#include "SpellMgr.h"
#include "Log.h"
#include "Group.h"
#include <algorithm>
#include <cmath>

namespace BotAI {

// ============================================================================
// COMBAT BEHAVIOR - BASE CLASS
// ============================================================================

CombatBehavior::CombatBehavior(Creature* bot, BotRole role)
    : bot_(bot), role_(role), currentTarget_(nullptr), 
      globalCooldown_(0), rotationTimer_(0), lastInterruptTime_(0),
      lastBuffTime_(0), resurrectionTimer_(0)
{
}

void CombatBehavior::Update(uint32 diff) {
    if (!bot_ || !bot_->IsAlive()) {
        HandleResurrection(diff);
        return;
    }
    
    // Update timers
    if (globalCooldown_ > 0) {
        globalCooldown_ = globalCooldown_ > diff ? globalCooldown_ - diff : 0;
    }
    
    if (rotationTimer_ > 0) {
        rotationTimer_ = rotationTimer_ > diff ? rotationTimer_ - diff : 0;
    }
    
    if (lastInterruptTime_ > 0) {
        lastInterruptTime_ = lastInterruptTime_ > diff ? lastInterruptTime_ - diff : 0;
    }
    
    if (lastBuffTime_ > diff) {
        lastBuffTime_ -= diff;
    }
    
    // Buff check (every 30 seconds)
    if (lastBuffTime_ == 0) {
        CheckBuffs();
        lastBuffTime_ = Constants::BUFF_CHECK_MS;
    }
    
    // HEALERS: Always check for healing, even out of combat
    if (role_ == BotRole::HEALER && rotationTimer_ == 0) {
        ExecuteRotation();
        rotationTimer_ = Constants::HEAL_ROTATION_MS;
        return;
    }
    
    // Everyone else: Only in combat
    if (bot_->IsInCombat() && rotationTimer_ == 0) {
        ExecuteRotation();
        rotationTimer_ = Constants::ROTATION_INTERVAL_MS;
    }
}

void CombatBehavior::HandleResurrection(uint32 diff) {
    if (bot_->IsAlive()) return;
    
    // Check if owner is alive
    Player* owner = GetOwner();
    if (!owner || !owner->IsInWorld() || !owner->IsAlive()) {
        return;
    }
    
    // Resurrection delay
    if (resurrectionTimer_ > diff) {
        resurrectionTimer_ -= diff;
        return;
    }
    
    // RESURRECT BOT
    bot_->Respawn();
    bot_->SetHealth(bot_->GetMaxHealth() * Constants::RESURRECTION_HEALTH_PCT / 100.0f);
    
    if (bot_->GetMaxPower(POWER_MANA) > 0) {
        bot_->SetPower(POWER_MANA, bot_->GetMaxPower(POWER_MANA) * Constants::RESURRECTION_MANA_PCT / 100.0f);
    }
    
    // Teleport to owner
    bot_->NearTeleportTo(owner->GetPosition());
    
    LOG_INFO("module", "{} has been resurrected!", bot_->GetName());
    resurrectionTimer_ = Constants::RESURRECTION_DELAY_MS;
}

void CombatBehavior::ExecuteRotation() {}

bool CombatBehavior::CanCastSpell(uint32 spellId) {
    if (!bot_ || globalCooldown_ > 0) return false;
    
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo) return false;
    
    uint32 manaCost = spellInfo->CalcPowerCost(bot_, spellInfo->GetSchoolMask());
    if (bot_->GetPower(POWER_MANA) < manaCost) return false;
    
    return true;
}

void CombatBehavior::CastSpell(Unit* target, uint32 spellId) {
    if (!bot_ || !target || !CanCastSpell(spellId)) return;
    
    bot_->CastSpell(target, spellId, false);
    globalCooldown_ = Constants::GLOBAL_COOLDOWN_MS;
}

Player* CombatBehavior::GetOwner() {
    auto controller = BotAI::BotControllerManager::instance()->GetController(bot_);
    return controller ? controller->GetOwner() : nullptr;
}

Unit* CombatBehavior::SelectTarget() {
    if constexpr (Constants::ENABLE_DEBUG_LOGGING) {
        LOG_DEBUG("module", "SelectTarget called for {}", bot_->GetName());
    }
    
    // Get owner and map info first
    Player* owner = GetOwner();
    if (!owner || !owner->IsInWorld()) {
        currentTarget_ = nullptr;
        return nullptr;
    }
    
    Map* map = bot_->GetMap();
    bool inDungeon = map && map->IsDungeon();
    
    // LEASH CHECK - Critical for all roles
    if (currentTarget_ && owner) {
        float distToOwner = bot_->GetDistance(owner);
        if (distToOwner > Constants::LEASH_DISTANCE) {
            if constexpr (Constants::ENABLE_DEBUG_LOGGING) {
                LOG_DEBUG("module", "{} beyond leash distance ({}y from owner), dropping target",
                         bot_->GetName(), distToOwner);
            }
            bot_->AttackStop();
            currentTarget_ = nullptr;
            return nullptr;
        }
    }
    
    // CRITICAL: Clear old target if in open world and owner has no target
    if (!inDungeon && owner && !owner->GetSelectedUnit()) {
        if (currentTarget_) {
            if constexpr (Constants::ENABLE_DEBUG_LOGGING) {
                LOG_DEBUG("module", "{} clearing old target (open world, no owner target)", 
                         bot_->GetName());
            }
            currentTarget_ = nullptr;
        }
        return nullptr;
    }
    
    // First priority: current target if still valid
    if (currentTarget_ && currentTarget_->IsAlive() && bot_->IsValidAttackTarget(currentTarget_)) {
        // In open world, only keep target if it's the owner's target
        if (!inDungeon && owner && owner->GetSelectedUnit() != currentTarget_) {
            if constexpr (Constants::ENABLE_DEBUG_LOGGING) {
                LOG_DEBUG("module", "{} clearing invalid target (not owner's target)", 
                         bot_->GetName());
            }
            currentTarget_ = nullptr;
        } else {
            return currentTarget_;
        }
    }
    
    // Second priority: Owner's target (ALWAYS in open world, preferred in dungeons)
    if (owner && owner->GetSelectedUnit()) {
        Unit* ownerTarget = owner->GetSelectedUnit();
        if (ownerTarget->IsAlive() && bot_->IsValidAttackTarget(ownerTarget)) {
            currentTarget_ = ownerTarget;
            if constexpr (Constants::ENABLE_DEBUG_LOGGING) {
                LOG_DEBUG("module", "{} attacking owner's target: {}", 
                         bot_->GetName(), ownerTarget->GetName());
            }
            return currentTarget_;
        }
    }
    
    // Third priority: Only in dungeons, can attack nearby enemies
    if (inDungeon) {
        Unit* nearbyTarget = bot_->SelectNearestTarget(Constants::DUNGEON_ATTACK_RANGE);
        
        // Double check distance to owner
        if (nearbyTarget && owner && nearbyTarget->GetDistance(owner) > Constants::DUNGEON_ATTACK_RANGE) {
            nearbyTarget = nullptr;
        }
        
        if (nearbyTarget && nearbyTarget->IsAlive() && bot_->IsValidAttackTarget(nearbyTarget)) {
            currentTarget_ = nearbyTarget;
            if constexpr (Constants::ENABLE_DEBUG_LOGGING) {
                LOG_DEBUG("module", "{} (dungeon) attacking nearby: {}", 
                         bot_->GetName(), nearbyTarget->GetName());
            }
            return currentTarget_;
        }
    }
    
    // Open world: No valid target
    if constexpr (Constants::ENABLE_DEBUG_LOGGING) {
        LOG_DEBUG("module", "{} has no valid target", bot_->GetName());
    }
    currentTarget_ = nullptr;
    return nullptr;
}

float CombatBehavior::GetManaPercent() {
    if (bot_->GetMaxPower(POWER_MANA) == 0) return 100.0f;
    return (float)bot_->GetPower(POWER_MANA) / (float)bot_->GetMaxPower(POWER_MANA) * 100.0f;
}

float CombatBehavior::GetHealthPercent() {
    return bot_->GetHealthPct();
}

bool CombatBehavior::IsInMeleeRange(Unit* target) {
    if (!target) return false;
    return bot_->GetDistance(target) <= Constants::MELEE_RANGE;
}

void CombatBehavior::DirectHeal(Unit* target, uint32 amount) {
    if (!target || !bot_) {
        LOG_ERROR("module", "DirectHeal: target or bot is null!");
        return;
    }
    
    if constexpr (Constants::ENABLE_VERBOSE_COMBAT) {
        LOG_INFO("module", "DirectHeal: {} attempting to heal {} for {} HP", 
                 bot_->GetName(), target->GetName(), amount);
    }
    
    // Calculate mana cost
    uint32 manaCost = amount / 10;
    
    // Check mana
    if (bot_->GetPower(POWER_MANA) < manaCost) {
        LOG_ERROR("module", "{} OOM! Cannot heal (need {} mana, have {})", 
                  bot_->GetName(), manaCost, bot_->GetPower(POWER_MANA));
        return;
    }
    
    // Consume mana
    bot_->SetPower(POWER_MANA, bot_->GetPower(POWER_MANA) - manaCost);
    
    // VISUAL: Cast Flash Heal animation
    bot_->CastSpell(target, Spells::Priest::FLASH_HEAL, true);
    
    // Apply healing with threat reduction
    uint32 currentHealth = target->GetHealth();
    uint32 maxHealth = target->GetMaxHealth();
    if (maxHealth == 0) {
        LOG_ERROR("module", "DirectHeal: {} has zero max health, skipping heal", target->GetName());
        return;
    }
    uint32 newHealth = std::min(currentHealth + amount, maxHealth);
    
    target->SetHealth(newHealth);
    
    // Reduce healer threat
    if (currentTarget_ && role_ == BotRole::HEALER) {
        float threatReduction = amount * Constants::HEALER_THREAT_MULTIPLIER;
        currentTarget_->AddThreat(bot_, -threatReduction);
    }
    
    if constexpr (Constants::ENABLE_VERBOSE_COMBAT) {
        LOG_DEBUG("module", "{} healed {} for {} HP ({}% -> {}%)",
                  bot_->GetName(), target->GetName(), amount,
                  (currentHealth * 100 / maxHealth),
                  (newHealth * 100 / maxHealth));
    }
}

void CombatBehavior::DirectDamage(Unit* target, uint32 amount) {
    if (!target || !bot_) return;
    
    Unit::DealDamage(bot_, target, amount, nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL);

    // TANK THREAT BOOST
    if (role_ == BotRole::TANK && currentTarget_) {
        float threatAmount = amount * Constants::TANK_THREAT_MULTIPLIER;
        currentTarget_->AddThreat(bot_, threatAmount);
    }
}

void CombatBehavior::MaintainRangedDistance(float minDist, float maxDist, float idealDist) {
    Unit* target = SelectTarget();
    if (!target) return;
    
    float distance = bot_->GetDistance(target);
    
    if (distance < minDist) {
        // Too close - back up
        float angle = bot_->GetAngle(target) + M_PI;
        float backupDist = (minDist - distance) + 2.0f;
        float x = bot_->GetPositionX() + backupDist * cos(angle);
        float y = bot_->GetPositionY() + backupDist * sin(angle);
        
        bot_->GetMotionMaster()->MovePoint(0, x, y, bot_->GetPositionZ());
    } else if (distance > maxDist) {
        // Too far - move closer
        bot_->GetMotionMaster()->MoveChase(target, idealDist);
    }
}

bool CombatBehavior::TryInterrupt() {
    if (lastInterruptTime_ > 0) return false;
    
    Unit* target = SelectTarget();
    if (!target || !target->IsNonMeleeSpellCast(false)) return false;
    
    // Get cast info
    Spell const* spell = target->GetCurrentSpell(CURRENT_GENERIC_SPELL);
    if (!spell) return false;
    
    // Check if worth interrupting
    uint32 castTime = spell->GetCastTime();
    if (castTime < Constants::INTERRUPT_PRIORITY_THRESHOLD * 1000) {
        return false; // Too short to bother
    }
    
    // Try to interrupt based on class
    uint32 interruptSpell = 0;
    
    // Determine interrupt spell based on bot class
    // This is a placeholder - you'll need to add proper spell IDs
    if (role_ == BotRole::TANK) {
        interruptSpell = 72; // Shield Bash (warrior)
    } else if (role_ == BotRole::DPS) {
        interruptSpell = 2139; // Counterspell (mage)
    }
    
    if (interruptSpell && CanCastSpell(interruptSpell)) {
        CastSpell(target, interruptSpell);
        lastInterruptTime_ = Constants::INTERRUPT_COOLDOWN_MS;
        LOG_INFO("module", "{} interrupted {}!", bot_->GetName(), target->GetName());
        return true;
    }
    
    return false;
}

void CombatBehavior::CheckBuffs() {
    Player* owner = GetOwner();
    if (!owner || !owner->IsInWorld()) return;
    
    // Get all bots in group
    auto allBots = BotAI::BotSpawner::instance()->GetPlayerBots(owner);
    
    // Apply buffs based on bot class
    ApplyClassBuffs(owner, allBots);
}

void CombatBehavior::ApplyClassBuffs(Player* owner, const std::vector<Creature*>& allBots) {
    // Placeholder for buff system - implement based on bot class
    // This will be expanded in BotBuffSystem.cpp
}

// ============================================================================
// TANK COMBAT
// ============================================================================

void TankCombat::ExecuteRotation() {
    Unit* target = SelectTarget();
    if (!target) return;
    
    // CRITICAL: LEASH CHECK
    Player* owner = GetOwner();
    if (owner && owner->IsInWorld()) {
        float distToOwner = bot_->GetDistance(owner);
        if (distToOwner > Constants::LEASH_DISTANCE) {
            if constexpr (Constants::ENABLE_DEBUG_LOGGING) {
                LOG_DEBUG("module", "Tank {} beyond leash ({}y), stopping combat",
                         bot_->GetName(), distToOwner);
            }
            bot_->AttackStop();
            currentTarget_ = nullptr;
            return;
        }
    }
    
    // Check for interrupt opportunity
    TryInterrupt();
    
    // Taunt if needed
    if (target->GetVictim() != bot_ && CanCastSpell(Spells::Warrior::TAUNT)) {
        CastSpell(target, Spells::Warrior::TAUNT);
        target->AddThreat(bot_, Constants::TAUNT_THREAT_AMOUNT);
        return;
    }
    
    // Position check
    PositionForMelee();
    
    // Sunder Armor
    if (CanCastSpell(Spells::Warrior::SUNDER_ARMOR) && IsInMeleeRange(target)) {
        CastSpell(target, Spells::Warrior::SUNDER_ARMOR);
        return;
    }
    
    // Thunder Clap (AoE threat)
    if (CanCastSpell(Spells::Warrior::THUNDER_CLAP) && IsInMeleeRange(target)) {
        CastSpell(target, Spells::Warrior::THUNDER_CLAP);
        return;
    }
    
    // Heroic Strike
    if (CanCastSpell(Spells::Warrior::HEROIC_STRIKE) && IsInMeleeRange(target)) {
        CastSpell(target, Spells::Warrior::HEROIC_STRIKE);
        return;
    }
    
    // Fallback direct damage
    uint32 damage = Constants::TANK_BASE_DAMAGE + (bot_->GetLevel() * Constants::TANK_DAMAGE_PER_LEVEL);
    DirectDamage(target, damage);
}

void TankCombat::PositionForMelee() {
    Unit* target = SelectTarget();
    if (!target) return;
    
    float distance = bot_->GetDistance(target);
    
    // Stay in melee range
    if (distance > Constants::MELEE_RANGE + 2.0f) {
        bot_->GetMotionMaster()->MoveChase(target, Constants::MELEE_RANGE);
    }
}

// ============================================================================
// HEALER COMBAT
// ============================================================================

Unit* HealerCombat::FindHealTarget() {
    if constexpr (Constants::ENABLE_DEBUG_LOGGING) {
        LOG_DEBUG("module", "FindHealTarget: Looking for targets to heal...");
    }

    // Get owner
    Player* owner = GetOwner();
    if (!owner || !owner->IsInWorld()) {
        LOG_ERROR("module", "Healer {} has no owner!", bot_->GetName());
        return nullptr;
    }

    // PRIORITY 1: Owner (YOU) - if below threshold
    if (owner && owner->IsAlive() && owner->GetHealthPct() < Constants::HEAL_OWNER_PCT) {
        if constexpr (Constants::ENABLE_DEBUG_LOGGING) {
            LOG_DEBUG("module", "Healing OWNER at {}%", owner->GetHealthPct());
        }
        return owner;
    }

    // PRIORITY 2: Other bots in group - if below threshold
    auto allBots = BotAI::BotSpawner::instance()->GetPlayerBots(owner);
    Unit* lowestBot = nullptr;
    float lowestHp = 100.0f;

    for (Creature* botCreature : allBots) {
        if (!botCreature || !botCreature->IsAlive()) continue;
        if (botCreature == bot_) continue; // Don't consider self yet
        
        float botHp = botCreature->GetHealthPct();
        if (botHp < Constants::HEAL_OTHER_BOTS_PCT && botHp < lowestHp) {
            // Check range
            if (bot_->GetDistance(botCreature) <= Constants::MAX_HEAL_TARGET_RANGE) {
                lowestBot = botCreature;
                lowestHp = botHp;
            }
        }
    }

    if (lowestBot) {
        if constexpr (Constants::ENABLE_DEBUG_LOGGING) {
            LOG_DEBUG("module", "Healing BOT {} at {}%", lowestBot->GetName(), lowestHp);
        }
        return lowestBot;
    }

    // PRIORITY 3: Self (healer) - if below threshold
    if (bot_->GetHealthPct() < Constants::HEAL_SELF_PCT) {
        if constexpr (Constants::ENABLE_DEBUG_LOGGING) {
            LOG_DEBUG("module", "Healing SELF at {}%", bot_->GetHealthPct());
        }
        return bot_;
    }

    if constexpr (Constants::ENABLE_DEBUG_LOGGING) {
        LOG_DEBUG("module", "Everyone healthy - no healing needed");
    }
    return nullptr;
}

void HealerCombat::ExecuteRotation() {
    // CRITICAL: CHECK MANA FIRST
    if (GetManaPercent() < Constants::LOW_MANA_PCT) {
        LOG_WARN("module", "{} is OOM ({:.1f}% mana) - cannot heal", 
                 bot_->GetName(), GetManaPercent());
        return;
    }
    
    Unit* healTarget = FindHealTarget();
    if (!healTarget) {
        // No one needs healing - maintain position
        PositionForRanged();
        return;
    }
    
    float targetHp = healTarget->GetHealthPct();
    
    // Only heal if actually needed
    if (targetHp >= Constants::HEAL_TOPOFF_PCT) {
        return;
    }
    
    // Emergency heal - big heal!
    if (targetHp < Constants::HEAL_EMERGENCY_PCT) {
        uint32 healAmount = healTarget->GetMaxHealth() / 2;
        DirectHeal(healTarget, healAmount);
        LOG_INFO("module", "EMERGENCY HEAL on {} at {:.1f}%", healTarget->GetName(), targetHp);
        rotationTimer_ = Constants::EMERGENCY_HEAL_CD_MS;
        return;
    }
    
    // Low HP - medium heal
    if (targetHp < Constants::HEAL_MEDIUM_PCT) {
        uint32 healAmount = healTarget->GetMaxHealth() / 3;
        DirectHeal(healTarget, healAmount);
        if constexpr (Constants::ENABLE_VERBOSE_COMBAT) {
            LOG_INFO("module", "Medium heal on {} at {:.1f}%", healTarget->GetName(), targetHp);
        }
        rotationTimer_ = Constants::MEDIUM_HEAL_CD_MS;
        return;
    }
    
    // Top off - small heal
    if (targetHp < Constants::HEAL_TOPOFF_PCT) {
        uint32 healAmount = healTarget->GetMaxHealth() / 10;
        DirectHeal(healTarget, healAmount);
        if constexpr (Constants::ENABLE_VERBOSE_COMBAT) {
            LOG_INFO("module", "Top-off heal on {} at {:.1f}%", healTarget->GetName(), targetHp);
        }
        rotationTimer_ = Constants::TOPOFF_HEAL_CD_MS;
        return;
    }
}

void HealerCombat::PositionForRanged() {
    MaintainRangedDistance(Constants::HEALER_MIN_RANGE, 
                          Constants::HEALER_MAX_RANGE, 
                          Constants::HEALER_IDEAL_RANGE);
}

// ============================================================================
// DPS COMBAT
// ============================================================================

void DpsCombat::ExecuteRotation() {
    Unit* target = SelectTarget();
    if (!target) return;
    
    // CRITICAL: LEASH CHECK
    Player* owner = GetOwner();
    if (owner && owner->IsInWorld()) {
        float distToOwner = bot_->GetDistance(owner);
        if (distToOwner > Constants::LEASH_DISTANCE) {
            if constexpr (Constants::ENABLE_DEBUG_LOGGING) {
                LOG_DEBUG("module", "DPS {} beyond leash ({}y), stopping combat",
                         bot_->GetName(), distToOwner);
            }
            bot_->AttackStop();
            currentTarget_ = nullptr;
            return;
        }
    }
    
    // Position for ranged
    PositionForRanged();
    
    // Check for interrupt opportunity
    TryInterrupt();
    
    // Fireball
    if (CanCastSpell(Spells::Mage::FIREBALL)) {
        uint32 damage = Constants::DPS_BASE_DAMAGE + (bot_->GetLevel() * Constants::DPS_DAMAGE_PER_LEVEL);
        DirectDamage(target, damage);
        return;
    }
    
    // Fire Blast
    if (CanCastSpell(Spells::Mage::FIRE_BLAST)) {
        uint32 damage = Constants::DPS_FIREBLAST_BASE + (bot_->GetLevel() * Constants::DPS_FIREBLAST_PER_LEVEL);
        DirectDamage(target, damage);
        return;
    }
}

void DpsCombat::PositionForRanged() {
    MaintainRangedDistance(Constants::DPS_MIN_RANGE, 
                          Constants::DPS_MAX_RANGE, 
                          Constants::DPS_IDEAL_RANGE);
}

// ============================================================================
// COMBAT MANAGER
// ============================================================================

BotCombatManager* BotCombatManager::instance_ = nullptr;

BotCombatManager* BotCombatManager::instance() {
    if (!instance_) {
        instance_ = new BotCombatManager();
    }
    return instance_;
}

void BotCombatManager::RegisterBot(Creature* bot, BotRole role) {
    if (!bot) return;

    // Delete old behavior if exists (prevent memory leak)
    auto existing = behaviors_.find(bot->GetGUID().GetCounter());
    if (existing != behaviors_.end()) {
        LOG_WARN("module", "Bot {} already has combat behavior, replacing", bot->GetName());
        delete existing->second;
        behaviors_.erase(existing);
    }
    
    CombatBehavior* behavior = nullptr;
    
    switch (role) {
        case BotRole::TANK:
            behavior = new TankCombat(bot);
            break;
        case BotRole::HEALER:
            behavior = new HealerCombat(bot);
            break;
        case BotRole::DPS:
            behavior = new DpsCombat(bot);
            break;
    }
    
    if (behavior) {
        behaviors_[bot->GetGUID().GetCounter()] = behavior;
        LOG_INFO("module", "Combat registered for {} ({})", 
                 bot->GetName(), role == BotRole::TANK ? "TANK" : 
                                role == BotRole::HEALER ? "HEALER" : "DPS");
    }
}

void BotCombatManager::UnregisterBot(Creature* bot) {
    if (!bot) return;
    
    auto it = behaviors_.find(bot->GetGUID().GetCounter());
    if (it != behaviors_.end()) {
        delete it->second;
        behaviors_.erase(it);
        LOG_INFO("module", "Combat unregistered for {}", bot->GetName());
    }
}

void BotCombatManager::UpdateAllCombat(uint32 diff) {
    // CRITICAL FIX: Copy to vector to prevent race condition
    std::vector<CombatBehavior*> toUpdate;
    toUpdate.reserve(behaviors_.size());
    
    for (auto& pair : behaviors_) {
        if (pair.second) {
            toUpdate.push_back(pair.second);
        }
    }
    
    // Now safe to update without iterator invalidation
    for (CombatBehavior* behavior : toUpdate) {
        if (behavior && behavior->GetBot() && behavior->GetBot()->IsInWorld()) {
            behavior->Update(diff);
        }
    }
}

CombatBehavior* BotCombatManager::GetBehavior(Creature* bot) {
    if (!bot) return nullptr;
    
    auto it = behaviors_.find(bot->GetGUID().GetCounter());
    return (it != behaviors_.end()) ? it->second : nullptr;
}

} // namespace BotAI
