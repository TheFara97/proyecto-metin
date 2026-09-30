__author__ = "Light05"
__system__ = "Client Optimisation - IntroLoading Renewal"
__date__ = "30.08.2022"
__ver__ = "0.15"

import ui, net, wndMgr, app

class LoadingWindow(ui.Window):
	def __init__(self, stream):
		ui.Window.__init__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_LOAD, self)
		self.stream = stream
		self.loadingImage = None

	def __del__(self):
		net.SetPhaseWindow(net.PHASE_WINDOW_LOAD, 0)
		ui.Window.__del__(self)

	def LoadData(self, playerX, playerY):
		"""Called by C++ when character data is received"""
		app.SetGlobalCenterPosition(playerX, playerY)
		net.StartGame()

	def Open(self):
		# Create and setup loading image
		self.loadingImage = ui.ExpandedImageBox()
		self.loadingImage.SetParent(self)
		self.loadingImage.SetPosition(0, 0)
		self.loadingImage.LoadImage("elvion/game/loading/bg.png")
		
		# Scale the image to fit screen
		imgWidth = self.loadingImage.GetWidth()
		imgHeight = self.loadingImage.GetHeight()
		
		if imgWidth > 0 and imgHeight > 0:
			width = float(wndMgr.GetScreenWidth()) / float(imgWidth)
			height = float(wndMgr.GetScreenHeight()) / float(imgHeight)
			self.loadingImage.SetScale(width, height)
		
		self.loadingImage.Show()
		self.Show()
		app.SetFrameSkip(0)
		
		# Send the select character packet
		net.SendSelectCharacterPacket(self.stream.GetCharacterSlot())

	def Close(self):
		app.SetFrameSkip(1)
		
		if self.loadingImage:
			self.loadingImage.Hide()
			self.loadingImage = None
		
		self.Hide()

	def OnPressEscapeKey(self):
		app.SetFrameSkip(1)
		self.stream.SetLoginPhase()
		return True