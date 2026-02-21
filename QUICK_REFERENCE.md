# 🎮 MUFFIN COMPANIONS - Quick Reference

## Essential Commands

```bash
# Spawn Bots
.mbot add tank          # Random tank
.mbot add healer        # Random healer
.mbot add dps           # Random DPS
.mbot add brule         # Dr Steve Brule!

# Groups
.mbot group dungeon     # 5-man (1T, 1H, 2D)
.mbot group raid10      # 10-man (2T, 3H, 5D)
.mbot group raid        # 25-man (3T, 6H, 16D)

# Management
.mbot list              # List all bots + stats
.mbot remove all        # Dismiss all bots
```

---

## Bot Behaviors

### Tanks 🛡️
- Stand in front
- Taunt enemies
- Hold threat
- **Won't** chase past 30 yards

### Healers 💚
- Stay 15 yards back
- Heal owner when < 95% HP
- Heal bots when < 70% HP
- Heal self when < 60% HP
- **Won't** heal when OOM (< 10% mana)

### DPS ⚔️
- Stay 10-20 yards away
- Attack owner's target
- **Won't** chase past 30 yards

---

## Combat Mechanics

### Open World
✅ Bots only attack YOUR target  
❌ Bots won't attack random mobs  
✅ Leash at 30 yards (they stop chasing)

### Dungeons/Raids
✅ Bots attack nearby enemies (< 40 yards)  
✅ Bots help with trash packs  
✅ Still leash at 30 yards from you

---

## Special Features

### Auto-Resurrection
- Bots resurrect when you do
- 50% HP, 20% mana
- 1-second delay

### Formation
- Semicircle behind you
- 3 bots per row
- Organized spacing

### Level Scaling
- Bots match your level
- HP scales with level
- Mana scales with level

---

## Limits

| Feature | Limit |
|---------|-------|
| Max bots per player | 25 |
| Leash distance | 30 yards |
| Healer range | 40 yards |
| Follow distance | Varies by role |

---

## Troubleshooting

**Bot won't heal:**
→ Check if healer has mana
→ Check if target is < 85% HP
→ Check if within 40 yards

**Bot chasing too far:**
→ Leash is 30 yards (should stop)
→ Report if still chasing beyond

**Bot names show "%%s":**
→ Known issue in some AC versions
→ Functionality still works

**Bots not spawning:**
→ Check you haven't hit 25-bot limit
→ Try different zone
→ Check server logs

---

## Bot Roster

### Tanks (5)
- **Dr Steve** - Human Warrior (goofy)
- **Ironforge** - Dwarf Warrior
- **Grimshield** - Human Paladin
- **Frostguard** - DK
- **Bearfang** - Druid

### Healers (4)
- **Mirana** - Priest
- **Brightheal** - Paladin
- **Earthmender** - Shaman
- **Moonwhisper** - Druid

### DPS (9)
- **Kael** - Mage (sarcastic)
- **Frostwhisper** - Mage
- **Shadowstrike** - Rogue
- **Vex** - Rogue
- **Swiftarrow** - Hunter
- **Eagleeye** - Hunter
- **Voidcaller** - Warlock
- **Doomwhisper** - Warlock
- **Thrain** - Ret Paladin

---

## Performance Tips

**For Better Performance:**
- Use fewer bots in crowded areas
- Dismiss bots when not needed
- `.mbot remove all` before logout

**For More Challenge:**
- Spawn only 1-2 bots
- Try solo with just a healer
- No tanks = hard mode!

---

## Fun Facts

- Dr Steve says "For your health!" 🥦
- Healers prioritize YOU over other bots
- Tanks generate 3x threat from damage
- Bots have unique personalities (coming soon!)
- 70 dungeons pre-mapped (navigation coming!)

---

## Quick Config

Edit `BotConstants.h` to change:

```cpp
MAX_BOTS_PER_PLAYER = 25      // Change bot limit
LEASH_DISTANCE = 30.0f         // Change leash
HEAL_EMERGENCY_PCT = 40.0f     // When to big heal
TANK_HP_MULTIPLIER = 8.0f      // Tank HP amount
```

Recompile after changes:
```bash
cd ~/azerothcore-wotlk/build
make -j$(nproc) && make install
```

---

## Support

**GitHub:** [Open an issue]  
**Discord:** [Join server]  
**Docs:** [Full documentation]

---

**Made with ❤️ for the AzerothCore community**

*For your health!*  
*- Dr Steve Brule*
