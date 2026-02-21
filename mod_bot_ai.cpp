#include "ScriptMgr.h"
#include "Player.h"
#include "Chat.h"
#include "BotAI.h"
#include "BotSpawner.h"
#include "BotCombatSystem.h"
#include "BotConstants.h"

using namespace Acore::ChatCommands;

class BotAIWorldScript : public WorldScript
{
public:
    BotAIWorldScript() : WorldScript("BotAIWorldScript") { }
    
    void OnStartup() override
    {
        LOG_INFO("module", "");
        LOG_INFO("module", "========================================");
        LOG_INFO("module", "   MUFFIN COMPANIONS - Bot AI Module");
        LOG_INFO("module", "  The Ultimate AI Bot System");
        LOG_INFO("module", "========================================");
        LOG_INFO("module", "");
        LOG_INFO("module", "Features:");
        LOG_INFO("module", "  ✓ Intelligent Combat AI");
        LOG_INFO("module", "  ✓ Smart Healing & Tanking");
        LOG_INFO("module", "  ✓ Auto-Resurrection");
        LOG_INFO("module", "  ✓ Leash System");
        LOG_INFO("module", "  ✓ Formation Control");
        LOG_INFO("module", "  ✓ Dr Steve Brule!");
        LOG_INFO("module", "");
        LOG_INFO("module", "Commands:");
        LOG_INFO("module", "  .mbot add <role>    - Spawn a bot");
        LOG_INFO("module", "  .mbot add brule     - Spawn Dr Steve!");
        LOG_INFO("module", "  .mbot group dungeon - 5-man group");
        LOG_INFO("module", "  .mbot group raid    - 25-man raid");
        LOG_INFO("module", "  .mbot remove all    - Dismiss all bots");
        LOG_INFO("module", "========================================");
        LOG_INFO("module", "");
        
        BotAI::BotSpawner::instance();
    }
    
    void OnUpdate(uint32 diff) override
    {
        BotAI::BotControllerManager::instance()->UpdateAllBots(diff);
        BotAI::BotCombatManager::instance()->UpdateAllCombat(diff);
    }
};

class BotAIPlayerScript : public PlayerScript
{
public:
    BotAIPlayerScript() : PlayerScript("BotAIPlayerScript") { }

    void OnLogout(Player* player) override
    {
        // Clean up all bots when player logs out
        BotAI::BotSpawner::instance()->RemoveAllBots(player);
    }
};

class mbot_commandscript : public CommandScript
{
public:
    mbot_commandscript() : CommandScript("mbot_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable mbotCommandTable =
        {
            { "add",    HandleMbotAddCommand,    SEC_GAMEMASTER, Console::No },
            { "list",   HandleMbotListCommand,   SEC_GAMEMASTER, Console::No },
            { "remove", HandleMbotRemoveCommand, SEC_GAMEMASTER, Console::No },
            { "group",  HandleMbotGroupCommand,  SEC_GAMEMASTER, Console::No },
        };

        static ChatCommandTable commandTable =
        {
            { "mbot", mbotCommandTable },
        };

        return commandTable;
    }

    static bool HandleMbotAddCommand(ChatHandler* handler, Optional<std::string> role)
    {
        if (!role)
        {
            handler->SendSysMessage("Usage: .mbot add <tank|healer|dps|brule>");
            return false;
        }

        Player* player = handler->GetPlayer();
        if (!player)
            return false;

        // Check bot limit
        auto existingBots = BotAI::BotSpawner::instance()->GetPlayerBots(player);
        if (existingBots.size() >= BotAI::Constants::MAX_BOTS_PER_PLAYER)
        {
            handler->PSendSysMessage("|cffff0000You have reached the maximum bot limit (%u)|r", 
                                    BotAI::Constants::MAX_BOTS_PER_PLAYER);
            return false;
        }

        BotAI::BotRole botRole = BotAI::BotRole::DPS;

        // SPECIAL: Dr Steve Brule!
        if (*role == "brule" || *role == "drsteve")
        {
            BotAI::BotProfile drSteve = {
                "Dr Steve",                   // Name
                BotAI::BotClass::WARRIOR,     // Warrior class
                1,                            // Human race
                0,                            // Male
                BotAI::BotRole::TANK,         // Tank role
                "brule"                       // Personality
            };
            
            BotAI::BotSpawner::instance()->SpawnBot(player, drSteve);
            handler->SendSysMessage("|cff00ff00Dr Steve Brule has joined the party!|r");
            handler->SendSysMessage("|cffffcc00\"Here we go ya dingus! For your health!\"|r");
            return true;
        }
        else if (*role == "tank")
        {
            botRole = BotAI::BotRole::TANK;
        }
        else if (*role == "healer")
        {
            botRole = BotAI::BotRole::HEALER;
        }
        else if (*role == "dps")
        {
            botRole = BotAI::BotRole::DPS;
        }
        else
        {
            handler->SendSysMessage("Invalid role. Use: tank, healer, dps, or brule");
            return false;
        }

        // Spawn random bot of requested role
        std::string botName = BotAI::BotSpawner::instance()->GetRandomBotName(botRole);
        BotAI::BotSpawner::instance()->SpawnBot(player, botName, botRole);
        
        const char* roleColor = botRole == BotAI::BotRole::TANK ? "|cff0070dd" :
                               botRole == BotAI::BotRole::HEALER ? "|cff00ff00" :
                               "|cffff0000";
        
        handler->PSendSysMessage("%s%s|r (%s) has joined your party!", 
                                roleColor, botName.c_str(), role->c_str());
        
        return true;
    }

    static bool HandleMbotListCommand(ChatHandler* handler)
    {
        Player* player = handler->GetPlayer();
        if (!player)
            return false;

        auto bots = BotAI::BotSpawner::instance()->GetPlayerBots(player);

        if (bots.empty())
        {
            handler->SendSysMessage("|cffccccccNo bots currently active.|r");
            return true;
        }

        handler->PSendSysMessage("|cff00ff00Active Bots (%u/%u):|r", 
                                (uint32)bots.size(), 
                                BotAI::Constants::MAX_BOTS_PER_PLAYER);
        
        for (auto bot : bots)
        {
            if (bot)
            {
                auto controller = BotAI::BotControllerManager::instance()->GetController(bot);
                if (controller)
                {
                    std::string roleStr = controller->GetClassString();
                    const char* roleColor = controller->GetRole() == BotAI::BotRole::TANK ? "|cff0070dd" :
                                           controller->GetRole() == BotAI::BotRole::HEALER ? "|cff00ff00" :
                                           "|cffff0000";
                    
                    handler->PSendSysMessage("  %s%s|r - %s (Lv%u) [HP: %.0f%%, Mana: %.0f%%]",
                                            roleColor,
                                            bot->GetName().c_str(),
                                            roleStr.c_str(),
                                            bot->GetLevel(),
                                            bot->GetHealthPct(),
                                            controller->GetManaPercent());
                }
                else
                {
                    handler->PSendSysMessage("  |cffcccccc%s|r (Lv%u)",
                                            bot->GetName().c_str(),
                                            bot->GetLevel());
                }
            }
        }

        return true;
    }

    static bool HandleMbotRemoveCommand(ChatHandler* handler, Optional<std::string> target)
    {
        Player* player = handler->GetPlayer();
        if (!player)
            return false;

        if (!target || *target != "all")
        {
            handler->SendSysMessage("Usage: .mbot remove all");
            return false;
        }

        auto bots = BotAI::BotSpawner::instance()->GetPlayerBots(player);
        uint32 count = bots.size();
        
        BotAI::BotSpawner::instance()->RemoveAllBots(player);
        handler->PSendSysMessage("|cffff0000All %u bots dismissed!|r", count);
        
        return true;
    }

    static bool HandleMbotGroupCommand(ChatHandler* handler, Optional<std::string> type)
    {
        Player* player = handler->GetPlayer();
        if (!player)
            return false;

        if (!type)
        {
            handler->SendSysMessage("Usage: .mbot group <dungeon|raid>");
            return false;
        }

        if (*type == "dungeon")
        {
            handler->SendSysMessage("|cff00ff00Assembling 5-man dungeon group...|r");

            // Spawn standard dungeon comp: 1 tank, 1 healer, 2 DPS
            std::string tank = BotAI::BotSpawner::instance()->GetRandomBotName(BotAI::BotRole::TANK);
            BotAI::BotSpawner::instance()->SpawnBot(player, tank, BotAI::BotRole::TANK);

            std::string healer = BotAI::BotSpawner::instance()->GetRandomBotName(BotAI::BotRole::HEALER);
            BotAI::BotSpawner::instance()->SpawnBot(player, healer, BotAI::BotRole::HEALER);

            for (int i = 0; i < 2; i++)
            {
                std::string dps = BotAI::BotSpawner::instance()->GetRandomBotName(BotAI::BotRole::DPS);
                BotAI::BotSpawner::instance()->SpawnBot(player, dps, BotAI::BotRole::DPS);
            }

            handler->SendSysMessage("|cff00ff00✓ Dungeon group ready! (1 Tank, 1 Healer, 2 DPS)|r");
        }
        else if (*type == "raid")
        {
            handler->SendSysMessage("|cffff8000Assembling 25-man raid...|r");

            // 25-man raid comp: 3 tanks, 6 healers, 16 DPS
            for (int i = 0; i < 3; i++)
            {
                std::string tank = BotAI::BotSpawner::instance()->GetRandomBotName(BotAI::BotRole::TANK);
                BotAI::BotSpawner::instance()->SpawnBot(player, tank, BotAI::BotRole::TANK);
            }

            for (int i = 0; i < 6; i++)
            {
                std::string healer = BotAI::BotSpawner::instance()->GetRandomBotName(BotAI::BotRole::HEALER);
                BotAI::BotSpawner::instance()->SpawnBot(player, healer, BotAI::BotRole::HEALER);
            }

            for (int i = 0; i < 16; i++)
            {
                std::string dps = BotAI::BotSpawner::instance()->GetRandomBotName(BotAI::BotRole::DPS);
                BotAI::BotSpawner::instance()->SpawnBot(player, dps, BotAI::BotRole::DPS);
            }

            handler->SendSysMessage("|cffff8000✓ Raid ready! (3 Tanks, 6 Healers, 16 DPS)|r");
        }
        else if (*type == "raid10")
        {
            handler->SendSysMessage("|cffff8000Assembling 10-man raid...|r");

            // 10-man raid comp: 2 tanks, 3 healers, 5 DPS
            for (int i = 0; i < 2; i++)
            {
                std::string tank = BotAI::BotSpawner::instance()->GetRandomBotName(BotAI::BotRole::TANK);
                BotAI::BotSpawner::instance()->SpawnBot(player, tank, BotAI::BotRole::TANK);
            }

            for (int i = 0; i < 3; i++)
            {
                std::string healer = BotAI::BotSpawner::instance()->GetRandomBotName(BotAI::BotRole::HEALER);
                BotAI::BotSpawner::instance()->SpawnBot(player, healer, BotAI::BotRole::HEALER);
            }

            for (int i = 0; i < 5; i++)
            {
                std::string dps = BotAI::BotSpawner::instance()->GetRandomBotName(BotAI::BotRole::DPS);
                BotAI::BotSpawner::instance()->SpawnBot(player, dps, BotAI::BotRole::DPS);
            }

            handler->SendSysMessage("|cffff8000✓ 10-man raid ready! (2 Tanks, 3 Healers, 5 DPS)|r");
        }
        else
        {
            handler->SendSysMessage("Invalid group type. Use: dungeon, raid, or raid10");
            return false;
        }

        return true;
    }
};

void AddBotAIScripts()
{
    new BotAIWorldScript();
    new BotAIPlayerScript();
    new mbot_commandscript();
}
