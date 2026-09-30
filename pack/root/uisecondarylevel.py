import ui
import mouseModule
import net
import uiCommon
import localeInfo
import player
import item
import dbg
import snd
import app
import uiScriptLocale
import sndLevelMgr
import grp
from _weakref import proxy

import uiToolTip
AFFECT_DICT = uiToolTip.ItemToolTip.AFFECT_DICT_POLY


LEVEL_NAMES = {
	0 : localeInfo.SECONDARY_LEVEL_0,
	1 : localeInfo.SECONDARY_LEVEL_1,
	2 : localeInfo.SECONDARY_LEVEL_2,
	3 : localeInfo.SECONDARY_LEVEL_3,
	4 : localeInfo.SECONDARY_LEVEL_4,
	5 : localeInfo.SECONDARY_LEVEL_5,
	6 : localeInfo.SECONDARY_LEVEL_6,
	7 : localeInfo.SECONDARY_LEVEL_7,
	8 : localeInfo.SECONDARY_LEVEL_8,
	9 : localeInfo.SECONDARY_LEVEL_9,
	10 : localeInfo.SECONDARY_LEVEL_10,
	11 : localeInfo.SECONDARY_LEVEL_11,
	12 : localeInfo.SECONDARY_LEVEL_12,
	13 : localeInfo.SECONDARY_LEVEL_13,
	14 : localeInfo.SECONDARY_LEVEL_14,
	15 : localeInfo.SECONDARY_LEVEL_15,
	16 : localeInfo.SECONDARY_LEVEL_16,
	17 : localeInfo.SECONDARY_LEVEL_17,
	18 : localeInfo.SECONDARY_LEVEL_18,
	19 : localeInfo.SECONDARY_LEVEL_19,
	20 : localeInfo.SECONDARY_LEVEL_20,
	21 : localeInfo.SECONDARY_LEVEL_21,
	22 : localeInfo.SECONDARY_LEVEL_22,
	23 : localeInfo.SECONDARY_LEVEL_23,
	24 : localeInfo.SECONDARY_LEVEL_24,
	25 : localeInfo.SECONDARY_LEVEL_25,
	26 : localeInfo.SECONDARY_LEVEL_26,
	27 : localeInfo.SECONDARY_LEVEL_27,
	28 : localeInfo.SECONDARY_LEVEL_28,
	29 : localeInfo.SECONDARY_LEVEL_29,
	30 : localeInfo.SECONDARY_LEVEL_30,
}

def GetAffectString(affectType, affectValue):
	if 0 == affectType:
		return None
	if 0 == affectValue:
		return None
	try:
		return AFFECT_DICT[affectType] % (affectValue)
	except TypeError:
		return "UNKNOWN_VALUE[%s] %s" % (affectType, affectValue)
	except KeyError:
		return "UNKNOWN_TYPE[%s] %s" % (affectType, affectValue)
		
def GetSplittedBonus(text, offset = 0):
	index = text.rindex(' ')+1 - offset
	apply = text[:index]
	value = text[index:]
	return (apply, value)

class SecondaryLevelWindow(ui.ScriptWindow):
	def __init__(self):
		self.tooltipItem = None
		self.__questionDialog = None
		self.__loaded = False
		self.__tick = 0
		self.__lockBtnTick = 0
		
		self.actualBonusListImages = []
		self.actualBonusListTypeTexts = []
		self.actualBonusListValueTexts = []
		
		self.nextBonusListImages = []
		self.nextBonusListTypeTexts = []
		self.nextBonusListValueTexts = []
			
		ui.ScriptWindow.__init__(self)
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def SetItemToolTip(self, itemToolTip):
		self.tooltipItem = proxy(itemToolTip)

	def __LoadWindow(self):
		try:
			pythonScriptLoader = ui.PythonScriptLoader()
			pythonScriptLoader.LoadScriptFile(self, "uiscript/secondarylevelwindow.py")
			
			self.requiredItems = self.GetChild("ItemSlot")
			self.actualRank = self.GetChild("actual_rank")
			self.acutalTextRank = self.GetChild("actual_rank_text")
			self.nextRank = self.GetChild("next_rank")
			self.nextTextRank = self.GetChild("next_rank_text")
			
			self.opt1Chance = self.GetChild("option1_chance")
			self.opt1Cost = self.GetChild("option1_cost")
			self.opt1Button = self.GetChild("option1_button")
			
			self.opt2Chance = self.GetChild("option2_chance")
			self.opt2Cost = self.GetChild("option2_cost")
			self.opt2Button = self.GetChild("option2_button")
		except:
			import exception
			exception.Abort("RemoveItemDialog.__LoadWindow.LoadObject")
			
		self.GetChild("board").SetCloseEvent(ui.__mem_func__(self.Close))
		self.requiredItems.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.requiredItems.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		
		self.opt1Button.SetEvent(ui.__mem_func__(self.SelectOption1))
		self.opt2Button.SetEvent(ui.__mem_func__(self.SelectOption2))
			
		y = 47
		for i in xrange(5):
			img = ui.ImageBox()
			img.SetParent(self.GetChild("actual_bonus_list"))
			img.SetPosition(6, y)
			img.Show()
			
			txt = ui.TextLine()
			txt.SetParent(img)
			txt.SetPosition(20, 4)
			txt.SetPackedFontColor(grp.GenerateColor(0.6705, 0.6705, 0.6705, 1.0))
			txt.Show()
			
			txt2 = ui.TextLine()
			txt2.SetParent(img)
			txt2.SetPosition(215, 4)
			txt2.SetHorizontalAlignCenter()
			txt2.SetPackedFontColor(grp.GenerateColor(0.4078, 1.0, 0.4666, 1.0))
			txt2.Show()
			
			self.actualBonusListImages.append(img)
			self.actualBonusListTypeTexts.append(txt)
			self.actualBonusListValueTexts.append(txt2)
			y += 22
			
		y = 47
		for i in xrange(5):
			img = ui.ImageBox()
			img.SetParent(self.GetChild("next_bonus_list"))
			img.SetPosition(6, y)
			img.Show()
			
			txt = ui.TextLine()
			txt.SetParent(img)
			txt.SetPosition(20, 4)
			txt.SetPackedFontColor(grp.GenerateColor(0.6705, 0.6705, 0.6705, 1.0))
			txt.Show()
			
			txt2 = ui.TextLine()
			txt2.SetParent(img)
			txt2.SetPosition(215, 4)
			txt2.SetHorizontalAlignCenter()
			txt2.SetPackedFontColor(grp.GenerateColor(0.4078, 1.0, 0.4666, 1.0))
			txt2.Show()
			
			self.nextBonusListImages.append(img)
			self.nextBonusListTypeTexts.append(txt)
			self.nextBonusListValueTexts.append(txt2)
			y += 22

		self.__loaded = True
		self.Refresh()
		self.Hide()
		
	def SelectOption1(self):
		sndLevelMgr.SendPacket(0, 0)
		self.LockButtons()
		
	def SelectOption2(self):
		sndLevelMgr.SendPacket(0, 1)
		self.LockButtons()
		
	def LockButtons(self):
		self.opt1Button.Disable()
		self.opt1Button.Down()
		self.opt2Button.Disable()
		self.opt2Button.Down()
		self.__lockBtnTick = 200
		
	def Refresh(self):
		try:
			self.actualRank.LoadImage("zaris/secondary_level/level_icons/{}.tga".format(player.GetStatus(244)))
			self.acutalTextRank.SetText(LEVEL_NAMES[player.GetStatus(244)])
			self.nextRank.LoadImage("zaris/secondary_level/level_icons/{}.tga".format(player.GetStatus(244) + 1))
			self.nextTextRank.SetText(LEVEL_NAMES[player.GetStatus(244) + 1])
		except:
			pass

		for i in xrange(5):
			(apply, value) = sndLevelMgr.GetApply(i)
			if apply > 0:
				self.actualBonusListImages[i].LoadImage("zaris/secondary_level/bonus_layer.png")
				affString = GetAffectString(apply, value)
				(spltApply, spltValue) = GetSplittedBonus(affString)
				self.actualBonusListTypeTexts[i].SetText(spltApply)
				self.actualBonusListValueTexts[i].SetText(spltValue)
			else:
				self.actualBonusListImages[i].LoadImage("zaris/secondary_level/bonus_none.png")
				self.actualBonusListTypeTexts[i].SetText(localeInfo.SECONDARY_LEVEL_BONUS_NONE)
				self.actualBonusListValueTexts[i].SetText("")
				
		for i in xrange(5):
			(apply, value) = sndLevelMgr.GetNextApply(i)
			if apply > 0:
				self.nextBonusListImages[i].LoadImage("zaris/secondary_level/bonus_layer.png")
				affString = GetAffectString(apply, value)
				(spltApply, spltValue) = GetSplittedBonus(affString)
				self.nextBonusListTypeTexts[i].SetText(spltApply)
				self.nextBonusListValueTexts[i].SetText(spltValue)
			else:
				self.nextBonusListImages[i].LoadImage("zaris/secondary_level/bonus_none.png")
				self.nextBonusListTypeTexts[i].SetText(localeInfo.SECONDARY_LEVEL_BONUS_NONE)
				self.nextBonusListValueTexts[i].SetText("")
				
		freeSlot = 0
		for i in xrange(1):
			(vnum, count) = sndLevelMgr.GetRequiredItem(i)
			if vnum > 0:
				self.requiredItems.SetItemSlot(freeSlot, vnum, count)
				freeSlot += 1
				
				
		(chance1) = sndLevelMgr.GetChance(0)
		(cost1) = sndLevelMgr.GetRequiredGold(0)
		self.opt1Chance.SetText("{}%".format(chance1))
		self.opt1Cost.SetText("{}".format(localeInfo.NumberToString(cost1)))
		
		self.opt2Chance.SetText("{}%".format(sndLevelMgr.GetChance(1)))
		self.opt2Cost.SetText("{}".format(localeInfo.NumberToString(sndLevelMgr.GetRequiredGold(1))))
				
	def OverInItem(self, slotIndex):
		if self.tooltipItem:
			self.tooltipItem.SetItemToolTip(self.requiredItems.GetItemSlot(slotIndex))

	def OverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def Open(self):
		self.Refresh()
		self.Show()
		
	def Close(self):
		self.Hide()
		
	def OnUpdate(self):
		self.__tick += 1
		if self.__tick >= 20:
			self.__tick = 0
			self.Refresh()
		if self.__lockBtnTick > 0:
			self.__lockBtnTick -= 1
			if self.__lockBtnTick == 0:
				self.opt1Button.Enable()
				self.opt1Button.SetUp()
				self.opt2Button.Enable()
				self.opt2Button.SetUp()
				
	def OnPressEscapeKey(self):
		self.Close()
		return True
