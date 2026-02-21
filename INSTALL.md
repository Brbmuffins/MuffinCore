# 📦 MUFFIN COMPANIONS - Installation Guide

## Prerequisites

Before installing Muffin Companions, ensure you have:

- ✅ **AzerothCore** 3.3.5a (WotLK) - Fully compiled and running
- ✅ **CMake** 3.16 or higher
- ✅ **GCC** 8+ or **Clang** 6+ (C++17 support required)
- ✅ **Root or sudo access** to server
- ✅ **Basic Linux knowledge**

---

## Installation Methods

Choose the method that works best for you:

### Method 1: Git Clone (Recommended)

This is the easiest and most maintainable method.

```bash
# Navigate to your AzerothCore modules directory
cd ~/azerothcore-wotlk/modules

# Clone the repository
git clone https://github.com/yourusername/mod-muffin-companions.git

# Rename if needed
mv mod-muffin-companions mod-bot-ai

# Return to build directory
cd ../build

# Reconfigure CMake
cmake ../ -DCMAKE_INSTALL_PREFIX=$HOME/azeroth-server -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DWITH_WARNINGS=1 -DTOOLS_BUILD=db-only -DSCRIPTS=static -DMODULES=static

# Compile (use all CPU cores)
make -j$(nproc)

# Install
make install

# Restart your server
./azeroth-server/bin/worldserver
```

### Method 2: Manual File Copy

If you don't have git access or prefer manual control:

```bash
# Create module directory
cd ~/azerothcore-wotlk/modules
mkdir -p mod-bot-ai/src

# Copy all files from the package
cp /path/to/muffin-companions/src/* mod-bot-ai/src/
cp /path/to/muffin-companions/CMakeLists.txt mod-bot-ai/
cp /path/to/muffin-companions/README.md mod-bot-ai/

# Follow compilation steps from Method 1
cd ../build
cmake ../
make -j$(nproc)
make install
```

### Method 3: Docker Installation

If you're running AzerothCore in Docker:

```dockerfile
# Add to your Dockerfile
COPY mod-bot-ai/ /azerothcore/modules/mod-bot-ai/

# Rebuild your container
docker-compose build
docker-compose up -d
```

---

## Verification

After installation, verify the module is loaded:

### 1. Check Server Logs

```bash
tail -f ~/azeroth-server/logs/Server.log | grep -i "muffin"
```

You should see:
```
========================================
  MUFFIN COMPANIONS - Bot AI Module
  The Ultimate AI Bot System
========================================
```

### 2. Test In-Game

1. Log in to your server
2. Type `.mbot` - you should see command help
3. Try `.mbot add tank` - a tank should spawn
4. Use `.mbot list` to see your bot
5. Use `.mbot remove all` to clean up

### 3. Compile-Time Verification

During compilation, you should see:
```
[ XX%] Building CXX object BotAI.cpp
[ XX%] Building CXX object BotCombatSystem.cpp
[ XX%] Building CXX object QuickBotSpawner.cpp
```

---

## Common Installation Issues

### Issue 1: CMake Can't Find Module

**Symptom:**
```
CMake Warning: Manually-specified variables were not used by the project: mod-bot-ai
```

**Solution:**
```bash
# Ensure the module directory is named correctly
cd ~/azerothcore-wotlk/modules
ls -la mod-bot-ai/  # Should show CMakeLists.txt and src/

# Make sure CMakeLists.txt is in the root of mod-bot-ai/
# NOT in mod-bot-ai/src/
```

### Issue 2: Compilation Errors

**Symptom:**
```
error: 'BotConstants' has not been declared
```

**Solution:**
```bash
# Ensure all header files are present
cd ~/azerothcore-wotlk/modules/mod-bot-ai/src
ls -la *.h

# You should see:
# BotAI.h, BotCombatSystem.h, BotConstants.h, BotSpawner.h, etc.

# If BotConstants.h is missing, the installation is incomplete
```

### Issue 3: Module Not Loading

**Symptom:** No "MUFFIN COMPANIONS" message in server logs

**Solution:**
```bash
# Check if module was actually compiled
cd ~/azeroth-server/bin
ldd worldserver | grep -i bot

# Recompile with verbose output
cd ~/azerothcore-wotlk/build
make VERBOSE=1 | grep -i bot

# Check module configuration
cmake ../ -LAH | grep -i module
```

### Issue 4: Bots Don't Spawn

**Symptom:** `.mbot add tank` gives no error but no bot appears

**Solution:**
```bash
# Check server logs for errors
tail -100 ~/azeroth-server/logs/Server.log | grep -i "bot\|error"

# Common causes:
# 1. Database issue - check creature templates exist
# 2. Map not loaded - try in a different zone
# 3. Phase mismatch - ensure you're in phase 1
```

### Issue 5: "%%s" Instead of Names

**Symptom:** Bots show as "%%s" or blank names

**Solution:**
```cpp
// This is being addressed in QuickBotSpawner.cpp
// The code tries multiple methods to set names
// If it persists, it may be an AzerothCore version issue

// Temporary workaround:
// Edit QuickBotSpawner.cpp line ~200
// Add additional name-setting method for your AC version
```

---

## Post-Installation Configuration

### Adjust Bot Limits

Edit `BotConstants.h`:
```cpp
// Change maximum bots per player
constexpr uint32 MAX_BOTS_PER_PLAYER = 25;  // Default
// Change to your preference (1-40 recommended)
```

### Adjust Combat Behavior

Edit `BotConstants.h`:
```cpp
// Healing thresholds
constexpr float HEAL_EMERGENCY_PCT = 40.0f;  // Emergency heal trigger
constexpr float HEAL_MEDIUM_PCT = 60.0f;     // Medium heal trigger

// Leash distance
constexpr float LEASH_DISTANCE = 30.0f;      // How far bots chase

// Damage scaling
constexpr uint32 TANK_BASE_DAMAGE = 20;      // Tank damage
constexpr uint32 DPS_BASE_DAMAGE = 50;       // DPS damage
```

After changing constants:
```bash
cd ~/azerothcore-wotlk/build
make -j$(nproc)
make install
# Restart server
```

### Enable Debug Logging

Edit `BotConstants.h`:
```cpp
constexpr bool ENABLE_DEBUG_LOGGING = true;   // Enable verbose logs
constexpr bool ENABLE_VERBOSE_COMBAT = true;  // Combat details
```

Then check logs:
```bash
tail -f ~/azeroth-server/logs/Server.log | grep -i bot
```

---

## Upgrading

### From Version 1.x to 2.0

```bash
cd ~/azerothcore-wotlk/modules/mod-bot-ai

# Backup your current installation
cp -r . ../mod-bot-ai-backup

# Pull latest changes (if using git)
git pull origin main

# Or manually replace files
rm -rf src/*
cp /path/to/new/version/src/* src/

# Recompile
cd ../../build
make clean
cmake ../
make -j$(nproc)
make install
```

---

## Uninstallation

To completely remove Muffin Companions:

```bash
# Remove module directory
cd ~/azerothcore-wotlk/modules
rm -rf mod-bot-ai

# Recompile without module
cd ../build
cmake ../
make -j$(nproc)
make install

# Restart server
```

---

## Docker-Specific Instructions

### Add Module to Existing Container

```bash
# Copy module into running container
docker cp mod-bot-ai/ azerothcore:/azerothcore/modules/

# Enter container
docker exec -it azerothcore bash

# Compile inside container
cd /azerothcore/build
cmake ../
make -j$(nproc)
make install

# Exit and restart
exit
docker-compose restart
```

### Permanent Docker Installation

Edit your `docker-compose.yml`:
```yaml
services:
  ac-worldserver:
    volumes:
      - ./mod-bot-ai:/azerothcore/modules/mod-bot-ai:ro
```

Then rebuild:
```bash
docker-compose down
docker-compose build
docker-compose up -d
```

---

## Performance Tuning

### For High Player Count Servers

```cpp
// In BotConstants.h
constexpr uint32 MAX_BOTS_PER_PLAYER = 10;  // Reduce from 25
constexpr uint32 THINK_INTERVAL_MS = 2000;  // Reduce AI frequency
```

### For Solo/Low Player Servers

```cpp
// In BotConstants.h
constexpr uint32 MAX_BOTS_PER_PLAYER = 40;  // Allow more bots
constexpr uint32 THINK_INTERVAL_MS = 500;   // More responsive AI
```

---

## Testing Checklist

After installation, run through this checklist:

- [ ] Server starts without errors
- [ ] "MUFFIN COMPANIONS" appears in logs
- [ ] `.mbot` command shows help
- [ ] `.mbot add tank` spawns a tank
- [ ] Bot follows you around
- [ ] Bot attacks when you attack
- [ ] Bot stops chasing at 30 yards (leash)
- [ ] Healer bot heals when you're damaged
- [ ] `.mbot list` shows bot status
- [ ] `.mbot remove all` dismisses bots
- [ ] `.mbot group dungeon` spawns 4 bots
- [ ] `.mbot add brule` spawns "Dr Steve"
- [ ] Bots resurrect when you resurrect
- [ ] No memory leaks after 1 hour of play

---

## Getting Help

If you encounter issues:

1. **Check this guide first** - Most issues are covered above
2. **Check server logs** - `tail -100 ~/azeroth-server/logs/Server.log`
3. **Enable debug logging** - See "Enable Debug Logging" above
4. **Open a GitHub issue** - Include logs and steps to reproduce
5. **Join our Discord** - Live community support

---

## Next Steps

Once installed:

1. Read the [README.md](README.md) for features and commands
2. Check [CHANGELOG.md](CHANGELOG.md) for version history
3. Explore bot personalities with `.mbot add brule`
4. Try a full raid with `.mbot group raid`
5. Have fun! The bots are here to serve.

---

**Installation complete! For your health! 🥦**

*- Dr Steve Brule*
