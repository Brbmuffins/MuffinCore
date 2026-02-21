#ifndef _BOT_CONSTANTS_H
#define _BOT_CONSTANTS_H

#include "Define.h"

namespace BotAI {
namespace Constants {

// ============================================================================
// DISTANCE THRESHOLDS
// ============================================================================
constexpr float LEASH_DISTANCE = 30.0f;              // Max chase distance from owner
constexpr float MAX_FOLLOW_DISTANCE = 100.0f;        // Teleport if further than this
constexpr float TELEPORT_THRESHOLD = 100.0f;         // Emergency teleport distance
constexpr float DUNGEON_ATTACK_RANGE = 40.0f;        // Max attack range in dungeons
constexpr float FALL_HEIGHT_THRESHOLD = 50.0f;      // Consider fall recovery

// ============================================================================
// COMBAT POSITIONING
// ============================================================================
constexpr float TANK_FOLLOW_DISTANCE = 5.0f;         // Tanks stay close
constexpr float HEALER_FOLLOW_DISTANCE = 15.0f;      // Healers stay back
constexpr float DPS_FOLLOW_DISTANCE = 12.0f;         // DPS medium range
constexpr float MELEE_RANGE = 5.0f;                  // Melee attack range

// DPS Positioning
constexpr float DPS_MIN_RANGE = 10.0f;               // Too close - back up
constexpr float DPS_MAX_RANGE = 25.0f;               // Too far - move in
constexpr float DPS_IDEAL_RANGE = 15.0f;             // Perfect position

// Healer Positioning
constexpr float HEALER_MIN_RANGE = 15.0f;            // Too close - back up
constexpr float HEALER_MAX_RANGE = 30.0f;            // Too far - move in
constexpr float HEALER_IDEAL_RANGE = 20.0f;          // Perfect position

// ============================================================================
// HEALING THRESHOLDS (Percent)
// ============================================================================
constexpr float HEAL_EMERGENCY_PCT = 40.0f;          // Big emergency heal
constexpr float HEAL_MEDIUM_PCT = 60.0f;             // Medium heal
constexpr float HEAL_TOPOFF_PCT = 85.0f;             // Small top-off heal
constexpr float HEAL_OWNER_PCT = 95.0f;              // Heal owner above this
constexpr float HEAL_OTHER_BOTS_PCT = 70.0f;         // Heal bots above this
constexpr float HEAL_SELF_PCT = 60.0f;               // Heal self above this
constexpr float LOW_MANA_PCT = 10.0f;                // Consider OOM
constexpr float DRINK_MANA_PCT = 40.0f;              // Start drinking

// ============================================================================
// TIMERS (Milliseconds)
// ============================================================================
constexpr uint32 GLOBAL_COOLDOWN_MS = 500;           // GCD between spells
constexpr uint32 THINK_INTERVAL_MS = 1000;           // AI decision making
constexpr uint32 ROTATION_INTERVAL_MS = 500;         // Combat rotation
constexpr uint32 HEAL_ROTATION_MS = 100;             // Healer check frequency
constexpr uint32 EMERGENCY_HEAL_CD_MS = 3000;        // After emergency heal
constexpr uint32 MEDIUM_HEAL_CD_MS = 2500;           // After medium heal
constexpr uint32 TOPOFF_HEAL_CD_MS = 2000;           // After small heal
constexpr uint32 TELEPORT_DEBOUNCE_MS = 100;         // Prevent teleport spam
constexpr uint32 FALL_RECOVERY_THRESHOLD_MS = 500;   // Falling too long
constexpr uint32 MOVEMENT_REISSUE_MS = 500;          // Movement command throttle
constexpr uint32 RESURRECTION_CHECK_MS = 2000;       // Check for resurrection
constexpr uint32 BUFF_CHECK_MS = 30000;              // Check buffs every 30s
constexpr uint32 INTERRUPT_CHECK_MS = 100;           // Check for interrupts

// ============================================================================
// FORMATION CONSTANTS
// ============================================================================
constexpr float FORMATION_SPACING = 2.0f;            // Space between bots
constexpr float FORMATION_ROW_DEPTH = 2.0f;          // Depth per row
constexpr uint32 FORMATION_BOTS_PER_ROW = 3;         // Bots per row
constexpr float FORMATION_ARC = 3.14159f;            // PI radians (semicircle)

// ============================================================================
// LIMITS AND CONSTRAINTS
// ============================================================================
constexpr uint32 MAX_BOTS_PER_PLAYER = 25;           // Hard limit
constexpr uint32 DUNGEON_GROUP_SIZE = 5;             // Standard 5-man
constexpr uint32 RAID_10_SIZE = 10;                  // 10-man raid
constexpr uint32 RAID_25_SIZE = 25;                  // 25-man raid
constexpr uint32 MAX_HEAL_TARGET_RANGE = 40.0f;      // Max heal range
constexpr uint32 MAX_DPS_TARGET_RANGE = 30.0f;       // Max DPS range
constexpr uint32 MAX_TAUNT_RANGE = 30.0f;            // Max taunt range

// ============================================================================
// THREAT MANAGEMENT
// ============================================================================
constexpr float TANK_THREAT_MULTIPLIER = 3.0f;       // Tank damage → threat
constexpr float HEALER_THREAT_MULTIPLIER = 0.5f;     // Reduce healer threat
constexpr float DPS_THREAT_MULTIPLIER = 1.0f;        // Normal DPS threat
constexpr float TAUNT_THREAT_AMOUNT = 1000.0f;       // Taunt threat bonus

// ============================================================================
// LEVEL SCALING
// ============================================================================
constexpr float TANK_HP_MULTIPLIER = 8.0f;           // Tank HP scaling
constexpr float HEALER_HP_MULTIPLIER = 6.0f;         // Healer HP scaling
constexpr float DPS_HP_MULTIPLIER = 5.0f;            // DPS HP scaling
constexpr float HEALER_MANA_MULTIPLIER = 5.0f;       // Healer mana pool
constexpr float DPS_MANA_MULTIPLIER = 3.0f;          // DPS mana pool
constexpr float TANK_MANA_MULTIPLIER = 2.0f;         // Tank mana pool
constexpr uint32 BASE_MANA_PER_LEVEL = 100;          // Mana gained per level
constexpr uint32 BASE_MANA_START = 1000;             // Starting mana

// ============================================================================
// DAMAGE SCALING
// ============================================================================
constexpr uint32 TANK_BASE_DAMAGE = 20;              // Tank base damage
constexpr uint32 TANK_DAMAGE_PER_LEVEL = 1;          // Tank damage/level
constexpr uint32 DPS_BASE_DAMAGE = 50;               // DPS base damage
constexpr uint32 DPS_DAMAGE_PER_LEVEL = 2;           // DPS damage/level
constexpr uint32 DPS_FIREBLAST_BASE = 30;            // Fire blast base
constexpr uint32 DPS_FIREBLAST_PER_LEVEL = 1;        // Fire blast/level

// ============================================================================
// RESURRECTION SYSTEM
// ============================================================================
constexpr float RESURRECTION_HEALTH_PCT = 50.0f;     // Resurrect with 50% HP
constexpr float RESURRECTION_MANA_PCT = 20.0f;       // Resurrect with 20% mana
constexpr uint32 RESURRECTION_DELAY_MS = 1000;       // Delay after res

// ============================================================================
// PERSONALITY SYSTEM
// ============================================================================
constexpr uint32 CHAT_GLOBAL_COOLDOWN_MS = 2000;     // Between any bot talking
constexpr uint32 CHAT_CONTEXT_COOLDOWN_MS = 5000;    // Same context cooldown
constexpr float BRULE_TALKATIVENESS = 0.8f;          // Dr Steve is chatty
constexpr float SERIOUS_TALKATIVENESS = 0.3f;        // Professional bots
constexpr float SARCASTIC_TALKATIVENESS = 0.7f;      // Witty bots
constexpr float QUIET_TALKATIVENESS = 0.1f;          // Silent bots
constexpr float RIGHTEOUS_TALKATIVENESS = 0.6f;      // Noble bots

// ============================================================================
// BUFF DURATIONS
// ============================================================================
constexpr uint32 BUFF_DURATION_MINUTES = 60;         // Standard buff duration
constexpr uint32 BUFF_REFRESH_THRESHOLD_MS = 300000; // Refresh at 5 min left

// ============================================================================
// INTERRUPT SYSTEM
// ============================================================================
constexpr float INTERRUPT_PRIORITY_THRESHOLD = 0.5f; // Cast time to interrupt
constexpr uint32 INTERRUPT_COOLDOWN_MS = 1000;       // Between interrupts

// ============================================================================
// LOOT SYSTEM
// ============================================================================
constexpr uint32 LOOT_ROLL_DELAY_MS = 2000;          // Delay before rolling
constexpr float NEED_ROLL_CHANCE = 0.7f;             // Chance to need
constexpr float GREED_ROLL_CHANCE = 0.3f;            // Chance to greed

// ============================================================================
// PERFORMANCE TUNING
// ============================================================================
constexpr uint32 POSITION_HISTORY_SIZE = 20;         // Fall recovery history
constexpr uint32 METRIC_UPDATE_INTERVAL_MS = 5000;   // Performance metrics
constexpr bool ENABLE_DEBUG_LOGGING = false;         // Toggle debug logs
constexpr bool ENABLE_VERBOSE_COMBAT = false;        // Combat logging

} // namespace Constants
} // namespace BotAI

#endif // _BOT_CONSTANTS_H
