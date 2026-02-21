#include "BotAI.h"
#include "BotConstants.h"
#include "Map.h"
#include "Creature.h"
#include "Player.h"
#include "Group.h"
#include "ObjectAccessor.h"
#include "Log.h"
#include <cmath>

namespace BotAI {

// ============================================================================
// BOT CONTROLLER IMPLEMENTATION
// ============================================================================

BotController::BotController(Creature* bot, Player* owner, BotRole role)
    : bot_(bot), owner_(owner), role_(role), state_(BotState::IDLE),
      tankState_(TankState::WAITING_FOR_GROUP),
      healerState_(HealerState::READY),
      dpsState_(DpsState::READY)
{
    // Set role-appropriate follow distances
    if (role == BotRole::TANK) {
        followDistance_ = Constants::TANK_FOLLOW_DISTANCE;
    } else if (role == BotRole::HEALER) {
        followDistance_ = Constants::HEALER_FOLLOW_DISTANCE;
    } else {
        followDistance_ = Constants::DPS_FOLLOW_DISTANCE;
    }
    
    maxFollowDistance_ = Constants::MAX_FOLLOW_DISTANCE;
}

BotController::~BotController() {
}

// ============================================================================
// PHASE 1: STABILIZATION - CanUpdateAI() GATE
// ============================================================================

bool BotController::CanUpdateAI() {
    // GATE 1: Must be in world
    if (!bot_ || !bot_->IsInWorld()) {
        return false;
    }
    
    // GATE 2: Owner must exist and be valid (CRITICAL FIX: added IsInWorld check)
    if (!owner_ || !owner_->IsInWorld()) {
        return false;
    }
    
    // GATE 3: Not currently teleporting (debounce)
    if (teleportStartTime_ != 0) {
        uint32 now = getMSTime();
        if ((now - teleportStartTime_) < Constants::TELEPORT_DEBOUNCE_MS) {
            return false;
        }
        teleportStartTime_ = 0;
    }
    
    return true;
}

// ============================================================================
// PHASE 1: STABILIZATION - Activity Bubble
// ============================================================================

BotActivityState BotController::GetActivityState() const {
    if (!bot_ || !owner_) {
        return BotActivityState::DORMANT;
    }
    
    // FULL_ACTIVE: In instance
    if (IsInDungeon()) {
        return BotActivityState::FULL_ACTIVE;
    }
    
    // FULL_ACTIVE: In combat
    if (bot_->IsInCombat()) {
        return BotActivityState::FULL_ACTIVE;
    }
    
    // FULL_ACTIVE: Close to owner
    if (owner_->IsInWorld()) {
        float distToOwner = bot_->GetDistance(owner_);
        if (distToOwner < 50.0f) {
            return BotActivityState::FULL_ACTIVE;
        }
        
        // LIGHT_ACTIVE: Within grouping range but not close
        if (distToOwner < 100.0f) {
            return BotActivityState::LIGHT_ACTIVE;
        }
    }
    
    // DORMANT: Far away and not doing anything
    return BotActivityState::DORMANT;
}

// ============================================================================
// PHASE 1: STABILIZATION - Fall Recovery
// ============================================================================

void BotController::HandleFallRecovery(uint32 diff) {
    if (!bot_) return;
    
    float groundZ = bot_->GetMap()->GetHeight(0, bot_->GetPositionX(), bot_->GetPositionY(), bot_->GetPositionZ());
    bool isGrounded = (groundZ != INVALID_HEIGHT && std::fabs(bot_->GetPositionZ() - groundZ) < 1.0f);
    
    if (isGrounded) {
        // On ground: record position for recovery later
        if (fallStartTime_ != 0) {
            fallStartTime_ = 0;
        }
        positionHistory_.Record(bot_->GetPosition());
        return;
    }
    
    // Not grounded: check if falling too long
    if (fallStartTime_ == 0) {
        fallStartTime_ = getMSTime();
        return;
    }
    
    uint32 fallDuration = getMSTime() - fallStartTime_;
    
    if (fallDuration > Constants::FALL_RECOVERY_THRESHOLD_MS) {
        // Falling for more than threshold - attempt recovery
        LOG_WARN("module", "Bot {} falling for {}ms, attempting recovery", 
                 bot_->GetName(), fallDuration);
        
        // Try to find ground below current position
        float groundZ = bot_->GetMap()->GetHeight(0, bot_->GetPositionX(), bot_->GetPositionY(), bot_->GetPositionZ());
        
        if (groundZ != INVALID_HEIGHT && !std::isnan(groundZ)) {
            float heightDiff = std::fabs(bot_->GetPositionZ() - groundZ);
            
            if (heightDiff < Constants::FALL_HEIGHT_THRESHOLD) {
                // Ground is close, snap to it
                LOG_INFO("module", "Teleporting {} to ground (Z: {} -> {})", 
                         bot_->GetName(), bot_->GetPositionZ(), groundZ);
                bot_->NearTeleportTo(bot_->GetPositionX(), bot_->GetPositionY(), groundZ, 0);
                fallStartTime_ = 0;
                teleportStartTime_ = getMSTime();
                return;
            }
        }
        
        // Ground lookup failed, try last known good position
        if (positionHistory_.HasHistory()) {
            Position lastGood = positionHistory_.GetLastGood();
            LOG_INFO("module", "Teleporting {} to last good position", bot_->GetName());
            bot_->NearTeleportTo(lastGood.GetPositionX(), lastGood.GetPositionY(), lastGood.GetPositionZ(), 0);
            fallStartTime_ = 0;
            teleportStartTime_ = getMSTime();
            return;
        }
        
        // Last resort: snap to owner (CRITICAL: added IsInWorld check)
        if (owner_ && owner_->IsInWorld()) {
            LOG_WARN("module", "Teleporting {} to owner (emergency)", bot_->GetName());
            bot_->NearTeleportTo(owner_->GetPositionX(), owner_->GetPositionY(), owner_->GetPositionZ(), 0);
            fallStartTime_ = 0;
            teleportStartTime_ = getMSTime();
            return;
        }
    }
}

// ============================================================================
// PHASE 1: STABILIZATION - Movement Intent Throttling
// ============================================================================

void BotController::IssueMoveIfNeeded(const Position& dest, MovementIntent::Type intent) {
    if (!bot_) return;
    
    if (currentMovement_.ShouldReissue(dest)) {
        bot_->GetMotionMaster()->MovePoint(0, dest);
        currentMovement_.type = intent;
        currentMovement_.target = dest;
        currentMovement_.lastIssuedTime = getMSTime();
        
        if constexpr (Constants::ENABLE_DEBUG_LOGGING) {
            LOG_DEBUG("module", "Issued move for {} to ({}, {}, {})",
                      bot_->GetName(), dest.GetPositionX(), dest.GetPositionY(), dest.GetPositionZ());
        }
    }
}

// ============================================================================
// PHASE 1: STABILIZATION - Generic Regen Handler
// ============================================================================

void BotController::HandleRegen(uint32 diff) {
    if (!bot_) return;
    // Natural regen handled by core - no special logic needed
}

// ============================================================================
// MAIN UPDATE LOOP - NOW WITH GATES
// ============================================================================

void BotController::Update(uint32 diff) {
    // GATE 1: Can we think at all?
    if (!CanUpdateAI()) {
        HandleRegen(diff);
        return;
    }
    
    // GATE 2: What activity level?
    BotActivityState activity = GetActivityState();
    
    if (activity == BotActivityState::DORMANT) {
        // Just regen, don't do anything else
        HandleRegen(diff);
        HandleFallRecovery(diff);
        return;
    }
    
    // GATE 3: Handle critical systems
    HandleFallRecovery(diff);
    
    // Now we can do normal thinking for LIGHT and FULL
    if (thinkTimer_ > diff) {
        thinkTimer_ -= diff;
    } else {
        Think();
        thinkTimer_ = Constants::THINK_INTERVAL_MS;
    }
    
    ExecuteAction();
}

void BotController::Think() {
    if (!bot_ || !owner_ || !owner_->IsInWorld()) return;
    
    // Decision making
    if (bot_->IsInCombat()) {
        SetState(BotState::COMBAT);
    } else if (GetDistanceToOwner() > maxFollowDistance_) {
        // Too far, teleport back
        bot_->NearTeleportTo(owner_->GetPositionX(), owner_->GetPositionY(), owner_->GetPositionZ(), 0);
        teleportStartTime_ = getMSTime();
        SetState(BotState::FOLLOWING);
    } else if (GetDistanceToOwner() > followDistance_ + 5.0f) {
        SetState(BotState::FOLLOWING);
    } else {
        SetState(BotState::IDLE);
    }
}

void BotController::ExecuteAction() {
    if (!bot_ || !owner_ || !owner_->IsInWorld()) return;
    
    switch (state_) {
        case BotState::FOLLOWING:
            FollowOwner();
            break;
        case BotState::COMBAT:
            UpdateCombat(0);
            break;
        case BotState::DRINKING:
            CheckDrinking();
            break;
        default:
            break;
    }
}

void BotController::FollowOwner() {
    if (!bot_ || !owner_ || !owner_->IsInWorld()) return;
    
    float distance = GetDistanceToOwner();
    
    // Teleport back if way too far
    if (distance > maxFollowDistance_) {
        bot_->NearTeleportTo(owner_->GetPositionX(), owner_->GetPositionY(), owner_->GetPositionZ(), 0);
        teleportStartTime_ = getMSTime();
        return;
    }
    
    // Normal follow
    if (distance > followDistance_ + 5.0f) {
        IssueMoveIfNeeded(owner_->GetPosition(), MovementIntent::Type::FOLLOW);
    }
}

void BotController::UpdateCombat(uint32 diff) {
    // Handled by BotCombatSystem
}

void BotController::Attack(Unit* target) {
    if (!bot_ || !target) return;
    bot_->Attack(target, false);
}

Unit* BotController::GetOwnerTarget() {
    if (!owner_ || !owner_->IsInWorld()) return nullptr;
    return owner_->GetSelectedUnit();
}

bool BotController::NeedsDrink() {
    if (!bot_) return false;
    
    float manaPct = GetManaPercent();
    return manaPct < Constants::DRINK_MANA_PCT;
}

bool BotController::IsInDungeonOrRaid() {
    if (!bot_) return false;
    return IsInDungeon();
}

float BotController::GetDistanceToOwner() {
    if (!bot_ || !owner_ || !owner_->IsInWorld()) return 999.0f;
    return bot_->GetDistance(owner_);
}

void BotController::UpdateTank(uint32 diff) {
    // Role-specific logic
}

void BotController::UpdateHealer(uint32 diff) {
    // Role-specific logic
}

void BotController::UpdateDps(uint32 diff) {
    // Role-specific logic
}

void BotController::SetState(BotState newState) {
    if (state_ != newState) {
        if constexpr (Constants::ENABLE_DEBUG_LOGGING) {
            LOG_DEBUG("module", "Bot {} state: {} -> {}", 
                      bot_->GetName(),
                      static_cast<int>(state_),
                      static_cast<int>(newState));
        }
        state_ = newState;
    }
}

float BotController::GetManaPercent() const {
    if (!bot_) return 0.0f;
    uint32 maxMana = bot_->GetMaxPower(POWER_MANA);
    if (maxMana == 0) return 100.0f;
    return (float)bot_->GetPower(POWER_MANA) / (float)maxMana * 100.0f;
}

bool BotController::IsReady() const {
    if (!bot_) return false;
    return bot_->IsAlive() && !bot_->IsInCombat();
}

std::string BotController::GetStateString() const {
    switch (state_) {
        case BotState::IDLE: return "IDLE";
        case BotState::FOLLOWING: return "FOLLOWING";
        case BotState::COMBAT: return "COMBAT";
        case BotState::DRINKING: return "DRINKING";
        case BotState::DEAD: return "DEAD";
        default: return "UNKNOWN";
    }
}

std::string BotController::GetClassString() const {
    switch (role_) {
        case BotRole::TANK: return "TANK";
        case BotRole::HEALER: return "HEALER";
        case BotRole::DPS: return "DPS";
        default: return "UNKNOWN";
    }
}

void BotController::UpdateDungeonRoute() {
    // Dungeon-specific logic - placeholder for navigation system
}

bool BotController::IsInDungeon() const {
    if (!bot_) return false;
    Map* map = bot_->GetMap();
    return map && map->IsDungeon();
}

void BotController::CheckDrinking() {
    // Drinking logic - placeholder
}

void BotController::ReportStatus() {
    if (!bot_) return;
    LOG_INFO("module", "{} ({}) - State: {}, HP: {}%, Mana: {}%",
             bot_->GetName(), GetClassString(),
             GetStateString(),
             bot_->GetHealthPct(),
             GetManaPercent());
}

bool BotController::IsGroupReady() {
    if (!owner_ || !owner_->IsInWorld()) return false;
    
    if (Group* group = owner_->GetGroup()) {
        // Check if all group members are ready
        return true;  // Simplified for now
    }
    
    return true;  // Solo is always ready
}

// ============================================================================
// BOT CONTROLLER MANAGER IMPLEMENTATION
// ============================================================================

BotControllerManager* BotControllerManager::instance_ = nullptr;

BotControllerManager* BotControllerManager::instance() {
    if (!instance_) {
        instance_ = new BotControllerManager();
    }
    return instance_;
}

void BotControllerManager::RegisterBot(Creature* bot, Player* owner, BotRole role) {
    if (!bot) return;
    
    // CRITICAL FIX: Delete old controller if exists (prevent memory leak)
    auto it = controllers_.find(bot);
    if (it != controllers_.end()) {
        LOG_WARN("module", "Bot {} already registered, replacing controller", bot->GetName());
        delete it->second;
        controllers_.erase(it);
    }
    
    BotController* controller = new BotController(bot, owner, role);
    controllers_[bot] = controller;
    
    LOG_INFO("module", "✓ Controller registered for {}", bot->GetName());
}

void BotControllerManager::UnregisterBot(Creature* bot) {
    if (!bot) return;
    
    auto it = controllers_.find(bot);
    if (it != controllers_.end()) {
        delete it->second;
        controllers_.erase(it);
        LOG_INFO("module", "✓ Controller unregistered for {}", bot->GetName());
    }
}

BotController* BotControllerManager::GetController(Creature* bot) {
    if (!bot) return nullptr;
    
    auto it = controllers_.find(bot);
    if (it != controllers_.end()) {
        return it->second;
    }
    
    return nullptr;
}

void BotControllerManager::UpdateAllBots(uint32 diff) {
    // CRITICAL FIX: Copy to vector to prevent iterator invalidation
    std::vector<BotController*> toUpdate;
    toUpdate.reserve(controllers_.size());

    for (auto const& pair : controllers_) {
        if (pair.second) {
            toUpdate.push_back(pair.second);
        }
    }

    // Safe to update without iterator invalidation
    for (BotController* controller : toUpdate) {
        if (controller && controller->GetCreature() && controller->GetCreature()->IsInWorld()) {
            controller->Update(diff);
        }
    }
}

} // namespace BotAI
