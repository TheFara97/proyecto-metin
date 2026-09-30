import app
import player
import ime

FIXED_TIMESTEP_UPDATE = 167
ONE_SECOND = 10000


class UpdatableAbstract:
	def __init__(self):
		pass

	def OnUpdate(self):
		pass

	def Destroy(self):
		pass

	def CanUpdate(self):
		return False


class PickUpOnDownKey(UpdatableAbstract):
	def __init__(self):
		UpdatableAbstract.__init__(self)
		self.timer = 0
		self.delay = 1500  # 0.15s

	def OnUpdate(self):
		self.timer += FIXED_TIMESTEP_UPDATE
		if self.timer >= self.delay:
			player.PickCloseItem()
			self.timer = 0

	def CanUpdate(self):
		return not ime.IsCaptureInput() and (app.IsPressed(app.DIK_Z) or app.IsPressed(app.DIK_GRAVE))
