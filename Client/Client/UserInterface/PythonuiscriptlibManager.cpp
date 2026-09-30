#include "PythonuiscriptlibManager.h"
#include "../../Extern/include/Python27/Python.h"
#ifdef _DEBUG
	#pragma comment (lib, "uiscriptlib_d.lib")
#else
	#pragma comment (lib, "uiscriptlib.lib")
#endif

struct uiscriptlib_SMethodDef
{
	const char* func_name;
	void (*func)();
};

PyMODINIT_FUNC initacceptguildwardialog();
PyMODINIT_FUNC initAcce_AbsorbWindow();
PyMODINIT_FUNC initAcce_CombineWindow();
PyMODINIT_FUNC initArtefaktSystemWindow();
PyMODINIT_FUNC initAtlasWindow();
PyMODINIT_FUNC initAttachStoneDialog();
PyMODINIT_FUNC initAura_AbsorbWindow();
PyMODINIT_FUNC initAura_LevelUpWindow();
PyMODINIT_FUNC initBattlepassDialog();
PyMODINIT_FUNC initBeltInventoryWindow();
PyMODINIT_FUNC initBonus();
PyMODINIT_FUNC initBonusChanger();
PyMODINIT_FUNC initBuffNpcCreateWindow();
PyMODINIT_FUNC initBuffNpcRenameWindow();
PyMODINIT_FUNC initBuffNpcSelectSkillWindow();
PyMODINIT_FUNC initBuffNpcWindow();
PyMODINIT_FUNC initBuffNpcWindow_new();
PyMODINIT_FUNC initBuildGuildBuildingWindow();
PyMODINIT_FUNC initChangeGradeNameDialog();
PyMODINIT_FUNC initChangePasswordDialog();
PyMODINIT_FUNC initCharacterWindow();
PyMODINIT_FUNC initChatSettingWindow();
PyMODINIT_FUNC initChestDropInfoWindow();
PyMODINIT_FUNC initCollectionsWindow();
PyMODINIT_FUNC initCollectWindow();
PyMODINIT_FUNC initConnectingDialog();
PyMODINIT_FUNC initCostumeWindow();
PyMODINIT_FUNC initCreateCharacterWindow();
PyMODINIT_FUNC initCubeRenewalWindow();
PyMODINIT_FUNC initCubeResultWindow();
PyMODINIT_FUNC initCubeWindow();
PyMODINIT_FUNC initDeclareGuildWarDialog();
PyMODINIT_FUNC initDepoIsWindow();
PyMODINIT_FUNC initDopalaczeWindow();
PyMODINIT_FUNC initDragonSoulRefineWindow();
PyMODINIT_FUNC initDragonSoulWindow();
PyMODINIT_FUNC initDungeonInfoWindow();
PyMODINIT_FUNC initDungeonRankingWindow();
PyMODINIT_FUNC initEquipmentDialog();
PyMODINIT_FUNC initEventCalendar();
PyMODINIT_FUNC initExchangeDialog();
PyMODINIT_FUNC initExpandedMoneyTaskBar();
PyMODINIT_FUNC initExpandedTaskBar();
PyMODINIT_FUNC initExtendedInventoryWindow();
PyMODINIT_FUNC initFastEquip();
PyMODINIT_FUNC initFishingWindow();
PyMODINIT_FUNC initGameOptionDialog();
PyMODINIT_FUNC initGameOptionDialog_forMobile();
PyMODINIT_FUNC initGameWindow();
PyMODINIT_FUNC initGiftCodeWindow();
PyMODINIT_FUNC initGuildWindow();
PyMODINIT_FUNC initGuildWindow_BaseInfoPage();
PyMODINIT_FUNC initGuildWindow_BoardPage();
PyMODINIT_FUNC initGuildWindow_GradePage();
PyMODINIT_FUNC initGuildWindow_GuildInfoPage();
PyMODINIT_FUNC initGuildWindow_GuildInfoPage_Eu();
PyMODINIT_FUNC initGuildWindow_GuildInfoPage_Jp();
PyMODINIT_FUNC initGuildWindow_GuildSkillPage();
PyMODINIT_FUNC initGuildWindow_MemberPage();
PyMODINIT_FUNC initHelpWindow();
PyMODINIT_FUNC initHelpWindow2();
PyMODINIT_FUNC initHorseShoe();
PyMODINIT_FUNC initIkonki();
PyMODINIT_FUNC initIMEKOR();
PyMODINIT_FUNC initInputDialog();
PyMODINIT_FUNC initInputDialogWithDescription();
PyMODINIT_FUNC initInputDialogWithDescription2();
PyMODINIT_FUNC initInventoryWindow();
PyMODINIT_FUNC initInventoryWindowEx();
PyMODINIT_FUNC initItemShopEditorWindow();
PyMODINIT_FUNC initItemShopPopUpDialog();
PyMODINIT_FUNC initItemShopSelectAttributeDialog();
PyMODINIT_FUNC initItemShopSelectList();
PyMODINIT_FUNC initItemShopWindow();
PyMODINIT_FUNC initKeyChange_Window();
PyMODINIT_FUNC initLoadingWindow();
PyMODINIT_FUNC initLocationRenderMapDialog();
PyMODINIT_FUNC initLocationWindow();
PyMODINIT_FUNC initMallWindow();
PyMODINIT_FUNC initMapaSw();
PyMODINIT_FUNC initMarkListWindow();
PyMODINIT_FUNC initMarmur_Window();
PyMODINIT_FUNC initMessengerWindow();
PyMODINIT_FUNC initMiniGameRumiGamePage();
PyMODINIT_FUNC initMiniGameRumiWaitingPage();
PyMODINIT_FUNC initMiniGameWindow();
PyMODINIT_FUNC initMiniMap();
PyMODINIT_FUNC initMiniMapDungeonInfo();
PyMODINIT_FUNC initMoneyInputDialog();
PyMODINIT_FUNC initMount_Window();
PyMODINIT_FUNC initMouseButtonWindow();
PyMODINIT_FUNC initMoveChannelDialog();
PyMODINIT_FUNC initMusicListWindow();
PyMODINIT_FUNC initOdlamkiWindow();
PyMODINIT_FUNC initPartyMemberInfoBoard();
PyMODINIT_FUNC initPasswordDialog();
PyMODINIT_FUNC initPickEtcDialog();
PyMODINIT_FUNC initPickEtcDialog2();
PyMODINIT_FUNC initPickMoneyDialog();
PyMODINIT_FUNC initPopUpDialog();
PyMODINIT_FUNC initPrivateShopBuilder();
PyMODINIT_FUNC initQuestDialog();
PyMODINIT_FUNC initQuestionDialog();
PyMODINIT_FUNC initQuestionDialog2();
PyMODINIT_FUNC initQueueWindow();
PyMODINIT_FUNC initRefineDialog();
PyMODINIT_FUNC initRemoveItemDialog();
PyMODINIT_FUNC initRespDialog();
PyMODINIT_FUNC initRespMiniMapDialog();
PyMODINIT_FUNC initRespRenderMapDialog();
PyMODINIT_FUNC initRespRenderTargetDialog();
PyMODINIT_FUNC initRestartDialog();
PyMODINIT_FUNC initRightMouseButtonWindow();
PyMODINIT_FUNC initRuneWindow();
PyMODINIT_FUNC initSafeBoxWindow();
PyMODINIT_FUNC initSelectCharacterWindow();
PyMODINIT_FUNC initSelectEmpireWindow();
PyMODINIT_FUNC initSelectItemWindow();
PyMODINIT_FUNC initShopDialog();
PyMODINIT_FUNC initShopSearch();
PyMODINIT_FUNC initSkillChooseWindow();
PyMODINIT_FUNC initSkillPointResetDialog();
PyMODINIT_FUNC initStoneDetach();
PyMODINIT_FUNC initSystemDialog();
PyMODINIT_FUNC initSystemDialog_forMall();
PyMODINIT_FUNC initSystemDialog_forPortal();
PyMODINIT_FUNC initSystemOptionDialog();
PyMODINIT_FUNC initSystemsWindow();
PyMODINIT_FUNC initSystemsWindow_new();
PyMODINIT_FUNC initTaskBar();
PyMODINIT_FUNC initWarteSchleife();
PyMODINIT_FUNC initWebWindow();
PyMODINIT_FUNC initWeeklyRank();
PyMODINIT_FUNC initWhisperDialog();
PyMODINIT_FUNC initWonExchangeWindow();
PyMODINIT_FUNC init_Pety_script();

uiscriptlib_SMethodDef uiscriptlib_init_methods[] =
{
	{ "acceptguildwardialog", initacceptguildwardialog },
	{ "Acce_AbsorbWindow", initAcce_AbsorbWindow },
	{ "Acce_CombineWindow", initAcce_CombineWindow },
	{ "ArtefaktSystemWindow", initArtefaktSystemWindow },
	{ "AtlasWindow", initAtlasWindow },
	{ "AttachStoneDialog", initAttachStoneDialog },
	{ "Aura_AbsorbWindow", initAura_AbsorbWindow },
	{ "Aura_LevelUpWindow", initAura_LevelUpWindow },
	{ "BattlepassDialog", initBattlepassDialog },
	{ "BeltInventoryWindow", initBeltInventoryWindow },
	{ "Bonus", initBonus },
	{ "BonusChanger", initBonusChanger },
	{ "BuffNpcCreateWindow", initBuffNpcCreateWindow },
	{ "BuffNpcRenameWindow", initBuffNpcRenameWindow },
	{ "BuffNpcSelectSkillWindow", initBuffNpcSelectSkillWindow },
	{ "BuffNpcWindow", initBuffNpcWindow },
	{ "BuffNpcWindow_new", initBuffNpcWindow_new },
	{ "BuildGuildBuildingWindow", initBuildGuildBuildingWindow },
	{ "ChangeGradeNameDialog", initChangeGradeNameDialog },
	{ "ChangePasswordDialog", initChangePasswordDialog },
	{ "CharacterWindow", initCharacterWindow },
	{ "ChatSettingWindow", initChatSettingWindow },
	{ "ChestDropInfoWindow", initChestDropInfoWindow },
	{ "CollectionsWindow", initCollectionsWindow },
	{ "CollectWindow", initCollectWindow },
	{ "ConnectingDialog", initConnectingDialog },
	{ "CostumeWindow", initCostumeWindow },
	{ "CreateCharacterWindow", initCreateCharacterWindow },
	{ "CubeRenewalWindow", initCubeRenewalWindow },
	{ "CubeResultWindow", initCubeResultWindow },
	{ "CubeWindow", initCubeWindow },
	{ "DeclareGuildWarDialog", initDeclareGuildWarDialog },
	{ "DepoIsWindow", initDepoIsWindow },
	{ "DopalaczeWindow", initDopalaczeWindow },
	{ "DragonSoulRefineWindow", initDragonSoulRefineWindow },
	{ "DragonSoulWindow", initDragonSoulWindow },
	{ "DungeonInfoWindow", initDungeonInfoWindow },
	{ "DungeonRankingWindow", initDungeonRankingWindow },
	{ "EquipmentDialog", initEquipmentDialog },
	{ "EventCalendar", initEventCalendar },
	{ "ExchangeDialog", initExchangeDialog },
	{ "ExpandedMoneyTaskBar", initExpandedMoneyTaskBar },
	{ "ExpandedTaskBar", initExpandedTaskBar },
	{ "ExtendedInventoryWindow", initExtendedInventoryWindow },
	{ "FastEquip", initFastEquip },
	{ "FishingWindow", initFishingWindow },
	{ "GameOptionDialog", initGameOptionDialog },
	{ "GameOptionDialog_forMobile", initGameOptionDialog_forMobile },
	{ "GameWindow", initGameWindow },
	{ "GiftCodeWindow", initGiftCodeWindow },
	{ "GuildWindow", initGuildWindow },
	{ "GuildWindow_BaseInfoPage", initGuildWindow_BaseInfoPage },
	{ "GuildWindow_BoardPage", initGuildWindow_BoardPage },
	{ "GuildWindow_GradePage", initGuildWindow_GradePage },
	{ "GuildWindow_GuildInfoPage", initGuildWindow_GuildInfoPage },
	{ "GuildWindow_GuildInfoPage_Eu", initGuildWindow_GuildInfoPage_Eu },
	{ "GuildWindow_GuildInfoPage_Jp", initGuildWindow_GuildInfoPage_Jp },
	{ "GuildWindow_GuildSkillPage", initGuildWindow_GuildSkillPage },
	{ "GuildWindow_MemberPage", initGuildWindow_MemberPage },
	{ "HelpWindow", initHelpWindow },
	{ "HelpWindow2", initHelpWindow2 },
	{ "HorseShoe", initHorseShoe },
	{ "Ikonki", initIkonki },
	{ "IMEKOR", initIMEKOR },
	{ "InputDialog", initInputDialog },
	{ "InputDialogWithDescription", initInputDialogWithDescription },
	{ "InputDialogWithDescription2", initInputDialogWithDescription2 },
	{ "InventoryWindow", initInventoryWindow },
	{ "InventoryWindowEx", initInventoryWindowEx },
	{ "ItemShopEditorWindow", initItemShopEditorWindow },
	{ "ItemShopPopUpDialog", initItemShopPopUpDialog },
	{ "ItemShopSelectAttributeDialog", initItemShopSelectAttributeDialog },
	{ "ItemShopSelectList", initItemShopSelectList },
	{ "ItemShopWindow", initItemShopWindow },
	{ "KeyChange_Window", initKeyChange_Window },
	{ "LoadingWindow", initLoadingWindow },
	{ "LocationRenderMapDialog", initLocationRenderMapDialog },
	{ "LocationWindow", initLocationWindow },
	{ "MallWindow", initMallWindow },
	{ "MapaSw", initMapaSw },
	{ "MarkListWindow", initMarkListWindow },
	{ "Marmur_Window", initMarmur_Window },
	{ "MessengerWindow", initMessengerWindow },
	{ "MiniGameRumiGamePage", initMiniGameRumiGamePage },
	{ "MiniGameRumiWaitingPage", initMiniGameRumiWaitingPage },
	{ "MiniGameWindow", initMiniGameWindow },
	{ "MiniMap", initMiniMap },
	{ "MiniMapDungeonInfo", initMiniMapDungeonInfo },
	{ "MoneyInputDialog", initMoneyInputDialog },
	{ "Mount_Window", initMount_Window },
	{ "MouseButtonWindow", initMouseButtonWindow },
	{ "MoveChannelDialog", initMoveChannelDialog },
	{ "MusicListWindow", initMusicListWindow },
	{ "OdlamkiWindow", initOdlamkiWindow },
	{ "PartyMemberInfoBoard", initPartyMemberInfoBoard },
	{ "PasswordDialog", initPasswordDialog },
	{ "PickEtcDialog", initPickEtcDialog },
	{ "PickEtcDialog2", initPickEtcDialog2 },
	{ "PickMoneyDialog", initPickMoneyDialog },
	{ "PopUpDialog", initPopUpDialog },
	{ "PrivateShopBuilder", initPrivateShopBuilder },
	{ "QuestDialog", initQuestDialog },
	{ "QuestionDialog", initQuestionDialog },
	{ "QuestionDialog2", initQuestionDialog2 },
	{ "QueueWindow", initQueueWindow },
	{ "RefineDialog", initRefineDialog },
	{ "RemoveItemDialog", initRemoveItemDialog },
	{ "RespDialog", initRespDialog },
	{ "RespMiniMapDialog", initRespMiniMapDialog },
	{ "RespRenderMapDialog", initRespRenderMapDialog },
	{ "RespRenderTargetDialog", initRespRenderTargetDialog },
	{ "RestartDialog", initRestartDialog },
	{ "RightMouseButtonWindow", initRightMouseButtonWindow },
	{ "RuneWindow", initRuneWindow },
	{ "SafeBoxWindow", initSafeBoxWindow },
	{ "SelectCharacterWindow", initSelectCharacterWindow },
	{ "SelectEmpireWindow", initSelectEmpireWindow },
	{ "SelectItemWindow", initSelectItemWindow },
	{ "ShopDialog", initShopDialog },
	{ "ShopSearch", initShopSearch },
	{ "SkillChooseWindow", initSkillChooseWindow },
	{ "SkillPointResetDialog", initSkillPointResetDialog },
	{ "StoneDetach", initStoneDetach },
	{ "SystemDialog", initSystemDialog },
	{ "SystemDialog_forMall", initSystemDialog_forMall },
	{ "SystemDialog_forPortal", initSystemDialog_forPortal },
	{ "SystemOptionDialog", initSystemOptionDialog },
	{ "SystemsWindow", initSystemsWindow },
	{ "SystemsWindow_new", initSystemsWindow_new },
	{ "TaskBar", initTaskBar },
	{ "WarteSchleife", initWarteSchleife },
	{ "WebWindow", initWebWindow },
	{ "WeeklyRank", initWeeklyRank },
	{ "WhisperDialog", initWhisperDialog },
	{ "WonExchangeWindow", initWonExchangeWindow },
	{ "_Pety_script", init_Pety_script },
	{ NULL, NULL },
};

static PyObject* uiscriptlib_isExist(PyObject *self, PyObject *args)
{
	char* func_name;

	if(!PyArg_ParseTuple(args, "s", &func_name))
		return NULL;

	for (int i = 0; NULL != uiscriptlib_init_methods[i].func_name;i++)
	{
		if (0 == _stricmp(uiscriptlib_init_methods[i].func_name, func_name))
		{
			return Py_BuildValue("i", 1);
		}
	}
	return Py_BuildValue("i", 0);
}

static PyObject* uiscriptlib_moduleImport(PyObject *self, PyObject *args)
{
	char* func_name;

	if(!PyArg_ParseTuple(args, "s", &func_name))
		return NULL;

	for (int i = 0; NULL != uiscriptlib_init_methods[i].func_name;i++)
	{
		if (0 == _stricmp(uiscriptlib_init_methods[i].func_name, func_name))
		{
			uiscriptlib_init_methods[i].func();
			if (PyErr_Occurred())
				return NULL;
			PyObject* m = PyDict_GetItemString(PyImport_GetModuleDict(), uiscriptlib_init_methods[i].func_name);
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

static PyObject* uiscriptlib_getList(PyObject *self, PyObject *args)
{
	int iTupleSize = 0;
	while (NULL != uiscriptlib_init_methods[iTupleSize].func_name) {iTupleSize++;}

	PyObject* retTuple = PyTuple_New(iTupleSize);
	for (int i = 0; NULL != uiscriptlib_init_methods[i].func_name; i++)
	{
		PyObject* retSubString = PyString_FromString(uiscriptlib_init_methods[i].func_name);
		PyTuple_SetItem(retTuple, i, retSubString);
	}
	return retTuple;
}

void inituiscriptlibManager()
{
	static struct PyMethodDef methods[] =
	{
		{"isExist", uiscriptlib_isExist, METH_VARARGS},
		{"moduleImport", uiscriptlib_moduleImport, METH_VARARGS},
		{"getList", uiscriptlib_getList, METH_VARARGS},
		{NULL, NULL},
	};

	PyObject* m;
	m = Py_InitModule("uiscriptlib", methods);
}
