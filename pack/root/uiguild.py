import app
import ui
import dbg
import guild
import net
import wndMgr
import grp
import grpText
import uiPickMoney
import localeInfo
import player
import skill
import mouseModule
import uiUploadMark
import uiCommon
import uiToolTip
import playerSettingModule
import constInfo
import background
import miniMap
import chr
import chat
import item
import uiScriptLocale
from datetime import timedelta, datetime
import os
if app.ENABLE_CHEQUE_SYSTEM:
	import uiPickETC

from _weakref import proxy

DISABLE_GUILD_SKILL = False
DISABLE_DECLARE_WAR = True

def NumberToMoneyString(n):
	return localeInfo.NumberToMoneyString(n)

if (localeInfo.IsEUROPE() and app.GetLocalePath() != "locale/br"):
	def NumberToMoneyString(n):
		if n <= 0 :
			return "0"

		return "%s" % (','.join([ i-3<0 and str(n)[:i] or str(n)[i-3:i] for i in range(len(str(n))%3, len(str(n))+1, 3) if i ]))

MATERIAL_STONE_INDEX = 0
MATERIAL_LOG_INDEX = 1
MATERIAL_PLYWOOD_INDEX = 2

MATERIAL_STONE_ID = 90010
MATERIAL_LOG_ID = 90011
MATERIAL_PLYWOOD_ID = 90012

GUILD_BONUS_UPGRADE_COST = 1500  # Cost per material type per upgrade

# Per-skill config: (skill_name, unit_suffix, [value_at_level_0, level_1, ..., level_20])
GUILD_SKILL_DATA = [
	# 0: Max Member Count (+1 per level)
	("Max Guild Members",      " member(s)", [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]),
	# 1: Attbonus Stone (+1% per level)
	("Bonus vs. Stones",       "%",          [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]),
	# 2: Attbonus Boss (+1% per level)
	("Bonus vs. Bosses",       "%",          [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]),
	# 3: Double Drop Items (+1% per 5 levels)
	("Double Drop Chance",     "%",          [0,0,0,0,0,1,1,1,1,1,2,2,2,2,2,3,3,3,3,3,4]),
	# 4: More Raid Gold (+5%/level bonus gold from guild raid final boss kill)
	("Raid Gold Bonus",        "%",          [0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100]),
]

BUILDING_DATA_LIST = []

# Guild Dungeon Constants
BASE_PATH = "elvion/game/guild/"

MAX_DAILY_EXP = 5000  # Maximum daily experience a player can earn for the guild

TICKET_VNUM = 19
TICKET_COST = "500 DC"
DUNGEON_TYPES = [localeInfo.GUILD_RAID_EASY_MODE, localeInfo.GUILD_RAID_MEDIUM_MODE, localeInfo.GUILD_RAID_HARD_MODE]

LOGS_MAX_COUNT = 13
DUNGEONS = [
	[
		"Crimson Palace", [
			[
				1, "raid_1.png", 5, (60, 40, 0, 0), "15 min.", 1000000, 20,
				[
					(91250, 0, "Guild guardian sword"),	(91270, 0, "Guild guardian helmet"), 	(90010, 500, ""), 		(90013, 5, ""), 	(0, 0, ""), 	(0, 0, ""),
					(0, 0, ""), 	(91260, 0, "Guild guardian armor"), 	(90011, 500, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
					(0, 0, ""), 	(0, 0, ""), 	(90012, 500, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
				]
			],
			[
				51, "raid_1.png", 15, (0, 0, 0, 0), "15 min.", 1000000, 20,
				[
					(0, 0, ""),	(0, 0, ""), 	(0, 0, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
					(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
					(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
				]
			],
			[
				101, "raid_1.png", 25, (0, 0, 0, 0), "15 min.", 1000000, 20,
				[
					(0, 0, ""),	(0, 0, ""), 	(0, 0, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
					(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
					(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
				]
			],
		],
	],
	[
		"Blazing Citadel", [
			[
				2, "raid_1.png", 5, (0, 0, 0, 0), "15 min.", 1000000, 20,
				[
					(91280, 0, "Guild Guardian Shield"),	(91290, 0, "Guild Guardian Earrings"), 	(91300, 0, "Guild Guardian Bracelet"), 		(90010, 10, ""), 	(90011, 10, ""), 	(90012, 10, ""),
					(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
					(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
				]
			],
			[
				52, "raid_1.png", 15, (0, 0, 0, 0), "15 min.", 1000000, 20,
				[
					(0, 0, ""),	(0, 0, ""), 	(0, 0, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
					(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
					(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
				]
			],
			[
				102, "raid_1.png", 25, (0, 0, 0, 0), "15 min.", 1000000, 20,
				[
					(0, 0, ""),	(0, 0, ""), 	(0, 0, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
					(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
					(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
				]
			],
		],
	],
	[
		"Jade Shrine", [
			[
				4, "raid_3.png", 5, (0, 0, 0, 0), "15 min.", 1000000, 20,
				[
					(91310, 0, "Guild Guardian Necklace"),	(91320, 0, "Guild Guardian Belt"), 	(91330, 0, "Guild Guardian Shoes"), 		(90010, 10, ""), 	(90011, 10, ""), 	(90012, 10, ""),
					(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
					(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
				]
			],
			[
				54, "raid_3.png", 15, (0, 0, 0, 0), "15 min.", 1000000, 20,
				[
					(0, 0, ""),	(0, 0, ""), 	(0, 0, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
					(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
					(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
				]
			],
			[
				104, "raid_3.png", 25, (0, 0, 0, 0), "15 min.", 1000000, 20,
				[
					(0, 0, ""),	(0, 0, ""), 	(0, 0, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
					(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
					(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""), 		(0, 0, ""), 	(0, 0, ""), 	(0, 0, ""),
				]
			],
		],
	],
]

def GetGVGKey(srcGuildID, dstGuildID):
	minID = min(srcGuildID, dstGuildID)
	maxID = max(srcGuildID, dstGuildID)
	return minID*1000 + maxID
def unsigned32(n):
	return n & 0xFFFFFFFFL

class DeclareGuildWarDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.type=0
		self.__CreateDialog()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Open(self):
		self.inputValue.SetFocus()
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.ClearDictionary()
		self.board = None
		self.acceptButton = None
		self.cancelButton = None
		self.inputSlot = None
		self.inputValue = None
		self.Hide()

	def __CreateDialog(self):

		try:
			pyScrLoader = ui.PythonScriptLoader()

			if localeInfo.IsVIETNAM() :
				pyScrLoader.LoadScriptFile(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "declareguildwardialog.py")
			else:
				pyScrLoader.LoadScriptFile(self, "uiscript/declareguildwardialog.py")

		except:
			import exception
			exception.Abort("DeclareGuildWarWindow.__CreateDialog - LoadScript")

		try:
			getObject = self.GetChild
			self.board = getObject("Board")

			self.typeButtonList=[]
			self.typeButtonList.append(getObject("NormalButton"))
			self.typeButtonList.append(getObject("WarpButton"))
			self.typeButtonList.append(getObject("CTFButton"))

			self.acceptButton = getObject("AcceptButton")
			self.cancelButton = getObject("CancelButton")
			self.inputSlot = getObject("InputSlot")
			self.inputValue = getObject("InputValue")

			gameType=getObject("GameType")

		except:
			import exception
			exception.Abort("DeclareGuildWarWindow.__CreateDialog - BindObject")

		if constInfo.GUILD_WAR_TYPE_SELECT_ENABLE==0:
			gameType.Hide()

		self.typeButtonList[0].SAFE_SetEvent(self.__OnClickTypeButtonNormal)
		self.typeButtonList[1].SAFE_SetEvent(self.__OnClickTypeButtonWarp)
		self.typeButtonList[2].SAFE_SetEvent(self.__OnClickTypeButtonCTF)

		self.typeButtonList[0].SetToolTipWindow(self.__CreateGameTypeToolTip(localeInfo.GUILDWAR_NORMAL_TITLE, localeInfo.GUILDWAR_NORMAL_DESCLIST))
		self.typeButtonList[1].SetToolTipWindow(self.__CreateGameTypeToolTip(localeInfo.GUILDWAR_WARP_TITLE, localeInfo.GUILDWAR_WARP_DESCLIST))
		self.typeButtonList[2].SetToolTipWindow(self.__CreateGameTypeToolTip(localeInfo.GUILDWAR_CTF_TITLE, localeInfo.GUILDWAR_CTF_DESCLIST))

		self.__ClickRadioButton(self.typeButtonList, 0)

		self.SetAcceptEvent(ui.__mem_func__(self.__OnOK))
		self.SetCancelEvent(ui.__mem_func__(self.__OnCancel))

	def __OnOK(self):
		text = self.GetText()
		type = self.GetType()

		if ""==text:
			return

		net.SendChatPacket("/war %s %d" % (text, type))
		self.Close()

		return 1

	def __OnCancel(self):
		self.Close()
		return 1

	def __OnClickTypeButtonNormal(self):
		self.__ClickTypeRadioButton(0)

	def __OnClickTypeButtonWarp(self):
		self.__ClickTypeRadioButton(1)

	def __OnClickTypeButtonCTF(self):
		self.__ClickTypeRadioButton(2)

	def __ClickTypeRadioButton(self, type):
		self.__ClickRadioButton(self.typeButtonList, type)
		self.type=type

	def __ClickRadioButton(self, buttonList, buttonIndex):
		try:
			selButton=buttonList[buttonIndex]
		except IndexError:
			return

		for eachButton in buttonList:
			eachButton.SetUp()

		selButton.Down()

	def SetTitle(self, name):
		self.board.SetTitleName(name)

	def SetNumberMode(self):
		self.inputValue.SetNumberMode()

	def SetSecretMode(self):
		self.inputValue.SetSecret()

	def SetFocus(self):
		self.inputValue.SetFocus()

	def SetMaxLength(self, length):
		width = length * 6 + 10
		self.inputValue.SetMax(length)
		self.SetSlotWidth(width)
		self.SetBoardWidth(max(width + 50, 160))

	def SetSlotWidth(self, width):
		self.inputSlot.SetSize(width, self.inputSlot.GetHeight())
		self.inputValue.SetSize(width, self.inputValue.GetHeight())

	def SetBoardWidth(self, width):
		self.board.SetSize(max(width + 50, 160), self.GetHeight())
		self.SetSize(max(width + 50, 160), self.GetHeight())
		self.UpdateRect()

	def SetAcceptEvent(self, event):
		self.acceptButton.SetEvent(event)
		self.inputValue.OnIMEReturn = event

	def SetCancelEvent(self, event):
		self.board.SetCloseEvent(event)
		self.cancelButton.SetEvent(event)
		self.inputValue.OnPressEscapeKey = event

	def GetType(self):
		return self.type

	def GetText(self):
		return self.inputValue.GetText()

	def __CreateGameTypeToolTip(self, title, descList):
		toolTip = uiToolTip.ToolTip()
		toolTip.SetTitle(title)
		toolTip.AppendSpace(5)

		for desc in descList:
			toolTip.AutoAppendTextLine(desc)

		toolTip.AlignHorizonalCenter()
		return toolTip


class AcceptGuildWarDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.type=0
		self.__CreateDialog()

	def __del__(self):
		print "---------------------------------------------------------------------------- DELETE AcceptGuildWarDialog"
		ui.ScriptWindow.__del__(self)

	def Open(self, guildName, warType):
		self.guildName=guildName
		self.warType=warType
		self.__ClickSelectedTypeRadioButton()
		self.inputValue.SetText(guildName)
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def GetGuildName(self):
		return self.guildName

	def Close(self):
		self.ClearDictionary()
		self.board = None
		self.acceptButton = None
		self.cancelButton = None
		self.inputSlot = None
		self.inputValue = None
		self.Hide()

	def __ClickSelectedTypeRadioButton(self):
		self.__ClickTypeRadioButton(self.warType)

	def __CreateDialog(self):

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/acceptguildwardialog.py")
		except:
			import exception
			exception.Abort("DeclareGuildWarWindow.__CreateDialog - LoadScript")

		try:
			getObject = self.GetChild
			self.board = getObject("Board")

			self.typeButtonList=[]
			self.typeButtonList.append(getObject("NormalButton"))
			self.typeButtonList.append(getObject("WarpButton"))
			self.typeButtonList.append(getObject("CTFButton"))

			self.acceptButton = getObject("AcceptButton")
			self.cancelButton = getObject("CancelButton")
			self.inputSlot = getObject("InputSlot")
			self.inputValue = getObject("InputValue")

			gameType=getObject("GameType")

		except:
			import exception
			exception.Abort("DeclareGuildWarWindow.__CreateDialog - BindObject")

		if constInfo.GUILD_WAR_TYPE_SELECT_ENABLE==0:
			gameType.Hide()

		self.typeButtonList[0].SAFE_SetEvent(self.__OnClickTypeButtonNormal)
		self.typeButtonList[1].SAFE_SetEvent(self.__OnClickTypeButtonWarp)
		self.typeButtonList[2].SAFE_SetEvent(self.__OnClickTypeButtonCTF)

		self.typeButtonList[0].SetToolTipWindow(self.__CreateGameTypeToolTip(localeInfo.GUILDWAR_NORMAL_TITLE, localeInfo.GUILDWAR_NORMAL_DESCLIST))
		self.typeButtonList[1].SetToolTipWindow(self.__CreateGameTypeToolTip(localeInfo.GUILDWAR_WARP_TITLE, localeInfo.GUILDWAR_WARP_DESCLIST))
		self.typeButtonList[2].SetToolTipWindow(self.__CreateGameTypeToolTip(localeInfo.GUILDWAR_CTF_TITLE, localeInfo.GUILDWAR_CTF_DESCLIST))

		self.__ClickRadioButton(self.typeButtonList, 0)

	def __OnClickTypeButtonNormal(self):
		self.__ClickSelectedTypeRadioButton()

	def __OnClickTypeButtonWarp(self):
		self.__ClickSelectedTypeRadioButton()

	def __OnClickTypeButtonCTF(self):
		self.__ClickSelectedTypeRadioButton()

	def __ClickTypeRadioButton(self, type):
		self.__ClickRadioButton(self.typeButtonList, type)
		self.type=type

	def __ClickRadioButton(self, buttonList, buttonIndex):
		try:
			selButton=buttonList[buttonIndex]
		except IndexError:
			return

		for eachButton in buttonList:
			eachButton.SetUp()

		selButton.Down()

	def SetTitle(self, name):
		pass
		#self.board.SetTitleName(name)

	def SetNumberMode(self):
		self.inputValue.SetNumberMode()

	def SetSecretMode(self):
		self.inputValue.SetSecret()

	def SetFocus(self):
		self.inputValue.SetFocus()

	def SetMaxLength(self, length):
		width = length * 6 + 10
		self.inputValue.SetMax(length)
		self.SetSlotWidth(width)
		self.SetBoardWidth(max(width + 50, 160))

	def SetSlotWidth(self, width):
		self.inputSlot.SetSize(width, self.inputSlot.GetHeight())
		self.inputValue.SetSize(width, self.inputValue.GetHeight())

	def SetBoardWidth(self, width):
		#self.board.SetSize(max(width + 50, 160), self.GetHeight())
		self.SetSize(max(width + 50, 160), self.GetHeight())
		self.UpdateRect()

	def SAFE_SetAcceptEvent(self, event):
		self.SetAcceptEvent(ui.__mem_func__(event))

	def SAFE_SetCancelEvent(self, event):
		self.SetCancelEvent(ui.__mem_func__(event))

	def SetAcceptEvent(self, event):
		self.acceptButton.SetEvent(event)
		self.inputValue.OnIMEReturn = event

	def SetCancelEvent(self, event):
		#self.board.SetCloseEvent(event)
		self.cancelButton.SetEvent(event)
		self.inputValue.OnPressEscapeKey = event

	def GetType(self):
		return self.type

	def GetText(self):
		return self.inputValue.GetText()

	def __CreateGameTypeToolTip(self, title, descList):
		toolTip = uiToolTip.ToolTip()
		toolTip.SetTitle(title)
		toolTip.AppendSpace(5)

		for desc in descList:
			toolTip.AutoAppendTextLine(desc)

		toolTip.AlignHorizonalCenter()
		return toolTip



class GuildWarScoreBoard(ui.ThinBoard):

	def __init__(self):
		ui.ThinBoard.__init__(self)
		self.Initialize()

	def __del__(self):
		ui.ThinBoard.__del__(self)

	def Initialize(self):
		self.allyGuildID = 0
		self.enemyGuildID = 0
		self.allyDataDict = {}
		self.enemyDataDict = {}

	def Open(self, allyGuildID, enemyGuildID):

		self.allyGuildID = allyGuildID
		self.enemyGuildID = enemyGuildID

		self.SetPosition(10, wndMgr.GetScreenHeight() - 100)

		mark = ui.MarkBox()
		mark.SetParent(self)
		mark.SetIndex(allyGuildID)
		mark.SetPosition(10, 10 + 18*0)
		mark.Show()
		scoreText = ui.TextLine()
		scoreText.SetParent(self)
		scoreText.SetPosition(30, 10 + 18*0)
		scoreText.SetHorizontalAlignLeft()
		scoreText.Show()
		self.allyDataDict["NAME"] = guild.GetGuildName(allyGuildID)
		self.allyDataDict["SCORE"] = 0
		self.allyDataDict["MEMBER_COUNT"] = -1
		self.allyDataDict["MARK"] = mark
		self.allyDataDict["TEXT"] = scoreText

		mark = ui.MarkBox()
		mark.SetParent(self)
		mark.SetIndex(enemyGuildID)
		mark.SetPosition(10, 10 + 18*1)
		mark.Show()
		scoreText = ui.TextLine()
		scoreText.SetParent(self)
		scoreText.SetPosition(30, 10 + 18*1)
		scoreText.SetHorizontalAlignLeft()
		scoreText.Show()
		self.enemyDataDict["NAME"] = guild.GetGuildName(enemyGuildID)
		self.enemyDataDict["SCORE"] = 0
		self.enemyDataDict["MEMBER_COUNT"] = -1
		self.enemyDataDict["MARK"] = mark
		self.enemyDataDict["TEXT"] = scoreText

		self.__RefreshName()
		self.Show()

	def __GetDataDict(self, ID):
		if self.allyGuildID == ID:
			return self.allyDataDict
		if self.enemyGuildID == ID:
			return self.enemyDataDict

		return None

	def SetScore(self, gainGuildID, opponetGuildID, point):
		dataDict = self.__GetDataDict(gainGuildID)
		if not dataDict:
			return
		dataDict["SCORE"] = point
		self.__RefreshName()

	def UpdateMemberCount(self, guildID1, memberCount1, guildID2, memberCount2):
		dataDict1 = self.__GetDataDict(guildID1)
		dataDict2 = self.__GetDataDict(guildID2)
		if dataDict1:
			dataDict1["MEMBER_COUNT"] = memberCount1
		if dataDict2:
			dataDict2["MEMBER_COUNT"] = memberCount2
		self.__RefreshName()

	def __RefreshName(self):
		nameMaxLen = max(len(self.allyDataDict["NAME"]), len(self.enemyDataDict["NAME"]))

		if -1 == self.allyDataDict["MEMBER_COUNT"] or -1 == self.enemyDataDict["MEMBER_COUNT"]:
			self.SetSize(30+nameMaxLen*6+8*5, 50)
			self.allyDataDict["TEXT"].SetText("%s %d" % (self.allyDataDict["NAME"], self.allyDataDict["SCORE"]))
			self.enemyDataDict["TEXT"].SetText("%s %d" % (self.enemyDataDict["NAME"], self.enemyDataDict["SCORE"]))

		else:
			self.SetSize(30+nameMaxLen*6+8*5+15, 50)
			self.allyDataDict["TEXT"].SetText("%s(%d) %d" % (self.allyDataDict["NAME"], self.allyDataDict["MEMBER_COUNT"], self.allyDataDict["SCORE"]))
			self.enemyDataDict["TEXT"].SetText("%s(%d) %d" % (self.enemyDataDict["NAME"], self.enemyDataDict["MEMBER_COUNT"], self.enemyDataDict["SCORE"]))

class MouseReflector(ui.Window):
	def __init__(self, parent):
		ui.Window.__init__(self)
		self.SetParent(parent)
		self.AddFlag("not_pick")
		self.width = self.height = 0
		self.isDown = False

	def Down(self):
		self.isDown = True

	def Up(self):
		self.isDown = False

	def OnRender(self):

		if self.isDown:
			grp.SetColor(ui.WHITE_COLOR)
		else:
			grp.SetColor(ui.HALF_WHITE_COLOR)

		x, y = self.GetGlobalPosition()
		grp.RenderBar(x+2, y+2, self.GetWidth()-4, self.GetHeight()-4)

class EditableTextSlot(ui.ImageBox):
	def __init__(self, parent, x, y):
		ui.ImageBox.__init__(self)
		self.SetParent(parent)
		self.SetPosition(x, y)
		self.LoadImage("elvion/game/guild/member_auth/grade_name_slot.png")
		
		self.mouseReflector = MouseReflector(self)
		self.mouseReflector.SetSize(self.GetWidth(), self.GetHeight())
		
		self.Enable = True
		self.textLine = ui.MakeTextLine(self)
		self.event = lambda *arg: None
		self.arg = 0
		
		self.Show()
		self.mouseReflector.UpdateRect()
	
	def __del__(self):
		ui.ImageBox.__del__(self)
	
	def SetText(self, text):
		self.textLine.SetText(text)
	
	def SetEvent(self, event, arg):
		self.event = event
		self.arg = arg
	
	def Disable(self):
		self.Enable = False
	
	def OnMouseOverIn(self):
		if not self.Enable:
			return
		self.mouseReflector.Show()
	
	def OnMouseOverOut(self):
		if not self.Enable:
			return
		self.mouseReflector.Hide()
	
	def OnMouseLeftButtonDown(self):
		if not self.Enable:
			return
		self.mouseReflector.Down()
	
	def OnMouseLeftButtonUp(self):
		if not self.Enable:
			return
		self.mouseReflector.Up()
		self.event(self.arg)
	
	# Text customization methods
	def SetTextColor(self, color):
		"""Set text color using packed color format (0xAARRGGBB)"""
		self.textLine.SetPackedFontColor(color)
	
	def SetFontColor(self, r, g, b, a=1.0):
		"""Set text color using RGBA values (0.0-1.0)"""
		self.textLine.SetFontColor(r, g, b, a)
	
	def SetOutline(self, value=True):
		"""Enable/disable text outline"""
		if value:
			self.textLine.SetOutline()
	
	def SetFontName(self, fontName):
		"""Set font name (e.g., 'Tahoma:12')"""
		self.textLine.SetFontName(fontName)
	
	def SetHorizontalAlign(self, align):
		"""Set horizontal alignment: 'left', 'center', 'right'"""
		if align == "left":
			self.textLine.SetHorizontalAlignLeft()
		elif align == "center":
			self.textLine.SetHorizontalAlignCenter()
		elif align == "right":
			self.textLine.SetHorizontalAlignRight()
	
	def SetVerticalAlign(self, align):
		if align == "top":
			self.textLine.SetVerticalAlignTop()
		elif align == "center":
			self.textLine.SetVerticalAlignCenter()
		elif align == "bottom":
			self.textLine.SetVerticalAlignBottom()

class CheckBox(ui.ImageBox):
	def __init__(self, parent, x, y, event, filename = "d:/ymir work/ui/public/Parameter_Slot_01.sub"):
		ui.ImageBox.__init__(self)
		self.SetParent(parent)
		self.SetPosition(x, y)
		self.LoadImage(filename)

		self.mouseReflector = MouseReflector(self)
		self.mouseReflector.SetSize(self.GetWidth(), self.GetHeight())

		image = ui.MakeImageBox(self, "elvion/game/guild/member/check.png", 0, 0)
		image.AddFlag("not_pick")
		image.SetWindowHorizontalAlignCenter()
		image.SetWindowVerticalAlignCenter()
		image.Hide()
		self.Enable = True
		self.image = image
		self.event = event
		self.Show()

		self.mouseReflector.UpdateRect()

	def __del__(self):
		ui.ImageBox.__del__(self)

	def SetCheck(self, flag):
		if flag:
			self.image.Show()
		else:
			self.image.Hide()

	def Disable(self):
		self.Enable = False

	def OnMouseOverIn(self):
		if not self.Enable:
			return
		self.mouseReflector.Show()

	def OnMouseOverOut(self):
		if not self.Enable:
			return
		self.mouseReflector.Hide()

	def OnMouseLeftButtonDown(self):
		if not self.Enable:
			return
		self.mouseReflector.Down()

	def OnMouseLeftButtonUp(self):
		if not self.Enable:
			return
		self.mouseReflector.Up()
		self.event()

class ChangeGradeNameDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
	def Open(self):
		self.gradeNameSlot.SetText("")
		self.gradeNameSlot.SetFocus()
		xMouse, yMouse = wndMgr.GetMousePosition()
		self.SetPosition(xMouse - self.GetWidth()/2, yMouse + 50)
		self.SetTop()
		self.Show()
	def Close(self):
		self.gradeNameSlot.KillFocus()
		self.Hide()
		return True

	def SetGradeNumber(self, gradeNumber):
		self.gradeNumber = gradeNumber
	def GetGradeNumber(self):
		return self.gradeNumber
	def GetGradeName(self):
		return self.gradeNameSlot.GetText()

	def OnPressEscapeKey(self):
		self.Close()
		return True

class CommentSlot(ui.Window):

	TEXT_LIMIT = 35

	def __init__(self):
		ui.Window.__init__(self)

		self.slotImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/Parameter_Slot_06.sub", 0, 0)
		self.slotImage.AddFlag("not_pick")

		self.slotSimpleText = ui.MakeTextLine(self)
		self.slotSimpleText.SetPosition(2, 0)
		if localeInfo.IsARABIC() :
			self.slotSimpleText.SetWindowHorizontalAlignCenter()
			self.slotSimpleText.SetHorizontalAlignCenter()
		else :
			self.slotSimpleText.SetWindowHorizontalAlignLeft()
			self.slotSimpleText.SetHorizontalAlignLeft()

		self.bar = ui.SlotBar()
		self.bar.SetParent(self)
		self.bar.AddFlag("not_pick")
		self.bar.Hide()

		self.slotFullText = ui.MakeTextLine(self)
		self.slotFullText.SetPosition(2, 0)
		self.slotFullText.SetWindowHorizontalAlignLeft()
		self.slotFullText.SetHorizontalAlignLeft()

		self.SetSize(self.slotImage.GetWidth(), self.slotImage.GetHeight())
		self.len = 0

	def SetText(self, text):
		self.len = len(text)
		if len(text) > self.TEXT_LIMIT:
			limitText = grpText.GetSplitingTextLine(text, self.TEXT_LIMIT-3, 0)
			self.slotSimpleText.SetText(limitText + "...")
			self.bar.SetSize(self.len * 6 + 5, 17)

		else:
			self.slotSimpleText.SetText(text)

		self.slotFullText.SetText(text)
		self.slotFullText.SetPosition(2, 0)
		self.slotFullText.Hide()

	def OnMouseOverIn(self):
		if self.len > self.TEXT_LIMIT:
			self.bar.Show()
			self.slotFullText.Show()

	def OnMouseOverOut(self):
		if self.len > self.TEXT_LIMIT:
			self.bar.Hide()
			self.slotFullText.Hide()

class GuildWindow(ui.ScriptWindow):

	JOB_NAME = {	0 : localeInfo.JOB_WARRIOR,
			1 : localeInfo.JOB_ASSASSIN,
			2 : localeInfo.JOB_SURA,
			3 : localeInfo.JOB_SHAMAN, }

	if app.ENABLE_WOLFMAN_CHARACTER:
		JOB_NAME.update({4 : localeInfo.JOB_WOLFMAN,})

	GUILD_SKILL_PASSIVE_SLOT = 0
	GUILD_SKILL_ACTIVE_SLOT = 1
	GUILD_SKILL_AFFECT_SLOT = 2

	GRADE_SLOT_NAME = 0
	GRADE_ADD_MEMBER_AUTHORITY = 1
	GRADE_REMOVE_MEMBER_AUTHORITY = 2
	GRADE_NOTICE_AUTHORITY = 3
	GRADE_SKILL_AUTHORITY = 4
	GRADE_MANAGE_GUARD = 5

	MEMBER_LINE_COUNT = 13

	class PageWindow(ui.ScriptWindow):
		def __init__(self, parent, filename):
			ui.ScriptWindow.__init__(self)
			self.SetParent(parent)
			self.filename = filename
		def GetScriptFileName(self):
			return self.filename

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded=0

		self.__Initialize()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		print " ==================================== DESTROIED GUILD WINDOW"

	def __Initialize(self):

		#self.board = None
		self.pageName = None
		self.tabDict = None
		self.tabButtonDict = None
		self.pickDialog = None
		self.questionDialog = None
		self.offerDialog = None
		self.popupDialog = None
		self.moneyDialog = None
		self.changeGradeNameDialog = None
		self.popup = None

		self.popupMessage = None
		#self.commentSlot = None

		self.pageWindow = None
		self.tooltipSkill = None

		self.memberLinePos = 0
		self.openedDungeonIdx = -1
		self.openedDungeonDifficultyIdx = -1
		self.selectedDungeonIdx = -1
		self.selectedDungeonDifficultyIdx = -1

		self.nextUpdateTime = 0

		#self.enemyGuildNameList = []

	def Open(self):
		self.Show()
		self.SetTop()

		guildID = net.GetGuildID()
		self.largeMarkBox.SetIndex(guildID)
		self.largeMarkBox.SetScale(3)

	def Close(self):
		self.__CloseAllGuildMemberPageGradeComboBox()
		self.offerDialog.Close()
		self.popupDialog.Hide()
		self.changeGradeNameDialog.Hide()
		self.tooltipSkill.Hide()
	
		if hasattr(self, 'materialDonationDialog') and self.materialDonationDialog:
			self.materialDonationDialog.Close()
			self.materialDonationDialog = None
			
		self.Hide()

		self.pickDialog = None
		self.questionDialog = None
		self.popup = None

	def Destroy(self):
		self.ClearDictionary()

		if self.offerDialog:
			self.offerDialog.Destroy()

		if self.popupDialog:
			self.popupDialog.ClearDictionary()

		if self.changeGradeNameDialog:
			self.changeGradeNameDialog.ClearDictionary()

		if self.pageWindow:
			for window in self.pageWindow.values():
				window.ClearDictionary()
				# Clear member backgrounds
				if hasattr(window, 'memberBgDict'):
					window.memberBgDict.clear()
	
		if hasattr(self, 'expInfoWindow') and self.expInfoWindow:
			self.expInfoWindow.Close()
			self.expInfoWindow = None
		
		if hasattr(self, 'materialDonationDialog') and self.materialDonationDialog:
			self.materialDonationDialog.Close()
			self.materialDonationDialog = None

		self.__Initialize()

	def Show(self):
		if self.isLoaded==0:
			self.isLoaded=1

			self.__LoadWindow()

		self.RefreshGuildInfoPage()
		self.RefreshGuildBoardPage()
		self.RefreshGuildMemberPage()
		self.RefreshGuildSkillPage()
		self.RefreshGuildGradePage()

		ui.ScriptWindow.Show(self)

	def __LoadWindow(self):
		global DISABLE_GUILD_SKILL
		try:
			pyScrLoader = ui.PythonScriptLoader()

			if localeInfo.IsARABIC() :
				pyScrLoader.LoadScriptFile(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "guildwindow.py")
			else:
				pyScrLoader.LoadScriptFile(self, "uiscript/guildwindow.py")

			self.popupDialog = ui.ScriptWindow()
			pyScrLoader.LoadScriptFile(self.popupDialog, "UIScript/PopupDialog.py")

			self.changeGradeNameDialog = ChangeGradeNameDialog()
			pyScrLoader.LoadScriptFile(self.changeGradeNameDialog, "uiscript/changegradenamedialog.py")

			if localeInfo.IsARABIC():
				self.pageWindow = {
					"GUILD_INFO"	: self.PageWindow(self, "uiscript/guildwindow_guildinfopage_eu.py"),
					"BOARD"			: self.PageWindow(self, "uiscript/guildwindow_boardpage.py"),
					"MEMBER"		: self.PageWindow(self, "uiscript/guildwindow_memberpage.py"),
					"BASE_INFO"		: self.PageWindow(self, "uiscript/guildwindow_baseinfopage.py"),
					"SKILL"			: self.PageWindow(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "guildwindow_guildskillpage.py"),
					"GRADE"			: self.PageWindow(self, "uiscript/guildwindow_gradepage.py"),
				}
			elif localeInfo.IsJAPAN() :
				self.pageWindow = {
					"GUILD_INFO"	: self.PageWindow(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "guildwindow_guildinfopage.py"),
					"BOARD"			: self.PageWindow(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "guildwindow_boardpage.py"),
					"MEMBER"		: self.PageWindow(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "guildwindow_memberpage.py"),
					"BASE_INFO"		: self.PageWindow(self, "uiscript/guildwindow_baseinfopage.py"),
					"SKILL"			: self.PageWindow(self, "uiscript/guildwindow_guildskillpage.py"),
					"GRADE"			: self.PageWindow(self, "uiscript/guildwindow_gradepage.py"),
				}
			elif localeInfo.IsVIETNAM() :
				self.pageWindow = {
					"GUILD_INFO"	: self.PageWindow(self, "uiscript/guildwindow_guildinfopage_eu.py"),
					"BOARD"			: self.PageWindow(self, "uiscript/guildwindow_boardpage.py"),
					"MEMBER"		: self.PageWindow(self, "uiscript/guildwindow_memberpage.py"),
					"BASE_INFO"		: self.PageWindow(self, "uiscript/guildwindow_baseinfopage.py"),
					"SKILL"			: self.PageWindow(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "guildwindow_guildskillpage.py"),
					"GRADE"			: self.PageWindow(self, "uiscript/guildwindow_gradepage.py"),
				}
			elif localeInfo.IsEUROPE() and not app.GetLocalePath() == "locale/ca" :
				self.pageWindow = {
					"GUILD_INFO"	: self.PageWindow(self, "uiscript/guildwindow_guildinfopage_eu.py"),
					"BOARD"			: self.PageWindow(self, "uiscript/guildwindow_boardpage.py"),
					"MEMBER"		: self.PageWindow(self, "uiscript/guildwindow_memberpage.py"),
					"BASE_INFO"		: self.PageWindow(self, "uiscript/guildwindow_baseinfopage.py"),
					"SKILL"			: self.PageWindow(self, "uiscript/guildwindow_guildskillpage.py"),
					"GRADE"			: self.PageWindow(self, "uiscript/guildwindow_gradepage.py"),
				}
			else:
				self.pageWindow = {
					"GUILD_INFO"	: self.PageWindow(self, "uiscript/guildwindow_guildinfopage.py"),
					"BOARD"			: self.PageWindow(self, "uiscript/guildwindow_boardpage.py"),
					"MEMBER"		: self.PageWindow(self, "uiscript/guildwindow_memberpage.py"),
					"BASE_INFO"		: self.PageWindow(self, "uiscript/guildwindow_baseinfopage.py"),
					"SKILL"			: self.PageWindow(self, "uiscript/guildwindow_guildskillpage.py"),
					"GRADE"			: self.PageWindow(self, "uiscript/guildwindow_gradepage.py"),
				}

			for window in self.pageWindow.values():
				pyScrLoader.LoadScriptFile(window, window.GetScriptFileName())

		except:
			import exception
			exception.Abort("GuildWindow.__LoadWindow.LoadScript")

		try:
			getObject = self.GetChild

			#self.board = getObject("Board")
			self.pageName = {
				"GUILD_INFO"	: localeInfo.GUILD_TILE_INFO,
				"BOARD"			: localeInfo.GUILD_TILE_BOARD,
				"MEMBER"		: localeInfo.GUILD_TILE_MEMBER,
				"BASE_INFO"		: localeInfo.GUILD_TILE_BASEINFO,
				"SKILL"			: localeInfo.GUILD_TILE_SKILL,
				"GRADE"			: localeInfo.GUILD_TILE_GRADE,
			}

			self.tabDict = {
				"GUILD_INFO"	: getObject("Tab_01"),
				"BOARD"			: getObject("Tab_02"),
				"MEMBER"		: getObject("Tab_03"),
				"BASE_INFO"		: getObject("Tab_04"),
				"SKILL"			: getObject("Tab_05"),
				"GRADE"			: getObject("Tab_06"),
			}
			self.tabButtonDict = {
				"GUILD_INFO"	: getObject("Tab_Button_01"),
				"BOARD"			: getObject("Tab_Button_02"),
				"MEMBER"		: getObject("Tab_Button_03"),
				"BASE_INFO"		: getObject("Tab_Button_04"),
				"SKILL"			: getObject("Tab_Button_05"),
				"GRADE"			: getObject("Tab_Button_06"),
			}

			self.popupMessage = self.popupDialog.GetChild("message")
			self.popupDialog.GetChild("accept").SetEvent(ui.__mem_func__(self.popupDialog.Hide))

			self.changeGradeNameDialog.GetChild("AcceptButton").SetEvent(ui.__mem_func__(self.OnChangeGradeName))
			self.changeGradeNameDialog.GetChild("CancelButton").SetEvent(ui.__mem_func__(self.changeGradeNameDialog.Hide))
			self.changeGradeNameDialog.GetChild("Board").SetCloseEvent(ui.__mem_func__(self.changeGradeNameDialog.Hide))
			self.changeGradeNameDialog.gradeNameSlot = self.changeGradeNameDialog.GetChild("GradeNameValue")
			self.changeGradeNameDialog.gradeNameSlot.OnIMEReturn = ui.__mem_func__(self.OnChangeGradeName)
			self.changeGradeNameDialog.gradeNameSlot.OnPressEscapeKey = ui.__mem_func__(self.changeGradeNameDialog.Close)

			scrollBar = self.pageWindow["MEMBER"].GetChild("ScrollBar")
			scrollBar.SetScrollEvent(ui.__mem_func__(self.OnScrollMemberLine))
			self.pageWindow["MEMBER"].scrollBar = scrollBar

		except:
			import exception
			exception.Abort("GuildWindow.__LoadWindow.BindObject")

		self.itemTooltip = uiToolTip.ItemToolTip()
		self.__MakeInfoPage()
		self.__MakeBoardPage()
		self.__MakeMemberPage()
		self.__MakeBaseInfoPage()
		self.__MakeSkillPage()
		self.__MakeGradePage()

		for page in self.pageWindow.values():
			page.UpdateRect()

		for key, btn in self.tabButtonDict.items():
			btn.SetEvent(self.SelectPage, key)

		if DISABLE_GUILD_SKILL:
			self.tabButtonDict["SKILL"].Disable()

		self.SelectPage("GUILD_INFO")

		if app.ENABLE_CHEQUE_SYSTEM:
			self.offerDialog = uiPickETC.PickETCDialog()
		else:
			self.offerDialog = uiPickMoney.PickMoneyDialog()

		self.offerDialog.LoadDialog()
		self.offerDialog.SetMax(9)
		self.offerDialog.SetTitleName(localeInfo.GUILD_OFFER_EXP)
		self.offerDialog.SetAcceptEvent(ui.__mem_func__(self.OnOffer))

	def __MakeInfoPage(self):
		page = self.pageWindow["GUILD_INFO"]
		page.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
		page.isNeedLoadStorageInfo = True

		try:
			page.nameSlot = page.GetChild("GuildNameValue")
			page.masterNameSlot = page.GetChild("GuildMasterNameValue")
			page.guildLevelSlot = page.GetChild("GuildLevelValue")
			page.curExpSlot = page.GetChild("CurrentExperienceValue")
			page.memberCountSlot = page.GetChild("GuildMemberCountValue")
			page.levelAverageSlot = page.GetChild("GuildMemberLevelAverageValue")
			page.uploadMarkButton = page.GetChild("UploadGuildMarkButton")
			page.declareWarButton = page.GetChild("DeclareWarButton")
		
			try:
				page.expHoverInfo = page.GetChild("info_exp")
				page.expHoverInfo.SetEvent(ui.__mem_func__(self.__OnClickExpInfoButton))
			except:
				page.expHoverInfo = None
			
			# Add material slot
			page.materialSlot = page.GetChild("GuildMaterialSlot")
			page.materialSlot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			page.materialSlot.SetOverInItemEvent(ui.__mem_func__(self.__OverInMaterialItem))
			page.materialSlot.SetOverOutItemEvent(ui.__mem_func__(self.__OverOutMaterialItem))
			
			try:
				page.donateButton1 = page.GetChild("DonateItem1")
				page.donateButton2 = page.GetChild("DonateItem2")
				page.donateButton3 = page.GetChild("DonateItem3")

				page.donateButton1.SetEvent(ui.__mem_func__(self.__OnClickDonateButton), MATERIAL_STONE_INDEX)
				page.donateButton2.SetEvent(ui.__mem_func__(self.__OnClickDonateButton), MATERIAL_LOG_INDEX)
				page.donateButton3.SetEvent(ui.__mem_func__(self.__OnClickDonateButton), MATERIAL_PLYWOOD_INDEX)
			except:
				pass

			# Upgrade buttons for guild bonus skills
			BONUS_SLOT_NAMES = ["skill_bonus_1", "skill_bonus_2", "skill_bonus_3", "skill_bonus_4", "skill_bonus_5"]
			BONUS_BTN_NAMES  = ["upgrade_btn_0", "upgrade_btn_1", "upgrade_btn_2", "upgrade_btn_3", "upgrade_btn_4"]
			BONUS_VAL_NAMES  = ["double_drop_chance_value", "stone_value", "boss_value", "daily_raid_limit_value", "chance_double_fish_ore_value"]
			page.upgradeBtns      = []
			page.bonusValTexts    = []
			page.bonusSkillToolTips = []
			try:
				for i in xrange(5):
					btn = page.GetChild(BONUS_BTN_NAMES[i])
					btn.SetEvent(ui.__mem_func__(self.__OnClickUpgradeBonusSkill), i)
					tip = self.__CreateBonusSkillToolTip(i)
					page.bonusSkillToolTips.append(tip)
					btn.SetShowToolTipEvent(tip.ShowToolTip)
					btn.SetHideToolTipEvent(tip.HideToolTip)
					page.upgradeBtns.append(btn)
					page.bonusValTexts.append(page.GetChild(BONUS_VAL_NAMES[i]))
			except:
				pass
			
			# Add count text references
			page.materialCount1 = page.GetChild("item_1_count")
			page.materialCount2 = page.GetChild("item_2_count")
			page.materialCount3 = page.GetChild("item_3_count")

			try:
				page.guildMoneySlot = page.GetChild("GuildMoneyValue")
			except KeyError:
				page.guildMoneySlot = None

			try:
				page.GetChild("DepositButton").SetEvent(ui.__mem_func__(self.__OnClickDepositButton))
				page.GetChild("WithdrawButton").SetEvent(ui.__mem_func__(self.__OnClickWithdrawButton))
			except KeyError:
				pass

			page.uploadMarkButton.SetEvent(ui.__mem_func__(self.__OnClickSelectGuildMarkButton))
			page.declareWarButton.SetEvent(ui.__mem_func__(self.__OnClickDeclareWarButton))

			self.largeMarkBox = page.GetChild("LargeGuildMark")
		except:
			import exception
			exception.Abort("GuildWindow.__MakeInfoPage")

		self.largeMarkBox.AddFlag("not_pick")

		self.markSelectDialog=uiUploadMark.MarkSelectDialog()
		self.markSelectDialog.SAFE_SetSelectEvent(self.__OnSelectMark)

		self.symbolSelectDialog=uiUploadMark.SymbolSelectDialog()
		self.symbolSelectDialog.SAFE_SetSelectEvent(self.__OnSelectSymbol)


	def __MakeBoardPage(self):
		page = self.pageWindow["BOARD"]
		page.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
		page.isNeedLoadGuardPage = True
		
		try:
			# Get guardian equipment slot
			page.wndEquip = page.GetChild("GuardEq_Slots")
			page.wndEquip.SetSelectEmptySlotEvent(ui.__mem_func__(self.OnSelectEmptyGuardSlotItem))
			#page.wndEquip.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
			page.wndEquip.SetUnselectItemSlotEvent(ui.__mem_func__(self.OnClickEquippedGuardItem))
			page.wndEquip.SetUseSlotEvent(ui.__mem_func__(self.OnClickEquippedGuardItem))
			page.wndEquip.SetOverInItemEvent(ui.__mem_func__(self.OverInGuardItem))
			page.wndEquip.SetOverOutItemEvent(ui.__mem_func__(self.OverOutGuardItem))
		
			page.currentSetID = 0
			page.isCheckGuardItemButtonActiveTime = False
			page.equipmentActivateBtn = page.GetChild("GuardEq_Activate_Button")
			page.equipmentActivateBtn.SAFE_SetEvent(self.OnClickActivateGuardItems)
			page.equipmentActivateBtn.isDisabled = False
			page.equipmentDeactivateBtn = page.GetChild("GuardEq_Deactivate_Button")
			page.equipmentDeactivateBtn.SAFE_SetEvent(self.OnClickDeactivateGuardItems)
			page.equipmentDeactivateBtn.isDisabled = False
			page.equipmentActivateBtnTxt = page.GetChild("GuardEq_Activate_LimitTxt")
			page.equipmentDeactivateBtnTxt = page.GetChild("GuardEq_Deactivate_LimitTxt")
			page.guardActivateStatusText = page.GetChild("GuardEq_IsActivate")
			
			# Get bonus value text elements
			page.bonusValues = {}
			try:
				page.bonusValues["max_member"] = page.GetChild("maximum_member_count_value")
				page.bonusValues["bonus_raid"] = page.GetChild("chance_bonus_raid_value")
				page.bonusValues["vs_monster"] = page.GetChild("strong_against_monster_value")
				page.bonusValues["vs_elements"] = page.GetChild("strong_against_elements_value")
				page.bonusValues["vs_stones"] = page.GetChild("strong_against_stones_value")
				page.bonusValues["vs_boss"] = page.GetChild("strong_against_bosses_value")
				page.bonusValues["avg_damage"] = page.GetChild("average_damage_value")
				page.bonusValues["skill_damage"] = page.GetChild("skill_damage_value")
			except:
				pass
				
		except:
			import exception
			exception.Abort("GuildWindow.__MakeBoardPage")
			
	def OnClickGuardEqSetBtn(self, setID, force=False):
		if not force:
			if guild.GetActiveSetID() != 0:
				chat.AppendChat(1, "err 1")
				return
		page = self.pageWindow["BOARD"]
		page.currentSetID = setID

		self.RefreshGuardItemSlots()

	def RefreshGuardItemSlots(self):
		page = self.pageWindow["BOARD"]
		allItemsPower = 0
		allBonusesDict = {}
		
		# Collect all bonuses from equipped items
		for i in xrange(9):
			itemVnum = guild.GetGuardItemVnum(0, i)  # Set 0
			page.wndEquip.SetItemSlot(i, itemVnum, 0)
			if itemVnum == 0:
				continue
			
			allItemsPower += guild.GetGuardItemPower(0, i)
			
			item.SelectItem(itemVnum)
			for j in xrange(item.ITEM_APPLY_MAX_NUM):
				(affectType, affectValue) = item.GetAffect(j)
				if affectType == 0:
					continue
				if affectType in allBonusesDict:
					allBonusesDict[affectType] += affectValue
				else:
					allBonusesDict[affectType] = affectValue

		activeSetID = guild.GetActiveSetID()
		if activeSetID != 0:
			expireTime = guild.GetGuardBonusExpireTime()
			remaining = expireTime - app.GetGlobalTimeStamp()
			if remaining > 0:
				hours = remaining // 3600
				minutes = (remaining % 3600) // 60
				page.guardActivateStatusText.SetText("%s (%dh %dm)" % (localeInfo.GUILD_GUARDIAN_BONUSES_ACTIVE, hours, minutes))
			else:
				page.guardActivateStatusText.SetText(localeInfo.GUILD_GUARDIAN_BONUSES_ACTIVE)
			page.guardActivateStatusText.SetPackedFontColor(0xFF00FF00)
		else:
			page.guardActivateStatusText.SetText(localeInfo.GUILD_GUARDIAN_BONUSES_INACTIVE)
			page.guardActivateStatusText.SetPackedFontColor(0xFF616060)
		
		if "vs_monster" in page.bonusValues:
			vsMonster = allBonusesDict.get(item.APPLY_ATTBONUS_MONSTER, 0)
			page.bonusValues["vs_monster"].SetText("%d%%" % vsMonster if vsMonster > 0 else "0%")
		
		if "vs_elements" in page.bonusValues:
			vsElements = allBonusesDict.get(item.APPLY_ATTBONUS_ELEMENT, 0)
			page.bonusValues["vs_elements"].SetText("%d%%" % vsElements if vsElements > 0 else "0%")
		
		if "vs_stones" in page.bonusValues:
			vsStones = allBonusesDict.get(item.APPLY_ATTBONUS_STONE, 0)
			page.bonusValues["vs_stones"].SetText("%d%%" % vsStones if vsStones > 0 else "0%")
		
		if "vs_boss" in page.bonusValues:
			vsBoss = allBonusesDict.get(item.APPLY_ATTBONUS_BOSS, 0)
			page.bonusValues["vs_boss"].SetText("%d%%" % vsBoss if vsBoss > 0 else "0%")
		
		if "avg_damage" in page.bonusValues:
			avgDmg = allBonusesDict.get(item.APPLY_NORMAL_HIT_DAMAGE_BONUS, 0)
			if avgDmg == 0:
				avgDmg = allBonusesDict.get(item.APPLY_NORMAL_HIT_DAMAGE_BONUS, 0)
			page.bonusValues["avg_damage"].SetText("%d%%" % avgDmg if avgDmg > 0 else "0%")
		
		if "skill_damage" in page.bonusValues:
			skillDmg = allBonusesDict.get(item.APPLY_SKILL_DAMAGE_BONUS, 0)
			page.bonusValues["skill_damage"].SetText("%d%%" % skillDmg if skillDmg > 0 else "0%")

		page.wndEquip.RefreshSlot()

	def OnClickActivateGuardItems(self):
		if not guild.HaveAnyGuardSetItem(0):  # <-- Set 0
			chat.AppendChat(1, localeInfo.GUILD_GUARDIAN_NEED_ONE_ITEM)
			return
		chat.AppendChat(1, localeInfo.GUILD_GUARDIAN_ACTIVATE_FEE_INFO)
		net.SendChatPacket("/gguard_items 2")

	def OnClickDeactivateGuardItems(self):
		net.SendChatPacket("/gguard_items 0")

	def OnSelectEmptyGuardSlotItem(self, slotIdx):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		activeSetID = guild.GetActiveSetID()
		
		if activeSetID != 0:
			chat.AppendChat(1, "Deactivate bonuses of guild guardian items before unequpping them!")
			return

		if mouseModule.mouseController.isAttached():
			if player.SLOT_TYPE_INVENTORY == mouseModule.mouseController.GetAttachedType():
				itemIndex = mouseModule.mouseController.GetAttachedItemIndex()
				item.SelectItem(itemIndex)
				
				itemType = item.GetItemType()
				#itemSubType = item.GetSubType()
				
				if itemType == item.ITEM_TYPE_GUILD_GUARD:
					slotNumber = mouseModule.mouseController.GetAttachedSlotNumber()
					
					net.SendItemUseToItemPacket(player.INVENTORY, slotNumber, 0, slotIdx)
				else:
					chat.AppendChat(1, "ERROR: Item is not GUILD_GUARD type!")
			else:
				chat.AppendChat(1, "ERROR: Attached item is not from inventory!")
			
			mouseModule.mouseController.DeattachObject()
		else:
			chat.AppendChat(1, "ERROR: No item attached to mouse!")

	def UseGuardItemFromInventory(self, slotNumber):
		if not self.IsShow():
			chat.AppendChat(1, "Open guild guardian page first!")
			return
		if not self.pageWindow["BOARD"].IsShow():
			chat.AppendChat(1, "Open guild guardian page first!")
			return
		if guild.GetActiveSetID() != 0:
			chat.AppendChat(1, "Deactivate the guardian items bonuses first!")
			return
		net.SendItemUseToItemPacket(player.INVENTORY, slotNumber, 0, 0)

	def OnClickEquippedGuardItem(self, slotIdx):
		net.SendChatPacket("/gguard_items 1 %d %d"%(0, slotIdx))

	def RefreshCanActivateGuardItemsButton(self):
		page = self.pageWindow["BOARD"]
		restTime = guild.GetNextCanActivateTime()-app.GetGlobalTimeStamp()
		if restTime > 0:
			page.equipmentActivateBtnTxt.SetText("%ds."%restTime)
			page.equipmentDeactivateBtnTxt.SetText("%ds."%restTime)
			if not page.equipmentActivateBtn.isDisabled:
				page.equipmentActivateBtn.Down()
				page.equipmentActivateBtn.Disable()
				page.equipmentActivateBtn.isDisabled = True
				page.equipmentDeactivateBtn.Down()
				page.equipmentDeactivateBtn.Disable()
				page.equipmentDeactivateBtn.isDisabled = True
		else:
			page.isCheckGuardItemButtonActiveTime = False
			page.equipmentActivateBtnTxt.SetText("")
			page.equipmentDeactivateBtnTxt.SetText("")
			page.equipmentActivateBtn.Enable()
			page.equipmentActivateBtn.SetUp()
			page.equipmentActivateBtn.isDisabled = False
			page.equipmentDeactivateBtn.Enable()
			page.equipmentDeactivateBtn.SetUp()
			page.equipmentDeactivateBtn.isDisabled = False
			
			if guild.GetActiveSetID() != 0:
				page.equipmentActivateBtn.Hide()
				page.equipmentDeactivateBtn.Show()
			else:
				page.equipmentActivateBtn.Show()
				page.equipmentDeactivateBtn.Hide()

	def OnUpdate(self):
		if app.GetGlobalTimeStamp() >= self.nextUpdateTime:
			self.nextUpdateTime = app.GetGlobalTimeStamp() + 1
			
			if self.pageWindow["BOARD"].isCheckGuardItemButtonActiveTime:
				self.RefreshCanActivateGuardItemsButton()

	def OverInGuardItem(self, slotIdx):
		saveState = constInfo.SHOW_GUARD_OTHER_GUILD_SOCKET_INFO
		constInfo.SHOW_GUARD_OTHER_GUILD_SOCKET_INFO = False
		self.itemTooltip.SetItemToolTip(guild.GetGuardItemVnum(0, slotIdx))
		constInfo.SHOW_GUARD_OTHER_GUILD_SOCKET_INFO = saveState

	def OverOutGuardItem(self):
		self.itemTooltip.HideToolTip()
	
	def __UpdateGuardianBonuses(self):
		page = self.pageWindow["BOARD"]
		
		if not hasattr(page, 'bonusValues'):
			return
		
		bonuses = {
			"max_member": 0,
			"bonus_raid": 0,
			"vs_monster": 0,
			"vs_elements": 0,
			"vs_stones": 0,
			"vs_boss": 0,
			"avg_damage": 0,
			"skill_damage": 0
		}
		
		# Update text displays
		for key, textLine in page.bonusValues.items():
			bonus = bonuses.get(key, 0)
			if key == "max_member":
				textLine.SetText("+%d" % bonus)
			else:
				textLine.SetText("+%d%%" % bonus)

	def __MakeMemberPage(self):

		page = self.pageWindow["MEMBER"]

		page.memberClipWindow = ui.Window()
		page.memberClipWindow.SetParent(page)
		page.memberClipWindow.SetPosition(0, 67)
		page.memberClipWindow.SetSize(595, 230)
		page.memberClipWindow.SetInsideRender(True)
		page.memberClipWindow.AddFlag("not_pick")
		page.memberClipWindow.Show()

		page.memberWindow = page.GetChild("window_members")
		page.memberWindow.SetParent(page.memberClipWindow)
		page.memberWindow.SetPosition(0, 0)
		page.memberWindow.OnMouseWheel = ui.__mem_func__(self.OnMouseWheelMemberList)

		page.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))

		page.scrollBar = page.GetChild("ScrollBar")
		page.scrollBar.SetScrollEvent(ui.__mem_func__(self.OnScrollMemberList))

		lineStep = 20
		page.memberDict = {}
		page.memberBgDict = {}
		page.memberLineCount = 0

	def OnScrollMemberList(self):
		if "MEMBER" not in self.pageWindow:
			return
		page = self.pageWindow["MEMBER"]
		pos = page.scrollBar.GetPos()
		lineStep = 24
		visibleHeight = 238
		totalHeight = max(page.memberLineCount * lineStep, visibleHeight)
		scrollRange = max(0, totalHeight - visibleHeight)
		page.memberWindow.SetPosition(0, -int(scrollRange * pos))

	def OnMouseWheelMemberList(self, length):
		if "MEMBER" not in self.pageWindow:
			return True
		self.pageWindow["MEMBER"].scrollBar.OnMouseWheel(length)
		return True

	def OnScrollLogsLine(self):
		page = self.pageWindow["SKILL"]
		
		scrollBar = page.logsScrollBar
		pos = scrollBar.GetPos()
	
		count = guild.GetLogsCount()
		newLinePos = int(float(count - LOGS_MAX_COUNT) * pos)
	
		if newLinePos != page.logsLinePos:
			page.logsLinePos = newLinePos
			self.RefreshLogs(True)

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem
		if self.pageWindow and "GUILD_INFO" in self.pageWindow:
			page = self.pageWindow["GUILD_INFO"]
			if hasattr(page, 'materialSlot'):
				page.materialSlot.SetItemToolTip(tooltipItem)

	def __MakeBaseInfoPage(self):
		page = self.pageWindow["BASE_INFO"]
		page.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
		page.isNeedLoadHistory = True

		page.scrollBar = page.GetChild("DungeonsScrollBar")
		page.scrollBar.SetScrollEvent(ui.__mem_func__(self.OnScrollDungeonList))
		page.dungeonOpenBtn = page.GetChild("DungeonOpenButton")
		page.dungeonOpenBtn.ButtonText.SetOutline()
		page.dungeonOpenBtn.ButtonText.SetHorizontalAlignLeft()
		page.dungeonOpenBtn.ButtonText.SetWindowHorizontalAlignLeft()
		page.dungeonOpenBtn.ButtonText.SetPosition(7, 13)
		page.dungeonOpenBtn.SAFE_SetEvent(self.OnClickDungeonOpenButton)
		
		page.dungeonTeleportBtn = page.GetChild("DungeonTeleportButton")
		page.dungeonTeleportBtn.Disable()
		page.dungeonTeleportBtn.SAFE_SetEvent(self.OnClickDungeonTeleportButton)

		page.dungBg = page.GetChild("dung_bg")
		page.prefMemberCountTxt = page.GetChild("Dungeon_Suggest_MembersCount")
		#page.prefMonsterAttBonusTxt = page.GetChild("Dungeon_Suggest_MonsterAttBonus")
		page.prefMonsterDefBonusTxt = page.GetChild("Dungeon_Suggest_MonsterDefBonus")
		page.prefLordAttBonusTxt = page.GetChild("Dungeon_Suggest_LordAttBonus")
		#page.prefLordDefBonusTxt = page.GetChild("Dungeon_Suggest_LordDefBonus")
		#page.maxLimitTimeTxt = page.GetChild("Dungeon_MaxLimitTime")
		#page.doneCountTxt = page.GetChild("Dungeon_DoneCount")
		#page.bestDoneTimeTxt = page.GetChild("Dungeon_BestDoneTime")
		page.canGetYangCountTxt = page.GetChild("Dungeon_CanGetYangCount")
		#page.canGetExpCountTxt = page.GetChild("Dungeon_CanGetExpCount")

		page.dungeonsListBoxClipWindow = ui.Window()
		page.dungeonsListBoxClipWindow.SetParent(page)
		page.dungeonsListBoxClipWindow.SetPosition(13, 64)
		page.dungeonsListBoxClipWindow.SetSize(186, 242)
		page.dungeonsListBoxClipWindow.OnMouseWheel = page.scrollBar.OnMouseWheel
		page.dungeonsListBoxClipWindow.Show()
		
		page.dungeonsListBox = ui.Window()
		page.dungeonsListBox.SetParent(page.dungeonsListBoxClipWindow)
		page.dungeonsListBox.SetPosition(0, 0)
		page.dungeonsListBox.SetSize(186, 242)
		page.dungeonsListBox.OnMouseWheel = page.scrollBar.OnMouseWheel
		page.dungeonsListBox.Show()

		page.dungeonItems = []
		i = 0
		yPos = 0
		for dungData in DUNGEONS:
			item = DungeonListItem(i, dungData[0])
			item.SetParent(page.dungeonsListBox)
			item.SetPosition(0, yPos)
			item.SetSelectDungeonEvent(ui.__mem_func__(self.OnSelectDungeon))
			item.SetClippingMaskWindow(page.dungeonsListBoxClipWindow)
			item.Show()
			page.dungeonItems.append(item)
			yPos += 91
			i += 1
			
		totalContentHeight = len(DUNGEONS) * 91
		page.scrollBar.SetContentHeight(totalContentHeight)

		page.possibleDropItemsGrid = ui.GridSlotWindow()
		page.possibleDropItemsGrid.SetParent(page)
		page.possibleDropItemsGrid.SetPosition(212, 211)
		page.possibleDropItemsGrid.ArrangeSlot(0, 6, 3, 32, 32, 0, 0)
		page.possibleDropItemsGrid.RefreshSlot()
		page.possibleDropItemsGrid.SetOverInItemEvent(ui.__mem_func__(self.OverInDungeonPossibleDropItem))
		page.possibleDropItemsGrid.SetOverOutItemEvent(ui.__mem_func__(self.OverOutDungeonPossibleDropItem))
		page.possibleDropItemsGrid.SetSlotBaseImage("elvion/game/guild/guild_raids/slot.png", 1.0, 1.0, 1.0, 1.0)
		page.possibleDropItemsGrid.Show()

		self.OnSelectDungeon(0, 0)
		
		self.startDungeonBoard = ui.BoardWithTitleBar()
		self.startDungeonBoard.AddFlag("float")
		self.startDungeonBoard.SetSize(400, 201)
		self.startDungeonBoard.SetTitleName("Confirm opening of guild raid")
		self.startDungeonBoard.SetCloseEvent(ui.__mem_func__(self.OnClickCancelStartDungeon))
		self.startDungeonBoard.SetCenterPosition()
		self.startDungeonBoard.Hide()
		
		self.startDungeonBoardImg = ui.ImageBox()
		self.startDungeonBoardImg.SetParent(self.startDungeonBoard)
		self.startDungeonBoardImg.SetPosition(8, 33)
		self.startDungeonBoardImg.LoadImage("elvion/game/guild/guild_raids/bg_popup_open.png")
		self.startDungeonBoardImg.Show()

		self.startDungeonBoard_txt1 = ui.TextLine("Tahoma:14")
		self.startDungeonBoard_txt1.SetParent(self.startDungeonBoard)
		self.startDungeonBoard_txt1.SetPackedFontColor(0xFFDDDDDD)
		self.startDungeonBoard_txt1.SetPosition(0, 55)
		self.startDungeonBoard_txt1.SetHorizontalAlignCenter()
		self.startDungeonBoard_txt1.SetWindowHorizontalAlignCenter()
		self.startDungeonBoard_txt1.SetOutline()
		self.startDungeonBoard_txt1.SetText("You are currently opening guild raid.")
		self.startDungeonBoard_txt1.Show()

		self.startDungeonBoard_txt2 = ui.TextLine("Tahoma:14")
		self.startDungeonBoard_txt2.SetParent(self.startDungeonBoard)
		self.startDungeonBoard_txt2.SetPackedFontColor(0xFFDDDDDD)
		self.startDungeonBoard_txt2.SetPosition(0, 71)
		self.startDungeonBoard_txt2.SetHorizontalAlignCenter()
		self.startDungeonBoard_txt2.SetWindowHorizontalAlignCenter()
		self.startDungeonBoard_txt2.SetOutline()
		self.startDungeonBoard_txt2.SetText("Your guild will have 3 hours to finish the raid.")
		self.startDungeonBoard_txt2.Show()

		self.startDungeonBoard_txt3 = ui.TextLine("Tahoma:14")
		self.startDungeonBoard_txt3.SetParent(self.startDungeonBoard)
		self.startDungeonBoard_txt3.SetPackedFontColor(0xFFDDDDDD)
		self.startDungeonBoard_txt3.SetPosition(0, 92)
		self.startDungeonBoard_txt3.SetHorizontalAlignCenter()
		self.startDungeonBoard_txt3.SetWindowHorizontalAlignCenter()
		self.startDungeonBoard_txt3.SetOutline()
		self.startDungeonBoard_txt3.SetText("Remaining entries: 3 Free (+3 Paid)")
		self.startDungeonBoard_txt3.Show()

		self.startDungeonBoard_txt4 = ui.TextLine("Tahoma:14")
		self.startDungeonBoard_txt4.SetParent(self.startDungeonBoard)
		self.startDungeonBoard_txt4.SetPackedFontColor(0xFFDDDDDD)
		self.startDungeonBoard_txt4.SetPosition(0, 111)
		self.startDungeonBoard_txt4.SetHorizontalAlignCenter()
		self.startDungeonBoard_txt4.SetWindowHorizontalAlignCenter()
		self.startDungeonBoard_txt4.SetOutline()
		self.startDungeonBoard_txt4.SetText("Opening raid will take 1 free entry.")
		self.startDungeonBoard_txt4.Show()

		self.startDungeonBoard_txt5 = ui.TextLine("Tahoma:14")
		self.startDungeonBoard_txt5.SetParent(self.startDungeonBoard)
		self.startDungeonBoard_txt5.SetPackedFontColor(0xFFe0d38a)
		self.startDungeonBoard_txt5.SetPosition(0, 141)
		self.startDungeonBoard_txt5.SetHorizontalAlignCenter()
		self.startDungeonBoard_txt5.SetWindowHorizontalAlignCenter()
		self.startDungeonBoard_txt5.SetOutline()
		self.startDungeonBoard_txt5.SetText("Are you sure you want to open raid?")
		self.startDungeonBoard_txt5.Show()

		self.startDungeonBoard_acceptBtn = ui.Button()
		self.startDungeonBoard_acceptBtn.SetParent(self.startDungeonBoard)
		self.startDungeonBoard_acceptBtn.SetPosition(-34, 38)
		self.startDungeonBoard_acceptBtn.SetUpVisual("d:/ymir work/ui/public/small_thin_button_01.sub")
		self.startDungeonBoard_acceptBtn.SetOverVisual("d:/ymir work/ui/public/small_thin_button_02.sub")
		self.startDungeonBoard_acceptBtn.SetDownVisual("d:/ymir work/ui/public/small_thin_button_03.sub")
		self.startDungeonBoard_acceptBtn.SAFE_SetEvent(self.OnClickStartDungeon)
		self.startDungeonBoard_acceptBtn.SetText("Yes", -2)
		self.startDungeonBoard_acceptBtn.SetWindowHorizontalAlignCenter()
		self.startDungeonBoard_acceptBtn.SetWindowVerticalAlignBottom()
		self.startDungeonBoard_acceptBtn.Show()

		self.startDungeonBoard_cancelBtn = ui.Button()
		self.startDungeonBoard_cancelBtn.SetParent(self.startDungeonBoard)
		self.startDungeonBoard_cancelBtn.SetPosition(34, 38)
		self.startDungeonBoard_cancelBtn.SetUpVisual("d:/ymir work/ui/public/small_thin_button_01.sub")
		self.startDungeonBoard_cancelBtn.SetOverVisual("d:/ymir work/ui/public/small_thin_button_02.sub")
		self.startDungeonBoard_cancelBtn.SetDownVisual("d:/ymir work/ui/public/small_thin_button_03.sub")
		self.startDungeonBoard_cancelBtn.SAFE_SetEvent(self.OnClickCancelStartDungeon)
		self.startDungeonBoard_cancelBtn.SetText("No", -2)
		self.startDungeonBoard_cancelBtn.SetWindowHorizontalAlignCenter()
		self.startDungeonBoard_cancelBtn.SetWindowVerticalAlignBottom()
		self.startDungeonBoard_cancelBtn.Show()
		
		self.buyPremiumDungeonTicketBoard = ui.BoardWithTitleBar()
		self.buyPremiumDungeonTicketBoard.AddFlag("float")
		self.buyPremiumDungeonTicketBoard.SetSize(400, 201)
		self.buyPremiumDungeonTicketBoard.SetTitleName("Not enough free entries")
		self.buyPremiumDungeonTicketBoard.SetCloseEvent(ui.__mem_func__(self.OnClickCancelBuyExtraDungeonStartTicket))
		self.buyPremiumDungeonTicketBoard.SetCenterPosition()
		self.buyPremiumDungeonTicketBoard.Hide()
		
		self.buyPremiumDungeonTicketBoardImg = ui.ImageBox()
		self.buyPremiumDungeonTicketBoardImg.SetParent(self.buyPremiumDungeonTicketBoard)
		self.buyPremiumDungeonTicketBoardImg.SetPosition(8, 33)
		self.buyPremiumDungeonTicketBoardImg.LoadImage("elvion/game/guild/guild_raids/bg_popup.png")
		self.buyPremiumDungeonTicketBoardImg.Show()

		self.buyPremiumDungeonTicketBoard_txt1 = ui.TextLine("Tahoma:14")
		self.buyPremiumDungeonTicketBoard_txt1.SetParent(self.buyPremiumDungeonTicketBoard)
		self.buyPremiumDungeonTicketBoard_txt1.SetPackedFontColor(0xFFDDDDDD)
		self.buyPremiumDungeonTicketBoard_txt1.SetPosition(0, 55)
		self.buyPremiumDungeonTicketBoard_txt1.SetHorizontalAlignCenter()
		self.buyPremiumDungeonTicketBoard_txt1.SetWindowHorizontalAlignCenter()
		self.buyPremiumDungeonTicketBoard_txt1.SetOutline()
		self.buyPremiumDungeonTicketBoard_txt1.SetText("It looks like you already spent 3 free entries.")
		self.buyPremiumDungeonTicketBoard_txt1.Show()

		self.buyPremiumDungeonTicketBoard_txt2 = ui.TextLine("Tahoma:14")
		self.buyPremiumDungeonTicketBoard_txt2.SetParent(self.buyPremiumDungeonTicketBoard)
		self.buyPremiumDungeonTicketBoard_txt2.SetPackedFontColor(0xFFDDDDDD)
		self.buyPremiumDungeonTicketBoard_txt2.SetPosition(0, 71)
		self.buyPremiumDungeonTicketBoard_txt2.SetHorizontalAlignCenter()
		self.buyPremiumDungeonTicketBoard_txt2.SetWindowHorizontalAlignCenter()
		self.buyPremiumDungeonTicketBoard_txt2.SetOutline()
		self.buyPremiumDungeonTicketBoard_txt2.SetText("Free entries are reseted each midnight.")
		self.buyPremiumDungeonTicketBoard_txt2.Show()

		self.buyPremiumDungeonTicketBoard_txt3 = ui.TextLine("Tahoma:14")
		self.buyPremiumDungeonTicketBoard_txt3.SetParent(self.buyPremiumDungeonTicketBoard)
		self.buyPremiumDungeonTicketBoard_txt3.SetPackedFontColor(0xFFDDDDDD)
		self.buyPremiumDungeonTicketBoard_txt3.SetPosition(0, 92)
		self.buyPremiumDungeonTicketBoard_txt3.SetHorizontalAlignCenter()
		self.buyPremiumDungeonTicketBoard_txt3.SetWindowHorizontalAlignCenter()
		self.buyPremiumDungeonTicketBoard_txt3.SetOutline()
		self.buyPremiumDungeonTicketBoard_txt3.SetText("If you dont want to wait, you can buy premium entries.")
		self.buyPremiumDungeonTicketBoard_txt3.Show()

		self.buyPremiumDungeonTicketBoard_txt4 = ui.TextLine("Tahoma:14")
		self.buyPremiumDungeonTicketBoard_txt4.SetParent(self.buyPremiumDungeonTicketBoard)
		self.buyPremiumDungeonTicketBoard_txt4.SetPackedFontColor(0xFFDDDDDD)
		self.buyPremiumDungeonTicketBoard_txt4.SetPosition(0, 151)
		self.buyPremiumDungeonTicketBoard_txt4.SetHorizontalAlignCenter()
		self.buyPremiumDungeonTicketBoard_txt4.SetWindowHorizontalAlignCenter()
		self.buyPremiumDungeonTicketBoard_txt4.SetOutline()
		self.buyPremiumDungeonTicketBoard_txt4.SetText("Today you can buy: 3 premium entries")
		self.buyPremiumDungeonTicketBoard_txt4.Show()

		self.buyPremiumDungeonTicketBoard_txt5 = ui.TextLine("Tahoma:14")
		self.buyPremiumDungeonTicketBoard_txt5.SetParent(self.buyPremiumDungeonTicketBoard)
		self.buyPremiumDungeonTicketBoard_txt5.SetPackedFontColor(0xFFe0d38a)
		self.buyPremiumDungeonTicketBoard_txt5.SetPosition(0, 107)
		self.buyPremiumDungeonTicketBoard_txt5.SetHorizontalAlignCenter()
		self.buyPremiumDungeonTicketBoard_txt5.SetWindowHorizontalAlignCenter()
		self.buyPremiumDungeonTicketBoard_txt5.SetOutline()
		self.buyPremiumDungeonTicketBoard_txt5.SetText("Do you want to buy premium entry?")
		self.buyPremiumDungeonTicketBoard_txt5.Show()

		self.buyPremiumDungeonTicketBoard_txt6 = ui.TextLine("Tahoma:12")
		self.buyPremiumDungeonTicketBoard_txt6.SetParent(self.buyPremiumDungeonTicketBoard)
		self.buyPremiumDungeonTicketBoard_txt6.SetPackedFontColor(0xFFeed8c3)
		self.buyPremiumDungeonTicketBoard_txt6.SetPosition(0, 135)
		self.buyPremiumDungeonTicketBoard_txt6.SetHorizontalAlignCenter()
		self.buyPremiumDungeonTicketBoard_txt6.SetWindowHorizontalAlignCenter()
		self.buyPremiumDungeonTicketBoard_txt6.SetOutline()
		self.buyPremiumDungeonTicketBoard_txt6.SetText("Possible to buy today")
		self.buyPremiumDungeonTicketBoard_txt6.Show()

		self.buyPremiumDungeonTicketBoard_acceptBtn = ui.Button()
		self.buyPremiumDungeonTicketBoard_acceptBtn.SetParent(self.buyPremiumDungeonTicketBoard)
		self.buyPremiumDungeonTicketBoard_acceptBtn.SetPosition(-126, 34)
		self.buyPremiumDungeonTicketBoard_acceptBtn.SetUpVisual("elvion/game/guild/guild_raids/popup_btn_norm.png")
		self.buyPremiumDungeonTicketBoard_acceptBtn.SetOverVisual("elvion/game/guild/guild_raids/popup_btn_hover.png")
		self.buyPremiumDungeonTicketBoard_acceptBtn.SetDownVisual("elvion/game/guild/guild_raids/popup_btn_down.png")
		self.buyPremiumDungeonTicketBoard_acceptBtn.SAFE_SetEvent(self.OnClickBuyExtraDungeonStartTicket, 0)
		self.buyPremiumDungeonTicketBoard_acceptBtn.SetText("Yes, I have coupon", -1)
		self.buyPremiumDungeonTicketBoard_acceptBtn.SetWindowHorizontalAlignCenter()
		self.buyPremiumDungeonTicketBoard_acceptBtn.SetWindowVerticalAlignBottom()
		self.buyPremiumDungeonTicketBoard_acceptBtn.Show()

		self.buyPremiumDungeonTicketBoard_accept2Btn = ui.Button()
		self.buyPremiumDungeonTicketBoard_accept2Btn.SetParent(self.buyPremiumDungeonTicketBoard)
		self.buyPremiumDungeonTicketBoard_accept2Btn.SetPosition(0, 34)
		self.buyPremiumDungeonTicketBoard_accept2Btn.SetUpVisual("elvion/game/guild/guild_raids/popup_btn_norm.png")
		self.buyPremiumDungeonTicketBoard_accept2Btn.SetOverVisual("elvion/game/guild/guild_raids/popup_btn_hover.png")
		self.buyPremiumDungeonTicketBoard_accept2Btn.SetDownVisual("elvion/game/guild/guild_raids/popup_btn_down.png")
		self.buyPremiumDungeonTicketBoard_accept2Btn.SAFE_SetEvent(self.OnClickBuyExtraDungeonStartTicket, 1)
		self.buyPremiumDungeonTicketBoard_accept2Btn.SetText("Buy for - %s"%TICKET_COST, -1)
		self.buyPremiumDungeonTicketBoard_accept2Btn.SetWindowHorizontalAlignCenter()
		self.buyPremiumDungeonTicketBoard_accept2Btn.SetWindowVerticalAlignBottom()
		self.buyPremiumDungeonTicketBoard_accept2Btn.Show()

		self.buyPremiumDungeonTicketBoard_cancelBtn = ui.Button()
		self.buyPremiumDungeonTicketBoard_cancelBtn.SetParent(self.buyPremiumDungeonTicketBoard)
		self.buyPremiumDungeonTicketBoard_cancelBtn.SetPosition(126, 34)
		self.buyPremiumDungeonTicketBoard_cancelBtn.SetUpVisual("elvion/game/guild/guild_raids/popup_btn_norm.png")
		self.buyPremiumDungeonTicketBoard_cancelBtn.SetOverVisual("elvion/game/guild/guild_raids/popup_btn_hover.png")
		self.buyPremiumDungeonTicketBoard_cancelBtn.SetDownVisual("elvion/game/guild/guild_raids/popup_btn_down.png")
		self.buyPremiumDungeonTicketBoard_cancelBtn.SAFE_SetEvent(self.OnClickCancelBuyExtraDungeonStartTicket)
		self.buyPremiumDungeonTicketBoard_cancelBtn.SetText("No", -1)
		self.buyPremiumDungeonTicketBoard_cancelBtn.SetWindowHorizontalAlignCenter()
		self.buyPremiumDungeonTicketBoard_cancelBtn.SetWindowVerticalAlignBottom()
		self.buyPremiumDungeonTicketBoard_cancelBtn.Show()

	def __MakeSkillPage(self):
		page = self.pageWindow["SKILL"]
		page.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
		page.isNeedLoadLogsPage = True
		
		page.logsLinePos = 0
		
		page.nextCanSaveLogTime = 0
	
		page.logsScrollBar = page.GetChild("LogsScrollBar")
		page.logsScrollBar.SetScrollEvent(ui.__mem_func__(self.OnScrollLogsLine))
		
		# Enable smooth scrolling
		if hasattr(page.logsScrollBar, 'EnableSmoothMode'):
			page.logsScrollBar.EnableSmoothMode()
		
		page.GetChild("RefreshLogsButton").SAFE_SetEvent(self.OnClickRefreshLogsBtn)
		page.GetChild("SaveLogsButton").SAFE_SetEvent(self.OnClickSaveLogsBtn)
	
		page.logTxts = {}
		for i in xrange(13):
			logTxt = ui.TextLine("Verdana:13")
			logTxt.SetParent(page)
			logTxt.SetPackedFontColor(0xFFDDDDDD)
			logTxt.SetPosition(26, 69+18*i)
			logTxt.SetOutline()
			logTxt.SetText("No log")
			logTxt.Show()
			page.logTxts[i] = logTxt
	
		bg = page.GetChild("bg")
		bg.OnMouseWheel = page.logsScrollBar.OnMouseWheel
	
		page.OnMouseWheel = page.logsScrollBar.OnMouseWheel

	def __MakeGradePage(self):

		lineStep = 24
		page = self.pageWindow["GRADE"]
		page.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))

		page.gradeDict = {}

		for i in xrange(10):

			yPos = 69 + i*lineStep
			index = i+1
			gradeNumberSlot = ui.TextLine()
			gradeNumberSlot.SetParent(page)
			gradeNumberSlot.SetHorizontalAlignCenter()
			gradeNumberSlot.SetPosition(89, yPos)
			gradeNumberSlot.SetText(str(i+1))
			gradeNumberSlot.SetPackedFontColor(0xFFbfa185)
			gradeNumberSlot.SetOutline()
			gradeNumberSlot.Show()
			page.Children.append(gradeNumberSlot)

			if localeInfo.IsARABIC():
				gradeNameSlot = EditableTextSlot(page, 242, yPos)
			else:
				gradeNameSlot = EditableTextSlot(page, 58+108, yPos)
			
			gradeNameSlot.SetTextColor(0xFFbfa185)
			gradeNameSlot.SetOutline()
			gradeNameSlot.SetHorizontalAlign("center")
			
			gradeNameSlot.SetEvent(ui.__mem_func__(self.OnOpenChangeGradeName), index)
			page.Children.append(gradeNameSlot)

			event = lambda argSelf=proxy(self), argIndex=index, argAuthority=1<<0: apply(argSelf.OnCheckAuthority, (argIndex,argAuthority))
			if localeInfo.IsARABIC():
				inviteAuthorityCheckBox = CheckBox(page, 185, yPos, event, "elvion/game/guild/member_auth/auth_checkbox_bg.png")
			else:
				inviteAuthorityCheckBox = CheckBox(page, 358, yPos, event, "elvion/game/guild/member_auth/auth_checkbox_bg.png")
			page.Children.append(inviteAuthorityCheckBox)

			event = lambda argSelf=proxy(self), argIndex=index, argAuthority=1<<1: apply(argSelf.OnCheckAuthority, (argIndex,argAuthority))
			if localeInfo.IsARABIC():
				driveoutAuthorityCheckBox = CheckBox(page, 128, yPos, event, "elvion/game/guild/member_auth/auth_checkbox_bg.png")
			else:
				driveoutAuthorityCheckBox = CheckBox(page, 406, yPos, event, "elvion/game/guild/member_auth/auth_checkbox_bg.png")
			page.Children.append(driveoutAuthorityCheckBox)

			event = lambda argSelf=proxy(self), argIndex=index, argAuthority=1<<2: apply(argSelf.OnCheckAuthority, (argIndex,argAuthority))
			if localeInfo.IsARABIC():
				noticeAuthorityCheckBox = CheckBox(page, 71, yPos, event, "elvion/game/guild/member_auth/auth_checkbox_bg.png")
			else:
				noticeAuthorityCheckBox = CheckBox(page, 454, yPos, event, "elvion/game/guild/member_auth/auth_checkbox_bg.png")
			page.Children.append(noticeAuthorityCheckBox)

			event = lambda argSelf=proxy(self), argIndex=index, argAuthority=1<<3: apply(argSelf.OnCheckAuthority, (argIndex,argAuthority))
			if localeInfo.IsARABIC():
				skillAuthorityCheckBox = CheckBox(page, 14, yPos, event, "elvion/game/guild/member_auth/auth_checkbox_bg.png")
			else:
				skillAuthorityCheckBox = CheckBox(page, 502, yPos, event, "elvion/game/guild/member_auth/auth_checkbox_bg.png")
			page.Children.append(skillAuthorityCheckBox)

			event = lambda argSelf=proxy(self), argIndex=index, argAuthority=1<<4: apply(argSelf.OnCheckAuthority, (argIndex,argAuthority))
			if localeInfo.IsARABIC():
				guardAuthorityCheckBox = CheckBox(page, 14, yPos, event, "elvion/game/guild/member_auth/auth_checkbox_bg.png")
			else:
				guardAuthorityCheckBox = CheckBox(page, 551, yPos, event, "elvion/game/guild/member_auth/auth_checkbox_bg.png")
			page.Children.append(guardAuthorityCheckBox)

			gradeSlotList = []
			gradeSlotList.append(gradeNameSlot)
			gradeSlotList.append(inviteAuthorityCheckBox)
			gradeSlotList.append(driveoutAuthorityCheckBox)
			gradeSlotList.append(noticeAuthorityCheckBox)
			gradeSlotList.append(skillAuthorityCheckBox)
			gradeSlotList.append(guardAuthorityCheckBox)
			page.gradeDict[index] = gradeSlotList

		masterSlotList = page.gradeDict[1]
		for slot in masterSlotList:
			slot.Disable()

	def CanOpen(self):
		return guild.IsGuildEnable()

	def Open(self):
		self.Show()
		self.SetTop()

		guildID = net.GetGuildID()
		self.largeMarkBox.SetIndex(guildID)
		self.largeMarkBox.SetScale(3)
		if localeInfo.IsARABIC():
			self.largeMarkBox.SetPosition(self.largeMarkBox.GetWidth()+32,1)

	def Close(self):
		self.__CloseAllGuildMemberPageGradeComboBox()
		self.offerDialog.Close()
		self.popupDialog.Hide()
		self.changeGradeNameDialog.Hide()
		self.Hide()

		if self.tooltipSkill:
			self.tooltipSkill.Hide()
			
		#if self.tooltipExpInfo:
		#	self.tooltipExpInfo.Hide()
		
		if hasattr(self, 'expInfoWindow') and self.expInfoWindow:
			self.expInfoWindow.Close()
	
		self.pickDialog = None
		self.questionDialog = None
		self.moneyDialog = None
		self.popup = None

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def Destroy(self):
		self.ClearDictionary()
		#self.board = None
		self.pageName = None
		self.tabDict = None
		self.tabButtonDict = None
		self.pickDialog = None
		self.questionDialog = None
		self.markSelectDialog = None
		self.symbolSelectDialog = None

		if self.offerDialog:
			self.offerDialog.Destroy()
			self.offerDialog = None

		if self.popupDialog:
			self.popupDialog.ClearDictionary()
			self.popupDialog = None

		if self.changeGradeNameDialog:
			self.changeGradeNameDialog.ClearDictionary()
			self.changeGradeNameDialog = None

		self.popupMessage = None

		if self.pageWindow:
			for window in self.pageWindow.values():
				window.ClearDictionary()

		self.pageWindow = None
		self.tooltipSkill = None
		self.moneyDialog = None
	
	def OnScrollDungeonList(self):
		page = self.pageWindow["BASE_INFO"]
		if not page or not hasattr(page, 'scrollBar') or not hasattr(page, 'dungeonItems'):
			return
		
		scrollPos = page.scrollBar.GetPos()
		
		totalItems = len(DUNGEONS)
		itemHeight = 91
		visibleHeight = 242
		totalContentHeight = totalItems * itemHeight
		scrollableHeight = max(0, totalContentHeight - visibleHeight)
		
		offset = int(scrollPos * scrollableHeight)
		
		page.dungeonsListBox.SetPosition(0, -offset)

	def DeleteGuild(self):
		self.RefreshGuildInfoPage()
		self.RefreshGuildBoardPage()
		self.RefreshGuildMemberPage()
		self.RefreshGuildSkillPage()
		self.RefreshGuildGradePage()
		self.pageWindow["BOARD"].isNeedLoadGuardPage = True
		self.pageWindow["SKILL"].isNeedLoadLogsPage = True
		self.Hide()

	def SetSkillToolTip(self, tooltipSkill):
		self.tooltipSkill = tooltipSkill

	def SelectPage(self, arg):

		if arg == "GUILD_INFO" and self.pageWindow["GUILD_INFO"].isNeedLoadStorageInfo:
			self.pageWindow["GUILD_INFO"].isNeedLoadStorageInfo = False
			net.SendGuildLoadGuardItemsDataPacket()

		if arg == "BOARD" and self.pageWindow["BOARD"].isNeedLoadGuardPage:
			self.pageWindow["BOARD"].isNeedLoadGuardPage = False
			net.SendGuildLoadGuardItemsDataPacket()
			
		if arg == "BASE_INFO" and self.pageWindow["BASE_INFO"].isNeedLoadHistory:
			self.pageWindow["BASE_INFO"].isNeedLoadHistory = False
			net.SendGuildLoadDungeonHistoryPacket() 
		
		if arg == "SKILL" and self.pageWindow["SKILL"].isNeedLoadLogsPage:
			self.pageWindow["SKILL"].isNeedLoadLogsPage = False
			net.SendGuildLoadLogsPacket(guild.GetLastLogID())
		
	
		for key, btn in self.tabButtonDict.items():
			if arg != key:
				btn.SetUp()
		for key, img in self.tabDict.items():
			if arg == key:
				img.Show()
			else:
				img.Hide()
		for key, page in self.pageWindow.items():
			if arg == key:
				page.Show()
			else:
				page.Hide()
				
		if not self.tabButtonDict["SKILL"].IsDown():
			self.tabButtonDict["SKILL"].Enable()
		self.__CloseAllGuildMemberPageGradeComboBox()

	def RefreshGuildBoardPage(self):
		if self.isLoaded==0:
			return

		page = self.pageWindow["BOARD"]

		activeSetID = guild.GetActiveSetID()
		if activeSetID != 0:
			if activeSetID != page.currentSetID:
				self.OnClickGuardEqSetBtn(activeSetID, True)
			page.equipmentActivateBtn.Hide()
			page.equipmentDeactivateBtn.Show()
		else:
			page.equipmentActivateBtn.Show()
			page.equipmentDeactivateBtn.Hide()
		
		self.RefreshCanActivateGuardItemsButton()
		if guild.GetNextCanActivateTime() > app.GetGlobalTimeStamp():
			page.isCheckGuardItemButtonActiveTime = True
		else:
			page.isCheckGuardItemButtonActiveTime = False

		self.RefreshGuardItemSlots()

	def __CloseAllGuildMemberPageGradeComboBox(self):

		page = self.pageWindow["MEMBER"]
		for key, slotList in page.memberDict.items():
			slotList[1].CloseListBox()
			
	def __OverInMaterialItem(self, slotIndex):
		if mouseModule.mouseController.isAttached():
			return
			
		if self.tooltipItem:
			materialVnums = [MATERIAL_STONE_ID, MATERIAL_LOG_ID, MATERIAL_PLYWOOD_ID]
			if slotIndex < len(materialVnums):
				self.tooltipItem.ClearToolTip()
				self.tooltipItem.AddItemData(materialVnums[slotIndex], [])
				self.tooltipItem.ShowToolTip()
	
	def __OverOutMaterialItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()
		
	def RefreshGuildInfoPage(self):

		if self.isLoaded==0:
			return

		global DISABLE_DECLARE_WAR
		page = self.pageWindow["GUILD_INFO"]
		page.nameSlot.SetText(guild.GetGuildName())
		page.masterNameSlot.SetText(guild.GetGuildMasterName())
		page.guildLevelSlot.SetText(str(guild.GetGuildLevel()))
		if page.guildMoneySlot:
			page.guildMoneySlot.SetText(str(guild.GetGuildMoney()))

		curExp, lastExp = guild.GetGuildExperience()
		curExp *= 100
		lastExp *= 100
		page.curExpSlot.SetText("%d / %d" % (curExp, lastExp))

		curMemberCount, maxMemberCount = guild.GetGuildMemberCount()
		if maxMemberCount== 0xffff:
			page.memberCountSlot.SetText("%d / %s " % (curMemberCount, localeInfo.GUILD_MEMBER_COUNT_INFINITY))
		else:
			page.memberCountSlot.SetText("%d / %d" % (curMemberCount, maxMemberCount))

		page.levelAverageSlot.SetText(str(guild.GetGuildMemberLevelAverage()))
		
		page.materialSlot.SetItemSlot(0, MATERIAL_STONE_ID, 0)
		page.materialSlot.SetItemSlot(1, MATERIAL_LOG_ID, 0)
		page.materialSlot.SetItemSlot(2, MATERIAL_PLYWOOD_ID, 0)

		try:
			donateStone, donateLog, donatePlywood, dailyStone, dailyLog, dailyPlywood = guild.GetDonateStorage()
		except:
			donateStone, donateLog, donatePlywood, dailyStone, dailyLog, dailyPlywood = 0, 0, 0, 0, 0, 0

		page.materialCount1.SetText(str(donateStone))
		page.materialCount2.SetText(str(donateLog))
		page.materialCount3.SetText(str(donatePlywood))

		try:
			bonusLevels = guild.GetBonusLevels()
			for i in xrange(5):
				if i < len(page.bonusValTexts) and page.bonusValTexts[i]:
					page.bonusValTexts[i].SetText("Lv.%d" % bonusLevels[i])
		except:
			pass

		mainCharacterName = player.GetMainCharacterName()
		masterName = guild.GetGuildMasterName()

		isLeader = (mainCharacterName == masterName)
		if isLeader:
			page.uploadMarkButton.Show()

			if DISABLE_DECLARE_WAR:
				page.declareWarButton.Hide()
			else:
				page.declareWarButton.Show()
		else:
			page.uploadMarkButton.Hide()
			page.declareWarButton.Hide()

		try:
			for btn in page.upgradeBtns:
				if isLeader:
					btn.Show()
				else:
					btn.Hide()
		except:
			pass
		if not self.tabButtonDict["SKILL"].IsDown():
			self.tabButtonDict["SKILL"].Enable()

		if hasattr(self.pageWindow["BASE_INFO"], 'dungeonItems'):
			for item in self.pageWindow["BASE_INFO"].dungeonItems:
				item.RefreshGuildLevel()
		
		freeTicketCount = guild.GetGuildTicketData(guild.TICKET_TYPE_FREE)
		if freeTicketCount == 0:
			if guild.GetGuildTicketData(guild.TICKET_TYPE_PAID) > 0:
				self.pageWindow["BASE_INFO"].dungeonOpenBtn.SetText("Entries 0/3 (+1 premium)")
			elif guild.GetGuildTicketData(guild.TICKET_TYPE_PAID_LIMIT) < guild.TICKET_MAX_PAID_LIMIT:
				self.pageWindow["BASE_INFO"].dungeonOpenBtn.SetText("Entries 0/3 (premium available)")
			else:
				self.pageWindow["BASE_INFO"].dungeonOpenBtn.SetText("Entries 0/3 (tomorrow!)")
		else:
			self.pageWindow["BASE_INFO"].dungeonOpenBtn.SetText("Entries %d/3"%freeTicketCount)

	def RefreshGuildDungeonInfoPage(self):
		if self.isLoaded==0:
			return
	
		page = self.pageWindow["BASE_INFO"]
	
		if self.selectedDungeonIdx != -1:
			self.OnSelectDungeon(self.selectedDungeonIdx, self.selectedDungeonDifficultyIdx)
	
	def SecondToHMS(self, time):
		if time < 60:
			return "%d s."%time
	
		second = int(time % 60)
		minute = int((time / 60) % 60)
		hour = int((time / 60) / 60)
	
		text = ""
	
		if hour > 0:
			text = "%d h. "%hour
	
		if minute > 0:
			text += "%d m. "%minute
	
		if second > 0:
			text += "%d s."%second
	
		return text
	
	def OnSelectDungeon(self, dungIdx, diffIdx):
		page = self.pageWindow["BASE_INFO"]
	
		if self.selectedDungeonIdx != -1:
			page.dungeonItems[self.selectedDungeonIdx].UnselectButton(self.selectedDungeonDifficultyIdx)
		
		page.dungeonItems[dungIdx].SelectButton(diffIdx)
	
		self.selectedDungeonIdx = dungIdx
		self.selectedDungeonDifficultyIdx = diffIdx
		dungData = DUNGEONS[dungIdx][1][diffIdx]
		items = dungData[7]
		itemsLen = len(items)
		for i in xrange(6*3):
			if i < itemsLen and items[i][0] != 0:
				page.possibleDropItemsGrid.SetItemSlot(i, items[i][0], 0 if items[i][1] <= 1 else items[i][1])
			else:
				page.possibleDropItemsGrid.ClearSlot(i)
		page.possibleDropItemsGrid.RefreshSlot()
	
		page.dungBg.LoadImage(BASE_PATH + "guild_raids/raid_img/%s"%dungData[1])
		page.prefMemberCountTxt.SetText("%d"%dungData[2])
		page.prefMonsterDefBonusTxt.SetText("%d%%"%dungData[3][1])
		page.prefLordAttBonusTxt.SetText("%d%%"%dungData[3][2])
		doneCount, bestTime = guild.GetDungeonHistoryInfo(dungData[0])
		page.canGetYangCountTxt.SetText(NumberToMoneyString(dungData[5]) + " G")
		
		if dungIdx == self.openedDungeonIdx and diffIdx == self.openedDungeonDifficultyIdx:
			page.dungeonTeleportBtn.Enable()
		else:
			page.dungeonTeleportBtn.Disable()
	
	def OverInDungeonPossibleDropItem(self, slotIdx):
		self.itemTooltip.SetItemToolTip(DUNGEONS[self.selectedDungeonIdx][1][self.selectedDungeonDifficultyIdx][7][slotIdx][0])
		if DUNGEONS[self.selectedDungeonIdx][1][self.selectedDungeonDifficultyIdx][7][slotIdx][2] != "":
			self.itemTooltip.AppendDescription(DUNGEONS[self.selectedDungeonIdx][1][self.selectedDungeonDifficultyIdx][7][slotIdx][2], 69)
	
	def OverOutDungeonPossibleDropItem(self):
		self.itemTooltip.HideToolTip()
	
	def OnClickDungeonOpenButton(self):
		freeTicketCount = guild.GetGuildTicketData(guild.TICKET_TYPE_FREE)
		paidTicketCount = guild.GetGuildTicketData(guild.TICKET_TYPE_PAID)
		paidTicketLimitCount = guild.GetGuildTicketData(guild.TICKET_TYPE_PAID_LIMIT)
		if freeTicketCount > 0 or paidTicketCount > 0:
			self.startDungeonBoard_txt1.SetText("You are opening the raid |cFFccb59c%s|r (|cFFe0d38a%s|r)."%(DUNGEONS[self.selectedDungeonIdx][0], DUNGEON_TYPES[self.selectedDungeonDifficultyIdx]))

			self.startDungeonBoard_txt2.SetText("You and your guild will have |cFFe0d38a%s|r to finish it."%DUNGEONS[self.selectedDungeonIdx][1][self.selectedDungeonDifficultyIdx][4])

			self.startDungeonBoard_txt3.SetText("Remaining entries today: |cFFe0d38a%d|r free (+|cFFe0d38a%d|r premium)"%(freeTicketCount, 3-paidTicketLimitCount+paidTicketCount))
			
			if freeTicketCount > 0:
				self.startDungeonBoard_txt4.SetText("After opening raid |cFFe0d38a1|r free entry will be consumed.")
			else:
				self.startDungeonBoard_txt4.SetText("After opening raid |cFFe0d38a1|r premium entry will be consumed.")
		
			self.startDungeonBoard.Show()
			self.startDungeonBoard.SetTop()
			self.startDungeonBoard.Lock()
			return
		
		if paidTicketLimitCount >= 3:
			net.SendChatPacket("/guild_dungeon 4")
			return
		
		self.buyPremiumDungeonTicketBoard_txt4.SetText("|cFFe0d38a%d|r"%(3-paidTicketLimitCount))
		self.buyPremiumDungeonTicketBoard.Show()
		self.buyPremiumDungeonTicketBoard.SetTop()
		self.buyPremiumDungeonTicketBoard.Lock()
	
	def OnClickDungeonTeleportButton(self):
		net.SendChatPacket("/guild_dungeon 5 %d"%DUNGEONS[self.selectedDungeonIdx][1][self.selectedDungeonDifficultyIdx][0])
	
	def OnClickCancelStartDungeon(self):
		self.startDungeonBoard.Hide()
		self.startDungeonBoard.Unlock()
	
	def OnClickStartDungeon(self):
		self.startDungeonBoard.Hide()
		self.startDungeonBoard.Unlock()
		#chat.AppendChat(1, "Raids will be enabled once more players advance to higher levels.")
		net.SendChatPacket("/guild_dungeon 1 %d"%DUNGEONS[self.selectedDungeonIdx][1][self.selectedDungeonDifficultyIdx][0])
	
	def OnRecv_OpenedDungeonIdx(self, openedDungIdx, isShowInfo):
		if openedDungIdx == 0:
			self.openedDungeonIdx = -1
			self.openedDungeonDifficultyIdx = -1
			if self.selectedDungeonIdx != -1:
				self.OnSelectDungeon(self.selectedDungeonIdx, self.selectedDungeonDifficultyIdx)
			return
		dungIdx = 0
		dungDifficultyIdx = -1
		for data in DUNGEONS:
			for difficulty in xrange(3):
				if data[1][difficulty][0] == openedDungIdx:
					dungDifficultyIdx = difficulty
					break
			if dungDifficultyIdx != -1:
				break
			dungIdx += 1
		
		if dungDifficultyIdx == -1:
			return
		
		self.openedDungeonIdx = dungIdx
		self.openedDungeonDifficultyIdx = dungDifficultyIdx
		
		if self.isLoaded:
			if self.selectedDungeonIdx == dungIdx and self.selectedDungeonDifficultyIdx == dungDifficultyIdx:
				self.OnSelectDungeon(dungIdx, dungDifficultyIdx)

		if isShowInfo:
			chat.AppendChat(1, "|cFFFDEDBFGuild Raid |cFFccb59c%s|r ( %s|r) |cFFFDEDBFis now open!" % (DUNGEONS[dungIdx][0], DUNGEON_TYPES[dungDifficultyIdx]))
			chat.AppendChat(1, "|cFFFDEDBFGo to the guild panel and help in the fight!")
			chat.AppendChat(1, "|cFFFDEDBFAfter 2 minutes from the moment the first player joins the raid, no new players will be able to join.")
			chat.AppendChat(1, "|cFFFDEDBFThe raid will close after 1 hour, so to get the maximum time to complete it, you must join within 43 minutes.")
	
	def OnClickCancelBuyExtraDungeonStartTicket(self):
		self.buyPremiumDungeonTicketBoard.Hide()
		self.startDungeonBoard.Unlock()
	
	def OnClickBuyExtraDungeonStartTicket(self, buyType):
		net.SendChatPacket("/guild_dungeon %d"%(2+buyType))
		self.buyPremiumDungeonTicketBoard.Hide()
		self.startDungeonBoard.Unlock()
		
	def RefreshLogs(self, fromScrolling=False):
		page = self.pageWindow["SKILL"]
		lineIndex = 0
	
		logsCount = guild.GetLogsCount()
		
		totalContentHeight = logsCount * 18
		page.logsScrollBar.SetContentHeight(totalContentHeight)
		
		if logsCount <= LOGS_MAX_COUNT:
			page.logsScrollBar.Hide()
			page.logsLinePos = 0
		else:
			page.logsScrollBar.Show()
			
		logsCount = min(LOGS_MAX_COUNT, logsCount)
		
		for i in xrange(logsCount):
			logTime, logID, str1, uint1, str2, uint2 = guild.GetLogData(i+page.logsLinePos)
			if logTime == 0:
				continue
	
			page.logTxts[lineIndex].SetText("%s: %s"%(datetime.fromtimestamp(logTime).strftime('%d.%m.%Y %H:%M'), self.ParseLogData(logID, str1, uint1, str2, uint2)))
	
			lineIndex += 1
	
		for i in xrange(LOGS_MAX_COUNT - lineIndex):
			page.logTxts[lineIndex+i].SetText("")
		
		if not fromScrolling:
			if page.logsScrollBar.IsShow():
				page.logsScrollBar.SetPos(1.0)

	def ParseLogData(self, logID, str1, uint1, str2, uint2):
		parsedText = ""
		if logID == 1:
			parsedText = "|cFFccb59c%s|r Not used (reset skill points of guardian)" % str1
		elif logID == 2:
			item.SelectItem(uint2)
			parsedText = "|cFFccb59c%s|r unequipped |cFFe0d38a%s|r from the guild guardian" % (str1, item.GetItemName())
		elif logID == 3:
			item.SelectItem(uint2)
			parsedText = "|cFFccb59c%s|r equipped |cFFe0d38a%s|r on the guild guardian" % (str1, item.GetItemName())
		elif logID == 4:
			parsedText = "|cFFccb59c%s|r deactivated guardian item bonuses" % str1
		elif logID == 5:
			parsedText = "|cFFccb59c%s|r activated guardian item bonuses" % str1
		elif logID == 6:
			parsedText = "|cFFccb59c%s|r challenged the guild |cFFe0d38a%s|r to a guild war" % (str1, str2)
		elif logID == 7:
			parsedText = "Guild war with guild |cFFe0d38a%s|r has started" % str1
		elif logID == 8:
			parsedText = "Guild |cFFe0d38a%s|r rejected the guild War" % str1
		elif logID == 9:
			parsedText = "%s wants to end the war with guild %s" % (str1, str2)
		elif logID == 10:
			parsedText = "%s has resumed the war with guild %s" % (str1, str2)
		elif logID == 11:
			parsedText = "%s formed an alliance with guild %s" % (str1, str2)
		elif logID == 12:
			parsedText = "%s broke the alliance with guild %s" % (str1, str2)
		elif logID == 13:
			parsedText = "Guild %s broke the alliance with you" % str1
		elif logID == 14:
			parsedText = "Guild %s proposed an alliance to you" % str1
		elif logID == 15:
			if uint2 == 1:
				parsedText = "|cFFccb59c%s|r purchased a paid raid entry using a coupon" % str1
			else:
				parsedText = "|cFFccb59c%s|r purchased a paid raid entry using Dragon Coins" % str1
		elif logID == 16:
			dungName = ""
			for data in DUNGEONS:
				for difficulty in xrange(3):
					if data[1][difficulty][0] == uint1:
						dungName = data[0]
						break
				if dungName != "":
					break
	
			parsedText = "|cFFccb59c%s|r opened an entry to the raid |cFFe0d38a%s|r" % (str1, dungName)
		elif logID == 17:
			parsedText = "|cFFccb59c%s|r was promoted from Recruit to Member" % str1
		elif logID == 18:
			parsedText = "|cFFccb59c%s|r joined the guild" % str1
		elif logID == 19:
			parsedText = "|cFFccb59c%s|r left the guild" % str1
		elif logID == 20:
			parsedText = "|cFFe0d38a%s|r was kicked by |cFFccb59c%s|r" % (str1, str2)
		elif logID == 21:
			parsedText = "|cFFccb59c%s|r was promoted to Vice Leader" % str1
		elif logID == 22:
			parsedText = "Guild level increased to |cFFe0d38a%s|r" % uint1
		elif logID == 23:
			parsedText = "Guardian level increased to |cFFe0d38a%s|r" % uint1
		elif logID == 24:
			parsedText = "|cFFccb59c%s|r became the new Leader" % str1
		elif logID == 25:
			parsedText = "|cFFccb59c%s|r edited the note" % str1
		elif logID == 26:
			parsedText = "|cFFccb59c%s|r edited the guild logo" % str1
		else:
			parsedText = "Unknown log %d" % logID
		return parsedText

	def OnScrollLogsLine(self):
		page = self.pageWindow["SKILL"]
		
		scrollBar = page.logsScrollBar
		pos = scrollBar.GetPos()
	
		count = guild.GetLogsCount()
		newLinePos = int(float(count - LOGS_MAX_COUNT) * pos)
	
		if newLinePos != page.logsLinePos:
			page.logsLinePos = newLinePos
			self.RefreshLogs(True)

	def OnClickRefreshLogsBtn(self):
		if app.GetGlobalTimeStamp() < guild.GetNextCanRefreshLogTime():
			chat.AppendChat(1, "|cFFFDEDBFYou can refresh logs every 5 minutes! Remaining: |cFFe0d38a%s|r"%self.SecondToHMS(guild.GetNextCanRefreshLogTime()-app.GetGlobalTimeStamp()))
			return
		net.SendGuildLoadLogsPacket(guild.GetLastLogID())

	def OnClickSaveLogsBtn(self):
		page = self.pageWindow["SKILL"]
		if page.nextCanSaveLogTime > app.GetGlobalTimeStamp():
			chat.AppendChat(1, "|cFFFDEDBFYou can refresh logs every 5 minutes! Remaining: |cFFe0d38a%s|r"%self.SecondToHMS(page.nextCanSaveLogTime-app.GetGlobalTimeStamp()))
			return
		page.nextCanSaveLogTime = app.GetGlobalTimeStamp() + 60*5

		logsCount = guild.GetLogsCount()
		if logsCount <= 0:
			chat.AppendChat(1, "|cFFFDEDBFGuild logs are empty!")
			return
		
		if not os.path.exists('content/userdata/guild_logs'):
			os.makedirs('content/userdata/guild_logs')

		fileName = "logs_saved_%s.txt"%(datetime.fromtimestamp(app.GetGlobalTimeStamp()).strftime('%d_%m_%Y_%H_%M_%S'))
		with open("content/userdata/guild_logs/%s"%fileName, 'w') as file:
			for i in xrange(logsCount):
				logTime, logID, str1, uint1, str2, uint2 = guild.GetLogData(i)
				if logTime == 0:
					continue

				file.write("%s: %s\n"%(datetime.fromtimestamp(logTime).strftime('%H:%M:%S %d.%m.%Y'), self.ParseLogData(logID, str1, uint1, str2, uint2)))
			chat.AppendChat(1, "|cFFFDEDBFSaved guild logs to client folder - |cFFccb59c%s|r"%fileName)

	def RefreshGuildMemberPage(self):

		if self.isLoaded==0:
			return

		self.RefreshGuildMemberPageMemberList()

	def RefreshGuildMemberPageMemberList(self):
		if self.isLoaded == 0:
			return
	
		page = self.pageWindow["MEMBER"]
	
		mw = page.memberWindow

		for line in page.memberDict.keys():
			slotList = page.memberDict[line]

			if line in page.memberBgDict:
				page.memberBgDict[line].Hide()
				mw.Children.remove(page.memberBgDict[line])

			slotList[0].Hide()
			mw.Children.remove(slotList[0])
			if hasattr(slotList[0], 'GetParent'):
				parent = slotList[0].GetParent()
				if parent:
					parent.Hide()
					mw.Children.remove(parent)

			slotList[1].Hide()
			mw.Children.remove(slotList[1])

			for i in [2, 3, 4]:
				slotList[i].Hide()
				mw.Children.remove(slotList[i])
				if hasattr(slotList[i], 'GetParent'):
					parent = slotList[i].GetParent()
					if parent:
						parent.Hide()
						mw.Children.remove(parent)

			slotList[5].Hide()
			mw.Children.remove(slotList[5])

			for i in [6, 7]:
				if len(slotList) > i:
					slotList[i].Hide()
					if slotList[i] in mw.Children:
						mw.Children.remove(slotList[i])

		page.memberDict.clear()
		page.memberBgDict.clear()

		memberCount = guild.GetMemberCount()
		lineStep = 24
		yStartPos = 0

		page.memberWindow.SetPosition(0, 0)

		mw = page.memberWindow

		for line in xrange(memberCount):
			if not guild.IsMember(line):
				continue

			pid, name, grade, race, level, offer, general = self.GetMemberData(line)
			if pid < 0:
				continue

			#isOnline, lastSeen = 0

			yPos = yStartPos + line * lineStep

			memberBg = ui.ImageBox()
			memberBg.SetParent(mw)
			memberBg.SetPosition(13, yPos)
			memberBg.LoadImage("elvion/game/guild/member/member_bg.png")
			memberBg.SetClippingMaskWindow(page.memberClipWindow)
			memberBg.Show()
			mw.Children.append(memberBg)
			page.memberBgDict[line] = memberBg

			#statusIcon = ui.ImageBox()
			#statusIcon.SetParent(memberBg)
			#statusIcon.SetPosition(9, 8)
			##if isOnline:
			##	statusIcon.LoadImage("elvion/game/guild/online.png")
			##else:
			##	statusIcon.LoadImage("elvion/game/guild/offline.png")
			#statusIcon.Show()
			#mw.Children.append(statusIcon)

			nameSlot = ui.TextLine()
			nameSlot.SetParent(memberBg)
			nameSlot.SetHorizontalAlignCenter()
			nameSlot.SetPosition(85, 5)
			nameSlot.SetPackedFontColor(0xFFbfa185)
			nameSlot.SetOutline()
			nameSlot.SetText(name)
			nameSlot.Show()
			mw.Children.append(nameSlot)

			#if isOnline:
			#	lastSeenText = localeInfo.guild_member_online if hasattr(localeInfo, "guild_member_online") else "Online"
			#elif lastSeen == 0:
			#	lastSeenText = "?"
			#else:
			#	import time
			#	diff = int(time.time()) - lastSeen
			#	if diff < 60:
			#		lastSeenText = "Just now"
			#	elif diff < 3600:
			#		lastSeenText = "%dm ago" % (diff / 60)
			#	elif diff < 86400:
			#		lastSeenText = "%dh ago" % (diff / 3600)
			#	else:
			#		lastSeenText = "%dd ago" % (diff / 86400)
			#
			#lastSeenSlot = ui.TextLine()
			#lastSeenSlot.SetParent(memberBg)
			#lastSeenSlot.SetPosition(23, 5)
			#lastSeenSlot.SetPackedFontColor(0xFF888888 if not isOnline else 0xFF88cc88)
			#lastSeenSlot.SetText(lastSeenText)
			#lastSeenSlot.Show()
			#mw.Children.append(lastSeenSlot)

			gradeSlot = ui.ComboBoxImage(mw, "elvion/game/guild/member/member_grade_slot.png", 164, yPos + 4, dropdownParent=page)
			gradeSlot.SetSize(139, 17)
			gradeSlot.SetTextColor(0xFFbfa185)
			gradeSlot.SetOutline()
			gradeSlot.SetHorizontalAlign("center")
			gradeSlot.SetEvent(lambda gradeNumber, lineIndex=line, argSelf=proxy(self): argSelf.OnChangeMemberGrade(lineIndex, gradeNumber))
			gradeSlot.ClearItem()
			CAN_CHANGE_GRADE_COUNT = 10 - 1
			for i in xrange(CAN_CHANGE_GRADE_COUNT):
				gradeSlot.InsertItem(i + 2, guild.GetGradeName(i + 2))
			gradeSlot.SetCurrentItem(guild.GetGradeName(grade))
			if 1 != grade:
				gradeSlot.Enable()
			else:
				gradeSlot.Disable()
			mw.Children.append(gradeSlot)
			gradeSlot.Show()

			job = chr.RaceToJob(race)
			jobSlot = ui.TextLine()
			jobSlot.SetParent(memberBg)
			jobSlot.SetHorizontalAlignCenter()
			jobSlot.SetPosition(364, 5)
			jobSlot.SetPackedFontColor(0xFFbfa185)
			jobSlot.SetOutline()
			jobSlot.SetText(self.JOB_NAME.get(job, "?"))
			jobSlot.Show()
			mw.Children.append(jobSlot)

			levelSlot = ui.TextLine()
			levelSlot.SetParent(memberBg)
			levelSlot.SetHorizontalAlignCenter()
			levelSlot.SetPosition(459, 5)
			levelSlot.SetPackedFontColor(0xFFbfa185)
			levelSlot.SetOutline()
			levelSlot.SetText(str(level))
			levelSlot.Show()
			mw.Children.append(levelSlot)

			guildExperienceSummary = guild.GetGuildExperienceSummary()
			offerPercentage = 0
			if guildExperienceSummary > 0:
				offerPercentage = int(float(offer) / float(guildExperienceSummary) * 100.0)

			offerSlot = ui.TextLine()
			offerSlot.SetParent(memberBg)
			offerSlot.SetHorizontalAlignCenter()
			offerSlot.SetPosition(507, 5)
			offerSlot.SetPackedFontColor(0xFFbfa185)
			offerSlot.SetOutline()
			offerSlot.SetText(str(offerPercentage) + "%")
			offerSlot.Show()
			mw.Children.append(offerSlot)

			event = lambda argSelf=proxy(self), argIndex=line: apply(argSelf.OnEnableGeneral, (argIndex,))
			if localeInfo.IsJAPAN():
				generalEnableCheckBox = CheckBox(mw, 307, yPos, event, "d:/ymir work/ui/public/Parameter_Slot_00.sub")
			elif localeInfo.IsARABIC():
				generalEnableCheckBox = CheckBox(mw, 22, yPos, event, "d:/ymir work/ui/public/Parameter_Slot_00.sub")
			else:
				generalEnableCheckBox = CheckBox(mw, 547, yPos + 4, event, "elvion/game/guild/member/checkbox_bg.png")
			generalEnableCheckBox.SetCheck(general)
			mw.Children.append(generalEnableCheckBox)

			memberSlotList = []
			memberSlotList.append(nameSlot)
			memberSlotList.append(gradeSlot)
			memberSlotList.append(jobSlot)
			memberSlotList.append(levelSlot)
			memberSlotList.append(offerSlot)
			memberSlotList.append(generalEnableCheckBox)
			#memberSlotList.append(statusIcon)
			#memberSlotList.append(lastSeenSlot)
			page.memberDict[line] = memberSlotList

		page.memberLineCount = memberCount

		totalHeight = max(memberCount * lineStep, 238)
		page.memberWindow.SetSize(585, totalHeight)
		page.scrollBar.SetPos(0.0)
		page.scrollBar.SetContentHeight(totalHeight)

	def RefreshGuildMemberPageGradeComboBox(self):

		if self.isLoaded==0:
			return

		page = self.pageWindow["MEMBER"]

		self.CAN_CHANGE_GRADE_COUNT = 15 - 1
		for key, slotList in page.memberDict.items():

			gradeComboBox = slotList[1]
			gradeComboBox.Disable()

			if not guild.IsMember(key):
				continue

			pid, name, grade, job, level, offer, general = self.GetMemberData(key)
			if pid < 0:
				continue

			gradeComboBox.ClearItem()
			for i in xrange(self.CAN_CHANGE_GRADE_COUNT):
				gradeComboBox.InsertItem(i+2, guild.GetGradeName(i+2))
			gradeComboBox.SetCurrentItem(guild.GetGradeName(grade))
			if 1 != grade:
				gradeComboBox.Enable()

	def RefreshGuildSkillPage(self):
		if self.isLoaded==0:
			return
		#if self.isLoaded==0:
		#	return
		#
		#page = self.pageWindow["SKILL"]
		#
		#curPoint, maxPoint = guild.GetDragonPowerPoint()
		#maxPoint = max(maxPoint, 1)
		##page.gpValue.SetText(str(curPoint) + " / " + str(maxPoint))
		#
		#percentage = (float(curPoint) / float(maxPoint) * 100) * (float(173) / float(95))
		#page.gpGauge.SetPercentage(int(percentage), 100)
		#
		#skillPoint = guild.GetGuildSkillPoint()
		#page.skillPoint.SetText(str(skillPoint))
		#
		#page.passiveSlot.HideAllSlotButton()
		#page.activeSlot.HideAllSlotButton()
		#
		#"""
		#for i in xrange(len(playerSettingModule.PASSIVE_GUILD_SKILL_INDEX_LIST)):
		#
		#	slotIndex = page.passiveSlot.GetStartIndex()+i
		#	skillIndex = playerSettingModule.PASSIVE_GUILD_SKILL_INDEX_LIST[i]
		#	skillLevel = guild.GetSkillLevel(slotIndex)
		#	skillMaxLevel = skill.GetSkillMaxLevel(skillIndex)
		#
		#	page.passiveSlot.SetSlotCount(slotIndex, skillLevel)
		#	if skillPoint > 0:
		#		if skillLevel < skillMaxLevel:
		#			page.passiveSlot.ShowSlotButton(slotIndex)
		#"""
		#
		#for i in xrange(len(playerSettingModule.ACTIVE_GUILD_SKILL_INDEX_LIST)):
		#
		#	slotIndex = page.activeSlot.GetStartIndex()+i
		#	skillIndex = playerSettingModule.ACTIVE_GUILD_SKILL_INDEX_LIST[i]
		#	skillLevel = guild.GetSkillLevel(slotIndex)
		#	skillMaxLevel = skill.GetSkillMaxLevel(skillIndex)
		#
		#	page.activeSlot.SetSlotCount(slotIndex, skillLevel)
		#
		#	if skillLevel <= 0:
		#		page.activeSlot.DisableCoverButton(slotIndex)
		#	else:
		#		page.activeSlot.EnableCoverButton(slotIndex)
		#
		#	if skillPoint > 0:
		#		if skillLevel < skillMaxLevel:
		#			page.activeSlot.ShowSlotButton(slotIndex)

	def RefreshGuildGradePage(self):

		if self.isLoaded==0:
			return

		page = self.pageWindow["GRADE"]

		for key, slotList in page.gradeDict.items():
			name, authority = guild.GetGradeData(int(key))

			slotList[self.GRADE_SLOT_NAME].SetText(name)
			slotList[self.GRADE_ADD_MEMBER_AUTHORITY].SetCheck(authority & guild.AUTH_ADD_MEMBER)
			slotList[self.GRADE_REMOVE_MEMBER_AUTHORITY].SetCheck(authority & guild.AUTH_REMOVE_MEMBER)
			slotList[self.GRADE_NOTICE_AUTHORITY].SetCheck(authority & guild.AUTH_NOTICE)
			slotList[self.GRADE_SKILL_AUTHORITY].SetCheck(authority & guild.AUTH_SKILL)
			slotList[self.GRADE_MANAGE_GUARD].SetCheck(authority & guild.AUTH_MANAGE_GUARD)


	def __PopupMessage(self, msg):
		self.popupMessage.SetText(msg)
		self.popupDialog.SetTop()
		self.popupDialog.Show()

	def __OnClickSelectGuildMarkButton(self):
		if guild.GetGuildLevel() < int(localeInfo.GUILD_MARK_MIN_LEVEL):
			self.__PopupMessage(localeInfo.GUILD_MARK_NOT_ENOUGH_LEVEL)
		elif not guild.MainPlayerHasAuthority(guild.AUTH_NOTICE):
			self.__PopupMessage(localeInfo.GUILD_NO_NOTICE_PERMISSION)
		else:
			self.markSelectDialog.Open()

	def __OnClickSelectGuildSymbolButton(self):
		if guild.MainPlayerHasAuthority(guild.AUTH_NOTICE):
			self.symbolSelectDialog.Open()
		else:
			self.__PopupMessage(localeInfo.GUILD_NO_NOTICE_PERMISSION)

	def __OnClickDeclareWarButton(self):
		inputDialog = DeclareGuildWarDialog()
		inputDialog.Open()
		self.inputDialog = inputDialog

	def __OnSelectMark(self, markFileName):
		ret = net.UploadMark("content/upload/"+markFileName)

		if net.ERROR_MARK_UPLOAD_NEED_RECONNECT == ret:
			self.__PopupMessage(localeInfo.UPLOAD_MARK_UPLOAD_NEED_RECONNECT);

		return ret

	def __OnSelectSymbol(self, symbolFileName):
		net.UploadSymbol("content/upload/"+symbolFileName)

	#def __OnClickOfferButton(self):
	#
	#	curEXP = unsigned32(player.GetStatus(player.EXP))
	#
	#	if curEXP <= 100:
	#		self.__PopupMessage(localeInfo.GUILD_SHORT_EXP);
	#		return
	#
	#	self.offerDialog.Open(curEXP, 100)

	def __OnClickExpInfoButton(self):
		if not hasattr(self, 'expInfoWindow'):
			self.expInfoWindow = GuildExpInfoWindow()
		
		if self.expInfoWindow.IsShow():
			self.expInfoWindow.Close()
		else:
			self.expInfoWindow.Open()
			page = self.pageWindow.get("GUILD_INFO")
			if page and hasattr(page, 'expHoverInfo'):
				(x, y) = page.expHoverInfo.GetGlobalPosition()
				self.expInfoWindow.SetPosition(x - 171, y + 30)

	def __CreateBonusSkillToolTip(self, idx):
		if idx < 0 or idx >= len(GUILD_SKILL_DATA):
			return None
		skillName, unitSuffix, levelValues = GUILD_SKILL_DATA[idx]
		toolTip = uiToolTip.ToolTip(260)
		toolTip.SetTitle(skillName)
		toolTip.AppendSpace(5)
		toolTip.AutoAppendTextLine("Upgrade cost:", uiToolTip.ToolTip.TITLE_COLOR)
		toolTip.AutoAppendTextLine("  %d Mystic Stone" % GUILD_BONUS_UPGRADE_COST)
		toolTip.AutoAppendTextLine("  %d Tree Log" % GUILD_BONUS_UPGRADE_COST)
		toolTip.AutoAppendTextLine("  %d Plywood" % GUILD_BONUS_UPGRADE_COST)
		toolTip.AppendSpace(5)
		toolTip.AutoAppendTextLine("Bonus per level:", uiToolTip.ToolTip.TITLE_COLOR)
		maxLevel = len(levelValues) - 1
		for lvl in xrange(1, maxLevel + 1):
			val = levelValues[lvl] if lvl < len(levelValues) else levelValues[-1]
			toolTip.AutoAppendTextLine("  Lv.%d: +%s%s" % (lvl, val, unitSuffix))
		toolTip.AlignHorizonalCenter()
		return toolTip

	def __OnClickUpgradeBonusSkill(self, idx):
		net.SendChatPacket("/guild_upgrade_bonus %d" % idx)

	def __OnClickDonateButton(self, materialIndex):
		materialVnums = [MATERIAL_STONE_ID, MATERIAL_LOG_ID, MATERIAL_PLYWOOD_ID]
		materialNames = ["Mystic Stone", "Tree Log", "Plywood"]
		
		if materialIndex >= len(materialVnums):
			return
			
		itemVnum = materialVnums[materialIndex]
		itemName = materialNames[materialIndex]
		itemCount = player.GetItemCountByVnum(itemVnum)
		
		if itemCount <= 0:
			self.__PopupMessage(localeInfo.GUILD_NO_MATERIAL_TO_DONATE)
			return
		
		self.__OpenMaterialDonationDialog(itemVnum, itemName, itemCount, materialIndex)

	def __OpenMaterialDonationDialog(self, itemVnum, itemName, itemCount, materialIndex):
		import uiCommon

		donationDialog = GuildMaterialDonationDialog()
		donationDialog.Open(itemVnum, itemName, itemCount, materialIndex)
		donationDialog.SetAcceptEvent(ui.__mem_func__(self.__OnAcceptMaterialDonation))
		
		self.materialDonationDialog = donationDialog
	
	def __OnAcceptMaterialDonation(self):
		if not hasattr(self, 'materialDonationDialog') or not self.materialDonationDialog:
			return
			
		itemVnum = self.materialDonationDialog.GetItemVnum()
		donateCount = self.materialDonationDialog.GetDonateCount()
		
		if donateCount <= 0:
			self.__PopupMessage(localeInfo.GUILD_INVALID_DONATE_COUNT)
			self.__OnCancelMaterialDonation()
			return
		
		net.SendChatPacket("/guild_donate_material %d %d" % (itemVnum, donateCount))
		
		self.__OnCancelMaterialDonation()
		
		self.RefreshGuildInfoPage()
	
	def __OnCancelMaterialDonation(self):
		if hasattr(self, 'materialDonationDialog') and self.materialDonationDialog:
			self.materialDonationDialog.Close()
			self.materialDonationDialog = None

	def __OnClickDepositButton(self):
		moneyDialog = uiPickMoney.PickMoneyDialog()
		moneyDialog.LoadDialog()
		moneyDialog.SetMax(6)
		moneyDialog.SetTitleName(localeInfo.GUILD_DEPOSIT)
		moneyDialog.SetAcceptEvent(ui.__mem_func__(self.OnDeposit))
		moneyDialog.Open(player.GetMoney())
		self.moneyDialog = moneyDialog

	def __OnClickWithdrawButton(self):
		moneyDialog = uiPickMoney.PickMoneyDialog()
		moneyDialog.LoadDialog()
		moneyDialog.SetMax(6)
		moneyDialog.SetTitleName(localeInfo.GUILD_WITHDRAW)
		moneyDialog.SetAcceptEvent(ui.__mem_func__(self.OnWithdraw))
		moneyDialog.Open(guild.GetGuildMoney())
		self.moneyDialog = moneyDialog

	def __OnBlock(self):
		popup = uiCommon.PopupDialog()
		popup.SetText(localeInfo.NOT_YET_SUPPORT)
		popup.SetAcceptEvent(self.__OnClosePopupDialog)
		popup.Open()
		self.popup = popup

	def __OnClosePopupDialog(self):
		self.popup = None

	def OnDeposit(self, money):
		net.SendGuildDepositMoneyPacket(money)

	def OnWithdraw(self, money):
		net.SendGuildWithdrawMoneyPacket(money)

	def OnOffer(self, exp):
		net.SendGuildOfferPacket(exp)

	#def OnPostComment(self):
	#
	#	text = self.commentSlot.GetText()
	#	if not text:
	#		return False
	#
	#	net.SendGuildPostCommentPacket(text[:50])
	#	self.commentSlot.SetText("")
	#	return True

	#def OnDeleteComment(self, index):
	#
	#	commentID, chrName, comment = self.__GetGuildBoardCommentData(index)
	#	net.SendGuildDeleteCommentPacket(commentID)

	#def OnRefreshComments(self):
	#	net.SendGuildRefreshCommentsPacket(0)

	#def OnKeyDownInBoardPage(self, key):
	#	if key == 63:
	#		self.OnRefreshComments()
	#	return True

	def OnChangeMemberGrade(self, lineIndex, gradeNumber):
		PID = guild.MemberIndexToPID(lineIndex + self.memberLinePos)
		net.SendGuildChangeMemberGradePacket(PID, gradeNumber)

	def OnEnableGeneral(self, lineIndex):
		if not guild.IsMember(lineIndex):
			return

		pid, name, grade, job, level, offer, general = self.GetMemberData(lineIndex)
		if pid < 0:
			return

		net.SendGuildChangeMemberGeneralPacket(pid, 1 - general)

	def OnOpenChangeGradeName(self, arg):
		self.changeGradeNameDialog.SetGradeNumber(arg)
		self.changeGradeNameDialog.Open()

	def OnChangeGradeName(self):
		self.changeGradeNameDialog.Hide()
		gradeNumber = self.changeGradeNameDialog.GetGradeNumber()
		gradeName = self.changeGradeNameDialog.GetGradeName()

		if len(gradeName) == 0:
			gradeName = localeInfo.GUILD_DEFAULT_GRADE

		net.SendGuildChangeGradeNamePacket(gradeNumber, gradeName)
		return True

	def OnCheckAuthority(self, argIndex, argAuthority):
		name, authority = guild.GetGradeData(argIndex)
		net.SendGuildChangeGradeAuthorityPacket(argIndex, authority ^ argAuthority)

	def OnScrollMemberLine(self):
		self.RefreshGuildMemberPageMemberList()

	def GetMemberData(self, localPos):
		return guild.GetMemberData(localPos + self.memberLinePos)

	def __OnOpenHealGSPBoard(self):

		curPoint, maxPoint = guild.GetDragonPowerPoint()

		if maxPoint - curPoint <= 0:
			self.__PopupMessage(localeInfo.GUILD_CANNOT_HEAL_GSP_ANYMORE)
			return

		if app.ENABLE_CHEQUE_SYSTEM:
			pickDialog = uiPickETC.PickETCDialog()
		else:
			pickDialog = uiPickMoney.PickMoneyDialog()
			
		pickDialog.LoadDialog()
		pickDialog.SetMax(9)
		pickDialog.SetTitleName(localeInfo.GUILD_HEAL_GSP)
		pickDialog.SetAcceptEvent(ui.__mem_func__(self.__OnOpenHealGSPQuestionDialog))
		pickDialog.Open(maxPoint - curPoint, 1)
		self.pickDialog = pickDialog

	def __OnOpenHealGSPQuestionDialog(self, healGSP):

		money = healGSP * constInfo.GUILD_MONEY_PER_GSP

		questionDialog = uiCommon.QuestionDialog()
		questionDialog.SetText(localeInfo.GUILD_DO_YOU_HEAL_GSP % (money, healGSP))
		questionDialog.SetAcceptEvent(ui.__mem_func__(self.__OnHealGSP))
		questionDialog.SetCancelEvent(ui.__mem_func__(self.__OnCloseQuestionDialog))
		questionDialog.SetWidth(400)
		questionDialog.Open()
		questionDialog.healGSP = healGSP
		self.questionDialog = questionDialog

	def __OnHealGSP(self):
		net.SendGuildChargeGSPPacket(self.questionDialog.healGSP)
		self.__OnCloseQuestionDialog()

	def __OnCloseQuestionDialog(self):
		if self.questionDialog:
			self.questionDialog.Close()
		self.questionDialog = None

	def OnPickUpGuildSkill(self, skillSlotIndex, type):

		mouseController = mouseModule.mouseController

		if False == mouseController.isAttached():

			skillIndex = player.GetSkillIndex(skillSlotIndex)
			skillLevel = guild.GetSkillLevel(skillSlotIndex)

			if skill.CanUseSkill(skillIndex) and skillLevel > 0:

				if app.IsPressed(app.DIK_LCONTROL):

					player.RequestAddToEmptyLocalQuickSlot(player.SLOT_TYPE_SKILL, skillSlotIndex)
					return

				mouseController.AttachObject(self, player.SLOT_TYPE_SKILL, skillSlotIndex, skillIndex)

		else:
			mouseController.DeattachObject()

	def OnUseGuildSkill(self, slotNumber, type):
		skillIndex = player.GetSkillIndex(slotNumber)
		skillLevel = guild.GetSkillLevel(slotNumber)

		if skillLevel <= 0:
			return

		player.UseGuildSkill(slotNumber)

	def OnUpGuildSkill(self, slotNumber, type):
		skillIndex = player.GetSkillIndex(slotNumber)
		net.SendChatPacket("/gskillup " + str(skillIndex))

	def OnUseSkill(self, slotNumber, coolTime):

		if self.isLoaded==0:
			return

		page = self.pageWindow["SKILL"]

		if page.activeSlot.HasSlot(slotNumber):
			page.activeSlot.SetSlotCoolTime(slotNumber, coolTime)

	def OnStartGuildWar(self, guildSelf, guildOpp):

		if self.isLoaded==0:
			return

		if guild.GetGuildID() != guildSelf:
			return

		guildName = guild.GetGuildName(guildOpp)

	def OnEndGuildWar(self, guildSelf, guildOpp):

		if self.isLoaded==0:
			return

		if guild.GetGuildID() != guildSelf:
			return

		guildName = guild.GetGuildName(guildOpp)

	def OverInItem(self, slotNumber, type):

		if mouseModule.mouseController.isAttached():
			return

		if None != self.tooltipSkill:
			skillIndex = player.GetSkillIndex(slotNumber)
			skillLevel = guild.GetSkillLevel(slotNumber)

			self.tooltipSkill.SetSkill(skillIndex, skillLevel)

	def OverOutItem(self):
		if None != self.tooltipSkill:
			self.tooltipSkill.HideToolTip()

	def OnPressEscapeKey(self):
		self.Close()
		return True
		
class GuildMaterialDonationDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__CreateDialog()
		self.itemVnum = 0
		self.maxCount = 0
		self.acceptEvent = None
		
	def __CreateDialog(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/guild_donate_item.py")

		try:
			self.board = self.GetChild("board")
		except:
			self.board = self

		self.acceptButton = self.GetChild("AcceptButton")
		self.dailyLimitText = self.GetChild("donate_daily_limit_count")
		
		# Item icon
		self.itemIcon = ui.ImageBox()
		self.itemIcon.SetParent(self.board)
		self.itemIcon.SetPosition(89, 98)
		self.itemIcon.Show()
		
		# Item name
		#self.itemNameText = ui.TextLine()
		#self.itemNameText.SetParent(self.board)
		#self.itemNameText.SetPosition(87, 128)
		#self.itemNameText.SetPackedFontColor(0xFFcdc99c)
		#self.itemNameText.SetOutline()
		#self.itemNameText.Show()
		
		# Available count
		#self.availableText = ui.TextLine()
		#self.availableText.SetParent(self.board)
		#self.availableText.SetPosition(20, 75)
		#self.availableText.SetText("Available: ")
		#self.availableText.Show()
		
	def Open(self, itemVnum, itemName, maxCount, materialIndex):
		self.itemVnum = itemVnum
		self.maxCount = maxCount

		# Load item icon
		import item
		item.SelectItem(itemVnum)
		self.itemIcon.LoadImage(item.GetIconImageFileName())

		# Show per-item daily count
		try:
			_, _, _, dailyStone, dailyLog, dailyPlywood = guild.GetDonateStorage()
			dailyCounts = [dailyStone, dailyLog, dailyPlywood]
			donated = dailyCounts[materialIndex] if materialIndex < len(dailyCounts) else 0
		except:
			donated = 0
		self.dailyLimitText.SetText("Daily count %d/15" % donated)

		self.SetCenterPosition()
		self.SetTop()
		self.Show()
		
	def Close(self):
		self.Hide()
		
	def SetAcceptEvent(self, event):
		self.acceptEvent = event
		self.acceptButton.SetEvent(event)
		
	def GetItemVnum(self):
		return self.itemVnum
		
	def GetDonateCount(self):
		return self.maxCount
		
	def OnPressEscapeKey(self):
		self.Close()
		return True

class GuildExpInfoWindow(ui.ThinBoard):
	def __init__(self):
		ui.ThinBoard.__init__(self)
		self.__LoadWindow()

	def __LoadWindow(self):
		self.SetSize(356, 200)
		self.AddFlag("float")
		self.SetCenterPosition()

		title = ui.TextLine()
		title.SetParent(self)
		title.SetHorizontalAlignCenter()
		title.SetPackedFontColor(0xFFFBC401)
		title.SetOutline()
		title.SetText("Increasing Guild Level")
		title.SetPosition(self.GetWidth() / 2, 15)
		title.Show()
		self.title = title
		
		# Description text
		descText = "Guild experience can be increased by using Scroll of Contribution.#"\
		"Such item can be obtained from guild raid final bosses."

		description = ui.MultiTextLineNew()
		description.SetParent(self)
		description.SetTextRange(13)
		description.SetTextType("horizontal#center")
		description.SetPosition(self.GetWidth() / 2, 40)
		description.SetOutline(1)
		description.SetText(descText)
		description.Show()
		self.description = description

		# Item icon and name
		import item
		EXP_ITEM_VNUM = 90013  # Your guild exp item vnum
		
		item.SelectItem(EXP_ITEM_VNUM)
		itemIcon = ui.ImageBox()
		itemIcon.SetParent(self)
		itemIcon.LoadImage(item.GetIconImageFileName())
		itemIcon.SetPosition((self.GetWidth() / 2) - 16, 90)
		itemIcon.Show()
		self.itemIcon = itemIcon

		itemName = ui.TextLine()
		itemName.SetParent(self)
		itemName.SetHorizontalAlignCenter()
		itemName.SetPackedFontColor(0xFFFBC401)
		itemName.SetOutline()
		itemName.SetText(item.GetItemName())
		itemName.SetPosition(self.GetWidth() / 2, 90 + 32 + 5)
		itemName.Show()
		self.itemName = itemName
		
		infoText = "Required item to increase guild experience"
		
		additionalInfo = ui.TextLine()
		additionalInfo.SetParent(self)
		additionalInfo.SetHorizontalAlignCenter()
		additionalInfo.SetOutline()
		additionalInfo.SetText(infoText)
		additionalInfo.SetPosition(self.GetWidth() / 2, 160)
		additionalInfo.Show()
		self.additionalInfo = additionalInfo

	def Open(self):
		self.Show()
		self.SetTop()
		
	def Close(self):
		self.Hide()
		
	def OnPressEscapeKey(self):
		self.Close()
		return True

class DungeonListItem(ui.Window):
	def __init__(self, idx, dungName):
		ui.Window.__init__(self)

		self.idx = idx
		self.selectEvent = None
		self.selectedDiff = -1
		
		self.BuildWindow(dungName)

	def __del__(self):
		ui.Window.__del__(self)
	
	def BuildWindow(self, dungName):
		self.titleBarImg = ui.ImageBox()
		self.titleBarImg.SetParent(self)
		self.titleBarImg.LoadImage("elvion/game/guild/guild_raids/drop_down_main_norm.png")
		self.titleBarImg.SetPosition(0, 0)
		self.titleBarImg.Show()

		self.dungTitleTxt = ui.TextLine("Tahoma:12")
		self.dungTitleTxt.SetParent(self)
		self.dungTitleTxt.SetPosition(93, 11)
		self.dungTitleTxt.SetText(dungName)
		self.dungTitleTxt.SetHorizontalAlignCenter()
		self.dungTitleTxt.SetPackedFontColor(0xFF989797)
		self.dungTitleTxt.SetOutline()
		self.dungTitleTxt.Show()

		self.easyModeBtn = ui.Button()
		self.easyModeBtn.SetParent(self)
		self.easyModeBtn.SetUpVisual("elvion/game/guild/guild_raids/drop_down_sub_norm.png")
		self.easyModeBtn.SetOverVisual("elvion/game/guild/guild_raids/drop_down_sub_down.png")
		self.easyModeBtn.SetDownVisual("elvion/game/guild/guild_raids/drop_down_sub_down.png")
		self.easyModeBtn.SetDisableVisual("elvion/game/guild/guild_raids/drop_down_sub_norm.png")
		self.easyModeBtn.SetPosition(10, 34)
		self.easyModeBtn.SetText("")
		self.easyModeBtn.SAFE_SetEvent(self.OnClickEasyModeBtn)
		self.easyModeBtn.Show()

		self.easyModeTxt = ui.TextLine()
		self.easyModeTxt.SetParent(self.easyModeBtn)
		self.easyModeTxt.SetPosition(83, 1)
		self.easyModeTxt.SetText(localeInfo.GUILD_RAID_EASY_MODE)
		self.easyModeTxt.SetHorizontalAlignCenter()
		self.easyModeTxt.SetPackedFontColor(0xFFeed8c3)
		self.easyModeTxt.SetOutline()
		self.easyModeTxt.Show()

		self.mediumModeBtn = ui.Button()
		self.mediumModeBtn.SetParent(self)
		self.mediumModeBtn.SetUpVisual("elvion/game/guild/guild_raids/drop_down_sub_norm.png")
		self.mediumModeBtn.SetOverVisual("elvion/game/guild/guild_raids/drop_down_sub_down.png")
		self.mediumModeBtn.SetDownVisual("elvion/game/guild/guild_raids/drop_down_sub_down.png")
		self.mediumModeBtn.SetDisableVisual("elvion/game/guild/guild_raids/drop_down_sub_norm.png")
		self.mediumModeBtn.SetPosition(10, 53)
		self.mediumModeBtn.SAFE_SetEvent(self.OnClickMediumModeBtn)
		self.mediumModeBtn.Show()

		self.mediumModeTxt = ui.TextLine()
		self.mediumModeTxt.SetParent(self.mediumModeBtn)
		self.mediumModeTxt.SetPosition(83, 1)
		self.mediumModeTxt.SetText(localeInfo.GUILD_RAID_MEDIUM_MODE + "(lv. 10)")
		self.mediumModeTxt.SetHorizontalAlignCenter()
		self.mediumModeTxt.SetPackedFontColor(0xFFc09464)
		self.mediumModeTxt.SetOutline()
		self.mediumModeTxt.Show()

		self.hardModeBtn = ui.Button()
		self.hardModeBtn.SetParent(self)
		self.hardModeBtn.SetUpVisual("elvion/game/guild/guild_raids/drop_down_sub_norm.png")
		self.hardModeBtn.SetOverVisual("elvion/game/guild/guild_raids/drop_down_sub_down.png")
		self.hardModeBtn.SetDownVisual("elvion/game/guild/guild_raids/drop_down_sub_down.png")
		self.hardModeBtn.SetDisableVisual("elvion/game/guild/guild_raids/drop_down_sub_norm.png")
		self.hardModeBtn.SetPosition(10, 72)
		self.hardModeBtn.SAFE_SetEvent(self.OnClickHardModeBtn)
		self.hardModeBtn.Show()

		self.hardModeTxt = ui.TextLine()
		self.hardModeTxt.SetParent(self.hardModeBtn)
		self.hardModeTxt.SetPosition(83, 1)
		self.hardModeTxt.SetText(localeInfo.GUILD_RAID_HARD_MODE + "(lv. 20)")
		self.hardModeTxt.SetHorizontalAlignCenter()
		self.hardModeTxt.SetPackedFontColor(0xFFa73c3c)
		self.hardModeTxt.SetOutline()
		self.hardModeTxt.Show()
		
		self.SetSize(186, 91)
	
	def SetClippingMaskWindow(self, window):
		ui.Window.SetClippingMaskWindow(self, window)
		
		self.titleBarImg.SetClippingMaskWindow(window)
		self.dungTitleTxt.SetClippingMaskWindow(window)
		self.easyModeBtn.SetClippingMaskWindow(window)
		self.mediumModeBtn.SetClippingMaskWindow(window)
		self.hardModeBtn.SetClippingMaskWindow(window)
		self.easyModeTxt.SetClippingMaskWindow(window)
		self.mediumModeTxt.SetClippingMaskWindow(window)
		self.hardModeTxt.SetClippingMaskWindow(window)

	def RefreshGuildLevel(self):
		level = guild.GetGuildLevel()
		if level < 10:
			self.mediumModeBtn.SetDisableVisual("elvion/game/guild/guild_raids/drop_down_sub_norm.png")
			self.mediumModeBtn.Disable()
		else:
			self.mediumModeBtn.SetDisableVisual("elvion/game/guild/guild_raids/drop_down_sub_down.png")
			self.mediumModeBtn.Enable()
		if level < 20:
			self.hardModeBtn.SetDisableVisual("elvion/game/guild/guild_raids/drop_down_sub_norm.png")
			self.hardModeBtn.Disable()
		else:
			self.hardModeBtn.SetDisableVisual("elvion/game/guild/guild_raids/drop_down_sub_down.png")
			self.hardModeBtn.Enable()
			
		if self.selectedDiff != -1:
			self.SelectButton(self.selectedDiff)

	def UnselectButton(self, diffIdx):
		self.selectedDiff = -1
		if diffIdx == 0:
			self.easyModeBtn.Enable()
		elif diffIdx == 1:
			self.mediumModeBtn.Enable()
		elif diffIdx == 2:
			self.hardModeBtn.Enable()

	def SelectButton(self, diffIdx):
		self.selectedDiff = diffIdx
		if diffIdx == 0:
			self.easyModeBtn.Disable()
		elif diffIdx == 1:
			self.mediumModeBtn.Disable()
		elif diffIdx == 2:
			self.hardModeBtn.Disable()

	def SetSelectDungeonEvent(self, event):
		self.selectEvent = event

	def OnClickEasyModeBtn(self):
		self.selectEvent(self.idx, 0)

	def OnClickMediumModeBtn(self):
		self.selectEvent(self.idx, 1)

	def OnClickHardModeBtn(self):
		self.selectEvent(self.idx, 2)

class BuildGuildBuildingWindow(ui.ScriptWindow):

	if localeInfo.IsJAPAN():
		GUILD_CATEGORY_LIST = (
				("HEADQUARTER", "��b���z��"),
				("FACILITY", "�g�����z��"),
				("OBJECT", "���̑�"),
			)
	elif localeInfo.IsYMIR() or localeInfo.IsWE_KOREA():
		GUILD_CATEGORY_LIST = (
				("HEADQUARTER", "���ǹ�"),
				("FACILITY", "��ɰǹ�"),
				("OBJECT", "���湰"),
			)
	elif localeInfo.IsEUROPE() or localeInfo.IsHONGKONG():
		GUILD_CATEGORY_LIST = (
				("HEADQUARTER", localeInfo.GUILD_HEADQUARTER),
				("FACILITY", 	localeInfo.GUILD_FACILITY),
				("OBJECT", 	localeInfo.GUILD_OBJECT),
			)
	else:
		try :
			GUILD_CATEGORY_LIST = (
					("HEADQUARTER", localeInfo.GUILD_HEADQUARTER),
					("FACILITY", 	localeInfo.GUILD_FACILITY),
					("OBJECT", 	localeInfo.GUILD_OBJECT),
				)
		except:
			GUILD_CATEGORY_LIST = (
					("HEADQUARTER", "Main Building"),
					("FACILITY", "Facility"),
					("OBJECT", "Object"),
				)

	MODE_VIEW = 0
	MODE_POSITIONING = 1
	MODE_PREVIEW = 2

	BUILDING_ALPHA = 0.55

	ENABLE_COLOR = grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0)
	DISABLE_COLOR = grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0)

	START_INSTANCE_INDEX = 123450

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadWindow()

		self.closeEvent = None
		self.popup = None
		self.mode = self.MODE_VIEW
		self.race = 0
		self.type = None
		self.x = 0
		self.y = 0
		self.z = 0
		self.rot_x = 0
		self.rot_y = 0
		self.rot_z = 0
		self.rot_x_limit = 0
		self.rot_y_limit = 0
		self.rot_z_limit = 0
		self.needMoney = 0
		self.needStoneCount = 0
		self.needLogCount = 0
		self.needPlywoodCount = 0

		self.indexList = []
		self.raceList = []
		self.posList = []
		self.rotList = []

		index = 0
		for category in self.GUILD_CATEGORY_LIST:
			self.categoryList.InsertItem(index, category[1])
			index += 1

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self):

		try:
			pyScrLoader = ui.PythonScriptLoader()
			if localeInfo.IsARABIC():
				pyScrLoader.LoadScriptFile(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "buildguildbuildingwindow.py")
			elif localeInfo.IsVIETNAM():
				pyScrLoader.LoadScriptFile(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "buildguildbuildingwindow.py")
			else:
				pyScrLoader.LoadScriptFile(self, "uiscript/buildguildbuildingwindow.py")
		except:
			import exception
			exception.Abort("DeclareGuildWarWindow.__CreateDialog - LoadScript")

		try:
			getObject = self.GetChild
			self.board = getObject("Board")
			self.categoryList = getObject("CategoryList")
			self.buildingList = getObject("BuildingList")
			self.listScrollBar = getObject("ListScrollBar")
			self.positionButton = getObject("PositionButton")
			self.previewButton = getObject("PreviewButton")
			self.posValueX = getObject("BuildingPositionXValue")
			self.posValueY = getObject("BuildingPositionYValue")
			self.ctrlRotationX = getObject("BuildingRotationX")
			self.ctrlRotationY = getObject("BuildingRotationY")
			self.ctrlRotationZ = getObject("BuildingRotationZ")
			self.buildingPriceValue = getObject("BuildingPriceValue")
			self.buildingMaterialStoneValue = getObject("BuildingMaterialStoneValue")
			self.buildingMaterialLogValue = getObject("BuildingMaterialLogValue")
			self.buildingMaterialPlywoodValue = getObject("BuildingMaterialPlywoodValue")

			self.positionButton.SetEvent(ui.__mem_func__(self.__OnSelectPositioningMode))
			self.previewButton.SetToggleDownEvent(ui.__mem_func__(self.__OnEnterPreviewMode))
			self.previewButton.SetToggleUpEvent(ui.__mem_func__(self.__OnLeavePreviewMode))
			self.ctrlRotationX.SetEvent(ui.__mem_func__(self.__OnChangeRotation))
			self.ctrlRotationY.SetEvent(ui.__mem_func__(self.__OnChangeRotation))
			self.ctrlRotationZ.SetEvent(ui.__mem_func__(self.__OnChangeRotation))
			self.listScrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScrollBuildingList))

			getObject("CategoryList").SetEvent(ui.__mem_func__(self.__OnSelectCategory))
			getObject("BuildingList").SetEvent(ui.__mem_func__(self.__OnSelectBuilding))
			getObject("AcceptButton").SetEvent(ui.__mem_func__(self.Build))
			getObject("CancelButton").SetEvent(ui.__mem_func__(self.Close))
			self.board.SetCloseEvent(ui.__mem_func__(self.Close))

		except:
			import exception
			exception.Abort("BuildGuildBuildingWindow.__LoadWindow - BindObject")

	def __CreateWallBlock(self, race, x, y, rot=0.0 ):
		idx = self.START_INSTANCE_INDEX + len(self.indexList)
		self.indexList.append(idx)
		self.raceList.append(race)
		self.posList.append((x, y))
		self.rotList.append(rot)
		chr.CreateInstance(idx)
		chr.SelectInstance(idx)
		chr.SetVirtualID(idx)
		chr.SetInstanceType(chr.INSTANCE_TYPE_OBJECT)

		chr.SetRace(race)
		chr.SetArmor(0)
		chr.Refresh()
		chr.SetLoopMotion(chr.MOTION_WAIT)
		chr.SetBlendRenderMode(idx, self.BUILDING_ALPHA)
		chr.SetRotationAll(0.0, 0.0, rot)

		self.ctrlRotationX.SetSliderPos(0.5)
		self.ctrlRotationY.SetSliderPos(0.5)
		self.ctrlRotationZ.SetSliderPos(0.5)

	def __GetObjectSize(self, race):
		idx = self.START_INSTANCE_INDEX + 1000
		chr.CreateInstance(idx)
		chr.SelectInstance(idx)
		chr.SetVirtualID(idx)
		chr.SetInstanceType(chr.INSTANCE_TYPE_OBJECT)

		chr.SetRace(race)
		chr.SetArmor(0)
		chr.Refresh()
		chr.SetLoopMotion(chr.MOTION_WAIT)
		sx, sy, ex, ey = chr.GetBoundBoxOnlyXY(idx)
		chr.DeleteInstance(idx)
		return sx, sy, ex, ey

	def __GetBuildInPosition(self):

		zList = []
		zList.append( background.GetHeight(self.x+self.sxPos, self.y+self.syPos) )
		zList.append( background.GetHeight(self.x+self.sxPos, self.y+self.eyPos) )
		zList.append( background.GetHeight(self.x+self.exPos, self.y+self.syPos) )
		zList.append( background.GetHeight(self.x+self.exPos, self.y+self.eyPos) )
		zList.append( background.GetHeight(self.x+(self.exPos+self.sxPos)/2, self.y+(self.eyPos+self.syPos)/2) )
		zList.sort()
		return zList[3]

	def __CreateBuildInInstance(self,race):

		self.__DeleteInstance()

		object_base = race - race%10

		door_minX, door_minY, door_maxX, door_maxY = self.__GetObjectSize(object_base+4)
		corner_minX, corner_minY, corner_maxX, corner_maxY = self.__GetObjectSize(object_base+1)
		line_minX, line_minY, line_maxX, line_maxY = self.__GetObjectSize(object_base+2)
		line_width = line_maxX - line_minX
		line_width_half = line_width / 2

		X_SIZE_STEP = 2 * 2
		Y_SIZE_STEP = 8
		sxPos = door_maxX - corner_minX + (line_width_half*X_SIZE_STEP)
		exPos = -sxPos
		syPos = 0
		eyPos = -(corner_maxY*2 + line_width*Y_SIZE_STEP)

		self.sxPos = sxPos
		self.syPos = syPos
		self.exPos = exPos
		self.eyPos = eyPos

		z = self.__GetBuildInPosition()

		self.__CreateWallBlock(object_base+4, 0.0, syPos)

		self.__CreateWallBlock(object_base+1, sxPos, syPos)
		self.__CreateWallBlock(object_base+1, exPos, syPos, 270.0)
		self.__CreateWallBlock(object_base+1, sxPos, eyPos, 90.0)
		self.__CreateWallBlock(object_base+1, exPos, eyPos,180.0 )

		lineBlock = object_base+2
		line_startX = -door_maxX - line_minX - (line_width_half*X_SIZE_STEP)
		self.__CreateWallBlock(lineBlock, line_startX, eyPos)
		self.__CreateWallBlock(lineBlock, line_startX+line_width*1, eyPos)
		self.__CreateWallBlock(lineBlock, line_startX+line_width*2, eyPos)
		self.__CreateWallBlock(lineBlock, line_startX+line_width*3, eyPos)
		for i in xrange(X_SIZE_STEP):
			self.__CreateWallBlock(lineBlock, line_startX+line_width*(3+i+1), eyPos)
		for i in xrange(X_SIZE_STEP/2):
			self.__CreateWallBlock(lineBlock, door_minX - line_maxX - line_width*i, syPos)
			self.__CreateWallBlock(lineBlock, door_maxX - line_minX + line_width*i, syPos)
		for i in xrange(Y_SIZE_STEP):
			self.__CreateWallBlock(lineBlock, sxPos, line_minX + corner_minX - line_width*i, 90.0)
			self.__CreateWallBlock(lineBlock, exPos, line_minX + corner_minX - line_width*i, 90.0)

		self.SetBuildingPosition(int(self.x), int(self.y), self.__GetBuildInPosition())

	def __DeleteInstance(self):
		if not self.indexList:
			return

		for index in self.indexList:
			chr.DeleteInstance(index)

		self.indexList = []
		self.raceList = []
		self.posList = []
		self.rotList = []

	def __CreateInstance(self, race):

		self.__DeleteInstance()

		self.race = race

		idx = self.START_INSTANCE_INDEX
		self.indexList.append(idx)
		self.posList.append((0, 0))
		self.rotList.append(0)

		chr.CreateInstance(idx)
		chr.SelectInstance(idx)
		chr.SetVirtualID(idx)
		chr.SetInstanceType(chr.INSTANCE_TYPE_OBJECT)

		chr.SetRace(race)
		chr.SetArmor(0)
		chr.Refresh()
		chr.SetLoopMotion(chr.MOTION_WAIT)
		chr.SetBlendRenderMode(idx, self.BUILDING_ALPHA)

		self.SetBuildingPosition(int(self.x), int(self.y), 0)
		self.ctrlRotationX.SetSliderPos(0.5)
		self.ctrlRotationY.SetSliderPos(0.5)
		self.ctrlRotationZ.SetSliderPos(0.5)

	def Build(self):

		if not self.__IsEnoughMoney():
			self.__PopupDialog(localeInfo.GUILD_NOT_ENOUGH_MONEY)
			return
		if not self.__IsEnoughMaterialStone():
			self.__PopupDialog(localeInfo.GUILD_NOT_ENOUGH_MATERIAL)
			return
		if not self.__IsEnoughMaterialLog():
			self.__PopupDialog(localeInfo.GUILD_NOT_ENOUGH_MATERIAL)
			return
		if not self.__IsEnoughMaterialPlywood():
			self.__PopupDialog(localeInfo.GUILD_NOT_ENOUGH_MATERIAL)
			return

		if "BUILDIN" == self.type:
			for i in xrange(len(self.raceList)):
				race = self.raceList[i]
				xPos, yPos = self.posList[i]
				rot = self.rotList[i]
				net.SendChatPacket("/build c %d %d %d %d %d %d" % (race, int(self.x+xPos), int(self.y+yPos), self.rot_x, self.rot_y, rot))
		else:
			net.SendChatPacket("/build c %d %d %d %d %d %d" % (self.race, int(self.x), int(self.y), self.rot_x, self.rot_y, self.rot_z))

		self.Close()

	def Open(self):
		x, y, z = player.GetMainCharacterPosition()
		app.SetCameraSetting(int(x), int(-y), int(z), 3000, 0, 30)

		background.VisibleGuildArea()

		self.x = x
		self.y = y
		self.z = z
		self.categoryList.SelectItem(0)
		self.buildingList.SelectItem(0)
		self.SetTop()
		self.Show()
		self.__DisablePCBlocker()

		import debugInfo
		if debugInfo.IsDebugMode():
			self.categoryList.SelectItem(2)
			self.buildingList.SelectItem(0)

	def Close(self):

		self.__DeleteInstance()

		background.DisableGuildArea()

		self.Hide()
		self.__OnClosePopupDialog()
		self.__EnablePCBlocker()
		self.__UnlockCameraMoving()
		if self.closeEvent:
			self.closeEvent()

	def Destory(self):
		self.Close()

		self.ClearDictionary()
		self.board = None
		self.categoryList = None
		self.buildingList = None
		self.listScrollBar = None
		self.positionButton = None
		self.previewButton = None
		self.posValueX = None
		self.posValueY = None
		self.ctrlRotationX = None
		self.ctrlRotationY = None
		self.ctrlRotationZ = None
		self.buildingPriceValue = None
		self.buildingMaterialStoneValue = None
		self.buildingMaterialLogValue = None
		self.buildingMaterialPlywoodValue = None
		self.closeEvent = None

	def SetCloseEvent(self, event):
		self.closeEvent = event

	def __PopupDialog(self, text):
		popup = uiCommon.PopupDialog()
		popup.SetText(text)
		popup.SetAcceptEvent(self.__OnClosePopupDialog)
		popup.Open()
		self.popup = popup

	def __OnClosePopupDialog(self):
		self.popup = None

	def __EnablePCBlocker(self):
		chr.SetInstanceType(chr.INSTANCE_TYPE_BUILDING)

		for idx in self.indexList:
			chr.SetBlendRenderMode(idx, 1.0)

	def __DisablePCBlocker(self):
		chr.SetInstanceType(chr.INSTANCE_TYPE_OBJECT)

		for idx in self.indexList:
			chr.SetBlendRenderMode(idx, self.BUILDING_ALPHA)

	def __OnSelectPositioningMode(self):
		if self.MODE_PREVIEW == self.mode:
			self.positionButton.SetUp()
			return

		self.mode = self.MODE_POSITIONING
		self.Hide()

	def __OnEnterPreviewMode(self):

		if self.MODE_POSITIONING == self.mode:
			self.previewButton.SetUp()
			return

		self.mode = self.MODE_PREVIEW
		self.positionButton.SetUp()
		self.__UnlockCameraMoving()
		self.__EnablePCBlocker()

	def __OnLeavePreviewMode(self):
		self.__RestoreViewMode()

	def __RestoreViewMode(self):
		self.__DisablePCBlocker()
		self.__LockCameraMoving()
		self.mode = self.MODE_VIEW
		self.positionButton.SetUp()
		self.previewButton.SetUp()

	def __IsEnoughMoney(self):

		if app.IsEnableTestServerFlag():
			return True

		curMoney = player.GetMoney()
		if curMoney < self.needMoney:
			return False
		return True

	def __IsEnoughMaterialStone(self):

		if app.IsEnableTestServerFlag():
			return True

		curStoneCount = player.GetItemCountByVnum(MATERIAL_STONE_ID)
		if curStoneCount < self.needStoneCount:
			return False
		return True

	def __IsEnoughMaterialLog(self):

		if app.IsEnableTestServerFlag():
			return True

		curLogCount = player.GetItemCountByVnum(MATERIAL_LOG_ID)
		if curLogCount < self.needLogCount:
			return False
		return True

	def __IsEnoughMaterialPlywood(self):

		if app.IsEnableTestServerFlag():
			return True

		curPlywoodCount = player.GetItemCountByVnum(MATERIAL_PLYWOOD_ID)
		if curPlywoodCount < self.needPlywoodCount:
			return False
		return True

	def __OnSelectCategory(self):
		self.listScrollBar.SetPos(0.0)
		self.__RefreshItem()

	def __SetBuildingData(self, data):
		self.buildingPriceValue.SetText(NumberToMoneyString(data["PRICE"]))

		self.needMoney = int(data["PRICE"])

		materialList = data["MATERIAL"]
		self.needStoneCount = int(materialList[MATERIAL_STONE_INDEX])
		self.needLogCount = int(materialList[MATERIAL_LOG_INDEX])
		self.needPlywoodCount = int(materialList[MATERIAL_PLYWOOD_INDEX])

		if (localeInfo.IsEUROPE() and app.GetLocalePath() != "locale/ca") and (localeInfo.IsEUROPE() and app.GetLocalePath() != "locale/br"):
			self.buildingMaterialStoneValue.SetText(materialList[MATERIAL_STONE_INDEX])
			self.buildingMaterialLogValue.SetText(materialList[MATERIAL_LOG_INDEX] )
			self.buildingMaterialPlywoodValue.SetText(materialList[MATERIAL_PLYWOOD_INDEX])
		else:
			self.buildingMaterialStoneValue.SetText(materialList[MATERIAL_STONE_INDEX] + localeInfo.THING_COUNT)
			self.buildingMaterialLogValue.SetText(materialList[MATERIAL_LOG_INDEX] + localeInfo.THING_COUNT)
			self.buildingMaterialPlywoodValue.SetText(materialList[MATERIAL_PLYWOOD_INDEX] + localeInfo.THING_COUNT)
		if self.__IsEnoughMoney():
			self.buildingPriceValue.SetPackedFontColor(self.ENABLE_COLOR)
		else:
			self.buildingPriceValue.SetPackedFontColor(self.DISABLE_COLOR)

		if self.__IsEnoughMaterialStone():
			self.buildingMaterialStoneValue.SetPackedFontColor(self.ENABLE_COLOR)
		else:
			self.buildingMaterialStoneValue.SetPackedFontColor(self.DISABLE_COLOR)

		if self.__IsEnoughMaterialLog():
			self.buildingMaterialLogValue.SetPackedFontColor(self.ENABLE_COLOR)
		else:
			self.buildingMaterialLogValue.SetPackedFontColor(self.DISABLE_COLOR)

		if self.__IsEnoughMaterialPlywood():
			self.buildingMaterialPlywoodValue.SetPackedFontColor(self.ENABLE_COLOR)
		else:
			self.buildingMaterialPlywoodValue.SetPackedFontColor(self.DISABLE_COLOR)

		self.rot_x_limit = data["X_ROT_LIMIT"]
		self.rot_y_limit = data["Y_ROT_LIMIT"]
		self.rot_z_limit = data["Z_ROT_LIMIT"]
		self.ctrlRotationX.Enable()
		self.ctrlRotationY.Enable()
		self.ctrlRotationZ.Enable()
		if 0 == self.rot_x_limit:
			self.ctrlRotationX.Disable()
		if 0 == self.rot_y_limit:
			self.ctrlRotationY.Disable()
		if 0 == self.rot_z_limit:
			self.ctrlRotationZ.Disable()

	def __OnSelectBuilding(self):
		buildingIndex = self.buildingList.GetSelectedItem()
		if buildingIndex >= len(BUILDING_DATA_LIST):
			return

		categoryIndex = self.categoryList.GetSelectedItem()
		if categoryIndex >= len(self.GUILD_CATEGORY_LIST):
			return
		selectedType = self.GUILD_CATEGORY_LIST[categoryIndex][0]

		index = 0
		for data in BUILDING_DATA_LIST:
			type = data["TYPE"]
			vnum = data["VNUM"]
			if selectedType != type:
				continue

			if index == buildingIndex:
				self.type = type
				if "BUILDIN" == self.type:
					self.__CreateBuildInInstance(vnum)
				else:
					self.__CreateInstance(vnum)

				self.__SetBuildingData(data)

			index += 1

	def __OnScrollBuildingList(self):
		viewItemCount = self.buildingList.GetViewItemCount()
		itemCount = self.buildingList.GetItemCount()
		pos = self.listScrollBar.GetPos() * (itemCount-viewItemCount)
		self.buildingList.SetBasePos(int(pos))

	def __OnChangeRotation(self):
		self.rot_x = self.ctrlRotationX.GetSliderPos() * self.rot_x_limit - self.rot_x_limit/2
		self.rot_y = self.ctrlRotationY.GetSliderPos() * self.rot_y_limit - self.rot_y_limit/2
		self.rot_z = (self.ctrlRotationZ.GetSliderPos() * 360 + 180) % 360
		if "BUILDIN" == self.type:
			chr.SetRotationAll(self.rot_x, self.rot_y, self.rot_z)
		else:
			chr.SetRotationAll(self.rot_x, self.rot_y, self.rot_z)

	def __LockCameraMoving(self):
		app.SetCameraSetting(int(self.x), int(-self.y), int(self.z), 3000, 0, 30)

	def __UnlockCameraMoving(self):
		app.SetDefaultCamera()

	def __RefreshItem(self):

		self.buildingList.ClearItem()

		categoryIndex = self.categoryList.GetSelectedItem()
		if categoryIndex >= len(self.GUILD_CATEGORY_LIST):
			return
		selectedType = self.GUILD_CATEGORY_LIST[categoryIndex][0]

		index = 0
		for data in BUILDING_DATA_LIST:
			if selectedType != data["TYPE"]:
				continue

			if data["SHOW"]:
				self.buildingList.InsertItem(index, data["LOCAL_NAME"])

			index += 1

		self.buildingList.SelectItem(0)

		if self.buildingList.GetItemCount() < self.buildingList.GetViewItemCount():
			self.buildingList.SetSize(120, self.buildingList.GetHeight())
			self.buildingList.LocateItem()
			self.listScrollBar.Hide()
		else:
			self.buildingList.SetSize(105, self.buildingList.GetHeight())
			self.buildingList.LocateItem()
			self.listScrollBar.Show()

	def SettleCurrentPosition(self):
		guildID = miniMap.GetGuildAreaID(self.x, self.y)

		import debugInfo
		if debugInfo.IsDebugMode():
			guildID = player.GetGuildID()

		if guildID != player.GetGuildID():
			return

		self.__RestoreViewMode()
		self.__LockCameraMoving()
		self.Show()

	def SetBuildingPosition(self, x, y, z):
		self.x = x
		self.y = y
		self.posValueX.SetText(str(int(x)))
		self.posValueY.SetText(str(int(y)))

		for i in xrange(len(self.indexList)):
			idx = self.indexList[i]
			xPos, yPos = self.posList[i]

			chr.SelectInstance(idx)
			if 0 != z:
				self.z = z
				chr.SetPixelPosition(int(x+xPos), int(y+yPos), int(z))
			else:
				chr.SetPixelPosition(int(x+xPos), int(y+yPos))

	def IsPositioningMode(self):
		if self.MODE_POSITIONING == self.mode:
			return True
		return False

	def IsPreviewMode(self):
		if self.MODE_PREVIEW == self.mode:
			return True
		return False

	def OnPressEscapeKey(self):
		self.Close()
		return True

"""
- ��������

���ӵ��Խ�:
	RecvLandPacket:
		CPythonMiniMap::RegisterGuildArea

�����̵���:
	PythonPlayer::Update()
		CPythonPlayer::__Update_NotifyGuildAreaEvent()
			game.py.BINARY_Guild_EnterGuildArea
				uigameButton.GameButtonWindow.ShowBuildButton()
			game.py.BINARY_Guild_ExitGuildArea
				uigameButton.GameButtonWindow.HideBuildButton()

BuildButton:
!��������� ó�� ����
!�ǹ��� �־ ���� ��ư�� ����

!�ǹ��� �ӽ÷� ����ϴ� VID �� ������ �����ִ� �Ͱ� ȥ���� ������ ����
!�ǹ� VNUM �� BuildGuildBuildingWindow.BUILDING_VNUM_LIST �� �̿��� ��ȯ

!�ǹ� �������� /build c(reate)
!�ǹ� �μ����� /build d(estroy)
!rotation �� ������ degree

	interfaceModule.interface.__OnClickBuildButton:
		interfaceModule.interface.BUILD_OpenWindow:

AcceptButton:
	BuildGuildBuildingWindow.Build:
		net.SendChatPacket("/build c vnum x y x_rot y_rot z_rot")

PreviewButton:
	__OnPreviewMode:
	__RestoreViewMode:

�ǹ� �μ���:
	uiTarget.TargetBoard.__OnDestroyBuilding
		net.SendChatPacket("/build d vid")
"""

if __name__ == "__main__":

	import app
	import wndMgr
	import systemSetting
	import mouseModule
	import grp
	import ui


	app.SetMouseHandler(mouseModule.mouseController)
	app.SetHairColorEnable(True)
	wndMgr.SetMouseHandler(mouseModule.mouseController)
	wndMgr.SetScreenSize(systemSetting.GetWidth(), systemSetting.GetHeight())
	app.Create("METIN2 CLOSED BETA", systemSetting.GetWidth(), systemSetting.GetHeight(), 1)
	mouseModule.mouseController.Create()

	import chrmgr
	chrmgr.CreateRace(0)
	chrmgr.SelectRace(0)
	chrmgr.SetPathName("d:/ymir Work/pc/warrior/")
	chrmgr.LoadRaceData("warrior.msm")
	chrmgr.SetPathName("d:/ymir work/pc/warrior/general/")
	chrmgr.RegisterMotionMode(chr.MOTION_MODE_GENERAL)
	chrmgr.RegisterMotionData(chr.MOTION_MODE_GENERAL, chr.MOTION_WAIT, "wait.msa")
	chrmgr.RegisterMotionData(chr.MOTION_MODE_GENERAL, chr.MOTION_RUN, "run.msa")

	def LoadGuildBuildingList(filename):
		handle = app.OpenTextFile(filename)
		count = app.GetTextFileLineCount(handle)
		for i in xrange(count):
			line = app.GetTextFileLine(handle, i)
			tokens = line.split("\t")

			TOKEN_VNUM = 0
			TOKEN_TYPE = 1
			TOKEN_NAME = 2
			TOKEN_LOCAL_NAME = 3
			NO_USE_TOKEN_SIZE_1 = 4
			NO_USE_TOKEN_SIZE_2 = 5
			NO_USE_TOKEN_SIZE_3 = 6
			NO_USE_TOKEN_SIZE_4 = 7
			TOKEN_X_ROT_LIMIT = 8
			TOKEN_Y_ROT_LIMIT = 9
			TOKEN_Z_ROT_LIMIT = 10
			TOKEN_PRICE = 11
			TOKEN_MATERIAL = 12
			TOKEN_NPC = 13
			TOKEN_GROUP = 14
			TOKEN_DEPEND_GROUP = 15
			TOKEN_ENABLE_FLAG = 16
			LIMIT_TOKEN_COUNT = 17

			if not tokens[TOKEN_VNUM].isdigit():
				continue

			if not int(tokens[TOKEN_ENABLE_FLAG]):
				continue

			if len(tokens) < LIMIT_TOKEN_COUNT:
				import dbg
				dbg.TraceError("Strange token count [%d/%d] [%s]" % (len(tokens), LIMIT_TOKEN_COUNT, line))
				continue

			ENABLE_FLAG_TYPE_NOT_USE = False
			ENABLE_FLAG_TYPE_USE = True
			ENABLE_FLAG_TYPE_USE_BUT_HIDE = 2

			if ENABLE_FLAG_TYPE_NOT_USE == int(tokens[TOKEN_ENABLE_FLAG]):
				continue

			vnum = int(tokens[TOKEN_VNUM])
			type = tokens[TOKEN_TYPE]
			name = tokens[TOKEN_NAME]
			localName = tokens[TOKEN_LOCAL_NAME]
			xRotLimit = int(tokens[TOKEN_X_ROT_LIMIT])
			yRotLimit = int(tokens[TOKEN_Y_ROT_LIMIT])
			zRotLimit = int(tokens[TOKEN_Z_ROT_LIMIT])
			price = tokens[TOKEN_PRICE]
			material = tokens[TOKEN_MATERIAL]

			folderName = ""
			if "HEADQUARTER" == type:
				folderName = "headquarter"
			elif "FACILITY" == type:
				folderName = "facility"
			elif "OBJECT" == type:
				folderName = "object"

			materialList = ["0", "0", "0"]
			if material[0] == "\"":
				material = material[1:]
			if material[-1] == "\"":
				material = material[:-1]
			for one in material.split("/"):
				data = one.split(",")
				if 2 != len(data):
					continue
				itemID = int(data[0])
				count = data[1]

				if itemID == MATERIAL_STONE_ID:
					materialList[MATERIAL_STONE_INDEX] = count
				elif itemID == MATERIAL_LOG_ID:
					materialList[MATERIAL_LOG_INDEX] = count
				elif itemID == MATERIAL_PLYWOOD_ID:
					materialList[MATERIAL_PLYWOOD_INDEX] = count

			import chrmgr
			chrmgr.RegisterRaceSrcName(name, folderName)
			chrmgr.RegisterRaceName(vnum, name)

			appendingData = { "VNUM":vnum,
							  "TYPE":type,
							  "NAME":name,
							  "LOCAL_NAME":localName,
							  "X_ROT_LIMIT":xRotLimit,
							  "Y_ROT_LIMIT":yRotLimit,
							  "Z_ROT_LIMIT":zRotLimit,
							  "PRICE":price,
							  "MATERIAL":materialList,
							  "SHOW" : True }

			if ENABLE_FLAG_TYPE_USE_BUT_HIDE == int(tokens[TOKEN_ENABLE_FLAG]):
				appendingData["SHOW"] = False

			BUILDING_DATA_LIST.append(appendingData)

		app.CloseTextFile(handle)

	LoadGuildBuildingList(app.GetLocalePath()+"/GuildBuildingList.txt")

	class TestGame(ui.Window):
		def __init__(self):
			ui.Window.__init__(self)

			x = 30000
			y = 40000

			self.wndGuildBuilding = None
			self.onClickKeyDict = {}
			self.onClickKeyDict[app.DIK_SPACE] = lambda: self.OpenBuildGuildBuildingWindow()

			background.Initialize()
			background.LoadMap("metin2_map_a1", x, y, 0)
			background.SetShadowLevel(background.SHADOW_ALL)

			self.MakeCharacter(1, 0, x, y)
			player.SetMainCharacterIndex(1)
			chr.SelectInstance(1)

		def __del__(self):
			ui.Window.__del__(self)

		def MakeCharacter(self, index, race, x, y):
			chr.CreateInstance(index)
			chr.SelectInstance(index)
			chr.SetVirtualID(index)
			chr.SetInstanceType(chr.INSTANCE_TYPE_PLAYER)

			chr.SetRace(race)
			chr.SetArmor(0)
			chr.SetHair(0)
			chr.Refresh()
			chr.SetMotionMode(chr.MOTION_MODE_GENERAL)
			chr.SetLoopMotion(chr.MOTION_WAIT)

			chr.SetPixelPosition(x, y)
			chr.SetDirection(chr.DIR_NORTH)

		def OpenBuildGuildBuildingWindow(self):
			self.wndGuildBuilding = BuildGuildBuildingWindow()
			self.wndGuildBuilding.Open()
			self.wndGuildBuilding.SetParent(self)
			self.wndGuildBuilding.SetTop()

		def OnKeyUp(self, key):
			if key in self.onClickKeyDict:
				self.onClickKeyDict[key]()
			return True

		def OnMouseLeftButtonDown(self):
			if self.wndGuildBuilding:
				if self.wndGuildBuilding.IsPositioningMode():
					self.wndGuildBuilding.SettleCurrentPosition()
					return

			player.SetMouseState(player.MBT_LEFT, player.MBS_PRESS);
			return True

		def OnMouseLeftButtonUp(self):
			if self.wndGuildBuilding:
				return

			player.SetMouseState(player.MBT_LEFT, player.MBS_CLICK)
			return True

		def OnMouseRightButtonDown(self):
			player.SetMouseState(player.MBT_RIGHT, player.MBS_PRESS);
			return True

		def OnMouseRightButtonUp(self):
			player.SetMouseState(player.MBT_RIGHT, player.MBS_CLICK);
			return True

		def OnMouseMiddleButtonDown(self):
			player.SetMouseMiddleButtonState(player.MBS_PRESS)

		def OnMouseMiddleButtonUp(self):
			player.SetMouseMiddleButtonState(player.MBS_CLICK)

		def OnUpdate(self):
			app.UpdateGame()

			if self.wndGuildBuilding:
				if self.wndGuildBuilding.IsPositioningMode():
					x, y, z = background.GetPickingPoint()
					self.wndGuildBuilding.SetBuildingPosition(x, y, z)

		def OnRender(self):
			app.RenderGame()
			grp.PopState()
			grp.SetInterfaceRenderState()

	game = TestGame()
	game.SetSize(systemSetting.GetWidth(), systemSetting.GetHeight())
	game.Show()

	wndGuildBuilding = BuildGuildBuildingWindow()
	wndGuildBuilding.Open()
	wndGuildBuilding.SetTop()

	app.Loop()

	"""
	- ��������

���ӵ��Խ�:
	RecvLandPacket:
		CPythonMiniMap::RegisterGuildArea

�����̵���:
	PythonPlayer::Update()
		CPythonPlayer::__Update_NotifyGuildAreaEvent()
			game.py.BINARY_Guild_EnterGuildArea
				uigameButton.GameButtonWindow.ShowBuildButton()
			game.py.BINARY_Guild_ExitGuildArea
				uigameButton.GameButtonWindow.HideBuildButton()

BuildButton:
!��������� ó�� ����
!�ǹ��� �־ ���� ��ư�� ����

!�ǹ��� �ӽ÷� ����ϴ� VID �� ������ �����ִ� �Ͱ� ȥ���� ������ ����
!�ǹ� VNUM �� BuildGuildBuildingWindow.BUILDING_VNUM_LIST �� �̿��� ��ȯ

!�ǹ� �������� /build c(reate)
!�ǹ� �μ����� /build d(estroy)
!rotation �� ������ degree

	interfaceModule.interface.__OnClickBuildButton:
		interfaceModule.interface.BUILD_OpenWindow:

AcceptButton:
	BuildGuildBuildingWindow.Build:
		net.SendChatPacket("/build c vnum x y x_rot y_rot z_rot")

	x_rot, y_rot �� AffectContainer�� ����

PreviewButton:
	__OnPreviewMode:
	__RestoreViewMode:

�ǹ� �μ���:
	uiTarget.TargetBoard.__OnDestroyBuilding
		net.SendChatPacket("/build d vid")
	"""

