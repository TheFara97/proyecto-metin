
#include "../../common/CommonDefines.h"

#ifdef ENABLE_GLOVE_SYSTEM
#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "log.h"
#include "item.h"
#include "item_manager.h"

#include "stdafx.h"
#include "glove_bonus_manager.h"

std::map<int, int> GLOVE_BONUS_TABLE = {
    {APPLY_FINAL_DMG_BONUS, 20},
    {APPLY_NORMAL_HIT_DAMAGE_BONUS, 30},
    {APPLY_ATTBONUS_DUNGEON, 30},
    {APPLY_ATTBONUS_HUMAN, 50},
    {APPLY_SKILL_DAMAGE_BONUS, 25},
    {APPLY_STEAL_HP, 15},
    {APPLY_MAX_HP, 2000},
    {APPLY_CRITICAL_PCT, 20},
    {APPLY_ATT_SPEED, 8},
    {APPLY_MOV_SPEED, 8},
    {APPLY_SLOW_PCT, 8},
    {APPLY_POISON_PCT, 5},
    {APPLY_RESIST_HUMAN, 20},
};

int CGloveBonusManager::GetRefineNewGloveBonus(int attributeType, int baseValue) 
{
    auto it = GLOVE_BONUS_TABLE.find(attributeType);
    if (it != GLOVE_BONUS_TABLE.end()) {
        return baseValue + (it->second / 9);
    }

    sys_err("GLOVE_SYSTEM: attribute not found in the table: glove_bonus_table.cpp - GLOVE_BONUS_TABLE");
    return baseValue;
}

const std::map<int, int> CGloveBonusManager::GLOVE_RANDOM_BONUS_TABLE = {
    { APPLY_FINAL_DMG_BONUS, { 3 } }, 
    { APPLY_NORMAL_HIT_DAMAGE_BONUS, { 5 } }, 
    { APPLY_ATTBONUS_DUNGEON, { 5 } }, 
    { APPLY_ATTBONUS_HUMAN, { 5 } }, 
    { APPLY_SKILL_DAMAGE_BONUS, { 3 } }, 
    { APPLY_STEAL_HP, { 3 } },  
    { APPLY_MAX_HP, { 300 } }, 
    { APPLY_CRITICAL_PCT, { 3 } }, 
    { APPLY_ATT_SPEED, { 3 } }, 
    { APPLY_MOV_SPEED, { 3 } }, 
    { APPLY_SLOW_PCT, { 3 } }, 
    { APPLY_POISON_PCT, { 1 } },
    { APPLY_RESIST_HUMAN, { 3 } },
};

int CGloveBonusManager::GetRandomGloveBonus(int attributeType, int baseValue) 
{
    auto it = GLOVE_RANDOM_BONUS_TABLE.find(attributeType);
    if (it != GLOVE_RANDOM_BONUS_TABLE.end()) {
        return baseValue + it->second;
    }

    sys_err("GLOVE_SYSTEM: attribute not found in the table: glove_bonus_table.cpp - GLOVE_RANDOM_BONUS_TABLE");
    return baseValue;
}

#endif