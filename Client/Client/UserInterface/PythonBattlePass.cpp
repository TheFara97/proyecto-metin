#include "StdAfx.h"

#ifdef _ENABLE_BATTLEPASS_
#include "PythonNetworkStream.h"
#include "../eterPack/EterPackManager.h"
#include "PythonBattlePass.h"

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/error/en.h>

static EBattlePassMissionTypes parseTrigger(const std::string& s) {
	static const std::map<std::string, EBattlePassMissionTypes> tbl = {
		{"kill",         MISSION_TYPE_KILL},
		{"sell",         MISSION_TYPE_SELL},
		{"destroy",      MISSION_TYPE_ITEM_DESTROY},
		{"drop",         MISSION_TYPE_ITEM_DROP},
		{"craft",        MISSION_TYPE_CRAFT},
		{"chest",        MISSION_TYPE_CHEST_OPEN},
		{"use_item",     MISSION_TYPE_USE_ITEM},
		{"dungeon",      MISSION_TYPE_FINISH_DUNGEON},
		{"polymorph",    MISSION_TYPE_POLYMORPH},
		{"shout",        MISSION_TYPE_MESSAGES},
		{"playtime",     MISSION_TYPE_PLAYTIME},
		{"refine",       MISSION_TYPE_REFINE},
		{"switchbot",    MISSION_TYPE_SWITCH_ITEMS},
		{"avg_damage",   MISSION_TYPE_AVERAGE_DAMAGE},
		{"kill_stone",   MISSION_TYPE_KILL_STONES},
		{"kill_boss",    MISSION_TYPE_KILL_BOSS},
		{"sash",         MISSION_TYPE_SASH},
		{"xmas_raffle",  MISSION_TYPE_CHRISTMAS_RAFFLE},
		{"xmas_map",     MISSION_TYPE_CHRISTMAS_MAP},
	};
	auto it = tbl.find(s);
	return (it != tbl.end()) ? it->second : MISSION_TYPE_KILL;
}

static EBattlePassMissionTime parseTrack(const std::string& s) {
	if (s == "weekly") return MISSION_TIME_WEEKLY;
	if (s == "event")  return MISSION_TIME_EVENT;
	return MISSION_TIME_DAILY;
}

CPythonBattlePass::CPythonBattlePass()
{
	missionMap.clear();
	playerBattlePass.clear();
	normalPoints = 0;
	normalPremium = false;
	eventPoints = 0;
	eventPremium = false;
	eventActive = false;
}

CPythonBattlePass::~CPythonBattlePass()
{
	missionMap.clear();
	playerBattlePass.clear();
}

bool CPythonBattlePass::Initialize(const char* filename)
{
	if (!missionMap.empty())
		missionMap.clear();

	CMappedFile File;
	LPCVOID pData;
	if (!CEterPackManager::Instance().Get(File, filename, &pData))
		return false;

	rapidjson::Document document;
	document.Parse(reinterpret_cast<const char*>(pData), File.Size());

	if (document.HasParseError())
	{
		TraceError("JSON Parse Error: %s (offset: %zu)",
			rapidjson::GetParseError_En(document.GetParseError()),
			document.GetErrorOffset());
		return false;
	}

	// ── season ───────────────────────────────────────────────────────────────
	if (document.HasMember("season") && document["season"].IsObject()) {
		const auto& s = document["season"];
		if (s.HasMember("daily")   && s["daily"].IsUint())   battlePassSettings.maxDailyMissions  = static_cast<uint8_t>(s["daily"].GetUint());
		if (s.HasMember("weekly")  && s["weekly"].IsUint())  battlePassSettings.maxWeeklyMissions = static_cast<uint8_t>(s["weekly"].GetUint());
		if (s.HasMember("endsAt")  && s["endsAt"].IsUint64()) battlePassSettings.endTime           = static_cast<uint32_t>(s["endsAt"].GetUint64());
	}

	// ── event ─────────────────────────────────────────────────────────────────
	if (document.HasMember("event") && document["event"].IsObject()) {
		const auto& e = document["event"];
		if (e.HasMember("active")   && e["active"].IsBool())    battlePassSettings.eventActive      = e["active"].GetBool();
		if (e.HasMember("endsAt")   && e["endsAt"].IsUint64())  battlePassSettings.eventEndTime     = static_cast<uint32_t>(e["endsAt"].GetUint64());
		if (e.HasMember("slots")    && e["slots"].IsUint())     battlePassSettings.maxEventMissions = static_cast<uint8_t>(e["slots"].GetUint());
	}

	// ── rewards ───────────────────────────────────────────────────────────────
	auto parseRewards = [](const rapidjson::Value& arr, std::vector<std::pair<int32_t,int32_t>>& out) {
		if (!arr.IsArray()) return;
		for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
			const auto& e = arr[i];
			if (e.IsArray() && e.Size() == 2 && e[0].IsUint() && e[1].IsUint())
				out.emplace_back(e[0].GetUint(), e[1].GetUint());
		}
	};
	if (document.HasMember("rewards") && document["rewards"].IsObject()) {
		const auto& r = document["rewards"];
		if (r.HasMember("free"))    parseRewards(r["free"],    battlePassSettings.rewardsFree);
		if (r.HasMember("premium")) parseRewards(r["premium"], battlePassSettings.rewardsPremium);
		if (r.HasMember("event"))   parseRewards(r["event"],   battlePassSettings.rewardsEvent);
	}

	// ── missions ──────────────────────────────────────────────────────────────
	if (document.HasMember("missions") && document["missions"].IsArray()) {
		const auto& missions = document["missions"];
		for (rapidjson::SizeType i = 0; i < missions.Size(); ++i) {
			const auto& node = missions[i];
			if (!node.IsObject()) continue;

			TBattlePassParser mission;
			mission.id = static_cast<uint16_t>(i);

			mission.name        = (node.HasMember("name")    && node["name"].IsString())    ? node["name"].GetString()    : "Unknown";
			mission.missionType = (node.HasMember("trigger") && node["trigger"].IsString()) ? parseTrigger(node["trigger"].GetString()) : MISSION_TYPE_KILL;
			mission.timeInfo    = (node.HasMember("track")   && node["track"].IsString())   ? parseTrack(node["track"].GetString())     : MISSION_TIME_DAILY;
			mission.maxProgress = (node.HasMember("goal")    && node["goal"].IsUint())      ? node["goal"].GetUint()    : 1;
			mission.pointsReward= (node.HasMember("pts")     && node["pts"].IsUint())       ? static_cast<uint16_t>(node["pts"].GetUint()) : 0;
			mission.battlePassType = (mission.timeInfo == MISSION_TIME_EVENT) ? BATTLE_PASS_EVENT : BATTLE_PASS_NORMAL;

			// target: "any" → GLOBAL; array → SPECIFIC + fill vnums
			mission.missionSpecific = MISSION_TYPE_GLOBAL;
			if (node.HasMember("target")) {
				const auto& tgt = node["target"];
				if (tgt.IsArray()) {
					mission.missionSpecific = MISSION_TYPE_SPECIFIC;
					for (rapidjson::SizeType j = 0; j < tgt.Size(); ++j)
						if (tgt[j].IsUint()) mission.vnums.push_back(tgt[j].GetUint());
				}
			}

			missionMap[i] = mission;
		}
	}

	return true;
}


void CPythonBattlePass::Destroy()
{
	missionMap.clear(); //Clear map
	playerBattlePass.clear();
}

const TBattlePassParser* CPythonBattlePass::GetBattlePassMap(uint32_t vnum)
{
	auto it = missionMap.find(vnum);
	if (it == missionMap.end())
		return NULL;

	return &it->second;
}

void CPythonBattlePass::UpdateBattlePass(const TPlayerBattlePass& info)
{
	playerBattlePass.push_back(info);
}

PyObject* battlepassGetPlayerInfo(PyObject* poSelf, PyObject* poArgs)
{
	int battlePassType;
	if (!PyTuple_GetInteger(poArgs, 0, &battlePassType))
		return Py_BuildException();

	if (battlePassType == BATTLE_PASS_EVENT) {
		const auto& [points, isPremium] = CPythonBattlePass::Instance().GetEventPointsAndPremium();
		return Py_BuildValue("ii", points, static_cast<uint8_t>(isPremium));
	}
	else {
		const auto& [points, isPremium] = CPythonBattlePass::Instance().GetPointsAndPremium();
		return Py_BuildValue("ii", points, static_cast<uint8_t>(isPremium));
	}
}

PyObject* battlepassIsEventActive(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonBattlePass::Instance().IsEventActive() ? 1 : 0);
}

PyObject* battlepassGetEventSettings(PyObject* poSelf, PyObject* poArgs)
{
	const auto settings = CPythonBattlePass::Instance().GetBattlePassSettings();
	return Py_BuildValue("iii", settings.eventActive ? 1 : 0, settings.eventEndTime, settings.maxEventMissions);
}

PyObject* battlepassRewardsEvent(PyObject* poSelf, PyObject* poArgs)
{
	const auto battlepassList = CPythonBattlePass::Instance().GetBattlePassSettings();

	const auto list = PyList_New(battlepassList.rewardsEvent.size());

	int32_t i = 0;
	for (const auto& battlepass : battlepassList.rewardsEvent) {
		const auto dict = PyDict_New();
		PyDict_SetItem(dict, PyString_FromString("vnum"), Py_BuildValue("I", battlepass.first));
		PyDict_SetItem(dict, PyString_FromString("count"), Py_BuildValue("I", battlepass.second));

		PyList_SET_ITEM(list, i++, dict);
	}

	return Py_BuildValue("O", list);
}

PyObject* battlepassGetMissionInfo(PyObject* poSelf, PyObject* poArgs)
{
	int vnum;
	if (!PyTuple_GetInteger(poArgs, 0, &vnum))
		return Py_BuildException();

	const TBattlePassParser* mission = CPythonBattlePass::Instance().GetBattlePassMap(vnum);
	if (!mission)
		return Py_BuildValue("iis", 0, 0, "None");

	return Py_BuildValue("iis", mission->maxProgress, mission->pointsReward, mission->name.c_str());
}

PyObject* battlepassGetMissionType(PyObject* poSelf, PyObject* poArgs)
{
	int vnum;
	if (!PyTuple_GetInteger(poArgs, 0, &vnum))
		return Py_BuildException();

	const TBattlePassParser* mission = CPythonBattlePass::Instance().GetBattlePassMap(vnum);
	if (!mission)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", mission->missionType);
}

PyObject* battlepassGetMissions(PyObject* poSelf, PyObject* poArgs)
{
	const auto battlepassList = CPythonBattlePass::Instance().GetPlayerBattlePass();

	const auto list = PyList_New(battlepassList.size());

	int32_t i = 0;
	for (const auto& battlepass : battlepassList) {
		const auto dict = PyDict_New();
		PyDict_SetItem(dict, PyString_FromString("id"), Py_BuildValue("I", battlepass.missionID));
		PyDict_SetItem(dict, PyString_FromString("type"), Py_BuildValue("I", battlepass.type));

		PyList_SET_ITEM(list, i++, dict);
	}

	return Py_BuildValue("O", list);
}

PyObject* battlepassGetMissionProgress(PyObject* poSelf, PyObject* poArgs)
{
	uint16_t vnum;
	if (!PyTuple_GetInteger(poArgs, 0, &vnum))
		return Py_BuildException();

	const auto battlepassList = CPythonBattlePass::Instance().GetPlayerBattlePass();
	auto it = std::find_if(battlepassList.begin(), battlepassList.end(), [vnum](const TPlayerBattlePass& elem) {
		return elem.missionID == vnum;
		});

	if (it != battlepassList.end()) {
		return Py_BuildValue("iii", it->progress, it->endTime, it->type);
	}

	return Py_BuildValue("iii", 0, 0, 0);
}

PyObject* battlepassRewardsFree(PyObject* poSelf, PyObject* poArgs)
{
	const auto battlepassList = CPythonBattlePass::Instance().GetBattlePassSettings();

	const auto list = PyList_New(battlepassList.rewardsFree.size());

	int32_t i = 0;
	for (const auto& battlepass : battlepassList.rewardsFree) {
		const auto dict = PyDict_New();
		PyDict_SetItem(dict, PyString_FromString("vnum"), Py_BuildValue("I", battlepass.first));
		PyDict_SetItem(dict, PyString_FromString("count"), Py_BuildValue("I", battlepass.second));

		PyList_SET_ITEM(list, i++, dict);
	}

	return Py_BuildValue("O", list);
}

PyObject* battlepassRewardsPremium(PyObject* poSelf, PyObject* poArgs)
{
	const auto battlepassList = CPythonBattlePass::Instance().GetBattlePassSettings();

	const auto list = PyList_New(battlepassList.rewardsPremium.size());

	int32_t i = 0;
	for (const auto& battlepass : battlepassList.rewardsPremium) {
		const auto dict = PyDict_New();
		PyDict_SetItem(dict, PyString_FromString("vnum"), Py_BuildValue("I", battlepass.first));
		PyDict_SetItem(dict, PyString_FromString("count"), Py_BuildValue("I", battlepass.second));

		PyList_SET_ITEM(list, i++, dict);
	}

	return Py_BuildValue("O", list);
}

void initBattlePass()
{
	static PyMethodDef s_methods[] =
	{
		{ "GetMissionInfo",			battlepassGetMissionInfo,		METH_VARARGS },
		{ "GetMissionType",			battlepassGetMissionType,		METH_VARARGS },
		{ "GetMissions",			battlepassGetMissions,			METH_VARARGS },

		{ "GetRewardsFree",			battlepassRewardsFree,			METH_VARARGS },
		{ "GetRewardsPremium",		battlepassRewardsPremium,		METH_VARARGS },
		{ "GetRewardsEvent",		battlepassRewardsEvent,			METH_VARARGS },

		{ "GetMissionProgress",		battlepassGetMissionProgress,	METH_VARARGS },
		{ "GetPlayerInfo",			battlepassGetPlayerInfo,		METH_VARARGS },

		// Event battle pass functions
		{ "IsEventActive",			battlepassIsEventActive,		METH_VARARGS },
		{ "GetEventSettings",		battlepassGetEventSettings,		METH_VARARGS },

		{ NULL, NULL, NULL },
	};

	PyObject* poModule = Py_InitModule("battlepass", s_methods);
}
#endif
