import app
import constInfo

AUTOBAN_QUIZ_ANSWER = "ANSWER"
AUTOBAN_QUIZ_REFRESH = "REFRESH"
AUTOBAN_QUIZ_REST_TIME = "REST_TIME"

OPTION_SHADOW = "SHADOW"

CODEPAGE = str(app.GetDefaultCodePage())
name = app.GetLocalePath()


import os
import re
import chat

def ord_hash(astring):
	sum = 0
	for pos in range(len(astring)):
		sum = sum + ord(astring[pos])

	return_value = sum % 10000
	return return_value + (return_value/10)

def SaveWhisper(my_name, target_name ,text):
	whisper_key = "%s_%s" % (my_name, target_name)

	if not whisper_key in constInfo.whisper_history_state or constInfo.whisper_history_state[whisper_key] != 2:
		constInfo.whisper_history_state[whisper_key] = 1

	if not my_name in constInfo.MESSENGER_LIST or len(constInfo.MESSENGER_LIST[my_name]) == 0:
		constInfo.MESSENGER_LIST[my_name] = {}

	constInfo.MESSENGER_LIST[my_name][str(target_name)] = app.GetGlobalTimeStamp()

	if len(str(target_name)) == 0 or target_name != "[INFO]" and str(target_name)[0] == "[":
		return

	f = file("content/userdata/messages/"+ str(my_name) + "_" + str(target_name), "a")
	text = text.replace("\r", " ").replace("\n", " ").replace('\t', " ")
	str_hash = ord_hash(text)
	f.write("%s\t%s\t%s\n" % (str(str_hash), str(text), str(app.GetGlobalTimeStamp())))
	f.close()

def LoadWhisper(my_name, target_name):
	if target_name != "[INFO]" and str(target_name)[0] == "[":
		return

	file_name = "content/userdata/messages/"+ str(my_name) + "_" + str(target_name)

	if os.path.exists(file_name):
		line = open(file_name, "r").read().split("\n")

		line_start = 0
		line_end = len(line)-1

		if (line_end > 1000):
			line_start = line_end-1000

		for i in range(int(line_start), int(line_end)):
			line_split = line[i].split("\t")

			if len(line_split) == 3 and line_split[0] and int(line_split[0]) == ord_hash(line_split[1]):
				chat.AppendWhisper(chat.WHISPER_TYPE_CHAT, target_name, str(line_split[1]))

def DeleteWhisper(my_name, target_name):
	del constInfo.MESSENGER_LIST[my_name][target_name]
	file_name = "content/userdata/messages/"+ str(my_name) + "_" + str(target_name)
	os.remove(file_name)

def WhisperExists(my_name, target_name):
	file_name = "content/userdata/messages/"+ str(my_name) + "_" + str(target_name)

	if os.path.exists(file_name):
		return True
	
	return False

def LoadMessengerList(my_name):
	if not os.path.exists('content/userdata/messages'):
		os.makedirs('content/userdata/messages')

	if not my_name in constInfo.MESSENGER_LIST_LOADED:
		if not my_name in constInfo.MESSENGER_LIST:
			constInfo.MESSENGER_LIST[my_name] = {}
	else:
		return

	for filename in os.listdir('content/userdata/messages/'):
		if filename.startswith(str(my_name) + "_"):
			playername = filename.split("_",1)[1]
			fullPath = 'content/userdata/messages/' + str(filename)

			if os.path.exists(fullPath):
				fileHandle = open(fullPath, "r")
				lineList = fileHandle.readlines()
				fileHandle.close()

				if len(lineList) > 0:
					last_line = lineList[-1].split("\t")
					timestamp = 0

					if len(last_line) >= 3:
						timestamp = ''.join(re.findall(r'\d', last_line[2]))

					constInfo.MESSENGER_LIST[my_name][playername] = int(timestamp)

	constInfo.MESSENGER_LIST_LOADED[my_name] = True

def LoadLocaleFile(srcFileName, localeDict):
	localeDict["CUBE_INFO_TITLE"] = "Recipe"
	localeDict["CUBE_REQUIRE_MATERIAL"] = "Requirements"
	localeDict["CUBE_REQUIRE_MATERIAL_OR"] = "or"

	try:
		lines = open(srcFileName, "r").readlines()
	except IOError:
		import dbg
		dbg.LogBox("LoadUIScriptLocaleError(%(srcFileName)s)" % locals())
		app.Abort()

	for line in lines:
		tokens = line[:-1].split("\t")

		if len(tokens) >= 2:
			localeDict[tokens[0]] = tokens[1]

		else:
			print len(tokens), lines.index(line), line

import re
from decimal import Decimal
def ConvertKKK(number, divisor, KKKType):
	convertedNum = Decimal(number) / Decimal(divisor)
	convertedNum = format(convertedNum.quantize(Decimal("0.001")).normalize(), "f")
	return "%s%s" % (str(convertedNum), KKKType)
	
def NumberToKKK(number):
	number = long(number)
	trilion = long(10**15)
	biolion = long(10**12)
	mld = long(10**9)
	mio = long(10**6)
	ths = long(10**3)

	if number >= trilion:
		return ConvertKKK(number, trilion, "T")

	elif number >= biolion:
		return ConvertKKK(number, biolion, "B")

	elif number >= mld:
		return ConvertKKK(number, mld, "kkk")

	elif number >= mio:
		return ConvertKKK(number, mio, "kk")

	elif number >= ths:
		return ConvertKKK(number, ths, "k")

	return ConvertKKK(number, 1, "")

ITEM_NAMES = "%s/item_names.txt" % (name)
itemNamesDict = {}
for line in open(ITEM_NAMES, 'r'):
	split = line.split('\t')
	if len(split) == 2:
		itemNamesDict[int(split[0])] = split[1].replace("\r", "").replace("\n", "")
		
def FindItemName(vnum):
	if vnum in itemNamesDict:
		return itemNamesDict[vnum]
	else:
		return "Noname"

def RemoveDiacritic(text):
	text = str(text)
	charList = {
		"ě" : "e",
		"Ě" : "e",
		"š" : "s",
		"Š" : "s",
		"č" : "c",
		"Č" : "c",
		"ř" : "r",
		"Ř" : "r",
		"ž" : "z",
		"Ž" : "z",
		"ý" : "y",
		"Ý" : "y",
		"á" : "a",
		"Á" : "a",
		"í" : "i",
		"Í" : "i",
		"é" : "e",
		"É" : "e",
		"ď" : "d",
		"Ď" : "d",
		"ť" : "t",
		"Ť" : "t",
		"ú" : "u",
		"Ú" : "u",
		"ů" : "u",
		"Ů" : "u",
		"Ó" : "o",
		"ó" : "o",
		"Ň" : "n",
		"ň" : "n",
	}

	for key, value in charList.items():
		text = text.replace(key, value)

	return text


if constInfo.ENABLE_LOAD_EX_DATA:
	LOCALE_UISCRIPT_PATH = "locale/ui/"
else:
	LOCALE_UISCRIPT_PATH = "%s/ui/" % (name)
LOGIN_PATH = "%s/ui/login/" % (name)
if app.ENABLE_OFFLINE_SHOP_SYSTEM:
	LOCALE_OFFLINESHOP_PATH = "%s/offlineshop" % (name)
EMPIRE_PATH = "%s/ui/empire/" % (name)
if constInfo.ENABLE_LOAD_EX_DATA:
	GUILD_PATH = "locale/ui/guild/"
else:
	GUILD_PATH = "%s/ui/guild/" % (name)
SELECT_PATH = "%s/ui/select/" % (name)
WINDOWS_PATH = "%s/ui/windows/" % (name)
MAPNAME_PATH = "%s/ui/mapname/" % (name)

JOBDESC_WARRIOR_PATH = "%s/jobdesc_warrior.txt" % (name)
JOBDESC_ASSASSIN_PATH = "%s/jobdesc_assassin.txt" % (name)
JOBDESC_SURA_PATH = "%s/jobdesc_sura.txt" % (name)
JOBDESC_SHAMAN_PATH = "%s/jobdesc_shaman.txt" % (name)
if app.ENABLE_WOLFMAN_CHARACTER:
	JOBDESC_WOLFMAN_PATH = "%s/jobdesc_wolfman.txt" % (name)

EMPIREDESC_A = "%s/empiredesc_a.txt" % (name)
EMPIREDESC_B = "%s/empiredesc_b.txt" % (name)
EMPIREDESC_C = "%s/empiredesc_c.txt" % (name)

LOCALE_INTERFACE_FILE_NAME = "arezzo/locale_interface.txt"
CARDS_DESC = "arezzo/mini_game_okey_desc.txt"

LOCALE_INTERFACE_FILE_NAME = "%s/locale_interface.txt" % (name)
LOCALE_INTERFACE2 = "%s/locale_interface_new.txt" % (name)
LoadLocaleFile(LOCALE_INTERFACE_FILE_NAME, locals())
LoadLocaleFile(LOCALE_INTERFACE2, locals())
if constInfo.ENABLE_LOAD_EX_DATA:
	LoadLocaleFile("locale/ex/locale_interface_ex.txt", locals())

if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
	LoadLocaleFile("%s/aslan_systems/locale_buff_npc.txt" % app.GetLocalePath(), locals())