#include "stdafx.h"
#include "constants.h"
#include "utils.h"
#include "item.h"
#include "item_addon.h"
#include "char_manager.h"
#include "char.h"
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
#include "AchievementTitle.h"
#endif

CItemAddonManager::CItemAddonManager()
{
}

CItemAddonManager::~CItemAddonManager()
{
}

void CItemAddonManager::ApplyAddonTo(int iAddonType, LPITEM pItem)
{
	if (!pItem)
	{
		sys_err("ITEM pointer null");
		return;
	}
	
	const auto switchAverageDamageEvent = CHARACTER_MANAGER::Instance().CheckEventIsActive(SWITCH_AVERAGE_EVENT, 0, pItem->GetOwner());
	
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
	LPCHARACTER owner = pItem->GetOwner();
	bool hasTitle9 = (owner && (owner->GetTitleAchievement() == 9 || owner->GetTitleAchievementPremium() == 9));
#endif
	
	// Enhanced skill bonus generation - biased toward positive values
	float skillRoll = gauss_random(0, 5) + 0.5f;
	
	// Apply bias toward positive values to increase 15%+ chances
	if (skillRoll > 0)
		skillRoll *= 1.3f; // Amplify positive values by 30%
	
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
	// Title 9: Further amplify positive skill values
	if (hasTitle9 && skillRoll > 0)
		skillRoll *= 1.2f; // Additional 20% amplification for title 9
#endif
	
	int iSkillBonus = MINMAX(-30, (int)skillRoll, 30);
	
	int iNormalHitBonus = switchAverageDamageEvent ? 63 : 60;
	
	if (abs(iSkillBonus) <= 20)
	{
		int minRange = switchAverageDamageEvent ? 8 : 1;
		int maxRange = switchAverageDamageEvent ? 24 : 19;
		
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
		// Title 9: Shift the random range higher for better normal hit bonus
		if (hasTitle9)
		{
			int bonus = number(minRange, maxRange);
			// Boost the bonus by 10-20%
			bonus += bonus * number(10, 20) / 100;
			iNormalHitBonus = -1.8 * iSkillBonus + abs(number(-7, 10) + number(-7, 10)) + bonus;
		}
		else
#endif
		{
			iNormalHitBonus = -1.8 * iSkillBonus + abs(number(-7, 10) + number(-7, 10)) + number(minRange, maxRange);
		}
	}
	else
	{
		int minRange = switchAverageDamageEvent ? 8 : 1;
		int maxRange = switchAverageDamageEvent ? 24 : 19;
		
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
		// Title 9: Shift the random range higher for better normal hit bonus
		if (hasTitle9)
		{
			int bonus = number(minRange, maxRange);
			// Boost the bonus by 10-20%
			bonus += bonus * number(10, 20) / 100;
			iNormalHitBonus = -1.8 * iSkillBonus + bonus;
		}
		else
#endif
		{
			iNormalHitBonus = -1.8 * iSkillBonus + number(minRange, maxRange);
		}
	}
	
	// Apply the new cap
	if (iNormalHitBonus > (switchAverageDamageEvent ? 63 : 60))
		iNormalHitBonus = switchAverageDamageEvent ? 63 : 60;

	pItem->RemoveAttributeType(APPLY_SKILL_DAMAGE_BONUS);
	pItem->RemoveAttributeType(APPLY_NORMAL_HIT_DAMAGE_BONUS);
	pItem->AddAttribute(APPLY_NORMAL_HIT_DAMAGE_BONUS, iNormalHitBonus);
	pItem->AddAttribute(APPLY_SKILL_DAMAGE_BONUS, iSkillBonus);
}
//martysama0134's ceqyqttoaf71vasf9t71218