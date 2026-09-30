#-*- coding: iso-8859-1 -*-
import net
import player
import grp
import localeInfo
import ui
import uiToolTip
import wndMgr
import constInfo
import app
import uiScriptLocale

ROOT_PATH = "elvion/game/ranking_boss/"

class LegendDamageWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.rank_slot = []
		self.rankDataBackground = None
		self.rankData = None
		self.vid = 0
		self.autoRefreshLastTime = app.GetTime() + 1.0
		self.LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.rank_slot = []
		self.rankDataBackground = None
		self.rankData = None
		self.vid = 0
		self.autoRefreshLastTime = 0

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/LegendDamageWindow.py")
		except:
			import exception
			exception.Abort("LegendDamageWindow.LoadWindow")

		try:
			self.Board = self.GetChild("Board")
			self.titleBar = self.GetChild("TitleBar")
		except:
			import exception
			exception.Abort("LegendDamageWindow.LoadWindow.BindObject")

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

		self.BuildRankSlots()

	def BuildRankSlots(self):
		x_pos = 9
		y_pos = 64

		self.rankDataBackground = {
			"ID": [],
			"EMPIRE": [],
			"NAME": [],
			"LEVEL": [],
			"DAMAGE": [],
		}
		self.rankData = {
			"ID": [],
			"EMPIRE": [],
			"NAME": [],
			"LEVEL": [],
			"DAMAGE": [],
		}

		for i in xrange(15):
			rank_slots = ui.ImageBox()
			rank_slots.SetParent(self.Board)
			rank_slots.SetPosition(x_pos, y_pos)
			rank_slots.LoadImage(ROOT_PATH + str(i + 1) + ".png")
			rank_slots.Show()
			self.rank_slot.append(rank_slots)
			y_pos += 26

			# ID Background
			ID_BACKGROUND = ui.Bar("TOP_MOST")
			ID_BACKGROUND.SetParent(rank_slots)
			ID_BACKGROUND.SetPosition(5, 2)
			ID_BACKGROUND.SetSize(63, 22)
			ID_BACKGROUND.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.0))
			ID_BACKGROUND.Show()
			self.rankDataBackground["ID"].append(ID_BACKGROUND)

			# Empire Background
			EMPIRE_BACKGROUND = ui.Bar("TOP_MOST")
			EMPIRE_BACKGROUND.SetParent(rank_slots)
			EMPIRE_BACKGROUND.SetPosition(72, 6)
			EMPIRE_BACKGROUND.SetSize(20, 10)
			EMPIRE_BACKGROUND.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.0))
			EMPIRE_BACKGROUND.Show()
			self.rankDataBackground["EMPIRE"].append(EMPIRE_BACKGROUND)

			# Name Background
			NAME_BACKGROUND = ui.Bar("TOP_MOST")
			NAME_BACKGROUND.SetParent(rank_slots)
			NAME_BACKGROUND.SetPosition(72, 0)
			NAME_BACKGROUND.SetSize(111, 22)
			NAME_BACKGROUND.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.0))
			NAME_BACKGROUND.Show()
			self.rankDataBackground["NAME"].append(NAME_BACKGROUND)

			# Level Background
			LEVEL_BACKGROUND = ui.Bar("TOP_MOST")
			LEVEL_BACKGROUND.SetParent(rank_slots)
			LEVEL_BACKGROUND.SetPosition(167, 0)
			LEVEL_BACKGROUND.SetSize(61, 22)
			LEVEL_BACKGROUND.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.0))
			LEVEL_BACKGROUND.Show()
			self.rankDataBackground["LEVEL"].append(LEVEL_BACKGROUND)

			# Damage Background
			DAMAGE_BACKGROUND = ui.Bar("TOP_MOST")
			DAMAGE_BACKGROUND.SetParent(rank_slots)
			DAMAGE_BACKGROUND.SetPosition(228, 0)
			DAMAGE_BACKGROUND.SetSize(75, 22)
			DAMAGE_BACKGROUND.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.0))
			DAMAGE_BACKGROUND.Show()
			self.rankDataBackground["DAMAGE"].append(DAMAGE_BACKGROUND)

			# Text lines
			ID = ui.MakeTextLine(self.rankDataBackground["ID"][i])
			EMPIRE = ui.MakeTextLine(self.rankDataBackground["EMPIRE"][i])
			NAME = ui.MakeTextLine(self.rankDataBackground["NAME"][i])
			LEVEL = ui.MakeTextLine(self.rankDataBackground["LEVEL"][i])
			DAMAGE = ui.MakeTextLine(self.rankDataBackground["DAMAGE"][i])
			
			self.rankData["ID"].append(ID)
			self.rankData["EMPIRE"].append(EMPIRE)
			self.rankData["NAME"].append(NAME)
			self.rankData["LEVEL"].append(LEVEL)
			self.rankData["DAMAGE"].append(DAMAGE)

	def OnUpdate(self):
		if app.GetTime() > self.autoRefreshLastTime:
			self.autoRefreshLastTime = app.GetTime() + 1.0

			if self.vid in constInfo.LEGEND_DAMAGE_DATA:
				for pos in range(len(constInfo.LEGEND_DAMAGE_DATA[self.vid]["NAME"])):
					if constInfo.LEGEND_DAMAGE_DATA[self.vid]["NAME"][pos] == "":
						self.rankData["EMPIRE"][pos].SetText("")
						self.rankData["NAME"][pos].SetText("-")
						self.rankData["LEVEL"][pos].SetText("-")
						self.rankData["DAMAGE"][pos].SetText("-")
						continue

					self.rankData["EMPIRE"][pos].SetText("|Eranking/flag_%d|e" % (constInfo.LEGEND_DAMAGE_DATA[self.vid]["EMPIRE"][pos]))
					self.rankData["NAME"][pos].SetText("%s" % (constInfo.LEGEND_DAMAGE_DATA[self.vid]["NAME"][pos]))
					self.rankData["LEVEL"][pos].SetText("%s" % (constInfo.LEGEND_DAMAGE_DATA[self.vid]["LEVEL"][pos]))
					self.rankData["DAMAGE"][pos].SetText("%s" % (localeInfo.NumberWithDots(constInfo.LEGEND_DAMAGE_DATA[self.vid]["DAMAGE"][pos])))

					if constInfo.LEGEND_DAMAGE_DATA[self.vid]["NAME"][pos] == player.GetName():
						self.rankData["NAME"][pos].SetPackedFontColor(0xffe0c198)
						self.rankData["NAME"][pos].SetFontName(localeInfo.UI_BOLD_FONT)
						self.rankData["NAME"][pos].SetOutline()
						self.rankData["LEVEL"][pos].SetPackedFontColor(0xffe0c198)
						self.rankData["LEVEL"][pos].SetFontName(localeInfo.UI_BOLD_FONT)
						self.rankData["LEVEL"][pos].SetOutline()
						self.rankData["DAMAGE"][pos].SetPackedFontColor(0xffe0c198)
						self.rankData["DAMAGE"][pos].SetFontName(localeInfo.UI_BOLD_FONT)
						self.rankData["DAMAGE"][pos].SetOutline()
					else:
						self.rankData["NAME"][pos].SetPackedFontColor(grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0))
						self.rankData["NAME"][pos].SetFontName(localeInfo.UI_DEF_FONT)
						self.rankData["NAME"][pos].SetOutline()
						self.rankData["LEVEL"][pos].SetPackedFontColor(grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0))
						self.rankData["LEVEL"][pos].SetFontName(localeInfo.UI_DEF_FONT)
						self.rankData["LEVEL"][pos].SetOutline()
						if pos >= 10:
							self.rankData["DAMAGE"][pos].SetPackedFontColor(0xffed443e)
						else:
							self.rankData["DAMAGE"][pos].SetPackedFontColor(grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0))
						self.rankData["DAMAGE"][pos].SetFontName(localeInfo.UI_DEF_FONT)
			else:
				for pos in range(15):
					self.rankData["EMPIRE"][pos].SetText("")
					self.rankData["NAME"][pos].SetText("-")
					self.rankData["LEVEL"][pos].SetText("-")
					self.rankData["DAMAGE"][pos].SetText("-")

	def BindInterfaceClass(self, interface):
		self.interface = interface
		
	def Show(self, vid):
		if self.IsShow():
			self.Close()
		else:
			ui.ScriptWindow.Show(self)
			self.vid = vid

	def Close(self):
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE