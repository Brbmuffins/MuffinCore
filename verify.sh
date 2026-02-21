#!/bin/bash
# MUFFIN COMPANIONS - Installation Verification Script

echo "=========================================="
echo "  MUFFIN COMPANIONS - Verification"
echo "=========================================="
echo ""

ERRORS=0

# Check all required files exist
echo "Checking files..."

FILES=(
    "README.md"
    "INSTALL.md"
    "CHANGELOG.md"
    "LICENSE"
    "CMakeLists.txt"
    "src/BotAI.cpp"
    "src/BotAI.h"
    "src/BotCombatSystem.cpp"
    "src/BotCombatSystem.h"
    "src/BotConstants.h"
    "src/QuickBotSpawner.cpp"
    "src/mod_bot_ai.cpp"
)

for file in "${FILES[@]}"; do
    if [ -f "$file" ]; then
        echo "  ✓ $file"
    else
        echo "  ✗ $file MISSING!"
        ((ERRORS++))
    fi
done

echo ""
echo "Checking code integrity..."

# Check for critical fixes
if grep -q "LEASH_DISTANCE" src/BotConstants.h; then
    echo "  ✓ Leash system constants present"
else
    echo "  ✗ Leash constants missing!"
    ((ERRORS++))
fi

if grep -q "delete it->second" src/BotAI.cpp; then
    echo "  ✓ Memory leak fix present"
else
    echo "  ✗ Memory leak fix missing!"
    ((ERRORS++))
fi

if grep -q "Dr Steve" src/QuickBotSpawner.cpp; then
    echo "  ✓ Dr Steve special spawn present"
else
    echo "  ✗ Dr Steve spawn missing!"
    ((ERRORS++))
fi

if grep -q "GetManaPercent() < Constants::LOW_MANA_PCT" src/BotCombatSystem.cpp; then
    echo "  ✓ Healer mana check present"
else
    echo "  ✗ Healer mana check missing!"
    ((ERRORS++))
fi

echo ""
echo "Checking documentation..."

if grep -q "Dr Steve" README.md; then
    echo "  ✓ README complete"
else
    echo "  ✗ README incomplete!"
    ((ERRORS++))
fi

if grep -q "\[2.0.0\]" CHANGELOG.md; then
    echo "  ✓ CHANGELOG up to date"
else
    echo "  ✗ CHANGELOG outdated!"
    ((ERRORS++))
fi

echo ""
echo "=========================================="

if [ $ERRORS -eq 0 ]; then
    echo "✅ ALL CHECKS PASSED!"
    echo ""
    echo "Your Muffin Companions package is complete and ready to install."
    echo "See INSTALL.md for installation instructions."
    echo ""
    echo "For your health! 🥦"
    exit 0
else
    echo "❌ $ERRORS ERRORS FOUND!"
    echo ""
    echo "Please check the errors above and ensure all files are present."
    exit 1
fi
