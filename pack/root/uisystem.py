import net
import app
import ui
import uiOption
import uiSystemOption
import uiScriptLocale
import networkModule
import constInfo
import localeInfo

class SystemDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __Initialize(self):
		self.eventOpenHelpWindow = None
		self.gameOptionDlg = None
		self.systemOptionDlg = None
		self.moveChannelDlg = None

	def LoadDialog(self):
		try:
			pyScriptLoader = ui.PythonScriptLoader()
			pyScriptLoader.LoadScriptFile(self, "uiscript/systemdialog.py")
		except:
			import exception
			exception.Abort("SystemDialog.LoadDialog.LoadScript")

		try:
			self.GetChild("klawisze_button").SetEvent(ui.__mem_func__(self.__OnClickKeyChangeButton))
			self.GetChild("mall_button").SetEvent(ui.__mem_func__(self.__OnClickMallButton))
			self.GetChild("option_button").SetEvent(ui.__mem_func__(self.__OnClickOptionButton))
			self.GetChild("movechannel_button").SetEvent(ui.__mem_func__(self.__OnClickMoveChannelButton))
			self.GetChild("change_button").SetEvent(ui.__mem_func__(self.__OnClickChangeCharacterButton))
			self.GetChild("logout_button").SetEvent(ui.__mem_func__(self.__OnClickLogOutButton))
			self.GetChild("exit_button").SetEvent(ui.__mem_func__(self.__OnClickExitButton))
			self.GetChild("cancel_button").SetEvent(ui.__mem_func__(self.Close))
		except:
			import exception
			exception.Abort("SystemDialog.LoadDialog.BindObject")

	def Destroy(self):
		self.ClearDictionary()
		if self.gameOptionDlg:
			self.gameOptionDlg.Destroy()
			self.gameOptionDlg = None
		if self.systemOptionDlg:
			self.systemOptionDlg.Destroy()
			self.systemOptionDlg = None
		if self.moveChannelDlg:
			self.moveChannelDlg.Destroy()
			self.moveChannelDlg = None
		self.__Initialize()

	def OpenDialog(self):
		self.Show()

	def SetOpenHelpWindowEvent(self, event):
		self.eventOpenHelpWindow = event

	def RefreshMobile(self):
		if self.systemOptionDlg and hasattr(self.systemOptionDlg, "RefreshMobile"):
			self.systemOptionDlg.RefreshMobile()

	def OnMobileAuthority(self):
		if self.systemOptionDlg and hasattr(self.systemOptionDlg, "OnMobileAuthority"):
			self.systemOptionDlg.OnMobileAuthority()

	def OnBlockMode(self, mode=0):
		if self.systemOptionDlg and hasattr(self.systemOptionDlg, "OnBlockMode"):
			self.systemOptionDlg.OnBlockMode(mode)
		if self.gameOptionDlg and hasattr(self.gameOptionDlg, "OnChangePKMode"):
			self.gameOptionDlg.OnBlockMode(mode)

	def OnChangePKMode(self):
		if self.systemOptionDlg and hasattr(self.systemOptionDlg, "OnChangePKMode"):
			self.systemOptionDlg.OnChangePKMode()
		if self.gameOptionDlg and hasattr(self.gameOptionDlg, "OnChangePKMode"):
			self.gameOptionDlg.OnChangePKMode()

	def __OnClickKeyChangeButton(self):
		self.Close()

	def __OnClickMallButton(self):
		self.Close()
		net.SendChatPacket("/click_mall")

	def __OnClickOptionButton(self):
		self.Close()
		if not self.systemOptionDlg:
			self.systemOptionDlg = uiSystemOption.OptionDialog()
		self.systemOptionDlg.Show()

	def __OnClickMoveChannelButton(self):
		self.Close()
		if getattr(app, "BL_MOVE_CHANNEL", False):
			if not self.moveChannelDlg:
				self.moveChannelDlg = MoveChannelDialog()
			self.moveChannelDlg.Open()

	def __OnClickChangeCharacterButton(self):
		self.Close()
		net.LogOutGame()

	def __OnClickLogOutButton(self):
		self.Close()
		net.LogOutGame()

	def __OnClickExitButton(self):
		self.Close()
		net.ExitGame()

	def Close(self):
		self.Hide()
		return True

	def OnPressEscapeKey(self):
		self.Close()
		return True

if getattr(app, "BL_MOVE_CHANNEL", False):
	class MoveChannelDialog(ui.ScriptWindow):
		def __init__(self):
			ui.ScriptWindow.__init__(self)
			self.channelButtonDict = {}
			self.__LoadDialog()

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def __LoadDialog(self):
			try:
				pyScriptLoader = ui.PythonScriptLoader()
				pyScriptLoader.LoadScriptFile(self, "uiscript/movechanneldialog.py")
			except:
				import exception
				exception.Abort("MoveChannelDialog.LoadDialog.LoadScript")

			try:
				self.board = self.GetChild("MoveChannelBoard")
				self.blackBoard = self.GetChild("BlackBoard")
				self.titleBar = self.GetChild("MoveChannelTitle")
				self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
			except:
				import exception
				exception.Abort("MoveChannelDialog.LoadDialog.BindObject")

			channelCount = 4
			channelList = range(1, channelCount + 1)

			btnHeight = 25
			startY = 10

			for ch in channelList:
				btn = ui.Button()
				btn.SetParent(self.blackBoard)
				btn.SetPosition(10, startY + (ch - 1) * (btnHeight + 4))
				btn.SetUpVisual("d:/ymir work/ui/public/XLarge_Button_01.sub")
				btn.SetOverVisual("d:/ymir work/ui/public/XLarge_Button_02.sub")
				btn.SetDownVisual("d:/ymir work/ui/public/XLarge_Button_03.sub")
				btn.SetText("CH %d" % ch)
				btn.SetEvent(ui.__mem_func__(self.__SelectChannel), ch)
				btn.Show()
				self.channelButtonDict[ch] = btn

			boardWidth = 190
			boardHeight = 45 + len(channelList) * (btnHeight + 4) + 15
			self.board.SetSize(boardWidth, boardHeight)
			self.blackBoard.SetSize(boardWidth - 26, boardHeight - 48)
			self.SetSize(boardWidth, boardHeight)

		def __SelectChannel(self, ch):
			self.Close()
			if net.GetChannelNumber() == ch:
				return
			net.MoveChannelGame(ch)

		def Destroy(self):
			self.ClearDictionary()
			self.channelButtonDict = {}

		def Open(self):
			self.Show()
			self.SetTop()

		def Close(self):
			self.Hide()

		def OnPressEscapeKey(self):
			self.Close()
			return True
