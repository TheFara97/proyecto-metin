import ui
import localeInfo
import chr
import app
import skill
import player
import uiToolTip
import math
import uiCommon
import net
import chat
import emoji
import item
import constInfo

# Define priority lists
PRIORITY_FIRST = {
	chr.NEW_AFFECT_AUTO_HP_RECOVERY,
	chr.NEW_AFFECT_AUTO_SP_RECOVERY,
	chr.AFFECT_ATT_SPEED_POTION,
	chr.AFFECT_MOV_SPEED_POTION,
}  #These elements will appear first
PRIORITY_LAST = {
	chr.NEW_AFFECT_POLYMORPH,
}  # These elements will appear at the end

# Affects that survive player death — kept in the shower even after ClearAffects()
PERSIST_THROUGH_DEATH = {
	chr.AFFECT_JEONGWI,
	chr.AFFECT_GEOMGYEONG,
	chr.AFFECT_CHEONGEUN,
	chr.AFFECT_GWIGEOM,
	chr.AFFECT_JUMAGAP,
	chr.AFFECT_HOSIN,
	chr.AFFECT_BOHO,
	chr.AFFECT_KWAESOK,
	chr.AFFECT_HEUKSIN,
	chr.AFFECT_MUYEONG,
	chr.AFFECT_GICHEON,
	chr.AFFECT_JEUNGRYEOK,
}

if app.ENABLE_MULTI_FARM_BLOCK:
	class MultiFarmImage(ui.ExpandedImageBox):
		def __del__(self):
			ui.ExpandedImageBox.__del__(self)
		def Destroy(self):
			self.multiFarmStatus = 0
			self.players = []
		def __init__(self):
			ui.ExpandedImageBox.__init__(self)
			self.Destroy()
		def SetMultiFarmInfo(self, multiFarmStatus):
			self.multiFarmStatus = multiFarmStatus
			if multiFarmStatus:
				self.LoadImage("elvion/game/multifarm_block/can_farm.tga")
			else:
				self.LoadImage("elvion/game/multifarm_block/cant_farm.tga")
		def OnMouseOverIn(self):
			interface = constInfo.GetInterfaceInstance()
			if interface != None:
				if interface.tooltipItem:
					interface.tooltipItem.ClearToolTip()
					interface.tooltipItem.AutoAppendTextLine("|cffF1E6C0"+localeInfo.MULTI_FARM_TITLE)
					interface.tooltipItem.AppendSpace(5)
					interface.tooltipItem.AutoAppendTextLine(localeInfo.MULTI_FARM_TEXT)
					interface.tooltipItem.AppendSpace(5)
					for name in self.players:
						interface.tooltipItem.AutoAppendTextLine("|cffF1E6C0"+name)
					interface.tooltipItem.ShowToolTip()
		def OnMouseOverOut(self):
			interface = constInfo.GetInterfaceInstance()
			if interface != None:
				if interface.tooltipItem:
					interface.tooltipItem.HideToolTip()

class ItemAffectImage(ui.ExpandedImageBox):
	def __init__(self):
		ui.ExpandedImageBox.__init__(self)

		self.toolTip = uiToolTip.ItemToolTip(100)
		self.toolTip.HideToolTip()

	def OnMouseOverIn(self):
		self.toolTip.ShowToolTip()

	def OnMouseOverOut(self):
		self.toolTip.HideToolTip()

class LovePointImage(ui.ExpandedImageBox):

	FILE_PATH = "d:/ymir work/ui/pattern/LovePoint/"
	FILE_DICT = {
		0 : FILE_PATH + "01.dds",
		1 : FILE_PATH + "02.dds",
		2 : FILE_PATH + "02.dds",
		3 : FILE_PATH + "03.dds",
		4 : FILE_PATH + "04.dds",
		5 : FILE_PATH + "05.dds",
	}

	def __init__(self):
		ui.ExpandedImageBox.__init__(self)

		self.loverName = ""
		self.lovePoint = 0

		self.toolTip = uiToolTip.ToolTip(100)
		self.toolTip.HideToolTip()

	def __del__(self):
		ui.ExpandedImageBox.__del__(self)

	def SetLoverInfo(self, name, lovePoint):
		self.loverName = name
		self.lovePoint = lovePoint
		self.__Refresh()

	def OnUpdateLovePoint(self, lovePoint):
		self.lovePoint = lovePoint
		self.__Refresh()

	def __Refresh(self):
		self.lovePoint = max(0, self.lovePoint)
		self.lovePoint = min(100, self.lovePoint)

		if 0 == self.lovePoint:
			loveGrade = 0
		else:
			loveGrade = self.lovePoint / 25 + 1
		fileName = self.FILE_DICT.get(loveGrade, self.FILE_PATH+"00.dds")

		try:
			self.LoadImage(fileName)
		except:
			import dbg
			dbg.TraceError("LovePointImage.SetLoverInfo(lovePoint=%d) - LoadError %s" % (self.lovePoint, fileName))

		self.SetScale(0.7, 0.7)

		self.toolTip.ClearToolTip()
		self.toolTip.SetTitle(self.loverName)
		self.toolTip.AppendTextLine(localeInfo.AFF_LOVE_POINT % (self.lovePoint))
		self.toolTip.ResizeToolTip()

	def OnMouseOverIn(self):
		self.toolTip.ShowToolTip()
		if app.ENABLE_RENEWAL_AFFECT_SHOWER:
			self.SetScale(0.9,0.9)

	def OnMouseOverOut(self):
		self.toolTip.HideToolTip()
		if app.ENABLE_RENEWAL_AFFECT_SHOWER:
			self.SetScale(0.7, 0.7)


class HorseImage(ui.ExpandedImageBox):

	FILE_PATH = "d:/ymir work/ui/pattern/HorseState/"

	FILE_DICT = {
		00 : FILE_PATH+"00.dds",
		01 : FILE_PATH+"00.dds",
		02 : FILE_PATH+"00.dds",
		03 : FILE_PATH+"00.dds",
		10 : FILE_PATH+"10.dds",
		11 : FILE_PATH+"11.dds",
		12 : FILE_PATH+"12.dds",
		13 : FILE_PATH+"13.dds",
		20 : FILE_PATH+"20.dds",
		21 : FILE_PATH+"21.dds",
		22 : FILE_PATH+"22.dds",
		23 : FILE_PATH+"23.dds",
		30 : FILE_PATH+"30.dds",
		31 : FILE_PATH+"31.dds",
		32 : FILE_PATH+"32.dds",
		33 : FILE_PATH+"33.dds",
	}

	def __init__(self):
		ui.ExpandedImageBox.__init__(self)

		self.toolTip = uiToolTip.ToolTip(100)
		self.toolTip.HideToolTip()

	def __GetHorseGrade(self, level):
		if 0 == level:
			return 0

		return (level-1)/10 + 1

	def SetState(self, level, health, battery):
		self.toolTip.ClearToolTip()

		if level>0:

			try:
				grade = self.__GetHorseGrade(level)
				self.__AppendText(localeInfo.LEVEL_LIST[grade])
			except IndexError:
				print "HorseImage.SetState(level=%d, health=%d, battery=%d) - Unknown Index" % (level, health, battery)
				return

			try:
				healthName=localeInfo.HEALTH_LIST[health]
				if len(healthName)>0:
					self.__AppendText(healthName)
			except IndexError:
				print "HorseImage.SetState(level=%d, health=%d, battery=%d) - Unknown Index" % (level, health, battery)
				return

			if health>0:
				if battery==0:
					self.__AppendText(localeInfo.NEEFD_REST)

			try:
				fileName=self.FILE_DICT[health*10+battery]
			except KeyError:
				print "HorseImage.SetState(level=%d, health=%d, battery=%d) - KeyError" % (level, health, battery)

			try:
				self.LoadImage(fileName)
			except:
				print "HorseImage.SetState(level=%d, health=%d, battery=%d) - LoadError %s" % (level, health, battery, fileName)

		self.SetScale(0.7, 0.7)

	def __AppendText(self, text):

		self.toolTip.AppendTextLine(text)


	def OnMouseOverIn(self):

		self.toolTip.ShowToolTip()
		if app.ENABLE_RENEWAL_AFFECT_SHOWER:
			self.SetScale(0.9,0.9)

	def OnMouseOverOut(self):

		self.toolTip.HideToolTip()
		if app.ENABLE_RENEWAL_AFFECT_SHOWER:
			self.SetScale(0.7, 0.7)

class ItemImage(ItemAffectImage):
	def __init__(self, cell):
		ItemAffectImage.__init__(self)

		self.cell = cell

	def SetImage(self, filename):
		self.LoadImage(filename)

		self.SetScale(0.7, 0.7)

	def Update(self):
		if not self.toolTip.IsShow():
			return

		self.toolTip.ClearToolTip()
		self.toolTip.SetInventoryItem(self.cell)

	def OnMouseOverIn(self):
		ItemAffectImage.OnMouseOverIn(self)
		self.Update()


class AutoPotionImage(ItemImage):
	FILE_PATH_HP = "d:/ymir work/ui/pattern/auto_hpgauge/"
	FILE_PATH_SP = "d:/ymir work/ui/pattern/auto_spgauge/"

	def __init__(self, subType, cell):
		ItemImage.__init__(self, cell)

		self.subType = subType
		self.oldGrade = -1

	def Update(self):
		itemVnum = player.GetItemIndex(player.INVENTORY, self.cell)
		if itemVnum == 0:
			return

		item.SelectItem(itemVnum)
		metinSocket = [player.GetItemMetinSocket(self.cell, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]	

		totalAmount = metinSocket[2]
		usedAmount = metinSocket[1]
		currentAmount = totalAmount - usedAmount

		if 0 == totalAmount:
			totalAmount = 100
			usedAmount = 0
			currentAmount = 100

		amountPercent = 100 * currentAmount / totalAmount

		if self.subType == item.TOGGLE_AUTO_RECOVERY_HP:
			path = self.FILE_PATH_HP
		else:
			path = self.FILE_PATH_SP

		grade = self.__GetGradeFromPercent(amountPercent)
		if self.oldGrade != grade:
			fileName = "%s%.2d.dds" % (path, grade)

			print("AutoPotion: %d %d %s", self.subType, amountPercent, fileName)

			try:
				self.SetImage(fileName)
			except RuntimeError:
				import dbg
				dbg.TraceError("Failed to load auto-potion image %s" % fileName)

			self.oldGrade = grade

		ItemImage.Update(self)

	def __GetGradeFromPercent(self, percent):
		if percent > 80:
			return 5
		if percent > 60:
			return 4
		if percent > 40:
			return 3
		if percent > 20:
			return 2

		return 1

class ToggleItemImage(ItemImage):
	def __init__(self, cell):
		ItemImage.__init__(self, cell)

	def Update(self):
		itemVnum = player.GetItemIndex(player.INVENTORY, self.cell)
		if itemVnum == 0:
			return

		item.SelectItem(itemVnum)
		self.SetImage(item.GetIconImageFileName())
		
		ItemImage.Update(self)

if app.ENABLE_RENEWAL_AFFECT_SHOWER:
	class AffectImage(ui.ExpandedImageBox):
		NORMAL_COLOR = 0xffC5C7C4
		BONUS_COLOR = 0xff95A693
		WHITE_COLOR = 0xffffffff
		def __init__(self):
			ui.ExpandedImageBox.__init__(self)
			self.toolTip = uiToolTip.ToolTip()
			self.toolTip.HideToolTip()
			self.isToolTipVisible = False 
			self.skillIndex = None
			self.priority = 0
			self.Canberemoved = False
			self.isSkillAffect = True
			self.description = None
			self.endTime = 0
			self.affect = None
			self.affectValue = 0  # Store the actual affect value

		def SetCanberemoved(self, Canberemoved):
			self.Canberemoved = Canberemoved

		def SetAffectValue(self, value):
			self.affectValue = value

		def SetPriority(self, priority):
			self.priority = priority

		def GetPriority(self):
			return self.priority

		def SetAffect(self, affect):
			self.affect = affect

		def GetAffect(self):
			return self.affect

		def SetToolTipText(self, text):
			if hasattr(self, "currentToolTipText") and self.currentToolTipText == text:
				return

			self.toolTip.ClearToolTip()
			self.toolTip.AutoAppendTextLine(text, self.WHITE_COLOR)
			if self.Canberemoved:
				self.toolTip.AutoAppendTextLine(localeInfo.AFFECTSHOWER_REMOVE_TEXT, self.NORMAL_COLOR)
			self.toolTip.AlignHorizonalCenter()
			self.toolTip.ResizeToolTip()
			self.currentToolTipText = text

		def SetDescription(self, description):
			self.description = description
			self.UpdateDescription()

		def SetDuration(self, duration):
			self.endTime = 0
			if duration > 0:
				self.endTime = app.GetGlobalTimeStamp() + duration
				self.UpdateDescription()

		def UpdateAutoPotionDescription(self):
			if not self.toolTip:
				return

			potionType = 0
			if self.affect == chr.NEW_AFFECT_AUTO_HP_RECOVERY:
				potionType = player.AUTO_POTION_TYPE_HP
				PotionName = localeInfo.TOOLTIP_AUTO_POTION_HP
			else:
				potionType = player.AUTO_POTION_TYPE_SP
				PotionName = localeInfo.TOOLTIP_AUTO_POTION_SP

			isActivated, currentAmount, totalAmount, slotIndex = player.GetAutoPotionInfo(potionType)

			amountPercent = 0.0

			try:
				amountPercent = (float(currentAmount) / totalAmount) * 100.0
			except:
				amountPercent = 100.0

			self.toolTip.ClearToolTip()
			self.toolTip.AutoAppendTextLine(PotionName, self.WHITE_COLOR)
			self.toolTip.AutoAppendTextLine(self.description % amountPercent, self.NORMAL_COLOR)

			self.toolTip.AlignHorizonalCenter()
			self.toolTip.ResizeToolTip()

		def FormatTime(self, time):
			if time < 0:
				return "0s"

			(d, remainder) = divmod(time, 86400)
			(h, remainder) = divmod(remainder, 3600)
			(m, s) = divmod(remainder, 60)

			time_parts = [
				"{}d".format(d) if d > 0 else "",
				"{}h".format(h) if h > 0 else "",
				"{}m".format(m) if m > 0 else "",
				"{}s".format(s) if s > 0 else "",
			]

			formatted_time = " ".join(filter(None, time_parts))
			return formatted_time

		def UpdateDescription(self):
			if not self.toolTip:
				return

			if not self.description:
				return

			self.toolTip.ClearToolTip()

			self.toolTip.AutoAppendTextLine(self.description, self.WHITE_COLOR)
			if self.endTime > 0:
				remaining_time = self.endTime - app.GetGlobalTimeStamp()
				leftTime = self.FormatTime(remaining_time)
				timeText = "(%s : %s)" % (localeInfo.LEFT_TIME, leftTime)
				self.toolTip.AutoAppendTextLine(timeText, self.NORMAL_COLOR)

			if self.Canberemoved:
				self.toolTip.AutoAppendTextLine(localeInfo.AFFECTSHOWER_REMOVE_TEXT, self.NORMAL_COLOR)

			self.toolTip.AlignHorizonalCenter()
			self.toolTip.ResizeToolTip()
			
		if app.ENABLE_VIP_SYSTEM: 
			def OnConvertVIPQuestionDialog(self):
				import uiCommon
				self.convertVIPQuestionDialog = uiCommon.QuestionDialog()
				self.convertVIPQuestionDialog.SetText(localeInfo.VIP_CONVERT_QUESTION)
				self.convertVIPQuestionDialog.SetWidth(350)
				self.convertVIPQuestionDialog.SetAcceptEvent(lambda arg = TRUE: self.ConvertVIPQuestionDialog(arg))
				self.convertVIPQuestionDialog.SetCancelEvent(lambda arg = FALSE: self.ConvertVIPQuestionDialog(arg))
				self.convertVIPQuestionDialog.Open()
		
			def ConvertVIPQuestionDialog(self, answer):
				if not self.convertVIPQuestionDialog:
					return
	
				self.convertVIPQuestionDialog.Close()
				self.convertVIPQuestionDialog = None
	
				if not answer:
					return
			
				net.SendChatPacket("/convert_vip_affect")
				return TRUE

		def OnMouseLeftButtonDown(self):
			if app.ENABLE_VIP_SYSTEM and chr.NEW_AFFECT_VIP == self.affect: 
				self.OnConvertVIPQuestionDialog()
				return

		def SetSkillIndex(self, skillIndex):
			self.skillIndex = skillIndex

		def UpdateSkillDescription(self):
			if self.skillIndex is None:
				return
		
			self.toolTip.ClearToolTip()
		
			skillName = skill.GetSkillName(self.skillIndex)
			self.toolTip.AutoAppendTextLine(skillName, self.WHITE_COLOR)
		
			# If we have a stored affect value, create custom descriptions
			if self.affectValue > 0:
				try:
					# Create custom descriptions based on skill type and actual value
					if self.skillIndex == chr.SKILL_HOSIN:  # Blessing
						desc = localeInfo.TOOLTIP_APPLY_MAX_HP_PCT % int(self.affectValue)
						self.toolTip.AutoAppendTextLine(desc, self.BONUS_COLOR)
					elif self.skillIndex == chr.SKILL_GICHEON:  # Reflect
						desc = localeInfo.TOOLTIP_APPLY_REFLECT_MELEE % int(self.affectValue)
						self.toolTip.AutoAppendTextLine(desc, self.BONUS_COLOR)
					elif self.skillIndex == chr.SKILL_JEUNGRYEOK:  # Strength
						desc = localeInfo.TOOLTIP_APPLY_ATT_GRADE_BONUS % int(self.affectValue)
						self.toolTip.AutoAppendTextLine(desc, self.BONUS_COLOR)
					elif self.skillIndex == chr.SKILL_KWAESOK:  # Swiftness
						desc = localeInfo.TOOLTIP_APPLY_ATT_SPEED % int(self.affectValue)
						self.toolTip.AutoAppendTextLine(desc, self.BONUS_COLOR)
					elif self.skillIndex == chr.SKILL_BOHO:  # Protection
						desc = localeInfo.TOOLTIP_APPLY_DEF_GRADE_BONUS % int(self.affectValue)
						self.toolTip.AutoAppendTextLine(desc, self.BONUS_COLOR)
					elif self.skillIndex == chr.SKILL_JUMAGAP:  # Magic Defense
						desc = localeInfo.TOOLTIP_APPLY_MAGIC_DEF_BONUS % int(self.affectValue)
						self.toolTip.AutoAppendTextLine(desc, self.BONUS_COLOR)
					else:
						desc = "%s +%d" % (skillName, int(self.affectValue))
						self.toolTip.AutoAppendTextLine(desc, self.BONUS_COLOR)
				except:
					desc = "%s: +%d" % (skillName, int(self.affectValue))
					self.toolTip.AutoAppendTextLine(desc, self.BONUS_COLOR)
			else:
				slotIndex = player.GetSkillSlotIndex(self.skillIndex)
				if slotIndex != -1:
					skillCurrentPercentage = player.GetSkillCurrentEfficientPercentage(slotIndex)
					for i in xrange(skill.GetSkillAffectDescriptionCount(self.skillIndex)):
						description = skill.GetSkillAffectDescription(self.skillIndex, i, skillCurrentPercentage)
						self.toolTip.AutoAppendTextLine(description, self.BONUS_COLOR)
		
			if self.endTime > 0:
				remaining_time = self.endTime - app.GetGlobalTimeStamp()
				leftTime = self.FormatTime(remaining_time)
				timeText = "(%s : %s)" % (localeInfo.LEFT_TIME, leftTime)
				self.toolTip.AutoAppendTextLine(timeText, self.NORMAL_COLOR)
		
			if self.Canberemoved:
				self.toolTip.AutoAppendTextLine(localeInfo.AFFECTSHOWER_REMOVE_TEXT, self.NORMAL_COLOR)
		
			self.toolTip.AlignHorizonalCenter()
			self.toolTip.ResizeToolTip()

		def SetSkillAffectFlag(self, flag):
			self.isSkillAffect = flag

		def IsSkillAffect(self):
			return self.isSkillAffect

		def OnMouseOverIn(self):
			self.SetScale(0.9,0.9)
			if self.toolTip:
				self.toolTip.ShowToolTip()
				self.isToolTipVisible = True

		def OnMouseOverOut(self):
			self.SetScale(0.7,0.7)
			if self.toolTip:
				self.toolTip.HideToolTip()
				self.isToolTipVisible = False
else:
	class AffectImage(ui.ExpandedImageBox):

		def __init__(self):
			ui.ExpandedImageBox.__init__(self)

			self.toolTipText = None
			self.isSkillAffect = True
			self.description = None
			self.endTime = 0
			self.affect = None
			self.isClocked = True

		def SetAffect(self, affect):
			self.affect = affect

		def GetAffect(self):
			return self.affect

		def SetToolTipText(self, text, x = 0, y = -19):

			if not self.toolTipText:
				textLine = ui.TextLine()
				textLine.SetParent(self)
				textLine.SetSize(0, 0)
				textLine.SetOutline()
				textLine.Hide()
				self.toolTipText = textLine

			self.toolTipText.SetText(text)
			w, h = self.toolTipText.GetTextSize()
			if localeInfo.IsARABIC():
				self.toolTipText.SetPosition(w+20, y)
			else:
				self.toolTipText.SetPosition(max(0, x + self.GetWidth()/2 - w/2), y)

		def SetDescription(self, description):
			self.description = description

		def SetDuration(self, duration):
			self.endTime = 0
			if duration > 0:
				self.endTime = app.GetGlobalTimeStamp() + duration

		def UpdateAutoPotionDescription(self):

			potionType = 0
			if self.affect == chr.NEW_AFFECT_AUTO_HP_RECOVERY:
				potionType = player.AUTO_POTION_TYPE_HP
			else:
				potionType = player.AUTO_POTION_TYPE_SP

			isActivated, currentAmount, totalAmount, slotIndex = player.GetAutoPotionInfo(potionType)

			#print "UpdateAutoPotionDescription ", isActivated, currentAmount, totalAmount, slotIndex

			amountPercent = 0.0

			try:
				amountPercent = (float(currentAmount) / totalAmount) * 100.0
			except:
				amountPercent = 100.0

			self.SetToolTipText(self.description % amountPercent, 0, 40)

		def SetClock(self, isClocked):
			self.isClocked = isClocked

		def UpdateDescription(self):
			if not self.isClocked:
				self.__UpdateDescription2()
				return

			if not self.description:
				return

			toolTip = self.description
			if self.endTime > 0:
				leftTime = localeInfo.SecondToDHM(self.endTime - app.GetGlobalTimeStamp())
				toolTip += " (%s : %s)" % (localeInfo.LEFT_TIME, leftTime)
			self.SetToolTipText(toolTip, 0, 40)

		def __UpdateDescription2(self):
			if not self.description:
				return

			toolTip = self.description
			self.SetToolTipText(toolTip, 0, 40)

		def SetSkillAffectFlag(self, flag):
			self.isSkillAffect = flag

		def IsSkillAffect(self):
			return self.isSkillAffect

		def OnMouseOverIn(self):
			if self.toolTipText:
				self.toolTipText.Show()

		def OnMouseOverOut(self):
			if self.toolTipText:
				self.toolTipText.Hide()

class AffectShower(ui.Window):

	MALL_DESC_IDX_START = 1000
	if app.ENABLE_RENEWAL_AFFECT_SHOWER:
		##changes to organize the list if you use the display system waters, dew and blessings from quin
		WATER_DESC_IDX_START = 1200
		DEW_DESC_IDX_START = 1400
		DEW_RANGE_OFFSET = 200
		TOOLTIP_UPDATE_INTERVAL = 500
		MAX_ITEMS_PER_ROW = 10
	FISH_DESC_IDX_START = 800
	DRAGON_GOD_DESC_IDX_START = 1600
	IMAGE_STEP = 25
	AFFECT_MAX_NUM = 32

	INFINITE_AFFECT_DURATION = 0x1FFFFFFF

	AFFECT_DATA_DICT =	{
			chr.AFFECT_POISON : (localeInfo.SKILL_TOXICDIE, "d:/ymir work/ui/skill/common/affect/poison.sub"),
			chr.AFFECT_SLOW : (localeInfo.SKILL_SLOW, "d:/ymir work/ui/skill/common/affect/slow.sub"),
			chr.AFFECT_STUN : (localeInfo.SKILL_STUN, "d:/ymir work/ui/skill/common/affect/stun.sub"),

			chr.AFFECT_ATT_SPEED_POTION : (localeInfo.SKILL_INC_ATKSPD, "d:/ymir work/ui/skill/common/affect/Increase_Attack_Speed.sub"),
			chr.AFFECT_MOV_SPEED_POTION : (localeInfo.SKILL_INC_MOVSPD, "d:/ymir work/ui/skill/common/affect/Increase_Move_Speed.sub"),
			chr.AFFECT_FISH_MIND : (localeInfo.SKILL_FISHMIND, "d:/ymir work/ui/skill/common/affect/fishmind.sub"),
			chr.AFFECT_PREMIUM_MINE : (localeInfo.TOOLTIP_MALL_PREMIUM_MINE, "d:/ymir work/ui/skill/common/affect/ruda.tga"),

			chr.AFFECT_JEONGWI : (localeInfo.SKILL_JEONGWI, "d:/ymir work/ui/skill/warrior/jeongwi_03.sub",),
			chr.AFFECT_GEOMGYEONG : (localeInfo.SKILL_GEOMGYEONG, "d:/ymir work/ui/skill/warrior/geomgyeong_03.sub",),
			chr.AFFECT_CHEONGEUN : (localeInfo.SKILL_CHEONGEUN, "d:/ymir work/ui/skill/warrior/cheongeun_03.sub",),
			chr.AFFECT_GYEONGGONG : (localeInfo.SKILL_GYEONGGONG, "d:/ymir work/ui/skill/assassin/gyeonggong_03.sub",),
			chr.AFFECT_EUNHYEONG : (localeInfo.SKILL_EUNHYEONG, "d:/ymir work/ui/skill/assassin/eunhyeong_03.sub",),
			chr.AFFECT_GWIGEOM : (localeInfo.SKILL_GWIGEOM, "d:/ymir work/ui/skill/sura/gwigeom_03.sub",),
			chr.AFFECT_GONGPO : (localeInfo.SKILL_GONGPO, "d:/ymir work/ui/skill/sura/gongpo_03.sub",),
			chr.AFFECT_JUMAGAP : (localeInfo.SKILL_JUMAGAP, "d:/ymir work/ui/skill/sura/jumagap_03.sub"),
			chr.AFFECT_HOSIN : (localeInfo.SKILL_HOSIN, "d:/ymir work/ui/skill/shaman/hosin_03.sub",),
			chr.AFFECT_BOHO : (localeInfo.SKILL_BOHO, "d:/ymir work/ui/skill/shaman/boho_03.sub",),
			chr.AFFECT_KWAESOK : (localeInfo.SKILL_KWAESOK, "d:/ymir work/ui/skill/shaman/kwaesok_03.sub",),
			chr.AFFECT_HEUKSIN : (localeInfo.SKILL_HEUKSIN, "d:/ymir work/ui/skill/sura/heuksin_03.sub",),
			chr.AFFECT_MUYEONG : (localeInfo.SKILL_MUYEONG, "d:/ymir work/ui/skill/sura/muyeong_03.sub",),
			chr.AFFECT_GICHEON : (localeInfo.SKILL_GICHEON, "d:/ymir work/ui/skill/shaman/gicheon_03.sub",),
			chr.AFFECT_JEUNGRYEOK : (localeInfo.SKILL_JEUNGRYEOK, "d:/ymir work/ui/skill/shaman/jeungryeok_03.sub",),

			chr.AFFECT_PABEOP : (localeInfo.SKILL_PABEOP, "d:/ymir work/ui/skill/sura/pabeop_03.sub",),
			chr.AFFECT_FALLEN_CHEONGEUN : (localeInfo.SKILL_CHEONGEUN, "d:/ymir work/ui/skill/warrior/cheongeun_03.sub",),
			28 : (localeInfo.SKILL_FIRE, "d:/ymir work/ui/skill/sura/hwayeom_03.sub",),
			chr.AFFECT_CHINA_FIREWORK : (localeInfo.SKILL_POWERFUL_STRIKE, "d:/ymir work/ui/skill/common/affect/powerfulstrike.sub",),

			chr.NEW_AFFECT_EXP_BONUS : (localeInfo.TOOLTIP_MALL_EXPBONUS_STATIC, "d:/ymir work/ui/game/affectshower/pdvip.tga",),
			chr.NEW_AFFECT_ITEM_BONUS : (localeInfo.TOOLTIP_MALL_ITEMBONUS_STATIC_NEW, "d:/ymir work/ui/game/affectshower/itemvip.tga",),

			chr.NEW_AFFECT_SAFEBOX : (localeInfo.TOOLTIP_MALL_SAFEBOX, "d:/ymir work/ui/skill/common/affect/magazyn.tga",),
			chr.NEW_AFFECT_AUTOLOOT : (localeInfo.TOOLTIP_MALL_AUTOLOOT, "d:/ymir work/ui/skill/common/affect/trzecia_reka.tga",),
			chr.NEW_AFFECT_FISH_MIND : (localeInfo.TOOLTIP_MALL_FISH_MIND, "d:/ymir work/ui/skill/common/affect/ryba.tga",),
			chr.NEW_AFFECT_MARRIAGE_FAST : (localeInfo.TOOLTIP_MALL_MARRIAGE_FAST, "d:/ymir work/ui/skill/common/affect/marriage_fast.sub",),
			chr.NEW_AFFECT_GOLD_BONUS : (localeInfo.TOOLTIP_MALL_GOLDBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/moneta.tga",),
			chr.AFFECT_PICKUP : (localeInfo.TOOLTIP_MALL_PICKUP, "d:/ymir work/ui/skill/common/affect/auto_podnoszenie.tga",),
			chr.NEW_AFFECT_PREMIUM_MINE : (localeInfo.TOOLTIP_MALL_PREMIUM_MINE, "d:/ymir work/ui/skill/common/affect/ruda.tga",),

			chr.NEW_AFFECT_NO_DEATH_PENALTY : (localeInfo.TOOLTIP_APPLY_NO_DEATH_PENALTY, "d:/ymir work/ui/skill/common/affect/gold_premium.sub"),
			chr.NEW_AFFECT_SKILL_BOOK_BONUS : (localeInfo.TOOLTIP_APPLY_SKILL_BOOK_BONUS, "d:/ymir work/ui/game/affectshower/rada.tga"),
			chr.NEW_AFFECT_SKILL_BOOK_NO_DELAY : (localeInfo.TOOLTIP_APPLY_SKILL_BOOK_NO_DELAY, "d:/ymir work/ui/game/affectshower/egzo.tga"),
			
			chr.NEW_AFFECT_SKILE_PAS_BOOK_BONUS : (localeInfo.TOOLTIP_APPLY_SKILE_PAS_BOOK_BONUS, "d:/ymir work/ui/game/affectshower/rada0.tga"),
			chr.NEW_AFFECT_SKILE_PAS_NO_BOOK_DELAY : (localeInfo.TOOLTIP_APPLY_SKILE_PAS_NO_BOOK_DELAY, "d:/ymir work/ui/game/affectshower/egzo0.tga"),
			
			chr.NEW_AFFECT_SKILE_PET_BOOK_BONUS : (localeInfo.TOOLTIP_APPLY_SKILE_PET_BOOK_BONUS, "d:/ymir work/ui/game/affectshower/rada2.tga"),
			chr.NEW_AFFECT_SKILE_PET_NO_BOOK_DELAY : (localeInfo.TOOLTIP_APPLY_SKILE_PET_NO_BOOK_DELAY, "d:/ymir work/ui/game/affectshower/egzo2.tga"),
			
			chr.NEW_AFFECT_SKILE_BUFF_BOOK_BONUS : (localeInfo.TOOLTIP_APPLY_SKILE_BUFF_BOOK_BONUS, "d:/ymir work/ui/game/affectshower/rada1.tga"),
			chr.NEW_AFFECT_SKILE_BUFF_NO_BOOK_DELAY : (localeInfo.TOOLTIP_APPLY_SKILE_BUFF_NO_BOOK_DELAY, "d:/ymir work/ui/game/affectshower/egzo1.tga"),

			chr.NEW_AFFECT_AUTO_HP_RECOVERY : (localeInfo.TOOLTIP_AUTO_POTION_REST, "d:/ymir work/ui/pattern/auto_hpgauge/05.dds"),
			chr.NEW_AFFECT_AUTO_SP_RECOVERY : (localeInfo.TOOLTIP_AUTO_POTION_REST, "d:/ymir work/ui/pattern/auto_spgauge/05.dds"),

			551 : (localeInfo.TOOLTIP_PASYWKI_MASTER, "d:/ymir work/ui/game/affectshower/pasywki.tga"),


			MALL_DESC_IDX_START+player.POINT_MALL_ATTBONUS : (localeInfo.TOOLTIP_MALL_ATTBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/att_bonus.sub",),
			MALL_DESC_IDX_START+player.POINT_MALL_DEFBONUS : (localeInfo.TOOLTIP_MALL_DEFBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/def_bonus.sub",),
			MALL_DESC_IDX_START+player.POINT_MALL_EXPBONUS : (localeInfo.TOOLTIP_MALL_EXPBONUS, "d:/ymir work/ui/game/affectshower/exp.tga",),
			MALL_DESC_IDX_START+player.POINT_MALL_ITEMBONUS : (localeInfo.TOOLTIP_MALL_ITEMBONUS_STATIC_NEW, "elvion/game/affects/thief_gloves.png",),
			MALL_DESC_IDX_START+player.POINT_MALL_GOLDBONUS : (localeInfo.TOOLTIP_MALL_GOLDBONUS, "d:/ymir work/ui/skill/common/affect/gold_bonus.sub",),
			MALL_DESC_IDX_START+player.POINT_CRITICAL_PCT : (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT,"d:/ymir work/ui/skill/common/affect/critical.sub"),
			MALL_DESC_IDX_START+player.POINT_PENETRATE_PCT : (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, "d:/ymir work/ui/skill/common/affect/gold_premium.sub"),
			MALL_DESC_IDX_START+player.POINT_MAX_HP_PCT : (localeInfo.TOOLTIP_MAX_HP_PCT, "d:/ymir work/ui/skill/common/affect/gold_premium.sub"),
			MALL_DESC_IDX_START+player.POINT_MAX_SP_PCT : (localeInfo.TOOLTIP_MAX_SP_PCT, "d:/ymir work/ui/skill/common/affect/gold_premium.sub"),

			MALL_DESC_IDX_START+player.POINT_PC_BANG_EXP_BONUS : (localeInfo.TOOLTIP_MALL_EXPBONUS_P_STATIC, "d:/ymir work/ui/skill/common/affect/EXP_Bonus_p_on.sub",),
			MALL_DESC_IDX_START+player.POINT_PC_BANG_DROP_BONUS: (localeInfo.TOOLTIP_MALL_ITEMBONUS_P_STATIC, "d:/ymir work/ui/skill/common/affect/Item_Bonus_p_on.sub",),
	}
	if app.ENABLE_DRAGON_SOUL_SYSTEM:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_DRAGON_SOUL_DECK1] = (localeInfo.TOOLTIP_DRAGON_SOUL_DECK1, "d:/ymir work/ui/dragonsoul/buff_ds_sky1.tga")
		AFFECT_DATA_DICT[chr.NEW_AFFECT_DRAGON_SOUL_DECK2] = (localeInfo.TOOLTIP_DRAGON_SOUL_DECK2, "d:/ymir work/ui/dragonsoul/buff_ds_land1.tga")
		
	AFFECT_DATA_DICT[chr.AFFECT_NEW_PET] = (localeInfo.TOOLTIP_BONUS_PET, "elvion/game/affects/bonus_pet.png")
	AFFECT_DATA_DICT[chr.AFFECT_PASYWKI_EXTRABONUS] = (localeInfo.TOOLTIP_BONUS_PASSIVE_SKILLS, "elvion/game/affects/bonus_passive_skills.png")
	
	if app.ENABLE_MULTI_FARM_BLOCK:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_MULTI_FARM] =  (localeInfo.MULTI_FARM_PREMIUM_EFFECT, "elvion/game/multifarm_block/can_farm_premimum.tga")

	if app.ENABLE_AFFECT_POLYMORPH_REMOVE:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_POLYMORPH] = (localeInfo.POLYMORPH_AFFECT_TOOLTIP, "d:/ymir work/ui/polymorph_marble_icon.tga")

	if app.__AUTO_QUQUE_ATTACK__:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_AUTO_METIN_FARM] =  (localeInfo.NEW_AFFECT_AUTO_METIN_FARM, "d:/ymir work/ui/game/affectshower/61400.tga")

	if app.ENABLE_USE_FISH_SYSTEM:
		AFFECT_DATA_DICT[812] = (localeInfo.TOOLTIP_STR, "d:/ymir work/ui/game/affectshower/80400.tga")
		AFFECT_DATA_DICT[848] = (localeInfo.TOOLTIP_APPLY_ATTBONUS_DEVIL, "d:/ymir work/ui/game/affectshower/80401.tga")
		AFFECT_DATA_DICT[815] = (localeInfo.TOOLTIP_INT, "d:/ymir work/ui/game/affectshower/80402.tga")
		AFFECT_DATA_DICT[814] = (localeInfo.TOOLTIP_DEX, "d:/ymir work/ui/game/affectshower/80403.tga")
		AFFECT_DATA_DICT[853] = (localeInfo.TOOLTIP_APPLY_ATTBONUS_MONSTER, "d:/ymir work/ui/game/affectshower/80404.tga")
		AFFECT_DATA_DICT[922] = (localeInfo.TOOLTIP_NORMAL_HIT_DAMAGE_BONUS, "d:/ymir work/ui/game/affectshower/80405.tga")
		AFFECT_DATA_DICT[883] = (localeInfo.TOOLTIP_MALL_EXPBONUS, "d:/ymir work/ui/game/affectshower/80406.tga")
		AFFECT_DATA_DICT[895] = (localeInfo.TOOLTIP_ATT_GRADE, "d:/ymir work/ui/game/affectshower/80407.tga")
		AFFECT_DATA_DICT[952] = (localeInfo.TOOLTIP_APPLY_ATTBONUS_BOSS, "d:/ymir work/ui/game/affectshower/27804.tga")
		AFFECT_DATA_DICT[954] = (localeInfo.TOOLTIP_APPLY_ATTBONUS_STONE, "d:/ymir work/ui/game/affectshower/80418.tga")
		AFFECT_DATA_DICT[964] = (localeInfo.TOOLTIP_APPLY_FINAL_DMG_BONUS, "d:/ymir work/ui/game/affectshower/80419.tga")
		AFFECT_DATA_DICT[956] = (localeInfo.TOOLTIP_APPLY_ATTBONUS_WLADCA, "d:/ymir work/ui/game/affectshower/80420.tga")
		AFFECT_DATA_DICT[951] = (localeInfo.TOOLTIP_APPLY_RESIST_HUMAN, "d:/ymir work/ui/game/affectshower/80421.tga")
		AFFECT_DATA_DICT[951] = (localeInfo.TOOLTIP_APPLY_RESIST_HUMAN, "d:/ymir work/ui/game/affectshower/80421.tga")
		AFFECT_DATA_DICT[1295] = (localeInfo.TOOLTIP_ATT_GRADE, "d:/ymir work/ui/game/affectshower/50841.tga")

	AFFECT_DATA_DICT[746] = (localeInfo.RUNE_DECK5_TOOLTIP, "d:/ymir work/ui/game/affectshower/runy.tga")
	AFFECT_DATA_DICT[747] = (localeInfo.RUNE_DECK9_TOOLTIP, "d:/ymir work/ui/game/affectshower/runy.tga")

	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.POINT_PENETRATE_PCT] = (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, 	"d:/ymir work/ui/game/affectshower/50813.tga")
	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.POINT_CRITICAL_PCT] = (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT, 	"d:/ymir work/ui/game/affectshower/50814.tga")
	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.ATT_BONUS] = (localeInfo.TOOLTIP_ATT_GRADE, 			"d:/ymir work/ui/game/affectshower/50817.tga")
	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.DEF_BONUS] = (localeInfo.TOOLTIP_DEF_GRADE, 			"d:/ymir work/ui/game/affectshower/50818.tga")
	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.RESIST_MAGIC] = (localeInfo.TOOLTIP_MAGIC_DEF_GRADE, 		"d:/ymir work/ui/game/affectshower/50819.tga")
	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.ATT_SPEED] = (localeInfo.TOOLTIP_ATT_SPEED, 			"d:/ymir work/ui/game/affectshower/50820.tga")
	
	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.POINT_CRITICAL_PCT] = (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT, 	"d:/ymir work/ui/game/affectshower/50821.tga")
	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.POINT_PENETRATE_PCT] = (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, 	"d:/ymir work/ui/game/affectshower/50822.tga")
	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.ATT_SPEED] = (localeInfo.TOOLTIP_ATT_SPEED, 			"d:/ymir work/ui/game/affectshower/50823.tga")
	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.RESIST_MAGIC] = (localeInfo.TOOLTIP_RESIST_MAGIC, 			"d:/ymir work/ui/game/affectshower/50823.tga")
	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.ATT_BONUS] = (localeInfo.TOOLTIP_ATT_GRADE, 			"d:/ymir work/ui/game/affectshower/50825.tga")
	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.DEF_BONUS] = (localeInfo.TOOLTIP_DEF_GRADE, 			"d:/ymir work/ui/game/affectshower/50826.tga")
	
	AFFECT_DATA_DICT[DEW_DESC_IDX_START+player.ENERGY] = (localeInfo.TOOLTIP_ENERGY, 			"d:/ymir work/ui/game/affectshower/51002.tga")
	AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+6] = (localeInfo.TOOLTIP_MAX_HP, "d:/ymir work/ui/game/affectshower/71027.tga")
	AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_ATTBONUS_PVM_SKILL] = (localeInfo.TOOLTIP_ATTBONUS_PVM_SKILL, "d:/ymir work/ui/game/affectshower/71029.tga")
	AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+132] = (localeInfo.TOOLTIP_MELEE_MAGIC_ATTBONUS_PER, "d:/ymir work/ui/game/affectshower/71028.tga")
	AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+93] = (localeInfo.TOOLTIP_MALL_ATTBONUS_STATIC, "d:/ymir work/ui/game/affectshower/71028.tga")
	AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_RESIST_MONSTER] = (localeInfo.TOOLTIP_RESIST_MONSTER, "d:/ymir work/ui/game/affectshower/71030.tga")
	AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_CRITICAL_PCT] = (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT, "d:/ymir work/ui/game/affectshower/71044.tga")
	AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_PENETRATE_PCT] = (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, "d:/ymir work/ui/game/affectshower/71045.tga")
	AFFECT_DATA_DICT[DEW_DESC_IDX_START+6] = (localeInfo.TOOLTIP_MAX_HP, 			"d:/ymir work/ui/game/affectshower/71027.tga")
	AFFECT_DATA_DICT[DEW_DESC_IDX_START+132] = (localeInfo.TOOLTIP_MELEE_MAGIC_ATTBONUS_PER, "d:/ymir work/ui/game/affectshower/71028.tga")
	AFFECT_DATA_DICT[DEW_DESC_IDX_START+player.POINT_ATTBONUS_PVM_SKILL] = (localeInfo.TOOLTIP_ATTBONUS_PVM_SKILL, "d:/ymir work/ui/game/affectshower/71029.tga")
	AFFECT_DATA_DICT[DEW_DESC_IDX_START+player.POINT_RESIST_MONSTER] = (localeInfo.TOOLTIP_RESIST_MONSTER, "d:/ymir work/ui/game/affectshower/71030.tga")
	AFFECT_DATA_DICT[DEW_DESC_IDX_START+40] = (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT, 			"d:/ymir work/ui/game/affectshower/71044.tga")
	AFFECT_DATA_DICT[DEW_DESC_IDX_START+41] = (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, 			"d:/ymir work/ui/game/affectshower/71045.tga")
	AFFECT_DATA_DICT[DEW_DESC_IDX_START+152] = (localeInfo.TOOLTIP_APPLY_ATTBONUS_BOSS, "d:/ymir work/ui/game/affectshower/50839.tga")
	AFFECT_DATA_DICT[DEW_DESC_IDX_START+152] = (localeInfo.TOOLTIP_APPLY_ATTBONUS_BOSS, "d:/ymir work/ui/game/affectshower/50839.tga")
	AFFECT_DATA_DICT[DEW_DESC_IDX_START+53] = (localeInfo.TOOLTIP_APPLY_ATTBONUS_MONSTER, "d:/ymir work/ui/game/affectshower/50837.tga")
	AFFECT_DATA_DICT[DEW_DESC_IDX_START+154] = (localeInfo.TOOLTIP_APPLY_ATTBONUS_STONE, "d:/ymir work/ui/game/affectshower/50838.tga")
	AFFECT_DATA_DICT[DEW_DESC_IDX_START+95] = (localeInfo.TOOLTIP_ATT_GRADE, "d:/ymir work/ui/game/affectshower/50825.tga")
	AFFECT_DATA_DICT[DEW_DESC_IDX_START+93] = (localeInfo.TOOLTIP_ATT_GRADE, "d:/ymir work/ui/game/affectshower/50825.tga")
	if app.ENABLE_VIP_SYSTEM:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_VIP] = (localeInfo.VIP_TOOLTIP, "elvion/game/affects/premium.png")
	if app.ENABLE_VOTE4BUFF:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_VOTE4BUFF] = (localeInfo.VOTE4BUFF_AFFECT_TITLE, "elvion/game/affects/vote4buff.png")

	AFFECT_DATA_DICT[1217] = (localeInfo.TOOLTIP_MOV_SPEED, "d:/ymir work/ui/game/affectshower/27109.tga")
	AFFECT_DATA_DICT[1219] = (localeInfo.TOOLTIP_ATT_SPEED, "d:/ymir work/ui/game/affectshower/27105.tga")

	AFFECT_DATA_DICT[750] = (localeInfo.TOOLTIP_MALL_EXPBONUS, "d:/ymir work/ui/game/affectshower/71153.tga")

	if app.ENABLE_WOLFMAN_CHARACTER:
		AFFECT_DATA_DICT[chr.AFFECT_BLEEDING] = (localeInfo.SKILL_BLEEDING, "d:/ymir work/ui/skill/common/affect/poison.sub")
		AFFECT_DATA_DICT[chr.AFFECT_RED_POSSESSION] = (localeInfo.SKILL_GWIGEOM, "d:/ymir work/ui/skill/wolfman/red_possession_03.sub")
		AFFECT_DATA_DICT[chr.AFFECT_BLUE_POSSESSION] = (localeInfo.SKILL_CHEONGEUN, "d:/ymir work/ui/skill/wolfman/blue_possession_03.sub")

	if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_PREMIUM_PRIVATE_SHOP] = (localeInfo.TOOLTIP_AFFECT_PREMIUM_PRIVATE_SHOP, "d:/ymir work/ui/skill/common/affect/premium_private_shop.sub")

	if app.ENABLE_RENEWAL_AFFECT_SHOWER:
		def Show(self):
			ui.Window.Show(self)
			self.SyncAffectImages()
			
	def __init__(self):
		ui.Window.__init__(self)

		self.serverPlayTime=0
		self.clientPlayTime=0

		self.lastUpdateTime=0
		self.affectImageDict={}
		self.toggleImageDict = {}
		self.horseImage=None
		self.lovePointImage=None
		if app.ENABLE_MULTI_FARM_BLOCK:
			self.multiFarmBlockDialog=None
			self.farmStatusImage=None

		self.SetPosition(10, 10)
		self.Show()
		
	def ClearAllAffects(self):
		self.horseImage=None
		self.lovePointImage=None
		if app.ENABLE_MULTI_FARM_BLOCK:
			self.multiFarmBlockDialog=None
			self.farmStatusImage=None
		self.affectImageDict={}
		self.__ArrangeImageList()

	def ClearAffects(self):
		self.living_affectImageDict={}
		for key, image in self.affectImageDict.items():
			if not image.IsSkillAffect() or key in PERSIST_THROUGH_DEATH:
				self.living_affectImageDict[key] = image
		self.affectImageDict = self.living_affectImageDict
		self.__ArrangeImageList()

	if app.ENABLE_RENEWAL_AFFECT_SHOWER:
		def CalculatePriority(self, affect):
			# check PRIORITY_FIRST o PRIORITY_LAST
			if affect in PRIORITY_FIRST:
				return 2  # hight priority
			elif affect in PRIORITY_LAST:
				return 5  # low priority

			##changes to organize the list if you use the display system waters, dew and blessings from quin
			priority_ranges = [
				(self.MALL_DESC_IDX_START, self.WATER_DESC_IDX_START, 4),
				# (self.WATER_DESC_IDX_START, self.DEW_DESC_IDX_START, 6),
				# (self.DEW_DESC_IDX_START, self.DEW_DESC_IDX_START + self.DEW_RANGE_OFFSET, 4),
			]

			for start, end, priority in priority_ranges:
				if start <= affect < end:
					return priority

			skillIndex = player.AffectIndexToSkillIndex(affect)
			if skillIndex != 0:
				return 1
			return 3
			
	def BINARY_NEW_AddAffect(self, type, pointIdx, value, duration):
		print "BINARY_NEW_AddAffect", type, pointIdx, value, duration
		
		# Enhanced filtering with renewal affect shower support
		if app.ENABLE_RENEWAL_AFFECT_SHOWER:
			affect = player.SkillIndexToAffectIndex(type)
			if affect == 0:
				affect = type
			original_affect = affect
			
			allowed_affects = [
				chr.NEW_AFFECT_POLYMORPH,
			]
			
			Skill_affects = [
				chr.AFFECT_JEONGWI,
				chr.AFFECT_GEOMGYEONG,
				chr.AFFECT_CHEONGEUN,
				chr.AFFECT_FALLEN_CHEONGEUN,
				chr.AFFECT_GYEONGGONG,
				chr.AFFECT_EUNHYEONG,
				chr.AFFECT_GWIGEOM,
				chr.AFFECT_GONGPO,
				chr.AFFECT_JUMAGAP,
				chr.AFFECT_HOSIN,
				chr.AFFECT_BOHO,
				chr.AFFECT_KWAESOK,
				chr.AFFECT_HEUKSIN,
				chr.AFFECT_MUYEONG,
				chr.AFFECT_GICHEON,
				chr.AFFECT_JEUNGRYEOK,
				chr.AFFECT_PABEOP,
			]
	
			if type < 500 and type not in allowed_affects and affect not in Skill_affects:
				return
		else:
			# Fallback to simple filtering with polymorph exception
			if type < 499 and type != chr.NEW_AFFECT_POLYMORPH:
				return
	
		# Enhanced affect mapping with support for additional types
		if type == chr.NEW_AFFECT_MALL:
			affect = self.MALL_DESC_IDX_START + pointIdx
		elif type == chr.NEW_AFFECT_BLEND:
			affect = self.DEW_DESC_IDX_START + pointIdx
		elif (app.ENABLE_EXTENDED_BLEND and type == chr.NEW_AFFECT_BLEND_EX):
			affect = self.DEW_DESC_IDX_START + pointIdx
		elif type == chr.NEW_AFFECT_WATER:
			affect = self.WATER_DESC_IDX_START + pointIdx
		elif type == chr.NEW_AFFECT_MALL_EX:
			affect = self.DRAGON_GOD_DESC_IDX_START + pointIdx
		elif type == chr.AFFECT_BLEND_FISH:
			affect = self.FISH_DESC_IDX_START + pointIdx
		else:
			if app.ENABLE_RENEWAL_AFFECT_SHOWER:
				affect = original_affect
			else:
				affect = type
	
		# Debug output for game masters
		if chr.IsGameMaster(0):
			chat.AppendChat(chat.CHAT_TYPE_INFO, "BINARY_NEW_AddAffect - type: "+str(type)+" =):"+str(affect)+" pointIdx:"+str(pointIdx)+" value:"+str(value)+" duration:"+str(duration))
	
		# Enhanced duplicate checking with renewal support
		if app.ENABLE_RENEWAL_AFFECT_SHOWER:
			if self.affectImageDict.get(affect):
				if player.AffectIndexToSkillIndex(affect) == 0:
					return
		else:
			if self.affectImageDict.has_key(affect):
				return
	
		# Check if affect data exists
		if not self.AFFECT_DATA_DICT.has_key(affect):
			return
	
		# Calculate priority for renewal system
		if app.ENABLE_RENEWAL_AFFECT_SHOWER:
			priority = self.CalculatePriority(affect)
	
		# Set infinite duration for specific affects
		if affect in [
			chr.NEW_AFFECT_NO_DEATH_PENALTY,
			chr.NEW_AFFECT_SKILL_BOOK_BONUS,
			chr.NEW_AFFECT_AUTO_SP_RECOVERY,
			chr.NEW_AFFECT_AUTO_HP_RECOVERY,
			chr.NEW_AFFECT_SKILL_BOOK_NO_DELAY,
			chr.AFFECT_RUNE_DECK5,
			chr.AFFECT_RUNE_DECK9,
		]:
			duration = 0
	
		affectData = self.AFFECT_DATA_DICT[affect]
		description = affectData[0]
		filename = affectData[1]
	
		# Handle mall bonus calculations
		if pointIdx == player.POINT_MALL_ITEMBONUS or\
		pointIdx == player.POINT_MALL_GOLDBONUS:
			value = 1 + float(value) / 100.0
	
		# Process description with value (skip for auto potions and specific affects)
		if affect != chr.NEW_AFFECT_AUTO_SP_RECOVERY and affect != chr.NEW_AFFECT_AUTO_HP_RECOVERY and affect != 551 and affect != chr.AFFECT_RUNE_DECK5 and affect != chr.AFFECT_RUNE_DECK9 and affect != chr.NEW_AFFECT_VOTE4BUFF:
			if callable(description):
				description = description(float(value))
	
		try:
			print "Add affect %s" % affect
			image = AffectImage()
			image.SetParent(self)
			image.LoadImage(filename)
			image.SetDescription(description)
			image.SetDuration(duration)
			image.SetAffect(affect)
			
			# Set extra descriptions if available
			try:
				image.SetExtraDescriptions(affect)
			except:
				pass
	
			# Enhanced skill affect handling with renewal support
			if affect == player.SkillIndexToAffectIndex(type) and app.ENABLE_RENEWAL_AFFECT_SHOWER:
				skillIndex = player.AffectIndexToSkillIndex(player.SkillIndexToAffectIndex(type))
				image.SetSkillIndex(skillIndex)
				image.SetSkillAffectFlag(True)
				image.SetCanberemoved(True)
				image.SetAffectValue(value)
				image.UpdateSkillDescription()
				image.SetEvent(ui.__mem_func__(self.__QuestionRemoveAffect),"mouse_click", skillIndex, description)
			else:
				image.SetSkillAffectFlag(False)
				
				# Define affect groups
				EXP_BONUS_AFFECTS = {chr.NEW_AFFECT_EXP_BONUS_EURO_FREE, chr.NEW_AFFECT_EXP_BONUS_EURO_FREE_UNDER_15}
				AUTO_POTION_AFFECTS = {chr.NEW_AFFECT_AUTO_SP_RECOVERY, chr.NEW_AFFECT_AUTO_HP_RECOVERY}
				
				# Handle different affect types
				if affect in EXP_BONUS_AFFECTS or affect == 551 or self.INFINITE_AFFECT_DURATION < duration:
					if not app.ENABLE_RENEWAL_AFFECT_SHOWER:
						image.SetClock(False)
					image.UpdateDescription()
				elif affect in AUTO_POTION_AFFECTS:
					image.UpdateAutoPotionDescription()
				elif affect == chr.NEW_AFFECT_POLYMORPH and app.ENABLE_RENEWAL_AFFECT_SHOWER:
					image.UpdateDescription()
					image.SetCanberemoved(True)
					image.SetEvent(ui.__mem_func__(self.__QuestionRemoveAffect),"mouse_click", affect, description)
				else:
					image.UpdateDescription()
	
			# Set scale for dragon soul affects
			DRAGON_SOUL_AFFECTS = {chr.NEW_AFFECT_DRAGON_SOUL_DECK1, chr.NEW_AFFECT_DRAGON_SOUL_DECK2}
			if affect in DRAGON_SOUL_AFFECTS:
				image.SetScale(1, 1)
			else:
				image.SetScale(0.7, 0.7)
	
			# Set priority for renewal system
			if app.ENABLE_RENEWAL_AFFECT_SHOWER:
				image.SetPriority(priority)
	
			image.Show()
			self.affectImageDict[affect] = image
			self.__ArrangeImageList()
			
		except Exception, e:
			print "except Aff affect ", e
			pass

	def BINARY_NEW_RemoveAffect(self, type, pointIdx):
		if type == chr.NEW_AFFECT_MALL:
			affect = self.MALL_DESC_IDX_START + pointIdx
		elif type == chr.NEW_AFFECT_BLEND:
			affect = self.DEW_DESC_IDX_START + pointIdx
		elif (app.ENABLE_EXTENDED_BLEND and type == chr.NEW_AFFECT_BLEND_EX):
			affect = self.DEW_DESC_IDX_START + pointIdx
		elif type == chr.NEW_AFFECT_WATER:
			affect = self.WATER_DESC_IDX_START + pointIdx
		elif type == chr.NEW_AFFECT_MALL_EX:
			affect = self.DRAGON_GOD_DESC_IDX_START + pointIdx
		elif type == chr.AFFECT_BLEND_FISH:
			affect = self.FISH_DESC_IDX_START + pointIdx
		else:
			affect = type
	
		print "Remove Affect %s %s" % ( type , pointIdx )
		self.__RemoveAffect(affect)
		self.__ArrangeImageList()

		

				

	if app.ENABLE_AFFECT_FIX:
		def BINARY_NEW_RefreshAffect(self):
			self.__ArrangeImageList()

	def SetAffect(self, affect):
		self.__AppendAffect(affect)
		self.__ArrangeImageList()

	def ResetAffect(self, affect):
		self.__RemoveAffect(affect)
		self.__ArrangeImageList()

	def SetLoverInfo(self, name, lovePoint):
		image = LovePointImage()
		image.SetParent(self)
		image.SetLoverInfo(name, lovePoint)
		self.lovePointImage = image
		self.__ArrangeImageList()

	if app.ENABLE_MULTI_FARM_BLOCK:
		def SetMultiFarmPlayer(self, playerName):
			if self.farmStatusImage:
				self.farmStatusImage.players.append(playerName)
		def __OnClickMultiFarm(self, emptyArg):
			multiFarmBlockDialog = uiCommon.QuestionDialog()
			if self.farmStatusImage.multiFarmStatus:
				multiFarmBlockDialog.SetText(localeInfo.MULTI_FARM_DEACTIVE_TEXT)
			else:
				multiFarmBlockDialog.SetText(localeInfo.MULTI_FARM_ACTIVE_TEXT)
			multiFarmBlockDialog.SetAcceptEvent(lambda arg = True: self.OnCloseMultiFarm(arg))
			multiFarmBlockDialog.SetCancelEvent(lambda arg = False: self.OnCloseMultiFarm(arg))
			multiFarmBlockDialog.Open()
			self.multiFarmBlockDialog = multiFarmBlockDialog
		def OnCloseMultiFarm(self, answer):
			if not self.multiFarmBlockDialog:
				return
			if answer:
				net.SendChatPacket("/multi_farm")
			self.multiFarmBlockDialog.Close()
			self.multiFarmBlockDialog = None
		def SetMultiFarmInfo(self, farmStatus):
			image = MultiFarmImage()
			image.SetParent(self)
			image.SetMultiFarmInfo(farmStatus)
			image.SetEvent(ui.__mem_func__(self.__OnClickMultiFarm),"mouse_click")
			image.Show()
			self.farmStatusImage = image
			self.__ArrangeImageList()
			
	def ShowLoverState(self):
		if self.lovePointImage:
			self.lovePointImage.Show()
			self.__ArrangeImageList()

	def HideLoverState(self):
		if self.lovePointImage:
			self.lovePointImage.Hide()
			self.__ArrangeImageList()

	def ClearLoverState(self):
		self.lovePointImage = None
		self.__ArrangeImageList()

	def OnUpdateLovePoint(self, lovePoint):
		if self.lovePointImage:
			self.lovePointImage.OnUpdateLovePoint(lovePoint)

	def SetHorseState(self, level, health, battery):
		if level==0:
			self.horseImage=None
		else:
			image = HorseImage()
			image.SetParent(self)
			image.SetState(level, health, battery)
			image.Show()

			self.horseImage=image
			self.__ArrangeImageList()

	def SetPlayTime(self, playTime):
		self.serverPlayTime = playTime
		self.clientPlayTime = app.GetTime()

		if localeInfo.IsVIETNAM():
			image = PlayTimeImage()
			image.SetParent(self)
			image.SetPlayTime(playTime)
			image.Show()

			self.playTimeImage=image
			self.__ArrangeImageList()

	def __AppendAffect(self, affect):

		if self.affectImageDict.has_key(affect):
			return

		try:
			affectData = self.AFFECT_DATA_DICT[affect]
		except KeyError:
			return

		name = affectData[0]
		filename = affectData[1]

		skillIndex = player.AffectIndexToSkillIndex(affect)
		if 0 != skillIndex:
			name = skill.GetSkillName(skillIndex)
			

		image = AffectImage()
		image.SetParent(self)
		image.SetSkillAffectFlag(True)
		if app.ENABLE_RENEWAL_AFFECT_SHOWER:
			priority = self.CalculatePriority(affect)
			image.SetPriority(priority)
		image.SetSkillIndex(skillIndex)

		try:
			image.LoadImage(filename)
		except:
			pass
			
		if app.ENABLE_RENEWAL_AFFECT_SHOWER:
			if skillIndex != 0:
				image.SetCanberemoved(True)
				image.SetToolTipText(name)
				image.SetEvent(ui.__mem_func__(self.__QuestionRemoveAffect),"mouse_click", skillIndex, name)
			else:
				image.SetToolTipText(name)
		else:
			image.SetToolTipText(name, 0, 40)
		image.SetScale(0.7, 0.7)
		image.Show()
		self.affectImageDict[affect] = image

	def __RemoveAffect(self, affect):
		"""
		if affect == chr.NEW_AFFECT_AUTO_SP_RECOVERY:
			self.autoPotionImageSP.Hide()

		if affect == chr.NEW_AFFECT_AUTO_HP_RECOVERY:
			self.autoPotionImageHP.Hide()
		"""
			
		if not self.affectImageDict.has_key(affect):
			print "__RemoveAffect %s ( No Affect )" % affect
			return

		print "__RemoveAffect %s ( Affect )" % affect
		del self.affectImageDict[affect]
		
		self.__ArrangeImageList()

	if app.ENABLE_RENEWAL_AFFECT_SHOWER:
		def __ArrangeImageList(self):
			ordered_elements = sorted(
				self.affectImageDict.values(),
				key=lambda image: image.GetPriority()
			)

			xPos, yPos = 0, 0
			xMax = 0
			countRow = 0

			if self.lovePointImage and self.lovePointImage.IsShow():
				self.lovePointImage.SetPosition(xPos, yPos)
				xPos += self.IMAGE_STEP
				countRow += 1

			if self.horseImage:
				self.horseImage.SetPosition(xPos, yPos)
				xPos += self.IMAGE_STEP
				countRow += 1

			for count, image in enumerate(ordered_elements):
				image.SetPosition(xPos, yPos)
				xPos += self.IMAGE_STEP
				countRow += 1

				if xMax < xPos:
					xMax = xPos

				if countRow == self.MAX_ITEMS_PER_ROW:
					xPos = 0
					yPos += self.IMAGE_STEP
					countRow = 0

			self.SetSize(xMax if xMax > 0 else self.IMAGE_STEP * self.MAX_ITEMS_PER_ROW, yPos + 26)
	else:
		def __ArrangeImageList(self):
			width = len(self.affectImageDict) * self.IMAGE_STEP
			if self.lovePointImage:
				width+=self.IMAGE_STEP
			if app.ENABLE_MULTI_FARM_BLOCK:
				if self.farmStatusImage:
					width+=self.IMAGE_STEP
			if self.horseImage:
				width+=self.IMAGE_STEP

			self.SetSize(width, 26)

			xPos = 0

			if app.ENABLE_MULTI_FARM_BLOCK:
				if self.farmStatusImage:
					if self.farmStatusImage.IsShow():
						self.farmStatusImage.SetPosition(xPos, 0)
						xPos += self.IMAGE_STEP
					
			if self.lovePointImage:
				if self.lovePointImage.IsShow():
					self.lovePointImage.SetPosition(xPos, 0)
					xPos += self.IMAGE_STEP

			if self.horseImage:
				self.horseImage.SetPosition(xPos, 0)
				xPos += self.IMAGE_STEP

			for image in self.affectImageDict.values():
				image.SetPosition(xPos, 0)
				xPos += self.IMAGE_STEP

	ArrangeImageList = __ArrangeImageList

	def DeleteToggleImage(self, cell):
		if cell in self.toggleImageDict:
			del self.toggleImageDict[cell]

	def RefreshInventory(self):
		for cell in xrange(player.INVENTORY_SLOT_COUNT):
			itemVnum = player.GetItemIndex(player.INVENTORY, cell)
			if itemVnum == 0:
				self.DeleteToggleImage(cell)
				continue
			
			item.SelectItem(itemVnum)
			
			if item.GetItemType() != item.ITEM_TYPE_TOGGLE:
				self.DeleteToggleImage(cell)
				continue
			
			if 0 != player.GetItemMetinSocket(cell, 3):
				if cell in self.toggleImageDict:
					continue
				
				if item.GetItemSubType() == item.TOGGLE_AUTO_RECOVERY_HP or \
				   item.GetItemSubType() == item.TOGGLE_AUTO_RECOVERY_SP:
					image = AutoPotionImage(item.GetItemSubType(), cell)
				elif item.GetItemSubType() == item.TOGGLE_AFFECT:
					image = ToggleItemImage(cell)
				else:
					continue
				
				image.SetParent(self)
				image.Update()
				image.Show()
				
				self.toggleImageDict[cell] = image
			else:
				self.DeleteToggleImage(cell)

		self.__ArrangeImageList()

	def OnUpdate(self):
		try:
			currentTime = app.GetGlobalTime()
			if currentTime - self.lastUpdateTime > self.TOOLTIP_UPDATE_INTERVAL:
				self.lastUpdateTime = currentTime

				for image in self.affectImageDict.values():
					if not image.isToolTipVisible:
						continue

					if image.GetAffect() in [chr.NEW_AFFECT_AUTO_HP_RECOVERY, chr.NEW_AFFECT_AUTO_SP_RECOVERY]:
						image.UpdateAutoPotionDescription()
					elif image.IsSkillAffect():
						image.UpdateSkillDescription()
					else:
						image.UpdateDescription()
		except Exception as e:
			print "AffectShower::OnUpdate error:", e
			
	if app.ENABLE_RENEWAL_AFFECT_SHOWER:
		def SyncAffectImages(self):
			for image in self.affectImageDict.values():
				image.isToolTipVisible = False
				image.toolTip.HideToolTip()

		def __QuestionRemoveAffect(self, arg, affect, name):
			self.RemoveQuestionDialog = uiCommon.QuestionDialog()
			self.RemoveQuestionDialog.SetText(localeInfo.AFFECT_REMOVE_QUESTION % name)
			self.RemoveQuestionDialog.SetAcceptEvent(lambda arg=True: self.AnswerRemoveAffect(arg))
			self.RemoveQuestionDialog.SetCancelEvent(lambda arg=False: self.AnswerRemoveAffect(arg))
			self.RemoveQuestionDialog.affect = affect
			self.RemoveQuestionDialog.Open()

		def AnswerRemoveAffect(self, flag):
			if not self.RemoveQuestionDialog:
				return
			if flag:
				net.SendChatPacket("/remove_affect %d" % self.RemoveQuestionDialog.affect)
			self.RemoveQuestionDialog.Close()
			self.RemoveQuestionDialog = None