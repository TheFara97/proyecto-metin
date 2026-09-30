import ui
import exception
import net
import chat

class DepoIsWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)

	def Open(self):
		if self.IsShow():
			self.Close()
		else:
			self.Show()

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return
			
		self.isLoaded = 1
		try:
			ui.PythonScriptLoader().LoadScriptFile(self, "uiscript/depoiswindow.py")
		except:
			exception.Abort("DepoISWindow.LoadDialog.LoadScript")

		try:
			self.titleBar = self.GetChild("TitleBar")
			self.depo = self.GetChild("DepoButton")
			self.itemshop = self.GetChild("IsButton")
			self.depo.SetEvent(ui.__mem_func__(self.OpenDepo))
			self.itemshop.SetEvent(ui.__mem_func__(self.OpenIS))
			self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
		except:
			exception.Abort("DepoISWindow.LoadDialog.BindObject")
		self.SetCenterPosition()
		self.SetTop()

	def Destroy(self):
		self.Hide()
		self.ClearDictionary()

	def Close(self):
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE
	
	def OpenDepo(self):
		self.Close()
		net.SendChatPacket("/click_safebox")

	def OpenIS(self):
		self.Close()
		net.SendChatPacket("/click_mall")
	