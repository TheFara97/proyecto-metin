import app
import re
import _winreg

MESSENGER_LIST = {}
MESSENGER_NEWS_LIST = []
MESSENGER_LIST_LOADED = {}

players_online = {}
whisper_afk = 0
whisper_history_state = {}
CAMERA_WAYPOINTS = {}
CAMERA_BEZIER_POINTS = {}

if app.__DUNGEON_INFO__:
	dungeonInfoCMD = ""
	dungeonInfoQuestIdx = -1

IS_VIP = False

CURTAIN_ACTIVE = 1
SHOW_GUARD_OTHER_GUILD_SOCKET_INFO = True

if app.ENABLE_VOTE4BUFF:
	VOTE4BUFF_DATA = { # DO NOT TOUCH THOSE VALUES
		"LOADED": False,
		"LAST_VOTE_TIME": 0,
		"SELECTED_BONUS": 0,
		"CHANGE_BONUS_CD": 0,
		"AVAILABLE_BONUSES": [],
		"ITEM_REWARDS": [],
		"CHANGE_BONUS_PRICE": 0,
		"VOTE_URL": "",
		"API_URL": "",
	}

ITEMSHOP = {
	'items' : {
			'startpage' : {
					'mostBought' : [],
					'hotOffers' : [],
					},
			'itemshop' : {},
			'voteshop' : {},
			'achievementshop' : {},
		},
	'category': [], ##(id, name, image)
	'subCategories': {}, ## categoryId: [...name]
	'tableUpdate' : '0000-00-00 00:00:00',
	'qid'	: 0,
	'questCMD' : '',
}

if app.ENABLE_WIKI:
	_main_wiki_instance = None
	_listbox_wiki_instance = None
	def SetMainParent(instance):
		global _main_wiki_instance
		if _main_wiki_instance:
			del _main_wiki_instance
		_main_wiki_instance = instance
	def GetMainParent():
		global _main_wiki_instance
		return _main_wiki_instance
	def SetListBox(instance):
		global _listbox_wiki_instance
		if _listbox_wiki_instance:
			del _listbox_wiki_instance
		_listbox_wiki_instance = instance
	def GetListBox():
		global _listbox_wiki_instance
		return _listbox_wiki_instance
	def IS_SET(value, flag):
		return (value & flag) == flag

def SaveWhisperHistoryState():
	for key in whisper_history_state.keys():
		whisper_history_state[key] = 2

ENABLE_UI_DEBUG_WINDOW = False
itemRemoveGrid = []
listRemoveItem = []

CLIENT_VERSION = 30
if app.WORLD_BOSS_YUMA:
	WORLD_BOSS_TEXT_POSITION = [0, 0, 0 ,0 ,0]

if app.ENABLE_SKILL_SELECT_FEATURE:
	ARE_ENABLED_6TH_SKILLS = 0
	
MOUNT_LEVEL = 0

BOTCONTROL_TIME = 450

MY_ORE_COUNT = 0
MY_FISH_COUNT = 0
TITLE_BOSS_COUNT = 0
TITLE_STONE_COUNT = 0
TITLE_RARE_COUNT = 0
TITLE_RELICT_COUNT = 0
TITLE_ENCHANT_COUNT = 0
TITLE_REFINE_SUCCESS_COUNT = 0
TITLE_REFINED_ALCHEMY_COUNT = 0
TITLE_REFINE_FAIL_COUNT = 0
TITLE_PLAYTIME_COUNT = 0
TITLE_DUNGEON_FASTEST_TIME = 0
TITLE_DUNGEON_DUNGEON_BOSS_KILLED = 0
TITLE_FASTEST_DUNGEON = 0
TITLE_DIE_TO_MONSTER = 0
TITLE_DUNGEON_NO_DAMAGE = 0

if app.ENABLE_EXTRABONUS_SYSTEM:
	if app.ENABLE_SYSTEM_RUNE:
		RUNE_EXTRABONUS_VALUE = 20
	if app.ENABLE_EXTRABONUS_SYSTEM:
		PET_EXTRABONUS_VALUE = 20

if app.TAKE_LEGEND_DAMAGE_BOARD_SYSTEM:
	LEGEND_DAMAGE_DATA = {
		"NAME": [None] * 15,
		"LEVEL": [None] * 15,
		"RACE": [None] * 15,
		"EMPIRE": [None] * 15,
		"DAMAGE": [None] * 15
	}

BuffSkillInfo_Master = 0
LAST_CHESTINFO_VNUM = 0
EXTENDED_INVENTORY_IS_OPEN = 0
WPADANIE_DODATKOWE = 0
CHESTDROP_INFO_IS_OPEN = 0

ZAPAMIETAJ_OSTATNIA_STRONE = False
ZAPAMIETAJ_OKNO_ZAPISU = False
ZAPAMIETAJ_OKNO_TPMETKIBOSSY = False
LASTPAGE = 0

ANTY_EXP_STATUS = 2

ZNAJDZ_SKRZYNIE_TOOLTIP = 1
ROZDZIEL_TOOLTIP = 1
WYCIAGNIJ_TOOLTIP = 1
DODAJ_DODATKOWE_TOOLTIP = 1

if app.ENABLE_AUTO_SHOUT:
	AUTO_SHOUT_ACTIVATED = 0
	SHOUT_PER_SECOND = 15
	LAST_SHOUT_MESSAGE = ""

DragonSoulPageIndex = 0
DragonSoulInvIndex = 0
IS_DRAGON_SOUL_OPEN = False

IkonkiExpander = 1
MoneyTaskBarExpander = 1
ENABLE_EXPANDED_MONEY_TASKBAR = 1

ExpandedTaskBarExpander = 1

OFFLINESHOP_DIALOG = 0
GIFT_CODE_SYSTEM = 1
IS_ACCE_WINDOW = False
buy_without_dialog = 0
price_color1 = "|cFFb0a996"

windowStatusOpened = "opened"
windowStatusClosed = "closed"
windowStatus = {
	"switchbot" : {
		"status": windowStatusClosed,
		"pos": [None, None],
		"slot": None,
		"switchType": None,
		"switchTypeVnum": None,
	},
}

def windowIsOpened(window):
	return window in windowStatus and windowStatus[window]["status"] == windowStatusOpened

def getWindowValue(window, key):
	if window in windowStatus and key in windowStatus[window]:
		return windowStatus[window][key]

	return None

def setWindowValue(window, key, value):
	if window in windowStatus:
		windowStatus[window][key] = value

if app.ENABLE_EVENT_MANAGER:
	_interface_instance = None
	def GetInterfaceInstance():
		global _interface_instance
		return _interface_instance
	def SetInterfaceInstance(instance):
		global _interface_instance
		if _interface_instance:
			del _interface_instance
		_interface_instance = instance

if app.ENABLE_COLLECT_WINDOW:
	CollectWindowQID = [0 for i in xrange(5)]

ENABLE_NEW_LEVELSKILL_SYSTEM = 0
ENABLE_RANDOM_CHANNEL_SEL = 0
PREV_CHANNEL = 0
ENABLE_CLEAN_DATA_IF_FAIL_LOGIN = 0
ENABLE_PASTE_FEATURE = 1
ENABLE_FULLSTONE_DETAILS = 1
ENABLE_REFINE_PCT = 1
IS_BONUS_CHANGER = False
EXTRA_UI_FEATURE = 1

if app.ENABLE_HIDE_COSTUME_SYSTEM:
	HIDDEN_BODY_COSTUME = 0
	HIDDEN_HAIR_COSTUME = 0
	HIDDEN_ACCE_COSTUME = 0
	HIDDEN_WEAPON_COSTUME = 0
	HIDDEN_AURA_COSTUME = 0
	HIDDEN_STOLE_COSTUME = 0

NEW_678TH_SKILL_ENABLE = 1
SELECT_CHAR_NO_DELAY = 0.3
ENABLE_LOAD_EX_DATA = False
ENABLE_POTIONS_AFFECTSHOWER = 0
ENABLE_SAVE_ACCOUNT = True
CONFIG_YOL = "locale/pl/ui/config/"
DISABLE_MODEL_PREVIEW = 0
SYSTEMS_WINDOW_KAPAT = 0
SYSTEMS_WINDOW_OPEN = 0
AFFECT_SHOWER_TOOLTIP_RENEWAL = 1

PO = 0
DRAGON_SOULD_REFINE_OPEN = 0
DRAGON_SOULD_STEP_OPEN = 0

def NumberToStrRomanNumerals(number):
	ROMAN = [
		(1000, "M"),
		( 900, "CM"),
		( 500, "D"),
		( 400, "CD"),
		( 100, "C"),
		(  90, "XC"),
		(  50, "L"),
		(  40, "XL"),
		(  10, "X"),
		(   9, "IX"),
		(   5, "V"),
		(   4, "IV"),
		(   1, "I"),
	]
	
	result = ""
	for (arabic, roman) in ROMAN:
		(factor, number) = divmod(number, arabic)
		result += roman * factor
	return result

if app.ENABLE_SHOP_SEARCH:
	import grp
	def GetPriceColor(price):
		if price < 500000000:
			return grp.GenerateColor(1.0, 0.7843, 0.0, 1.0)
		elif price < 1000000000:
			return grp.GenerateColor( (255.0/255.0), (153.0/255.0), (51.0/255.0), 1.0)
		elif price < 10000000000:
			return grp.GenerateColor( (153.0/255.0), (76.0/255.0), (0.0/255.0), 1.0)
		else:
			return grp.GenerateColor( (255.0/255.0), (51.0/255.0), (51.0/255.0), 1.0)
			
def ConvertMoneyText(text, powers = dict(k = 10**3, m = 10**6, b = 10**9)):
	"""
	Format string value in thousands, millions or billions.

	'1k' = 1.000
	'100kk' = 100.000.000
	'100m' = 100.000.000
	'1b' = 1.000.000.000
	'1kmb' = 1.000 (can't use multiple suffixes types)
	"""

	match = re.search(r'(\d+)({:s}+)?'.format('+|'.join(powers.keys())), text, re.I)
	if match:
		moneyValue, suffixName = match.groups()
		moneyValue = int(moneyValue)
		if not suffixName:
			return moneyValue

		return moneyValue * (powers[suffixName[0]] ** len(suffixName))

	return 0

def ConvertChequeText(text, powers = dict(k = 10**3, m = 10**6, b = 10**9)):
	"""
	Format string value in thousands, millions or billions.

	'1k' = 1.000
	'100kk' = 100.000.000
	'100m' = 100.000.000
	'1b' = 1.000.000.000
	'1kmb' = 1.000 (can't use multiple suffixes types)
	"""

	match = re.search(r'(\d+)({:s}+)?'.format('+|'.join(powers.keys())), text, re.I)
	if match:
		chequeValue, suffixName = match.groups()
		chequeValue = int(chequeValue)
		if not suffixName:
			return chequeValue

		return chequeValue * (powers[suffixName[0]] ** len(suffixName))

	return 0

EnvanterAcilsinmi = 0
enablefastopenopen = 0

lastSentenceStack = []
lastSentencePos = 0

INPUT_IGNORE = 0

if app.ENABLE_REFINE_RENEWAL:
	IS_AUTO_REFINE = False
	AUTO_REFINE_TYPE = 0
	AUTO_REFINE_DATA = {
		"ITEM" : [-1, -1],
		"NPC" : [0, -1, -1, 0]
	}


IN_GAME_SHOP_ENABLE = 0
CONSOLE_ENABLE = 0

PVPMODE_ENABLE = 1
PVPMODE_TEST_ENABLE = 0
PVPMODE_ACCELKEY_ENABLE = 1
PVPMODE_ACCELKEY_DELAY = 0.5
PVPMODE_PROTECTED_LEVEL = 15

FOG_LEVEL0 = 4800.0
FOG_LEVEL1 = 9600.0
FOG_LEVEL2 = 12800.0
FOG_LEVEL = FOG_LEVEL0
FOG_LEVEL_LIST=[FOG_LEVEL0, FOG_LEVEL1, FOG_LEVEL2]

CAMERA_MAX_DISTANCE_SHORT = 2500.0
CAMERA_MAX_DISTANCE_LONG = 3500.0
CAMERA_MAX_DISTANCE_LIST=[CAMERA_MAX_DISTANCE_SHORT, CAMERA_MAX_DISTANCE_LONG]
CAMERA_MAX_DISTANCE = CAMERA_MAX_DISTANCE_SHORT

CHRNAME_COLOR_INDEX = 0

ENVIRONMENT_NIGHT="d:/ymir work/environment/moonlight04.msenv"

HIGH_PRICE = 500000
MIDDLE_PRICE = 50000
ERROR_METIN_STONE = 28960
SUB2_LOADING_ENABLE = 1
EXPANDED_COMBO_ENABLE = 1
CONVERT_EMPIRE_LANGUAGE_ENABLE = 0
USE_ITEM_WEAPON_TABLE_ATTACK_BONUS = 0
ADD_DEF_BONUS_ENABLE = 0
LOGIN_COUNT_LIMIT_ENABLE = 0

USE_SKILL_EFFECT_UPGRADE_ENABLE = 1

VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD = 1
GUILD_MONEY_PER_GSP = 100
GUILD_WAR_TYPE_SELECT_ENABLE = 1
TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE = 10

HAIR_COLOR_ENABLE = 1
ARMOR_SPECULAR_ENABLE = 1
WEAPON_SPECULAR_ENABLE = 1
SEQUENCE_PACKET_ENABLE = 1
KEEP_ACCOUNT_CONNETION_ENABLE = 1
MINIMAP_POSITIONINFO_ENABLE = 1

isItemQuestionDialog = 0

def GET_ITEM_QUESTION_DIALOG_STATUS():
	global isItemQuestionDialog
	return isItemQuestionDialog

def SET_ITEM_QUESTION_DIALOG_STATUS(flag):
	global isItemQuestionDialog
	isItemQuestionDialog = flag

import app
import net

	
def SET_DEFAULT_FOG_LEVEL():
	global FOG_LEVEL
	app.SetMinFog(FOG_LEVEL)

def SET_FOG_LEVEL_INDEX(index):
	global FOG_LEVEL
	global FOG_LEVEL_LIST
	try:
		FOG_LEVEL=FOG_LEVEL_LIST[index]
	except IndexError:
		FOG_LEVEL=FOG_LEVEL_LIST[0]
	app.SetMinFog(FOG_LEVEL)

def GET_FOG_LEVEL_INDEX():
	global FOG_LEVEL
	global FOG_LEVEL_LIST
	return FOG_LEVEL_LIST.index(FOG_LEVEL)


def SET_DEFAULT_CAMERA_MAX_DISTANCE():
	global CAMERA_MAX_DISTANCE
	app.SetCameraMaxDistance(CAMERA_MAX_DISTANCE)

def SET_CAMERA_MAX_DISTANCE_INDEX(index):
	global CAMERA_MAX_DISTANCE
	global CAMERA_MAX_DISTANCE_LIST
	try:
		CAMERA_MAX_DISTANCE=CAMERA_MAX_DISTANCE_LIST[index]
	except:
		CAMERA_MAX_DISTANCE=CAMERA_MAX_DISTANCE_LIST[0]

	app.SetCameraMaxDistance(CAMERA_MAX_DISTANCE)

def GET_CAMERA_MAX_DISTANCE_INDEX():
	global CAMERA_MAX_DISTANCE
	global CAMERA_MAX_DISTANCE_LIST
	return CAMERA_MAX_DISTANCE_LIST.index(CAMERA_MAX_DISTANCE)


import chrmgr
import player
import app

def SET_DEFAULT_CHRNAME_COLOR():
	global CHRNAME_COLOR_INDEX
	chrmgr.SetEmpireNameMode(CHRNAME_COLOR_INDEX)

def SET_CHRNAME_COLOR_INDEX(index):
	global CHRNAME_COLOR_INDEX
	CHRNAME_COLOR_INDEX=index
	chrmgr.SetEmpireNameMode(index)

def GET_CHRNAME_COLOR_INDEX():
	global CHRNAME_COLOR_INDEX
	return CHRNAME_COLOR_INDEX

def SET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD(index):
	global VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD
	VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD = index

def GET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD():
	global VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD
	return VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD

def SET_DEFAULT_CONVERT_EMPIRE_LANGUAGE_ENABLE():
	global CONVERT_EMPIRE_LANGUAGE_ENABLE
	net.SetEmpireLanguageMode(CONVERT_EMPIRE_LANGUAGE_ENABLE)

def SET_DEFAULT_USE_ITEM_WEAPON_TABLE_ATTACK_BONUS():
	global USE_ITEM_WEAPON_TABLE_ATTACK_BONUS
	player.SetWeaponAttackBonusFlag(USE_ITEM_WEAPON_TABLE_ATTACK_BONUS)

def SET_DEFAULT_USE_SKILL_EFFECT_ENABLE():
	global USE_SKILL_EFFECT_UPGRADE_ENABLE
	app.SetSkillEffectUpgradeEnable(USE_SKILL_EFFECT_UPGRADE_ENABLE)

def SET_TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE():
	global TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE
	app.SetTwoHandedWeaponAttSpeedDecreaseValue(TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE)

import item

ACCESSORY_MATERIAL_LIST = [50623, 50624, 50625, 50626, 50627, 50628, 50629, 50630, 50631, 50632, 50633, 50634, 50635, 50636, 50637, 50638, 50639, 50640, 50641, 50642, 50643, 50644, 50647, 50649]
JewelAccessoryInfos = [
		[ 50639,	89110,	89120,	89130 ],
		[ 50640,	32180,	32280,	32380 ],
		[ 50641,	32120,	32220,	32320 ],
		[ 50642,	32150,	32250,	32350 ],
		[ 50643,	32130,	32230,	32330 ],
		[ 50644,	32160,	32260,	32360 ],
		[ 50639,	17200,	14200,	16200 ],
		[ 50640,	32170,	32270,	32370 ],
		[ 50641,	32140,	32240,	32340 ],
		[ 50642,	32100,	32200,	32300 ],
		[ 50643,	32190,	32290,	32390 ],
		[ 50644,	32110,	32210,	32310 ],
		[ 50647,	32670,	32680,	32690 ],
		[ 50649,	32600,	32610,	32620,	32630,	32640,	32650,	32720,	32790, 89160, 89170, 89180, 89190, 89200, 89210 ],
	]
def GET_ACCESSORY_MATERIAL_VNUM(vnum, subType):
	ret = vnum
	item_base = (vnum / 10) * 10
	for info in JewelAccessoryInfos:
		if item.ARMOR_WRIST == subType:
			if info[1] == item_base:
				return info[0]
		elif item.ARMOR_NECK == subType:
			if info[2] == item_base:
				return info[0]
		elif item.ARMOR_EAR == subType:
			if info[3] == item_base:
				return info[0]

	if vnum >= 16210 and vnum <= 16219:
		return 50625

	if item.ARMOR_WRIST == subType:
		WRIST_ITEM_VNUM_BASE = 14000
		ret -= WRIST_ITEM_VNUM_BASE
	elif item.ARMOR_NECK == subType:
		NECK_ITEM_VNUM_BASE = 16000
		ret -= NECK_ITEM_VNUM_BASE
	elif item.ARMOR_EAR == subType:
		EAR_ITEM_VNUM_BASE = 17000
		ret -= EAR_ITEM_VNUM_BASE

	type = ret/20

	if type<0 or type>=len(ACCESSORY_MATERIAL_LIST):
		type = (ret-170) / 20
		if type<0 or type>=len(ACCESSORY_MATERIAL_LIST):
			return 0

	return ACCESSORY_MATERIAL_LIST[type]


def GET_BELT_MATERIAL_VNUM(vnum, subType = 0):
	return 50649


def IS_AUTO_POTION(itemVnum):
	return IS_AUTO_POTION_HP(itemVnum) or IS_AUTO_POTION_SP(itemVnum)

def IS_AUTO_POTION_HP(itemVnum):
	if 72723 <= itemVnum and 72726 >= itemVnum:
		return 1
	elif itemVnum >= 76021 and itemVnum <= 76022:
		return 1

	return 0

def IS_AUTO_POTION_SP(itemVnum):
	if 72727 <= itemVnum and 72730 >= itemVnum:
		return 1
	elif itemVnum >= 76004 and itemVnum <= 76005:
		return 1

	return 0

if app.ENABLE_EXTENDED_BLEND:
	def IS_BLEND_ITEM(itemVnum):
		item.SelectItem(itemVnum)
		itemType = item.GetItemType()
		
		if itemType == item.ITEM_TYPE_BLEND:
			return 1

		return 0

	def IS_PERMANANET_BLEND_ITEM(itemVnum):
		if 51821 <= itemVnum and 51826 >= itemVnum:
			return 1

		if 51813 <= itemVnum and 51820 >= itemVnum:
			return 1

		if 40017 <= itemVnum and 40025 >= itemVnum:
			return 1

		return 0



if app.ENABLE_NEW_PET_SYSTEM:
	gui_pets = {
		"NAME" : "",
		"RACE" : 0,
		"LEVEL" : 0,
		"EXP" : 0,
		"EXP_NEED" : 0,
		"POINTS" : 0,
		"SKILLS" : [],
	}
	gui_mount = {
		"LEVEL" : 0,
		"EXP" : 0,
		"EXP_NEED" : 0,
		"SKILLS" : [],
	}
if app.ENABLE_DROP_INFO:
	dropInfoDict = {}
	
if app.ENABLE_PERFORMANCE_IMPROVEMENTS_NEW:
	isGameLoaded = False

REG_PATH = r"SOFTWARE\Elvion"

def set_reg(name, value):
	try:
		_winreg.CreateKey(_winreg.HKEY_CURRENT_USER, REG_PATH)
		registry_key = _winreg.OpenKey(_winreg.HKEY_CURRENT_USER, REG_PATH, 0, _winreg.KEY_WRITE)
		_winreg.SetValueEx(registry_key, name, 0, _winreg.REG_SZ, value)
		_winreg.CloseKey(registry_key)
		return True
	except WindowsError:
		return False

def get_reg(name):
	try:
		registry_key = _winreg.OpenKey(_winreg.HKEY_CURRENT_USER, REG_PATH, 0, _winreg.KEY_READ)
		value, regtype = _winreg.QueryValueEx(registry_key, name)
		_winreg.CloseKey(registry_key)
		return str(value)
	except WindowsError:
		return None

_game_instance = None
def GetGameInstance():
	global _game_instance
	return _game_instance
def SetGameInstance(instance):
	global _game_instance
	if _game_instance:
		del _game_instance
	_game_instance = instance
	
_interface_instance = None
def GetInterfaceInstance():
	global _interface_instance
	return _interface_instance
def SetInterfaceInstance(instance):
	global _interface_instance
	if _interface_instance:
		del _interface_instance
	_interface_instance = instance