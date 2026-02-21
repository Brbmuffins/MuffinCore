# ============================================================================
# MUFFIN COMPANIONS - Bot AI Module for AzerothCore
# ============================================================================
# Version: 2.0
# Compatible with: AzerothCore 3.3.5a (WotLK)
# License: MIT
# ============================================================================

AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/loader.h")

AC_ADD_SCRIPT_LOADER("BotAI" "${CMAKE_CURRENT_LIST_DIR}/src/loader.h")

# ============================================================================
# SOURCE FILES
# ============================================================================

CU_ADD_SOURCES(
    # Core AI System
    "${CMAKE_CURRENT_LIST_DIR}/src/BotAI.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/BotCombatSystem.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/QuickBotSpawner.cpp"
    
    # Feature Systems
    "${CMAKE_CURRENT_LIST_DIR}/src/BotPersonality.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/DungeonRoutes.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/RealPlayerBotFactory.cpp"
    
    # Command Handler
    "${CMAKE_CURRENT_LIST_DIR}/src/mod_bot_ai.cpp"
)

# ============================================================================
# HEADER FILES
# ============================================================================

CU_ADD_HEADERS(
    # Core Headers
    "${CMAKE_CURRENT_LIST_DIR}/src/BotAI.h"
    "${CMAKE_CURRENT_LIST_DIR}/src/BotCombatSystem.h"
    "${CMAKE_CURRENT_LIST_DIR}/src/BotSpawner.h"
    "${CMAKE_CURRENT_LIST_DIR}/src/BotConstants.h"
    
    # Feature Headers
    "${CMAKE_CURRENT_LIST_DIR}/src/BotPersonality.h"
    "${CMAKE_CURRENT_LIST_DIR}/src/BotGovernor.h"
    "${CMAKE_CURRENT_LIST_DIR}/src/DungeonRoutes.h"
    "${CMAKE_CURRENT_LIST_DIR}/src/RealPlayerBotFactory.h"
    
    # Loader
    "${CMAKE_CURRENT_LIST_DIR}/src/loader.h"
)

# ============================================================================
# MODULE INFORMATION
# ============================================================================

message(STATUS "")
message(STATUS "========================================")
message(STATUS "  MUFFIN COMPANIONS Bot AI Module")
message(STATUS "  Version 2.0 - Production Ready")
message(STATUS "========================================")
message(STATUS "  Features:")
message(STATUS "    • Intelligent Combat AI")
message(STATUS "    • Tank/Healer/DPS Roles")
message(STATUS "    • Auto-Resurrection")
message(STATUS "    • Leash System")
message(STATUS "    • 70 Dungeon Routes")
message(STATUS "    • Dr Steve Brule!")
message(STATUS "========================================")
message(STATUS "")
