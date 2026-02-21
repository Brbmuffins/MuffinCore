#ifndef _BOT_PERSONALITY_H
#define _BOT_PERSONALITY_H

#include <string>
#include "Define.h"
#include <vector>
// Forward declarations
class Creature;
class Player;
class Unit;

#include <map>

namespace BotAI {

// Personality types
enum class PersonalityType {
    BRULE,          // For your health! Goofy tank
    SERIOUS,        // Professional healer
    SARCASTIC,      // Witty mage
    QUIET,          // Silent rogue (rare comments)
    RIGHTEOUS       // Paladin justice warrior
};

// Chat context - WHEN they're talking
enum class ChatContext {
    PULL_READY,         // "Ready to pull!"
    WAITING_MANA,       // "Need mana, hold on"
    WAITING_BUFFS,      // "Someone needs buffs"
    PULLING,            // "Pulling now!"
    LOW_HEALTH,         // "I'm dying here!"
    LOW_MANA,           // "OOM!"
    VICTORY,            // "We got 'em!"
    WIPE,               // "Well that didn't work"
    LOOT,               // "Ooh shiny!"
    IDLE_BANTER,        // Random chatting
    COMPLIMENT,         // Praising another bot
    COMPLAINT,          // Griping about something
    BOSS_WARNING,       // "Big pull incoming"
    MISTAKE,            // "My bad!"
    IMPATIENT           // "Can we go already?"
};

// Emotional state affects dialogue
enum class EmotionalState {
    CONFIDENT,      // Things going well
    NERVOUS,        // Close calls
    FRUSTRATED,     // Wiping repeatedly  
    EXCITED,        // Good loot, good pulls
    BORED,          // Too easy
    EXHAUSTED       // Long dungeon run
};

struct DialogueLine {
    std::string text;
    ChatContext context;
    float probability;      // 0.0 to 1.0, how often to say it
    uint32 cooldownMs;      // Don't spam
};

class BotPersonality {
public:
    BotPersonality(PersonalityType type, const std::string& name);
    
    // Get appropriate dialogue
    std::string GetDialogue(ChatContext context, EmotionalState mood);
    
    // Contextual responses
    std::string ExplainWait(const std::string& reason);  // "Waiting because..."
    std::string ReactToSituation(const std::string& situation);
    std::string BanterWith(const std::string& otherBotName, PersonalityType otherType);
    
    // Personality traits
    float GetTalkativeness() const { return talkativeness_; }
    float GetHumorLevel() const { return humorLevel_; }
    bool WillCommentOn(ChatContext context);
    
    // Emotional tracking
    void UpdateMood(EmotionalState newMood);
    EmotionalState GetMood() const { return currentMood_; }
    
private:
    PersonalityType type_;
    std::string name_;
    EmotionalState currentMood_;
    
    float talkativeness_;   // 0.0 to 1.0
    float humorLevel_;      // 0.0 to 1.0
    float patience_;        // 0.0 to 1.0
    
    std::map<ChatContext, std::vector<DialogueLine>> dialogueLibrary_;
    std::map<ChatContext, uint32> lastSaidTime_;
    
    void InitializeDialogue();
    void InitializeBruleDialogue();
    void InitializeSeriousDialogue();
    void InitializeSarcasticDialogue();
    void InitializeQuietDialogue();
    void InitializeRighteousDialogue();
};

// Chat Manager - Handles ALL bot communication
class BotChatManager {
public:
    static BotChatManager* instance();
    
    // Say things (with natural delays!)
    void Say(Creature* bot, const std::string& message, bool isPartyChat = true);
    void Yell(Creature* bot, const std::string& message);
    void Emote(Creature* bot, const std::string& action);  // *waves*
    
    // Transparency - explain what bots are doing
    void AnnounceWait(Creature* bot, const std::string& reason);
    void AnnounceAction(Creature* bot, const std::string& action);
    
    // Natural conversation between bots
    void TriggerBanter(Creature* bot1, Creature* bot2);
    void RandomBanter();  // Occasional random chatter
    
    // Reaction system
    void ReactToEvent(const std::string& event, Creature* reactor);
    
    // Anti-spam
    bool CanTalk(Creature* bot, ChatContext context);
    void RegisterTalk(Creature* bot, ChatContext context);
    
private:
    BotChatManager() = default;
    static BotChatManager* instance_;
    
    std::map<uint64, uint32> lastTalkTime_;  // Anti-spam per bot
    std::map<uint64, std::map<ChatContext, uint32>> contextCooldowns_;
    
    uint32 globalChatCooldown_ = 2000;  // 2 sec between any bot talking
    uint32 lastGlobalChat_ = 0;
};

// Pre-written dialogue for each personality
namespace Dialogue {

// BRULE LINES (Steve Brule style)
const std::vector<std::string> BRULE_WAITING_MANA = {
    "Hold on ya dingus, healer needs a drink for their health!",
    "Wait up! Gotta let the priest get their mana juices back!",
    "Healer's drinkin'. It's good for ya!",
    "Can't pull yet, healer's drier than my uncle's turkey, check it out!"
};

const std::vector<std::string> BRULE_PULLING = {
    "Here we go ya dingus! For your health!",
    "Brule's comin' in hot! Watch this!",
    "Time to tank it up! *charges*",
    "Let's make this quick, got a doctor's appointment later!"
};

const std::vector<std::string> BRULE_LOW_HEALTH = {
    "Ow! That hurt my body!",
    "I'm bleedin' here! Little help!?",
    "My health is goin' down! Not good for ya!",
    "HEALS! I need heals, not pills!"
};

const std::vector<std::string> BRULE_VICTORY = {
    "We got 'em! Good job team, for your health!",
    "That's how Brule does it! Check it out!",
    "Easy peasy, just like Dr. Steve showed me!",
    "Another successful procedure! High five!"
};

const std::vector<std::string> BRULE_IDLE_BANTER = {
    "Ya know, my uncle once fought a bear... didn't go well",
    "This dungeon smells like my doctor's office",
    "Anybody else hungry? I could go for some broccoli",
    "Tanking is easy, you just stand there and get hit! For your health!"
};

// MIRANA (Serious Healer) LINES
const std::vector<std::string> MIRANA_WAITING_MANA = {
    "Drinking. Give me 10 seconds.",
    "Mana: 40%. Need a moment.",
    "Hold, I'm out of mana.",
    "Waiting for mana. Don't pull."
};

const std::vector<std::string> MIRANA_LOW_MANA = {
    "OOM. Need to drink after this.",
    "Mana critical. Conserving heals.",
    "Running dry. Be careful.",
    "Can't heal much longer."
};

const std::vector<std::string> MIRANA_FRUSTRATED = {
    "*sighs* Please wait for mana.",
    "Brule, I JUST said I need mana...",
    "Stop. Pulling. While. I'm. Drinking.",
    "Do you WANT to wipe?"
};

// KAEL (Sarcastic Mage) LINES
const std::vector<std::string> KAEL_WAITING = {
    "Oh good, another break. My fingers were getting tired.",
    "Standing around is my favorite part of dungeons.",
    "I'll just... conjure some more water I guess.",
    "Take your time, I'm only doing 90% of the damage here."
};

const std::vector<std::string> KAEL_COMBAT = {
    "Fireball. Fireball. Fireball. Riveting.",
    "Try not to die while I'm busy actually doing things.",
    "Oh look, I pulled aggro from Mr. 'Tank' over there.",
    "And THAT is how you DPS, class."
};

const std::vector<std::string> KAEL_BANTER_BRULE = {
    "Brule, do you even KNOW what threat means?",
    "Fascinating technique there, Brule. Very... creative.",
    "I'm not saying you're bad at tanking, but...",
    "Your health! *snicker*"
};

// VEX (Quiet Rogue) LINES - Rare, impactful
const std::vector<std::string> VEX_RARE_COMMENTS = {
    "...",
    "*nods*",
    "Interrupting.",
    "Got it.",
    "Behind you.",
    "*stabs*"
};

// THRAIN (Righteous Paladin) LINES
const std::vector<std::string> THRAIN_READY = {
    "By the Light, I am ready!",
    "Justice awaits! Let us proceed!",
    "The Light guides us. Forward!",
    "Fear not, for righteousness protects us!"
};

const std::vector<std::string> THRAIN_VICTORY = {
    "Evil has been vanquished!",
    "The Light prevails!",
    "Another victory for justice!",
    "Well fought, companions!"
};

const std::vector<std::string> THRAIN_BANTER_BRULE = {
    "Brule... your methods are unorthodox, but effective.",
    "For your health? I believe you mean 'for the Light'.",
    "Brother Brule fights with... unique passion.",
    "The Light works in mysterious ways. Very mysterious."
};

} // namespace Dialogue

// REALISM FEATURES
class BotRealismSystem {
public:
    static BotRealismSystem* instance();
    
    // Make bots feel human
    void AddReactionDelay(Creature* bot, uint32 delayMs);  // 150-350ms human delay
    void AddTypingDelay(Creature* bot, const std::string& message);  // Looks like typing
    void AddDecisionHesitation(Creature* bot);  // Occasional "uh..." moment
    
    // Mistakes (intentional imperfection!)
    bool ShouldMakeMistake(float skillLevel);  // 0.9 = 10% failure rate
    void MakeHumanMistake(Creature* bot);  // Stand in fire briefly, miss interrupt
    
    // Personality quirks
    void AddQuirk(Creature* bot, const std::string& quirk);
    // Examples: "always checks gear", "drinks too early", "impatient"
    
    // Emotions
    void ShowEmotion(Creature* bot, EmotionalState emotion);
    // *sighs*, *celebrates*, *panics*
    
    // Memory
    void RememberEvent(Creature* bot, const std::string& event);
    std::string RecallMemory(Creature* bot, const std::string& context);
    // "Last time we wiped here, remember?"
    
private:
    BotRealismSystem() = default;
    static BotRealismSystem* instance_;
    
    std::map<uint64, std::vector<std::string>> botMemories_;
    std::map<uint64, std::vector<std::string>> botQuirks_;
};

} // namespace BotAI

#endif
