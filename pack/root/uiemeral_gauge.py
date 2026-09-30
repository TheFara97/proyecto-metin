#-*- coding: iso-8859-1 -*-

import ui
import wndMgr
import localeInfo

PATH = "elvion/game/dungeons/emerald/"

class EmeraldGauge(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__BuildUI()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __BuildUI(self):
		self.imgEmpty = ui.ImageBox()
		self.imgEmpty.SetParent(self)
		self.imgEmpty.SetPosition(0, 0)
		self.imgEmpty.LoadImage(PATH + "gauge_empty.png")
		self.imgEmpty.Show()

		self.imgFull = ui.ExpandedImageBox()
		self.imgFull.SetParent(self)
		self.imgFull.SetPosition(0, 0)
		self.imgFull.LoadImage(PATH + "gauge_full.png")
		self.imgFull.SetRenderingRect(0.0, 0.0, -1.0, 0.0)
		self.imgFull.Show()

		self.imgDone = ui.ImageBox()
		self.imgDone.SetParent(self)
		self.imgDone.SetPosition(0, 0)
		self.imgDone.LoadImage(PATH + "gauge_full_done.png")
		self.imgDone.Hide()

		w = self.imgEmpty.GetWidth()
		h = self.imgEmpty.GetHeight()
		if w <= 0: w = 256
		if h <= 0: h = 64

		self.txtPercent = ui.TextLine()
		self.txtPercent.SetParent(self)
		self.txtPercent.SetHorizontalAlignCenter()
		self.txtPercent.SetVerticalAlignCenter()
		self.txtPercent.SetPosition(227, 61)
		self.txtPercent.SetFontName("Verdana:16b")
		self.txtPercent.SetPackedFontColor(0xFFafdea2)
		self.txtPercent.SetOutline()
		self.txtPercent.SetText("0%")
		self.txtPercent.Show()

		self.SetSize(w, h)
		scrW = wndMgr.GetScreenWidth()
		scrH = wndMgr.GetScreenHeight()
		self.SetPosition(40, scrH - h - 188)

	def SetPercent(self, pct):
		pct = max(0, min(100, pct))
		self.txtPercent.SetText(localeInfo.DUNGEON_PROGRESSBAR_EMERALD + str(int(pct)) + "%")
		if pct >= 100:
			self.imgFull.Hide()
			self.imgDone.Show()
		else:
			self.imgDone.Hide()
			self.imgFull.Show()
			if pct <= 0:
				self.imgFull.SetRenderingRect(0.0, 0.0, -1.0, 0.0)
			else:
				right_crop = -(1.0 - pct / 100.0)
				self.imgFull.SetRenderingRect(0.0, 0.0, right_crop, 0.0)

	def Open(self, pct=0):
		self.SetPercent(pct)
		self.Show()

	def Close(self):
		self.Hide()
