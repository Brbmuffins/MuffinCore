#ifndef _BOT_AI_H
#define _BOT_AI_H

#include "Player.h"
#include "Creature.h"
#include "ScriptMgr.h"
#include <map>
#include <vector>
#include <deque>

namespace BotAI {

// ============================================================================
// ENUMS AND STATE DEFINITIONS
// ============================================================================

enum class BotState {
    IDLE,
    FOLLOWING,
    COMBAT,
    DRINKING,
    DEAD
};

enum class BotRole {
    TANK,
    HEALER,
    DPS
};

enum class BotActivityState {
    DORMANT,        // Not active, minimal updates
    LIGHT_ACTIVE,   // Player nearby, basic responses
    FULL_ACTIVE     // In instance, in combat, or grouped
};

enum class TankState {
    WAITING_FOR_GROUP,
    READY_TO_PULL,
    TANKING,
    HOLDING_THREAT,
    REPOSITIONING
};

enum class HealerState {
    READY,
    HEALING,
    OUT_OF_MANA,
    MANA_CHECK,
    DRINKING
};

enum class DpsState {
    READY,
    ATTACKING,
    WAITING_FOR_THREAT
};

// ============================================================================
// STABILIZATION STRUCTURES (Phase 1)
// ============================================================================

struct MovementIntent {
    enum Type {
        NONE,
        FOLLOW,
        HOLD,
        REPOSITION,
        EVADE,
        RETURN
    } type = NONE;
    
    Position target;
    uint32 lastIssuedTime = 0;
    
    bool ShouldReissue(const Position& newTarget) const {
        if (type == NONE) return true;  // First move
        
        uint32 now = getMSTime();
        if (now < lastIssuedTime) return true;  // Clock wrapped
        
        uint32 elapsed = now - lastIssuedTime;
        if (elapsed < 500) return false;  // Rate limit: only one move per 500ms
        
        float dx = target.GetPositionX() - newTarget.GetPositionX(); float dy = target.GetPositionY() - newTarget.GetPositionY(); if (sqrt(dx*dx + dy*dy) > 2.0f) return true;  // Dest changed significantly
        
        return false;
    }
};

struct PositionHistory {
    static constexpr int HISTORY_SIZE = 20;
    
    struct Entry {
        Position pos;
        uint32 timestamp;
    };
    
    std::deque<Entry> history;
    
    void Record(const Position& pos) {
        history.push_back({pos, getMSTime()});
        if (history.size() > HISTORY_SIZE) {
            history.pop_front();
        }
    }
    
    Position GetLastGood() const {
        if (history.empty()) return Position();
        return history.back().pos;
    }
    
    bool HasHistory() const { return !history.empty(); }
};

// ============================================================================
// BOT CONTROLLER - Main AI Loop
// ============================================================================

class BotController {
public:
    BotController(Creature* bot, Player* owner, BotRole role);
    ~BotController();
    
    void Update(uint32 diff);
    void SetState(BotState newState);
    
    // Getters
    Creature* GetCreature() { return bot_; }
    Player* GetOwner() { return owner_; }
    
    // Polish: State query methods
    BotState GetState() const { return state_; }
    BotRole GetRole() const { return role_; }
    float GetHealthPercent() const { return bot_ ? bot_->GetHealthPct() : 0.0f; }
    float GetManaPercent() const;
    bool IsReady() const;
    std::string GetStateString() const;
    std::string GetClassString() const;
    
    // Dungeon systems
    void UpdateDungeonRoute();
    bool IsInDungeon() const;
    
    // Polish features
    void CheckDrinking();
    void ReportStatus();
    bool IsGroupReady();
    
    // PHASE 1: Stabilization methods
    bool CanUpdateAI();
    BotActivityState GetActivityState() const;

private:
    // Core AI methods
    void Think();
    void ExecuteAction();
    void FollowOwner();
    void UpdateCombat(uint32 diff);
    void Attack(Unit* target);
    Unit* GetOwnerTarget();
    bool NeedsDrink();
    bool IsInDungeonOrRaid();
    float GetDistanceToOwner();
    void UpdateTank(uint32 diff);
    void UpdateHealer(uint32 diff);
    void UpdateDps(uint32 diff);
    
    // PHASE 1: Stabilization methods
    void HandleRegen(uint32 diff);
    void HandleFallRecovery(uint32 diff);
    void IssueMoveIfNeeded(const Position& dest, MovementIntent::Type intent);
    
    // Data
    Creature* bot_;
    Player* owner_;
    BotRole role_;
    BotState state_;
    
    uint32 thinkTimer_ = 0;
    uint32 actionTimer_ = 0;
    uint32 globalCooldown_ = 0;
    
    TankState tankState_;
    HealerState healerState_;
    DpsState dpsState_;
    
    Unit* currentTarget_ = nullptr;
    float threatLevel_ = 0.0f;
    float followDistance_ = 8.0f;
    float maxFollowDistance_ = 50.0f;
    
    // PHASE 1: Stabilization data
    MovementIntent currentMovement_;
    PositionHistory positionHistory_;
    uint32 fallStartTime_ = 0;
    uint32 teleportStartTime_ = 0;
};

// ============================================================================
// MANAGER FOR ALL BOT CONTROLLERS
// ============================================================================

class BotControllerManager {
public:
    static BotControllerManager* instance();
    
    void RegisterBot(Creature* bot, Player* owner, BotRole role);
    void UnregisterBot(Creature* bot);
    BotController* GetController(Creature* bot);
    void UpdateAllBots(uint32 diff);

private:
    BotControllerManager() {}
    static BotControllerManager* instance_;
    std::map<Creature*, BotController*> controllers_;
};

} // namespace BotAI

#endif
