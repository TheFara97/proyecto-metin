
import ui
import time
import localeInfo
import exception
import constInfo

config = {
	'duration': 5.0,
	'move_distance' : 165.0,
	'x_move_speed' : 8,
	'y_move_speed': 14.0,
}

class WorldbossNotification(ui.ScriptWindow):
	def __del__(self):
		ui.ScriptWindow.__del__(self)
		
	def __init__(self, szString, iPosition):
		ui.ScriptWindow.__init__(self)

		self.moveX = 0.0
		self.moveY = 0.0

		self.position = iPosition
		self.hideAnimation = 0
		self.visible = 1

		self.__loadWindow()
		self.__displayNotification(szString)

		self.Show()
		self.SetTop()

		self.WaitDialog = WaitingDialog()
		self.WaitDialog.Open(config['duration'])
		self.WaitDialog.SAFE_SetTimeOverEvent(self.endTimerEvent)

	def __loadWindow(self):
		try:
			ui.PythonScriptLoader().LoadScriptFile(self, "UIScript/worldbosswindow.py")

			self.childName = self.GetChild("Player_Name")
			self.childText = self.GetChild("Status_Text")
		except Exception as e:
			exception.Abort(constInfo.getLogText(e))

	def __displayNotification(self, szString):
		self.moveX = 100
		self.moveY = config["move_distance"] * self.position

		self.childName.SetText(localeInfo.WORLDBOSS_GUARD_NAME)
		self.childText.SetText(szString)

		self.x, self.y = self.GetGlobalPosition()
		self.SetPosition(self.x, self.y + self.moveY)

	def endTimerEvent(self):
		self.hideAnimation = 1
		self.moveX = 400

	def OnUpdate(self):
		if self.visible:

			if self.moveX > 0.0:
				self.x, self.y = self.GetGlobalPosition()

				if self.hideAnimation:
					self.SetPosition(self.x - config['x_move_speed'], self.y)
				else:
					self.SetPosition(self.x + config['x_move_speed'], self.y)

				self.moveX -= config['x_move_speed']
			else:
				if self.hideAnimation:
					self.Close()

			"""if self.moveY > 0.0:
				self.x, self.y = self.GetGlobalPosition()

				self.SetPosition(self.x, self.y + config['y_move_speed'])
				self.moveY -= config['y_move_speed']"""

	def Close(self):
		constInfo.WORLD_BOSS_TEXT_POSITION[self.position] = 0
		self.visible = 0
		self.Hide()

	def Destroy(self):
		self.Hide()
		self.Close()

class WaitingDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.eventTimeOver = lambda *arg: None
		self.eventExit = lambda *arg: None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Open(self, waitTime):
		curTime = time.clock()
		self.endTime = curTime + waitTime
		self.Show()

	def Close(self):
		self.Hide()

	def Destroy(self):
		self.Hide()

	def SAFE_SetTimeOverEvent(self, event):
		self.eventTimeOver = ui.__mem_func__(event)

	def SAFE_SetExitEvent(self, event):
		self.eventExit = ui.__mem_func__(event)
		
	def OnUpdate(self):
		lastTime = max(0, self.endTime - time.clock())
		if 0 == lastTime:
			self.Close()
			self.eventTimeOver()
		else:
			return