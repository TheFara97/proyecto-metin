#include "PythonrootlibManager.h"
#include "../../Extern/include/Python27/Python.h"
#include "StdAfx.h"
#ifdef _DEBUG
#pragma comment (lib, "rootlib_d.lib")
#else
#pragma comment (lib, "rootlib.lib")
#endif

struct rootlib_SMethodDef
{
	const char* func_name;
	void (*func)();
};

PyMODINIT_FUNC initcolorInfo();
PyMODINIT_FUNC initconsoleModule();
PyMODINIT_FUNC initconstInfo();
PyMODINIT_FUNC initdebugInfo();
PyMODINIT_FUNC initdragon_soul_refine_settings();
PyMODINIT_FUNC initemotion();
PyMODINIT_FUNC initexception();
PyMODINIT_FUNC initgame();
PyMODINIT_FUNC initinterfaceModule();
PyMODINIT_FUNC initintroCreate();
PyMODINIT_FUNC initintroEmpire();
PyMODINIT_FUNC initintroLoading();
PyMODINIT_FUNC initintroLogin();
PyMODINIT_FUNC initintroLogo();
PyMODINIT_FUNC initintroSelect();
PyMODINIT_FUNC initlocaleInfo();
PyMODINIT_FUNC initmouseModule();
PyMODINIT_FUNC initmusicInfo();
PyMODINIT_FUNC initnetworkModule();
PyMODINIT_FUNC initnew_uiWeeklyRank();
PyMODINIT_FUNC initplayerSettingModule();
PyMODINIT_FUNC initPrototype();
PyMODINIT_FUNC initRootLibCythonizer();
PyMODINIT_FUNC initserverCommandParser();
PyMODINIT_FUNC initserverInfo();
PyMODINIT_FUNC initstringCommander();
PyMODINIT_FUNC initsystem();
PyMODINIT_FUNC inittest_affect();
PyMODINIT_FUNC initui();
PyMODINIT_FUNC inituiAcce();
PyMODINIT_FUNC inituiAffectShower();
PyMODINIT_FUNC inituiArtefaktSystem();
PyMODINIT_FUNC inituiAttachMetin();
PyMODINIT_FUNC inituiAura();
PyMODINIT_FUNC inituiAutoban();
PyMODINIT_FUNC inituiBattlePass();
PyMODINIT_FUNC inituiBlend();
PyMODINIT_FUNC inituiBonus();
PyMODINIT_FUNC inituiBonusChanger();
PyMODINIT_FUNC inituiBuffNpc();
PyMODINIT_FUNC inituiCandidate();
PyMODINIT_FUNC inituiCards();
PyMODINIT_FUNC inituiCharacter();
PyMODINIT_FUNC inituiChat();
PyMODINIT_FUNC inituiChestDropInfo();
PyMODINIT_FUNC inituiCollections();
PyMODINIT_FUNC inituiCollectWindow();
PyMODINIT_FUNC inituiCommon();
PyMODINIT_FUNC inituiCube();
PyMODINIT_FUNC inituiCubeRenewal();
PyMODINIT_FUNC inituiDepoIs();
PyMODINIT_FUNC inituiDragonSoul();
PyMODINIT_FUNC inituiDungeonInfo();
PyMODINIT_FUNC inituiEquipmentDialog();
PyMODINIT_FUNC inituiEventCalendar();
PyMODINIT_FUNC inituiEx();
PyMODINIT_FUNC inituiExchange();
PyMODINIT_FUNC inituiGameButton();
PyMODINIT_FUNC inituiGameOption();
PyMODINIT_FUNC inituiGiftCodeWindow();
PyMODINIT_FUNC inituiGuild();
PyMODINIT_FUNC inituiHelp();
PyMODINIT_FUNC inituiIkonki();
PyMODINIT_FUNC inituiInventory();
PyMODINIT_FUNC inituiItemShop();
PyMODINIT_FUNC inituiKeyChange();
PyMODINIT_FUNC inituiLegendDamageWindow();
PyMODINIT_FUNC inituiLocation();
PyMODINIT_FUNC inituiMapaSw();
PyMODINIT_FUNC inituiMapNameShower();
PyMODINIT_FUNC inituiMarbleShop();
PyMODINIT_FUNC inituiMessenger();
PyMODINIT_FUNC inituiMiniMap();
PyMODINIT_FUNC inituiMiniMapDungeonInfo();
PyMODINIT_FUNC inituiMount();
PyMODINIT_FUNC inituiOdlamki();
PyMODINIT_FUNC inituiOfflineShop();
PyMODINIT_FUNC inituiOfflineShopBuilder();
PyMODINIT_FUNC inituiOption();
PyMODINIT_FUNC inituiParty();
PyMODINIT_FUNC inituiPhaseCurtain();
PyMODINIT_FUNC inituiPickEtc();
PyMODINIT_FUNC inituiPickMoney();
PyMODINIT_FUNC inituiPlayerGauge();
PyMODINIT_FUNC inituiPointReset();
PyMODINIT_FUNC inituiPotions();
PyMODINIT_FUNC inituiPrivateShopBuilder();
PyMODINIT_FUNC inituiQuest();
PyMODINIT_FUNC inituiRefine();
PyMODINIT_FUNC inituiRemoveItem();
PyMODINIT_FUNC inituiResp();
PyMODINIT_FUNC inituiRestart();
PyMODINIT_FUNC inituiSafebox();
PyMODINIT_FUNC inituiScriptLocale();
PyMODINIT_FUNC inituiselectitem();
PyMODINIT_FUNC inituiSelectMusic();
PyMODINIT_FUNC inituiShop();
PyMODINIT_FUNC inituiShopSearch();
PyMODINIT_FUNC inituiSkillChoose();
PyMODINIT_FUNC inituiStoneDetach();
PyMODINIT_FUNC inituiSwitchBot();
PyMODINIT_FUNC inituiSystem();
PyMODINIT_FUNC inituiSystemOption();
PyMODINIT_FUNC inituiTarget();
PyMODINIT_FUNC inituiTaskBar();
PyMODINIT_FUNC inituiTip();
PyMODINIT_FUNC inituiToolTip();
PyMODINIT_FUNC inituiUploadMark();
PyMODINIT_FUNC inituiWarpShower();
PyMODINIT_FUNC inituiWeb();
PyMODINIT_FUNC inituiWhisper();
PyMODINIT_FUNC inituiWonExchange();
PyMODINIT_FUNC initutils();
PyMODINIT_FUNC initzmianaEq();
PyMODINIT_FUNC init_Pety();

rootlib_SMethodDef rootlib_init_methods[] =
{
	{ "colorInfo", initcolorInfo },
	{ "consoleModule", initconsoleModule },
	{ "constInfo", initconstInfo },
	{ "debugInfo", initdebugInfo },
	{ "dragon_soul_refine_settings", initdragon_soul_refine_settings },
	{ "emotion", initemotion },
	{ "exception", initexception },
	{ "game", initgame },
	{ "interfaceModule", initinterfaceModule },
	{ "introCreate", initintroCreate },
	{ "introEmpire", initintroEmpire },
	{ "introLoading", initintroLoading },
	{ "introLogin", initintroLogin },
	{ "introLogo", initintroLogo },
	{ "introSelect", initintroSelect },
	{ "localeInfo", initlocaleInfo },
	{ "mouseModule", initmouseModule },
	{ "musicInfo", initmusicInfo },
	{ "networkModule", initnetworkModule },
	{ "new_uiWeeklyRank", initnew_uiWeeklyRank },
	{ "playerSettingModule", initplayerSettingModule },
	{ "Prototype", initPrototype },
	{ "RootLibCythonizer", initRootLibCythonizer },
	{ "serverCommandParser", initserverCommandParser },
	{ "serverInfo", initserverInfo },
	{ "stringCommander", initstringCommander },
	{ "system", initsystem },
	{ "test_affect", inittest_affect },
	{ "ui", initui },
	{ "uiAcce", inituiAcce },
	{ "uiAffectShower", inituiAffectShower },
	{ "uiArtefaktSystem", inituiArtefaktSystem },
	{ "uiAttachMetin", inituiAttachMetin },
	{ "uiAura", inituiAura },
	{ "uiAutoban", inituiAutoban },
	{ "uiBattlePass", inituiBattlePass },
	{ "uiBlend", inituiBlend },
	{ "uiBonus", inituiBonus },
	{ "uiBonusChanger", inituiBonusChanger },
	{ "uiBuffNpc", inituiBuffNpc },
	{ "uiCandidate", inituiCandidate },
	{ "uiCards", inituiCards },
	{ "uiCharacter", inituiCharacter },
	{ "uiChat", inituiChat },
	{ "uiChestDropInfo", inituiChestDropInfo },
	{ "uiCollections", inituiCollections },
	{ "uiCollectWindow", inituiCollectWindow },
	{ "uiCommon", inituiCommon },
	{ "uiCube", inituiCube },
	{ "uiCubeRenewal", inituiCubeRenewal },
	{ "uiDepoIs", inituiDepoIs },
	{ "uiDragonSoul", inituiDragonSoul },
	{ "uiDungeonInfo", inituiDungeonInfo },
	{ "uiEquipmentDialog", inituiEquipmentDialog },
	{ "uiEventCalendar", inituiEventCalendar },
	{ "uiEx", inituiEx },
	{ "uiExchange", inituiExchange },
	{ "uiGameButton", inituiGameButton },
	{ "uiGameOption", inituiGameOption },
	{ "uiGiftCodeWindow", inituiGiftCodeWindow },
	{ "uiGuild", inituiGuild },
	{ "uiHelp", inituiHelp },
	{ "uiIkonki", inituiIkonki },
	{ "uiInventory", inituiInventory },
	{ "uiItemShop", inituiItemShop },
	{ "uiKeyChange", inituiKeyChange },
	{ "uiLegendDamageWindow", inituiLegendDamageWindow },
	{ "uiLocation", inituiLocation },
	{ "uiMapaSw", inituiMapaSw },
	{ "uiMapNameShower", inituiMapNameShower },
	{ "uiMarbleShop", inituiMarbleShop },
	{ "uiMessenger", inituiMessenger },
	{ "uiMiniMap", inituiMiniMap },
	{ "uiMiniMapDungeonInfo", inituiMiniMapDungeonInfo },
	{ "uiMount", inituiMount },
	{ "uiOdlamki", inituiOdlamki },
	{ "uiOfflineShop", inituiOfflineShop },
	{ "uiOfflineShopBuilder", inituiOfflineShopBuilder },
	{ "uiOption", inituiOption },
	{ "uiParty", inituiParty },
	{ "uiPhaseCurtain", inituiPhaseCurtain },
	{ "uiPickEtc", inituiPickEtc },
	{ "uiPickMoney", inituiPickMoney },
	{ "uiPlayerGauge", inituiPlayerGauge },
	{ "uiPointReset", inituiPointReset },
	{ "uiPotions", inituiPotions },
	{ "uiPrivateShopBuilder", inituiPrivateShopBuilder },
	{ "uiQuest", inituiQuest },
	{ "uiRefine", inituiRefine },
	{ "uiRemoveItem", inituiRemoveItem },
	{ "uiResp", inituiResp },
	{ "uiRestart", inituiRestart },
	{ "uiSafebox", inituiSafebox },
	{ "uiScriptLocale", inituiScriptLocale },
	{ "uiselectitem", inituiselectitem },
	{ "uiSelectMusic", inituiSelectMusic },
	{ "uiShop", inituiShop },
	{ "uiShopSearch", inituiShopSearch },
	{ "uiSkillChoose", inituiSkillChoose },
	{ "uiStoneDetach", inituiStoneDetach },
	{ "uiSwitchBot", inituiSwitchBot },
	{ "uiSystem", inituiSystem },
	{ "uiSystemOption", inituiSystemOption },
	{ "uiTarget", inituiTarget },
	{ "uiTaskBar", inituiTaskBar },
	{ "uiTip", inituiTip },
	{ "uiToolTip", inituiToolTip },
	{ "uiUploadMark", inituiUploadMark },
	{ "uiWarpShower", inituiWarpShower },
	{ "uiWeb", inituiWeb },
	{ "uiWhisper", inituiWhisper },
	{ "uiWonExchange", inituiWonExchange },
	{ "utils", initutils },
	{ "zmianaEq", initzmianaEq },
	{ "_Pety", init_Pety },
	{ NULL, NULL },
};

static PyObject* rootlib_isExist(PyObject* self, PyObject* args)
{
	char* func_name;

	if (!PyArg_ParseTuple(args, "s", &func_name))
		return NULL;

	for (int i = 0; NULL != rootlib_init_methods[i].func_name; i++)
	{
		if (0 == _stricmp(rootlib_init_methods[i].func_name, func_name))
		{
			return Py_BuildValue("i", 1);
		}
	}
	return Py_BuildValue("i", 0);
}

static PyObject* rootlib_moduleImport(PyObject* self, PyObject* args)
{
	char* func_name;

	if (!PyArg_ParseTuple(args, "s", &func_name))
		return NULL;

	for (int i = 0; NULL != rootlib_init_methods[i].func_name; i++)
	{
		if (0 == _stricmp(rootlib_init_methods[i].func_name, func_name))
		{
			rootlib_init_methods[i].func();
			if (PyErr_Occurred())
				return NULL;
			PyObject* m = PyDict_GetItemString(PyImport_GetModuleDict(), rootlib_init_methods[i].func_name);
			if (m == NULL) {
				PyErr_SetString(PyExc_SystemError,
					"dynamic module not initialized properly");
				return NULL;
			}
			Py_INCREF(m);
			return Py_BuildValue("S", m);
		}
	}
	return NULL;
}

static PyObject* rootlib_getList(PyObject* self, PyObject* args)
{
	int iTupleSize = 0;
	while (NULL != rootlib_init_methods[iTupleSize].func_name) { iTupleSize++; }

	PyObject* retTuple = PyTuple_New(iTupleSize);
	for (int i = 0; NULL != rootlib_init_methods[i].func_name; i++)
	{
		PyObject* retSubString = PyString_FromString(rootlib_init_methods[i].func_name);
		PyTuple_SetItem(retTuple, i, retSubString);
	}
	return retTuple;
}

void initrootlibManager()
{
	static struct PyMethodDef methods[] =
	{
		{"isExist", rootlib_isExist, METH_VARARGS},
		{"moduleImport", rootlib_moduleImport, METH_VARARGS},
		{"getList", rootlib_getList, METH_VARARGS},
		{NULL, NULL},
	};

	PyObject* m;
	m = Py_InitModule("rootlib", methods);
}
