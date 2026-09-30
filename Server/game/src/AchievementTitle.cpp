#include "stdafx.h"
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
#include "AchievementTitle.h"
#include "buffer_manager.h"
#include "char.h"
#include "db.h"
#include "desc.h"
#include "constants.h"
#include "../../common/length.h"
#ifdef ENABLE_NEWSTUFF
#include "../../common/PulseManager.h"
#endif

// Achievement conditions - Using 1-based indexing for visible titles
static std::vector<TitleAchievementData> titleAchievements = {
	{
		1,
		{{APPLY_ATTBONUS_STONE, 5}},
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("AchievementTitle.stones_killed") >= 1000;
		}
	},
	{
		2,
		{{APPLY_ATTBONUS_STONE, 10}},
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("AchievementTitle.stones_killed") >= 20000;
		}
	},
	{
		3,
		{{APPLY_ATTBONUS_STONE, 20}},
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("AchievementTitle.stones_killed") >= 100000;
		}
	},
	{
		4,
		{{APPLY_ATTBONUS_BOSS, 5}},
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("AchievementTitle.boss_killed") >= 1000;
		}
	},
	{
		5,
		{{APPLY_ATTBONUS_BOSS, 10}},
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("AchievementTitle.boss_killed") >= 2000;
		}
	},
	{
		6,
		{{APPLY_ATTBONUS_BOSS, 20}},
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("AchievementTitle.boss_killed") >= 20000;
		}
	},
	{
		7,
		{},
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("AchievementTitle.fastest_dungeon") >= 1;
		}
	},
	{
		8,
		{}, // Higher 1-5 Rolls
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("AchievementTitle.enchant_used") >= 50000;
		}
	},
	{
		9,
		{}, // Higher average/skill damage rolls
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("AchievementTitle.enchant_used") >= 500000;
		}
	},
	{
		10,
		{}, // Chance to preserve the enchant relic item when switching relics
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("AchievementTitle.relict_used") >= 1000;
		}
	},
	{
		11,
		{}, // Refine success inreased by 5%
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("AchievementTitle.refine_succes") >= 1000;
		}
	},
	{
		12,
		{}, // Alchemy Refine success inreased by 5%
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("AchievementTitle.refine_alchemy") >= 1000;
		}
	},
	{
		13,
		{{APPLY_MINING_SUCCESS_CHANCE, 5}},
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("mining.my_ore_count") >= 20000;
		}
	},
	{
		14,
		{{APPLY_MINING_SUCCESS_CHANCE, 10}},
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("mining.my_ore_count") >= 200000;
		}
	},
	{
		15,
		{{APPLY_MINING_SUCCESS_CHANCE, 15}},
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("mining.my_ore_count") >= 1000000;
		}
	},
	{
		16,
		{{APPLY_FISHING_SUCCESS_CHANCE, 5}},
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("fishing.my_fish_count") >= 1000;
		}
	},
	{
		17,
		{{APPLY_FISHING_SUCCESS_CHANCE, 10}},
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("fishing.my_fish_count") >= 10000;
		}
	},
	{
		18,
		{{APPLY_FISHING_SUCCESS_CHANCE, 15}},
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("fishing.my_fish_count") >= 100000;
		}
	},
	{
		19,
		{},
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("AchievementTitle.refine_fail") >= 20;
		}
	},
	{
		20,
		{},
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("AchievementTitle.die_to_monster") >= 100;
		}
	},
	{
		21,
		{},
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("AchievementTitle.refine_fail") >= 30;
		}
	},
	{
		22,
		{{APPLY_ATTBONUS_WLADCA, 10}},
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("AchievementTitle.killed_dungeon_bosses") >= 1000;
		}
	},
	{
		23,
		{},
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("AchievementTitle.playtime") >= 2000;
		}
	},
	{
		24,
		{},
		[](LPCHARACTER ch) -> bool {
			return ch->GetQuestFlag("AchievementTitle.dungeon_no_damage") >= 1;
		}
	}
};

static void RecalcDungeonFastestBonuses(LPCHARACTER ch)
{
	if (!ch) return;

	// Boss vnum -> map index (matches m_mapDungeonIndexList in constants.cpp)
	static const std::pair<DWORD, uint16_t> dungeonMap[] = {
		{693, 28}, {1093, 18}, {2598, 300}, {202202, 51}, {202208, 21},
		{202214, 16}, {202218, 12}, {202224, 14}, {202230, 27}
	};
	static const size_t dungeonMapSize = sizeof(dungeonMap) / sizeof(dungeonMap[0]);

	bool hasDungeonTitle = (ch->GetTitleAchievement() == 7 || ch->GetTitleAchievementPremium() == 7);

	for (size_t i = 0; i < dungeonMapSize; ++i)
	{
		uint16_t mapIdx = dungeonMap[i].second;
		std::string bonusFlag = "dungeon_limit." + std::to_string(mapIdx) + "_rank_bonus";
		ch->SetQuestFlag(bonusFlag.c_str(), hasDungeonTitle ? 5 : 0);
	}
}

auto CTitleAchievementTitle::RecalcDungeonBonuses(LPCHARACTER ch) -> void
{
	RecalcDungeonFastestBonuses(ch);
}

auto CTitleAchievementTitle::CheckTitleRequirement(LPCHARACTER ch, const BYTE titleIDX) -> bool {
	if (!ch || titleIDX == 0 || titleIDX > titleAchievements.size()) {
		return false;
	}
	
	const auto& titleData = titleAchievements[titleIDX - 1];
	return titleData.checkCondition(ch);
}

auto CTitleAchievementTitle::HasTitleUnlocked(LPCHARACTER ch, const BYTE titleIDX) -> bool {
	if (!ch || titleIDX == 0 || titleIDX > titleAchievements.size()) return false;

	std::string flagName = "AchievementTitle.title_unlocked_" + std::to_string(titleIDX);
	return ch->GetQuestFlag(flagName.c_str()) > 0;
}

auto CTitleAchievementTitle::UnlockTitle(LPCHARACTER ch, const BYTE titleIDX) -> void {
	if (!ch || titleIDX == 0 || titleIDX > titleAchievements.size()) return;
	
	if (HasTitleUnlocked(ch, titleIDX)) return;
	
	if (!CheckTitleRequirement(ch, titleIDX)) return;
	
	std::string unlockFlag = "AchievementTitle.title_unlocked_" + std::to_string(titleIDX);
	ch->SetQuestFlag(unlockFlag.c_str(), 1);
	
	std::string notifyFlag = "AchievementTitle.title_notified_" + std::to_string(titleIDX);
	if (ch->GetQuestFlag(notifyFlag.c_str()) == 0) {
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10135]");
		ch->SetQuestFlag(notifyFlag.c_str(), 1);
	}
}

auto CTitleAchievementTitle::CheckAndUnlockTitles(LPCHARACTER ch) -> void {
	if (!ch) return;
	
	for (size_t i = 1; i <= titleAchievements.size(); ++i) {
		if (HasTitleUnlocked(ch, i)) continue;
		
		if (CheckTitleRequirement(ch, i)) {
			UnlockTitle(ch, i);
		}
	}
}

auto CTitleAchievementTitle::UpdateTitle(LPCHARACTER ch, const int8_t changeIdx) -> void {
	if (!ch || !ch->GetDesc()) { 
		return; 
	}

	RemoveTitleEffects(ch);
	
	if (changeIdx > 0 && changeIdx <= titleAchievements.size()) {
		ApplyTitleEffects(ch, changeIdx);
		ch->SetTitleAchievement(changeIdx);
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10136]");
	} else {
		ch->SetTitleAchievement(0);
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10137]");
	}

	RecalcDungeonFastestBonuses(ch);
	ch->SendDungeonCooldown(0); // refresh effective daily limits on client after title change

	// Save to database
	SaveTitleToDatabase(ch);

	// Send updated data to client
	SendToClient(ch);
}

auto CTitleAchievementTitle::ApplyTitleEffects(LPCHARACTER ch, const BYTE titleIDX) -> void {
	if (!ch || titleIDX == 0 || titleIDX > titleAchievements.size()) {
		return;
	}
	
	// Convert 1-based titleIDX to 0-based array index
	const auto& titleData = titleAchievements[titleIDX - 1];
	uint8_t affectIdx = 0;
	
	for (const auto& effect : titleData.bonusEffects) {
		if (affectIdx >= 3) break; // Limit to 3 affects per title
		
		ch->AddAffect( AFFECT_TITLE_ACHIEVEMENT_1 + affectIdx, aApplyInfo[effect.first].bPointType, effect.second, 0, INFINITE_AFFECT_DURATION, 0, false);
		affectIdx++;
	}
}

auto CTitleAchievementTitle::RemoveTitleEffects(LPCHARACTER ch) -> void {
	if (!ch) return;
	
	for (uint16_t i = AFFECT_TITLE_ACHIEVEMENT_1; i <= AFFECT_TITLE_ACHIEVEMENT_3; ++i) {
		ch->RemoveAffect(i);
	}
}

auto CTitleAchievementTitle::CanActivateTogether(const BYTE title1, const BYTE title2) -> bool
{
    if (title1 == 0 || title2 == 0 || title1 == title2)
        return false;
    
    // Get effect types for both titles
    auto effects1 = GetTitleEffectTypes(title1);
    auto effects2 = GetTitleEffectTypes(title2);
    
    // Check if they share any effect types
    for (const auto& eff1 : effects1)
    {
        for (const auto& eff2 : effects2)
        {
            if (eff1 == eff2)
                return false;  // Conflict found
        }
    }
    
    return true;  // No conflicts
}

auto CTitleAchievementTitle::GetTitleEffectTypes(const BYTE titleIDX) -> std::vector<uint8_t>
{
    std::vector<uint8_t> effectTypes;
    
    if (titleIDX == 0 || titleIDX > titleAchievements.size())
        return effectTypes;
    
    const auto& titleData = titleAchievements[titleIDX - 1];
    for (const auto& effect : titleData.bonusEffects)
    {
        effectTypes.push_back(effect.first);
    }
    
    return effectTypes;
}

auto CTitleAchievementTitle::SelectTitle(const BYTE titleIDX, LPCHARACTER ch) -> void {
	if (!ch || !ch->GetDesc()) { 
		return; 
	}
#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::TitleChange, std::chrono::milliseconds(5000)))
    {
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10134;%.2f]", PULSEMANAGER_CLOCK_TO_SEC2(ch->GetPlayerID(), ePulse::TitleChange));
		return;
    }
#endif
	
	if (titleIDX == 0) {
		UpdateTitle(ch, 0);
		return;
	}
	
	if (titleIDX > titleAchievements.size()) {
		ch->ChatPacket(CHAT_TYPE_INFO, "Invalid title ID.");
		return;
	}
	
	if (ch->GetTitleAchievement() == titleIDX) {
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10138]");
		return;
	}
	
	if (!HasTitleUnlocked(ch, titleIDX)) {
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10139]");
		return;
	}
	
	// NEW: Check if this title is already active in premium slot
	if (ch->GetTitleAchievementPremium() == titleIDX) {
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10140]");
		return;
	}
	
	// NEW: Check conflict with premium title
	BYTE premiumTitle = ch->GetTitleAchievementPremium();
	if (premiumTitle > 0 && !CanActivateTogether(titleIDX, premiumTitle)) {
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10141]");
		return;
	}
	
	UpdateTitle(ch, titleIDX);
}

auto CTitleAchievementTitle::SelectTitlePremium(const BYTE titleIDX, LPCHARACTER ch) -> void
{
    if (!ch || !ch->GetDesc())
        return;
    
#ifdef ENABLE_NEWSTUFF
    if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::TitleChange, std::chrono::milliseconds(5000)))
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10134;%.2f]", PULSEMANAGER_CLOCK_TO_SEC2(ch->GetPlayerID(), ePulse::TitleChange));
        return;
    }
#endif
    
    // Check if player has premium
    if (!ch->IsVIP())
    {
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10142]");
        return;
    }
    
    if (titleIDX == 0)
    {
        UpdateTitlePremium(ch, 0);
        return;
    }
    
    if (titleIDX > titleAchievements.size())
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Invalid title ID.");
        return;
    }
    
    if (ch->GetTitleAchievementPremium() == titleIDX)
    {
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10143]");
        return;
    }
    
    if (!HasTitleUnlocked(ch, titleIDX))
    {
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10144]");
        return;
    }
    
    // Check conflict with primary title
    BYTE primaryTitle = ch->GetTitleAchievement();
    if (primaryTitle > 0 && !CanActivateTogether(primaryTitle, titleIDX))
    {
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10145]");
        return;
    }
    
    UpdateTitlePremium(ch, titleIDX);
}

auto CTitleAchievementTitle::UpdateTitlePremium(LPCHARACTER ch, const int8_t changeIdx) -> void
{
    if (!ch || !ch->GetDesc())
        return;
    
    RemoveTitleEffectsPremium(ch);
    
    if (changeIdx > 0 && changeIdx <= titleAchievements.size())
    {
        ApplyTitleEffectsPremium(ch, changeIdx);
        ch->SetTitleAchievementPremium(changeIdx);
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10146]");
    }
    else
    {
        ch->SetTitleAchievementPremium(0);
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10147]");
    }

    RecalcDungeonFastestBonuses(ch);
    ch->SendDungeonCooldown(0); // refresh effective daily limits on client after title change

    SaveTitleToDatabase(ch);
    SendToClient(ch);
}

auto CTitleAchievementTitle::ApplyTitleEffectsPremium(LPCHARACTER ch, const BYTE titleIDX) -> void
{
    if (!ch || titleIDX == 0 || titleIDX > titleAchievements.size())
        return;
    
    const auto& titleData = titleAchievements[titleIDX - 1];
    uint8_t affectIdx = 0;
    
    // Use different affect slots for premium title (4-6)
    for (const auto& effect : titleData.bonusEffects)
    {
        if (affectIdx >= 3) break;
        
        ch->AddAffect(AFFECT_TITLE_ACHIEVEMENT_PREMIUM_1 + affectIdx, 
                      aApplyInfo[effect.first].bPointType, 
                      effect.second, 0, INFINITE_AFFECT_DURATION, 0, false);
        affectIdx++;
    }
}

auto CTitleAchievementTitle::RemoveTitleEffectsPremium(LPCHARACTER ch) -> void
{
    if (!ch) return;
    
    for (uint16_t i = AFFECT_TITLE_ACHIEVEMENT_PREMIUM_1; i <= AFFECT_TITLE_ACHIEVEMENT_PREMIUM_3; ++i)
    {
        ch->RemoveAffect(i);
    }
}

auto CTitleAchievementTitle::SendToClient(LPCHARACTER ch) -> void
{
    if (!ch || !ch->GetDesc())
        return;
    
    TPacketGCTitleAchievementInfo titleInfo{};
    
    for (size_t i = 0; i < titleAchievements.size() && i < 24; ++i)
    {
        size_t titleId = i + 1;
        titleInfo.bTitleUnlocked[i] = HasTitleUnlocked(ch, titleId) ? 1 : 0;
    }
    
    int8_t currentTitle = ch->GetTitleAchievement();
    int8_t currentTitlePremium = ch->GetTitleAchievementPremium();
    
    titleInfo.bCurrentTitle = currentTitle;
    titleInfo.bCurrentTitlePremium = currentTitlePremium;
    
    // Send effects for primary title
    if (currentTitle > 0 && currentTitle <= titleAchievements.size())
    {
        const auto& titleData = titleAchievements[currentTitle - 1];
        uint8_t effectIdx = 0;
        for (const auto& effect : titleData.bonusEffects)
        {
            if (effectIdx >= 3) break;
            titleInfo.wAffType[effectIdx] = effect.first;
            titleInfo.bAffValue[effectIdx] = effect.second;
            effectIdx++;
        }
    }
    
    // Send effects for premium title
    if (currentTitlePremium > 0 && currentTitlePremium <= titleAchievements.size())
    {
        const auto& titleData = titleAchievements[currentTitlePremium - 1];
        uint8_t effectIdx = 0;
        for (const auto& effect : titleData.bonusEffects)
        {
            if (effectIdx >= 3) break;
            titleInfo.wAffTypePremium[effectIdx] = effect.first;
            titleInfo.bAffValuePremium[effectIdx] = effect.second;
            effectIdx++;
        }
    }
    
    ch->GetDesc()->Packet(&titleInfo, sizeof(TPacketGCTitleAchievementInfo));
}

auto CTitleAchievementTitle::GetTitleCount() -> uint8_t {
	return static_cast<uint8_t>(titleAchievements.size());
}

auto CTitleAchievementTitle::GetTitleBonusEffects(const BYTE titleIDX) -> std::vector<std::pair<uint8_t, uint16_t>> {
	if (titleIDX == 0 || titleIDX > titleAchievements.size()) {
		return {};
	}
	// Convert 1-based titleIDX to 0-based array index
	return titleAchievements[titleIDX - 1].bonusEffects;
}

auto CTitleAchievementTitle::CheckAllTitlesProgress(LPCHARACTER ch) -> void {
	if (!ch) return;
	
	// Check and unlock titles that meet requirements
	CheckAndUnlockTitles(ch);
}

auto CTitleAchievementTitle::SaveTitleToDatabase(LPCHARACTER ch) -> void
{
    if (!ch) return;
    ch->SetQuestFlag("AchievementTitle.active_title", ch->GetTitleAchievement());
    ch->SetQuestFlag("AchievementTitle.active_title_premium", ch->GetTitleAchievementPremium());
}

auto CTitleAchievementTitle::LoadTitleFromDatabase(LPCHARACTER ch) -> void
{
    if (!ch) return;
    
    int titleId = ch->GetQuestFlag("AchievementTitle.active_title");
    int titleIdPremium = ch->GetQuestFlag("AchievementTitle.active_title_premium");
    
    // Always remove existing affects first to prevent duplication on teleport/relog
    RemoveTitleEffects(ch);
    RemoveTitleEffectsPremium(ch);
    
    // Load primary title
    if (titleId > 0 && titleId <= (int)titleAchievements.size() && HasTitleUnlocked(ch, titleId))
    {
        ch->SetTitleAchievement(titleId);
        ApplyTitleEffects(ch, titleId);
    }
    else
    {
        ch->SetTitleAchievement(0);
    }
    
    // Load premium title
    if (titleIdPremium > 0 && titleIdPremium <= (int)titleAchievements.size() &&
        HasTitleUnlocked(ch, titleIdPremium))
    {
        if (titleId > 0 && !CanActivateTogether(titleId, titleIdPremium))
        {
            ch->SetTitleAchievementPremium(0);
        }
        else
        {
            ch->SetTitleAchievementPremium(titleIdPremium);
            ApplyTitleEffectsPremium(ch, titleIdPremium);
        }
    }
    else
    {
        ch->SetTitleAchievementPremium(0);
    }

    RecalcDungeonFastestBonuses(ch);
}

#endif