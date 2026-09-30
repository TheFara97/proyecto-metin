import ui
import constInfo
import chat

class IsIngameIcon(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def BindInterfaceClass(self, interface):
		self.interface = interface

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/ikonki.py")
		except:
			import exception
			exception.Abort("Ikonki.LoadWindow.LoadObject")
			
		try:
			self.button = self.GetChild("ItemShopButton")
			self.ItemShopButtonAni = self.GetChild("ItemShopButtonAni")
			self.button1 = self.GetChild("BattlepassButton")
			self.BattlepassButtonAni = self.GetChild("BattlepassButtonAni")
			#self.button2 = self.GetChild("EventButton")
			self.button3 = self.GetChild("WheelOfFortuneButton")
			self.WorldbossButtonAni = self.GetChild("WorldbossButtonAni")
			self.button4 = self.GetChild("WheelOfFortuneRealButton")
			self.expander = self.GetChild("expander")
		except:
			import exception
			exception.Abort("Ikonki.LoadWindow.BindObject")

		self.button.SetEvent(ui.__mem_func__(self.__OnClickIs))
		self.button1.SetEvent(ui.__mem_func__(self.__OpenCodeWindow))
		#self.button2.SetEvent(ui.__mem_func__(self.interface.OpenEventCalendar))
		self.button3.SetEvent(ui.__mem_func__(self.interface.OpenWorldboss))
		self.button4.SetEvent(ui.__mem_func__(self.__OnClickWheelPage))
		self.expander.SetEvent(ui.__mem_func__(self.Expander))

		if constInfo.IkonkiExpander == 0:
			self.button.Hide()
			self.ItemShopButtonAni.Hide()
			self.button1.Hide()
			self.BattlepassButtonAni.Hide()
			#self.button2.Hide()
			self.button3.Hide()
			self.button4.Hide()
			self.WorldbossButtonAni.Hide()
		else:
			self.button.Show()
			self.ItemShopButtonAni.Show()
			self.button1.Show()
			self.BattlepassButtonAni.Show()
			#self.button2.Show()
			self.button3.Show()
			self.button4.Show()
			self.WorldbossButtonAni.Show()
		

	def Expander(self):
		if constInfo.IkonkiExpander == 1:
			self.button.Hide()
			self.ItemShopButtonAni.Hide()
			self.button1.Hide()
			self.BattlepassButtonAni.Hide()
			#self.button2.Hide()
			self.button3.Hide()
			self.button4.Hide()
			self.WorldbossButtonAni.Hide()
			constInfo.IkonkiExpander = 0
		else:
			self.button.Show()
			self.ItemShopButtonAni.Show()
			self.button1.Show()
			self.BattlepassButtonAni.Show()
			#self.button2.Show()
			self.button3.Show()
			self.button4.Show()
			self.WorldbossButtonAni.Show()
			constInfo.IkonkiExpander = 1

	def __OpenCodeWindow(self):
		self.interface.ToggleBattlePass()

	def __OnClickIs(self):
		self.interface.OpenItemShopMainWindow()

	def __OnClickWheelPage(self):
		self.interface.OpenItemShopToWheel()

	def Open(self, id):
		self.id = id
		self.Show()

	def Close(self):
		self.Hide()

	def Destroy(self):
		self.ClearDictionary()