#pragma once

class CPythonBattlePass : public CSingleton<CPythonBattlePass>
{
public:
	CPythonBattlePass();
	~CPythonBattlePass();

	bool Initialize(const char* filename);
	void Destroy();

	const TBattlePassParser* GetBattlePassMap(uint32_t vnum);
	void UpdateBattlePass(const TPlayerBattlePass& info);

	void ClearPlayerBattlePass() { playerBattlePass.clear(); }
	const std::vector<TPlayerBattlePass>& GetPlayerBattlePass() const { return playerBattlePass; }
	const TBattlePassSettings& GetBattlePassSettings() const { return battlePassSettings; }

	// Normal battle pass
	void SetPoints(int points) { normalPoints = points; }
	void SetPremium(bool premium) { normalPremium = premium; }
	std::pair<int, bool> GetPointsAndPremium() const { return { normalPoints, normalPremium }; }

	// NEW: Event battle pass
	void SetEventPoints(int points) { eventPoints = points; }
	void SetEventPremium(bool premium) { eventPremium = premium; }
	void SetEventActive(bool active) { eventActive = active; }
	std::pair<int, bool> GetEventPointsAndPremium() const { return { eventPoints, eventPremium }; }
	bool IsEventActive() const { return eventActive; }

private:
	std::map<uint32_t, TBattlePassParser> missionMap;
	std::vector<TPlayerBattlePass> playerBattlePass;
	TBattlePassSettings battlePassSettings;

	// Normal battle pass
	int normalPoints;
	bool normalPremium;

	// Event battle pass
	int eventPoints;
	bool eventPremium;
	bool eventActive;
};
