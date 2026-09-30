#pragma once
#include "../../common/CommonDefines.h"

#ifdef ENABLE_GLOVE_SYSTEM
#include <map>

extern std::map<int, int> GLOVE_BONUS_TABLE;

class CGloveBonusManager
{
    public:
        static int GetRefineNewGloveBonus(int attributeType, int baseValue);
        static int GetRandomGloveBonus(int attributeType, int baseValue);

        static const std::map<int, int> GLOVE_RANDOM_BONUS_TABLE;
};
#endif