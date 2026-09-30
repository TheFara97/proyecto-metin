import ui
import app
import net
import player
import background
import localeInfo
import uiCommon
import uiToolTip
import miniMap
import constInfo
from _weakref import proxy

isShowWindow = False
lastUsedPos = -1
savedWindowX = -1
savedWindowY = -1
savedPage = 0

class AtlasToolTip(uiToolTip.ToolTip):
	class AtlasRender(ui.Window):
		def __init__(self):
			ui.Window.__init__(self)
			self.AddFlag("not_pick")
			self.isLoaded = False

		def __del__(self):
			ui.Window.__del__(self)

		def Show(self):
			ui.Window.Show(self)
			miniMap.ShowToolTipAtlas()

		def Hide(self):
			ui.Window.Hide(self)
			miniMap.HideToolTipAtlas()

		def Load(self, x, y):
			isLoaded = miniMap.LoadToolTipAtlas(x, y)
			self.isLoaded = isLoaded

		def IsLoad(self):
			return self.isLoaded

		def OnRender(self):
			if not self.IsLoad():
				return

			(x, y) = self.GetGlobalPosition()
			miniMap.RenderToolTipAtlas(float(x), float(y))

	def __init__(self):
		uiToolTip.ToolTip.__init__(self)

	def __del__(self):
		uiToolTip.ToolTip.__del__(self)

	def SetAtlasToolTip(self, x, y):
		(mapZone, iGlobalX, iGlobalY) = background.GlobalPositionToMapInfo(x, y)
		self.ClearToolTip()
		self.AppendTextLine("Teleport:", 0xffC4B58B)
		self.AppendTextLine("{} ({}, {})".format(localeInfo.GetMapNameByZone(mapZone), int(x - iGlobalX) / 100, int(y - iGlobalY) / 100))
		self.AppendAtlasRender(x, y)
		self.ShowToolTip()

	def ClearToolTip(self):
		uiToolTip.ToolTip.ClearToolTip(self)
		self.toolTipWidth = 190

	def AppendAtlasRender(self, x, y):
		self.atlasRender = self.AtlasRender()
		self.atlasRender.Load(x, y)
		if self.atlasRender.IsLoad():
			self.atlasRender.SetParent(self)
			self.atlasRender.Show()

			(iSizeX, iSizeY) = miniMap.GetToolTipAtlasSize()
			self.toolTipWidth = max(iSizeX + 20, self.toolTipWidth)
			self.atlasRender.SetPosition((self.toolTipWidth - iSizeX) / 2, self.toolTipHeight)
			self.toolTipHeight += iSizeY

			self.ResizeToolTip()
			self.AlignHorizonalCenter()
				
class SaveLocationWindow(ui.ScriptWindow):
	class LocationElement(ui.ListBoxEx.Item):
		OBJECT_SIZE = (315, 30)

		def	__init__(self, uiparent, parent, key):
			ui.ListBoxEx.Item.__init__(self)

			self.headParent = proxy(parent)
			self.SetParent(uiparent)
			self.key = key
			self.state = False

			self.__BuildObject(key)

		def SetParent(self, parent):
			ui.ListBoxEx.Item.SetParent(self, parent)
			self.parent = proxy(parent)

		def	__del_(self):
			ui.ListBoxEx.Item.__del__(self)

		def	__BuildObject(self, key):
			self.SetSize(*self.OBJECT_SIZE)

			self.Image = ui.ImageBox()
			self.Image.SetParent(self)
			self.Image.LoadImage("Assets/ui/location/unlocked.png")
			self.Image.AddFlag("not_pick")
			self.Image.Show()

			self.Text = ui.TextLine()
			self.Text.SetParent( self.Image )
			self.Text.SetPosition(10, 0)
			self.Text.SetWindowVerticalAlignCenter()
			self.Text.SetVerticalAlignCenter()
			self.Text.SetText("Interesting: ")
			self.Text.Show()

			button = [ui.Button() for _ in range(2)]
			button[0].SetParent( self.Image )
			button[0].SetPosition(250, 4)
			button[0].SetUpVisual  ("Assets/ui/location/save_1.png")
			button[0].SetOverVisual("Assets/ui/location/save_2.png")
			button[0].SetDownVisual("Assets/ui/location/save_1.png")
			button[0].SetEvent(ui.__mem_func__(self.Register))
			button[0].Show()

			button[1].SetParent( self.Image )
			button[1].SetPosition(250 + button[0].GetWidth() + 8, 4)
			button[1].SetUpVisual  ("Assets/ui/location/teleport_1.png")
			button[1].SetOverVisual("Assets/ui/location/teleport_2.png")
			button[1].SetDownVisual("Assets/ui/location/teleport_1.png")
			button[1].SetEvent(ui.__mem_func__(self.Warp))
			button[1].Show()

			self.Buttons = button
			
			self.Show()

		def Register(self):
			if self.state:
				self.headParent.OpenSaveLocationQuestionDialog(self.key)
			else:
				self.headParent.OpenSaveLocationInputNameDialog(net.SAVELOCATION_SUB_HEADER_ADD, self.key)

		def Warp(self):
			if self.state:
				global lastUsedPos
				lastUsedPos = self.key + (self.headParent.savePage * 10)
				self.headParent.SendSaveLocationPacket(net.SAVELOCATION_SUB_HEADER_WARP, self.key)

		def RegisterName(self, text, state, isLastUsed=False):
			self.Text.SetText(text)
			self.state = state
			if isLastUsed:
				self.Text.SetPackedFontColor(0xFFFFD700)
			else:
				self.Text.SetPackedFontColor(0xFFFFFFFF)

	def __init__(self):
		if not app.ENABLE_SAVE_LOCATION_SYSTEM:
			return

		ui.ScriptWindow.__init__(self)

		self.isLoaded = False

		self.xStart = 0
		self.yStart = 0

		self.savePage = 0
		self.Objects = {}
		self.saveLocationDict = {}

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		self.Objects = {}

	def __LoadWindow(self):
		if self.isLoaded:
			return

		self.isLoaded = True

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiScript/SaveLocationWindow.py")
		except:
			pass

		try:
			self.GetChild("board").SetCloseEvent(self.Close)

			self.Objects["ElementContainer"] = self.GetChild("ElementContainer")
			self.Objects["ElementContainer"].SetItemSize(315, 30)
			self.Objects["ElementContainer"].SetItemStep(27)
			self.Objects["ElementContainer"].SetViewItemCount(16)

			for _ in range(10):
				item = self.LocationElement(self.Objects["ElementContainer"], self, _)
				self.Objects["ElementContainer"].AppendItem(item)

			self.Objects["Page1_Button"] = self.GetChild("Page1_Button")
			self.Objects["Page1_Button"].SetEvent(lambda: self.SetPage(0))

			self.Objects["Page2_Button"] = self.GetChild("Page2_Button")
			self.Objects["Page2_Button"].SetEvent(lambda: self.SetPage(1))

		except:
			pass

		self.atlasToolTip = AtlasToolTip()

		self.savePage = 0

	def Show(self):
		global isShowWindow, savedWindowX, savedWindowY, savedPage
		isShowWindow = True

		self.savePage = savedPage
		self.__RefreshPageButtons()
		self.RefreshData()
		ui.ScriptWindow.Show(self)

		if savedWindowX != -1:
			self.SetPosition(savedWindowX, savedWindowY)

		(self.xStart, self.yStart, z) = player.GetMainCharacterPosition()

	def Close(self):
		global isShowWindow, savedWindowX, savedWindowY, savedPage
		isShowWindow = False

		savedWindowX = self.GetLeft()
		savedWindowY = self.GetTop()
		savedPage = self.savePage
		self.Hide()
		
	def Destroy(self):
		self.ClearDictionary()
		self.atlasToolTip = None
		self.xStart = 0
		self.yStart = 0
		self.saveLocationDict = {}

	def IsShow(self):
		global isShowWindow
		return isShowWindow

	def SetPage(self, arg):
		global savedPage
		if arg == 1 and not constInfo.IS_VIP:
			return
		self.savePage = arg
		savedPage = arg
		self.__RefreshPageButtons()
		self.RefreshData()

	def __RefreshPageButtons(self):
		if not self.isLoaded or "Page2_Button" not in self.Objects:
			return
		isVip = constInfo.IS_VIP
		if isVip:
			self.Objects["Page2_Button"].Show()
		else:
			self.Objects["Page2_Button"].Hide()
			if self.savePage == 1:
				self.savePage = 0

	def UpdateSaveLocation(self, pos, name, x, y):
		self.saveLocationDict.update( { pos : [name, x, y] } )
		self.RefreshData()

	def DeleteSaveLocation(self, pos):
		if self.saveLocationDict.has_key(pos):
			self.saveLocationDict.pop(pos)
		self.RefreshData()

	def SendSaveLocationPacket(self, subheader, pos):
		pos += (self.savePage*10)
		if pos >= player.SAVE_LOCATION_MAX:
			return
		net.SendSaveLocationPacket(subheader, pos)

	def OpenSaveLocationInputNameDialog(self, subheader, pos):
		inputDialog = uiCommon.InputDialog()
		inputDialog.SetTitle(localeInfo.SAVE_LOCATION_ENTER_NAME)
		inputDialog.SetMaxLength(12)
		inputDialog.SetAcceptEvent(lambda arg1=subheader, arg2=pos: self.OnAcceptInput(arg1, arg2))
		inputDialog.SetCancelEvent(ui.__mem_func__(self.OnCancelInput))
		inputDialog.Open()

		self.inputDialog  = inputDialog

	def OnAcceptInput(self, subheader, pos):
		if not self.inputDialog:
			return True

		if not len(self.inputDialog.GetText()):
			return True

		pos += (self.savePage*10)
		if pos >= player.SAVE_LOCATION_MAX:
			return True

		net.SendSaveLocationPacket(subheader, pos, self.inputDialog.GetText())
		self.OnCancelInput()
		return True

	def OnCancelInput(self):
		if self.inputDialog:
			self.inputDialog.Close()
			self.inputDialog = None

		return True

	def OpenSaveLocationQuestionDialog(self, pos):
		pos += (self.savePage*10)
		if pos >= player.SAVE_LOCATION_MAX:
			return True

		questionDialog = uiCommon.QuestionDialog()
		questionDialog.SetText(localeInfo.SAVE_LOCATION_REMOVE_LOCATION_QUESTION)
		questionDialog.SetAcceptEvent(lambda arg=pos: self.OnAccept(arg))
		questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCancel))
		questionDialog.Open()

		self.questionDialog = questionDialog

	def OnAccept(self, pos):
		if not self.questionDialog:
			return

		net.SendSaveLocationPacket(net.SAVELOCATION_SUB_HEADER_DEL, pos)
		self.OnCancel()

	def OnCancel(self):
		if self.questionDialog:
			self.questionDialog.Close()
			self.questionDialog = None
			
	def RefreshData(self):
		global lastUsedPos
		for i in xrange(10):
			pos = i + (self.savePage*10)
			item = self.Objects["ElementContainer"].GetItem(i)
			if not item:
				continue

			if self.saveLocationDict.has_key(pos):
				(name, _x, _y) = self.saveLocationDict.get(pos, ("", 0, 0))
				(mapZone, iGlobalX, iGlobalY) = background.GlobalPositionToMapInfo(_x, _y)

				item.RegisterName(str(name), True, pos == lastUsedPos)
			else:
				item.RegisterName("--", False)


	def __OverIn(self, pos):
		if self.saveLocationDict.has_key(pos):
			(_name, x, y) = self.saveLocationDict.get(pos, ("", 0, 0))
			self.atlasToolTip.SetAtlasToolTip(x, y)
			self.atlasToolTip.Show()

	def __OverOut(self):
		self.atlasToolTip.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

