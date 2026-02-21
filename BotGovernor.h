#ifndef _BOT_GOVERNOR_H
#define _BOT_GOVERNOR_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <chrono>
#include <atomic>
#include <mutex>

// Forward declarations for AzerothCore types
class Player;
class Unit;
class Group;

namespace BotAI {

// =============================================================================
// ENUMS AND CONSTANTS
// =============================================================================

enum class GovernorRole {
    TANK,
    HEALER,
    DPS_MELEE,
    DPS_RANGED
};

enum class PullStyle {
    NORMAL,      // Standard pull
    LOS_PULL,    // Line-of-sight pull (casters)
    BODY_PULL,   // Tank body-pulls
    SKIP         // Skip this pack
};

enum class SafeMode {
    NORMAL,      // All systems operational
    DEGRADED,    // LLM disabled, tactics slower
    EMERGENCY    // Minimal brain, survival only
};

enum class ActionType {
    // Safe actions (always allowed)
    SET_PULL_GATE,
    SET_INTERRUPT_ORDER,
    ADJUST_TEMPO,
    MARK_PACK_DANGEROUS,
    CHAT_MESSAGE,
    
    // Risky actions (require special validation)
    SPAWN_CREATURE,
    MODIFY_ENCOUNTER,
    CHANGE_ROUTE,
    
    // Forbidden (never allowed from LLM)
    CAST_SPELL_DIRECT,
    TELEPORT_PLAYER,
    GIVE_ITEM,
    MODIFY_STATS
};

// =============================================================================
// DATA STRUCTURES
// =============================================================================

struct GovernorProfile {
    std::string name;
    GovernorRole role;
    
    // Reaction timing (makes bots feel human)
    uint32 reaction_delay_min_ms = 150;
    uint32 reaction_delay_max_ms = 350;
    
    // Decision making
    float risk_tolerance = 0.5f;      // 0=ultra safe, 1=yolo
    float greediness = 0.5f;          // Finish casts vs move early
    float kick_reliability = 0.9f;    // Sometimes miss interrupts
    
    // Personality
    float talkativeness = 0.3f;       // How often to chat
    std::string voice_style = "brule"; // Personality voice
    
    // Combat preferences
    std::map<std::string, float> spell_preferences; // Spell weights
};

struct GroupBlackboard {
    // Route and pull state
    uint32 current_route_step = 0;
    uint32 next_pack_id = 0;
    
    // Pull gates (when it's safe to engage)
    float required_mana_pct = 0.6f;
    bool require_all_buffed = true;
    float required_hp_pct = 0.8f;
    
    // Strategy
    PullStyle next_pull_style = PullStyle::NORMAL;
    std::vector<uint64> interrupt_rotation; // GUIDs in order
    std::vector<uint32> dispel_priorities;  // Spell IDs
    
    // Danger tracking
    float current_danger_level = 0.0f; // 0-1 scale
    uint32 predicted_spike_in_ms = 0;  // Healer warning
    
    // Learned memory
    std::set<uint32> dangerous_packs;
    std::map<uint32, std::string> pack_notes; // "LOS required", etc
    
    // Tempo control
    bool slow_mode = false;  // True after scary pull
    uint32 drink_time_ms = 0; // Extra wait time
};

struct SafeModeConfig {
    // Performance thresholds
    float min_tps = 50.0f;
    uint32 max_errors_per_min = 10;
    uint32 max_actions_per_min = 100;
    float memory_growth_mb_per_min = 50.0f;
    
    // Combat thresholds
    uint32 max_deaths_per_min = 3;
    float wipe_spiral_threshold = 0.7f; // Avg HP dropping
};

struct TelemetryData {
    std::chrono::steady_clock::time_point last_reset;
    
    // Performance
    float current_tps = 60.0f;
    uint32 errors_this_minute = 0;
    uint32 actions_this_minute = 0;
    
    // Combat
    uint32 deaths_this_minute = 0;
    float avg_group_hp = 1.0f;
    
    // Memory (approximate)
    size_t memory_mb = 0;
    size_t memory_growth_mb = 0;
};

struct LLMIntent {
    ActionType action;
    std::map<std::string, std::string> params;
    std::chrono::steady_clock::time_point timestamp;
};

struct ValidationResult {
    bool allowed = false;
    std::string reason;
    
    static ValidationResult Allow() { return {true, ""}; }
    static ValidationResult Deny(const std::string& why) { return {false, why}; }
};

// =============================================================================
// THE GOVERNOR - Stability Wall
// =============================================================================

class BotGovernor {
public:
    BotGovernor();
    ~BotGovernor();
    
    // === CORE VALIDATION ===
    ValidationResult ValidateIntent(const LLMIntent& intent);
    
    // === SAFE MODE MANAGEMENT ===
    void UpdateTelemetry(const TelemetryData& data);
    SafeMode GetCurrentMode() const { return current_mode_; }
    bool IsLLMEnabled() const { return current_mode_ == SafeMode::NORMAL; }
    
    // === RATE LIMITING ===
    bool CheckRateLimit(ActionType type);
    void RecordAction(ActionType type);
    
    // === CONFIGURATION ===
    void SetSafeModeConfig(const SafeModeConfig& config);
    const SafeModeConfig& GetConfig() const { return config_; }
    
    // === TELEMETRY ===
    TelemetryData GetTelemetry() const { return telemetry_; }
    void LogError(const std::string& error);
    
private:
    // Validation helpers
    bool IsActionAllowed(ActionType type) const;
    bool ValidateNumericParam(const std::string& key, const std::string& value, 
                             float min, float max) const;
    bool ValidateContext(const LLMIntent& intent) const;
    
    // Safe mode logic
    void CheckSafeMode();
    void EnterSafeMode(SafeMode mode, const std::string& reason);
    void TryExitSafeMode();
    
    // Rate limiting
    struct RateLimitEntry {
        std::chrono::steady_clock::time_point last_action;
        uint32 count_this_minute = 0;
    };
    std::map<ActionType, RateLimitEntry> rate_limits_;
    
    // State
    SafeMode current_mode_ = SafeMode::NORMAL;
    SafeModeConfig config_;
    TelemetryData telemetry_;
    
    std::chrono::steady_clock::time_point last_safe_mode_check_;
    std::chrono::steady_clock::time_point safe_mode_entered_at_;
    
    // Thread safety
    mutable std::mutex mutex_;
};

// =============================================================================
// REFLEX ENGINE - Fast Loop (100-200ms)
// =============================================================================

class BotReflexEngine {
public:
    BotReflexEngine(Player* bot, const GovernorProfile& profile);
    
    // Main update loop (called every ~100-200ms)
    void Update(uint32 diff_ms);
    
    // Combat reflexes
    void UpdateCombat();
    void UpdateMovement();
    void UpdateEmergencyDefensives();
    
    // Role-specific reflexes
    void UpdateTankReflexes();
    void UpdateHealerReflexes();
    void UpdateDPSReflexes();
    
private:
    Player* bot_;
    GovernorProfile profile_;
    
    // Reaction delay simulation
    uint32 GetReactionDelay() const;
    std::chrono::steady_clock::time_point last_action_time_;
};

// =============================================================================
// TACTICS ENGINE - Slow Loop (500-1000ms)
// =============================================================================

class BotTacticsEngine {
public:
    BotTacticsEngine(Group* group);
    
    // Main update (called every ~500-1000ms)
    void Update(uint32 diff_ms);
    
    // Pull management
    bool CanPullNext() const;
    void ExecutePull();
    
    // Target assignment
    void UpdateTargetPriorities();
    void AssignInterrupts();
    void AssignDispels();
    
    // Tempo control
    void AdjustTempo(bool slow_down);
    
    // Access blackboard
    GroupBlackboard& GetBlackboard() { return blackboard_; }
    const GroupBlackboard& GetBlackboard() const { return blackboard_; }
    
private:
    Group* group_;
    GroupBlackboard blackboard_;
    
    // Pull gating checks
    bool CheckManaGate() const;
    bool CheckBuffsGate() const;
    bool CheckHealthGate() const;
};

// =============================================================================
// DIRECTOR API - Communication Bridge
// =============================================================================

struct SnapshotData {
    // Group state
    uint32 group_id;
    std::vector<std::string> member_names;
    std::vector<float> member_hp_pct;
    std::vector<float> member_mana_pct;
    
    // Combat state
    bool in_combat;
    uint32 current_target_count;
    std::vector<std::string> active_buffs;
    std::vector<std::string> active_debuffs;
    
    // Route state
    uint32 current_route_step;
    uint32 next_pack_id;
    std::string dungeon_name;
    
    // Memory
    std::vector<uint32> recent_wipes; // Pack IDs
    std::map<uint32, std::string> learned_notes;
    
    // Telemetry
    float danger_level;
    SafeMode safe_mode;
};

class DirectorAPI {
public:
    DirectorAPI(BotGovernor* governor);
    
    // Snapshot generation (for LLM context)
    SnapshotData GenerateSnapshot(Group* group) const;
    
    // Intent application (from LLM)
    bool ApplyIntent(const LLMIntent& intent, Group* group);
    
    // Health check
    bool IsHealthy() const;
    
private:
    BotGovernor* governor_;
};

} // namespace BotAI

#endif // _BOT_GOVERNOR_H
