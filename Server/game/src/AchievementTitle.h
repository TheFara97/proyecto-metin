#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>

// Forward declarations
class CHARACTER;
typedef CHARACTER* LPCHARACTER;

// Title achievement data structure - simplified
struct TitleAchievementData {
	uint8_t titleId;
	std::vector<std::pair<uint8_t, uint16_t>> bonusEffects;
	std::function<bool(LPCHARACTER)> checkCondition;
};

class CTitleAchievementTitle : public singleton<CTitleAchievementTitle>
{
public:
	CTitleAchievementTitle() = default;
	~CTitleAchievementTitle() = default;

	// Core title management functions
	auto SelectTitle(BYTE titleIDX, LPCHARACTER ch) -> void;
	auto UpdateTitle(LPCHARACTER ch, int8_t changeIdx = -1) -> void;
	auto SendToClient(LPCHARACTER ch) -> void;

	// Title requirement and validation functions
	auto CheckTitleRequirement(LPCHARACTER ch, const BYTE titleIDX) -> bool;
	auto HasTitleUnlocked(LPCHARACTER ch, const BYTE titleIDX) -> bool;
	auto UnlockTitle(LPCHARACTER ch, const BYTE titleIDX) -> void;
	auto CheckAndUnlockTitles(LPCHARACTER ch) -> void;
	auto CheckAllTitlesProgress(LPCHARACTER ch) -> void;
	auto ApplyTitleEffects(LPCHARACTER ch, const BYTE titleIDX) -> void;
	auto RemoveTitleEffects(LPCHARACTER ch) -> void;

	// Title information getters
	auto GetTitleCount() -> uint8_t;
	auto GetTitleBonusEffects(const BYTE titleIDX) -> std::vector<std::pair<uint8_t, uint16_t>>;

	// Database operations
	auto SaveTitleToDatabase(LPCHARACTER ch) -> void;
	auto LoadTitleFromDatabase(LPCHARACTER ch) -> void;

	// Title system management
	auto IsValidTitleIndex(const BYTE titleIDX) -> bool;
	auto GetPlayerActiveTitle(LPCHARACTER ch) -> int8_t;

	// Progress tracking utilities
	auto GetPlayerProgress(LPCHARACTER ch, const std::string& progressType) -> uint32_t;
	auto SetPlayerProgress(LPCHARACTER ch, const std::string& progressType, uint32_t value) -> void;
	auto IncrementPlayerProgress(LPCHARACTER ch, const std::string& progressType, uint32_t amount = 1) -> void;

	// Admin/GM functions
	auto ForceUnlockTitle(LPCHARACTER ch, const BYTE titleIDX) -> bool;
	auto ResetPlayerTitles(LPCHARACTER ch) -> void;
	
	auto SelectTitlePremium(BYTE titleIDX, LPCHARACTER ch) -> void;
	auto ApplyTitleEffectsPremium(LPCHARACTER ch, const BYTE titleIDX) -> void;
	auto RemoveTitleEffectsPremium(LPCHARACTER ch) -> void;
	auto UpdateTitlePremium(LPCHARACTER ch, const int8_t changeIdx) -> void;
	auto CanActivateTogether(const BYTE title1, const BYTE title2) -> bool;
	auto RecalcDungeonBonuses(LPCHARACTER ch) -> void;

private:
	// Internal helper functions
	auto ValidateCharacter(LPCHARACTER ch) -> bool;
	auto GetTitleData(const BYTE titleIDX) -> const TitleAchievementData*;
	auto UpdateTitleNotificationFlag(LPCHARACTER ch, const BYTE titleIDX) -> void;
	auto CalculateTitleEffectValue(const BYTE titleIDX, const uint8_t effectType) -> uint16_t;
	auto GetTitleEffectTypes(const BYTE titleIDX) -> std::vector<uint8_t>;
};