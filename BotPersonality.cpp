#include "BotPersonality.h"
#include "Creature.h"
#include "Chat.h"
#include "Player.h"
#include "ObjectAccessor.h"
#include "Log.h"
#include <algorithm>
#include "GameTime.h"

namespace BotAI {

// ============================================================================
// BOT PERSONALITY
// ============================================================================

BotPersonality::BotPersonality(PersonalityType type, const std::string& name)
    : type_(type), name_(name), currentMood_(EmotionalState::CONFIDENT)
{
    // Set personality traits based on type
    switch (type_) {
        case PersonalityType::BRULE:
            talkativeness_ = 0.8f;  // Very talkative
            humorLevel_ = 0.9f;     // Very funny
            patience_ = 0.3f;       // Impatient
            break;
            
        case PersonalityType::SERIOUS:
            talkativeness_ = 0.3f;  // Quiet, professional
            humorLevel_ = 0.1f;     // Rarely jokes
            patience_ = 0.9f;       // Very patient
            break;
            
        case PersonalityType::SARCASTIC:
            talkativeness_ = 0.7f;  // Talkative
            humorLevel_ = 0.8f;     // Sarcastic humor
            patience_ = 0.4f;       // Gets annoyed
            break;
            
        case PersonalityType::QUIET:
            talkativeness_ = 0.1f;  // Very quiet
            humorLevel_ = 0.2f;     // Dry humor
            patience_ = 0.8f;       // Patient
            break;
            
        case PersonalityType::RIGHTEOUS:
            talkativeness_ = 0.6f;  // Moderate
            humorLevel_ = 0.3f;     // Serious but kind
            patience_ = 0.7f;       // Patient with good
            break;
    }
    
    InitializeDialogue();
}

void BotPersonality::InitializeDialogue() {
    switch (type_) {
        case PersonalityType::BRULE:
            InitializeBruleDialogue();
            break;
        case PersonalityType::SERIOUS:
            InitializeSeriousDialogue();
            break;
        case PersonalityType::SARCASTIC:
            InitializeSarcasticDialogue();
            break;
        case PersonalityType::QUIET:
            InitializeQuietDialogue();
            break;
        case PersonalityType::RIGHTEOUS:
            InitializeRighteousDialogue();
            break;
    }
}

void BotPersonality::InitializeBruleDialogue() {
    // BRULE - Steve Brule style goofy tank
    
    // Waiting for mana
    dialogueLibrary_[ChatContext::WAITING_MANA] = {
        {"Hold on ya dingus, healer needs a drink for their health!", ChatContext::WAITING_MANA, 1.0f, 600000},
        {"Wait up! Gotta let the priest get their mana juices back!", ChatContext::WAITING_MANA, 1.0f, 600000},
        {"Healer's drinkin'. It's good for ya!", ChatContext::WAITING_MANA, 1.0f, 600000},
        {"Can't pull yet, healer's drier than my uncle's turkey!", ChatContext::WAITING_MANA, 0.7f, 600000}
    };
    
    // Pulling
    dialogueLibrary_[ChatContext::PULLING] = {
        {"Here we go ya dingus! For your health!", ChatContext::PULLING, 1.0f, 600000},
        {"Brule's comin' in hot! Watch this!", ChatContext::PULLING, 0.8f, 600000},
        {"Time to tank it up! *charges*", ChatContext::PULLING, 0.9f, 600000},
        {"Let's make this quick, got a doctor's appointment later!", ChatContext::PULLING, 0.5f, 600000}
    };
    
    // Low health
    dialogueLibrary_[ChatContext::LOW_HEALTH] = {
        {"Ow! That hurt my body!", ChatContext::LOW_HEALTH, 1.0f, 600000},
        {"I'm bleedin' here! Little help!?", ChatContext::LOW_HEALTH, 0.9f, 600000},
        {"My health is goin' down! Not good for ya!", ChatContext::LOW_HEALTH, 0.8f, 600000},
        {"HEALS! I need heals, not pills!", ChatContext::LOW_HEALTH, 0.7f, 600000}
    };
    
    // Victory
    dialogueLibrary_[ChatContext::VICTORY] = {
        {"We got 'em! Good job team, for your health!", ChatContext::VICTORY, 1.0f, 600000},
        {"That's how Brule does it! Check it out!", ChatContext::VICTORY, 0.8f, 600000},
        {"Easy peasy, just like Dr. Steve showed me!", ChatContext::VICTORY, 0.6f, 600000},
        {"Another successful procedure! High five!", ChatContext::VICTORY, 0.7f, 600000}
    };
    
    // Idle banter
    dialogueLibrary_[ChatContext::IDLE_BANTER] = {
        {"Ya know, my uncle once fought a bear... didn't go well", ChatContext::IDLE_BANTER, 0.3f, 600000},
        {"This dungeon smells like my doctor's office", ChatContext::IDLE_BANTER, 0.2f, 600000},
        {"Anybody else hungry? I could go for some broccoli", ChatContext::IDLE_BANTER, 0.2f, 600000},
        {"Tanking is easy, you just stand there and get hit!", ChatContext::IDLE_BANTER, 0.4f, 600000}
    };
}

void BotPersonality::InitializeSeriousDialogue() {
    // MIRANA - Professional, concise healer
    
    dialogueLibrary_[ChatContext::WAITING_MANA] = {
        {"Drinking. Give me 10 seconds.", ChatContext::WAITING_MANA, 1.0f, 600000},
        {"Mana low. Need a moment.", ChatContext::WAITING_MANA, 1.0f, 600000},
        {"Hold, I'm out of mana.", ChatContext::WAITING_MANA, 1.0f, 600000}
    };
    
    dialogueLibrary_[ChatContext::LOW_MANA] = {
        {"OOM. Need to drink after this.", ChatContext::LOW_MANA, 1.0f, 600000},
        {"Mana critical. Conserving heals.", ChatContext::LOW_MANA, 0.9f, 600000},
        {"Running dry. Be careful.", ChatContext::LOW_MANA, 0.8f, 600000}
    };
    
    dialogueLibrary_[ChatContext::IMPATIENT] = {
        {"*sighs* Please wait for mana.", ChatContext::IMPATIENT, 0.6f, 600000},
        {"Brule, I JUST said I need mana...", ChatContext::IMPATIENT, 0.5f, 600000},
        {"Stop. Pulling. While. I'm. Drinking.", ChatContext::IMPATIENT, 0.7f, 600000}
    };
}

void BotPersonality::InitializeSarcasticDialogue() {
    // KAEL - Sarcastic mage
    
    dialogueLibrary_[ChatContext::WAITING_MANA] = {
        {"Oh good, another break. My fingers were getting tired.", ChatContext::WAITING_MANA, 0.8f, 600000},
        {"Standing around is my favorite part of dungeons.", ChatContext::WAITING_MANA, 0.7f, 600000},
        {"I'll just... conjure some more water I guess.", ChatContext::WAITING_MANA, 0.6f, 600000}
    };
    
    dialogueLibrary_[ChatContext::VICTORY] = {
        {"And THAT is how you DPS, class.", ChatContext::VICTORY, 0.7f, 600000},
        {"Try not to die while I'm busy doing things.", ChatContext::VICTORY, 0.5f, 600000},
        {"Flawless, as always.", ChatContext::VICTORY, 0.6f, 600000}
    };
    
    dialogueLibrary_[ChatContext::IDLE_BANTER] = {
        {"Brule, do you even KNOW what threat means?", ChatContext::IDLE_BANTER, 0.3f, 600000},
        {"Fascinating technique there. Very... creative.", ChatContext::IDLE_BANTER, 0.3f, 600000},
        {"I'm doing 90% of the damage here. Just saying.", ChatContext::IDLE_BANTER, 0.4f, 600000}
    };
}

void BotPersonality::InitializeQuietDialogue() {
    // VEX - Quiet rogue, rare short comments
    
    dialogueLibrary_[ChatContext::PULL_READY] = {
        {"Ready.", ChatContext::PULL_READY, 0.8f, 600000},
        {"...", ChatContext::PULL_READY, 0.5f, 600000}
    };
    
    dialogueLibrary_[ChatContext::VICTORY] = {
        {"*nods*", ChatContext::VICTORY, 0.6f, 600000},
        {"Done.", ChatContext::VICTORY, 0.4f, 600000}
    };
    
    dialogueLibrary_[ChatContext::IDLE_BANTER] = {
        {"...", ChatContext::IDLE_BANTER, 0.8f, 600000}
    };
}

void BotPersonality::InitializeRighteousDialogue() {
    // THRAIN - Righteous paladin
    
    dialogueLibrary_[ChatContext::PULL_READY] = {
        {"By the Light, I am ready!", ChatContext::PULL_READY, 0.9f, 600000},
        {"Justice awaits! Let us proceed!", ChatContext::PULL_READY, 0.7f, 600000},
        {"The Light guides us. Forward!", ChatContext::PULL_READY, 0.8f, 600000}
    };
    
    dialogueLibrary_[ChatContext::VICTORY] = {
        {"Evil has been vanquished!", ChatContext::VICTORY, 0.8f, 600000},
        {"The Light prevails!", ChatContext::VICTORY, 0.9f, 600000},
        {"Well fought, companions!", ChatContext::VICTORY, 0.7f, 600000}
    };
    
    dialogueLibrary_[ChatContext::IDLE_BANTER] = {
        {"Brule fights with... unique passion.", ChatContext::IDLE_BANTER, 0.3f, 600000},
        {"For your health? I believe you mean 'for the Light'.", ChatContext::IDLE_BANTER, 0.4f, 600000},
        {"The Light works in mysterious ways.", ChatContext::IDLE_BANTER, 0.3f, 600000}
    };
}

std::string BotPersonality::GetDialogue(ChatContext context, EmotionalState mood) {
    auto it = dialogueLibrary_.find(context);
    if (it == dialogueLibrary_.end() || it->second.empty()) {
        return "";
    }
    
    // Filter by cooldown and probability
    std::vector<DialogueLine> available;
    uint32 now = GameTime::GetGameTimeMS().count();
    
    for (const auto& line : it->second) {
        // Check cooldown
        auto cdIt = lastSaidTime_.find(context);
        if (cdIt != lastSaidTime_.end() && (now - cdIt->second) < line.cooldownMs) {
            continue;
        }
        
        // Check probability (affected by mood)
        float chance = line.probability;
        if (mood == EmotionalState::FRUSTRATED) {
            chance *= 1.3f;  // More likely to talk when frustrated
        } else if (mood == EmotionalState::BORED) {
            chance *= 0.5f;  // Less talkative when bored
        }
        
        if (frand(0.0f, 1.0f) <= chance) {
            available.push_back(line);
        }
    }
    
    if (available.empty()) {
        return "";
    }
    
    // Pick random from available
    const DialogueLine& chosen = available[urand(0, available.size() - 1)];
    lastSaidTime_[context] = now;
    
    return chosen.text;
}

std::string BotPersonality::ExplainWait(const std::string& reason) {
    // Generate explanation based on personality
    switch (type_) {
        case PersonalityType::BRULE:
            return "Hold on ya dingus! " + reason + " For your health!";
            
        case PersonalityType::SERIOUS:
            return "Waiting: " + reason;
            
        case PersonalityType::SARCASTIC:
            return "Oh, we're waiting. Again. Because " + reason + ".";
            
        case PersonalityType::QUIET:
            return "...waiting";
            
        case PersonalityType::RIGHTEOUS:
            return "Patience, friends. " + reason + " The Light will guide us.";
    }
    return reason;
}

bool BotPersonality::WillCommentOn(ChatContext context) {
    // Check if bot will comment based on talkativeness
    float baseChance = talkativeness_;
    
    // Some contexts are more important
    if (context == ChatContext::WIPE || context == ChatContext::LOW_HEALTH) {
        baseChance = 1.0f;  // Always comment on critical situations
    }
    
    return frand(0.0f, 1.0f) <= baseChance;
}

void BotPersonality::UpdateMood(EmotionalState newMood) {
    currentMood_ = newMood;
}

// ============================================================================
// BOT CHAT MANAGER
// ============================================================================

BotChatManager* BotChatManager::instance_ = nullptr;

BotChatManager* BotChatManager::instance() {
    if (!instance_) {
        instance_ = new BotChatManager();
    }
    return instance_;
}

void BotChatManager::Say(Creature* bot, const std::string& message, bool /*isPartyChat*/) {
    if (!bot || message.empty()) {
        return;
    }
    
    // Check global cooldown
    uint32 now = GameTime::GetGameTimeMS().count();
    if (now - lastGlobalChat_ < globalChatCooldown_) {
        return;  // Too soon, don't spam
    }
    
    // Actually say it
    // DISABLED: bot->Say(message, LANG_UNIVERSAL);
    
    lastGlobalChat_ = now;
    lastTalkTime_[bot->GetGUID().GetCounter()] = now;
    
    LOG_DEBUG("module", "Bot {} says: {}", bot->GetName(), message);
}

void BotChatManager::Yell(Creature* bot, const std::string& message) {
    if (!bot || message.empty()) {
        return;
    }
    
    bot->Yell(message, LANG_UNIVERSAL);
    lastGlobalChat_ = GameTime::GetGameTimeMS().count();
}

void BotChatManager::Emote(Creature* bot, const std::string& action) {
    if (!bot || action.empty()) {
        return;
    }
    
    std::string emoteText = bot->GetName() + " " + action;
    // DISABLED: bot->TextEmote(emoteText, nullptr, false);
}

void BotChatManager::AnnounceWait(Creature* bot, const std::string& reason) {
    if (!bot) {
        return;
    }
    
    // TRANSPARENCY - explain why waiting
    Say(bot, reason, true);
}

void BotChatManager::AnnounceAction(Creature* bot, const std::string& action) {
    if (!bot) {
        return;
    }
    
    Say(bot, action, true);
}

bool BotChatManager::CanTalk(Creature* bot, ChatContext context) {
    if (!bot) {
        return false;
    }
    
    uint32 now = GameTime::GetGameTimeMS().count();
    uint64 guid = bot->GetGUID().GetCounter();
    
    // Check context-specific cooldown
    auto it = contextCooldowns_.find(guid);
    if (it != contextCooldowns_.end()) {
        auto cdIt = it->second.find(context);
        if (cdIt != it->second.end() && (now - cdIt->second) < 5000) {
            return false;  // On cooldown
        }
    }
    
    return true;
}

void BotChatManager::RegisterTalk(Creature* bot, ChatContext context) {
    if (!bot) {
        return;
    }
    
    contextCooldowns_[bot->GetGUID().GetCounter()][context] = GameTime::GetGameTimeMS().count();
}

} // namespace BotAI
