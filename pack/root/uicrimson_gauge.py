#-*- coding: iso-8859-1 -*-

import ui
import wndMgr
import localeInfo

PATH = "elvion/game/dungeons/crimson/"

GAUGE_GAP = 6  # vertical gap between red and blue bar

class CrimsonGauge(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__BuildUI()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __BuildUI(self):
		# ---- Red bar ------------------------------------------------
		self.imgRedEmpty = ui.ImageBox()
		self.imgRedEmpty.SetParent(self)
		self.imgRedEmpty.SetPosition(0, 0)
		self.imgRedEmpty.LoadImage(PATH + "gauge_red_empty.png")
		self.imgRedEmpty.Show()

		self.imgRedFull = ui.ExpandedImageBox()
		self.imgRedFull.SetParent(self)
		self.imgRedFull.SetPosition(0, 0)
		self.imgRedFull.LoadImage(PATH + "gauge_red_full.png")
		self.imgRedFull.SetRenderingRect(0.0, 0.0, -1.0, 0.0)
		self.imgRedFull.Show()

		gw = self.imgRedEmpty.GetWidth()
		gh = self.imgRedEmpty.GetHeight()
		if gw <= 0: gw = 256
		if gh <= 0: gh = 64

		self.txtRed = ui.TextLine()
		self.txtRed.SetParent(self)
		self.txtRed.SetHorizontalAlignCenter()
		self.txtRed.SetVerticalAlignCenter()
		self.txtRed.SetPosition(gw / 2, gh / 2)
		self.txtRed.SetFontName("Verdana:14b")
		self.txtRed.SetPackedFontColor(0xFFFF6060)
		self.txtRed.SetOutline()
		self.txtRed.SetText("Red: 0 / 10")
		self.txtRed.Show()

		# ---- Blue bar -----------------------------------------------
		blueY = gh + GAUGE_GAP

		self.imgBlueEmpty = ui.ImageBox()
		self.imgBlueEmpty.SetParent(self)
		self.imgBlueEmpty.SetPosition(0, blueY)
		self.imgBlueEmpty.LoadImage(PATH + "gauge_blue_empty.png")
		self.imgBlueEmpty.Show()

		self.imgBlueFull = ui.ExpandedImageBox()
		self.imgBlueFull.SetParent(self)
		self.imgBlueFull.SetPosition(0, blueY)
		self.imgBlueFull.LoadImage(PATH + "gauge_blue_full.png")
		self.imgBlueFull.SetRenderingRect(0.0, 0.0, -1.0, 0.0)
		self.imgBlueFull.Show()

		self.txtBlue = ui.TextLine()
		self.txtBlue.SetParent(self)
		self.txtBlue.SetHorizontalAlignCenter()
		self.txtBlue.SetVerticalAlignCenter()
		self.txtBlue.SetPosition(gw / 2, blueY + gh / 2)
		self.txtBlue.SetFontName("Verdana:14b")
		self.txtBlue.SetPackedFontColor(0xFF6495ED)
		self.txtBlue.SetOutline()
		self.txtBlue.SetText("Blue: 0 / 10")
		self.txtBlue.Show()

		# ---- Window size & position ---------------------------------
		totalH = blueY + gh
		self.SetSize(gw, totalH)

		scrH = wndMgr.GetScreenHeight()
		self.SetPosition(40, scrH - totalH - 188)

	def __UpdateBar(self, imgFull, current, total):
		pct = float(current) / float(total) if total > 0 else 0.0
		pct = max(0.0, min(1.0, pct))
		if pct <= 0.0:
			imgFull.SetRenderingRect(0.0, 0.0, -1.0, 0.0)
		else:
			imgFull.SetRenderingRect(0.0, 0.0, -(1.0 - pct), 0.0)

	def SetRedProgress(self, current, total=10):
		self.__UpdateBar(self.imgRedFull, current, total)
		self.txtRed.SetText(localeInfo.DUNGEON_PROGRESSBAR_CRIMSON_RED + str(int(current)) + " / " + str(int(total)))

	def SetBlueProgress(self, current, total=10):
		self.__UpdateBar(self.imgBlueFull, current, total)
		self.txtBlue.SetText(localeInfo.DUNGEON_PROGRESSBAR_CRIMSON_BLUE + str(int(current)) + " / " + str(int(total)))

	def Open(self, redCurrent=0, blueCurrent=0, total=10):
		self.SetRedProgress(redCurrent, total)
		self.SetBlueProgress(blueCurrent, total)
		self.Show()

	def Close(self):
		self.Hide()
