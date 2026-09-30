import app
import ime
import grp
import snd
import wndMgr
import item
import skill
import localeInfo
import dbg
import guild
import constInfo
import sys
import exception

from _weakref import proxy

BACKGROUND_COLOR = grp.GenerateColor(0.0, 0.0, 0.0, 1.0)
DARK_COLOR = grp.GenerateColor(0.2, 0.2, 0.2, 1.0)
BRIGHT_COLOR = grp.GenerateColor(0.7, 0.7, 0.7, 1.0)

if localeInfo.IsCANADA():
	SELECT_COLOR = grp.GenerateColor(0.9, 0.03, 0.01, 0.4)
else:
	SELECT_COLOR = grp.GenerateColor(0.0, 0.0, 0.5, 0.3)

WHITE_COLOR = grp.GenerateColor(1.0, 1.0, 1.0, 0.5)
HALF_WHITE_COLOR = grp.GenerateColor(1.0, 1.0, 1.0, 0.2)

if app.ENABLE_KEYCHANGE_SYSTEM:
	RED_COLOR = grp.GenerateColor(1.0, 0.0, 0.0, 0.5)
	HALF_HALF_RED_COLOR = grp.GenerateColor(1.0, 0.0, 0.0, 0.4)
	HALF_RED_COLOR = grp.GenerateColor(1.0, 0.0, 0.0, 0.3)

createToolTipWindowDict = {}
def RegisterCandidateWindowClass(codePage, candidateWindowClass):
	EditLine.candidateWindowClassDict[codePage]=candidateWindowClass
def RegisterToolTipWindow(type, createToolTipWindow):
	createToolTipWindowDict[type]=createToolTipWindow

if not app.ENABLE_DYNAMIC_FONTS:
	app.SetDefaultFontName(localeInfo.UI_DEF_FONT)



class __mem_func__:
	class __noarg_call__:
		def __init__(self, cls, obj, func):
			self.cls=cls
			self.obj=proxy(obj)
			self.func=proxy(func)

		def __call__(self, *arg):
			return self.func(self.obj)

	class __arg_call__:
		def __init__(self, cls, obj, func):
			self.cls=cls
			self.obj=proxy(obj)
			self.func=proxy(func)

		def __call__(self, *arg):
			return self.func(self.obj, *arg)

	def __init__(self, mfunc):
		if mfunc.im_func.func_code.co_argcount>1:
			self.call=__mem_func__.__arg_call__(mfunc.im_class, mfunc.im_self, mfunc.im_func)
		else:
			self.call=__mem_func__.__noarg_call__(mfunc.im_class, mfunc.im_self, mfunc.im_func)

	def __call__(self, *arg):
		return self.call(*arg)


class Window(object):
	def NoneMethod(cls):
		pass

	NoneMethod = classmethod(NoneMethod)

	def __init__(self, layer = "UI"):
		self.hWnd = None
		self.parentWindow = 0
		self.onMouseLeftButtonUpEvent = None
		self.clickEvent = None
		if app.ENABLE_MOUSEWHEEL_EVENT:
			self.onMouseWheelScrollEvent=None
		self.eventDict={}
		self.eventFunc = {"mouse_click" : None, "mouse_over_in" : None, "MOUSE_OVER_OUT" : None}
		self.eventArgs = {"mouse_click" : None, "mouse_over_in" : None, "MOUSE_OVER_OUT" : None}
		self.argDict={}
		self.RegisterWindow(layer)
		self.mouseLeftButtonDoubleClickEvent = None
		self.mouseRightButtonDownEvent = None
		self.mouseRightButtonDownArgs = None
		if app.ENABLE_WIKI:
			self.exPos = (0,0)
			self.itsRendered = False
			self.itsNeedDoubleRender = False
			self.sortIndex = 0
		self.Hide()
		if app.ENABLE_QUEST_RENEWAL:
			self.propertyList = {}

	def __del__(self):
		wndMgr.Destroy(self.hWnd)

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.Register(self, layer)

	def Destroy(self):
		pass

	def GetWindowHandle(self):
		return self.hWnd

	def AddFlag(self, style):
		wndMgr.AddFlag(self.hWnd, style)

	def SetClickEvent(self, event):
		self.clickEvent = __mem_func__(event)

	def IsRTL(self):
		return wndMgr.IsRTL(self.hWnd)

	def SetWindowName(self, Name):
		wndMgr.SetName(self.hWnd, Name)

	def GetWindowName(self):
		return wndMgr.GetName(self.hWnd)

	def SetParent(self, parent):
		wndMgr.SetParent(self.hWnd, parent.hWnd)

	def SetInsideRender(self, flag):
		wndMgr.SetInsideRender(self.hWnd, flag)

	def SetClipRect(self, left, top, right, bottom, isVertical = False):
		wndMgr.SetClipRect(self.hWnd, left, top, right, bottom, isVertical)

	def SetParentProxy(self, parent):
		self.parentWindow=proxy(parent)
		wndMgr.SetParent(self.hWnd, parent.hWnd)

	def OnMouseOverIn(self):
		try:
			self.eventDict["MOUSE_OVER_IN"]()
		except KeyError:
			pass

	def OnMouseOverOut(self):
		try:
			self.eventDict["MOUSE_OVER_OUT"]()
		except KeyError:
			pass

	def GetParentProxy(self):
		return self.parentWindow

	def SetPickAlways(self):
		wndMgr.SetPickAlways(self.hWnd)

	def SetWindowHorizontalAlignLeft(self):
		wndMgr.SetWindowHorizontalAlign(self.hWnd, wndMgr.HORIZONTAL_ALIGN_LEFT)

	def SetWindowHorizontalAlignCenter(self):
		wndMgr.SetWindowHorizontalAlign(self.hWnd, wndMgr.HORIZONTAL_ALIGN_CENTER)

	def SetWindowHorizontalAlignRight(self):
		wndMgr.SetWindowHorizontalAlign(self.hWnd, wndMgr.HORIZONTAL_ALIGN_RIGHT)

	def SetWindowVerticalAlignTop(self):
		wndMgr.SetWindowVerticalAlign(self.hWnd, wndMgr.VERTICAL_ALIGN_TOP)

	def SetWindowVerticalAlignCenter(self):
		wndMgr.SetWindowVerticalAlign(self.hWnd, wndMgr.VERTICAL_ALIGN_CENTER)

	def SetWindowVerticalAlignBottom(self):
		wndMgr.SetWindowVerticalAlign(self.hWnd, wndMgr.VERTICAL_ALIGN_BOTTOM)

	def SetTop(self):
		wndMgr.SetTop(self.hWnd)

	def Show(self):
		wndMgr.Show(self.hWnd)

	def Hide(self):
		wndMgr.Hide(self.hWnd)

	def SetVisible(self, isVisible):
		if isVisible:
			self.Show()
		else:
			self.Hide()

	def Lock(self):
		wndMgr.Lock(self.hWnd)

	def Unlock(self):
		wndMgr.Unlock(self.hWnd)

	def IsShow(self):
		return wndMgr.IsShow(self.hWnd)

	def UpdateRect(self):
		wndMgr.UpdateRect(self.hWnd)

	def SetSize(self, width, height):
		wndMgr.SetWindowSize(self.hWnd, width, height)

	def GetWidth(self):
		return wndMgr.GetWindowWidth(self.hWnd)

	def GetLeft(self):
		x, y = self.GetLocalPosition()
		return x

	def GetGlobalLeft(self):
		x, y = self.GetGlobalPosition()
		return x

	def GetTop(self):
		x, y = self.GetLocalPosition()
		return y

	def GetGlobalTop(self):
		x, y = self.GetGlobalPosition()
		return y

	def GetRight(self):
		return self.GetLeft() + self.GetWidth()

	def GetBottom(self):
		return self.GetTop() + self.GetHeight()

	def GetHeight(self):
		return wndMgr.GetWindowHeight(self.hWnd)

	def GetSize(self):
		return self.GetWidth(), self.GetHeight(),
		
	def GetLocalPosition(self):
		return wndMgr.GetWindowLocalPosition(self.hWnd)

	def GetGlobalPosition(self):
		return wndMgr.GetWindowGlobalPosition(self.hWnd)

	def GetMouseLocalPosition(self):
		return wndMgr.GetMouseLocalPosition(self.hWnd)

	def GetRect(self):
		return wndMgr.GetWindowRect(self.hWnd)

	def SetPosition(self, x, y, flag = False):
		if flag:  # Change this line - remove the explicit True comparison
			self.exPos = (x,y)
		wndMgr.SetWindowPosition(self.hWnd, x, y)

	#def SetPosition(self, x, y):
	#	wndMgr.SetWindowPosition(self.hWnd, x, y)

	def SetCenterPosition(self, x = 0, y = 0):
		self.SetPosition((wndMgr.GetScreenWidth() - self.GetWidth()) / 2 + x, (wndMgr.GetScreenHeight() - self.GetHeight()) / 2 + y)

	def IsFocus(self):
		return wndMgr.IsFocus(self.hWnd)

	def SetFocus(self):
		wndMgr.SetFocus(self.hWnd)

	def KillFocus(self):
		wndMgr.KillFocus(self.hWnd)

	def GetChildCount(self):
		return wndMgr.GetChildCount(self.hWnd)

	def SetMouseLeftButtonDoubleClickEvent(self, event):
		self.mouseLeftButtonDoubleClickEvent = event

	def OnMouseLeftButtonDoubleClick(self):
		if self.mouseLeftButtonDoubleClickEvent:
			self.mouseLeftButtonDoubleClickEvent()
	
	def SetMouseRightButtonDownEvent(self, event, *args):
		self.mouseRightButtonDownEvent = event
		self.mouseRightButtonDownArgs = args
	def OnMouseRightButtonDown(self):
		if self.mouseRightButtonDownEvent:
			apply(self.mouseRightButtonDownEvent, self.mouseRightButtonDownArgs)

	def IsIn(self):
		return wndMgr.IsIn(self.hWnd)

	def SetOnMouseLeftButtonUpEvent(self, event):
		self.onMouseLeftButtonUpEvent = event

	def OnMouseLeftButtonUp(self):
		if self.onMouseLeftButtonUpEvent:
			self.onMouseLeftButtonUpEvent()
			
	if app.ENABLE_DYNAMIC_FONTS:
		def SetDynamicFont(self, flag):
			wndMgr.SetDynamicFont(self.hWnd, flag)	

	if app.ENABLE_QUEST_RENEWAL:
		def SetProperty(self, propName, propValue):
			self.propertyList[propName] = propValue

		def GetProperty(self, propName):
			if propName in self.propertyList:
				return self.propertyList[propName]

			return None

	if app.ENABLE_MOUSEWHEEL_EVENT:
		def SetMouseWheelScrollEvent(self, event):
			self.onMouseWheelScrollEvent = event
			wndMgr.SetScrollable(self.hWnd)


		def OnMouseWheelScroll(self, mode = "UP"):
			print("OnMouseWheelScroll")
			if self.onMouseWheelScrollEvent:
				self.onMouseWheelScrollEvent(mode)

	if app.ENABLE_CLIP_MASK:	
		def SetClippingMaskRect(self, left, top, right, bottom):
			wndMgr.SetClippingMaskRect(self.hWnd, left, top, right, bottom)
			
		def SetClippingMaskWindow(self, clipping_mask_window):
			wndMgr.SetClippingMaskWindow(self.hWnd, clipping_mask_window.hWnd)

	def IsInPosition(self):
		xMouse, yMouse = wndMgr.GetMousePosition()
		x, y = self.GetGlobalPosition()
		return xMouse >= x and xMouse < x + self.GetWidth() and yMouse >= y and yMouse < y + self.GetHeight()

class CheckBox_Biolog(Window):

	STATE_UNSELECTED = 0
	STATE_SELECTED = 1

	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)

		self.state = self.STATE_UNSELECTED
		self.eventFunc = None
		self.eventArgs = None

		self.overIn = ""

		self.btnBox = {
			self.STATE_UNSELECTED : self.__init_MakeButton("d:/ymir work/ui/checkbox/checkbox_new_unselected.tga"),
			self.STATE_SELECTED : self.__init_MakeButton("d:/ymir work/ui/checkbox/checkbox_new_selected.tga", "d:/ymir work/ui/checkbox/checkbox_new_selected.tga"),
		}

		text = TextLine()
		text.SetParent(self)
		text.SetWindowVerticalAlignCenter()
		text.SetVerticalAlignCenter()
		text.Show()
		self.text = text

		self.__Refresh()

		self.SetWindowName("NONAME_CheckBox")

	def __del__(self):
		Window.__del__(self)

	def __ConvertPath(self, path, subStr):
		if path.find("%s") != -1:
			return path % subStr
		else:
			return path

	def __init_MakeButton(self, path, disablePath = None):
		btn = Button()
		btn.SetParent(self)
		btn.SetWindowVerticalAlignCenter()
		btn.SetUpVisual(self.__ConvertPath(path, "01"))
		btn.SetOverVisual(self.__ConvertPath(path, "02"))
		btn.SetDownVisual(self.__ConvertPath(path, "03"))
		if disablePath:
			btn.SetDisableVisual(disablePath)
		else:
			btn.SetDisableVisual(self.__ConvertPath(path, "01"))
		btn.SAFE_SetEvent(self.OnClickButton)
		btn.baseWidth = btn.GetWidth()
		return btn

	def __UpdateRect(self):
		if self.text.GetText():
			width = self.btnBox[self.state].baseWidth + 5 + self.text.GetTextWidth()
		else:
			width = self.btnBox[self.state].baseWidth
		height = max(self.btnBox[self.state].GetHeight(), self.text.GetTextHeight())
		self.SetSize(width, height)

		self.btnBox[self.state].SetSize(width, self.btnBox[self.state].GetHeight())
		self.text.SetPosition(self.btnBox[self.state].baseWidth + 5, 0)

		self.text.UpdateRect()
		self.btnBox[self.state].UpdateRect()
		self.UpdateRect()

	def __Refresh(self):
		self.__UpdateRect()

		self.btnBox[self.STATE_UNSELECTED].SetVisible(self.state == self.STATE_UNSELECTED)
		self.btnBox[self.STATE_SELECTED].SetVisible(self.state == self.STATE_SELECTED)

	def SAFE_SetOverInData(self, data):
		self.btnBox[self.state].SetToolTipText(data)

	def OnClickButton(self):
		if self.state == self.STATE_UNSELECTED:
			self.state = self.STATE_SELECTED
		else:
			self.state = self.STATE_UNSELECTED

		self.__Refresh()

		if self.eventFunc:
			apply(self.eventFunc, self.eventArgs)

	def SetChecked(self, state):
		self.state = state
		self.__Refresh()

	def IsChecked(self):
		return self.state != self.STATE_UNSELECTED

	def SetText(self, text):
		self.text.SetText(text)
		self.__UpdateRect()

	def SetEvent(self, event, *args):
		self.eventFunc = event
		self.eventArgs = args

	def SAFE_SetEvent(self, event, *args):
		self.eventFunc = __mem_func__(event)
		self.eventArgs = args

	def Disable(self):
		self.btnBox[self.STATE_UNSELECTED].Disable()
		self.btnBox[self.STATE_SELECTED].Disable()

	def Enable(self):
		self.btnBox[self.STATE_UNSELECTED].Enable()
		self.btnBox[self.STATE_SELECTED].Enable()

class BoxedBoard(Window):
	BORDER_TOP = 0
	BORDER_RIGHT = 1
	BORDER_BOTTOM = 2
	BORDER_LEFT = 3
	
	DEFAULT_BORDER_COLOR = grp.GenerateColor(0.3, 0.3, 0.3, 1.0)
	DEFAULT_BASE_COLOR = grp.GenerateColor(0, 0, 0, 0.5)
	
	def __init__(self):
		Window.__init__(self)
		
		self.borderSize = 1
		
		self.borders = [
			Bar(),
			Bar(),
			Bar(),
			Bar()
		]
		
		for border in self.borders:
			border.SetParent(self)
			border.AddFlag("not_pick")
			border.Show()
		
		self.base = Bar()
		self.base.SetParent(self)
		self.base.AddFlag("not_pick")
		self.base.Show()
		
		self.SetBorderColor(self.DEFAULT_BORDER_COLOR)
		self.SetBaseColor(self.DEFAULT_BASE_COLOR)
	
	def __del__(self):
		self.Destroy()
		Window.__del__(self)
	
	def Destroy(self):
		del self.borders[:]
		self.base = None
		
		Window.Destroy(self)
	
	def SetBorderColor(self, color):
		for border in self.borders:
			border.SetColor(color)
	
	def SetBorderSize(self, borderSize):
		self.borderSize = borderSize
		self.SetSize(self.GetWidth(), self.GetHeight())
	
	def SetBaseColor(self, color):
		self.base.SetColor(color)
	
	def SetSize(self, width, height):
		width = max(width, (2 * self.borderSize) + 1)
		height = max(height, (2 * self.borderSize) + 1)
		
		Window.SetSize(self, width, height)
		self.UpdateBoard()
	
	def UpdateBoard(self):
		width = self.GetWidth()
		height = self.GetHeight()
		
		top, right, bottom, left = self.borders
		
		top.SetSize(width - self.borderSize, self.borderSize)
		
		right.SetSize(self.borderSize, height - self.borderSize)
		right.SetPosition(width - self.borderSize, 0)
		
		bottom.SetSize(width - self.borderSize, self.borderSize)
		bottom.SetPosition(self.borderSize, height - self.borderSize)
		
		left.SetSize(self.borderSize, height - self.borderSize)
		left.SetPosition(0, self.borderSize)
		
		self.base.SetSize(width - (2 * self.borderSize), height - (2 * self.borderSize))
		self.base.SetPosition(self.borderSize, self.borderSize)


class AutoGrowingVerticalContainer(Window):
	def __init__(self, layer="UI"):
		Window.__init__(self, layer)
		self.containerItems = []
		self.width = 0
		self.height = 0

		self.elementOffSetX = 0
		self.elementOffSetY = 0

		self.elementStartOffSetY = 0

	def SetWidth(self, width):
		self.width = width

	def SetElementOffset(self, offsetY):
		self.elementOffSetY = offsetY

	def AppendItem(self, item):
		self.height += item.GetHeight() + self.elementOffSetY
		self.containerItems.append(item)
		self.Update()

	def Update(self):
		self.SetSize(self.width, self.height)
		self.UpdateRect()

	def RecalculateHeight(self):
		self.height = 0
		for item in self.containerItems:
			self.height = self.height + item.GetHeight() + self.elementOffSetY
		self.Update()

	def GetElementByIndex(self, index):
		if index >= len(self.containerItems):
			return None

		return self.containerItems[index]

	def GetElementsCount(self):
		return len(self.containerItems)

	def GetElements(self):
		return self.containerItems

	def ClearItems(self):
		self.containerItems[:] = []
		self.RecalculateHeight()

class ListBoxEx(Window):

	class Item(Window):
		def __init__(self):
			Window.__init__(self)
			self.selectedRenderColor = grp.GenerateColor(0.0, 0.0, 0.7, 0.7)

		def __del__(self):
			Window.__del__(self)

		def SetParent(self, parent):
			Window.SetParent(self, parent)
			self.parent=proxy(parent)

		def OnMouseLeftButtonDown(self):
			self.parent.SelectItem(self)

		def OnRender(self):
			if self.parent.GetSelectedItem()==self:
				self.OnSelectedRender()

		def OnSelect(self):
			pass

		def OnUnselect(self):
			pass
		
		def SetSelectedRenderColor(self, color):
			self.selectedRenderColor = color
		
		def OnSelectedRender(self):
			if self.selectedRenderColor:
				x, y = self.GetGlobalPosition()
				grp.SetColor(self.selectedRenderColor)
				grp.RenderBar(x, y, self.GetWidth(), self.GetHeight())

	def __init__(self):
		Window.__init__(self)

		self.viewItemCount=10
		self.basePos=0
		self.itemHeight=16
		self.itemStep=20
		self.selItem=0
		self.itemList=[]
		self.onSelectItemEvent = lambda *arg: None

		if localeInfo.IsARABIC():
			self.itemWidth=130
		else:
			self.itemWidth=100

		self.scrollBar=None
		self.isModernScrollBar = False
		self.__UpdateSize()

	def __del__(self):
		Window.__del__(self)

	def __UpdateSize(self):
		height=self.itemStep*self.__GetViewItemCount()

		self.SetSize(self.itemWidth, height)

	def IsEmpty(self):
		if len(self.itemList)==0:
			return 1
		return 0

	def SetItemStep(self, itemStep):
		self.itemStep=itemStep
		self.__UpdateSize()

	def SetItemSize(self, itemWidth, itemHeight):
		self.itemWidth=itemWidth
		self.itemHeight=itemHeight
		self.__UpdateSize()

	def SetViewItemCount(self, viewItemCount):
		self.viewItemCount=viewItemCount

	def SetSelectEvent(self, event):
		self.onSelectItemEvent = event

	def SetBasePos(self, basePos):
		for oldItem in self.itemList[self.basePos:self.basePos+self.viewItemCount]:
			oldItem.Hide()

		self.basePos=basePos

		pos=basePos
		for newItem in self.itemList[self.basePos:self.basePos+self.viewItemCount]:
			(x, y)=self.GetItemViewCoord(pos, newItem.GetWidth())
			newItem.SetPosition(x, y)
			newItem.Show()
			pos+=1

	def GetItemIndex(self, argItem):
		return self.itemList.index(argItem)

	def GetSelectedItem(self):
		return self.selItem

	def SelectIndex(self, index):

		if index >= len(self.itemList) or index < 0:
			self.selItem = None
			return
			
		oldSelItem = self.selItem
		
		try:
			self.selItem=self.itemList[index]
		except:
			pass
			
		if oldSelItem:
			oldSelItem.OnUnselect()
			
		self.selItem.OnSelect()

	def SelectItem(self, selItem):
		if self.selItem: 
			self.selItem.OnUnselect() 
			
		self.selItem=selItem
		selItem.OnSelect() 
		self.onSelectItemEvent(selItem)

	def RemoveAllItems(self):
		for i in self.itemList:
			if i.IsShow():
				i.Hide()

		if self.selItem:
			self.selItem.OnUnselect()
			
		self.selItem=None
		del self.itemList[:]
		self.itemList=[]

		if self.scrollBar:
			self.scrollBar.SetPos(0)

	def RemoveItem(self, delItem):
		if delItem==self.selItem:
			delItem.OnUnselect()
			self.selItem=None

		self.itemList.remove(delItem)
		
		self.UpdateContentHeight()


	def AppendButton(self, newItem, isButton = False):
		newItem.SetParent(self)
		newItem.SetSize(self.itemWidth, self.itemHeight)

		pos=len(self.itemList)
		if self.__IsInViewRange(pos):
			(x, y)=self.GetItemViewCoord(pos, newItem.GetWidth())
			newItem.SetPosition(x, y)
			newItem.Show()
		else:
			newItem.Hide()
		
		if isButton:
			newItem.SAFE_SetEvent(self.SelectItem, proxy(newItem))
			
		self.itemList.append(newItem)

	def AppendItem(self, newItem, bSorted = False, sortKey = None):
		newItem.SetParent(self)
		newItem.SetSize(self.itemWidth, self.itemHeight)

		pos=len(self.itemList)
		if self.__IsInViewRange(pos):
			(x, y)=self.GetItemViewCoord(pos, newItem.GetWidth())
			newItem.SetPosition(x, y)
			newItem.Show()
		else:
			newItem.Hide()

		self.itemList.append(newItem)

		if bSorted:
			self.itemList.sort(key = lambda rItem : rItem.bDone)

			self.SetBasePos(0)

	def SetScrollBar(self, scrollBar, isModern = False):
		scrollBar.SetScrollEvent(__mem_func__(self.__OnScroll))
		self.scrollBar=scrollBar
		self.isModernScrollBar = isModern

	def __OnScroll(self):
		self.SetBasePos(int(self.scrollBar.GetPos()*self.__GetScrollLen()))

	def __GetScrollLen(self):
		scrollLen=self.__GetItemCount()-self.__GetViewItemCount()
		if scrollLen<0:
			return 0

		return scrollLen
	
	def GetScrollLen(self):
		return self.__GetScrollLen()

	def __GetViewItemCount(self):
		return self.viewItemCount

	def GetViewItemCount(self):
		return self.__GetViewItemCount()
	
	def __GetItemCount(self):
		return len(self.itemList)
	
	def GetItemCount(self):
		return self.__GetItemCount()

	GetItemCount = __GetItemCount

	def GetItem(self, index):
		if index >= len(self.itemList) or index < 0:
			pass
		else:
			return self.itemList[index]
		
	def GetItems(self):
		return self.itemList

	def GetItemViewCoord(self, pos, itemWidth):
		if localeInfo.IsARABIC():
			return (self.GetWidth()-itemWidth-10, (pos-self.basePos)*self.itemStep)
		else:
			return (0, (pos-self.basePos)*self.itemStep)

	def __IsInViewRange(self, pos):
		if pos<self.basePos:
			return 0
		if pos>=self.basePos+self.viewItemCount:
			return 0
		return 1

	def UpdateContentHeight(self):
		if not self.isModernScrollBar:
			return
		
		self.scrollBar.SetContentHeight(self.__GetItemCount() * self.itemStep)
	
	def OnMouseWheel(self, len):
		if not self.scrollBar:
			return False
		
		if not self.isModernScrollBar:
			return False
		
		return self.scrollBar.OnMouseWheel(len)

class CandidateListBox(ListBoxEx):

	HORIZONTAL_MODE = 0
	VERTICAL_MODE = 1

	class Item(ListBoxEx.Item):
		def __init__(self, text):
			ListBoxEx.Item.__init__(self)

			self.textBox=TextLine()
			self.textBox.SetParent(self)
			self.textBox.SetText(text)
			self.textBox.Show()

		def __del__(self):
			ListBoxEx.Item.__del__(self)

	def __init__(self, mode = HORIZONTAL_MODE):
		ListBoxEx.__init__(self)
		self.itemWidth=32
		self.itemHeight=32
		self.mode = mode

	def __del__(self):
		ListBoxEx.__del__(self)

	def SetMode(self, mode):
		self.mode = mode

	def AppendItem(self, newItem):
		ListBoxEx.AppendItem(self, newItem)

	def GetItemViewCoord(self, pos):
		if self.mode == self.HORIZONTAL_MODE:
			return ((pos-self.basePos)*self.itemStep, 0)
		elif self.mode == self.VERTICAL_MODE:
			return (0, (pos-self.basePos)*self.itemStep)


class TextLine(Window):
	def __init__(self, font = None):
		Window.__init__(self)
		self.max = 0
		if font == None:
			self.SetFontName(localeInfo.UI_DEF_FONT)
		else:
			self.SetFontName(font)

	def __del__(self):
		Window.__del__(self)

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterTextLine(self, layer)

	def SetMax(self, max):
		wndMgr.SetMax(self.hWnd, max)

	def SetLimitWidth(self, width):
		wndMgr.SetLimitWidth(self.hWnd, width)

	def SetMultiLine(self):
		wndMgr.SetMultiLine(self.hWnd, True)

	def SetHorizontalAlignArabic(self):
		wndMgr.SetHorizontalAlign(self.hWnd, wndMgr.TEXT_HORIZONTAL_ALIGN_ARABIC)

	def SetHorizontalAlignLeft(self):
		wndMgr.SetHorizontalAlign(self.hWnd, wndMgr.TEXT_HORIZONTAL_ALIGN_LEFT)

	def SetHorizontalAlignRight(self):
		wndMgr.SetHorizontalAlign(self.hWnd, wndMgr.TEXT_HORIZONTAL_ALIGN_RIGHT)

	def SetHorizontalAlignCenter(self):
		wndMgr.SetHorizontalAlign(self.hWnd, wndMgr.TEXT_HORIZONTAL_ALIGN_CENTER)

	def SetVerticalAlignTop(self):
		wndMgr.SetVerticalAlign(self.hWnd, wndMgr.TEXT_VERTICAL_ALIGN_TOP)

	def SetVerticalAlignBottom(self):
		wndMgr.SetVerticalAlign(self.hWnd, wndMgr.TEXT_VERTICAL_ALIGN_BOTTOM)

	def SetVerticalAlignCenter(self):
		wndMgr.SetVerticalAlign(self.hWnd, wndMgr.TEXT_VERTICAL_ALIGN_CENTER)

	def SetSecret(self, Value=True):
		wndMgr.SetSecret(self.hWnd, Value)

	def SetOutline(self, Value=True):
		wndMgr.SetOutline(self.hWnd, Value)

	def SetFeather(self, value=True):
		wndMgr.SetFeather(self.hWnd, value)

	def SetFontName(self, fontName):
		wndMgr.SetFontName(self.hWnd, fontName)

	def SetDefaultFontName(self):
		wndMgr.SetFontName(self.hWnd, localeInfo.UI_DEF_FONT)

	def SetFontColor(self, red, green, blue):
		wndMgr.SetFontColor(self.hWnd, red, green, blue)

	def SetTextColor(self, color):
		self.SetPackedFontColor(color)

	def SetPackedFontColor(self, color):
		wndMgr.SetFontColor(self.hWnd, color)

	def SetText(self, text):
		wndMgr.SetText(self.hWnd, text)

	def GetText(self):
		return wndMgr.GetText(self.hWnd)

	def GetTextSize(self):
		return wndMgr.GetTextSize(self.hWnd)

	def GetTextWidth(self):
		(w, h) = self.GetTextSize()
		return w

	def GetTextHeight(self):
		(w, h) = self.GetTextSize()
		return h

	if app.WJ_MULTI_TEXTLINE:
		def SetEnterToken(self, bool):
			wndMgr.SetEnterToken(self.hWnd, bool)

		def SetLineHeight(self, Height):
			wndMgr.SetLineHeight(self.hWnd, Height)

		def GetTextLineCount(self):
			return wndMgr.GetTextLineCount(self.hWnd)

		def GetLineHeight(self):
			return wndMgr.GetLineHeight(self.hWnd)

class EmptyCandidateWindow(Window):
	def __init__(self):
		Window.__init__(self)

	def __del__(self):
		Window.__init__(self)

	def Load(self):
		pass

	def SetCandidatePosition(self, x, y, textCount):
		pass

	def Clear(self):
		pass

	def Append(self, text):
		pass

	def Refresh(self):
		pass

	def Select(self):
		pass

class EditLine(TextLine):
	candidateWindowClassDict = {}

	def __init__(self):
		TextLine.__init__(self)

		self.eventReturn = Window.NoneMethod
		self.eventUpdate = Window.NoneMethod
		self.eventEscape = Window.NoneMethod
		self.eventUpdateArgs = None
		self.eventReturnArgs = None
		self.eventEscapeArgs = None
		self.eventTab = None
		self.numberMode = False
		self.useIME = True

		self.bCodePage = False

		self.infoMsg = ""
		self.backText=None
		self.isNeedEmpty=True

		self.candidateWindowClass = None
		self.candidateWindow = None
		self.SetCodePage(app.GetDefaultCodePage())

		self.readingWnd = ReadingWnd()
		self.readingWnd.Hide()

		self.backgroundText = TextLine()
		self.backgroundText.SetParent(self)
		self.backgroundText.SetPosition(0, 0)
		self.backgroundText.SetPackedFontColor(WHITE_COLOR)
		self.backgroundText.Hide()

		self.eventUpdate = None
		
		self.overLay = TextLine()
		self.overLay.SetParent(self)
		self.overLay.SetPosition(0, 0)
		self.overLay.SetPackedFontColor(WHITE_COLOR)
		self.overLay.Hide()

	def __del__(self):
		TextLine.__del__(self)

		self.eventReturn = Window.NoneMethod
		self.eventUpdate = Window.NoneMethod
		self.eventEscape = Window.NoneMethod
		self.eventUpdateArgs = None
		self.eventReturnArgs = None
		self.eventEscapeArgs = None
		self.eventTab = None
		self.backgroundText = None


	def SetCodePage(self, codePage):
		candidateWindowClass=EditLine.candidateWindowClassDict.get(codePage, EmptyCandidateWindow)
		self.__SetCandidateClass(candidateWindowClass)

	def __SetCandidateClass(self, candidateWindowClass):
		if self.candidateWindowClass==candidateWindowClass:
			return

		self.candidateWindowClass = candidateWindowClass
		self.candidateWindow = self.candidateWindowClass()
		self.candidateWindow.Load()
		self.candidateWindow.Hide()

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterTextLine(self, layer)

	def SAFE_SetReturnEvent(self, event, *args):
		self.eventReturn = __mem_func__(event)
		self.eventReturnArgs = args

	def SetReturnEvent(self, event, *args):
		self.eventReturn = event
		self.eventReturnArgs = args

	def SAFE_SetUpdateEvent(self, event, *args):
		self.eventUpdate = __mem_func__(event)		
		self.eventUpdateArgs = args
		
	def SetUpdateEvent(self, event, *args):
		self.eventUpdate = event
		self.eventUpdateArgs = args

	def SetEscapeEvent(self, event, *args):
		self.eventEscape = event
		self.eventEscapeArgs = args

	def SetTabEvent(self, event, *args):
		self.eventTab = event
		self.eventArgs = args
		
	def SetBackgroundText(self, text):
		self.backgroundText.SetText(text)
		
		if not self.backgroundText.IsShow():
			self.backgroundText.Show()
		
	def GetBackgroundText(self):
		return self.backgroundText.GetText()

	def SetTabEvent(self, event):
		self.eventTab = event
		
	def SetInfoMessage(self, msg, font="Verdana:14"):
		self.infoMsg = msg
		if self.backText == None:
			self.backText = TextLine()
			self.backText.SetParent(self)
			self.backText.SetPosition(0,0)
			self.backText.SetFontColor(128,128,128)
			self.backText.SetFontName(font)
		self.backText.SetText(msg)

		if self.isNeedEmpty:
			if len(self.GetText()) > 0:
				self.backText.Hide()
			else:
				self.backText.Show()
		else:
			self.backText.Show()

	def SetOverlayText(self, text):
		self.overLay.SetText(text)
		self.__RefreshOverlay()
	
	def GetOverlayText(self):
		return self.overLay.GetText()
	
	def __RefreshOverlay(self):
		if len(self.GetText()):
			self.overLay.Hide()
		else:
			self.overLay.Show()
	
	def IsShowCursor(self):
		return wndMgr.IsShowCursor(self.hWnd)

	def SetMax(self, max):
		self.max = max
		wndMgr.SetMax(self.hWnd, self.max)
		ime.SetMax(self.max)
		self.SetUserMax(self.max)

	def SetUserMax(self, max):
		self.userMax = max
		ime.SetUserMax(self.userMax)

	def SetNumberMode(self):
		if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
			self.inputMode = ime.MODE_NUMBER
		else:
			self.numberMode = True

	if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
		def SetCurrencyMode(self):
			self.inputMode = ime.MODE_CURRENCY



	def SetIMEFlag(self, flag):
		self.useIME = flag

	def SetText(self, text):
		wndMgr.SetText(self.hWnd, text)

		if self.IsFocus():
			ime.SetText(text)

	def CanEdit(self, flag):
		self.CanClick = flag

	def Enable(self):
		wndMgr.ShowCursor(self.hWnd)

	def Disable(self):
		wndMgr.HideCursor(self.hWnd)

	def SetEndPosition(self):
		ime.MoveEnd()

	def OnSetFocus(self):
		Text = self.GetText()
		ime.SetText(Text)
		ime.SetMax(self.max)
		ime.SetUserMax(self.userMax)
		ime.SetCursorPosition(-1)
		if self.numberMode:
			ime.SetNumberMode()
		else:
			ime.SetStringMode()
		ime.EnableCaptureInput()
		if self.useIME:
			ime.EnableIME()
		else:
			ime.DisableIME()
		wndMgr.ShowCursor(self.hWnd, True)

	def OnKillFocus(self):
		self.SetText(ime.GetText(self.bCodePage))
		self.OnIMECloseCandidateList()
		self.OnIMECloseReadingWnd()
		ime.DisableIME()
		ime.DisableCaptureInput()
		wndMgr.HideCursor(self.hWnd)

	def OnIMEChangeCodePage(self):
		self.SetCodePage(ime.GetCodePage())

	def OnIMEOpenCandidateList(self):
		self.candidateWindow.Show()
		self.candidateWindow.Clear()
		self.candidateWindow.Refresh()

		gx, gy = self.GetGlobalPosition()
		self.candidateWindow.SetCandidatePosition(gx, gy, len(self.GetText()))

		return True

	def OnIMECloseCandidateList(self):
		self.candidateWindow.Hide()
		return True

	def OnIMEOpenReadingWnd(self):
		gx, gy = self.GetGlobalPosition()
		textlen = len(self.GetText())-2
		reading = ime.GetReading()
		readinglen = len(reading)
		self.readingWnd.SetReadingPosition( gx + textlen*6-24-readinglen*6, gy )
		self.readingWnd.SetText(reading)
		if ime.GetReadingError() == 0:
			self.readingWnd.SetTextColor(0xffffffff)
		else:
			self.readingWnd.SetTextColor(0xffff0000)
		self.readingWnd.SetSize(readinglen * 6 + 4, 19)
		self.readingWnd.Show()
		return True

	def OnIMECloseReadingWnd(self):
		self.readingWnd.Hide()
		return True

	def OnIMEUpdate(self):
		snd.PlaySound("sound/ui/type.wav")
		TextLine.SetText(self, ime.GetText(self.bCodePage))

		if self.eventUpdate:
			if self.eventUpdateArgs:
				apply(self.eventUpdate, self.eventUpdateArgs)
			else:
				self.eventUpdate()

		self.__RefreshOverlay()

		if self.backText:
			if self.isNeedEmpty:
				if len(self.GetText()) > 0:
					self.backText.Hide()
				else:
					self.backText.Show()
			else:
				self.backText.Show()

	def OnIMETab(self):
		if self.eventTab:
			self.eventTab()
			return True

		return False

	def OnIMEReturn(self):
		snd.PlaySound("sound/ui/click.wav")
		if self.eventReturn:
			if self.eventReturnArgs:
				apply(self.eventReturn, self.eventReturnArgs)
			else:
				self.eventReturn()
				
		return True

	def OnPressEscapeKey(self):
		if self.eventEscape:
			if self.eventEscapeArgs:
				apply(self.eventEscape, self.eventEscapeArgs)
			else:
				self.eventEscape()
		return True

	def OnKeyDown(self, key):
		if key in range(app.DIK_F1, app.DIK_F12):
			return False
		if app.DIK_LALT == key:
			return False
		if app.DIK_SYSRQ == key:
			return False
		if app.DIK_LCONTROL == key:
			return False
		if app.DIK_V == key:
			if app.IsPressed(app.DIK_LCONTROL):
				ime.PasteTextFromClipBoard()

		return True

	def OnKeyUp(self, key):
		if key in range(app.DIK_F1, app.DIK_F12):
			return False
		if app.DIK_LALT == key:
			return False
		if app.DIK_SYSRQ == key:
			return False
		if app.DIK_LCONTROL == key:
			return False

		return True

	def OnIMEKeyDown(self, key):
		if app.VK_LEFT == key:
			ime.MoveLeft()
			return True
		if app.VK_RIGHT == key:
			ime.MoveRight()
			return True

		if app.VK_HOME == key:
			ime.MoveHome()
			return True
		if app.VK_END == key:
			ime.MoveEnd()
			return True

		if app.VK_DELETE == key:
			ime.Delete()
			TextLine.SetText(self, ime.GetText(self.bCodePage))
			return True

		return True

	def OnMouseLeftButtonDown(self):
		if False == self.IsIn():
			return False

		self.SetFocus()
		PixelPosition = wndMgr.GetCursorPosition(self.hWnd)
		ime.SetCursorPosition(PixelPosition)

class MarkBox(Window):
	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)

	def __del__(self):
		Window.__del__(self)

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterMarkBox(self, layer)

	def Load(self):
		wndMgr.MarkBox_Load(self.hWnd)

	def SetScale(self, scale):
		wndMgr.MarkBox_SetScale(self.hWnd, scale)

	def SetIndex(self, guildID):
		MarkID = guild.GuildIDToMarkID(guildID)
		wndMgr.MarkBox_SetImageFilename(self.hWnd, guild.GetMarkImageFilenameByMarkID(MarkID))
		wndMgr.MarkBox_SetIndex(self.hWnd, guild.GetMarkIndexByMarkID(MarkID))

	def SetAlpha(self, alpha):
		wndMgr.MarkBox_SetDiffuseColor(self.hWnd, 1.0, 1.0, 1.0, alpha)

class ImageBox(Window):
	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)
		self.eventDict={}
		self.eventFunc = {"mouse_click" : None, "mouse_over_in" : None, "MOUSE_OVER_OUT" : None}
		self.eventArgs = {"mouse_click" : None, "mouse_over_in" : None, "MOUSE_OVER_OUT" : None}
		self.argDict={}
		self.ToolTipTextNew = None
		self.vasto = None
		self.a = 1.0
	def __del__(self):
		Window.__del__(self)
		self.eventFunc = None
		self.eventArgs = None
	def GetAlpha(self):
		return self.a
	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterImageBox(self, layer)
	def LoadImage(self, imageName):
		self.name=imageName
		wndMgr.LoadImage(self.hWnd, imageName)
		if len(self.eventDict)!=0:
			print "LOAD IMAGE", self, self.eventDict
	def SetAlpha(self, alpha):
		wndMgr.SetDiffuseColor(self.hWnd, 1.0, 1.0, 1.0, alpha)
	def GetWidth(self):
		return wndMgr.GetWidth(self.hWnd)
	def GetHeight(self):
		return wndMgr.GetHeight(self.hWnd)
	def SetFormToolTipNewText(self, type, text, x, y):
		if not self.ToolTipTextNew:
			self.vasto = ThinBoardNew()
			self.vasto.SetParent(self)
			self.vasto.SetWindowHorizontalAlignCenter()
			self.vasto.AddFlag("not_pick")
			self.vasto.Hide()
			if y == 0:
				self.vasto.SetPosition(x, y-25)
			else:
				self.vasto.SetPosition(x, y)
			toolTip=createToolTipWindowDict[type]()
			toolTip.SetParent(self.vasto)
			toolTip.SetSize(0, 0)
			toolTip.SetWindowVerticalAlignCenter()
			toolTip.SetVerticalAlignCenter()
			toolTip.SetOutline()
			toolTip.Hide()
			self.ToolTipTextNew=toolTip
		self.ToolTipTextNew.SetText(text)
		self.vasto.SetSize(self.ToolTipTextNew.GetTextWidth()+30,3)
		toolTip.SetPosition(15,-1)
	def SetToolTipWindow(self, toolTip):
		self.ToolTipText=toolTip
		self.ToolTipText.SetParentProxy(self)
	def SetToolTipTextNewColor(self, color):
		self.ToolTipTextNew.SetPackedFontColor(color)
	def SetToolTipTextNew(self, text, x=0, y = -19):
		self.SetFormToolTipNewText("TEXT", text, x, y)
	def SAFE_SetStringEvent(self, event, func, *args):
		self.eventDict[event]=__mem_func__(func)
		self.argDict[event]=args
	def SetEvent(self, func, *args):
		result = self.eventFunc.has_key(args[0])
		if result:
			self.eventFunc[args[0]] = func
			self.eventArgs[args[0]] = args
		else:
			print "[ERROR] ui.py SetEvent, Can`t Find has_key : %s" % args[0]
	def OnMouseLeftButtonUp(self) :
		if self.eventFunc["mouse_click"] :
			apply(self.eventFunc["mouse_click"], self.eventArgs["mouse_click"])
	def SetStringEvent(self, event, func, *args):
		self.eventDict[event]=func
		self.argDict[event]=args
	def OnMouseOverIn(self) :
		if self.ToolTipTextNew:
			if not self.ToolTipTextNew.IsShow():
				self.vasto.Show()
				self.ToolTipTextNew.Show()
			if self.vasto.IsIn():
				self.vasto.Hide()
				self.ToolTipTextNew.Hide()
		if self.eventFunc["mouse_over_in"] :
			apply(self.eventFunc["mouse_over_in"], self.eventArgs["mouse_over_in"])
		elif self.eventDict.has_key("MOUSE_OVER_IN"):
			if self.argDict.has_key("MOUSE_OVER_IN") and self.argDict["MOUSE_OVER_IN"]:
				apply(self.eventDict["MOUSE_OVER_IN"], self.argDict["MOUSE_OVER_IN"])
			else:
				self.eventDict["MOUSE_OVER_IN"]()
	def OnMouseOverOut(self):
		if self.ToolTipTextNew:
			if self.ToolTipTextNew.IsShow():
				self.vasto.Hide()
				self.ToolTipTextNew.Hide()
		if self.eventFunc["MOUSE_OVER_OUT"]:
			apply(self.eventFunc["MOUSE_OVER_OUT"], self.eventArgs["MOUSE_OVER_OUT"])
		elif self.eventDict.has_key("MOUSE_OVER_OUT"):
			if self.argDict.has_key("MOUSE_OVER_OUT") and self.argDict["MOUSE_OVER_OUT"]:
				apply(self.eventDict["MOUSE_OVER_OUT"], self.argDict["MOUSE_OVER_OUT"])
			else:
				self.eventDict["MOUSE_OVER_OUT"]()
	
	def SAFE_SetMouseClickEvent(self, func, *args):
		self.eventDict["MOUSE_LEFT_DOWN"]=__mem_func__(func)
		self.argDict["MOUSE_LEFT_DOWN"]=args
	def SetCoolTime(self, time):
		wndMgr.SetCoolTimeImageBox(self.hWnd, time)
	def SetStartCoolTime(self, time):
		wndMgr.SetStartCoolTimeImageBox(self.hWnd, time)


class ExpandedImageBox(ImageBox):
	def __init__(self, layer = "UI"):
		ImageBox.__init__(self, layer)
		self.eventDict={}
		self.eventFunc = {"mouse_click" : None, "mouse_over_in" : None, "MOUSE_OVER_OUT" : None}
		self.eventArgs = {"mouse_click" : None, "mouse_over_in" : None, "MOUSE_OVER_OUT" : None}
		self.eventDict2={}
		self.xScale = 1.0
		self.yScale = 1.0

	def __del__(self):
		ImageBox.__del__(self)
		self.eventFunc = None
		self.eventArgs = None

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterExpandedImageBox(self, layer)

	def SetScale(self, xScale, yScale):
		wndMgr.SetScale(self.hWnd, xScale, yScale)
		self.xScale = xScale
		self.yScale = yScale

	def GetScale(self):
		return (self.xScale, self.yScale)

	def SetOrigin(self, x, y):
		wndMgr.SetOrigin(self.hWnd, x, y)

	def SetRotation(self, rotation):
		wndMgr.SetRotation(self.hWnd, rotation)

	def SetRenderingMode(self, mode):
		wndMgr.SetRenderingMode(self.hWnd, mode)

	def SetRenderingRect(self, left, top, right, bottom):
		wndMgr.SetRenderingRect(self.hWnd, left, top, right, bottom)

	def SetClipRect(self, left, top, right, bottom, isVertical = False):
		wndMgr.SetClipRect(self.hWnd, left, top, right, bottom, isVertical)

	def SetPercentage(self, curValue, maxValue):
		if maxValue:
			self.SetRenderingRect(0.0, 0.0, -1.0 + float(curValue) / float(maxValue), 0.0)
		else:
			self.SetRenderingRect(0.0, 0.0, 0.0, 0.0)

	def SetPercentageEx(self, minValue, curValue, maxValue):
		if minValue and maxValue:
			self.SetRenderingRect(0.0, 0.0, -1.0 + (float(curValue) - float(minValue)) / (float(maxValue) - float(minValue)), 0.0)
		else:
			self.SetRenderingRect(0.0, 0.0, 0.0, 0.0)

	def GetWidth(self):
		return wndMgr.GetWindowWidth(self.hWnd)

	def GetHeight(self):
		return wndMgr.GetWindowHeight(self.hWnd)

	if app.ENABLE_WIKI:
		def SetEvent(self, func, *args) :
			result = self.eventFunc.has_key(args[0])
			if result :
				self.eventFunc[args[0]] = func
				self.eventArgs[args[0]] = args
			else :
				print "[ERROR] ui.py SetEvent, Can`t Find has_key : %s" % args[0]

class AniImageBox(Window):
	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)
		self.eventEndFrame = None

	def __del__(self):
		Window.__del__(self)
		self.eventEndFrame = None

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterAniImageBox(self, layer)

	def SetDelay(self, delay):
		wndMgr.SetDelay(self.hWnd, delay)

	def AppendImage(self, filename):
		wndMgr.AppendImage(self.hWnd, filename)

	def SetRenderingRect(self, left, top, right, bottom):
		wndMgr.SetRenderingRect(self.hWnd, left, top, right, bottom)

	def SetPercentage(self, curValue, maxValue):
		wndMgr.SetRenderingRect(self.hWnd, 0.0, 0.0, -1.0 + float(curValue) / float(maxValue), 0.0)
		
	def ResetFrame(self):
		wndMgr.ResetFrame(self.hWnd)
		
	def SetOnEndFrame(self, event):
		self.eventEndFrame = event

	def OnEndFrame(self):
		if self.eventEndFrame:
			self.eventEndFrame()

class CheckBox(Window):
	def __init__(self):
		Window.__init__(self)
		
		self.backgroundImage = None
		self.checkImage = None

		self.eventFunc = { "ON_CHECK" : None, "ON_UNCKECK" : None, }
		self.eventArgs = { "ON_CHECK" : None, "ON_UNCKECK" : None, }
		self.ToolTipText = None
	
		self.CreateElements()
		
	def __del__(self):
		Window.__del__(self)
		
		self.backgroundImage = None
		self.checkImage = None
		
		self.eventFunc = { "ON_CHECK" : None, "ON_UNCKECK" : None, }
		self.eventArgs = { "ON_CHECK" : None, "ON_UNCKECK" : None, }
		
	def CreateElements(self):
		self.backgroundImage = ImageBox()
		self.backgroundImage.SetParent(self)
		self.backgroundImage.AddFlag("not_pick")
		self.backgroundImage.LoadImage("d:/ymir work/ui/game/refine/checkbox.tga")
		self.backgroundImage.Show()
		
		self.checkImage = ImageBox()
		self.checkImage.SetParent(self)
		self.checkImage.AddFlag("not_pick")
		self.checkImage.SetPosition(0, -4)
		self.checkImage.LoadImage("d:/ymir work/ui/game/refine/checked.tga")
		self.checkImage.Hide()
		
		self.textInfo = TextLine()
		self.textInfo.SetParent(self)
		self.textInfo.SetPosition(20, -2)
		self.textInfo.Show()
		
		self.SetSize(self.backgroundImage.GetWidth() + self.textInfo.GetTextSize()[0], self.backgroundImage.GetHeight() + self.textInfo.GetTextSize()[1])
		
	def SetTextInfo(self, info):
		if self.textInfo:
			self.textInfo.SetText(info)
			
		self.SetSize(self.backgroundImage.GetWidth() + self.textInfo.GetTextSize()[0], self.backgroundImage.GetHeight() + self.textInfo.GetTextSize()[1])

	def SetToolTipText(self, text, x=0, y = -19):
		if not self.ToolTipText:
			toolTip=createToolTipWindowDict["TEXT"]()
			toolTip.SetParent(self)
			toolTip.SetSize(0, 0)
			toolTip.SetHorizontalAlignCenter()
			toolTip.SetOutline()
			toolTip.Hide()
			toolTip.SetPosition(x + self.GetWidth()/2, y)
			self.ToolTipText=toolTip

		self.ToolTipText.SetText(text)
		
	def SetCheckStatus(self, flag):
		if flag:
			self.checkImage.Show()
		else:
			self.checkImage.Hide()
	
	def GetCheckStatus(self):
		if self.checkImage:
			return self.checkImage.IsShow()
			
		return False

	def SetTextOutline(self, flag):
		if self.textInfo:
			self.textInfo.SetOutline(flag)
		
	def SetEvent(self, func, *args) :
		result = self.eventFunc.has_key(args[0])		
		if result :
			self.eventFunc[args[0]] = func
			self.eventArgs[args[0]] = args
		else :
			print "[ERROR] ui.py SetEvent, Can`t Find has_key : %s" % args[0]
		
	def OnMouseLeftButtonUp(self):
		if self.checkImage:
			if self.checkImage.IsShow():
				self.checkImage.Hide()

				if self.eventFunc["ON_UNCKECK"]:
					apply(self.eventFunc["ON_UNCKECK"], self.eventArgs["ON_UNCKECK"])
			else:
				self.checkImage.Show()

				if self.eventFunc["ON_CHECK"]:
					apply(self.eventFunc["ON_CHECK"], self.eventArgs["ON_CHECK"])

class Button(Window):
	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)

		self.eventFunc = None
		self.eventArgs = None

		self.overFunc = None
		self.overArgs = None
		self.overOutFunc = None
		self.overOutArgs = None

		self.ButtonText = None
		self.ToolTipText = None
		self.ToolTipTextNew = None
		self.vasto = None

		self.argsDict = {}
		
		self.showtooltipevent = None
		self.showtooltiparg = None
		self.hidetooltipevent = None
		self.hidetooltiparg = None

	def __del__(self):
		Window.__del__(self)

		self.eventFunc = None
		self.eventArgs = None

		self.overFunc = None
		self.overArgs = None
		self.overOutFunc = None
		self.overOutArgs = None

		self.argsDict = {}

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterButton(self, layer)

	def SetUpVisual(self, filename):
		wndMgr.SetUpVisual(self.hWnd, filename)

	def SetOverVisual(self, filename):
		wndMgr.SetOverVisual(self.hWnd, filename)

	def SetDownVisual(self, filename):
		wndMgr.SetDownVisual(self.hWnd, filename)

	def SetDisableVisual(self, filename):
		wndMgr.SetDisableVisual(self.hWnd, filename)

	def GetUpVisualFileName(self):
		return wndMgr.GetUpVisualFileName(self.hWnd)

	def GetOverVisualFileName(self):
		return wndMgr.GetOverVisualFileName(self.hWnd)

	def GetDownVisualFileName(self):
		return wndMgr.GetDownVisualFileName(self.hWnd)
		
	def ClearToolTip(self):
		self.TextChild = []
		self.ToolTipText = None

	def Flash(self):
		wndMgr.Flash(self.hWnd)

	def Enable(self):
		wndMgr.Enable(self.hWnd)

	def Disable(self):
		wndMgr.Disable(self.hWnd)

	def EnableFlash(self):
		wndMgr.EnableFlash(self.hWnd)

	def DisableFlash(self):
		wndMgr.DisableFlash(self.hWnd)

	def Down(self):
		wndMgr.Down(self.hWnd)

	def SetUp(self):
		wndMgr.SetUp(self.hWnd)

	def SAFE_SetEvent(self, func, *args):
		self.eventFunc = __mem_func__(func)
		self.eventArgs = args

	def SetEvent(self, func, *args):
		self.eventFunc = func
		self.eventArgs = args

	def SetOverEvent(self, func, *args):
		self.overFunc = func
		self.overArgs = args

	def SetOverOutEvent(self, func, *args):
		self.overOutFunc = func
		self.overOutArgs = args

	def SetTextColor(self, color):
		if not self.ButtonText:
			return
		self.ButtonText.SetPackedFontColor(color)

	def SetToolTipColor(self,color):
		self.ToolTipText.SetPackedFontColor(color)

	def SetToolTipColor2(self,color):
		self.ToolTipText2.SetPackedFontColor(color)

	if app.ENABLE_RESP_SYSTEM:
		def __CreateTextLine(self, height=4):
			if not self.ButtonText:
				textLine = TextLine()
				textLine.SetParent(self)
				textLine.SetPosition(self.GetWidth() / 2, self.GetHeight() / 2 + height)
				textLine.SetVerticalAlignCenter()
				textLine.SetHorizontalAlignCenter()
				textLine.Show()
				self.ButtonText = textLine

		def SetTextColor(self, color):
			self.__CreateTextLine()
			self.ButtonText.SetPackedFontColor(color)

		def SetOutline(self, flag):
			self.__CreateTextLine()
			self.ButtonText.SetOutline(flag)

		def SetFontName(self, fontname):
			self.__CreateTextLine()
			self.ButtonText.SetFontName(fontname)

		def SetText(self, text, height=0):
			self.__CreateTextLine(height)
			self.ButtonText.SetText(text)

		def SetTextAddPos(self, text, x_add = 0, y_add = 0, height = 4):
			if not self.ButtonText:
				textLine = TextLine()
				textLine.SetParent(self)
				textLine.SetPosition(self.GetWidth() / 2 + x_add, self.GetHeight() / 2 + y_add)
				textLine.SetVerticalAlignCenter()
				textLine.SetHorizontalAlignCenter()
				textLine.Show()
				self.ButtonText = textLine
			self.ButtonText.SetText(text)
	else:
		def SetText(self, text, height = 4):
			if not self.ButtonText:
				textLine = TextLine()
				textLine.SetParent(self)
				textLine.SetPosition(self.GetWidth()/2, self.GetHeight()/2)
				textLine.SetVerticalAlignCenter()
				textLine.SetHorizontalAlignCenter()
				textLine.Show()
				self.ButtonText = textLine

			self.ButtonText.SetText(text)

	if constInfo.EXTRA_UI_FEATURE:
		def GetText(self):
			if not self.ButtonText:
				return ""
			return self.ButtonText.GetText()
	
	def SetTextOutline(self, val):
		if self.ButtonText:
			self.ButtonText.SetOutline(val)

	def SetFormToolTipText(self, type, text, x, y):
		if not self.ToolTipText:
			toolTip=createToolTipWindowDict[type]()
			if app.__DUNGEON_INFO__:
				toolTip.SetWindowName("not_render_tooltip")
			toolTip.SetParent(self)
			toolTip.SetSize(0, 0)
			toolTip.SetHorizontalAlignCenter()
			toolTip.SetOutline()
			toolTip.Hide()
			toolTip.SetPosition(x + self.GetWidth()/2, y-10)
			self.ToolTipText=toolTip

		self.ToolTipText.SetText(text)

	def SetFormToolTipNewText(self, type, text, x, y):
		if not self.ToolTipTextNew:
			self.vasto = ThinBoardNew()
			self.vasto.SetParent(self)
			self.vasto.SetWindowHorizontalAlignCenter()
			self.vasto.AddFlag("not_pick")
			self.vasto.Hide()
			if y == 0:
				self.vasto.SetPosition(x, y-25)
			else:
				self.vasto.SetPosition(x, y)

			toolTip=createToolTipWindowDict[type]()
			toolTip.SetParent(self.vasto)
			toolTip.SetSize(0, 0)
			toolTip.SetWindowVerticalAlignCenter()
			toolTip.SetVerticalAlignCenter()
			toolTip.SetOutline()
			toolTip.Hide()
			self.ToolTipTextNew=toolTip
	
		self.ToolTipTextNew.SetText(text)
		self.vasto.SetSize(self.ToolTipTextNew.GetTextWidth()+30,3)
		toolTip.SetPosition(15,-1)

	def SetToolTipWindow(self, toolTip):
		self.ToolTipText=toolTip
		self.ToolTipText.SetParentProxy(self)

	def SetToolTipText(self, text, x=0, y = -19):
		self.SetFormToolTipText("TEXT", text, x, y)

	def SetToolTipTextNew(self, text, x=0, y = -19):
		self.SetFormToolTipNewText("TEXT", text, x, y)

	def SetToolTipTextNewColor(self, color):
		self.ToolTipTextNew.SetPackedFontColor(color)

	def CallEvent(self):
		snd.PlaySound("sound/ui/click.wav")

		if self.eventFunc:
			apply(self.eventFunc, self.eventArgs)

	def ShowToolTip(self):
		if self.ToolTipText:
			self.ToolTipText.Show()

		if self.ToolTipTextNew:
			self.vasto.Show()
			self.ToolTipTextNew.Show()

		if self.showtooltipevent:
			apply(self.showtooltipevent, self.showtooltiparg)
	
	def HideToolTip(self):
		if self.ToolTipText:
			self.ToolTipText.Hide()

		if self.ToolTipTextNew:
			self.vasto.Hide()
			self.ToolTipTextNew.Hide()
		
		if self.hidetooltipevent:
			apply(self.hidetooltipevent, self.hidetooltiparg)

	def SetShowToolTipEvent(self, func, *args):
		self.showtooltipevent = func
		self.showtooltiparg = args

	def SetShowToolTipEvent(self, func, *args):
		self.showtooltipevent = func
		self.showtooltiparg = args
		
	def SetHideToolTipEvent(self, func, *args):
		self.hidetooltipevent = func
		self.hidetooltiparg = args

	def IsDown(self):
		return wndMgr.IsDown(self.hWnd)
	
	
	if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
		def SetAlpha(self, alpha):
			wndMgr.SetButtonDiffuseColor(self.hWnd, 1.0, 1.0, 1.0, alpha)
				
		def IsDisable(self):
			return wndMgr.IsDisable(self.hWnd)
		
		def SetScale(self, scale_x, scale_y):
			wndMgr.SetButtonScale(self.hWnd, scale_x, scale_y)
			
		def SetDiffuseColor(self, r, g, b, a):
			wndMgr.SetButtonDiffuseColor(self.hWnd, r, g, b, a)
			
			
	if app.ENABLE_DYNAMIC_FONTS:
		def SetFontName(self, font):
			if self.ButtonText:
				self.ButtonText.SetFontName(font)
				
		def SetDynamicFont(self, flag):
			if self.ButtonText:
				self.ButtonText.SetDynamicFont(flag)

	if app.ENABLE_QUEST_RENEWAL:
		def GetText(self):
			if not self.ButtonText:
				return ""

			return self.ButtonText.GetText()

		def SetTextAlignLeft(self, text, height = 4):
			if not self.ButtonText:
				textLine = TextLine()
				textLine.SetParent(self)
				textLine.SetPosition(27, self.GetHeight()/2)
				textLine.SetVerticalAlignCenter()
				textLine.SetHorizontalAlignLeft()
				textLine.Show()
				self.ButtonText = textLine

			self.ButtonText.SetText(text)
			self.ButtonText.SetPosition(27, self.GetHeight()/2)
			self.ButtonText.SetVerticalAlignCenter()
			self.ButtonText.SetHorizontalAlignLeft()

class RadioButton(Button):
	def __init__(self):
		Button.__init__(self)

	def __del__(self):
		Button.__del__(self)

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterRadioButton(self, layer)

class RadioButtonGroup:
	def __init__(self):
		self.buttonGroup = []
		self.selectedBtnIdx = -1

	def __del__(self):
		for button, ue, de in self.buttonGroup:
			button.__del__()

	def Show(self):
		for (button, selectEvent, unselectEvent) in self.buttonGroup:
			button.Show()

	def Hide(self):
		for (button, selectEvent, unselectEvent) in self.buttonGroup:
			button.Hide()

	def SetText(self, idx, text):
		if idx >= len(self.buttonGroup):
			return
		(button, selectEvent, unselectEvent) = self.buttonGroup[idx]
		button.SetText(text)

	def OnClick(self, btnIdx):
		if btnIdx == self.selectedBtnIdx:
			return
		(button, selectEvent, unselectEvent) = self.buttonGroup[self.selectedBtnIdx]
		if unselectEvent:
			unselectEvent()
		button.SetUp()

		self.selectedBtnIdx = btnIdx
		(button, selectEvent, unselectEvent) = self.buttonGroup[btnIdx]
		if selectEvent:
			selectEvent()

		button.Down()

	def AddButton(self, button, selectEvent, unselectEvent):
		i = len(self.buttonGroup)
		button.SetEvent(lambda : self.OnClick(i))
		self.buttonGroup.append([button, selectEvent, unselectEvent])
		button.SetUp()

	def Create(rawButtonGroup):
		radioGroup = RadioButtonGroup()
		for (button, selectEvent, unselectEvent) in rawButtonGroup:
			radioGroup.AddButton(button, selectEvent, unselectEvent)

		radioGroup.OnClick(0)

		return radioGroup

	Create=staticmethod(Create)

class ToggleButton(Button):
	def __init__(self):
		Button.__init__(self)

		self.eventUp = None
		self.eventDown = None

	def __del__(self):
		Button.__del__(self)

		self.eventUp = None
		self.eventDown = None

	if app.ENABLE_HIDE_COSTUME_SYSTEM:
		def SetToggleUpEvent(self, event, *args):
			self.eventUp = event
			self.eventUpArgs = args

		def SetToggleDownEvent(self, event, *args):
			self.eventDown = event
			self.eventDownArgs = args

		def OnToggleUp(self):
			if self.eventUp:
				if self.eventUpArgs:
					apply(self.eventUp, self.eventUpArgs)
				else:
					self.eventUp()

		def OnToggleDown(self):
			if self.eventDown:
				if self.eventDownArgs:
					apply(self.eventDown, self.eventDownArgs)
				else:
					self.eventDown()

	else:
		def SetToggleUpEvent(self, event):
			self.eventUp = event

		def SetToggleDownEvent(self, event):
			self.eventDown = event

		def OnToggleUp(self):
			if self.eventUp:
				self.eventUp()

		def OnToggleDown(self):
			if self.eventDown:
				self.eventDown()

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterToggleButton(self, layer)

class DragButton(Button):
	def __init__(self):
		Button.__init__(self)
		self.AddFlag("movable")

		self.callbackEnable = True
		self.eventMove = lambda: None

	def __del__(self):
		Button.__del__(self)

		self.eventMove = lambda: None

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterDragButton(self, layer)

	def SetMoveEvent(self, event):
		self.eventMove = event

	def SetRestrictMovementArea(self, x, y, width, height):
		wndMgr.SetRestrictMovementArea(self.hWnd, x, y, width, height)

	def TurnOnCallBack(self):
		self.callbackEnable = True

	def TurnOffCallBack(self):
		self.callbackEnable = False

	def OnMove(self):
		if self.callbackEnable:
			self.eventMove()

class NewSlimScrollBar(Window):

	SCROLLBAR_WIDTH = 14
	SCROLLBAR_MIDDLE_HEIGHT = 1
	SCROLLBAR_BUTTON_WIDTH = 17
	SCROLLBAR_BUTTON_HEIGHT = 17
	SCROLL_BTN_XDIST = 1
	SCROLL_BTN_YDIST = 2

	class MiddleBar(DragButton):
		def __init__(self):
			DragButton.__init__(self)
			self.AddFlag("movable")

			self.SetWindowName("NONAME_ScrollBar_MiddleBar")

		def MakeImage(self):
			top = ImageBox()
			top.SetParent(self)
			top.LoadImage("d:/ymir work/ui/pattern/newslimscroll/ScrollBar_Middle_Top_bottom.tga")
			top.AddFlag("not_pick")
			top.SetPosition(0, 0)
			top.Show()
			topScale = ExpandedImageBox()
			topScale.SetParent(self)
			topScale.SetPosition(0, top.GetHeight())
			topScale.LoadImage("d:/ymir work/ui/pattern/newslimscroll/ScrollBar_Middle_TopScale.tga")
			topScale.AddFlag("not_pick")
			topScale.Show()

			bottom = ImageBox()
			bottom.SetParent(self)
			bottom.LoadImage("d:/ymir work/ui/pattern/newslimscroll/ScrollBar_Middle_Top_bottom.tga")
			bottom.AddFlag("not_pick")
			bottom.Show()
			bottomScale = ExpandedImageBox()
			bottomScale.SetParent(self)
			bottomScale.LoadImage("d:/ymir work/ui/pattern/newslimscroll/ScrollBar_Middle_TopScale.tga")
			bottomScale.AddFlag("not_pick")
			bottomScale.Show()

			middle = ExpandedImageBox()
			middle.SetParent(self)
			middle.LoadImage("d:/ymir work/ui/pattern/newslimscroll/ScrollBar_Middle_Middle.tga")
			middle.AddFlag("not_pick")
			middle.Show()

			self.top = top
			self.topScale = topScale
			self.bottom = bottom
			self.bottomScale = bottomScale
			self.middle = middle

		def SetSize(self, height):
			minHeight = self.top.GetHeight() + self.bottom.GetHeight() + self.middle.GetHeight()
			height = max(minHeight, height)
			DragButton.SetSize(self, 10, height)

			scale = (height - minHeight) / 2
			extraScale = 0
			if (height - minHeight) % 2 == 1:
				extraScale = 1

			self.topScale.SetRenderingRect(0, 0, 0, scale - 1)
			self.middle.SetPosition(-0.99, self.top.GetHeight() + scale)
			self.bottomScale.SetPosition(-0.99, self.middle.GetBottom())
			self.bottomScale.SetRenderingRect(0, 0, 0, scale - 1 + extraScale)
			self.bottom.SetPosition(-0.99, height - self.bottom.GetHeight())

	def __init__(self):
		Window.__init__(self)

		self.pageSize = 1
		self.curPos = 0.0
		self.eventScroll = None
		self.eventArgs = None
		self.lockFlag = False

		self.CreateScrollBar()
		self.SetScrollBarSize(0)

		self.scrollStep = 0.20
		self.SetWindowName("NONAME_ScrollBar")

	def __del__(self):
		Window.__del__(self)

	def CreateScrollBar(self):
		middleImage = ExpandedImageBox()
		middleImage.SetParent(self)
		middleImage.AddFlag("not_pick")
		middleImage.SetPosition(0, 1)
		middleImage.LoadImage("d:/ymir work/ui/pattern/newslimscroll/SlimScrollBar_Middle.tga")
		middleImage.Show()
		self.middleImage = middleImage

		middleBar = self.MiddleBar()
		middleBar.SetParent(self)
		middleBar.SetMoveEvent(__mem_func__(self.OnMove))
		middleBar.Show()
		middleBar.MakeImage()
		middleBar.SetSize(0)
		self.middleBar = middleBar

	def Destroy(self):
		self.eventScroll = None
		self.eventArgs = None

	def SetScrollEvent(self, event, *args):
		self.eventScroll = event
		self.eventArgs = args

	def SetMiddleBarSize(self, pageScale):
		self.middleBar.SetSize(int(pageScale * float(self.GetHeight() - self.SCROLL_BTN_YDIST*2)))
		realHeight = self.GetHeight() - self.SCROLL_BTN_YDIST*2 - self.middleBar.GetHeight()
		self.pageSize = realHeight

	def SetScrollBarSize(self, height):
		self.SetSize(self.SCROLLBAR_WIDTH, height)

		self.pageSize = height - self.SCROLL_BTN_YDIST*2 - self.middleBar.GetHeight()

		middleImageScale = float((height - self.SCROLL_BTN_YDIST*2) - self.middleImage.GetHeight()) / float(self.middleImage.GetHeight())
		self.middleImage.SetRenderingRect(0, 0, 0, middleImageScale)

		self.middleBar.SetRestrictMovementArea(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST, \
			self.middleBar.GetWidth(), height - self.SCROLL_BTN_YDIST * 2)
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST)

	def SetScrollStep(self, step):
		self.scrollStep = step

	def GetScrollStep(self):
		return self.scrollStep

	def GetPos(self):
		return self.curPos

	def OnUp(self):
		self.SetPos(self.curPos-self.scrollStep)

	def OnDown(self):
		self.SetPos(self.curPos+self.scrollStep)

	def SetPos(self, pos, moveEvent = True):
		pos = max(0.0, pos)
		pos = min(1.0, pos)

		newPos = float(self.pageSize) * pos
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, int(newPos) + self.SCROLL_BTN_YDIST)
		if moveEvent == True:
			self.OnMove()

	def OnMove(self):
		if self.lockFlag:
			return

		if 0 == self.pageSize:
			return

		(xLocal, yLocal) = self.middleBar.GetLocalPosition()
		self.curPos = float(yLocal - self.SCROLL_BTN_YDIST) / float(self.pageSize)

		if self.eventScroll:
			apply(self.eventScroll, self.eventArgs)

	def OnMouseLeftButtonDown(self):
		(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()
		newPos = float(yMouseLocalPosition) / float(self.GetHeight())
		self.SetPos(newPos)

	def LockScroll(self):
		self.lockFlag = True

	def UnlockScroll(self):
		self.lockFlag = False

class ScrollBarSpecial(Window):
	BASE_COLOR = grp.GenerateColor(0.0, 0.0, 0.0, 1.0)
	CORNERS_AND_LINES_COLOR = grp.GenerateColor(0.3411, 0.3411, 0.3411, 1.0)
	BAR_NUMB = 9 #This is static value! Please dont touch in him.
	SCROLL_WIDTH= 8
	class MiddleBar(DragButton):
		MIDDLE_BAR_COLOR = grp.GenerateColor(0.6470, 0.6470, 0.6470, 1.0)
		def __init__(self, horizontal_scroll):
			DragButton.__init__(self)
			self.AddFlag("movable")
			self.horizontal_scroll = horizontal_scroll
			self.middle = Bar()
			self.middle.SetParent(self)
			self.middle.AddFlag("attach")
			self.middle.AddFlag("not_pick")
			self.middle.SetColor(self.MIDDLE_BAR_COLOR)
			self.middle.SetSize(1, 1)
			self.middle.Show()
		def SetStaticScale(self, size):
			(base_width, base_height) = (self.middle.GetWidth(), self.middle.GetHeight())
			if not self.horizontal_scroll:
				DragButton.SetSize(self, base_width, size)
				self.middle.SetSize(base_width, size)
			else:
				DragButton.SetSize(self, size, base_height)
				self.middle.SetSize(size, base_height)
		def SetSize(self, selfSize, fullSize):
			(base_width, base_height) = (self.middle.GetWidth(), self.middle.GetHeight())
			
			if not self.horizontal_scroll:
				DragButton.SetSize(self, base_width, operator.truediv(int(selfSize), int(fullSize)) * selfSize)
				self.middle.SetSize(base_width, operator.truediv(int(selfSize), int(fullSize)) * selfSize)
			else:
				DragButton.SetSize(self, operator.truediv(int(selfSize), int(fullSize)) * selfSize, base_height)
				self.middle.SetSize(operator.truediv(int(selfSize), int(fullSize)) * selfSize, base_height)
		def SetStaticSize(self, size):
			size = max(2, size)
			
			if not self.horizontal_scroll:
				DragButton.SetSize(self, size, self.middle.GetHeight())
				self.middle.SetSize(size, self.middle.GetHeight())
			else:
				DragButton.SetSize(self, self.middle.GetWidth(), size)
				self.middle.SetSize(self.middle.GetWidth(), size)
	def __init__(self, horizontal_scroll = False):
		Window.__init__(self)
		self.horizontal_scroll = horizontal_scroll
		self.scrollEvent = None
		self.scrollSpeed = 50
		self.sizeScale = 1.0
		self.bars = []
		for i in xrange(self.BAR_NUMB):
			br = Bar()
			br.SetParent(self)
			br.AddFlag("attach")
			br.AddFlag("not_pick")
			br.SetColor([self.CORNERS_AND_LINES_COLOR, self.BASE_COLOR][i == (self.BAR_NUMB-1)])
			if not (i % 2 == 0): br.SetSize(1, 1)
			br.Show()
			self.bars.append(br)
		self.middleBar = self.MiddleBar(self.horizontal_scroll)
		self.middleBar.SetParent(self)
		self.middleBar.SetMoveEvent(__mem_func__(self.OnScrollMove))
		self.middleBar.Show()
	def OnScrollMove(self):
		if not self.scrollEvent:
			return
		arg = float(self.middleBar.GetLocalPosition()[1] - 1) / float(self.GetHeight() - 2 - self.middleBar.GetHeight()) if not self.horizontal_scroll else\
				float(self.middleBar.GetLocalPosition()[0] - 1) / float(self.GetWidth() - 2 - self.middleBar.GetWidth())
		self.scrollEvent(arg)
	def SetScrollEvent(self, func):
		self.scrollEvent = func
	def SetScrollSpeed(self, speed):
		self.scrollSpeed = speed
	def OnMouseWheel(self, length):
		if not self.IsShow():
			return False
		length = int((length * 0.01) * self.scrollSpeed)
		if not self.horizontal_scroll:
			val = min(max(1, self.middleBar.GetLocalPosition()[1] - (length * 0.01) * self.scrollSpeed * self.sizeScale), self.GetHeight() - self.middleBar.GetHeight() - 1)
			self.middleBar.SetPosition(1, val)
		else:
			val = min(max(1, self.middleBar.GetLocalPosition()[0] - (length * 0.01) *  self.scrollSpeed * self.sizeScale), self.GetWidth() - self.middleBar.GetWidth() - 1)
			self.middleBar.SetPosition(val, 1)
		self.OnScrollMove()
		return True
	def GetPos(self):
		return float(self.middleBar.GetLocalPosition()[1] - 1) / float(self.GetHeight() - 2 - self.middleBar.GetHeight()) if not self.horizontal_scroll else float(self.middleBar.GetLocalPosition()[0] - 1) / float(self.GetWidth() - 2 - self.middleBar.GetWidth())
	def OnMouseLeftButtonDown(self):
		(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()
		if not self.horizontal_scroll:
			if xMouseLocalPosition == 0 or xMouseLocalPosition == self.GetWidth():
				return
			y_pos = (yMouseLocalPosition - self.middleBar.GetHeight() / 2)
			self.middleBar.SetPosition(1, y_pos)
		else:
			if yMouseLocalPosition == 0 or yMouseLocalPosition == self.GetHeight():
				return
			x_pos = (xMouseLocalPosition - self.middleBar.GetWidth() / 2)
			self.middleBar.SetPosition(x_pos, 1)
		self.OnScrollMove()
	def SetSize(self, w, h):
		(width, height) = (max(3, w), max(3, h))
		Window.SetSize(self, width, height)
		self.bars[0].SetSize(1, (height - 2))
		self.bars[0].SetPosition(0, 1)
		self.bars[2].SetSize((width - 2), 1)
		self.bars[2].SetPosition(1, 0)
		self.bars[4].SetSize(1, (height - 2))
		self.bars[4].SetPosition((width - 1), 1)
		self.bars[6].SetSize((width - 2), 1)
		self.bars[6].SetPosition(1, (height - 1))
		self.bars[8].SetSize((width - 2), (height - 2))
		self.bars[8].SetPosition(1, 1)
		self.bars[1].SetPosition(0, 0)
		self.bars[3].SetPosition((width - 1), 0)
		self.bars[5].SetPosition((width - 1), (height - 1))
		self.bars[7].SetPosition(0, (height - 1))
		if not self.horizontal_scroll:
			self.middleBar.SetStaticSize(width - 2)
			self.middleBar.SetSize(12, self.GetHeight())
		else:
			self.middleBar.SetStaticSize(height - 2)
			self.middleBar.SetSize(12, self.GetWidth())
		self.middleBar.SetRestrictMovementArea(1, 1, width - 2, height - 2)
	def SetScale(self, selfSize, fullSize):
		self.sizeScale = float(selfSize)/float(fullSize)
		if self.sizeScale <= 0.0305:
			self.sizeScale = 0.05
		self.middleBar.SetSize(selfSize, fullSize)
	def SetStaticScale(self, r_size):
		self.middleBar.SetStaticScale(r_size)
	def SetPosScale(self, fScale):
		pos = (math.ceil((self.GetHeight() - 2 - self.middleBar.GetHeight()) * fScale) + 1) if not self.horizontal_scroll else (math.ceil((self.GetWidth() - 2 - self.middleBar.GetWidth()) * fScale) + 1)
		self.SetPos(pos)
	def SetPos(self, pos):
		wPos = (1, pos) if not self.horizontal_scroll else (pos, 1)
		self.middleBar.SetPosition(*wPos)

class ScrollBarTemplate(Window):

	MIDDLE_BAR_POS = 5
	MIDDLE_BAR_UPPER_PLACE = 2
	MIDDLE_BAR_DOWNER_PLACE = 2
	TEMP_SPACE = MIDDLE_BAR_UPPER_PLACE + MIDDLE_BAR_DOWNER_PLACE


	class MiddleBar(DragButton):
		def __init__(self):
			self.middle = None
			DragButton.__init__(self)
			self.AddFlag("movable")

		def MakeImage(self, img):
			middle = ExpandedImageBox()
			middle.SetParent(self)
			middle.LoadImage(img)
			middle.SetPosition(0, 0)
			middle.AddFlag("not_pick")
			middle.Show()
			self.middle = middle
			self.SetSize(self.GetHeight())

		def SetSize(self, height):
			height = max(12, height)
			if self.middle:
				DragButton.SetSize(self, self.middle.GetWidth(), height)
				val = 0
				if self.middle.GetHeight() != 0:
					val = float(height)/self.middle.GetHeight()
				self.middle.SetRenderingRect(0, 0, 0, -1.0 + val)
			else:
				DragButton.SetSize(self, self.GetWidth(), height)

	def __init__(self):
		Window.__init__(self)

		self.pageSize = 1
		self.curPos = 0.0
		self.eventScroll = None
		self.eventArgs = None
		self.lockFlag = False

		self.SCROLLBAR_WIDTH = 0
		self.SCROLLBAR_BUTTON_WIDTH = 0
		self.SCROLLBAR_BUTTON_HEIGHT = 0
		self.SCROLLBAR_MIDDLE_HEIGHT = 0

		self.CreateScrollBar()

		self.scrollStep = 0.20

	def SetUpButton(self, upVisual, overVisual, downVisual):
		self.upButton.SetUpVisual(upVisual)
		self.upButton.SetOverVisual(overVisual)
		self.upButton.SetDownVisual(downVisual)
		self.upButton.Show()
		self.SCROLLBAR_BUTTON_WIDTH = self.upButton.GetWidth()
		self.SCROLLBAR_BUTTON_HEIGHT = self.upButton.GetHeight() + 3

	def SetDownButton(self, upVisual, overVisual, downVisual):
		self.downButton.SetUpVisual(upVisual)
		self.downButton.SetOverVisual(overVisual)
		self.downButton.SetDownVisual(downVisual)
		self.downButton.Show()

	def SetMiddleImage(self, img):
		self.middleBar.MakeImage(img)
		self.SCROLLBAR_MIDDLE_HEIGHT = self.middleBar.GetHeight()
		self.MIDDLE_BAR_POS = (self.SCROLLBAR_WIDTH - self.middleBar.GetWidth()) / 2

	def SetBarPartImages(self, topImg, centerImg, bottomImg):
		self.barTopImage.LoadImage(topImg)
		self.barTopImage.Show()
		self.barCenterImage.LoadImage(centerImg)
		self.barCenterImage.SetPosition(0, self.barTopImage.GetHeight())
		self.barCenterImage.Show()
		self.barBottomImage.LoadImage(bottomImg)
		self.barBottomImage.Show()
		self.SCROLLBAR_WIDTH = max(self.barTopImage.GetWidth(), self.SCROLLBAR_WIDTH)
		self.MIDDLE_BAR_POS = (self.SCROLLBAR_WIDTH - self.middleBar.GetWidth()) / 2

	def SetBarImage(self, img):
		self.barImage.LoadImage(img)
		self.barImage.Show()
		self.SCROLLBAR_WIDTH = max(self.barImage.GetWidth(), self.SCROLLBAR_WIDTH)
		self.MIDDLE_BAR_POS = (self.SCROLLBAR_WIDTH - self.middleBar.GetWidth()) / 2

	def CreateScrollBar(self):
		barImage = ExpandedImageBox()
		barImage.SetParent(self)
		barImage.AddFlag("not_pick")
		barImage.Hide()

		barTopImage = ImageBox()
		barTopImage.SetParent(self)
		barTopImage.AddFlag("not_pick")
		barTopImage.Hide()

		barCenterImage = ExpandedImageBox()
		barCenterImage.SetParent(self)
		barCenterImage.AddFlag("not_pick")
		barCenterImage.Hide()

		barBottomImage = ImageBox()
		barBottomImage.SetParent(self)
		barBottomImage.AddFlag("not_pick")
		barBottomImage.Hide()

		middleBar = self.MiddleBar()
		middleBar.SetParent(self)
		middleBar.SetMoveEvent(__mem_func__(self.OnMove))
		middleBar.Show()
		middleBar.SetSize(12)

		upButton = Button()
		upButton.SetParent(self)
		upButton.SetEvent(__mem_func__(self.OnUp))
		upButton.SetWindowHorizontalAlignCenter()
		upButton.Hide()

		downButton = Button()
		downButton.SetParent(self)
		downButton.SetEvent(__mem_func__(self.OnDown))
		downButton.SetWindowHorizontalAlignCenter()
		downButton.Hide()

		self.upButton = upButton
		self.downButton = downButton
		self.middleBar = middleBar
		self.barImage = barImage
		self.barTopImage = barTopImage
		self.barCenterImage = barCenterImage
		self.barBottomImage = barBottomImage

	def Destroy(self):
		self.middleBar = None
		self.upButton = None
		self.downButton = None
		self.barImage = None
		self.barTopImage = None
		self.barCenterImage = None
		self.barBottomImage = None
		self.eventScroll = None
		self.eventArgs = None

	def SetScrollEvent(self, event, *args):
		self.eventScroll = event
		self.eventArgs = args




	def SetMiddleBarSize(self, pageScale):
		realHeight = self.GetHeight() - self.SCROLLBAR_BUTTON_HEIGHT*2
		self.SCROLLBAR_MIDDLE_HEIGHT = max(12, int(pageScale * float(realHeight)))
		self.middleBar.SetSize(self.SCROLLBAR_MIDDLE_HEIGHT)
		self.pageSize = (self.GetHeight() - self.SCROLLBAR_BUTTON_HEIGHT*2) - self.SCROLLBAR_MIDDLE_HEIGHT - (self.TEMP_SPACE)

	def SetScrollBarSize(self, height):
		self.pageSize = (height - self.SCROLLBAR_BUTTON_HEIGHT*2) - self.SCROLLBAR_MIDDLE_HEIGHT - (self.TEMP_SPACE)
		self.SetSize(self.SCROLLBAR_WIDTH, height)
		self.upButton.SetPosition(0, 3)
		self.downButton.SetPosition(0, height - self.SCROLLBAR_BUTTON_HEIGHT)
		self.middleBar.SetRestrictMovementArea(self.MIDDLE_BAR_POS, self.SCROLLBAR_BUTTON_HEIGHT + self.MIDDLE_BAR_UPPER_PLACE, self.middleBar.GetWidth(), height - self.SCROLLBAR_BUTTON_HEIGHT*2 - self.TEMP_SPACE)
		self.middleBar.SetPosition(self.MIDDLE_BAR_POS, 0)

		self.UpdateBarImage()
		self.upButton.UpdateRect()
		self.downButton.UpdateRect()

	def SetScrollStep(self, step):
		self.scrollStep = step

	def GetScrollStep(self):
		return self.scrollStep

	def UpdateBarImage(self):
		if self.barImage.IsShow():
			val = 0

			if self.barImage.GetHeight() != 0:
				val = self.GetHeight() / float(self.barImage.GetHeight())

			self.barImage.SetRenderingRect(0.0, 0.0, 0.0, -1.0 + val)

		if self.barCenterImage.IsShow():
			centerHeight = self.GetHeight() - (self.barTopImage.GetHeight() + self.barBottomImage.GetHeight())

			val = 0

			if self.barCenterImage.GetHeight() != 0:
				val = (centerHeight / float(self.barCenterImage.GetHeight()))

			self.barCenterImage.SetRenderingRect(0.0, 0.0, 0.0, -1.0 + val)

		if self.barBottomImage.IsShow():
			self.barBottomImage.SetPosition(0, self.GetHeight() - self.barBottomImage.GetHeight())

	def GetPos(self):
		return self.curPos

	def SetPos(self, pos):
		pos = max(0.0, pos)
		pos = min(1.0, pos)

		newPos = float(self.pageSize) * pos

		self.middleBar.SetPosition(self.MIDDLE_BAR_POS, int(newPos) + self.SCROLLBAR_BUTTON_HEIGHT + self.MIDDLE_BAR_UPPER_PLACE)
		self.OnMove()

	def OnUp(self):
		self.SetPos(self.curPos-self.scrollStep)

	def OnDown(self):
		self.SetPos(self.curPos+self.scrollStep)

	def OnMove(self):

		if self.lockFlag:
			return

		if 0 == self.pageSize:
			return

		(xLocal, yLocal) = self.middleBar.GetLocalPosition()

		self.curPos = float(yLocal - self.SCROLLBAR_BUTTON_HEIGHT - self.MIDDLE_BAR_UPPER_PLACE) / float(self.pageSize)

		if self.eventScroll:
			apply(self.eventScroll, self.eventArgs)

	def OnMouseLeftButtonDown(self):
		(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()

		pickedPos = yMouseLocalPosition - self.SCROLLBAR_BUTTON_HEIGHT - self.SCROLLBAR_MIDDLE_HEIGHT/2
		newPos = float(pickedPos) / float(self.pageSize)

		self.SetPos(newPos)

	def LockScroll(self):
		self.lockFlag = True

	def UnlockScroll(self):
		self.lockFlag = False

	def OnWheelMove(self, iLen):
		pos = self.GetPos()

		iLen = (float(abs(iLen)-100)/100.0)*(iLen/abs(iLen))

		iLen *= -1

		pos += iLen
		self.SetPos(pos)
		return True

class NumberLine(Window):

	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)

	def __del__(self):
		Window.__del__(self)

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterNumberLine(self, layer)

	def SetHorizontalAlignCenter(self):
		wndMgr.SetNumberHorizontalAlignCenter(self.hWnd)

	def SetHorizontalAlignRight(self):
		wndMgr.SetNumberHorizontalAlignRight(self.hWnd)

	def SetPath(self, path):
		wndMgr.SetPath(self.hWnd, path)

	def SetNumber(self, number):
		wndMgr.SetNumber(self.hWnd, number)


class Box(Window):

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterBox(self, layer)

	def SetColor(self, color):
		wndMgr.SetColor(self.hWnd, color)
		
class ColourBox(Box):

	BASE_OPACITY = 40

	def	__init__(self):
		Box.__init__(self)

		self.boxContent = Bar()
		self.boxContent.SetParent(self)
		self.boxContent.SetPosition(1, 1)
		self.boxContent.Show()

	def	__del__(self):
		Box.__del__(self)
		self.boxContent = None

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterBox(self, layer)

	def	SetSize(self, iWidth, iHeight):
		Box.SetSize(self, iWidth, iHeight)

		self.boxContent.SetSize(iWidth-2, iHeight-2)

	def SetColor(self, color, cntColor = None):
		Box.SetColor(self, color)

		try:
			colorText = hex(color)
			color = int(hex(int(255*self.BASE_OPACITY/100)) + colorText[4:len(colorText)-1], 16)
		except Exception as e:
			print "Invalid color value was provided", e

		self.boxContent.SetColor(color if not cntColor else cntColor)

class Bar(Window):

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterBar(self, layer)

	def SetColor(self, color):
		wndMgr.SetColor(self.hWnd, color)

class Line(Window):

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterLine(self, layer)

	def SetColor(self, color):
		wndMgr.SetColor(self.hWnd, color)

class SlotBar(Window):

	def __init__(self):
		Window.__init__(self)

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterBar3D(self, layer)

class Bar3D(Window):

	def __init__(self):
		Window.__init__(self)

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterBar3D(self, layer)

	def SetColor(self, left, right, center):
		wndMgr.SetColor(self.hWnd, left, right, center)

class SlotWindow(Window):

	def __init__(self):
		Window.__init__(self)

		self.StartIndex = 0

		self.eventSelectEmptySlot = None
		self.eventSelectItemSlot = None
		self.eventUnselectEmptySlot = None
		self.eventUnselectItemSlot = None
		self.eventUseSlot = None
		self.eventOverInItem = None
		self.eventOverOutItem = None
		self.eventPressedSlotButton = None
		self.eventDict={}
		self.__indices = []

	def __del__(self):
		Window.__del__(self)

		self.eventSelectEmptySlot = None
		self.eventSelectItemSlot = None
		self.eventUnselectEmptySlot = None
		self.eventUnselectItemSlot = None
		self.eventUseSlot = None
		self.eventOverInItem = None
		self.eventOverOutItem = None
		self.eventPressedSlotButton = None
		self.eventDict={}

	def OnMouseOverIn(self):
		try:
			self.eventDict["MOUSE_OVER_IN"]()
		except KeyError:
			pass

	def OnMouseOverOut(self):
		try:
			self.eventDict["MOUSE_OVER_OUT"]()
		except KeyError:
			pass

	def SAFE_SetStringEvent(self, event, func):
		self.eventDict[event]=__mem_func__(func)

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterSlotWindow(self, layer)

	def SetSlotStyle(self, style):
		wndMgr.SetSlotStyle(self.hWnd, style)

	def HasSlot(self, slotIndex):
		return wndMgr.HasSlot(self.hWnd, slotIndex)

	def SetSlotBaseImage(self, imageFileName, r, g, b, a):
		wndMgr.SetSlotBaseImage(self.hWnd, imageFileName, r, g, b, a)

	def SetCoverButton(self,\
						slotIndex,\
						upName="d:/ymir work/ui/public/slot_cover_button_01.sub",\
						overName="d:/ymir work/ui/public/slot_cover_button_02.sub",\
						downName="d:/ymir work/ui/public/slot_cover_button_03.sub",\
						disableName="d:/ymir work/ui/public/slot_cover_button_04.sub",\
						LeftButtonEnable = False,\
						RightButtonEnable = True):
		wndMgr.SetCoverButton(self.hWnd, slotIndex, upName, overName, downName, disableName, LeftButtonEnable, RightButtonEnable)

	def EnableCoverButton(self, slotIndex):
		wndMgr.EnableCoverButton(self.hWnd, slotIndex)

	def DisableCoverButton(self, slotIndex):
		wndMgr.DisableCoverButton(self.hWnd, slotIndex)

	def SetAlwaysRenderCoverButton(self, slotIndex, bAlwaysRender = True):
		wndMgr.SetAlwaysRenderCoverButton(self.hWnd, slotIndex, bAlwaysRender)

	def AppendSlotButton(self, upName, overName, downName):
		wndMgr.AppendSlotButton(self.hWnd, upName, overName, downName)

	def ShowSlotButton(self, slotNumber):
		wndMgr.ShowSlotButton(self.hWnd, slotNumber)

	def HideAllSlotButton(self):
		wndMgr.HideAllSlotButton(self.hWnd)

	def AppendRequirementSignImage(self, filename):
		wndMgr.AppendRequirementSignImage(self.hWnd, filename)

	def ShowRequirementSign(self, slotNumber):
		wndMgr.ShowRequirementSign(self.hWnd, slotNumber)

	def HideRequirementSign(self, slotNumber):
		wndMgr.HideRequirementSign(self.hWnd, slotNumber)

	def ActivateSlot(self, slotNumber, r = 1.0, g = 1.0, b = 1.0, a = 1.0):
		wndMgr.ActivateSlot(self.hWnd, slotNumber, r, g, b, a)

	def DeactivateSlot(self, slotNumber):
		wndMgr.DeactivateSlot(self.hWnd, slotNumber)

	def ShowSlotBaseImage(self, slotNumber):
		wndMgr.ShowSlotBaseImage(self.hWnd, slotNumber)

	def HideSlotBaseImage(self, slotNumber):
		wndMgr.HideSlotBaseImage(self.hWnd, slotNumber)

	def SAFE_SetButtonEvent(self, button, state, event):
		if "LEFT"==button:
			if "EMPTY"==state:
				self.eventSelectEmptySlot=__mem_func__(event)
			elif "EXIST"==state:
				self.eventSelectItemSlot=__mem_func__(event)
			elif "ALWAYS"==state:
				self.eventSelectEmptySlot=__mem_func__(event)
				self.eventSelectItemSlot=__mem_func__(event)
		elif "RIGHT"==button:
			if "EMPTY"==state:
				self.eventUnselectEmptySlot=__mem_func__(event)
			elif "EXIST"==state:
				self.eventUnselectItemSlot=__mem_func__(event)
			elif "ALWAYS"==state:
				self.eventUnselectEmptySlot=__mem_func__(event)
				self.eventUnselectItemSlot=__mem_func__(event)

	def SetSelectEmptySlotEvent(self, empty):
		self.eventSelectEmptySlot = empty

	def SetSelectItemSlotEvent(self, item):
		self.eventSelectItemSlot = item

	def SetUnselectEmptySlotEvent(self, empty):
		self.eventUnselectEmptySlot = empty

	def SetUnselectItemSlotEvent(self, item):
		self.eventUnselectItemSlot = item

	def SetUseSlotEvent(self, use):
		self.eventUseSlot = use

	def SetOverInItemEvent(self, event):
		self.eventOverInItem = event

	def SetOverOutItemEvent(self, event):
		self.eventOverOutItem = event

	def SetPressedSlotButtonEvent(self, event):
		self.eventPressedSlotButton = event

	def GetSlotCount(self):
		return wndMgr.GetSlotCount(self.hWnd)

	def SetUseMode(self, flag):
		wndMgr.SetUseMode(self.hWnd, flag)

	def SetUsableItem(self, flag):
		wndMgr.SetUsableItem(self.hWnd, flag)

	if app.ENABLE_SLOT_WINDOW_EX:
		def IsActivatedSlot(self, slotNumber):
			return wndMgr.IsActivatedSlot(self.hWnd, slotNumber)

		def GetSlotCoolTime(self, slotIndex):
			return wndMgr.GetSlotCoolTime(self.hWnd, slotIndex)

	def SetSlotCoolTime(self, slotIndex, coolTime, elapsedTime = 0.0):
		wndMgr.SetSlotCoolTime(self.hWnd, slotIndex, coolTime, elapsedTime)
		
	if app.WJ_ENABLE_TRADABLE_ICON:
		def SetCanMouseEventSlot(self, slotIndex):
			wndMgr.SetCanMouseEventSlot(self.hWnd, slotIndex)

		def SetCantMouseEventSlot(self, slotIndex):
			wndMgr.SetCantMouseEventSlot(self.hWnd, slotIndex)

		def SetUsableSlotOnTopWnd(self, slotIndex):
			wndMgr.SetUsableSlotOnTopWnd(self.hWnd, slotIndex)

		def SetUnusableSlotOnTopWnd(self, slotIndex):
			wndMgr.SetUnusableSlotOnTopWnd(self.hWnd, slotIndex)

	def DisableSlot(self, slotIndex):
		wndMgr.DisableSlot(self.hWnd, slotIndex)

	def EnableSlot(self, slotIndex):
		wndMgr.EnableSlot(self.hWnd, slotIndex)

	def LockSlot(self, slotIndex):
		wndMgr.LockSlot(self.hWnd, slotIndex)

	def UnlockSlot(self, slotIndex):
		wndMgr.UnlockSlot(self.hWnd, slotIndex)

	def SetFishAddonOnSlot(self, slotNumber):
		wndMgr.SetFishAddonOnSlot(self.hWnd, slotNumber)

	def RefreshSlot(self):
		wndMgr.RefreshSlot(self.hWnd)

	def ClearSlot(self, slotNumber):
		wndMgr.ClearSlot(self.hWnd, slotNumber)

	def ClearAllSlot(self):
		wndMgr.ClearAllSlot(self.hWnd)
		
	def GetSlotIndexList(self):
		return wndMgr.GetSlotIndexList(self.hWnd)

	if app.ENABLE_OFFLINE_SHOP:
		def SetGridSpecial(self, w, h):
			wndMgr.SetGridSpecial(self.hWnd, w, h)
		
		def GetEmptyGrid(self, item_size):
			return wndMgr.GetEmptyGrid(self.hWnd, item_size)
		
		def ClearGrid(self):
			wndMgr.ClearGrid(self.hWnd)
		
		def PutItemGrid(self, pos, size):
			wndMgr.PutItemGrid(self.hWnd, pos, size)
			
		def ClearItemGrid(self, pos, size):
			wndMgr.ClearItemGrid(self.hWnd, pos, size)

	def AppendSlot(self, index, x, y, width, height):
		wndMgr.AppendSlot(self.hWnd, index, x, y, width, height)

	def SetSlot(self, slotIndex, itemIndex, width, height, icon, diffuseColor = (1.0, 1.0, 1.0, 1.0)):
		wndMgr.SetSlot(self.hWnd, slotIndex, itemIndex, width, height, icon, diffuseColor)

	def SetSlotCount(self, slotNumber, count):
		wndMgr.SetSlotCount(self.hWnd, slotNumber, count)

	def SetSlotCountNew(self, slotNumber, grade, count):
		wndMgr.SetSlotCountNew(self.hWnd, slotNumber, grade, count)

	def SetItemSlot(self, renderingSlotNumber, ItemIndex, ItemCount = 0, diffuseColor = (1.0, 1.0, 1.0, 1.0)):
		if 0 == ItemIndex or None == ItemIndex:
			wndMgr.ClearSlot(self.hWnd, renderingSlotNumber)
			return

		item.SelectItem(ItemIndex)
		itemIcon = item.GetIconImage()

		item.SelectItem(ItemIndex)
		(width, height) = item.GetItemSize()

		wndMgr.SetSlot(self.hWnd, renderingSlotNumber, ItemIndex, width, height, itemIcon, diffuseColor)
		wndMgr.SetSlotCount(self.hWnd, renderingSlotNumber, ItemCount)

	def SetCardSlot(self, renderingSlotNumber, CardIndex, cardIcon, diffuseColor = (1.0, 1.0, 1.0, 1.0)):
		if 0 == CardIndex or None == CardIndex:
			wndMgr.ClearSlot(self.hWnd, renderingSlotNumber)
			return

		item.SelectItem(CardIndex)
		(width, height) = item.GetItemSize()
		
		wndMgr.SetCardSlot(self.hWnd, renderingSlotNumber, CardIndex, width, height, cardIcon, diffuseColor)

	def SetSkillSlot(self, renderingSlotNumber, skillIndex, skillLevel):

		skillIcon = skill.GetIconImage(skillIndex)

		if 0 == skillIcon:
			wndMgr.ClearSlot(self.hWnd, renderingSlotNumber)
			return

		wndMgr.SetSlot(self.hWnd, renderingSlotNumber, skillIndex, 1, 1, skillIcon)
		wndMgr.SetSlotCount(self.hWnd, renderingSlotNumber, skillLevel)

	def SetSkillSlotNew(self, renderingSlotNumber, skillIndex, skillGrade, skillLevel):

		skillIcon = skill.GetIconImageNew(skillIndex, skillGrade)

		if 0 == skillIcon:
			wndMgr.ClearSlot(self.hWnd, renderingSlotNumber)
			return

		wndMgr.SetSlot(self.hWnd, renderingSlotNumber, skillIndex, 1, 1, skillIcon)

	def SetEmotionSlot(self, renderingSlotNumber, emotionIndex):
		import player
		icon = player.GetEmotionIconImage(emotionIndex)

		if 0 == icon:
			wndMgr.ClearSlot(self.hWnd, renderingSlotNumber)
			return

		wndMgr.SetSlot(self.hWnd, renderingSlotNumber, emotionIndex, 1, 1, icon)

	def OnSelectEmptySlot(self, slotNumber):
		if self.eventSelectEmptySlot:
			self.eventSelectEmptySlot(slotNumber)

	def OnSelectItemSlot(self, slotNumber):
		if self.eventSelectItemSlot:
			self.eventSelectItemSlot(slotNumber)

	def OnUnselectEmptySlot(self, slotNumber):
		if self.eventUnselectEmptySlot:
			self.eventUnselectEmptySlot(slotNumber)

	def OnUnselectItemSlot(self, slotNumber):
		if self.eventUnselectItemSlot:
			self.eventUnselectItemSlot(slotNumber)

	def OnUseSlot(self, slotNumber):
		if self.eventUseSlot:
			self.eventUseSlot(slotNumber)

	def OnOverInItem(self, slotNumber):
		if self.eventOverInItem:
			self.eventOverInItem(slotNumber)

	def OnOverOutItem(self):
		if self.eventOverOutItem:
			self.eventOverOutItem()

	def OnPressedSlotButton(self, slotNumber):
		if self.eventPressedSlotButton:
			self.eventPressedSlotButton(slotNumber)

	def GetStartIndex(self):
		return 0
		
	def SetSlotType(self, flag):
		wndMgr.SetSlotType(self.hWnd, flag)
	
	def SetRenderSlot(self, renderingSlotNumber, diffuseColor = (1.0, 1.0, 1.0, 1.0)):
		wndMgr.SetSlot(self.hWnd, renderingSlotNumber, 1, 1, 1, 0, diffuseColor)
		wndMgr.SetSlotCount(self.hWnd, renderingSlotNumber, 0)

	def RefreshIndices(self):
		self.__indices = wndMgr.GetSlotIndices(self.hWnd)
	
	def RefreshItems(self, vnumFunction, countFunction = lambda x: 0):
		for index in self.__indices:
			self.SetItemSlot(index, vnumFunction(index), countFunction(index))
		
		self.RefreshSlot()

class Grid: # from KeN
	def __init__(self, width, height):
		self.width = width
		self.height = height
		self.reset()
	def find_blank(self, width, height):
		if width > self.width or height > self.height:
			return -1

		for row in range(self.height):
			for col in range(self.width):
				index = row * self.width + col
				if self.is_empty(index, width, height):
					return index
		return -1
	def put(self, pos, width, height):
		if not self.is_empty(pos, width, height):
			return False
		for row in range(height):
			start = pos + (row * self.width)
			self.grid[start] = True
			col = 1
			while col < width:
				self.grid[start + col] = True
				col += 1
		return True
	def clear(self, pos, width, height):
		if pos < 0 or pos >= (self.width * self.height):
			return
		for row in range(height):
			start = pos + (row * self.width)
			self.grid[start] = True
			col = 1
			while col < width:
				self.grid[start + col] = False
				col += 1
	def is_empty(self, pos, width, height):
		if pos < 0:
			return False
		row = pos // self.width
		if (row + height) > self.height:
			return False
		if (pos + width) > ((row * self.width) + self.width):
			return False
		for row in range(height):
			start = pos + (row * self.width)
			if self.grid[start]:
				return False
			col = 1
			while col < width:
				if self.grid[start + col]:
					return False
				col += 1
		return True
	def get_size(self):
		return self.width * self.height
	def reset(self):
		self.grid = [False] * (self.width * self.height)
		#self.put(self.width,1,1) # fix from dracaryS

class GridSlotWindow(SlotWindow):

	def __init__(self):
		SlotWindow.__init__(self)

		self.startIndex = 0

	def __del__(self):
		SlotWindow.__del__(self)

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterGridSlotWindow(self, layer)

	def ArrangeSlot(self, StartIndex, xCount, yCount, xSize, ySize, xBlank, yBlank):

		self.startIndex = StartIndex

		wndMgr.ArrangeSlot(self.hWnd, StartIndex, xCount, yCount, xSize, ySize, xBlank, yBlank)
		self.startIndex = StartIndex

	def GetStartIndex(self):
		return self.startIndex

class TitleBar(Window):

	BLOCK_WIDTH = 60
	BLOCK_WIDTH2 = 40
	BLOCK_HEIGHT = 23

	def __init__(self):
		Window.__init__(self)
		self.AddFlag("attach")

	def __del__(self):
		Window.__del__(self)

	def MakeTitleBar(self, width, color):


		width = max(64, width)

		imgLeft = ImageBox()
		imgCenter = ExpandedImageBox()
		imgRight = ImageBox()
		imgLeft.AddFlag("not_pick")
		imgCenter.AddFlag("not_pick")
		imgRight.AddFlag("not_pick")
		imgLeft.SetParent(self)
		imgCenter.SetParent(self)
		imgRight.SetParent(self)

		if localeInfo.IsARABIC():
			imgLeft.LoadImage("locale/ae/ui/pattern/titlebar_left.tga")
			imgCenter.LoadImage("locale/ae/ui/pattern/titlebar_center.tga")
			imgRight.LoadImage("locale/ae/ui/pattern/titlebar_right.tga")
		else:
			imgLeft.LoadImage("d:/ymir work/ui/pattern/titlebar_left.tga")
			imgCenter.LoadImage("d:/ymir work/ui/pattern/titlebar_center.tga")
			imgRight.LoadImage("d:/ymir work/ui/pattern/titlebar_right.tga")

		imgLeft.Show()
		imgCenter.Show()
		imgRight.Show()

		btnClose = Button()
		btnClose.SetParent(self)
		btnClose.SetUpVisual("elvion/game/titlebar/close_btn_norm.png")
		btnClose.SetOverVisual("elvion/game/titlebar/close_btn_hover.png")
		btnClose.SetDownVisual("elvion/game/titlebar/close_btn_down.png")
		btnClose.Show()

		self.imgLeft = imgLeft
		self.imgCenter = imgCenter
		self.imgRight = imgRight
		self.btnClose = btnClose

		self.SetWidth(width)

	def SetWidth(self, width):
		self.imgCenter.SetRenderingRect(0.0, 0.0, float((width - self.BLOCK_WIDTH2*2) - self.BLOCK_WIDTH2) / self.BLOCK_WIDTH2, 0.0)
		self.imgCenter.SetPosition(self.BLOCK_WIDTH-12, -4)
		self.imgRight.SetPosition(width+12 - self.BLOCK_WIDTH, -4)
		self.imgLeft.SetPosition(-12, -4)

		if localeInfo.IsARABIC():
			self.btnClose.SetPosition(3, 3)
		else:
			self.btnClose.SetPosition(width - self.btnClose.GetWidth() + 12, -3)

		self.SetSize(width + 12, self.BLOCK_HEIGHT)

	def SetCloseEvent(self, event):
		self.btnClose.SetEvent(event)

if app.ENABLE_QUEST_RENEWAL:
	class SubTitleBar(Button):
		def __init__(self):
			Button.__init__(self)

		def __del__(self):
			Button.__del__(self)

		def MakeSubTitleBar(self, width, color):
			width = max(64, width)
			self.SetWidth(width)
			self.SetUpVisual("d:/ymir work/ui/quest_re/quest_tab_01.tga")
			self.SetOverVisual("d:/ymir work/ui/quest_re/quest_tab_01.tga")
			self.SetDownVisual("d:/ymir work/ui/quest_re/quest_tab_01.tga")
			self.Show()

			scrollImage = ImageBox()
			scrollImage.SetParent(self)
			scrollImage.LoadImage("d:/ymir work/ui/quest_re/quest_down.tga")
			scrollImage.SetPosition(5, 2.5)
			scrollImage.AddFlag("not_pick")
			scrollImage.Show()
			self.scrollImage = scrollImage

		def OpenCategory(self, qcount = 0):
			if qcount > 0:
				self.scrollImage.LoadImage("d:/ymir work/ui/quest_re/quest_up.tga")
			else:
				self.scrollImage.LoadImage("d:/ymir work/ui/quest_re/quest_down.tga")

		def CloseCategory(self, qcount = 0):
			self.scrollImage.LoadImage("d:/ymir work/ui/quest_re/quest_down.tga")

		def SetQuestLabel(self, filename, qcount):
			tabColor = ImageBox()
			tabColor.SetParent(self)
			tabColor.LoadImage(filename)
			tabColor.AddFlag("not_pick")
			tabColor.SetPosition(188, 12)
			if qcount > 0:
				tabColor.Show()
			else:
				tabColor.Hide()
			self.tabColor = tabColor

		def SetWidth(self, width):
			self.SetPosition(32, 0)
			self.SetSize(width, 23)

	class ListBar(Button):
		def __init__(self):
			Button.__init__(self)

		def __del__(self):
			Button.__del__(self)

		def MakeListBar(self, width, color):
			width = max(64, width)
			self.SetWidth(width)
			self.Show()

			checkbox = ImageBox()
			checkbox.SetParent(self)
			checkbox.LoadImage("d:/ymir work/ui/quest_re/quest_new.tga")
			checkbox.SetPosition(10, 9)
			checkbox.AddFlag("not_pick")
			checkbox.Show()
			self.checkbox = checkbox
			self.isChecked = False

		def SetWidth(self, width):
			self.SetPosition(32, 0)
			self.SetSize(width, 23)

		def CallEvent(self):
			self.OnClickEvent()
			super(ListBar, self).CallEvent()

		def OnClickEvent(self):
			self.checkbox.Hide()
			self.isChecked = True

		def SetSlot(self, slotIndex, itemIndex, width, height, icon, diffuseColor = (1.0, 1.0, 1.0, 1.0)):
			wndMgr.SetSlot(self.hWnd, slotIndex, itemIndex, width, height, icon, diffuseColor)

class HorizontalBar(Window):

	BLOCK_WIDTH = 32
	BLOCK_HEIGHT = 17

	def __init__(self):
		Window.__init__(self)
		self.AddFlag("attach")

	def __del__(self):
		Window.__del__(self)

	def Create(self, width):

		width = max(96, width)

		imgLeft = ImageBox()
		imgLeft.SetParent(self)
		imgLeft.AddFlag("not_pick")
		imgLeft.LoadImage("d:/ymir work/ui/pattern/horizontalbar_left.tga")
		imgLeft.Show()

		imgCenter = ExpandedImageBox()
		imgCenter.SetParent(self)
		imgCenter.AddFlag("not_pick")
		imgCenter.LoadImage("d:/ymir work/ui/pattern/horizontalbar_center.tga")
		imgCenter.Show()

		imgRight = ImageBox()
		imgRight.SetParent(self)
		imgRight.AddFlag("not_pick")
		imgRight.LoadImage("d:/ymir work/ui/pattern/horizontalbar_right.tga")
		imgRight.Show()

		self.imgLeft = imgLeft
		self.imgCenter = imgCenter
		self.imgRight = imgRight
		self.SetWidth(width)

	def SetWidth(self, width):
		self.imgCenter.SetRenderingRect(0.0, 0.0, float((width - self.BLOCK_WIDTH*2) - self.BLOCK_WIDTH) / self.BLOCK_WIDTH, 0.0)
		self.imgCenter.SetPosition(self.BLOCK_WIDTH, 0)
		self.imgRight.SetPosition(width - self.BLOCK_WIDTH, 0)
		self.SetSize(width, self.BLOCK_HEIGHT)

class Gauge(Window):

	SLOT_WIDTH = 2
	SLOT_HEIGHT = 16

	GAUGE_TEMPORARY_PLACE = 2
	GAUGE_WIDTH = 16

	def __init__(self):
		Window.__init__(self)
		self.__oldValue = 0
		self.__newValue = 0
		self.width = 0

	def __del__(self):
		Window.__del__(self)

	def MakeGauge(self, width, color):

		self.width = max(48, width)

		imgSlotLeft = ImageBox()
		imgSlotLeft.SetParent(self)
		imgSlotLeft.LoadImage("elvion/game/target/gauge_slot_left.png")
		imgSlotLeft.Show()

		imgSlotRight = ImageBox()
		imgSlotRight.SetParent(self)
		imgSlotRight.LoadImage("elvion/game/target/gauge_slot_right.png")
		imgSlotRight.Show()
		imgSlotRight.SetPosition(width - self.SLOT_WIDTH, 0)

		imgSlotCenter = ExpandedImageBox()
		imgSlotCenter.SetParent(self)
		imgSlotCenter.LoadImage("elvion/game/target/gauge_slot_center.png")
		imgSlotCenter.Show()
		imgSlotCenter.SetRenderingRect(0.0, 0.0, float((width - self.SLOT_WIDTH*2) - self.SLOT_WIDTH) / self.SLOT_WIDTH, 0.0)
		imgSlotCenter.SetPosition(self.SLOT_WIDTH, 0)

		imgGaugeBack = ExpandedImageBox()
		imgGaugeBack.SetParent(self)
		imgGaugeBack.LoadImage("elvion/game/target/gauge_yellow.png")
		imgGaugeBack.Hide()
		imgGaugeBack.SetRenderingRect(0.0, 0.0, 0.0, 0.0)
		imgGaugeBack.SetPosition(self.GAUGE_TEMPORARY_PLACE, 0)

		imgGauge = ExpandedImageBox()
		imgGauge.SetParent(self)
		imgGauge.LoadImage("elvion/game/target/gauge_" + color + ".png")
		imgGauge.Show()
		imgGauge.SetRenderingRect(0.0, 0.0, 0.0, 0.0)
		imgGauge.SetPosition(self.GAUGE_TEMPORARY_PLACE, 0)

		imgSlotLeft.AddFlag("attach")
		imgSlotCenter.AddFlag("attach")
		imgGaugeBack.AddFlag("attach")
		imgSlotRight.AddFlag("attach")


		self.imgLeft = imgSlotLeft
		self.imgCenter = imgSlotCenter
		self.imgRight = imgSlotRight
		self.imgGauge = imgGauge
		self.imgGaugeBack = imgGaugeBack

		self.SetSize(width, self.SLOT_HEIGHT)

	def SetPercentage(self, curValue, maxValue):

		if maxValue > 0.0:
			percentage = min(1.0, float(curValue)/float(maxValue))
		else:
			percentage = 0.0

		self.__oldValue = self.__newValue
		self.__newValue = percentage

		gaugeSize = -1.0 + float(self.width - self.GAUGE_TEMPORARY_PLACE*2) * percentage / self.GAUGE_WIDTH
		self.imgGauge.SetRenderingRect(0.0, 0.0, gaugeSize, 0.0)

		self.SetPercentageBack(self.__oldValue, 1.0)

	def SetPercentageBack(self, curValue, maxValue):
		if not self.imgGaugeBack.IsShow():
			self.imgGaugeBack.Show()

		if maxValue > 0.0:
			percentage = min(1.0, float(curValue) / float(maxValue))
		else:
			percentage = 0.0

		gaugeSize = -1.0 + float(self.width - self.GAUGE_TEMPORARY_PLACE * 2) * percentage / self.GAUGE_WIDTH
		self.imgGaugeBack.SetRenderingRect(0.0, 0.0, gaugeSize, 0.0)

	def OnUpdate(self):
		if self.IsShow() and self.__oldValue > self.__newValue:
			self.__oldValue = self.__oldValue - 0.005
			self.SetPercentageBack(self.__oldValue, 1.0)


	def SetGaugeColor(self, color):
		if self.imgGauge:
			self.imgGauge.LoadImage("elvion/game/target/gauge_" + color + ".png")

class Board(Window):

	CORNER_WIDTH = 32
	CORNER_HEIGHT = 32
	LINE_WIDTH = 128
	LINE_HEIGHT = 128

	LT = 0
	LB = 1
	RT = 2
	RB = 3
	L = 0
	R = 1
	T = 2
	B = 3

	def __init__(self):
		Window.__init__(self)

		self.MakeBoard("d:/ymir work/ui/pattern/Board_Corner_", "d:/ymir work/ui/pattern/Board_Line_")
		self.MakeBase()

	def MakeBoard(self, cornerPath, linePath):

		CornerFileNames = [ cornerPath+dir+".tga" for dir in ("LeftTop", "LeftBottom", "RightTop", "RightBottom", ) ]
		LineFileNames = [ linePath+dir+".tga" for dir in ("Left", "Right", "Top", "Bottom", ) ]
		"""
		CornerFileNames = (
							"d:/ymir work/ui/pattern/Board_Corner_LeftTop.tga",
							"d:/ymir work/ui/pattern/Board_Corner_LeftBottom.tga",
							"d:/ymir work/ui/pattern/Board_Corner_RightTop.tga",
							"d:/ymir work/ui/pattern/Board_Corner_RightBottom.tga",
							)
		LineFileNames = (
							"d:/ymir work/ui/pattern/Board_Line_Left.tga",
							"d:/ymir work/ui/pattern/Board_Line_Right.tga",
							"d:/ymir work/ui/pattern/Board_Line_Top.tga",
							"d:/ymir work/ui/pattern/Board_Line_Bottom.tga",
							)
		"""

		self.Corners = []
		for fileName in CornerFileNames:
			Corner = ExpandedImageBox()
			Corner.AddFlag("not_pick")
			Corner.LoadImage(fileName)
			Corner.SetParent(self)
			Corner.SetPosition(0, 0)
			Corner.Show()
			self.Corners.append(Corner)

		self.Lines = []
		for fileName in LineFileNames:
			Line = ExpandedImageBox()
			Line.AddFlag("not_pick")
			Line.LoadImage(fileName)
			Line.SetParent(self)
			Line.SetPosition(0, 0)
			Line.Show()
			self.Lines.append(Line)

		self.Lines[self.L].SetPosition(0, self.CORNER_HEIGHT)
		self.Lines[self.T].SetPosition(self.CORNER_WIDTH, 0)

	def MakeBase(self):
		self.Base = ExpandedImageBox()
		self.Base.AddFlag("not_pick")
		self.Base.LoadImage("d:/ymir work/ui/pattern/Board_Base.tga")
		self.Base.SetParent(self)
		self.Base.SetPosition(self.CORNER_WIDTH, self.CORNER_HEIGHT)
		self.Base.Show()

	def __del__(self):
		Window.__del__(self)

	def SetSize(self, width, height):

		width = max(self.CORNER_WIDTH*2, width)
		height = max(self.CORNER_HEIGHT*2, height)
		Window.SetSize(self, width, height)

		self.Corners[self.LB].SetPosition(0, height - self.CORNER_HEIGHT)
		self.Corners[self.RT].SetPosition(width - self.CORNER_WIDTH, 0)
		self.Corners[self.RB].SetPosition(width - self.CORNER_WIDTH, height - self.CORNER_HEIGHT)
		self.Lines[self.R].SetPosition(width - self.CORNER_WIDTH, self.CORNER_HEIGHT)
		self.Lines[self.B].SetPosition(self.CORNER_HEIGHT, height - self.CORNER_HEIGHT)

		verticalShowingPercentage = float((height - self.CORNER_HEIGHT*2) - self.LINE_HEIGHT) / self.LINE_HEIGHT
		horizontalShowingPercentage = float((width - self.CORNER_WIDTH*2) - self.LINE_WIDTH) / self.LINE_WIDTH
		self.Lines[self.L].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.R].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.T].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
		self.Lines[self.B].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)

		if self.Base:
			self.Base.SetRenderingRect(0, 0, horizontalShowingPercentage, verticalShowingPercentage)

class BorderB(Board):
	CORNER_WIDTH = 16
	CORNER_HEIGHT = 16
	LINE_WIDTH = 16
	LINE_HEIGHT = 16
	
	BASE_PATH = "d:/ymir work/ui/pattern"
	
	IMAGES = {
		'CORNER' : {
			0 : "border_b_left_top",
			1 : "border_b_left_bottom",
			2 : "border_b_right_top",
			3 : "border_b_right_bottom"
		},
		'BAR' : {
			0 : "border_b_left",
			1 : "border_b_right",
			2 : "border_b_top",
			3 : "border_b_bottom"
		},
		'FILL' : "border_b_center"
	}
	
	def __init__(self):
		Board.__init__(self)
			
		self.eventFunc = {
			"MOUSE_LEFT_BUTTON_UP" : None,
		}
		self.eventArgs = {
			"MOUSE_LEFT_BUTTON_UP" : None,
		}

	
	def __del__(self):
		Board.__del__(self)
		self.eventFunc = None
		self.eventArgs = None
	
	def SetSize(self, width, height):
		Board.SetSize(self, width, height)
		
	def SetEvent(self, func, *args) :
		result = self.eventFunc.has_key(args[0])       
		if result:
			self.eventFunc[args[0]] = func
			self.eventArgs[args[0]] = args
		else:
			pass
			
	def OnMouseLeftButtonUp(self):
		if self.eventFunc["MOUSE_LEFT_BUTTON_UP"] :
			apply(self.eventFunc["MOUSE_LEFT_BUTTON_UP"], self.eventArgs["MOUSE_LEFT_BUTTON_UP"])
			
class BorderA(Window):
	CORNER_WIDTH = 16
	CORNER_HEIGHT = 16
	LINE_WIDTH = 16
	LINE_HEIGHT = 16
	BOARD_COLOR = grp.GenerateColor(0.17, 0.027, 0.0, 0.70)


	LT = 0
	LB = 1
	RT = 2
	RB = 3
	L = 0
	R = 1
	T = 2
	B = 3

	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)

		CornerFileNames = [ "d:/ymir work/ui/pattern/border_a/border_a_"+dir+".tga" for dir in ["Left_Top","Left_Bottom","Right_Top","Right_Bottom"] ]
		LineFileNames = [ "d:/ymir work/ui/pattern/border_a/border_a_"+dir+".tga" for dir in ["Left","Right","Top","Bottom"] ]
		
		self.Corners = []
		for fileName in CornerFileNames:
			Corner = ExpandedImageBox()
			Corner.AddFlag("not_pick")
			Corner.LoadImage(fileName)
			Corner.SetParent(self)
			Corner.SetPosition(0, 0)
			Corner.Show()
			self.Corners.append(Corner)

		self.Lines = []
		for fileName in LineFileNames:
			Line = ExpandedImageBox()
			Line.AddFlag("not_pick")
			Line.LoadImage(fileName)
			Line.SetParent(self)
			Line.SetPosition(0, 0)
			Line.Show()
			self.Lines.append(Line)

		self.Base = ExpandedImageBox()
		self.Base.AddFlag("not_pick")
		self.Base.LoadImage("d:/ymir work/ui/pattern/border_a/border_a_center.tga")
		self.Base.SetParent(self)
		self.Base.SetPosition(self.CORNER_WIDTH, self.CORNER_HEIGHT)
		self.Base.Show()
		
		self.Lines[self.L].SetPosition(0, self.CORNER_HEIGHT)
		self.Lines[self.T].SetPosition(self.CORNER_WIDTH, 0)

	def __del__(self):
		Window.__del__(self)

	def SetSize(self, width, height):

		width = max(self.CORNER_WIDTH*2, width)
		height = max(self.CORNER_HEIGHT*2, height)
		Window.SetSize(self, width, height)

		self.Corners[self.LB].SetPosition(0, height - self.CORNER_HEIGHT)
		self.Corners[self.RT].SetPosition(width - self.CORNER_WIDTH, 0)
		self.Corners[self.RB].SetPosition(width - self.CORNER_WIDTH, height - self.CORNER_HEIGHT)
		self.Lines[self.R].SetPosition(width - self.CORNER_WIDTH, self.CORNER_HEIGHT)
		self.Lines[self.B].SetPosition(self.CORNER_HEIGHT, height - self.CORNER_HEIGHT)

		verticalShowingPercentage = float((height - self.CORNER_HEIGHT*2) - self.LINE_HEIGHT) / self.LINE_HEIGHT
		horizontalShowingPercentage = float((width - self.CORNER_WIDTH*2) - self.LINE_WIDTH) / self.LINE_WIDTH
		self.Lines[self.L].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.R].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.T].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
		self.Lines[self.B].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)

		if self.Base:
			self.Base.SetRenderingRect(0, 0, horizontalShowingPercentage, verticalShowingPercentage)

	def ShowInternal(self):
		self.Base.Show()
		for wnd in self.Lines:
			wnd.Show()
		for wnd in self.Corners:
			wnd.Show()

	def HideInternal(self):
		self.Base.Hide()
		for wnd in self.Lines:
			wnd.Hide()
		for wnd in self.Corners:
			wnd.Hide()

class BoardWithTitleBar(Board):
	def __init__(self):
		Board.__init__(self)

		titleBar = TitleBar()
		titleBar.SetParent(self)
		titleBar.MakeTitleBar(0, "red")
		titleBar.SetPosition(21, 10)
		titleBar.Show()

		titleName = TextLine()
		titleName.SetParent(titleBar)
		titleName.SetPosition(0, 1)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		titleName.SetOutline()
		titleName.Show()

		self.titleBar = titleBar
		self.titleName = titleName

		self.SetCloseEvent(self.Hide)

	def __del__(self):
		Board.__del__(self)
		self.titleBar = None
		self.titleName = None

	def SetSize(self, width, height):
		self.titleBar.SetWidth(width - 42)
		Board.SetSize(self, width, height)
		self.titleName.UpdateRect()

	def SetTitleColor(self, color):
		self.titleName.SetPackedFontColor(color)

	def SetTitleName(self, name):
		self.titleName.SetText(name)

	def SetCloseEvent(self, event):
		self.titleBar.SetCloseEvent(event)

class ModernScrollBar(Window):
	MIDDLE_BAR_POS = 2
	MIDDLE_BAR_UPPER_PLACE = 1
	MIDDLE_BAR_DOWNER_PLACE = 1
	TEMP_SPACE = MIDDLE_BAR_UPPER_PLACE + MIDDLE_BAR_DOWNER_PLACE
	if app.__BL_SMOOTH_SCROLL__:
		SMOOTH_RATIO = 10.0  # UPPER = SLOWER

	class MiddleBar(DragButton):
		BUTTON_COLOR = 0xFF806b58
		BUTTON_ACTIVE_COLOR = 0xFFc2a993

		def __init__(self):
			DragButton.__init__(self)
			self.AddFlag("movable")

			self.bar = Bar()

		def MakeImage(self):
			self.bar = Bar()
			self.bar.SetParent(self)
			self.bar.SetPosition(0, 0)
			self.bar.SetSize(10, 0)
			self.bar.SetColor(self.BUTTON_COLOR)
			self.bar.AddFlag("not_pick")
			self.bar.Show()

		def OnMouseOverIn(self):
			self.bar.SetColor(self.BUTTON_ACTIVE_COLOR)

		def OnMouseOverOut(self):
			self.bar.SetColor(self.BUTTON_COLOR)

		def SetSize(self, width, height):
			height = max(12, height)
			DragButton.SetSize(self, width, height)
			self.bar.SetSize(width, height)

	def __init__(self):
		Window.__init__(self)

		self.pageSize = 1
		self.curPos = 0.0
		self.eventScroll = lambda *arg: None
		self.lockFlag = False
		self.scrollStep = 0.20
		self.contentHeight = 0
		self.scroll_span = 0
		
		if app.__BL_SMOOTH_SCROLL__:
			self.smooth_mode = False
			self.actual_pos = 0.0
			self.target_pos = 0.0

		self.CreateScrollBar()

	def __del__(self):
		Window.__del__(self)

	def CreateScrollBar(self):
		self.SetSize(10, 0)

		self.barSlot = BoxedBoard()
		self.barSlot.SetParent(self)
		self.barSlot.AddFlag("not_pick")
		self.barSlot.Show()

		self.middleBar = self.MiddleBar()
		self.middleBar.SetParent(self)
		self.middleBar.SetMoveEvent(__mem_func__(self.OnMove))
		self.middleBar.Show()
		self.middleBar.MakeImage()
		self.middleBar.SetSize(self.GetWidth() - 2, 12)

	def Destroy(self):
		self.barSlot = None
		self.middleBar = None
		self.eventScroll = lambda *arg: None

	def SetScrollEvent(self, event):
		self.eventScroll = event

	def __SetMiddleBarSize(self, pageScale):
		realHeight = self.GetHeight() - 2
		self.middleBar.SetSize(self.GetWidth() - 2, int(pageScale * float(realHeight)))
		self.pageSize = self.GetHeight() - self.middleBar.GetHeight() - self.TEMP_SPACE

	def SetMiddleBarSize(self, tmp):
		self.SetContentHeight(self.contentHeight)

	def SetContentHeight(self, contentHeight):
		contentHeight = max(contentHeight, self.GetHeight() - self.TEMP_SPACE)
		self.contentHeight = contentHeight
		self.__SetMiddleBarSize(float(self.GetHeight() - 2) / float(contentHeight))

	def SetScrollBarSize(self, height):
		self.pageSize = height - self.middleBar.GetHeight() - self.TEMP_SPACE
		self.SetSize(self.GetWidth(), height)
		self.middleBar.SetRestrictMovementArea(1, self.MIDDLE_BAR_UPPER_PLACE, self.middleBar.GetWidth(), height - self.TEMP_SPACE)
		self.middleBar.SetPosition(1, self.MIDDLE_BAR_UPPER_PLACE)

		self.UpdateBarSlot()

	def UpdateBarSlot(self):
		self.barSlot.SetSize(self.GetWidth(), self.GetHeight())

	def SetWidth(self, width):
		self.SetSize(width, self.GetHeight())
		self.middleBar.SetSize(width - 2, self.middleBar.GetHeight())
		self.SetScrollBarSize(self.GetHeight())

	def GetPos(self):
		return self.curPos

	def SetSpan(self, span):
		self.scroll_span = span

	def IncreaseSpan(self, span):
		self.scroll_span += span

	def GetSpan(self):
		return self.scroll_span

	def SetPos(self, pos):
		pos = max(0.0, pos)
		pos = min(1.0, pos)

		newPos = float(self.pageSize) * pos
		self.middleBar.SetPosition(self.MIDDLE_BAR_POS, int(newPos) + self.MIDDLE_BAR_UPPER_PLACE)
		self.OnMove()

	def SetScrollStep(self, step):
		self.scrollStep = step

	def GetScrollStep(self):
		return self.scrollStep

	def OnUp(self):
		if app.__BL_SMOOTH_SCROLL__ and self.smooth_mode:
			self.actual_pos = min(1.0, max(0.0, self.curPos))
			self.target_pos = min(1.0, max(0.0, self.curPos - self.scrollStep))
		else:
			self.SetPos(self.curPos - self.scrollStep)

	def OnDown(self):
		if app.__BL_SMOOTH_SCROLL__ and self.smooth_mode:
			self.actual_pos = min(1.0, max(0.0, self.curPos))
			self.target_pos = min(1.0, max(0.0, self.curPos + self.scrollStep))
		else:
			self.SetPos(self.curPos + self.scrollStep)

	def OnMove(self):
		if self.lockFlag:
			return

		if 0 == self.pageSize:
			return

		(xLocal, yLocal) = self.middleBar.GetLocalPosition()
		self.curPos = float(yLocal - self.MIDDLE_BAR_UPPER_PLACE) / float(self.pageSize)
		self.eventScroll()

	def OnMouseLeftButtonDown(self):
		(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()
		pickedPos = yMouseLocalPosition - (self.middleBar.GetHeight() / 2)
		newPos = float(pickedPos) / float(self.pageSize)
		self.SetPos(newPos)

	def LockScroll(self):
		self.lockFlag = True

	def UnlockScroll(self):
		self.lockFlag = False

	def OnMouseWheel(self, len):
		if app.__BL_SMOOTH_SCROLL__ and self.smooth_mode:
			# Calculate new target position based on wheel direction
			curY = int(self.GetPos() * float(self.contentHeight)) - len
			newPos = float(curY) / float(self.contentHeight)
			
			self.actual_pos = min(1.0, max(0.0, self.curPos))
			self.target_pos = min(1.0, max(0.0, newPos))
		else:
			curY = int(self.GetPos() * float(self.contentHeight)) - len
			self.SetPos(float(curY) / float(self.contentHeight))
		return True

	if app.__BL_SMOOTH_SCROLL__:
		def EnableSmoothMode(self):
			self.smooth_mode = True

		def OnUpdate(self):
			if not self.smooth_mode:
				return
			
			if self.lockFlag:
				return
			
			if 0 == self.pageSize:
				return
			
			if self.actual_pos == self.target_pos:
				return

			distance = abs(self.actual_pos - self.target_pos)
			smooth_step = max(distance / ModernScrollBar.SMOOTH_RATIO, 0.005)
		
			if self.actual_pos < self.target_pos:
				self.actual_pos = min(self.actual_pos + smooth_step, self.target_pos)
			elif self.actual_pos > self.target_pos:
				self.actual_pos = max(self.actual_pos - smooth_step, self.target_pos)
			
			self.SetPos(self.actual_pos)

class ThinBoardCircle(Window):
	CORNER_WIDTH = 4
	CORNER_HEIGHT = 4
	LINE_WIDTH = 4
	LINE_HEIGHT = 4
	BOARD_COLOR = grp.GenerateColor(0.0, 0.0, 0.0, 1.0)

	LT = 0
	LB = 1
	RT = 2
	RB = 3
	L = 0
	R = 1
	T = 2
	B = 3

	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)

		CornerFileNames = [ "d:/ymir work/ui/pattern/thinboardcircle/ThinBoard_Corner_"+dir+"_Circle.tga" for dir in ["LeftTop","LeftBottom","RightTop","RightBottom"] ]
		LineFileNames = [ "d:/ymir work/ui/pattern/thinboardcircle/ThinBoard_Line_"+dir+"_Circle.tga" for dir in ["Left","Right","Top","Bottom"] ]

		self.Corners = []
		for fileName in CornerFileNames:
			Corner = ExpandedImageBox()
			Corner.AddFlag("attach")
			Corner.AddFlag("not_pick")
			Corner.LoadImage(fileName)
			Corner.SetParent(self)
			Corner.SetPosition(0, 0)
			Corner.Show()
			self.Corners.append(Corner)

		self.Lines = []
		for fileName in LineFileNames:
			Line = ExpandedImageBox()
			Line.AddFlag("attach")
			Line.AddFlag("not_pick")
			Line.LoadImage(fileName)
			Line.SetParent(self)
			Line.SetPosition(0, 0)
			Line.Show()
			self.Lines.append(Line)

		Base = Bar()
		Base.SetParent(self)
		Base.AddFlag("attach")
		Base.AddFlag("not_pick")
		Base.SetPosition(self.CORNER_WIDTH, self.CORNER_HEIGHT)
		Base.SetColor(self.BOARD_COLOR)
		Base.Show()
		self.Base = Base
		
		self.ButtonText = None
		self.BonusId = 0

		self.Lines[self.L].SetPosition(0, self.CORNER_HEIGHT)
		self.Lines[self.T].SetPosition(self.CORNER_WIDTH, 0)

	def __del__(self):
		Window.__del__(self)

	def SetSize(self, width, height):

		width = max(self.CORNER_WIDTH*2, width)
		height = max(self.CORNER_HEIGHT*2, height)
		Window.SetSize(self, width, height)

		self.Corners[self.LB].SetPosition(0, height - self.CORNER_HEIGHT)
		self.Corners[self.RT].SetPosition(width - self.CORNER_WIDTH, 0)
		self.Corners[self.RB].SetPosition(width - self.CORNER_WIDTH, height - self.CORNER_HEIGHT)
		self.Lines[self.R].SetPosition(width - self.CORNER_WIDTH, self.CORNER_HEIGHT)
		self.Lines[self.B].SetPosition(self.CORNER_HEIGHT, height - self.CORNER_HEIGHT)

		verticalShowingPercentage = float((height - self.CORNER_HEIGHT*2) - self.LINE_HEIGHT) / self.LINE_HEIGHT
		horizontalShowingPercentage = float((width - self.CORNER_WIDTH*2) - self.LINE_WIDTH) / self.LINE_WIDTH
		self.Lines[self.L].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.R].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.T].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
		self.Lines[self.B].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
		self.Base.SetSize(width - self.CORNER_WIDTH*2, height - self.CORNER_HEIGHT*2)
		
	def SetText(self, text):
		if not self.ButtonText:
			textLine = TextLine()
			textLine.SetParent(self)
			textLine.SetPosition(self.GetWidth()/2, self.GetHeight()/2)
			textLine.SetVerticalAlignCenter()
			textLine.SetHorizontalAlignCenter()
			textLine.Show()
			self.ButtonText = textLine

		self.ButtonText.SetText(text)
		
	def GetText(self):
		if not self.ButtonText:
			return ""
		return self.ButtonText.GetText()
		
	def SetBonusId(self, bnsId):
		self.BonusId = bnsId
		
	def GetBonusId(self):
		if self.BonusId != 0:
			return self.BonusId

	def ShowInternal(self):
		self.Base.Show()
		for wnd in self.Lines:
			wnd.Show()
		for wnd in self.Corners:
			wnd.Show()

	def HideInternal(self):
		self.Base.Hide()
		for wnd in self.Lines:
			wnd.Hide()
		for wnd in self.Corners:
			wnd.Hide()
class ThinBoard(Window):

	CORNER_WIDTH = 16
	CORNER_HEIGHT = 16
	LINE_WIDTH = 16
	LINE_HEIGHT = 16
	BOARD_COLOR = grp.GenerateColor(0.0, 0.0, 0.0, 0.51)

	LT = 0
	LB = 1
	RT = 2
	RB = 3
	L = 0
	R = 1
	T = 2
	B = 3

	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)

		CornerFileNames = [ "d:/ymir work/ui/pattern/ThinBoard_Corner_"+dir+".tga" for dir in ["LeftTop","LeftBottom","RightTop","RightBottom"] ]
		LineFileNames = [ "d:/ymir work/ui/pattern/ThinBoard_Line_"+dir+".tga" for dir in ["Left","Right","Top","Bottom"] ]

		self.Corners = []
		for fileName in CornerFileNames:
			Corner = ExpandedImageBox()
			Corner.AddFlag("attach")
			Corner.AddFlag("not_pick")
			Corner.LoadImage(fileName)
			Corner.SetParent(self)
			Corner.SetPosition(0, 0)
			Corner.Show()
			self.Corners.append(Corner)

		self.Lines = []
		for fileName in LineFileNames:
			Line = ExpandedImageBox()
			Line.AddFlag("attach")
			Line.AddFlag("not_pick")
			Line.LoadImage(fileName)
			Line.SetParent(self)
			Line.SetPosition(0, 0)
			Line.Show()
			self.Lines.append(Line)

		Base = Bar()
		Base.SetParent(self)
		Base.AddFlag("attach")
		Base.AddFlag("not_pick")
		Base.SetPosition(self.CORNER_WIDTH, self.CORNER_HEIGHT)
		Base.SetColor(self.BOARD_COLOR)
		Base.Show()
		self.Base = Base

		self.Lines[self.L].SetPosition(0, self.CORNER_HEIGHT)
		self.Lines[self.T].SetPosition(self.CORNER_WIDTH, 0)

	def __del__(self):
		Window.__del__(self)

	def SetSize(self, width, height):

		width = max(self.CORNER_WIDTH*2, width)
		height = max(self.CORNER_HEIGHT*2, height)
		Window.SetSize(self, width, height)

		self.Corners[self.LB].SetPosition(0, height - self.CORNER_HEIGHT)
		self.Corners[self.RT].SetPosition(width - self.CORNER_WIDTH, 0)
		self.Corners[self.RB].SetPosition(width - self.CORNER_WIDTH, height - self.CORNER_HEIGHT)
		self.Lines[self.R].SetPosition(width - self.CORNER_WIDTH, self.CORNER_HEIGHT)
		self.Lines[self.B].SetPosition(self.CORNER_HEIGHT, height - self.CORNER_HEIGHT)

		verticalShowingPercentage = float((height - self.CORNER_HEIGHT*2) - self.LINE_HEIGHT) / self.LINE_HEIGHT
		horizontalShowingPercentage = float((width - self.CORNER_WIDTH*2) - self.LINE_WIDTH) / self.LINE_WIDTH
		self.Lines[self.L].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.R].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.T].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
		self.Lines[self.B].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
		self.Base.SetSize(width - self.CORNER_WIDTH*2, height - self.CORNER_HEIGHT*2)

	def ShowInternal(self):
		self.Base.Show()
		for wnd in self.Lines:
			wnd.Show()
		for wnd in self.Corners:
			wnd.Show()

	def HideInternal(self):
		self.Base.Hide()
		for wnd in self.Lines:
			wnd.Hide()
		for wnd in self.Corners:
			wnd.Hide()

class ThinBoardNew(Window):

	CORNER_WIDTH = 14
	CORNER_HEIGHT = 14
	LINE_WIDTH = 14
	LINE_HEIGHT = 14
	BOARD_COLOR = grp.GenerateColor(0.0, 0.0, 0.0, 0.51)

	LT = 0
	LB = 1
	RT = 2
	RB = 3
	L = 0
	R = 1
	T = 2
	B = 3

	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)

		CornerFileNames = [ "d:/ymir work/ui/pattern/thinboard_new/thinboard_corner_"+dir+".png" for dir in ["lefttop","leftbottom","righttop","rightbottom"] ]
		LineFileNames = [ "d:/ymir work/ui/pattern/thinboard_new/thinboard_line_"+dir+".png" for dir in ["left","right","top","bottom"] ]
		self.Corners = []
		for fileName in CornerFileNames:
			Corner = ExpandedImageBox()
			Corner.AddFlag("attach")
			Corner.AddFlag("not_pick")
			Corner.LoadImage(fileName)
			Corner.SetParent(self)
			Corner.SetPosition(0, 0)
			Corner.Show()
			self.Corners.append(Corner)

		self.Lines = []
		for fileName in LineFileNames:
			Line = ExpandedImageBox()
			Line.AddFlag("attach")
			Line.AddFlag("not_pick")
			Line.LoadImage(fileName)
			Line.SetParent(self)
			Line.SetPosition(0, 0)
			Line.Show()
			self.Lines.append(Line)

		Base = Bar()
		Base.SetParent(self)
		Base.AddFlag("attach")
		Base.AddFlag("not_pick")
		Base.SetPosition(self.CORNER_WIDTH, self.CORNER_HEIGHT)
		Base.SetColor(self.BOARD_COLOR)
		Base.Show()
		self.Base = Base

		self.Lines[self.L].SetPosition(0, self.CORNER_HEIGHT)
		self.Lines[self.T].SetPosition(self.CORNER_WIDTH, 0)

	def __del__(self):
		Window.__del__(self)

	def SetSize(self, width, height):

		width = max(self.CORNER_WIDTH*2, width)
		height = max(self.CORNER_HEIGHT*2, height)
		Window.SetSize(self, width, height)

		self.Corners[self.LB].SetPosition(0, height - self.CORNER_HEIGHT)
		self.Corners[self.RT].SetPosition(width - self.CORNER_WIDTH, 0)
		self.Corners[self.RB].SetPosition(width - self.CORNER_WIDTH, height - self.CORNER_HEIGHT)
		self.Lines[self.R].SetPosition(width - self.CORNER_WIDTH, self.CORNER_HEIGHT)
		self.Lines[self.B].SetPosition(self.CORNER_HEIGHT, height - self.CORNER_HEIGHT)

		verticalShowingPercentage = float((height - self.CORNER_HEIGHT*2) - self.LINE_HEIGHT) / self.LINE_HEIGHT
		horizontalShowingPercentage = float((width - self.CORNER_WIDTH*2) - self.LINE_WIDTH) / self.LINE_WIDTH
		self.Lines[self.L].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.R].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.T].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
		self.Lines[self.B].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
		self.Base.SetSize(width - self.CORNER_WIDTH*2, height - self.CORNER_HEIGHT*2)

	def ShowInternal(self):
		self.Base.Show()
		for wnd in self.Lines:
			wnd.Show()
		for wnd in self.Corners:
			wnd.Show()

	def HideInternal(self):
		self.Base.Hide()
		for wnd in self.Lines:
			wnd.Hide()
		for wnd in self.Corners:
			wnd.Hide()

class MultiTextLine(Window):
	def __del__(self):
		Window.__del__(self)
	def Destroy(self):
		self.textRules = {}
	def __init__(self):
		Window.__init__(self)
		self.Destroy()
		self.AddFlag("not_pick")
		self.textRules["textRange"] = 15
		self.textRules["text"] = ""
		self.textRules["textType"] = ""
		self.textRules["fontName"] = ""
		self.textRules["hexColor"] = 0
		self.textRules["fontColor"] = 0
		self.textRules["outline"] = 0
	def SetTextType(self, textType):
		self.textRules["textType"] = textType
		self.Refresh()
	def SetTextRange(self, textRange):
		self.textRules["textRange"] = textRange
		self.Refresh()
	def SetOutline(self, outline):
		self.textRules["outline"] = outline
		self.Refresh()
	def SetPackedFontColor(self, hexColor):
		self.textRules["hexColor"] = hexColor
		self.Refresh()
	def SetFontColor(self, r, g, b):
		self.textRules["fontColor"] =[r, g, b]
		self.Refresh()
	def SetFontName(self, fontName):
		self.textRules["fontName"] = fontName
		self.Refresh()
	def SetText(self, newText):
		self.textRules["text"] = newText
		self.Refresh()
	def Refresh(self):
		textRules = self.textRules
		if textRules["text"] == "":
			return
		self.children=[]
		outline = textRules["outline"]
		fontColor = textRules["fontColor"]
		hexColor = textRules["hexColor"]
		yRange = textRules["textRange"]
		fontName = textRules["fontName"]
		textTypeList = textRules["textType"].split("?")
		totalTextList = textRules["text"].split("#")

		(xPosition, yPosition) = (0, 0)
		width = 0
		for text in totalTextList:
			childText = TextLine()
			childText.SetParent(self)
			childText.AddFlag("not_pick")
			childText.SetPosition(xPosition, yPosition)
			if fontName != "":
				childText.SetFontName(fontName)
			if hexColor != 0:
				childText.SetPackedFontColor(hexColor)
			if fontColor != 0:
				childText.SetFontColor(*fontColor)
			if outline:
				childText.SetOutline()
			for textType in textTypeList:
				self.AddTextType(childText, textType.split("#"))
			childText.SetText(str(text))
			if childText.GetTextSize()[0] > width:
				width = childText.GetTextSize()[0]
			childText.Show()
			self.children.append(childText)
			yPosition+=yRange

	def AddTextType(self, text,  typeArg):
		if len(typeArg) != 2:
			return
		_typeDict = {
			"vertical": {
				"top":text.SetVerticalAlignTop,
				"bottom":text.SetVerticalAlignBottom,
				"center":text.SetVerticalAlignCenter,
			},
			"horizontal": {
				"left":text.SetHorizontalAlignLeft,
				"right":text.SetHorizontalAlignRight,
				"center":text.SetHorizontalAlignCenter,
			},
			"all_align": {
				"1" : [text.SetHorizontalAlignCenter,text.SetVerticalAlignCenter,text.SetWindowHorizontalAlignCenter,text.SetWindowVerticalAlignCenter],
			},
		}
		(firstToken, secondToken) = tuple(typeArg)
		if _typeDict.has_key(firstToken):
			textType = _typeDict[firstToken][secondToken] if _typeDict[firstToken].has_key(secondToken) else None
			if textType != None:
				if isinstance(textType, list):
					for rule in textType:
						rule()
				else:
					textType()
					
class NewSlimScrollBar(Window):

	SCROLLBAR_WIDTH = 13
	SCROLLBAR_MIDDLE_HEIGHT = 1
	SCROLLBAR_BUTTON_WIDTH = 17
	SCROLLBAR_BUTTON_HEIGHT = 17
	SCROLL_BTN_XDIST = 1
	SCROLL_BTN_YDIST = 2

	class MiddleBar(DragButton):
		def __init__(self):
			DragButton.__init__(self)
			self.AddFlag("movable")

			self.SetWindowName("NONAME_ScrollBar_MiddleBar")

		def MakeImage(self):
			top = ImageBox()
			top.SetParent(self)
			top.LoadImage("d:/ymir work/ui/pattern/newslimscroll/ScrollBar_Middle_Top_bottom.tga")
			top.AddFlag("not_pick")
			top.SetPosition(0, 0)
			top.Show()
			topScale = ExpandedImageBox()
			topScale.SetParent(self)
			topScale.SetPosition(0, top.GetHeight())
			topScale.LoadImage("d:/ymir work/ui/pattern/newslimscroll/ScrollBar_Middle_TopScale.tga")
			topScale.AddFlag("not_pick")
			topScale.Show()

			bottom = ImageBox()
			bottom.SetParent(self)
			bottom.LoadImage("d:/ymir work/ui/pattern/newslimscroll/ScrollBar_Middle_Top_bottom.tga")
			bottom.AddFlag("not_pick")
			bottom.Show()
			bottomScale = ExpandedImageBox()
			bottomScale.SetParent(self)
			bottomScale.LoadImage("d:/ymir work/ui/pattern/newslimscroll/ScrollBar_Middle_TopScale.tga")
			bottomScale.AddFlag("not_pick")
			bottomScale.Show()

			middle = ExpandedImageBox()
			middle.SetParent(self)
			middle.LoadImage("d:/ymir work/ui/pattern/newslimscroll/ScrollBar_Middle_Middle.tga")
			middle.AddFlag("not_pick")
			middle.Show()

			self.top = top
			self.topScale = topScale
			self.bottom = bottom
			self.bottomScale = bottomScale
			self.middle = middle

		def SetSize(self, height):
			minHeight = self.top.GetHeight() + self.bottom.GetHeight() + self.middle.GetHeight()
			height = max(minHeight, height)
			DragButton.SetSize(self, 10, height)

			scale = (height - minHeight) / 2
			extraScale = 0
			if (height - minHeight) % 2 == 1:
				extraScale = 1

			self.topScale.SetRenderingRect(0, 0, 0, scale - 1)
			self.middle.SetPosition(-0.99, self.top.GetHeight() + scale)
			self.bottomScale.SetPosition(-0.99, self.middle.GetBottom())
			self.bottomScale.SetRenderingRect(0, 0, 0, scale - 1 + extraScale)
			self.bottom.SetPosition(-0.99, height - self.bottom.GetHeight())

	def __init__(self):
		Window.__init__(self)

		self.pageSize = 1
		self.curPos = 0.0
		self.eventScroll = None
		self.eventArgs = None
		self.lockFlag = False

		self.CreateScrollBar()
		self.SetScrollBarSize(0)

		self.scrollStep = 0.20
		self.SetWindowName("NONAME_ScrollBar")

	def __del__(self):
		Window.__del__(self)

	def CreateScrollBar(self):
		middleImage = ExpandedImageBox()
		middleImage.SetParent(self)
		middleImage.AddFlag("not_pick")
		middleImage.SetPosition(0, 1)
		middleImage.LoadImage("d:/ymir work/ui/pattern/newslimscroll/SlimScrollBar_Middle.tga")
		middleImage.Show()
		self.middleImage = middleImage

		middleBar = self.MiddleBar()
		middleBar.SetParent(self)
		middleBar.SetMoveEvent(__mem_func__(self.OnMove))
		middleBar.Show()
		middleBar.MakeImage()
		middleBar.SetSize(0)
		self.middleBar = middleBar

	def Destroy(self):
		self.eventScroll = None
		self.eventArgs = None

	def SetScrollEvent(self, event, *args):
		self.eventScroll = event
		self.eventArgs = args

	def SetMiddleBarSize(self, pageScale):
		self.middleBar.SetSize(int(pageScale * float(self.GetHeight() - self.SCROLL_BTN_YDIST*2)))
		realHeight = self.GetHeight() - self.SCROLL_BTN_YDIST*2 - self.middleBar.GetHeight()
		self.pageSize = realHeight

	def SetScrollBarSize(self, height):
		self.SetSize(self.SCROLLBAR_WIDTH, height)

		self.pageSize = height - self.SCROLL_BTN_YDIST*2 - self.middleBar.GetHeight()

		middleImageScale = float((height - self.SCROLL_BTN_YDIST*2) - self.middleImage.GetHeight()) / float(self.middleImage.GetHeight())
		self.middleImage.SetRenderingRect(0, 0, 0, middleImageScale)

		self.middleBar.SetRestrictMovementArea(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST, \
			self.middleBar.GetWidth(), height - self.SCROLL_BTN_YDIST * 2)
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST)

	def SetScrollStep(self, step):
		self.scrollStep = step

	def GetScrollStep(self):
		return self.scrollStep

	def GetPos(self):
		return self.curPos

	def OnUp(self):
		self.SetPos(self.curPos-self.scrollStep)

	def OnDown(self):
		self.SetPos(self.curPos+self.scrollStep)

	def SetPos(self, pos, moveEvent = True):
		pos = max(0.0, pos)
		pos = min(1.0, pos)

		newPos = float(self.pageSize) * pos
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, int(newPos) + self.SCROLL_BTN_YDIST)
		if moveEvent == True:
			self.OnMove()

	def OnMove(self):
		if self.lockFlag:
			return

		if 0 == self.pageSize:
			return

		(xLocal, yLocal) = self.middleBar.GetLocalPosition()
		self.curPos = float(yLocal - self.SCROLL_BTN_YDIST) / float(self.pageSize)

		if self.eventScroll:
			apply(self.eventScroll, self.eventArgs)

	def OnMouseLeftButtonDown(self):
		(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()
		newPos = float(yMouseLocalPosition) / float(self.GetHeight())
		self.SetPos(newPos)

	def LockScroll(self):
		self.lockFlag = True

	def UnlockScroll(self):
		self.lockFlag = False


class MouseReflector(Window):
	def __init__(self, parent):
		Window.__init__(self)
		self.SetParent(parent)
		self.AddFlag("not_pick")
		self.width = self.height = 0
		self.isDown = False
	def __del__(self):
		Window.__del__(self)
	def Down(self):
		self.isDown = True
	def Up(self):
		self.isDown = False
	def OnRender(self):
		if self.isDown:
			grp.SetColor(WHITE_COLOR)
		else:
			grp.SetColor(HALF_WHITE_COLOR)
		x, y = self.GetGlobalPosition()
		grp.RenderBar(x + 2, y + 2, self.GetWidth() - 4, self.GetHeight() - 4)
		
class CheckBoxNew(ImageBox):
	def __init__(self, parent, x, y, event, filename = "d:/ymir work/ui/chat/chattingoption_check_box_off.sub"):
		ImageBox.__init__(self)
		self.SetParent(parent)
		self.SetPosition(x, y)
		self.LoadImage(filename)
		self.mouseReflector = MouseReflector(self)
		self.mouseReflector.SetSize(self.GetWidth(), self.GetHeight())
		image = MakeImageBox(self, "d:/ymir work/ui/public/check_image.sub", 0, 0)
		image.AddFlag("not_pick")
		image.SetWindowHorizontalAlignCenter()
		image.SetWindowVerticalAlignCenter()
		image.Hide()
		self.check = False
		self.enable = True
		self.image = image
		self.event = event
		self.Show()
		self.mouseReflector.UpdateRect()
	def __del__(self):
		ImageBox.__del__(self)
	def GetCheck(self):
		return self.check
	def SetCheck(self, flag):
		if flag:
			self.check = True
			self.image.Show()
		else:
			self.check = False
			self.image.Hide()
	def Disable(self):
		self.enable = False
	def OnMouseOverIn(self):
		if not self.enable:
			return
		self.mouseReflector.Show()
	def OnMouseOverOut(self):
		if not self.enable:
			return
		self.mouseReflector.Hide()
	def OnMouseLeftButtonDown(self):
		if not self.enable:
			return
		self.mouseReflector.Down()
	def OnMouseLeftButtonUp(self):
		if not self.enable:
			return
		self.mouseReflector.Up()
		self.event()

class ScrollBarAe(Window):
	SCROLLBAR_WIDTH = 7
	SCROLL_BTN_XDIST = 0
	SCROLL_BTN_YDIST = 0
	class MiddleBar(DragButton):
		def __init__(self):
			DragButton.__init__(self)
			self.AddFlag("movable")
			self.SetWindowName("scrollbar_middlebar")
		def MakeImage(self):
			top = ExpandedImageBox()
			top.SetParent(self)
			top.LoadImage("d:/ymir work/ui/game/gameoption/scrollbar/scrollbar_top.tga")
			top.AddFlag("not_pick")
			top.Show()
			topScale = ExpandedImageBox()
			topScale.SetParent(self)
			topScale.SetPosition(0, top.GetHeight())
			topScale.LoadImage("d:/ymir work/ui/game/gameoption/scrollbar/scrollbar_scale.tga")
			topScale.AddFlag("not_pick")
			topScale.Show()
			bottom = ExpandedImageBox()
			bottom.SetParent(self)
			bottom.LoadImage("d:/ymir work/ui/game/gameoption/scrollbar/scrollbar_bottom.tga")
			bottom.AddFlag("not_pick")
			bottom.Show()
			bottomScale = ExpandedImageBox()
			bottomScale.SetParent(self)
			bottomScale.LoadImage("d:/ymir work/ui/game/gameoption/scrollbar/scrollbar_scale.tga")
			bottomScale.AddFlag("not_pick")
			bottomScale.Show()
			middle = ExpandedImageBox()
			middle.SetParent(self)
			middle.LoadImage("d:/ymir work/ui/game/gameoption/scrollbar/scrollbar_mid.tga")
			middle.AddFlag("not_pick")
			middle.Show()
			self.top = top
			self.topScale = topScale
			self.bottom = bottom
			self.bottomScale = bottomScale
			self.middle = middle
		def SetSize(self, height):
			minHeight = self.top.GetHeight() + self.bottom.GetHeight() + self.middle.GetHeight()
			height = max(minHeight, height)
			DragButton.SetSize(self, 10, height)
			scale = (height - minHeight) / 2 
			extraScale = 0
			if (height - minHeight) % 2 == 1:
				extraScale = 1
			self.topScale.SetRenderingRect(0, 0, 0, scale - 1)
			self.middle.SetPosition(0, self.top.GetHeight() + scale)
			self.bottomScale.SetPosition(0, self.middle.GetBottom())
			self.bottomScale.SetRenderingRect(0, 0, 0, scale - 1 + extraScale)
			self.bottom.SetPosition(0, height - self.bottom.GetHeight())
	def __init__(self):
		Window.__init__(self)
		self.pageSize = 1
		self.curPos = 0.0
		self.eventScroll = None
		self.eventArgs = None
		self.lockFlag = False
		self.CreateScrollBar()
		self.SetScrollBarSize(0)
		self.scrollStep = 0.4
		self.SetWindowName("NONAME_ScrollBar")

	def __del__(self):
		Window.__del__(self)
	def CreateScrollBar(self):
		topImage = ExpandedImageBox()
		topImage.SetParent(self)
		topImage.AddFlag("not_pick")
		topImage.LoadImage("d:/ymir work/ui/game/gameoption/scrollbar/scroll_top.tga")
		topImage.Show()
		bottomImage = ExpandedImageBox()
		bottomImage.SetParent(self)
		bottomImage.AddFlag("not_pick")
		bottomImage.LoadImage("d:/ymir work/ui/game/gameoption/scrollbar/scroll_bottom.tga")
		bottomImage.Show()
		middleImage = ExpandedImageBox()
		middleImage.SetParent(self)
		middleImage.AddFlag("not_pick")
		middleImage.SetPosition(0, topImage.GetHeight())
		middleImage.LoadImage("d:/ymir work/ui/game/gameoption/scrollbar/scroll_mid.tga")
		middleImage.Show()
		self.topImage = topImage
		self.bottomImage = bottomImage
		self.middleImage = middleImage
		middleBar = self.MiddleBar()
		middleBar.SetParent(self)
		middleBar.SetMoveEvent(__mem_func__(self.OnMove))
		middleBar.Show()
		middleBar.MakeImage()
		middleBar.SetSize(12)
		self.middleBar = middleBar
	def Destroy(self):
		self.eventScroll = None
		self.eventArgs = None
	def SetScrollEvent(self, event, *args):
		self.eventScroll = event
		self.eventArgs = args
	def SetMiddleBarSize(self, pageScale):
		self.middleBar.SetSize(int(pageScale * float(self.GetHeight() - (self.SCROLL_BTN_YDIST*2))))
		realHeight = self.GetHeight() - (self.SCROLL_BTN_YDIST*2) - self.middleBar.GetHeight()
		self.pageSize = realHeight

	def SetScrollBarSize(self, height):
		self.SetSize(self.SCROLLBAR_WIDTH, height)
		self.pageSize = height - self.SCROLL_BTN_YDIST*2 - self.middleBar.GetHeight()
		middleImageScale = float((height - self.SCROLL_BTN_YDIST*2) - self.middleImage.GetHeight()) / float(self.middleImage.GetHeight())
		self.middleImage.SetRenderingRect(0, 0, 0, middleImageScale)
		self.bottomImage.SetPosition(0, height - self.bottomImage.GetHeight())
		self.middleBar.SetRestrictMovementArea(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST, \
			self.middleBar.GetWidth(), height - self.SCROLL_BTN_YDIST * 2)
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST)
	def SetScrollStep(self, step):
		self.scrollStep = step
	def OnUp(self):
		self.SetPos(self.curPos-self.scrollStep)
	def OnDown(self):
		self.SetPos(self.curPos+self.scrollStep)
	def GetScrollStep(self):
		return self.scrollStep
	def GetPos(self):
		return self.curPos
	def OnUp(self):
		self.SetPos(self.curPos-self.scrollStep)
	def OnDown(self):
		self.SetPos(self.curPos+self.scrollStep)
	def SetPos(self, pos, moveEvent = True):
		pos = max(0.0, pos)
		pos = min(1.0, pos)
		newPos = float(self.pageSize) * pos
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, int(newPos) + self.SCROLL_BTN_YDIST)
		if moveEvent == True:
			self.OnMove()
	def OnMove(self):
		if self.lockFlag:
			return
		if 0 == self.pageSize:
			return
		(xLocal, yLocal) = self.middleBar.GetLocalPosition()
		self.curPos = float(yLocal - self.SCROLL_BTN_YDIST) / float(self.pageSize)
		if self.eventScroll:
			apply(self.eventScroll, self.eventArgs)
	def OnMouseLeftButtonDown(self):
		(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()
		newPos = float(yMouseLocalPosition) / float(self.GetHeight())
		self.SetPos(newPos)
	def LockScroll(self):
		self.lockFlag = True
	def UnlockScroll(self):
		self.lockFlag = False

class ScrollBar(Window):

	SCROLLBAR_WIDTH = 17
	SCROLLBAR_MIDDLE_HEIGHT = 9
	SCROLLBAR_BUTTON_WIDTH = 17
	SCROLLBAR_BUTTON_HEIGHT = 17
	MIDDLE_BAR_POS = 5
	MIDDLE_BAR_UPPER_PLACE = 3
	MIDDLE_BAR_DOWNER_PLACE = 4
	TEMP_SPACE = MIDDLE_BAR_UPPER_PLACE + MIDDLE_BAR_DOWNER_PLACE
	if app.__BL_SMOOTH_SCROLL__:
		SMOOTH_RATIO = 10.0

	class MiddleBar(DragButton):
		def __init__(self):
			DragButton.__init__(self)
			self.AddFlag("movable")

		def MakeImage(self):
			top = ImageBox()
			top.SetParent(self)
			top.LoadImage("d:/ymir work/ui/pattern/ScrollBar_Top.tga")
			top.SetPosition(0, 0)
			top.AddFlag("not_pick")
			top.Show()
			bottom = ImageBox()
			bottom.SetParent(self)
			bottom.LoadImage("d:/ymir work/ui/pattern/ScrollBar_Bottom.tga")
			bottom.AddFlag("not_pick")
			bottom.Show()

			middle = ExpandedImageBox()
			middle.SetParent(self)
			middle.LoadImage("d:/ymir work/ui/pattern/ScrollBar_Middle.tga")
			middle.SetPosition(0, 4)
			middle.AddFlag("not_pick")
			middle.Show()

			self.top = top
			self.bottom = bottom
			self.middle = middle

		def SetSize(self, height):
			height = max(12, height)
			DragButton.SetSize(self, 10, height)
			self.bottom.SetPosition(0, height-4)

			height -= 4*3
			self.middle.SetRenderingRect(0, 0, 0, float(height)/4.0)

	def __init__(self):
		Window.__init__(self)

		self.pageSize = 1
		self.curPos = 0.0
		self.eventScroll = lambda *arg: None
		self.lockFlag = False
		self.scrollStep = 0.20
		if app.__BL_SMOOTH_SCROLL__:
			self.smooth_mode = False
			self.actual_pos = 0.0
			self.target_pos = 0.0
		self.scroll_span = 0
		self.CreateScrollBar()

	def __del__(self):
		Window.__del__(self)

	def CreateScrollBar(self):
		barSlot = Bar3D()
		barSlot.SetParent(self)
		barSlot.AddFlag("not_pick")
		barSlot.Show()

		middleBar = self.MiddleBar()
		middleBar.SetParent(self)
		middleBar.SetMoveEvent(__mem_func__(self.OnMove))
		middleBar.Show()
		middleBar.MakeImage()
		middleBar.SetSize(12)

		upButton = Button()
		upButton.SetParent(self)
		upButton.SetEvent(__mem_func__(self.OnUp))
		upButton.SetUpVisual("d:/ymir work/ui/public/scrollbar_up_button_01.sub")
		upButton.SetOverVisual("d:/ymir work/ui/public/scrollbar_up_button_02.sub")
		upButton.SetDownVisual("d:/ymir work/ui/public/scrollbar_up_button_03.sub")
		upButton.Show()

		downButton = Button()
		downButton.SetParent(self)
		downButton.SetEvent(__mem_func__(self.OnDown))
		downButton.SetUpVisual("d:/ymir work/ui/public/scrollbar_down_button_01.sub")
		downButton.SetOverVisual("d:/ymir work/ui/public/scrollbar_down_button_02.sub")
		downButton.SetDownVisual("d:/ymir work/ui/public/scrollbar_down_button_03.sub")
		downButton.Show()

		self.upButton = upButton
		self.downButton = downButton
		self.middleBar = middleBar
		self.barSlot = barSlot

		self.SCROLLBAR_WIDTH = self.upButton.GetWidth()
		self.SCROLLBAR_MIDDLE_HEIGHT = self.middleBar.GetHeight()
		self.SCROLLBAR_BUTTON_WIDTH = self.upButton.GetWidth()
		self.SCROLLBAR_BUTTON_HEIGHT = self.upButton.GetHeight()

		if app.ENABLE_MOUSEWHEEL_EVENT:
			self.upButton.SetMouseWheelScrollEvent(self.OnMouseWheelScroll_ScrollBar)
			self.downButton.SetMouseWheelScrollEvent(self.OnMouseWheelScroll_ScrollBar)
			self.middleBar.SetMouseWheelScrollEvent(self.OnMouseWheelScroll_ScrollBar)
			self.barSlot.SetMouseWheelScrollEvent(self.OnMouseWheelScroll_ScrollBar)
			self.SetMouseWheelScrollEvent(self.OnMouseWheelScroll_ScrollBar)


	if app.ENABLE_MOUSEWHEEL_EVENT:
		def OnMouseWheelScroll_ScrollBar(self,mode):
			eventDct = { "UP" : lambda : self.SetPos(self.curPos - (self.scrollStep/4)) , "DOWN" : lambda: self.SetPos(self.curPos + (self.scrollStep/4)) }

			if mode in eventDct:
				eventDct[mode]()


	def Destroy(self):
		self.middleBar = None
		self.upButton = None
		self.downButton = None
		self.eventScroll = lambda *arg: None

	def SetScrollEvent(self, event):
		self.eventScroll = event

	def SetMiddleBarSize(self, pageScale):
		realHeight = self.GetHeight() - self.SCROLLBAR_BUTTON_HEIGHT*2
		self.SCROLLBAR_MIDDLE_HEIGHT = int(pageScale * float(realHeight))
		self.middleBar.SetSize(self.SCROLLBAR_MIDDLE_HEIGHT)
		self.pageSize = (self.GetHeight() - self.SCROLLBAR_BUTTON_HEIGHT*2) - self.SCROLLBAR_MIDDLE_HEIGHT - (self.TEMP_SPACE)

	def SetScrollBarSize(self, height):
		self.pageSize = (height - self.SCROLLBAR_BUTTON_HEIGHT*2) - self.SCROLLBAR_MIDDLE_HEIGHT - (self.TEMP_SPACE)
		self.SetSize(self.SCROLLBAR_WIDTH, height)
		self.upButton.SetPosition(0, 0)
		self.downButton.SetPosition(0, height - self.SCROLLBAR_BUTTON_HEIGHT)
		self.middleBar.SetRestrictMovementArea(self.MIDDLE_BAR_POS, self.SCROLLBAR_BUTTON_HEIGHT + self.MIDDLE_BAR_UPPER_PLACE, self.MIDDLE_BAR_POS+2, height - self.SCROLLBAR_BUTTON_HEIGHT*2 - self.TEMP_SPACE)
		self.middleBar.SetPosition(self.MIDDLE_BAR_POS, 0)

		self.UpdateBarSlot()

	def UpdateBarSlot(self):
		self.barSlot.SetPosition(0, self.SCROLLBAR_BUTTON_HEIGHT)
		self.barSlot.SetSize(self.GetWidth() - 2, self.GetHeight() - self.SCROLLBAR_BUTTON_HEIGHT*2 - 2)

	def GetPos(self):
		return self.curPos

	def SetPos(self, pos):
		pos = max(0.0, pos)
		pos = min(1.0, pos)

		newPos = float(self.pageSize) * pos
		self.middleBar.SetPosition(self.MIDDLE_BAR_POS, int(newPos) + self.SCROLLBAR_BUTTON_HEIGHT + self.MIDDLE_BAR_UPPER_PLACE)
		self.OnMove()

	def SetScrollStep(self, step):
		self.scrollStep = step

	def GetScrollStep(self):
		return self.scrollStep

	def SetSpan(self, span):
		self.scroll_span = span

	def IncreaseSpan(self, span):
		self.scroll_span += span
	
	def GetSpan(self):
		return self.scroll_span

	def OnUp(self):
		if app.__BL_SMOOTH_SCROLL__ and self.smooth_mode:
			self.actual_pos = min(1.0, max(0.0, self.curPos))
			self.target_pos = min(1.0, max(0.0, self.curPos-self.scrollStep))
		else:
			self.SetPos(self.curPos-self.scrollStep)

	def OnDown(self):
		if app.__BL_SMOOTH_SCROLL__ and self.smooth_mode:
			self.actual_pos = min(1.0, max(0.0, self.curPos))
			self.target_pos = min(1.0, max(0.0, self.curPos+self.scrollStep))
		else:
			self.SetPos(self.curPos+self.scrollStep)

	def OnMove(self):

		if self.lockFlag:
			return

		if 0 == self.pageSize:
			return

		(xLocal, yLocal) = self.middleBar.GetLocalPosition()
		self.curPos = float(yLocal - self.SCROLLBAR_BUTTON_HEIGHT - self.MIDDLE_BAR_UPPER_PLACE) / float(self.pageSize)

		self.eventScroll()

	def OnMouseLeftButtonDown(self):
		(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()
		pickedPos = yMouseLocalPosition - self.SCROLLBAR_BUTTON_HEIGHT - self.SCROLLBAR_MIDDLE_HEIGHT/2
		newPos = float(pickedPos) / float(self.pageSize)
		if app.__BL_SMOOTH_SCROLL__ and self.smooth_mode:
			self.actual_pos = min(1.0, max(0.0, self.curPos))
			self.target_pos = min(1.0, max(0.0, newPos))
		else:
			self.SetPos(newPos)

	def LockScroll(self):
		self.lockFlag = True

	def UnlockScroll(self):
		self.lockFlag = False

	if app.__BL_SMOOTH_SCROLL__:
		def EnableSmoothMode(self):
			self.smooth_mode = True

		def OnUpdate(self):
			if not self.smooth_mode:
				return
			
			if self.lockFlag:
				return
			
			if 0 == self.pageSize:
				return
			
			if self.actual_pos == self.target_pos:
				return

			distance = abs(self.actual_pos - self.target_pos)
			smooth_step = max(distance / ScrollBar.SMOOTH_RATIO, 0.005)
		
			if self.actual_pos < self.target_pos:
				self.actual_pos = min(self.actual_pos + smooth_step, self.target_pos)
			elif self.actual_pos > self.target_pos:
				self.actual_pos = max(self.actual_pos - smooth_step, self.target_pos)
			
			self.SetPos(self.actual_pos)

	def OnMouseWheel(self, nLen):
		if nLen > 0:
			self.OnUp()
			return True
		elif nLen < 0:
			self.OnDown()
			return True
		return False
		

class ScrollBarZaris(Window):
	MIDDLE_BAR_POS = 0
	MIDDLE_BAR_UPPER_PLACE = 6
	MIDDLE_BAR_DOWNER_PLACE = 5
	TEMP_SPACE = MIDDLE_BAR_UPPER_PLACE + MIDDLE_BAR_DOWNER_PLACE

	class MiddleBar(DragButton):
		def __init__(self):
			DragButton.__init__(self)
			self.AddFlag("movable")
			DragButton.SetSize(self, 20, 22)

		def MakeImage(self):
			middle = ExpandedImageBox()
			middle.SetParent(self)
			middle.LoadImage("zaris/whisper/scroll_middle.png")
			middle.SetPosition(1, -5)
			middle.AddFlag("not_pick")
			middle.Show()
			self.middle = middle

		def SetSize(self, height):
			DragButton.SetSize(self, 20, 22)

	def __init__(self):
		Window.__init__(self)

		self.pageSize = 1
		self.curPos = 0.0
		self.eventScroll = lambda *arg: None
		self.lockFlag = False
		self.scrollStep = 0.20

		self.CreateScrollBar()
		self.SetScrollBarSize(217)

	def __del__(self):
		Window.__del__(self)

	def CreateScrollBar(self):
		barSlot = ExpandedImageBox()
		barSlot.SetParent(self)
		barSlot.LoadImage("zaris/whisper/scroll_base.png")
		barSlot.SetPosition(0, 0)
		barSlot.AddFlag("not_pick")
		barSlot.Show()

		middleBar = self.MiddleBar()
		middleBar.SetParent(self)
		middleBar.SetMoveEvent(__mem_func__(self.OnMove))
		middleBar.Show()
		middleBar.MakeImage()

		self.middleBar = middleBar
		self.barSlot = barSlot

		self.SCROLLBAR_WIDTH = self.middleBar.GetWidth()
		self.SCROLLBAR_MIDDLE_HEIGHT = self.middleBar.GetHeight()

	def Destroy(self):
		self.middleBar = None
		self.eventScroll = lambda *arg: None

	def SetScrollEvent(self, event):
		self.eventScroll = event

	def SetMiddleBarSize(self, pageScale):
		realHeight = self.GetHeight()
		self.SCROLLBAR_MIDDLE_HEIGHT = int(pageScale * float(realHeight))
		self.middleBar.SetSize(self.SCROLLBAR_MIDDLE_HEIGHT)
		self.pageSize = self.GetHeight() - self.SCROLLBAR_MIDDLE_HEIGHT - (self.TEMP_SPACE)

	def SetScrollBarSize(self, height):
		height = 217
		self.pageSize = height - self.SCROLLBAR_MIDDLE_HEIGHT - (self.TEMP_SPACE)
		self.SetSize(self.SCROLLBAR_WIDTH, height)
		self.middleBar.SetRestrictMovementArea(self.MIDDLE_BAR_POS, self.MIDDLE_BAR_UPPER_PLACE, self.MIDDLE_BAR_POS+2, height - self.TEMP_SPACE)
		self.middleBar.SetPosition(self.MIDDLE_BAR_POS, 0)

		self.UpdateBarSlot()

	def UpdateBarSlot(self):
		self.barSlot.SetPosition(0, 0)
		self.barSlot.SetSize(self.GetWidth() - 2, self.GetHeight() - 2)

	def GetPos(self):
		return self.curPos

	def SetPos(self, pos):
		pos = max(0.0, pos)
		pos = min(1.0, pos)

		newPos = float(self.pageSize) * pos
		self.middleBar.SetPosition(self.MIDDLE_BAR_POS, int(newPos) + self.MIDDLE_BAR_UPPER_PLACE)
		self.OnMove()

	def SetScrollStep(self, step):
		self.scrollStep = step

	def GetScrollStep(self):
		return self.scrollStep

	def OnUp(self):
		self.SetPos(self.curPos-self.scrollStep)

	def OnDown(self):
		self.SetPos(self.curPos+self.scrollStep)

	def OnMove(self):
		if self.lockFlag:
			return

		if 0 == self.pageSize:
			return

		(xLocal, yLocal) = self.middleBar.GetLocalPosition()
		self.curPos = float(yLocal - self.MIDDLE_BAR_UPPER_PLACE) / float(self.pageSize)
		self.eventScroll()

	def OnMouseLeftButtonDown(self):
		(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()
		pickedPos = yMouseLocalPosition - self.SCROLLBAR_MIDDLE_HEIGHT/2
		newPos = float(pickedPos) / float(self.pageSize)
		self.SetPos(newPos)

	def LockScroll(self):
		self.lockFlag = True

	def UnlockScroll(self):
		self.lockFlag = False

	def OnMouseWheel(self, nLen):
		if nLen > 0:
			self.OnUp()
			return True
		elif nLen < 0:
			self.OnDown()
			return True
		return False

class ThinScrollBar(ScrollBar):

	def CreateScrollBar(self):
		middleBar = self.MiddleBar()
		middleBar.SetParent(self)
		middleBar.SetMoveEvent(__mem_func__(self.OnMove))
		middleBar.Show()
		middleBar.SetUpVisual("d:/ymir work/ui/public/scrollbar_thin_middle_button_01.sub")
		middleBar.SetOverVisual("d:/ymir work/ui/public/scrollbar_thin_middle_button_02.sub")
		middleBar.SetDownVisual("d:/ymir work/ui/public/scrollbar_thin_middle_button_03.sub")

		upButton = Button()
		upButton.SetParent(self)
		upButton.SetUpVisual("d:/ymir work/ui/public/scrollbar_thin_up_button_01.sub")
		upButton.SetOverVisual("d:/ymir work/ui/public/scrollbar_thin_up_button_02.sub")
		upButton.SetDownVisual("d:/ymir work/ui/public/scrollbar_thin_up_button_03.sub")
		upButton.SetEvent(__mem_func__(self.OnUp))
		upButton.Show()

		downButton = Button()
		downButton.SetParent(self)
		downButton.SetUpVisual("d:/ymir work/ui/public/scrollbar_thin_down_button_01.sub")
		downButton.SetOverVisual("d:/ymir work/ui/public/scrollbar_thin_down_button_02.sub")
		downButton.SetDownVisual("d:/ymir work/ui/public/scrollbar_thin_down_button_03.sub")
		downButton.SetEvent(__mem_func__(self.OnDown))
		downButton.Show()

		self.middleBar = middleBar
		self.upButton = upButton
		self.downButton = downButton

		self.SCROLLBAR_WIDTH = self.upButton.GetWidth()
		self.SCROLLBAR_MIDDLE_HEIGHT = self.middleBar.GetHeight()
		self.SCROLLBAR_BUTTON_WIDTH = self.upButton.GetWidth()
		self.SCROLLBAR_BUTTON_HEIGHT = self.upButton.GetHeight()
		self.MIDDLE_BAR_POS = 0
		self.MIDDLE_BAR_UPPER_PLACE = 0
		self.MIDDLE_BAR_DOWNER_PLACE = 0
		self.TEMP_SPACE = 0

	def UpdateBarSlot(self):
		pass

class SmallThinScrollBar(ScrollBar):

	def CreateScrollBar(self):
		middleBar = self.MiddleBar()
		middleBar.SetParent(self)
		middleBar.SetMoveEvent(__mem_func__(self.OnMove))
		middleBar.Show()
		middleBar.SetUpVisual("d:/ymir work/ui/public/scrollbar_small_thin_middle_button_01.sub")
		middleBar.SetOverVisual("d:/ymir work/ui/public/scrollbar_small_thin_middle_button_01.sub")
		middleBar.SetDownVisual("d:/ymir work/ui/public/scrollbar_small_thin_middle_button_01.sub")

		upButton = Button()
		upButton.SetParent(self)
		upButton.SetUpVisual("d:/ymir work/ui/public/scrollbar_small_thin_up_button_01.sub")
		upButton.SetOverVisual("d:/ymir work/ui/public/scrollbar_small_thin_up_button_02.sub")
		upButton.SetDownVisual("d:/ymir work/ui/public/scrollbar_small_thin_up_button_03.sub")
		upButton.SetEvent(__mem_func__(self.OnUp))
		upButton.Show()

		downButton = Button()
		downButton.SetParent(self)
		downButton.SetUpVisual("d:/ymir work/ui/public/scrollbar_small_thin_down_button_01.sub")
		downButton.SetOverVisual("d:/ymir work/ui/public/scrollbar_small_thin_down_button_02.sub")
		downButton.SetDownVisual("d:/ymir work/ui/public/scrollbar_small_thin_down_button_03.sub")
		downButton.SetEvent(__mem_func__(self.OnDown))
		downButton.Show()

		self.middleBar = middleBar
		self.upButton = upButton
		self.downButton = downButton

		self.SCROLLBAR_WIDTH = self.upButton.GetWidth()
		self.SCROLLBAR_MIDDLE_HEIGHT = self.middleBar.GetHeight()
		self.SCROLLBAR_BUTTON_WIDTH = self.upButton.GetWidth()
		self.SCROLLBAR_BUTTON_HEIGHT = self.upButton.GetHeight()
		self.MIDDLE_BAR_POS = 0
		self.MIDDLE_BAR_UPPER_PLACE = 0
		self.MIDDLE_BAR_DOWNER_PLACE = 0
		self.TEMP_SPACE = 0


		if app.ENABLE_MOUSEWHEEL_EVENT:
			self.middleBar.SetMouseWheelScrollEvent(self.OnMouseWheelScroll_ScrollBar)
			self.upButton.SetMouseWheelScrollEvent(self.OnMouseWheelScroll_ScrollBar)
			self.downButton.SetMouseWheelScrollEvent(self.OnMouseWheelScroll_ScrollBar)
			self.SetMouseWheelScrollEvent(self.OnMouseWheelScroll_ScrollBar)

	def UpdateBarSlot(self):
		pass

class SliderBar(Window):

	def __init__(self):
		Window.__init__(self)

		self.curPos = 1.0
		self.pageSize = 1.0
		self.eventChange = None

		self.__CreateBackGroundImage()
		self.__CreateCursor()

	def __del__(self):
		Window.__del__(self)

	def __CreateBackGroundImage(self):
		img = ImageBox()
		img.SetParent(self)
		img.LoadImage("d:/ymir work/ui/game/windows/sliderbar.sub")
		img.Show()
		self.backGroundImage = img

		self.SetSize(self.backGroundImage.GetWidth(), self.backGroundImage.GetHeight())

	def __CreateCursor(self):
		cursor = DragButton()
		cursor.AddFlag("movable")
		cursor.AddFlag("restrict_y")
		cursor.SetParent(self)
		cursor.SetMoveEvent(__mem_func__(self.__OnMove))
		cursor.SetUpVisual("d:/ymir work/ui/game/windows/sliderbar_cursor.sub")
		cursor.SetOverVisual("d:/ymir work/ui/game/windows/sliderbar_cursor.sub")
		cursor.SetDownVisual("d:/ymir work/ui/game/windows/sliderbar_cursor.sub")
		cursor.Show()
		self.cursor = cursor

		self.cursor.SetRestrictMovementArea(0, 0, self.backGroundImage.GetWidth(), 0)
		self.pageSize = self.backGroundImage.GetWidth() - self.cursor.GetWidth()

	def __OnMove(self):
		(xLocal, yLocal) = self.cursor.GetLocalPosition()
		self.curPos = float(xLocal) / float(self.pageSize)

		if self.eventChange:
			self.eventChange()

	def SetSliderPos(self, pos):
		self.curPos = pos
		self.cursor.SetPosition(int(self.pageSize * pos), 0)

	def GetSliderPos(self):
		return self.curPos

	def SetEvent(self, event):
		self.eventChange = event

	def Enable(self):
		self.cursor.Show()

	def Disable(self):
		self.cursor.Hide()

	if app.ENABLE_FOV_OPTION:
		def SetBackgroundVisual(self, filename):
			if self.backGroundImage:
				self.backGroundImage.LoadImage(filename)

				self.SetSize(self.backGroundImage.GetWidth(), self.backGroundImage.GetHeight())
				self.pageSize = self.backGroundImage.GetWidth() - self.cursor.GetWidth()

			if self.cursor:
				self.cursor.SetRestrictMovementArea(0, 0, self.backGroundImage.GetWidth(), 0)

		def SetButtonVisual(self, path, up, over, down):
			if self.cursor:
				self.cursor.SetUpVisual(path + up)
				self.cursor.SetOverVisual(path + over)
				self.cursor.SetDownVisual(path + down)

class SliderBarNew(Window):

	def __init__(self):
		Window.__init__(self)

		self.curPos = 1.0
		self.pageSize = 1.0
		self.eventChange = None

		self.__CreateBackGroundImage()
		self.__CreateCursor()

	def __del__(self):
		Window.__del__(self)

	def __CreateBackGroundImage(self):
		img = ImageBox()
		img.SetParent(self)
		img.LoadImage("d:/ymir work/ui/settings/scroll_slot.png")
		img.Show()
		self.backGroundImage = img

		self.SetSize(self.backGroundImage.GetWidth(), self.backGroundImage.GetHeight())

	def __CreateCursor(self):
		cursor = DragButton()
		cursor.AddFlag("movable")
		cursor.AddFlag("restrict_y")
		cursor.SetParent(self)
		cursor.SetMoveEvent(__mem_func__(self.__OnMove))
		cursor.SetUpVisual("d:/ymir work/ui/itemshop/scroll_button.png")
		cursor.SetOverVisual("d:/ymir work/ui/itemshop/scroll_button.png")
		cursor.SetDownVisual("d:/ymir work/ui/itemshop/scroll_button.png")
		cursor.Show()
		self.cursor = cursor

		self.cursor.SetRestrictMovementArea(0, 0, self.backGroundImage.GetWidth(), 0)
		self.pageSize = self.backGroundImage.GetWidth() - self.cursor.GetWidth()

	def __OnMove(self):
		(xLocal, yLocal) = self.cursor.GetLocalPosition()
		self.curPos = float(xLocal) / float(self.pageSize)

		if self.eventChange:
			self.eventChange()

	def SetSliderPos(self, pos):
		self.curPos = pos
		self.cursor.SetPosition(int(self.pageSize * pos), 0)

	def GetSliderPos(self):
		return self.curPos

	def SetEvent(self, event):
		self.eventChange = event

	def Enable(self):
		self.cursor.Show()

	def Disable(self):
		self.cursor.Hide()

	if app.ENABLE_FOV_OPTION:
		def SetBackgroundVisual(self, filename):
			if self.backGroundImage:
				self.backGroundImage.LoadImage(filename)

				self.SetSize(self.backGroundImage.GetWidth(), self.backGroundImage.GetHeight())
				self.pageSize = self.backGroundImage.GetWidth() - self.cursor.GetWidth()

			if self.cursor:
				self.cursor.SetRestrictMovementArea(0, 0, self.backGroundImage.GetWidth(), 0)

		def SetButtonVisual(self, path, up, over, down):
			if self.cursor:
				self.cursor.SetUpVisual(path + up)
				self.cursor.SetOverVisual(path + over)
				self.cursor.SetDownVisual(path + down)

class ListBox(Window):

	TEMPORARY_PLACE = 3

	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)
		self.overLine = -1
		self.selectedLine = -1
		self.width = 0
		self.height = 0
		self.stepSize = 17
		self.basePos = 0
		self.showLineCount = 0
		self.itemCenterAlign = True
		self.itemList = []
		self.keyDict = {}
		self.textDict = {}
		self.event = lambda *arg: None
	def __del__(self):
		Window.__del__(self)

	def SetWidth(self, width):
		self.SetSize(width, self.height)

	def SetSize(self, width, height):
		Window.SetSize(self, width, height)
		self.width = width
		self.height = height

	def SetTextCenterAlign(self, flag):
		self.itemCenterAlign = flag

	def SetBasePos(self, pos):
		self.basePos = pos
		self._LocateItem()

	def ClearItem(self):
		self.keyDict = {}
		self.textDict = {}
		self.itemList = []
		self.overLine = -1
		self.selectedLine = -1

	def InsertItem(self, number, text):
		self.keyDict[len(self.itemList)] = number
		self.textDict[len(self.itemList)] = text

		textLine = TextLine()
		textLine.SetParent(self)
		textLine.SetText(text)
		textLine.Show()

		if self.itemCenterAlign:
			textLine.SetWindowHorizontalAlignCenter()
			textLine.SetHorizontalAlignCenter()

		self.itemList.append(textLine)

		self._LocateItem()

	def ChangeItem(self, number, text):
		for key, value in self.keyDict.items():
			if value == number:
				self.textDict[key] = text

				if number < len(self.itemList):
					self.itemList[key].SetText(text)

				return

	def LocateItem(self):
		self._LocateItem()

	def _LocateItem(self):

		skipCount = self.basePos
		yPos = 0
		self.showLineCount = 0

		for textLine in self.itemList:
			textLine.Hide()

			if skipCount > 0:
				skipCount -= 1
				continue

			if localeInfo.IsARABIC():
				w, h = textLine.GetTextSize()
				textLine.SetPosition(w+10, yPos + 3)
			else:
				textLine.SetPosition(0, yPos + 3)

			yPos += self.stepSize

			if yPos <= self.GetHeight():
				self.showLineCount += 1
				textLine.Show()

	def ArrangeItem(self):
		self.SetSize(self.width, len(self.itemList) * self.stepSize)
		self._LocateItem()

	def GetViewItemCount(self):
		return int(self.GetHeight() / self.stepSize)

	def GetItemCount(self):
		return len(self.itemList)

	def SetEvent(self, event):
		self.event = event

	def GetItemIndex(self, cmpItem):
		for i, item in enumerate(self.itemList):
			if item.GetText() == cmpItem:
				return i

	def SelectItem(self, line):

		if not self.keyDict.has_key(line):
			return

		if line == self.selectedLine:
			return

		self.selectedLine = line
		self.event(self.keyDict.get(line, 0), self.textDict.get(line, "None"))

	def GetSelectedItem(self):
		return self.keyDict.get(self.selectedLine, 0)

	def OnMouseLeftButtonDown(self):
		if self.overLine < 0:
			return

	def OnMouseLeftButtonUp(self):
		if self.overLine >= 0:
			self.SelectItem(self.overLine+self.basePos)

	def OnUpdate(self):

		self.overLine = -1

		if self.IsIn():
			x, y = self.GetGlobalPosition()
			height = self.GetHeight()
			xMouse, yMouse = wndMgr.GetMousePosition()

			if yMouse - y < height - 1:
				self.overLine = (yMouse - y) / self.stepSize

				if self.overLine < 0:
					self.overLine = -1
				if self.overLine >= len(self.itemList):
					self.overLine = -1

	def OnRender(self):
		xRender, yRender = self.GetGlobalPosition()
		yRender -= self.TEMPORARY_PLACE
		widthRender = self.width
		heightRender = self.height + self.TEMPORARY_PLACE*2

		if localeInfo.IsCIBN10:
			if -1 != self.overLine and self.keyDict[self.overLine] != -1:
				grp.SetColor(HALF_WHITE_COLOR)
				grp.RenderBar(xRender + 2, yRender + self.overLine*self.stepSize + 4, self.width - 3, self.stepSize)

			if -1 != self.selectedLine and self.keyDict[self.selectedLine] != -1:
				if self.selectedLine >= self.basePos:
					if self.selectedLine - self.basePos < self.showLineCount:
						grp.SetColor(SELECT_COLOR)
						grp.RenderBar(xRender + 2, yRender + (self.selectedLine-self.basePos)*self.stepSize + 4, self.width - 3, self.stepSize)

		else:
			if -1 != self.overLine:
				grp.SetColor(HALF_WHITE_COLOR)
				grp.RenderBar(xRender + 2, yRender + self.overLine*self.stepSize + 4, self.width - 3, self.stepSize)

			if -1 != self.selectedLine:
				if self.selectedLine >= self.basePos:
					if self.selectedLine - self.basePos < self.showLineCount:
						grp.SetColor(SELECT_COLOR)
						grp.RenderBar(xRender + 2, yRender + (self.selectedLine-self.basePos)*self.stepSize + 4, self.width - 3, self.stepSize)

class DropdownTree(Window):
	class Item(Window):
		def __init__(self):
			Window.__init__(self)
			self.id = -1
			self.parentId = -1
			self.offset = 0
			self.visible = False
			self.expanded = False
			self.event = None
			self.onCollapseEvent = None
			self.onExpandEvent = None

		def __del__(self):
			Window.__del__(self)

		def Destroy(self):
			self.id = 0
			self.parentId = 0
			self.offset = 0
			self.visible = 0
			self.expanded = 0
			self.event = 0
			self.onCollapseEvent = 0
			self.onExpandEvent = 0
			self.parent = 0

		def SetParent(self, parent):
			Window.SetParent(self, parent)
			self.parent=proxy(parent)

		def SetSize(self, width, height):
			Window.SetSize(self, width, height)

		def GetId(self):
			return self.id

		def SetId(self, id):
			self.id = id

		def GetParentId(self):
			return self.parentId

		def SetParentId(self, parentId):
			self.parentId = parentId
			
		def IsParent(self):
			return self.parentId == -1

		def SetVisible(self, visible):
			self.visible = visible
			
		def IsVisible(self):
			return self.visible
			
		def IsExpanded(self):
			return self.expanded
			
		def Expand(self):
			self.expanded = True
			if self.onExpandEvent:
				self.onExpandEvent()
			
		def Collapse(self):
			self.expanded = False
			if self.onCollapseEvent:
				self.onCollapseEvent()

		def SetOnExpandEvent(self, event):
			self.onExpandEvent = __mem_func__(event)

		def SetOnCollapseEvent(self, event):
			self.onCollapseEvent = __mem_func__(event)

		def SetOffset(self, offset):
			self.offset = offset

		def GetOffset(self):
			return self.offset

		def SetEvent(self, event):
			self.event = event

		def OnSelect(self):
			self.parent.SelectItem(self)

		def OnMouseLeftButtonDown(self):
			self.OnSelect()

	def __init__(self):
		Window.__init__(self)

		self.__curItemId=0
		self.viewItemCount=10
		self.basePos=0
		self.itemHeight=29
		self.isShopSearch=0
		self.itemStep=29
		self.selItem=0
		self.itemList=[]
		self.onSelectItemEvent = lambda *arg: None
		self.itemWidth=185

		self.scrollBar=None
		self.__UpdateSize()
	
	def __del__(self):
		Window.__del__(self)

	def __UpdateSize(self):
		height=self.itemStep*self.__GetViewItemCount()

		#self.SetSize(self.itemWidth, height)
		#self.SetSize(self.itemWidth, 375)

	def IsEmpty(self):
		if len(self.itemList)==0:
			return 1
		return 0

	#def OnMouseWheel(self, nLen):
	#	if self.scrollBar:
	#		self.scrollBar.OnMouseWheel(nLen)
	
	def OnMouseWheel(self, nLen):
		if self.scrollBar:
			if nLen > 0:
				self.scrollBar.OnUp()
			else:
				self.scrollBar.OnDown()
			return True
		return False

	def SetItemStep(self, itemStep):
		self.itemStep=itemStep
		self.__UpdateSize()

	def SetItemSize(self, itemWidth, itemHeight):
		self.itemWidth=itemWidth
		self.itemHeight=itemHeight
		self.__UpdateSize()
	
	def SetViewItemCount(self, viewItemCount):
		self.viewItemCount=viewItemCount
	
	def SetSelectEvent(self, event):
		self.onSelectItemEvent = event

	def SetBasePos(self, basePos):
		for oldItem in self.itemList:
			oldItem.Hide()

		self.basePos=basePos

		skipCount = basePos
		pos = basePos
		for lItem in self.itemList:
			if not lItem.IsVisible():
				continue
			
			if skipCount > 0:
				skipCount -= 1
				continue

			if pos >= (self.basePos+self.viewItemCount):
				break

			(x, y) = self.GetItemViewCoord(pos, lItem.GetWidth())
			lItem.SetPosition(x+lItem.GetOffset(), y)
			lItem.Show()
			pos+=1
		self.UpdateScrollbar()

	def GetItemIndex(self, argItem):
		return self.itemList.index(argItem)

	def GetSelectedItem(self):
		return self.selItem

	def SelectIndex(self, index):
		if index >= len(self.itemList) or index < 0:
			self.selItem = None
			return
		try:
			self.selItem=self.itemList[index]
		except:
			pass

	def ClearItem(self):
		self.selItem=None
		for lItem in self.itemList:
			lItem.Hide()
			lItem.Destroy()
			lItem = 0
		self.itemList=[]
		self.__curItemId = 0

		if self.scrollBar:
			self.scrollBar.SetPos(0)
		self.SetBasePos(0)

	def SelectItem(self, selItem):
		if self.isShopSearch:
			for item in self.itemList:
				if selItem != item:
					self.CloseTree(item, self.itemList)

		self.selItem = selItem
		if selItem.IsExpanded():
			self.CloseTree(selItem, self.itemList)
		else:
			if selItem.event:
				selItem.event()
			self.OpenTree(selItem, self.itemList)
		self.SetBasePos(self.basePos)
		

	def __AppendItem(self, newItem, parentId):
		curItemId = self.__curItemId
		self.__curItemId += 1
		
		newItem.SetParent(self)
		newItem.SetParentId(parentId)
		newItem.SetSize(self.itemWidth, self.itemHeight)
		newItem.SetId(curItemId)

		pos = self.__GetItemCount()
		self.itemList.append(newItem)

		if newItem.IsVisible() and self.__IsInViewRange(pos):
			(x, y)=self.GetItemViewCoord(pos, newItem.GetWidth())
			newItem.SetPosition(x, y)
			newItem.Show()
		else:
			newItem.Hide()

		self.UpdateScrollbar()

		return curItemId

	def AppendItemList(self, dict):
		self.__AppendItemList(-1, dict)
	
	def __AppendItemList(self, parentId, dict):
		for lItem in dict:
			if 'item' in lItem:
				id = self.__AppendItem(lItem['item'], parentId)
				if 'children' in lItem:
					self.__AppendItemList(id, lItem['children'])
				
	def SetScrollBar(self, scrollBar):
		scrollBar.SetScrollEvent(__mem_func__(self.__OnScroll))
		self.scrollBar=scrollBar

	def __OnScroll(self):
		self.SetBasePos(int(self.scrollBar.GetPos()*self.__GetScrollLen()))

	def __GetScrollLen(self):
		scrollLen=self.__GetItemCount()-self.__GetViewItemCount()
		if scrollLen<0:
			return 0

		return scrollLen

	def __GetViewItemCount(self):
		return self.viewItemCount

	def __GetItemCount(self):
		return sum(1 for lItem in self.itemList if lItem.IsVisible())

	def GetItemViewCoord(self, pos, itemWidth):
		return (0, (pos-self.basePos)*self.itemStep)

	def __IsInViewRange(self, pos):
		if pos<self.basePos:
			return 0
		if pos>=self.basePos+self.viewItemCount:
			return 0
		return 1
	
	def UpdateScrollbar(self):
		if self.__GetViewItemCount() < self.__GetItemCount():
			self.scrollBar.SetMiddleBarSize(float(self.__GetViewItemCount())/self.__GetItemCount())
			self.scrollBar.Show()
		else:
			self.scrollBar.Hide()

	def CloseTree(self, curItem, list):
		curItem.Collapse()
		for listboxItem in list:
			if listboxItem.GetParentId() == curItem.GetId():
				listboxItem.SetVisible(False)
				self.CloseTree(listboxItem, list)
		
	def OpenTree(self, curItem, list):
		curItem.Expand()
		for listboxItem in list:
			if listboxItem.GetParentId() == curItem.GetId():
				listboxItem.SetVisible(True)
				
class LastListItem(DropdownTree.Item):
	def __init__(self, text):
		DropdownTree.Item.__init__(self)
		self.overLine = False

		textLine=TextLine()
		textLine.SetParent(self)
		textLine.SetFontName(localeInfo.UI_DEF_FONT)
		textLine.SetWindowHorizontalAlignLeft()
		textLine.SetPosition(5,5)
		textLine.SetText(text)
		textLine.Show()

		self.textLine = textLine
		self.text = text

	def __del__(self):
		DropdownTree.Item.__del__(self)

	def Destroy(self):
		DropdownTree.Item.Destroy(self)
		self.overLine = 0
		self.textLine = 0
		self.text = 0

	def GetText(self):
		return self.text

	def SetSize(self, width, height):
		DropdownTree.Item.SetSize(self, width-self.GetOffset(), height)

	def OnMouseOverIn(self):
		self.overLine = True

	def OnMouseOverOut(self):
		self.overLine = False

	def OnRender(self):
		if self.overLine and self.parent.GetSelectedItem()!=self:
			x, y = self.GetGlobalPosition()
			grp.SetColor(grp.GenerateColor(1.0, 1.0, 1.0, 0.2))
			grp.RenderBar(x, y, self.GetWidth(), self.GetHeight())
		elif self.parent.GetSelectedItem()==self:
			x, y = self.GetGlobalPosition()
			grp.SetColor(grp.GenerateColor(0.0, 0.0, 1.0, 1.0))
			grp.RenderBar(x, y, self.GetWidth(), self.GetHeight())
		else:
			x, y = self.GetGlobalPosition()
			grp.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 1.0))
			grp.RenderBar(x, y, self.GetWidth(), self.GetHeight())

class ListBox2(ListBox):
	def __init__(self, *args, **kwargs):
		ListBox.__init__(self, *args, **kwargs)
		self.rowCount = 10
		self.barWidth = 0
		self.colCount = 0

	def SetRowCount(self, rowCount):
		self.rowCount = rowCount

	def SetSize(self, width, height):
		ListBox.SetSize(self, width, height)
		self._RefreshForm()

	def ClearItem(self):
		ListBox.ClearItem(self)
		self._RefreshForm()

	def InsertItem(self, *args, **kwargs):
		ListBox.InsertItem(self, *args, **kwargs)
		self._RefreshForm()

	def OnUpdate(self):
		mpos = wndMgr.GetMousePosition()
		self.overLine = self._CalcPointIndex(mpos)

	def OnRender(self):
		x, y = self.GetGlobalPosition()
		pos = (x + 2, y)

		if -1 != self.overLine:
			grp.SetColor(HALF_WHITE_COLOR)
			self._RenderBar(pos, self.overLine)

		if -1 != self.selectedLine:
			if self.selectedLine >= self.basePos:
				if self.selectedLine - self.basePos < self.showLineCount:
					grp.SetColor(SELECT_COLOR)
					self._RenderBar(pos, self.selectedLine-self.basePos)



	def _CalcPointIndex(self, mpos):
		if self.IsIn():
			px, py = mpos
			gx, gy = self.GetGlobalPosition()
			lx, ly = px - gx, py - gy

			col = lx / self.barWidth
			row = ly / self.stepSize
			idx = col * self.rowCount + row
			if col >= 0 and col < self.colCount:
				if row >= 0 and row < self.rowCount:
					if idx >= 0 and idx < len(self.itemList):
						return idx

		return -1

	def _CalcRenderPos(self, pos, idx):
		x, y = pos
		row = idx % self.rowCount
		col = idx / self.rowCount
		return (x + col * self.barWidth, y + row * self.stepSize)

	def _RenderBar(self, basePos, idx):
		x, y = self._CalcRenderPos(basePos, idx)
		grp.RenderBar(x, y, self.barWidth - 3, self.stepSize)

	def _LocateItem(self):
		pos = (0, self.TEMPORARY_PLACE)

		self.showLineCount = 0
		for textLine in self.itemList:
			x, y = self._CalcRenderPos(pos, self.showLineCount)
			textLine.SetPosition(x, y)
			textLine.Show()

			self.showLineCount += 1

	def _RefreshForm(self):
		if len(self.itemList) % self.rowCount:
			self.colCount = len(self.itemList) / self.rowCount + 1
		else:
			self.colCount = len(self.itemList) / self.rowCount

		if self.colCount:
			self.barWidth = self.width / self.colCount
		else:
			self.barWidth = self.width


class ExpandedButton(ExpandedImageBox):
	def __init__(self):
		ExpandedImageBox.__init__(self)

		self.images = {"UP": "", "OVER": "", "DOWN": ""}
		self.state = "NORMAL"

		self.xScale = 1.0
		self.yScale = 1.0

		self.isDown = False

		self.ButtonText = None

		self.eventDict = {}
		self.argsDict = {}

	def __del__(self):
		ExpandedImageBox.__del__(self)

		self.eventDict = {}
		self.argsDict = {}

	def SetScale(self, xScale, yScale):
		self.xScale = float(xScale)
		self.yScale = float(yScale)
		ExpandedImageBox.SetScale(self, xScale, yScale)

	def LoadImage(self, imgPath):
		ExpandedImageBox.LoadImage(self, imgPath)
		ExpandedImageBox.SetScale(self, self.xScale, self.yScale)

	def SetUpVisual(self, filename):
		self.images["UP"] = filename
		if self.state == "NORMAL":
			self.LoadImage(filename)

	def SetOverVisual(self, filename):
		self.images["OVER"] = filename
		if self.state == "OVER":
			self.LoadImage(filename)

	def SetDownVisual(self, filename):
		self.images["DOWN"] = filename
		if self.state == "DOWN":
			self.LoadImage(filename)

	def GetUpVisualFileName(self):
		return self.images["UP"]

	def GetOverVisualFileName(self):
		return self.images["OVER"]

	def GetDownVisualFileName(self):
		return self.images["DOWN"]

	def SetTextColor(self, color):
		if not self.ButtonText:
			return
		self.ButtonText.SetPackedFontColor(color)

	def SetRGBTextColor(self, r, g, b):
		if not self.ButtonText:
			return
		self.ButtonText.SetFontColor(r, g, b)

	def SetTextPosition(self, x, y):
		if self.ButtonText:
			self.ButtonText.SetHorizontalAlignLeft()
			self.ButtonText.SetPosition(x, y)
			
	if app.ENABLE_DYNAMIC_FONTS:
		def SetDynamicFont(self, flag):
			if self.ButtonText:
				self.ButtonText.SetDynamicFont(flag)
			else:
				# Create ButtonText if it doesn't exist
				self.SetText("")
				if self.ButtonText:
					self.ButtonText.SetDynamicFont(flag)
				
		def SetFontName(self, font):
			if self.ButtonText:
				self.ButtonText.SetFontName(font)
			else:
				# Create ButtonText if it doesn't exist
				self.SetText("")
				if self.ButtonText:
					self.ButtonText.SetFontName(font)

	def SetText(self, text, height=4):
		if not self.ButtonText:
			textLine = TextLine()
			textLine.SetParent(self)
			textLine.SetPosition(self.GetWidth() / 2, self.GetHeight() / 2)
			textLine.SetVerticalAlignCenter()
			textLine.SetHorizontalAlignCenter()
			
			# Handle dynamic fonts properly
			if app.ENABLE_DYNAMIC_FONTS:
				textLine.SetDynamicFont(True)
				textLine.SetFontName(localeInfo.UI_DEF_FONT)
			else:
				textLine.SetFontName(localeInfo.UI_DEF_FONT)
				
			textLine.Show()
			self.ButtonText = textLine
	
		self.ButtonText.SetText(text)
		
		# Force update the text rendering
		if hasattr(self.ButtonText, 'UpdateRect'):
			self.ButtonText.UpdateRect()

	def GetText(self):
		if self.ButtonText:
			return self.ButtonText.GetText()

	def IsDown(self):
		return self.isDown

	def Enable(self):
		try:
			apply(self.eventDict["ENABLE"], self.argsDict["ENABLE"])
		except KeyError:
			pass
		wndMgr.Enable(self.hWnd)

	def SetEnableEvent(self, func, *args):
		self.eventDict["ENABLE"] = __mem_func__(func)
		self.argsDict["ENABLE"] = args

	def Disable(self):
		try:
			apply(self.eventDict["DISABLE"], self.argsDict["DISABLE"])
		except KeyError:
			pass
		wndMgr.Disable(self.hWnd)

	def SetDisableEvent(self, func, *args):
		self.eventDict["DISABLE"] = __mem_func__(func)
		self.argsDict["DISABLE"] = args

	def SetUp(self):
		self.isDown = False
		self.LoadImage(self.images["UP"])

	def Down(self):
		self.isDown = True
		self.LoadImage(self.images["DOWN"])

	def OnMouseLeftButtonUp(self):
		if self.isDown:
			return
		self.state = "NORMAL"
		if self.IsIn():
			self.LoadImage(self.images["OVER"])
			snd.PlaySound("sound/ui/click.wav")
			try:
				apply(self.eventDict["MOUSE_CLICK"], self.argsDict["MOUSE_CLICK"])
			except KeyError:
				pass
		else:
			self.LoadImage(self.images["UP"])

	def SAFE_SetEvent(self, func, *args):
		self.eventDict["MOUSE_CLICK"] = __mem_func__(func)
		self.argsDict["MOUSE_CLICK"] = args

	def SetEvent(self, func, *args):
		self.eventDict["MOUSE_CLICK"] = func
		self.argsDict["MOUSE_CLICK"] = args

	def SetMouseLeftButtonDownEvent(self, func, *args):
		self.eventDict["MOUSE_DOWN"] = func
		self.argsDict["MOUSE_DOWN"] = args

	def GetEvent(self):
		return self.eventFunc, self.eventArgs

	def OnMouseLeftButtonDown(self):
		if self.isDown:
			return
		self.state = "DOWN"
		self.LoadImage(self.images["DOWN"])
		try:
			apply(self.eventDict["MOUSE_DOWN"], self.argsDict["MOUSE_DOWN"])
		except KeyError:
			pass

	def SetMouseDoubleClickEvent(self, func, *args):
		if self.isDown:
			return
		self.eventDict["MOUSE_DOUBLE_CLICK"] = __mem_func__(func)
		self.argsDict["MOUSE_DOUBLE_CLICK"] = args

	def OnMouseLeftButtonDoubleClick(self):
		if self.isDown:
			return
		try:
			apply(self.eventDict["MOUSE_DOUBLE_CLICK"], self.argsDict["MOUSE_DOUBLE_CLICK"])
		except KeyError:
			pass


class NewListBox(Window):
	def __init__(self):
		Window.__init__(self)

		self.items = []
		self.selected = None
		self.basePos = 0
		self.itemWidth = 100
		self.itemStep = 4
		self.totalItemHeight = 0
		self.scrollbar = None
		self.scrollBarPos = 0.0
		self.manualMode = False
		self.selectEvent = None

	def SetSize(self, w, h):
		Window.SetSize(self, w, h + self.itemStep)

		self.UpdateList()

	def SetScrollBar(self, scrollbar):
		self.scrollbar = scrollbar
		self.scrollbar.SetScrollEvent(__mem_func__(self.__OnScroll))
		self.scrollbar.SetScrollStep(0.07)
		self.UpdateList()

	def CalcTotalItemHeight(self):
		return self.totalItemHeight

	def ConfigureScrollBar(self):
		if self.scrollbar:
			itemheight = self.CalcTotalItemHeight()
			myheight = self.GetHeight() - 2 * self.itemStep
			dif = 0.97
			if itemheight > myheight and itemheight != 0:
				dif = 1.0 * myheight / itemheight

			self.scrollbar.SetScrollStep(0.12 * dif)
			self.scrollbar.SetMiddleBarSize(dif)

	def __OnScroll(self, position = None):
		pos = self.scrollbar.GetPos() if position == None else position
		self.scrollBarPos = pos
		toscr = self.CalcTotalItemHeight() - self.GetHeight() + 2 * self.itemStep
		self.basePos = toscr * pos

		self.UpdateList()

	def GetScrollBarPosition(self):
		return self.scrollBarPos

	def OnScroll(self, pos):
		self.__OnScroll(pos)

	def SelectItem(self, item):
		self.selected = item

		if self.selectEvent:
			self.selectEvent(item)

	def AppendItem(self, item):
		item.SetParent(self)
		item.Show()
		self.items.append(item)

		if not self.manualMode:
			self.UpdateList()

	def RemoveItem(self, item):
		item.Hide()

		self.items.remove(item)
		self.UpdateList()

	def ClearItems(self):
		map(lambda wnd: wnd.Hide(), self.items)
		del self.items[:]

		self.basePos = 0
		if self.scrollbar:
			self.scrollbar.SetPos(0)
		self.UpdateList()

	def UpdateList(self):
		self.ConfigureScrollBar()
		self.RecalcItemPositions()

	def IsEmpty(self):
		return len(self.items) == 0

	def SetItemWidth(self, w):
		self.itemWidth = w
		for item in self.items:
			item.SetWidth(w)

	def RecalcItemPositions(self):
		curbp = self.basePos

		itemheight = self.CalcTotalItemHeight()
		myheight = self.GetHeight() - 2 * self.itemStep

		if itemheight < myheight:
			curbp = 0

		fromPos = curbp
		curPos = 0
		curX = 0
		toPos = curbp + self.GetHeight()
		for item in self.items:
			hw = item.GetHeight()
			if curPos + hw < fromPos:
				item.Hide()
			elif curPos < fromPos < curPos + hw:
				item.SetRenderMin(fromPos - curPos)
				item.Show()
			elif curPos < toPos < curPos + hw:
				item.SetRenderMax(toPos - curPos)
				item.Show()
			elif curPos > toPos:
				item.Hide()
			else:
				item.SetRenderMin(0)
				item.Show()

			item.SetPosition(curX, curPos - fromPos)
			curX += item.GetWidth()
			if curX >= self.GetWidth():
				curX = 0
				curPos += hw + self.itemStep
			elif self.items.index(item) == len(self.items) - 1:
				curPos += item.GetHeight()

		if len(self.items):
			curPos += self.items[-1].GetHeight()

		self.totalItemHeight = curPos

class NewListBoxItem(Window):
	def __init__(self):
		Window.__init__(self)

		self.width = 0
		self.height = 0
		self.minh = 0
		self.maxh = 0

		self.components = []

	def __del__(self):
		Window.__del__(self)

	def SetColor(self, color=0xff0099ff):
		self.color = color

	def SetParent(self, parent):
		Window.SetParent(self, parent)

	def SetHeight(self, h):
		self.SetSize(self.width, h)

	def SetWidth(self, w):
		self.SetSize(w, self.height)

	def SetSize(self, w, h):
		self.width = w
		self.height = h
		self.maxh = h
		Window.SetSize(self, w, h)

	def SetRenderMin(self, minh):
		self.minh = minh
		self.maxh = self.height
		self.RecalculateRenderedComponents()

	def SetRenderMax(self, maxh):
		self.maxh = maxh
		self.minh = 0
		self.RecalculateRenderedComponents()

	def RegisterComponent(self, component):
		mtype = type(component).__name__
		if mtype == "Bar":
			(x, y, w, h) = component.GetRect()
			(x, y) = component.GetLocalPosition()
			component.__list_data = [x, y, w, h]
		if component not in self.components:
			self.components.append(component)
			component.Show()

	def UnregisterComponent(self, component):
		if component in self.components:
			self.components.remove(component)
			component.Hide()

	def RecalculateRenderedComponents(self):
		for component in self.components:
			(xl, yl) = component.GetLocalPosition()
			(x, y, w, h) = component.GetRect()
			mtype = type(component).__name__
			if mtype == "TextLine":
				(w, h) = component.GetTextSize()

			if yl + h < self.minh:
				component.Hide()
			elif yl > self.maxh:
				component.Hide()
			else:
				if mtype == "ExpandedImageBox" or mtype == "ExpandedButton":

					miny = 0
					if self.minh > 0 and yl < self.minh:
						miny = -float(self.minh - yl) / float(h)

					maxy = 0
					if h != 0:
						maxy = float(self.maxh - yl - h) / float(h)

					maxy = min(0, max(-1, maxy))

					component.SetRenderingRect(0.0, miny, 0.0, maxy)
					component.Show()
				elif mtype == "Bar":
					component.SetSize(w, min(self.maxh + self.minh, h))
					component.Show()
				else:
					if yl < self.minh or yl + h > self.maxh:
						component.Hide()
					else:
						component.Show()

	def OnRender(self):
		x, y = self.GetGlobalPosition()
		grp.SetColor(self.color)
		grp.RenderBar(x, y + self.minh, self.GetWidth(), self.maxh - self.minh)

class UnfoldListBox2(Window):

	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)
		self.width = 0
		self.height = 0
		self.basePos = 0
		self.xDif = 0
		self.yDif = 0
		self.showObjCount = 0
		self.showLineCount = 0
		self.fullLineCount = 0
		self.scrollPos = 0
		self.objList = []

		self.scrollBar = ScrollBar()
		self.scrollBar.SetParent(self)
		self.scrollBar.SetScrollEvent(self.OnScroll)
		self.scrollBar.Hide()

	def __del__(self):
		Window.__del__(self)

	def OnScroll(self):
		import math
		if self.fullLineCount <= 0 or self.showLineCount == self.fullLineCount:
			self.scrollPos = 0
			self.SetBasePos(0)
			return
		pos = self.scrollBar.GetPos()
		itempos = math.floor(float(pos) / float(float(1) / float(self.fullLineCount - self.showLineCount)) + 0.001)
		if itempos != self.scrollPos:
			self.scrollPos = itempos
			self.SetBasePos(itempos)

	def SetWidth(self, width):
		self.SetSize(width, self.height)

	def SetSize(self, width, height):
		Window.SetSize(self, width, height)
		self.width = width
		self.height = height
		self.scrollBar.SetScrollBarSize(self.height)
		self.scrollBar.SetPosition(self.width - self.scrollBar.GetWidth(), 0)

	def SetXDif(self, xDif):
		self.xDif = xDif
		self.LocateItem()

	def SetYDif(self, yDif):
		self.yDif = yDif
		self.LocateItem()

	def SetBasePos(self, pos):
		self.basePos = pos
		self.LocateItem(False)

	def ClearItem(self):
		for obj in self.objList:
			obj.Hide()

		self.objList = []
		self.scrollBar.SetPos(0)

	def InsertItem(self, obj, doLocate = True):
		obj.SetParent(self)
		self.objList.append(obj)

		if doLocate:
			self._LocateItem()

	def LocateItem(self, reloadScrollBar=True):
		self._LocateItem(reloadScrollBar)

	def _LocateItem(self, reloadScrollBar=True):
		import math
		skipCount = self.basePos
		yPos = 0
		xPos = 0
		if reloadScrollBar:
			self.showObjCount = 0
			self.showLineCount = 0
			self.fullLineCount = 0

		if len(self.objList) > 0:
			self.objPerLine = int(float(self.width-self.scrollBar.GetWidth()) / (self.objList[0].GetWidth()+self.xDif))

		for obj in self.objList:
			obj.Hide()


			if skipCount > 0:
				xPos += obj.GetWidth()+self.xDif
				if xPos-self.xDif > self.width-self.scrollBar.GetWidth():
					xPos = obj.GetWidth()+self.xDif
					skipCount -= 1
					if skipCount > 0:
						continue
					else:
						xPos = 0
				else:
					continue

			xPos += obj.GetWidth()+self.xDif
			if xPos-self.xDif > self.width-self.scrollBar.GetWidth():
				xPos = obj.GetWidth()+self.xDif
				yPos += obj.GetHeight()+self.yDif

			if reloadScrollBar:
				if xPos-obj.GetWidth()-self.xDif == 0:
					self.fullLineCount += 1
					if yPos+obj.GetHeight() <= self.height:
						self.showLineCount += 1

			obj.SetPosition(xPos-obj.GetWidth()-self.xDif, yPos)

			if yPos+obj.GetHeight() <= self.height:
				self.showObjCount += 1
				obj.Show()

		if reloadScrollBar:
			if self.showObjCount < len(self.objList):
				self.scrollBar.SetMiddleBarSize(float(self.showLineCount)/self.fullLineCount)
				self.scrollBar.Show()
			else:
				self.scrollBar.Hide()

	def ArrangeItem(self):
		self.SetSize(self.width, len(self.objList) * self.stepSize)
		self._LocateItem()

	def GetViewObjectCount(self):
		return self.showObjCount

	def GetObjectCount(self):
		return len(self.objList)

	def SetScrollBar(self, scrollBar):
		self.scrollBar = scrollBar
		self.scrollBar.SetScrollEvent(self.OnScroll)
		self.scrollBar.Hide()

class ComboBox(Window):

	class ListBoxWithBoard(ListBox):

		def __init__(self, layer):
			ListBox.__init__(self, layer)

		def OnRender(self):
			xRender, yRender = self.GetGlobalPosition()
			yRender -= self.TEMPORARY_PLACE
			widthRender = self.width
			heightRender = self.height + self.TEMPORARY_PLACE*2
			grp.SetColor(BACKGROUND_COLOR)
			grp.RenderBar(xRender, yRender, widthRender, heightRender)
			grp.SetColor(DARK_COLOR)
			grp.RenderLine(xRender, yRender, widthRender, 0)
			grp.RenderLine(xRender, yRender, 0, heightRender)
			grp.SetColor(BRIGHT_COLOR)
			grp.RenderLine(xRender, yRender+heightRender, widthRender, 0)
			grp.RenderLine(xRender+widthRender, yRender, 0, heightRender)

			ListBox.OnRender(self)

	def __init__(self):
		Window.__init__(self)
		self.x = 0
		self.y = 0
		self.width = 0
		self.height = 0
		self.isSelected = False
		self.isOver = False
		self.isListOpened = False
		self.event = lambda *arg: None
		self.enable = True

		self.textLine = MakeTextLine(self)
		self.textLine.SetText(localeInfo.UI_ITEM)

		self.listBox = self.ListBoxWithBoard("TOP_MOST")
		self.listBox.SetPickAlways()
		self.listBox.SetParent(self)
		self.listBox.SetEvent(__mem_func__(self.OnSelectItem))
		self.listBox.Hide()

	def __del__(self):
		Window.__del__(self)

	def Destroy(self):
		self.textLine = None
		self.listBox = None

	def SetPosition(self, x, y):
		Window.SetPosition(self, x, y)
		self.x = x
		self.y = y
		self.__ArrangeListBox()

	def SetSize(self, width, height):
		Window.SetSize(self, width, height)
		self.width = width
		self.height = height
		self.textLine.UpdateRect()
		self.__ArrangeListBox()

	def __ArrangeListBox(self):
		self.listBox.SetPosition(0, self.height + 5)
		self.listBox.SetWidth(self.width)

	def GetItemIndex(self, item):
		return self.listBox.GetItemIndex(item)
		
	def Enable(self):
		self.enable = True

	def Disable(self):
		self.enable = False
		self.textLine.SetText("")
		self.CloseListBox()

	def SetEvent(self, event):
		self.event = event

	def ClearItem(self):
		self.CloseListBox()
		self.listBox.ClearItem()

	def InsertItem(self, index, name):
		self.listBox.InsertItem(index, name)
		self.listBox.ArrangeItem()

	def SetCurrentItem(self, text):
		self.textLine.SetText(text)

	def SelectItem(self, key):
		self.listBox.SelectItem(key)

	def OnSelectItem(self, index, name):

		self.CloseListBox()
		self.event(index)

	def CloseListBox(self):
		self.isListOpened = False
		self.listBox.Hide()

	def OnMouseLeftButtonDown(self):

		if not self.enable:
			return

		self.isSelected = True

	def OnMouseLeftButtonUp(self):

		if not self.enable:
			return

		self.isSelected = False

		if self.isListOpened:
			self.CloseListBox()
		else:
			if self.listBox.GetItemCount() > 0:
				self.isListOpened = True
				self.listBox.Show()
				self.__ArrangeListBox()

	def OnUpdate(self):

		if not self.enable:
			return

		if self.IsIn():
			self.isOver = True
		else:
			self.isOver = False

	def OnRender(self):
		self.x, self.y = self.GetGlobalPosition()
		xRender = self.x
		yRender = self.y
		widthRender = self.width
		heightRender = self.height
		grp.SetColor(BACKGROUND_COLOR)
		grp.RenderBar(xRender, yRender, widthRender, heightRender)
		grp.SetColor(DARK_COLOR)
		grp.RenderLine(xRender, yRender, widthRender, 0)
		grp.RenderLine(xRender, yRender, 0, heightRender)
		grp.SetColor(BRIGHT_COLOR)
		grp.RenderLine(xRender, yRender+heightRender, widthRender, 0)
		grp.RenderLine(xRender+widthRender, yRender, 0, heightRender)

		if self.isOver:
			grp.SetColor(HALF_WHITE_COLOR)
			grp.RenderBar(xRender + 2, yRender + 3, self.width - 3, heightRender - 5)

			if self.isSelected:
				grp.SetColor(WHITE_COLOR)
				grp.RenderBar(xRender + 2, yRender + 3, self.width - 3, heightRender - 5)


class TextSlide(Window):
	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)
		self.SetWindowName("ListBox")
		self.globalParent = None
		self.slideNextTime = 0
		self.slideSpeed = 1
		self.slidePos = 0.0
		self.text = None

	def __del__(self):
		Window.__del__(self)
		self.globalParent = None
		self.slideNextTime = 0
		self.slideSpeed = 1
		self.slidePos = 0.0
		self.text = None
			
	def SetText(self, text, width, start_direction = "right"):
		self.direction = start_direction
		
		self.wndRender = Window()
		self.wndRender.SetParent(self)
		self.wndRender.SetPosition(0, 0)
		self.wndRender.SetInsideRender(True)
		self.wndRender.Show()
		
		self.text = TextLine()
		self.text.SetParent(self.wndRender)
		self.text.SetFontName(localeInfo.UI_DEF_FONT_LARGE)
		self.text.SetText(text)
		self.text.AddFlag("attach")
		self.text.AddFlag("not_pick")
		self.text.Show()
		
		xSize, ySize = self.text.GetTextSize()
		if self.direction == "right":
			self.xPos = width
			self.text.SetPosition(width, 0)
		else:
			self.xPos = 0 - xSize
			self.text.SetPosition(self.xPos, 0)
			
		self.SetSize(width, 30)
		self.wndRender.SetSize(width, 30)

	def OnUpdate(self):
		if not self.text:
			return

		xSize, ySize = self.text.GetTextSize() or (0, 0)
		if self.direction == "right":
			if 0 - xSize < self.xPos:
				self.xPos -= 1
				self.text.SetPosition(self.xPos, 0)
			else:
				self.xPos = self.wndRender.GetWidth()
				self.text.SetPosition(self.xPos, 0)
		else:
			if self.wndRender.GetWidth() > self.xPos:
				self.xPos += 1
				self.text.SetPosition(self.xPos, 0)
			else:
				self.xPos = 0 - xSize
				self.text.SetPosition(self.xPos, 0)


class ScriptWindow(Window):
	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)
		self.Children = []
		self.ElementDictionary = {}
	def __del__(self):
		Window.__del__(self)

	def ClearDictionary(self):
		self.Children = []
		self.ElementDictionary = {}
	def InsertChild(self, name, child):
		self.ElementDictionary[name] = child

	def IsChild(self, name):
		return self.ElementDictionary.has_key(name)
	def GetChild(self, name):
		return self.ElementDictionary[name]

	def GetChild2(self, name):
		return self.ElementDictionary.get(name, None)

	def LoadScript(self, window, filename):
		try:
			PythonScriptLoader().LoadScriptFile(window, filename)
			return True
		except Exception as e:
			dbg.TraceError("Failed to load uiscript file: {}".format(filename))
			dbg.TraceError(str(e))
			return False

if app.ENABLE_RESP_SYSTEM:
	class RespBoard(Window):
		CORNER_WIDTH = 16
		CORNER_HEIGHT = 16
		LINE_WIDTH = 16
		LINE_HEIGHT = 16
		BOARD_COLOR = grp.GenerateColor(0.03, 0.03, 0.03, 0.44)

		LT = 0
		LB = 1
		RT = 2
		RB = 3
		L = 0
		R = 1
		T = 2
		B = 3

		def __init__(self, layer="UI"):
			Window.__init__(self, layer)

			CornerFileNames = ["d:/ymir work/ui/game/resp/board/corner_" + dir + ".tga" for dir in
							   ["lefttop", "leftbottom", "righttop", "rightbottom"]]
			LineFileNames = ["d:/ymir work/ui/game/resp/board/Line_" + dir + ".tga" for dir in
							 ["left", "right", "top", "bottom"]]

			self.Corners = []
			for fileName in CornerFileNames:
				Corner = ExpandedImageBox()
				Corner.AddFlag("attach")
				Corner.AddFlag("not_pick")
				Corner.LoadImage(fileName)
				Corner.SetParent(self)
				Corner.SetPosition(0, 0)
				Corner.Show()
				self.Corners.append(Corner)

			self.Lines = []
			for fileName in LineFileNames:
				Line = ExpandedImageBox()
				Line.AddFlag("attach")
				Line.AddFlag("not_pick")
				Line.LoadImage(fileName)
				Line.SetParent(self)
				Line.SetPosition(0, 0)
				Line.Show()
				self.Lines.append(Line)

			Base = Bar()
			Base.SetParent(self)
			Base.AddFlag("attach")
			Base.AddFlag("not_pick")
			Base.SetPosition(self.CORNER_WIDTH, self.CORNER_HEIGHT)
			Base.SetColor(self.BOARD_COLOR)
			Base.Show()
			self.Base = Base

			self.Lines[self.L].SetPosition(0, self.CORNER_HEIGHT)
			self.Lines[self.T].SetPosition(self.CORNER_WIDTH, 0)

		def __del__(self):
			Window.__del__(self)

		def SetSize(self, width, height):

			width = max(self.CORNER_WIDTH * 2, width)
			height = max(self.CORNER_HEIGHT * 2, height)
			Window.SetSize(self, width, height)

			self.Corners[self.LB].SetPosition(0, height - self.CORNER_HEIGHT)
			self.Corners[self.RT].SetPosition(width - self.CORNER_WIDTH, 0)
			self.Corners[self.RB].SetPosition(width - self.CORNER_WIDTH, height - self.CORNER_HEIGHT)
			self.Lines[self.R].SetPosition(width - self.CORNER_WIDTH, self.CORNER_HEIGHT)
			self.Lines[self.B].SetPosition(self.CORNER_HEIGHT, height - self.CORNER_HEIGHT)

			verticalShowingPercentage = float((height - self.CORNER_HEIGHT * 2) - self.LINE_HEIGHT) / self.LINE_HEIGHT
			horizontalShowingPercentage = float((width - self.CORNER_WIDTH * 2) - self.LINE_WIDTH) / self.LINE_WIDTH
			self.Lines[self.L].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
			self.Lines[self.R].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
			self.Lines[self.T].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
			self.Lines[self.B].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
			self.Base.SetSize(width - self.CORNER_WIDTH * 2, height - self.CORNER_HEIGHT * 2)

		def ShowInternal(self):
			self.Base.Show()
			for wnd in self.Lines:
				wnd.Show()
			for wnd in self.Corners:
				wnd.Show()

		def HideInternal(self):
			self.Base.Hide()
			for wnd in self.Lines:
				wnd.Hide()
			for wnd in self.Corners:
				wnd.Hide()

	class RespCheckBox(ImageBox):
		def __init__(self):
			ImageBox.__init__(self)

			self.LoadImage("d:/ymir work/ui/game/resp/checkbox_empty.sub")

			image = MakeImageBox(self, "d:/ymir work/ui/game/resp/checkbox_full.sub", 0, 0)
			image.AddFlag("not_pick")
			image.Hide()

			self.image = image
			self.event = None
			self.uncheckEvent = None
			self.flag = False

			self.Show()

			self.SetCheck(False)

		def __del__(self):
			ImageBox.__del__(self)

		def IsChecked(self):
			return self.flag

		def SetCheck(self, flag):
			if flag:
				self.image.Show()
			else:
				self.image.Hide()
			self.flag = flag

		def SetEvent(self, event):
			self.event = event

		def SetUncheckEvent(self, event):
			self.uncheckEvent = event

		def OnMouseOverIn(self):
			if not self.flag:
				self.image.Show()

		def OnMouseOverOut(self):
			if self.flag:
				self.image.Show()
			else:
				self.image.Hide()

		def OnMouseLeftButtonUp(self):
			if self.IsChecked():
				self.SetCheck(False)
				if self.uncheckEvent:
					self.uncheckEvent()
			else:
				self.SetCheck(True)
				if self.event:
					self.event()

class SimplyWindow(Window):
	def __init__(self, layer, flags, width, height, initializeMethod = None, destroyMethod = None):
		Window.__init__(self, layer)
		self.children		= {}
		self.windowConfig	= {
			"size"	:	{
				"width"		:	width,
				"height"	:	height
			},
			"flag"	:	flags,
		}

		if destroyMethod:
			self.destroyMethod = __mem_func__(destroyMethod)

		if initializeMethod:
			self.Initialize(__mem_func__(initializeMethod))

	def __del__(self):
		Window.__del__(self)

	def AppendObject(self, key, child, multiply = False):
		parent = self.children.get(key, None)

		if isinstance(parent, (list, tuple)) and parent != None:
			self.children[key].append(child)
		else:
			if multiply == True:
				self.children[key] = [child]
			else:
				self.children[key] = child

	def DeleteObject(self, key):
		if key in self.children:
			del self.children[key]

	def GetObject(self, key, index = -1):
		child = self.children.get(key, None)

		if isinstance(child, (tuple, list)):
			if index != -1:
				return child[index]
			return child
		elif index != -1:
			return proxy(child[index])
		else:
			return proxy(child)

	def GetCountObject(self, key):
		try:
			return len(self.children.get(key, None))
		except (KeyError, TypeError):
			return 0

	def Initialize(self, initializeMethod):
		self.SetSize(self.windowConfig["size"]["width"], self.windowConfig["size"]["height"])
		map(self.AddFlag, self.windowConfig["flag"])
		self.SetCenterPosition()

		try:
			initializeMethod()
			return True
		except (Exception, ) as error:
			return self.PrintException(error)

	def PrintException(self, error):
		import exception
		exception.Abort(" ")
		
		self.Destroy()
		self.Close()
		return False

	def GetSize(self):
		return (self.GetWidth(), self.GetHeight(), )
	
	def Destroy(self):
		if hasattr(self, "destroyMethod"):
			self.destroyMethod()

		self.children = {}
		self.destroyMethod = None

	def Open(self):
		self.SetTop()
		self.Show()

	def Close(self):
		self.Hide()

	def OpenWindow(self):
		self.Close() if self.IsShow() == True else self.Open()

	def OnPressExitKey(self):
		self.Close()
		return True

	OnPressEscapeKey = OnPressExitKey
	
class TransparentButton(Window):
	def	__init__(self):
		Window.__init__(self)
		self.bBuilt = False

		self.bRunningEffect = False

		self.textLine = None
		self.buttonImages = {}
		self.bindEvents = {}

	def	__del__(self):
		Window.__del__(self)

		self.bRunningEffect = False

		self.textLine = None
		self.buttonImages = {}
		self.bindEvents = {}

	def	__BuildObject(self):
		if not self.bBuilt:
			if not "NORMAL" in self.buttonImages:
				print "Cannot build button! Normal image is incorrent!"
				return

			## Loading image
			self.buttonImage = ImageBox()
			self.buttonImage.SetParent(self)
			self.buttonImage.LoadImage(self.buttonImages["NORMAL"])
			self.buttonImage.SetPosition(0, 0)

			## Size
			self.SetSize(self.buttonImage.GetWidth(), self.buttonImage.GetHeight())
			print self.buttonImage.GetWidth(), self.buttonImage.GetHeight()

			self.buttonImage.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__ShowHover) 
			self.buttonImage.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__ShowNormal) 
			self.buttonImage.SetOnMouseLeftButtonUpEvent(self.__ClickButton)
			self.buttonImage.SetClickEvent(self.__ShowDark)

			self.buttonImage.Show()

			self.textLine = TextLine()
			self.textLine.SetParent(self.buttonImage)
			self.textLine.SetHorizontalAlignCenter()
			self.textLine.SetVerticalAlignCenter()
			self.textLine.SetWindowHorizontalAlignCenter()
			self.textLine.SetWindowVerticalAlignCenter()
			self.textLine.SetPackedFontColor(0xFFdcc2a9)
			self.textLine.SetOutline()
			self.textLine.SetPosition(0, 0)
			self.textLine.Show()

			self.bBuilt = True

	def	SetUpVisual(self, sImage):
		self.buttonImages["NORMAL"] = sImage

	def	SetOverVisual(self, sImage):
		self.buttonImages["HOVER"] = sImage

	def	SetDownVisual(self, sImage):
		self.buttonImages["DOWN"] = sImage

	def	BindEvent(self, key, event, *args):
		self.bindEvents[key] = (__mem_func__(event), args)

	def	RunningEffect(self, bState):
		self.bRunningEffect = bState

	def	ReloadImage(self):
		if self.bBuilt:
			if "NORMAL" in self.buttonImages:
				self.buttonImage.LoadImage(self.buttonImages["NORMAL"])

	## Deprecated
	def	SetDisableVisual(self, sImage):
		pass

	def	SetText(self, sText):
		self.textLine.SetText(sText)

	def	SetTextColor(self, rColour):
		pass

	def	SetToolTipText(self, sText):
		pass

	## Late binds
	def	__ShowHover(self):
		if not self.bRunningEffect:
			if "HOVER" in self.buttonImages:
				self.buttonImage.LoadImage(self.buttonImages["HOVER"])

			if "HOVER" in self.bindEvents:
				self.bindEvents["HOVER"][0](*self.bindEvents["HOVER"][1])

	def	__ShowNormal(self):
		if not self.bRunningEffect:
			if "NORMAL" in self.buttonImages:
				self.buttonImage.LoadImage(self.buttonImages["NORMAL"])

			if "NORMAL" in self.bindEvents:
				self.bindEvents["NORMAL"][0](*self.bindEvents["NORMAL"][1])

	def	__ShowDark(self):
		if not self.bRunningEffect:
			if "DARK" in self.buttonImages:
				self.buttonImage.LoadImage(self.buttonImages["DARK"])

			if "DARK" in self.bindEvents:
				self.bindEvents["DARK"][0](*self.bindEvents["DARK"][1])

	def	__ClickButton(self):
		if not self.bRunningEffect:
			self.buttonImage.LoadImage(self.buttonImages["NORMAL"])

			if "CLICK" in self.bindEvents:
				self.bindEvents["CLICK"][0](*self.bindEvents["CLICK"][1])

	def	Show(self):
		if not self.bBuilt:
			self.__BuildObject()

		Window.Show(self)


class TabBoard(Window):
	BUTTON_NORMAL	=	0
	BUTTON_HOVER	=	1
	BUTTON_ACTIVE	=	2
	def __init__(self):
		Window.__init__(self)

		self.__imageResources = {
			self.BUTTON_NORMAL : None,
			self.BUTTON_HOVER : None,
			self.BUTTON_ACTIVE : None,
		}

		self.main_board = None
		self.tab_buttons = {}
		self.tab_width = 85
		self.selected_tab = None

	def SetSize(self, width, height):
		Window.SetSize(self, width, height)

	def SetTabWidth(self, width):
		self.tab_width = width
	
	def SetVisual(self, default, hover = None, active = None):
		self.__imageResources[self.BUTTON_NORMAL] = default
		if hover:
			self.__imageResources[self.BUTTON_HOVER] = hover
		if active:
			self.__imageResources[self.BUTTON_ACTIVE] = active

	def AppendTab(self, name):
		self.tab_buttons[name] = RadioButton()
		self.tab_buttons[name].SetParent(self)
		self.tab_buttons[name].SetUpVisual(self.__imageResources[self.BUTTON_NORMAL])
		self.tab_buttons[name].SetOverVisual(self.__imageResources[self.BUTTON_HOVER])
		self.tab_buttons[name].SetDownVisual(self.__imageResources[self.BUTTON_ACTIVE])
		
		self.SetTabWidth(self.tab_buttons[name].GetWidth())
		self.tab_buttons[name].SetPosition((len(self.tab_buttons) - 1) * self.tab_width, 0)
		self.tab_buttons[name].Show()

		self.tab_buttons[name].description = TextLine()
		self.tab_buttons[name].description.SetParent(self.tab_buttons[name])
		self.tab_buttons[name].description.SetHorizontalAlignCenter()
		self.tab_buttons[name].description.SetVerticalAlignCenter()
		self.tab_buttons[name].description.SetWindowHorizontalAlignCenter()
		self.tab_buttons[name].description.SetWindowVerticalAlignCenter()
		self.tab_buttons[name].description.SetText(name)
		self.tab_buttons[name].description.Show()

		self.tab_buttons[name].SAFE_SetEvent(self.OnSelectTab, name)

	def SetTabEvent(self, name, event, *args):
		self.tab_buttons[name].event_func = event
		self.tab_buttons[name].event_args = args

	def SelectTab(self, name):
		for tab in self.tab_buttons.values():
			tab.SetUp()

		self.tab_buttons[name].Down()

		self.selected_tab = name

	def OnSelectTab(self, name):
		self.SelectTab(name)

		try:
			if self.tab_buttons[name].event_func:
				apply(self.tab_buttons[name].event_func, self.tab_buttons[name].event_args)
		except:
			pass
	def GetSelectedTab(self):
		return self.selected_tab

	def GetSelectedTabIndex(self):
		if self.selected_tab is None:
			return -1

		return list(self.tab_buttons.keys()).index(self.selected_tab)

class EditLineCentered(EditLine):
	def __init__(self):
		EditLine.__init__(self)
		self.basePos = (0, 0)

	def __del__(self):
		EditLine.__del__(self)
		del self.basePos

	def SetFocus(self):
		EditLine.SetFocus(self)
		self.AdjustTextPosition()

	def SetPosition(self, x, y):
		EditLine.SetPosition(self, x, y)
		self.basePos = (x, y)
		self.AdjustTextPosition()

	def OnIMEUpdate(self):
		EditLine.OnIMEUpdate(self)
		self.AdjustTextPosition()

	def SetText(self, text):
		EditLine.SetText(self, text)
		self.AdjustTextPosition()

	def CanEdit(self, flag):
		self.CanClick = flag

	def OnMouseLeftButtonDown(self):
		if False == self.CanEdit:
			return False

		if self.IsIn():
			EditLine.SetFocus(self)
			ime.SetCursorPosition(wndMgr.GetCursorPosition(self.hWnd))

	def AdjustTextPosition(self):
		(textX, textY) = EditLine.GetTextSize(self)
		(locX, locY) = self.basePos
		TextLine.SetPosition(self, locX + (self.GetWidth() / 2) - textX / 2, locY)


class PythonScriptLoader(object):

	BODY_KEY_LIST = ( "x", "y", "width", "height" )


	DEFAULT_KEY_LIST = ( "type", "x", "y", )
	WINDOW_KEY_LIST = ( "width", "height", )
	IMAGE_KEY_LIST = ( "image", )
	EXPANDED_IMAGE_KEY_LIST = ( "image", )
	ANI_IMAGE_KEY_LIST = ( "images", )
	SLOT_KEY_LIST = ( "width", "height", "slot", )
	CANDIDATE_LIST_KEY_LIST = ( "item_step", "item_xsize", "item_ysize", )
	GRID_TABLE_KEY_LIST = ( "start_index", "x_count", "y_count", "x_step", "y_step", )
	EDIT_LINE_KEY_LIST = ( "width", "height", "input_limit", )
	COMBO_BOX_KEY_LIST = ( "width", "height", "item", )
	TITLE_BAR_KEY_LIST = ( "width", )
	HORIZONTAL_BAR_KEY_LIST = ( "width", )
	BOARD_KEY_LIST = ( "width", "height", )
	BOARD_WITH_TITLEBAR_KEY_LIST = ( "width", "height", "title", )
	BOX_KEY_LIST = ( "width", "height", )
	BAR_KEY_LIST = ( "width", "height", )
	LINE_KEY_LIST = ( "width", "height", )
	SLOTBAR_KEY_LIST = ( "width", "height", )
	GAUGE_KEY_LIST = ( "width", "color", )
	SCROLLBAR_KEY_LIST = ( "size", )
	LIST_BOX_KEY_LIST = ( "width", "height", )
	RENDER_TARGET_KEY_LIST = ( "index", )
	MODERN_SCROLLBAR_KEY_LIST = ("size",)
	if app.ENABLE_QUEST_RENEWAL:
		SUB_TITLE_BAR_KEY_LIST = ( "width", )
		LIST_BAR_KEY_LIST = ( "width", )

	def __init__(self):
		self.Clear()

	def Clear(self):
		self.ScriptDictionary = { "SCREEN_WIDTH" : wndMgr.GetScreenWidth(), "SCREEN_HEIGHT" : wndMgr.GetScreenHeight() }
		self.InsertFunction = 0

	def LoadScriptData(self, window, code):
		self.Clear()
		exec code in self.ScriptDictionary
		Body = self.ScriptDictionary["window"]
		self.CheckKeyList("window", Body, self.BODY_KEY_LIST)
		window.ClearDictionary()
		self.InsertFunction = window.InsertChild
		window.SetPosition(int(Body["x"]), int(Body["y"]))
		if localeInfo.IsARABIC():
			w = wndMgr.GetScreenWidth()
			h = wndMgr.GetScreenHeight()
			if "width" in Body:
				w = int(Body["width"])
			if "height" in Body:
				h = int(Body["height"])
			window.SetSize(w, h)
		else:
			window.SetSize(int(Body["width"]), int(Body["height"]))
			if True == ("style" in Body):
				for StyleList in Body["style"]:
					window.AddFlag(StyleList)
		self.LoadChildren(window, Body)

	def LoadScriptFile(self, window, FileName):
		import exception
		import exceptions
		import os
		import errno
		self.Clear()

		print "===== Load Script File : %s" % (FileName)

		from utils import Sandbox
		sandbox = Sandbox(True, ["uiScriptLocale", "localeInfo", "sys", "item", "app", "player","utils", "emoji"])

		import chr
		import player
		import app
		self.ScriptDictionary["PLAYER_NAME_MAX_LEN"] = chr.PLAYER_NAME_MAX_LEN
		self.ScriptDictionary["DRAGON_SOUL_EQUIPMENT_SLOT_START"] = player.DRAGON_SOUL_EQUIPMENT_SLOT_START
		self.ScriptDictionary["LOCALE_PATH"] = app.GetLocalePath()

		if __USE_EXTRA_CYTHON__:
			from os.path import splitext as op_splitext, basename as op_basename, dirname as op_dirname
			def GetModName(filename):
				return op_splitext(op_basename(filename))[0]
			def IsInUiPath(filename):
				def ICmp(s1, s2):
					return s1.lower() == s2.lower()
				return ICmp(op_dirname(filename), "uiscript")
			modname = GetModName(FileName)
			import uiscriptlib
			tpl2Main = (
				"SCREEN_WIDTH","SCREEN_HEIGHT",
				"PLAYER_NAME_MAX_LEN", "DRAGON_SOUL_EQUIPMENT_SLOT_START","LOCALE_PATH"
			)
			import __builtin__ as bt
			for idx in tpl2Main:
				tmpVal = self.ScriptDictionary[idx]
				exec "bt.%s = tmpVal"%idx in globals(), locals()
		try:
			if __USE_EXTRA_CYTHON__ and IsInUiPath(FileName) and uiscriptlib.isExist(modname):
				m1 = uiscriptlib.moduleImport(modname)
				self.ScriptDictionary["window"] = m1.window.copy()
				del m1
			else:
				sandbox.execfile(FileName, self.ScriptDictionary)
		except IOError, err:
			dbg.TraceError("Failed to load script file : %s" % (FileName))
			dbg.TraceError("error  : %s" % (err))
			exception.Abort("LoadScriptFile1")
		except RuntimeError,err:
			dbg.TraceError("Failed to load script file : %s" % (FileName))
			dbg.TraceError("error  : %s" % (err))
			exception.Abort("LoadScriptFile2")
		except:
			dbg.TraceError("Failed to load script file : %s" % (FileName))
			exception.Abort("LoadScriptFile!!!!!!!!!!!!!!")


		Body = self.ScriptDictionary["window"]
		self.CheckKeyList("window", Body, self.BODY_KEY_LIST)

		window.ClearDictionary()
		self.InsertFunction = window.InsertChild

		window.SetPosition(int(Body["x"]), int(Body["y"]))

		if localeInfo.IsARABIC():
			w = wndMgr.GetScreenWidth()
			h = wndMgr.GetScreenHeight()
			if Body.has_key("width"):
				w = int(Body["width"])
			if Body.has_key("height"):
				h = int(Body["height"])

			window.SetSize(w, h)
		else:
			window.SetSize(int(Body["width"]), int(Body["height"]))
			if True == Body.has_key("style"):
				for StyleList in Body["style"]:
					window.AddFlag(StyleList)


		self.LoadChildren(window, Body)

	def LoadChildren(self, parent, dicChildren):

		if localeInfo.IsARABIC():
			parent.AddFlag( "rtl" )

		if True == dicChildren.has_key("style"):
			for style in dicChildren["style"]:
				parent.AddFlag(style)

		if False == dicChildren.has_key("children"):
			return False

		Index = 0

		ChildrenList = dicChildren["children"]
		parent.Children = range(len(ChildrenList))
		for ElementValue in ChildrenList:
			try:
				Name = ElementValue["name"]
			except KeyError:
				Name = ElementValue["name"] = "NONAME"

			try:
				Type = ElementValue["type"]
			except KeyError:
				Type = ElementValue["type"] = "window"

			if False == self.CheckKeyList(Name, ElementValue, self.DEFAULT_KEY_LIST):
				del parent.Children[Index]
				continue

			if Type == "window":
				parent.Children[Index] = ScriptWindow()
				parent.Children[Index].SetParent(parent)
				self.LoadElementWindow(parent.Children[Index], ElementValue, parent)

			elif Type == "render_target":	
				parent.Children[Index] = RenderTarget()
				parent.Children[Index].SetParent(parent)
				self.LoadElementRenderTarget(parent.Children[Index], ElementValue, parent)

			elif Type == "button":
				parent.Children[Index] = Button()
				parent.Children[Index].SetParent(parent)
				self.LoadElementButton(parent.Children[Index], ElementValue, parent)

			elif Type == "radio_button":
				parent.Children[Index] = RadioButton()
				parent.Children[Index].SetParent(parent)
				self.LoadElementButton(parent.Children[Index], ElementValue, parent)

			elif Type == "toggle_button":
				parent.Children[Index] = ToggleButton()
				parent.Children[Index].SetParent(parent)
				self.LoadElementButton(parent.Children[Index], ElementValue, parent)

			elif Type == "mark":
				parent.Children[Index] = MarkBox()
				parent.Children[Index].SetParent(parent)
				self.LoadElementMark(parent.Children[Index], ElementValue, parent)

			elif Type == "image":
				parent.Children[Index] = ImageBox()
				parent.Children[Index].SetParent(parent)
				self.LoadElementImage(parent.Children[Index], ElementValue, parent)

			elif Type == "expanded_image":
				parent.Children[Index] = ExpandedImageBox()
				parent.Children[Index].SetParent(parent)
				self.LoadElementExpandedImage(parent.Children[Index], ElementValue, parent)

			elif Type == "ani_image":
				parent.Children[Index] = AniImageBox()
				parent.Children[Index].SetParent(parent)
				self.LoadElementAniImage(parent.Children[Index], ElementValue, parent)

			elif Type == "slot":
				parent.Children[Index] = SlotWindow()
				parent.Children[Index].SetParent(parent)
				self.LoadElementSlot(parent.Children[Index], ElementValue, parent)

			elif Type == "candidate_list":
				parent.Children[Index] = CandidateListBox()
				parent.Children[Index].SetParent(parent)
				self.LoadElementCandidateList(parent.Children[Index], ElementValue, parent)

			elif Type == "grid_table":
				parent.Children[Index] = GridSlotWindow()
				parent.Children[Index].SetParent(parent)
				self.LoadElementGridTable(parent.Children[Index], ElementValue, parent)

			elif Type == "text":
				parent.Children[Index] = TextLine()
				parent.Children[Index].SetParent(parent)
				self.LoadElementText(parent.Children[Index], ElementValue, parent)

			elif Type == "editline":
				parent.Children[Index] = EditLine()
				parent.Children[Index].SetParent(parent)
				self.LoadElementEditLine(parent.Children[Index], ElementValue, parent)
				
			elif Type == "multi_text":
				parent.Children[Index] = MultiTextLineNew()
				parent.Children[Index].SetParent(parent)
				self.LoadElementText(parent.Children[Index], ElementValue, parent)

			elif Type == "titlebar":
				parent.Children[Index] = TitleBar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementTitleBar(parent.Children[Index], ElementValue, parent)

			elif Type == "horizontalbar":
				parent.Children[Index] = HorizontalBar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementHorizontalBar(parent.Children[Index], ElementValue, parent)

			elif Type == "board":
				parent.Children[Index] = Board()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBoard(parent.Children[Index], ElementValue, parent)

			elif Type == "board_with_titlebar":
				parent.Children[Index] = BoardWithTitleBar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBoardWithTitleBar(parent.Children[Index], ElementValue, parent)

			elif Type == "border_a":
				parent.Children[Index] = BorderA()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBorderA(parent.Children[Index], ElementValue, parent)

			elif Type == "thinboard":
				parent.Children[Index] = ThinBoard()
				parent.Children[Index].SetParent(parent)
				self.LoadElementThinBoard(parent.Children[Index], ElementValue, parent)
			
			elif Type == "thinboard_circle" or Type == "thinboardcircle":
				parent.Children[Index] = ThinBoardCircle()
				parent.Children[Index].SetParent(parent)
				self.LoadElementThinBoard(parent.Children[Index], ElementValue, parent)

			elif Type == "thinboard_new":
				parent.Children[Index] = ThinBoardNew()
				parent.Children[Index].SetParent(parent)
				self.LoadElementThinBoard(parent.Children[Index], ElementValue, parent)

			elif Type == "box":
				parent.Children[Index] = Box()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBox(parent.Children[Index], ElementValue, parent)

			elif Type == "bar":
				parent.Children[Index] = Bar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBar(parent.Children[Index], ElementValue, parent)

			elif Type == "line":
				parent.Children[Index] = Line()
				parent.Children[Index].SetParent(parent)
				self.LoadElementLine(parent.Children[Index], ElementValue, parent)

			elif Type == "slotbar":
				parent.Children[Index] = SlotBar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementSlotBar(parent.Children[Index], ElementValue, parent)

			elif Type == "gauge":
				parent.Children[Index] = Gauge()
				parent.Children[Index].SetParent(parent)
				self.LoadElementGauge(parent.Children[Index], ElementValue, parent)
				
			elif Type == "newslimscrollbar":
				parent.Children[Index] = NewSlimScrollBar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementScrollBar(parent.Children[Index], ElementValue, parent)

			elif Type == "scrollbar":
				parent.Children[Index] = ScrollBar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementScrollBar(parent.Children[Index], ElementValue, parent)

			elif Type == "scrollbar_special":
				parent.Children[Index] = ScrollBarSpecial()
				parent.Children[Index].SetParent(parent)
				self.LoadElementScrollBar(parent.Children[Index], ElementValue, parent)

			elif Type == "scrollbar_zaris":
				parent.Children[Index] = ScrollBarZaris()
				parent.Children[Index].SetParent(parent)
				self.LoadElementScrollBar(parent.Children[Index], ElementValue, parent)

			elif Type == "scrollbarae":
				parent.Children[Index] = ScrollBarAe()
				parent.Children[Index].SetParent(parent)
				self.LoadElementScrollBar(parent.Children[Index], ElementValue, parent)

			elif Type == "thin_scrollbar":
				parent.Children[Index] = ThinScrollBar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementScrollBar(parent.Children[Index], ElementValue, parent)

			elif Type == "small_thin_scrollbar":
				parent.Children[Index] = SmallThinScrollBar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementScrollBar(parent.Children[Index], ElementValue, parent)

			elif Type == "sliderbar":
				parent.Children[Index] = SliderBar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementSliderBar(parent.Children[Index], ElementValue, parent)

			elif Type == "sliderbarNew":
				parent.Children[Index] = SliderBarNew()
				parent.Children[Index].SetParent(parent)
				self.LoadElementSliderBar(parent.Children[Index], ElementValue, parent)

			elif Type == "listbox":
				parent.Children[Index] = ListBox()
				parent.Children[Index].SetParent(parent)
				self.LoadElementListBox(parent.Children[Index], ElementValue, parent)

			elif Type == "listbox2":
				parent.Children[Index] = ListBox2()
				parent.Children[Index].SetParent(parent)
				self.LoadElementListBox2(parent.Children[Index], ElementValue, parent)
			elif Type == "listboxex":
				parent.Children[Index] = ListBoxEx()
				parent.Children[Index].SetParent(parent)
				self.LoadElementListBoxEx(parent.Children[Index], ElementValue, parent)
			elif app.ENABLE_RESP_SYSTEM and Type == "resp_board":
				parent.Children[Index] = RespBoard()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBoard(parent.Children[Index], ElementValue, parent)
			elif app.ENABLE_RESP_SYSTEM and Type == "resp_checkbox":
				parent.Children[Index] = RespCheckBox()
				parent.Children[Index].SetParent(parent)
				self.LoadElementWindow(parent.Children[Index], ElementValue, parent)
			elif Type == "subtitlebar" and app.ENABLE_QUEST_RENEWAL:
				parent.Children[Index] = SubTitleBar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementSubTitleBar(parent.Children[Index], ElementValue, parent)

			elif Type == "listbar" and app.ENABLE_QUEST_RENEWAL:
				parent.Children[Index] = ListBar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementListBar(parent.Children[Index], ElementValue, parent)
			elif Type == "check_box":
				parent.Children[Index] = CheckBox()
				parent.Children[Index].SetParent(parent)
				self.LoadElementWindow(parent.Children[Index], ElementValue, parent)

			elif Type == "modern_scrollbar":
				parent.Children[Index] = ModernScrollBar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementModernScrollBar(parent.Children[Index], ElementValue, parent)

			elif Type == "editline_centered":
				parent.Children[Index] = EditLineCentered()
				parent.Children[Index].SetParent(parent)
				self.LoadElementEditLine(parent.Children[Index], ElementValue, parent)

			elif Type == "unfoldlistbox":
				parent.Children[Index] = UnfoldListBox2()
				parent.Children[Index].SetParent(parent)
				self.LoadElementUnfoldListBox(parent.Children[Index], ElementValue, parent)

			elif Type == "checkbox_biolog":
				parent.Children[Index] = CheckBox_Biolog()
				parent.Children[Index].SetParent(parent)
				self.LoadElementCheckBox(parent.Children[Index], ElementValue, parent)

			elif Type == "boxed_board":
				parent.Children[Index] = BoxedBoard()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBoxedBoard(parent.Children[Index], ElementValue, parent)
			elif app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL and Type == "thinboard_deco":
				parent.Children[Index] = ShopDecoThinboard()
				parent.Children[Index].SetParent(parent)
				self.LoadElementThinBoard(parent.Children[Index], ElementValue, parent)
		
			else:
				Index += 1
				continue

			parent.Children[Index].SetWindowName(Name)
			if 0 != self.InsertFunction:
				self.InsertFunction(Name, parent.Children[Index])

			self.LoadChildren(parent.Children[Index], ElementValue)
			Index += 1
			

	def CheckKeyList(self, name, value, key_list):

		for DataKey in key_list:
			if False == value.has_key(DataKey):
				print "Failed to find data key", "[" + name + "/" + DataKey + "]"
				return False

		return True

	def LoadDefaultData(self, window, value, parentWindow):
		loc_x = int(value["x"])
		loc_y = int(value["y"])
		if value.has_key("vertical_align"):
			if "center" == value["vertical_align"]:
				window.SetWindowVerticalAlignCenter()
			elif "bottom" == value["vertical_align"]:
				window.SetWindowVerticalAlignBottom()

		if parentWindow.IsRTL():
			loc_x = int(value["x"]) + window.GetWidth()
			if value.has_key("horizontal_align"):
				if "center" == value["horizontal_align"]:
					window.SetWindowHorizontalAlignCenter()
					loc_x = - int(value["x"])
				elif "right" == value["horizontal_align"]:
					window.SetWindowHorizontalAlignLeft()
					loc_x = int(value["x"]) - window.GetWidth()
			else:
				window.SetWindowHorizontalAlignRight()

			if value.has_key("all_align"):
				window.SetWindowVerticalAlignCenter()
				window.SetWindowHorizontalAlignCenter()
				loc_x = - int(value["x"])
		else:
			if value.has_key("horizontal_align"):
				if "center" == value["horizontal_align"]:
					window.SetWindowHorizontalAlignCenter()
				elif "right" == value["horizontal_align"]:
					window.SetWindowHorizontalAlignRight()

		window.SetPosition(loc_x, loc_y)
		window.Show()

	def LoadElementWindow(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.WINDOW_KEY_LIST):
			return False

		window.SetSize(int(value["width"]), int(value["height"]))
		self.LoadDefaultData(window, value, parentWindow)

		return True

	def LoadElementButton(self, window, value, parentWindow):

		if value.has_key("width") and value.has_key("height"):
			window.SetSize(int(value["width"]), int(value["height"]))

		if True == value.has_key("default_image"):
			window.SetUpVisual(value["default_image"])
		if True == value.has_key("over_image"):
			window.SetOverVisual(value["over_image"])
		if True == value.has_key("down_image"):
			window.SetDownVisual(value["down_image"])
		if True == value.has_key("disable_image"):
			window.SetDisableVisual(value["disable_image"])

		if True == value.has_key("text"):
			if app.ENABLE_RESP_SYSTEM:
				if value.has_key("fontname"):
					window.SetFontName(value["fontname"])
			if True == value.has_key("text_height"):
				window.SetText(value["text"], value["text_height"])
			else:
				window.SetText(value["text"])

			if app.ENABLE_RESP_SYSTEM:
				if value.has_key("text_color"):
					window.SetTextColor(value["text_color"])

				if value.has_key("outline"):
					window.SetOutline(value["outline"])

			if value.has_key("text_color"):
				window.SetTextColor(value["text_color"])
		
		if True == value.has_key("text"):
			if True == value.has_key("text_height"):
				window.SetText(value["text"], value["text_height"])
			else:
				window.SetText(value["text"])

		if True == value.has_key("tooltip_text"):
			if True == value.has_key("tooltip_x") and True == value.has_key("tooltip_y"):
				window.SetToolTipText(value["tooltip_text"], int(value["tooltip_x"]), int(value["tooltip_y"]))
			else:
				window.SetToolTipText(value["tooltip_text"])

		if True == value.has_key("tooltip_text_new"):
			if True == value.has_key("tooltip_x") and True == value.has_key("tooltip_y"):
				window.SetToolTipTextNew(value["tooltip_text_new"], int(value["tooltip_x"]), int(value["tooltip_y"]))
			else:
				window.SetToolTipTextNew(value["tooltip_text_new"])

			if True == value.has_key("tooltip_text_color"):
				window.SetToolTipTextNewColor(value["tooltip_text_color"])
			else:
				window.SetToolTipTextNewColor(0xffffffff)

		self.LoadDefaultData(window, value, parentWindow)

		return True

	def LoadElementMark(self, window, value, parentWindow):


		self.LoadDefaultData(window, value, parentWindow)

		return True

	def LoadElementImage(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.IMAGE_KEY_LIST):
			return False

		if True == value.has_key("tooltip_text_new"):
			if True == value.has_key("tooltip_x") and True == value.has_key("tooltip_y"):
				window.SetToolTipTextNew(value["tooltip_text_new"], int(value["tooltip_x"]), int(value["tooltip_y"]))
			else:
				window.SetToolTipTextNew(value["tooltip_text_new"])

			if True == value.has_key("tooltip_text_color"):
				window.SetToolTipTextNewColor(value["tooltip_text_color"])
			else:
				window.SetToolTipTextNewColor(0xffffffff)

		window.LoadImage(value["image"])
		self.LoadDefaultData(window, value, parentWindow)

		return True

	def LoadElementAniImage(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.ANI_IMAGE_KEY_LIST):
			return False

		if True == value.has_key("delay"):
			window.SetDelay(value["delay"])

		for image in value["images"]:
			window.AppendImage(image)

		if value.has_key("width") and value.has_key("height"):
			window.SetSize(value["width"], value["height"])

		self.LoadDefaultData(window, value, parentWindow)

		return True

	def LoadElementExpandedImage(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.EXPANDED_IMAGE_KEY_LIST):
			return False

		window.LoadImage(value["image"])

		if True == value.has_key("x_origin") and True == value.has_key("y_origin"):
			window.SetOrigin(float(value["x_origin"]), float(value["y_origin"]))

		if True == value.has_key("x_scale") and True == value.has_key("y_scale"):
			window.SetScale(float(value["x_scale"]), float(value["y_scale"]))

		if True == value.has_key("rect"):
			RenderingRect = value["rect"]
			window.SetRenderingRect(RenderingRect[0], RenderingRect[1], RenderingRect[2], RenderingRect[3])

		if True == value.has_key("mode"):
			mode = value["mode"]
			if "MODULATE" == mode:
				window.SetRenderingMode(wndMgr.RENDERING_MODE_MODULATE)

		self.LoadDefaultData(window, value, parentWindow)

		return True

	def LoadElementSlot(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.SLOT_KEY_LIST):
			return False

		global_x = int(value["x"])
		global_y = int(value["y"])
		global_width = int(value["width"])
		global_height = int(value["height"])

		window.SetPosition(global_x, global_y)
		window.SetSize(global_width, global_height)
		window.Show()

		r = 1.0
		g = 1.0
		b = 1.0
		a = 1.0

		if True == value.has_key("image_r") and \
			True == value.has_key("image_g") and \
			True == value.has_key("image_b") and \
			True == value.has_key("image_a"):
			r = float(value["image_r"])
			g = float(value["image_g"])
			b = float(value["image_b"])
			a = float(value["image_a"])

		SLOT_ONE_KEY_LIST = ("index", "x", "y", "width", "height")

		for slot in value["slot"]:
			if True == self.CheckKeyList(value["name"] + " - one", slot, SLOT_ONE_KEY_LIST):
				wndMgr.AppendSlot(window.hWnd,
									int(slot["index"]),
									int(slot["x"]),
									int(slot["y"]),
									int(slot["width"]),
									int(slot["height"]))

		window.RefreshIndices()

		if True == value.has_key("image"):
			wndMgr.SetSlotBaseImage(window.hWnd,
									value["image"],
									r, g, b, a)

		return True

	def LoadElementCandidateList(self, window, value, parentWindow):
		if False == self.CheckKeyList(value["name"], value, self.CANDIDATE_LIST_KEY_LIST):
			return False

		window.SetPosition(int(value["x"]), int(value["y"]))
		window.SetItemSize(int(value["item_xsize"]), int(value["item_ysize"]))
		window.SetItemStep(int(value["item_step"]))
		window.Show()

		return True

	def LoadElementGridTable(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.GRID_TABLE_KEY_LIST):
			return False

		xBlank = 0
		yBlank = 0
		if True == value.has_key("x_blank"):
			xBlank = int(value["x_blank"])
		if True == value.has_key("y_blank"):
			yBlank = int(value["y_blank"])

		if localeInfo.IsARABIC():
			pass
		else:
			window.SetPosition(int(value["x"]), int(value["y"]))

		window.ArrangeSlot(	int(value["start_index"]),
							int(value["x_count"]),
							int(value["y_count"]),
							int(value["x_step"]),
							int(value["y_step"]),
							xBlank,
							yBlank)
		if True == value.has_key("image"):
			r = 1.0
			g = 1.0
			b = 1.0
			a = 1.0
			if True == value.has_key("image_r") and \
				True == value.has_key("image_g") and \
				True == value.has_key("image_b") and \
				True == value.has_key("image_a"):
				r = float(value["image_r"])
				g = float(value["image_g"])
				b = float(value["image_b"])
				a = float(value["image_a"])
			wndMgr.SetSlotBaseImage(window.hWnd, value["image"], r, g, b, a)

		if True == value.has_key("style"):
			if "select" == value["style"]:
				wndMgr.SetSlotStyle(window.hWnd, wndMgr.SLOT_STYLE_SELECT)
		if localeInfo.IsARABIC():
			self.LoadDefaultData(window, value, parentWindow)
		else:
			window.Show()

		return True

	def LoadElementText(self, window, value, parentWindow):

		if value.has_key("fontsize"):
			fontSize = value["fontsize"]

			if "LARGE" == fontSize:
				window.SetFontName(localeInfo.UI_DEF_FONT_LARGE)

		elif value.has_key("fontname"):
			fontName = value["fontname"]
			window.SetFontName(fontName)

		if value.has_key("text_horizontal_align"):
			if "left" == value["text_horizontal_align"]:
				window.SetHorizontalAlignLeft()
				window.SetWindowHorizontalAlignLeft()
			elif "center" == value["text_horizontal_align"]:
				window.SetHorizontalAlignCenter()
			elif "right" == value["text_horizontal_align"]:
				window.SetHorizontalAlignRight()
				window.SetWindowHorizontalAlignRight()

		if value.has_key("text_vertical_align"):
			if "top" == value["text_vertical_align"]:
				window.SetVerticalAlignTop()
			elif "center" == value["text_vertical_align"]:
				window.SetVerticalAlignCenter()
			elif "bottom" == value["text_vertical_align"]:
				window.SetVerticalAlignBottom()

		if value.has_key("all_align"):
			window.SetHorizontalAlignCenter()
			window.SetVerticalAlignCenter()
			window.SetWindowHorizontalAlignCenter()
			window.SetWindowVerticalAlignCenter()

		if value.has_key("r") and value.has_key("g") and value.has_key("b"):
			window.SetFontColor(float(value["r"]), float(value["g"]), float(value["b"]))
		elif value.has_key("color"):
			window.SetPackedFontColor(value["color"])
		else:
			window.SetFontColor(0.8549, 0.8549, 0.8549)

		if value.has_key("outline"):
			if value["outline"]:
				window.SetOutline()
		if True == value.has_key("text"):
			window.SetText(value["text"])

		self.LoadDefaultData(window, value, parentWindow)

		return True

	def LoadElementEditLine(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.EDIT_LINE_KEY_LIST):
			return False


		if value.has_key("secret_flag"):
			window.SetSecret(value["secret_flag"])
		if value.has_key("with_codepage"):
			if value["with_codepage"]:
				window.bCodePage = True
		if value.has_key("only_number"):
			if value["only_number"]:
				window.SetNumberMode()
		if value.has_key("enable_codepage"):
			window.SetIMEFlag(value["enable_codepage"])
		if value.has_key("enable_ime"):
			window.SetIMEFlag(value["enable_ime"])
		if value.has_key("limit_width"):
			window.SetLimitWidth(value["limit_width"])
		if value.has_key("multi_line"):
			if value["multi_line"]:
				window.SetMultiLine()
		if value.has_key("info_msg"):
			window.SetInfoMessage(value["info_msg"])
		if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
			if value.has_key("only_currency"):
				if value["only_currency"]:
					window.SetCurrencyMode()
		window.SetMax(int(value["input_limit"]))
		window.SetSize(int(value["width"]), int(value["height"]))
		self.LoadElementText(window, value, parentWindow)

		return True

	def LoadElementRenderTarget(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.RENDER_TARGET_KEY_LIST):
			return False

		window.SetSize(value["width"], value["height"])
		
		if True == value.has_key("style"):
			for style in value["style"]:
				window.AddFlag(style)
				
		self.LoadDefaultData(window, value, parentWindow)
		
		if value.has_key("index"):
			window.SetRenderTarget(int(value["index"]))

		return True

	def LoadElementTitleBar(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.TITLE_BAR_KEY_LIST):
			return False

		window.MakeTitleBar(int(value["width"]), value.get("color", "red"))
		self.LoadDefaultData(window, value, parentWindow)

		return True

	def LoadElementHorizontalBar(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.HORIZONTAL_BAR_KEY_LIST):
			return False

		window.Create(int(value["width"]))
		self.LoadDefaultData(window, value, parentWindow)

		return True

	def LoadElementBoard(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.BOARD_KEY_LIST):
			return False

		window.SetSize(int(value["width"]), int(value["height"]))
		self.LoadDefaultData(window, value, parentWindow)

		return True

	def LoadElementBoardWithTitleBar(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.BOARD_WITH_TITLEBAR_KEY_LIST):
			return False

		window.SetSize(int(value["width"]), int(value["height"]))
		window.SetTitleName(value["title"])
		self.LoadDefaultData(window, value, parentWindow)

		return True

	def LoadElementBorderA(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.BOARD_KEY_LIST):
			return False

		window.SetSize(int(value["width"]), int(value["height"]))
		self.LoadDefaultData(window, value, parentWindow)

		return True

	def LoadElementThinBoard(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.BOARD_KEY_LIST):
			return False

		window.SetSize(int(value["width"]), int(value["height"]))
		self.LoadDefaultData(window, value, parentWindow)

		return True

	if app.ENABLE_QUEST_RENEWAL:
		def LoadElementSubTitleBar(self, window, value, parentWindow):
			if False == self.CheckKeyList(value["name"], value, self.SUB_TITLE_BAR_KEY_LIST):
				return False

			window.MakeSubTitleBar(int(value["width"]), value.get("color", "red"))
			self.LoadElementButton(window, value, parentWindow)
			window.Show()
			return True

		def LoadElementListBar(self, window, value, parentWindow):
			if False == self.CheckKeyList(value["name"], value, self.LIST_BAR_KEY_LIST):
				return False

			window.MakeListBar(int(value["width"]), value.get("color", "red"))
			self.LoadElementButton(window, value, parentWindow)

			return True
	def LoadElementBox(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.BOX_KEY_LIST):
			return False

		if True == value.has_key("color"):
			window.SetColor(value["color"])

		window.SetSize(int(value["width"]), int(value["height"]))
		self.LoadDefaultData(window, value, parentWindow)

		return True

	def LoadElementBar(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.BAR_KEY_LIST):
			return False

		if True == value.has_key("color"):
			window.SetColor(value["color"])

		window.SetSize(int(value["width"]), int(value["height"]))
		self.LoadDefaultData(window, value, parentWindow)

		return True

	def LoadElementLine(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.LINE_KEY_LIST):
			return False

		if True == value.has_key("color"):
			window.SetColor(value["color"])

		window.SetSize(int(value["width"]), int(value["height"]))
		self.LoadDefaultData(window, value, parentWindow)

		return True

	def LoadElementSlotBar(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.SLOTBAR_KEY_LIST):
			return False

		window.SetSize(int(value["width"]), int(value["height"]))
		self.LoadDefaultData(window, value, parentWindow)

		return True

	def LoadElementGauge(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.GAUGE_KEY_LIST):
			return False

		window.MakeGauge(value["width"], value["color"])
		self.LoadDefaultData(window, value, parentWindow)

		return True

	def LoadElementScrollBar(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.SCROLLBAR_KEY_LIST):
			return False

		window.SetScrollBarSize(value["size"])
		if app.__BL_SMOOTH_SCROLL__:
			if value.has_key("smooth"):
				if value["smooth"]:
					window.EnableSmoothMode()
					
		self.LoadDefaultData(window, value, parentWindow)

		return True

	def LoadElementSliderBar(self, window, value, parentWindow):

		self.LoadDefaultData(window, value, parentWindow)

		return True

	def LoadElementListBox(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.LIST_BOX_KEY_LIST):
			return False

		if value.has_key("item_align"):
			window.SetTextCenterAlign(value["item_align"])

		window.SetSize(value["width"], value["height"])
		self.LoadDefaultData(window, value, parentWindow)

		return True

	def LoadElementListBox2(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.LIST_BOX_KEY_LIST):
			return False

		window.SetRowCount(value.get("row_count", 10))
		window.SetSize(value["width"], value["height"])
		self.LoadDefaultData(window, value, parentWindow)

		if value.has_key("item_align"):
			window.SetTextCenterAlign(value["item_align"])

		return True
	def LoadElementListBoxEx(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.LIST_BOX_KEY_LIST):
			return False

		window.SetSize(value["width"], value["height"])
		self.LoadDefaultData(window, value, parentWindow)

		if value.has_key("itemsize_x") and value.has_key("itemsize_y"):
			window.SetItemSize(int(value["itemsize_x"]), int(value["itemsize_y"]))

		if value.has_key("itemstep"):
			window.SetItemStep(int(value["itemstep"]))

		if value.has_key("viewcount"):
			window.SetViewItemCount(int(value["viewcount"]))

		return True

	def LoadElementModernScrollBar(self, window, value, parentWindow):
		if not self.CheckKeyList(value["name"], value, self.MODERN_SCROLLBAR_KEY_LIST):
			return False
		window.SetScrollBarSize(value["size"])
		if value.has_key("content_height"):
			window.SetContentHeight(value["content_height"])
		if value.has_key("width"):
			window.SetWidth(value["width"])
		if value.has_key("scroll_step"):
			window.SetScrollStep(value["scroll_step"])
		if app.__BL_SMOOTH_SCROLL__:
			if value.has_key("smooth"):
				if value["smooth"]:
					window.EnableSmoothMode()
		self.LoadDefaultData(window, value, parentWindow)
		return True

	def LoadElementUnfoldListBox(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.LIST_BOX_KEY_LIST):
			return False

		if value.has_key("x_dif"):
			window.SetXDif(value["x_dif"])
		if value.has_key("y_dif"):
			window.SetYDif(value["y_dif"])

		window.SetSize(value["width"], value["height"])
		self.LoadDefaultData(window, value, parentWindow)

		return True

	def LoadElementCheckBox(self, window, value, parentWindow):

		if value.has_key("text"):
			window.SetText(value["text"])

		if value.has_key("checked") and value["checked"] == True:
			window.SetChecked(window.STATE_SELECTED)

		if value.has_key("disabled") and value["disabled"] == True:
			window.Disable()

		self.LoadDefaultData(window, value, parentWindow)

	def LoadElementBoxedBoard(self, window, value, parentWindow):
		if not self.CheckKeyList(value["name"], value, self.WINDOW_KEY_LIST):
			return False
		
		window.SetSize(value["width"], value["height"])
		
		if value.has_key("border_color"):
			window.SetBorderColor(value["border_color"])
		
		if value.has_key("border_size"):
			window.SetBorderSize(value["border_size"])
		
		if value.has_key("base_color"):
			window.SetBaseColor(value["base_color"])
		
		self.LoadDefaultData(window, value, parentWindow)
		return True

if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
	class DynamicListBox(Window):

		def GetSelectedItemText(self):
			return self.textDict.get(self.selectedLine, "")

		TEMPORARY_PLACE = 3

		def __init__(self, layer = "UI"):
			Window.__init__(self, layer)
			self.overLine = -1
			self.selectedLine = -1
			self.width = 0
			self.height = 0
			self.stepSize = 17
			self.basePos = 0
			self.showLineCount = 0
			self.visibleLineCount = 10
			self.currentPosition = 0
			self.itemCenterAlign = TRUE
			self.itemList = []
			self.keyDict = {}
			self.textDict = {}
			self.event = lambda *arg: None
			
		def __del__(self):
			Window.__del__(self)

		def SetWidth(self, width):
			self.SetSize(width, self.height)

		def SetSize(self, width, height):
			Window.SetSize(self, width, height)
			self.width = width
			self.height = height

		def SetTextCenterAlign(self, flag):
			self.itemCenterAlign = flag

		def SetBasePos(self, pos):
			self.basePos = pos
			self._LocateItem()

		def ClearItem(self):
			self.keyDict = {}
			self.textDict = {}
			self.itemList = []
			self.overLine = -1
			self.selectedLine = -1

		def InsertItem(self, number, text):
			self.keyDict[len(self.itemList)] = number
			self.textDict[len(self.itemList)] = text

			textLine = TextLine()
			textLine.SetParent(self)
			textLine.SetText(text)
			textLine.Hide()

			if self.itemCenterAlign:
				textLine.SetWindowHorizontalAlignCenter()
				textLine.SetHorizontalAlignCenter()

			self.itemList.append(textLine)

			self._LocateItem()

		def ChangeItem(self, number, text):
			for key, value in self.keyDict.items():
				if value == number:
					self.textDict[key] = text

					if number < len(self.itemList):
						self.itemList[key].SetText(text)

					return

		def LocateItem(self):
			self._LocateItem()
			
		def SetVisibleLineCount(self, count):
			self.visibleLineCount = count
			
		def GetVisibleLineCount(self):
			return self.visibleLineCount
			
		def GetVisibleHeight(self):
			return self.visibleLineCount * self.stepSize

		def _LocateItem(self):
			yPos = 0
			self.showLineCount = 0

			i = 0
			for textLine in self.itemList:
				if i >= self.currentPosition and i < (self.currentPosition + self.GetVisibleLineCount()):
					if localeInfo.IsARABIC():
						w, h = textLine.GetTextSize()
						textLine.SetPosition(w+10, yPos + 3)
					else:
						textLine.SetPosition(0, yPos + 3)
					
					textLine.Show()
					yPos += self.stepSize
				else:
					textLine.Hide()

				self.showLineCount += 1
				i += 1

		def ArrangeItem(self):
			self.SetSize(self.width, len(self.itemList) * self.stepSize)
			self._LocateItem()

		def GetViewItemCount(self):
			return int(self.GetHeight() / self.stepSize)

		def GetItemCount(self):
			return len(self.itemList)

		def SetEvent(self, event):
			self.event = event

		def SelectItem(self, line):

			if not self.keyDict.has_key(line):
				return

			if line == self.selectedLine:
				return

			self.selectedLine = line + self.currentPosition
			self.event(self.keyDict.get(self.selectedLine, 0), self.textDict.get(self.selectedLine, "None"))

		def GetSelectedItem(self):
			return self.keyDict.get(self.selectedLine, 0)

		def GetSelectedItemText(self):
			return self.itemList[self.selectedLine].GetText()

		def OnMouseLeftButtonDown(self):
			if self.overLine < 0:
				return

		def OnMouseLeftButtonUp(self):
			if self.overLine >= 0:
				self.SelectItem(self.overLine+self.basePos)
				
		def OnDown(self):
			if self.GetItemCount() > self.GetVisibleLineCount():
				self.currentPosition = min(self.GetItemCount() - self.GetVisibleLineCount(), self.currentPosition + 1)

				self._LocateItem()
			
		def OnUp(self):
			if self.GetItemCount() > self.GetVisibleLineCount():
				self.currentPosition = max(0, self.currentPosition - 1)

				self._LocateItem()
			
		def OnUpdate(self):
			self.overLine = -1

			if self.IsIn():
				x, y = self.GetGlobalPosition()
				height = self.GetHeight()
				xMouse, yMouse = wndMgr.GetMousePosition()

				if yMouse - y < height - 1:
					self.overLine = (yMouse - y) / self.stepSize

					if self.overLine < 0:
						self.overLine = -1
					if self.overLine >= len(self.itemList):
						self.overLine = -1

		def OnRender(self):
			xRender, yRender = self.GetGlobalPosition()
			yRender -= self.TEMPORARY_PLACE
			widthRender = self.width
			heightRender = self.height + self.TEMPORARY_PLACE*2
		
			if -1 != self.overLine:
				grp.SetColor(HALF_WHITE_COLOR)
				grp.RenderBar(xRender + 2, yRender + self.overLine*self.stepSize + 4, self.width - 3, self.stepSize)				

			if -1 != self.selectedLine:
				if self.selectedLine >= self.currentPosition and self.selectedLine < (self.currentPosition + self.GetVisibleLineCount()):
					if self.selectedLine - self.basePos < self.showLineCount:
						grp.SetColor(grp.GenerateColor(255.0 / 255.0, 150.0 / 255.0, 95.0 / 255.0, 0.2))
						grp.RenderBar(xRender + 2, yRender + (self.selectedLine-self.basePos-self.currentPosition) * self.stepSize + 4, self.width - 3, self.stepSize)

			max_count = min(self.GetVisibleLineCount(), self.GetItemCount())
			for i in range(1, max_count):
				grp.SetColor(WHITE_COLOR)
				grp.RenderBar(xRender, yRender + i*self.stepSize + 4, self.width, 1)		

	class DynamicComboBoxImage(Window):
		class ListBoxWithBoard(DynamicListBox):

			BG_COLOR = grp.GenerateColor(33.0 / 255.0, 33.0 / 255.0, 33.0 / 255.0, 1.0)

			def __init__(self, layer):
				DynamicListBox.__init__(self, layer)

			def OnRender(self):
				xRender, yRender = self.GetGlobalPosition()
				yRender -= self.TEMPORARY_PLACE
				widthRender = self.width
				heightRender = self.height + self.TEMPORARY_PLACE*2
				grp.SetColor(BACKGROUND_COLOR)
				grp.RenderBar(xRender, yRender, widthRender, heightRender)
				grp.SetColor(WHITE_COLOR)
				grp.RenderBox(xRender, yRender, widthRender, heightRender)
				DynamicListBox.OnRender(self)

		def __init__(self, parent, name, x, y):
			self.isSelected = False
			self.isOver = False
			self.isListOpened = False
			self.event = lambda *arg: None
			self.enable = True
			self.imagebox = None
			self.listBox = None
			self.titleText = None

			Window.__init__(self)
			
			image = ExpandedImageBox()
			image.SetParent(parent)
			image.LoadImage(name)
			image.SetPosition(x, y)
			image.Hide()
			self.imagebox = image
			
			self.x = x + 1
			self.y = y + 1
			self.width = self.imagebox.GetWidth() - 3
			self.height = self.imagebox.GetHeight() - 3
			self.SetParent(parent)

			self.textLine = MakeTextLine(self)
			self.textLine.SetText(localeInfo.UI_ITEM)
			
			self.listBox = self.ListBoxWithBoard("TOP_MOST")
			self.listBox.SetPickAlways()
			self.listBox.SetParent(self)
			self.listBox.SetVisibleLineCount(12)
			self.listBox.SetEvent(__mem_func__(self.OnSelectItem))
			self.listBox.Hide()
			
			Window.SetPosition(self, self.x, self.y)
			Window.SetSize(self, self.width, self.height)
			self.textLine.UpdateRect()
			self.listBox.SetPosition(0, self.height + 5)
			self.__ArrangeListBox()
			
		def __del__(self):
			Window.__del__(self)

		def Hide(self):
			Window.Hide(self)
			
			if self.listBox:
				self.CloseListBox()

			if self.imagebox:
				self.imagebox.Hide()

		def Show(self):
			Window.Show(self)

			if self.imagebox:
				self.imagebox.Show()

		def Destroy(self):
			self.textLine = None
			self.listBox = None
			self.imagebox = None

		def Clear(self):
			self.SelectItem(0)

		def SetPosition(self, x, y):
			Window.SetPosition(self, x, y)
			self.imagebox.SetPosition(x, y)
			self.x = x
			self.y = y
			self.__ArrangeListBox()

		def SetSize(self, width, height):
			Window.SetSize(self, width, height)
			self.width = width
			self.height = height
			self.textLine.UpdateRect()
			self.__ArrangeListBox()
			
		def SetImageScale(self, scale_x, scale_y):
			self.imagebox.SetScale(scale_x, scale_y)
			self.width = self.imagebox.GetWidth() - 3
			self.height = self.imagebox.GetHeight() - 3
			Window.SetSize(self, self.width, self.height)
			self.textLine.UpdateRect()
			self.__ArrangeListBox()
			
		def __ArrangeListBox(self):
			self.listBox.SetPosition(0, self.height + 5)

			if self.listBox.GetItemCount() <= self.listBox.GetVisibleLineCount():
				self.listBox.SetSize(self.width, self.listBox.GetHeight())
			else:
				self.listBox.SetSize(self.width, self.listBox.GetVisibleHeight())

		def Enable(self):
			self.enable = True

		def Disable(self):
			self.enable = False
			self.CloseListBox()

		def SetEvent(self, event):
			self.event = event

		def SetDefaultTitle(self, title):
			self.titleText = title

			self.SetCurrentItem(self.titleText)

		def UseDefaultTitle(self):
			self.SetCurrentItem(self.titleText)
			
		def SetTitle(self, title):
			self.SetCurrentItem(title)

		def GetTitle(self):
			return self.titleText

		def ClearItem(self):
			self.CloseListBox()
			self.listBox.ClearItem()

		def InsertItem(self, index, name):
			self.listBox.InsertItem(index, name)
			self.listBox.ArrangeItem()

		def SetCurrentItem(self, text):
			self.textLine.SetText(text)

		def GetSelectedItemText(self):
			return self.listBox.GetSelectedItemText()

		def SelectItem(self, key):
			self.listBox.SelectItem(key)

		def OnSelectItem(self, index, name):
			self.CloseListBox()
			self.event(index)

		def CloseListBox(self):
			self.isListOpened = False
			self.listBox.Hide()

		def OnMouseLeftButtonDown(self):
			if not self.enable:
				return

			self.isSelected = True

		def OnMouseLeftButtonUp(self):
			if not self.enable:
				return
			
			self.isSelected = False
			
			if self.isListOpened:
				self.CloseListBox()
			else:
				if self.listBox.GetItemCount() > 0:
					self.isListOpened = True
					self.listBox.Show()
					self.listBox.SetTop()
					self.__ArrangeListBox()
					
		def OnMouseWheel(self, nLen):
			if nLen > 0:
				self.listBox.OnUp()
				return True
				
			elif nLen < 0:
				self.listBox.OnDown()
				return True
				
			return False

		def OnUpdate(self):
			if not self.enable:
				return

			if self.IsIn():
				self.isOver = True
			else:
				self.isOver = False

		def OnRender(self):
			self.x, self.y = self.GetGlobalPosition()
			xRender = self.x
			yRender = self.y
			widthRender = self.width
			heightRender = self.height
			
			if self.isOver:
				grp.SetColor(HALF_WHITE_COLOR)
				grp.RenderBar(xRender + 2, yRender + 3, self.width - 3, heightRender - 5)
				
				if self.isSelected:
					grp.SetColor(WHITE_COLOR)
					grp.RenderBar(xRender + 2, yRender + 3, self.width - 3, heightRender - 5)
					
	class ShopDecoThinboard(Window):
		DEFAULT_VALUE = 16
		CORNER_WIDTH = 48
		CORNER_HEIGHT = 32
		LINE_WIDTH = 16
		LINE_HEIGHT = 32
		
		DEFAULT_CORNER_WIDTH = 16
		DEFAULT_CORNER_HEIGHT = 16
		DEFAULT_LINE_WIDTH = 16
		DEFAULT_LINE_HEIGHT = 16
		DEFAULT_BOARD_COLOR = grp.GenerateColor(0.0, 0.0, 0.0, 0.51)

		LT = 0
		LB = 1
		RT = 2
		RB = 3
		L = 0
		R = 1
		T = 2
		B = 3

		def __init__(self, type = 0, layer = "UI"):
			Window.__init__(self, layer)
			
			self.type = type
			
			base = Bar()
			base.SetParent(self)
			base.AddFlag("attach")
			base.AddFlag("not_pick")
			base.SetPosition(self.DEFAULT_CORNER_WIDTH, self.DEFAULT_CORNER_HEIGHT)
			base.SetColor(self.DEFAULT_BOARD_COLOR)
			base.Hide()
			self.base = base
		
			self.width = 190
			self.height = 32
			
			self.SetStyle(type)
			self.Refresh()
				
		def __del__(self):
			Window.__del__(self)
			
		def GetStyle(self, type):
			import privateShop

			(name, path, text_color) = privateShop.GetTitleDeco(type)
			
			CornerFileNames = [ path + "_"+dir+".tga" for dir in ["left_top","left_bottom","right_top","right_bottom"] ]
			LineFileNames = [ path + "_"+dir+".tga" for dir in ["left","right","top","bottom"] ]
			
			return CornerFileNames, LineFileNames
			
		def SetStyle(self, type):
			self.type = type
			
			CornerFileNames, LineFileNames = self.GetStyle(type)
			
			if CornerFileNames == None or LineFileNames == None :
				return

			self.Corners = []
			for fileName in CornerFileNames:
				Corner = ExpandedImageBox()
				Corner.AddFlag("attach")
				Corner.AddFlag("not_pick")
				Corner.LoadImage(fileName)
				Corner.SetParent(self)
				Corner.SetPosition(0, 0)
				Corner.Show()
				self.Corners.append(Corner)

			self.Lines = []
			for fileName in LineFileNames:
				Line = ExpandedImageBox()
				Line.AddFlag("attach")
				Line.AddFlag("not_pick")
				Line.LoadImage(fileName)
				Line.SetParent(self)
				Line.SetPosition(0, 0)
				Line.Show()
				self.Lines.append(Line)
				
			if self.type == 0:
				self.base.Show()
			else:
				self.base.Hide()
				
			self.Refresh()

		def SetBoardSize(self, width, height):
			if self.type == 0:
				self.width = max(self.DEFAULT_CORNER_WIDTH*2, width)
				self.height = max(self.DEFAULT_CORNER_HEIGHT*2, height)
				
			else:
				self.width = max(self.DEFAULT_VALUE*2, width)
				self.height = max(self.DEFAULT_VALUE*2, height)
				
			Window.SetSize(self, self.width, self.height)
			self.Refresh()

		def Refresh(self):
			if self.type == 0:
		
				self.Corners[self.LB].SetPosition(0, self.height - self.DEFAULT_CORNER_HEIGHT)
				self.Corners[self.RT].SetPosition(self.width - self.DEFAULT_CORNER_WIDTH, 0)
				self.Corners[self.RB].SetPosition(self.width - self.DEFAULT_CORNER_WIDTH, self.height - self.DEFAULT_CORNER_HEIGHT)
				
				self.Lines[self.L].SetPosition(0, self.DEFAULT_CORNER_HEIGHT)
				self.Lines[self.T].SetPosition(self.DEFAULT_CORNER_WIDTH, 0)
				self.Lines[self.R].SetPosition(self.width - self.DEFAULT_CORNER_WIDTH, self.DEFAULT_CORNER_HEIGHT)
				self.Lines[self.B].SetPosition(self.DEFAULT_CORNER_HEIGHT, self.height - self.DEFAULT_CORNER_HEIGHT)

				verticalShowingPercentage = float((self.height - self.DEFAULT_CORNER_HEIGHT*2) - self.DEFAULT_LINE_HEIGHT) / self.DEFAULT_LINE_HEIGHT
				horizontalShowingPercentage = float((self.width - self.DEFAULT_CORNER_WIDTH*2) - self.DEFAULT_LINE_WIDTH) / self.DEFAULT_LINE_WIDTH
				
				self.Lines[self.L].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
				self.Lines[self.R].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
				self.Lines[self.T].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
				self.Lines[self.B].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
				self.base.SetSize(self.width - self.DEFAULT_CORNER_WIDTH*2, self.height - self.DEFAULT_CORNER_HEIGHT*2)
				self.base.Show()
		
			else:
				self.Corners[self.LT].SetPosition(-self.CORNER_WIDTH + self.DEFAULT_VALUE, -self.CORNER_HEIGHT + self.DEFAULT_VALUE)
				self.Corners[self.LB].SetPosition(-self.CORNER_WIDTH + self.DEFAULT_VALUE, self.height - self.CORNER_HEIGHT + self.DEFAULT_VALUE)
				
				self.Corners[self.RT].SetPosition(self.width - self.DEFAULT_VALUE, -self.CORNER_HEIGHT + self.DEFAULT_VALUE)
				self.Corners[self.RB].SetPosition(self.width - self.DEFAULT_VALUE, self.height - self.CORNER_HEIGHT + self.DEFAULT_VALUE)
				
				self.Lines[self.L].SetPosition(0, self.DEFAULT_VALUE)
				self.Lines[self.R].SetPosition(self.width - self.DEFAULT_VALUE, self.DEFAULT_VALUE)
				self.Lines[self.B].SetPosition(self.DEFAULT_VALUE, self.height - self.LINE_HEIGHT + self.DEFAULT_VALUE)
				self.Lines[self.T].SetPosition(self.DEFAULT_VALUE, -self.LINE_HEIGHT + self.DEFAULT_VALUE)
				
				verticalShowingPercentage = float((self.height - self.DEFAULT_VALUE*2) - self.DEFAULT_VALUE) / self.DEFAULT_VALUE
				horizontalShowingPercentage = float((self.width - self.DEFAULT_VALUE*2) - self.DEFAULT_VALUE) / self.DEFAULT_VALUE
				
				self.Lines[self.L].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
				self.Lines[self.R].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
				self.Lines[self.T].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
				self.Lines[self.B].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
				self.base.Hide()

		def ShowInternal(self):
			for wnd in self.Lines:
				wnd.Show()
			for wnd in self.Corners:
				wnd.Show()

		def HideInternal(self):
			for wnd in self.Lines:
				wnd.Hide()
			for wnd in self.Corners:
				wnd.Hide()

class ReadingWnd(Bar):

	def __init__(self):
		Bar.__init__(self,"TOP_MOST")

		self.__BuildText()
		self.SetSize(80, 19)
		self.Show()

	def __del__(self):
		Bar.__del__(self)

	def __BuildText(self):
		self.text = TextLine()
		self.text.SetParent(self)
		self.text.SetPosition(4, 3)
		self.text.Show()

	def SetText(self, text):
		self.text.SetText(text)

	def SetReadingPosition(self, x, y):
		xPos = x + 2
		yPos = y  - self.GetHeight() - 2
		self.SetPosition(xPos, yPos)

	def SetTextColor(self, color):
		self.text.SetPackedFontColor(color)

def MakeSlotBar(parent, x, y, width, height):
	slotBar = SlotBar()
	slotBar.SetParent(parent)
	slotBar.SetSize(width, height)
	slotBar.SetPosition(x, y)
	slotBar.Show()
	return slotBar

def MakeText(parent, textlineText, x, y, color):
	textline = TextLine()
	if parent != None:
		textline.SetParent(parent)
	textline.SetPosition(x, y)
	if color != None:
		textline.SetFontColor(color[0], color[1], color[2])
	textline.SetText(textlineText)
	textline.Show()
	return textline

def MakeBoardWithTitleBar(parent, flag, title, closeEvent, width, height):
	board = BoardWithTitleBar()
	board.SetParent(parent)
	board.SetSize(width, height)
	board.AddFlag(flag)
	board.SetTitleName(title)
	board.SetCloseEvent(closeEvent)
	board.Show()
	return board

def MakeImageBox(parent, name, x, y):
	image = ImageBox()
	image.SetParent(parent)
	image.LoadImage(name)
	image.SetPosition(x, y)
	image.Show()
	return image

def AddTextLine(parent, x, y, text, outline = 0):
	textLine = TextLine()
	textLine.SetParent(parent)
	textLine.SetPosition(x, y)
	if outline != 0:
		textLine.SetOutline()
	textLine.SetText(text)
	textLine.Show()
	return textLine

def MakeNewTextLine(parent, horizontalAlign = True, verticalAlgin = True, x = 0, y = 0):
	textLine = TextLine()
	textLine.SetParent(parent)
	if horizontalAlign == True:
		textLine.SetWindowHorizontalAlignCenter()
	if verticalAlgin == True:
		textLine.SetWindowVerticalAlignCenter()
	textLine.SetHorizontalAlignCenter()
	textLine.SetVerticalAlignCenter()
	if app.WJ_MULTI_TEXTLINE:
		textLine.DisableEnterToken()
	if x != 0 and y != 0:
		textLine.SetPosition(x, y)
	textLine.Show()
	return textLine

def MakeExpandedImageBox(parent, name, x, y, flag = ""):
	image = ExpandedImageBox()
	image.SetParent(parent)
	image.LoadImage(name)
	image.SetPosition(x, y)
	if flag != "":
		image.AddFlag(flag)
	image.Show()
	return image

def MakeTextLine(parent):
	textLine = TextLine()
	textLine.SetParent(parent)
	textLine.SetWindowHorizontalAlignCenter()
	textLine.SetWindowVerticalAlignCenter()
	textLine.SetHorizontalAlignCenter()
	textLine.SetVerticalAlignCenter()
	textLine.Show()
	return textLine

def MakeTextLine2(parent, x = 0, y = 0, text = ""):
	textLine = TextLine()
	textLine.SetParent(parent)
	textLine.SetPosition(x, y)
	textLine.SetText(text)
	textLine.Show()
	return textLine
	
def MakeTextLineNew(parent, x, y, text):
	textLine = TextLine()
	textLine.SetParent(parent)
	textLine.SetPosition(x, y)
	textLine.SetText(text)
	textLine.Show()
	return textLine


def MakeEditline(parent, x, y, width, height, maxLength, limitWidth):
	editLine = EditLine()
	editLine.SetParent(parent)
	editLine.SetPosition(x, y)
	editLine.SetSize(width, height)
	editLine.SetMax(maxLength)
	editLine.SetLimitWidth(limitWidth)
	editLine.SetOutline()
	editLine.Show()
	return editLine
	
def MakeButton(parent, x, y, tooltipText, path, up, over, down, sameImage = False):
	button = Button()
	button.SetParent(parent)
	button.SetPosition(x, y)
	if sameImage:
		button.SetUpVisual(path + up)
		button.SetOverVisual(path + up)
		button.SetDownVisual(path + up)
	else:
		button.SetUpVisual(path + up)
		button.SetOverVisual(path + over)
		button.SetDownVisual(path + down)
	button.SetToolTipText(tooltipText)
	button.Show()
	return button

def MakeRadioButton(parent, x, y, path, up, over, down):
	button = RadioButton()
	button.SetParent(parent)
	button.SetPosition(x, y)
	button.SetUpVisual(path + up)
	button.SetOverVisual(path + over)
	button.SetDownVisual(path + down)
	button.Show()
	return button
	
def RenderRoundBox(x, y, width, height, color):
	grp.SetColor(color)
	grp.RenderLine(x+2, y, width-3, 0)
	grp.RenderLine(x+2, y+height, width-3, 0)
	grp.RenderLine(x, y+2, 0, height-4)
	grp.RenderLine(x+width, y+1, 0, height-3)
	grp.RenderLine(x, y+2, 2, -2)
	grp.RenderLine(x, y+height-2, 2, 2)
	grp.RenderLine(x+width-2, y, 2, 2)
	grp.RenderLine(x+width-2, y+height, 2, -2)

def GenerateColor(r, g, b):
	r = float(r) / 255.0
	g = float(g) / 255.0
	b = float(b) / 255.0
	return grp.GenerateColor(r, g, b, 1.0)

def EnablePaste(flag):
	ime.EnablePaste(flag)

def GetHyperlink():
	return wndMgr.GetHyperlink()

RegisterToolTipWindow("TEXT", TextLine)

class RenderTarget(Window):

	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)
		if app.ENABLE_WIKI:
			self.eventDict={}
			self.eventFunc = {"mouse_click" : None, "mouse_over_in" : None, "mouse_over_out" : None}
			self.eventArgs = {"mouse_click" : None, "mouse_over_in" : None, "mouse_over_out" : None}
		
		self.number = -1

	def Destroy(self):
		if app.ENABLE_WIKI:
			self.eventDict={}
			self.eventFunc = 0
			self.eventArgs = 0
			
	def __del__(self):
		Window.__del__(self)

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterRenderTarget(self, layer)
		
	def SetRenderTarget(self, number):
		self.number = number
		wndMgr.SetRenderTarget(self.hWnd, self.number)
		
	if app.ENABLE_WIKI:
		def SetRenderingRect(self, left, top, right, bottom):
			wndMgr.SetRenderingRect(self.hWnd, left, top, right, bottom)
		def SetEvent(self, func, *args):
			result = self.eventFunc.has_key(args[0])
			if result:
				self.eventFunc[args[0]] = func
				self.eventArgs[args[0]] = args
			else:
				#print "[ERROR] ui.py SetEvent, Can`t Find has_key : %s" % args[0]
				return
		def OnMouseLeftButtonUp(self) :
			if self.eventFunc["mouse_click"] :
				apply(self.eventFunc["mouse_click"], self.eventArgs["mouse_click"])
		def OnMouseOverIn(self):
			try:
				if self.eventFunc["mouse_over_in"]:
					if self.eventArgs:
						apply(self.eventFunc["mouse_over_in"], self.eventArgs["mouse_over_in"])
					else:
						self.eventFunc["mouse_over_in"]()
			except KeyError:
				pass
		def OnMouseOverOut(self):
			try:
				if self.eventFunc["mouse_over_out"]:
					if self.eventArgs:
						apply(self.eventFunc["mouse_over_out"], self.eventArgs["mouse_over_out"])
					else:
						self.eventFunc["mouse_over_out"]()
			except KeyError:
				pass

def calculateRect(curValue, maxValue):
	try:
		return -1.0 + float(curValue) / float(maxValue)
	except:
		return 0.0

class ComboBoxImage(Window):
	class ListBoxWithBoard(ListBox):
		def __init__(self, layer):
			ListBox.__init__(self, layer)
		def OnRender(self):
			xRender, yRender = self.GetGlobalPosition()
			yRender -= self.TEMPORARY_PLACE
			widthRender = self.width
			heightRender = self.height + self.TEMPORARY_PLACE*2
			grp.SetColor(BACKGROUND_COLOR)
			grp.RenderBar(xRender, yRender, widthRender, heightRender)
			grp.SetColor(DARK_COLOR)
			grp.RenderLine(xRender, yRender, widthRender, 0)
			grp.RenderLine(xRender, yRender, 0, heightRender)
			ListBox.OnRender(self)
	
	def __init__(self, parent, name, x, y, dropdownParent=None):
		Window.__init__(self)
		self.isSelected = False
		self.isOver = False
		self.isListOpened = False
		self.event = lambda *arg: None
		self.enable = True
		self.imagebox = None
		self.dropdownParent = dropdownParent
		
		image = ImageBox()
		image.SetParent(parent)
		image.LoadImage(name)
		image.SetPosition(x, y)
		image.Show()
		self.imagebox = image
		
		# Adjust these values to match your image's inner area
		self.borderLeft = 0    # Left border size
		self.borderTop = 0     # Top border size
		self.borderRight = 0   # Right border size
		self.borderBottom = 0  # Bottom border size
		
		self.x = x + self.borderLeft
		self.y = y + self.borderTop
		self.width = self.imagebox.GetWidth() - self.borderLeft - self.borderRight
		self.height = self.imagebox.GetHeight() - self.borderTop - self.borderBottom
		
		self.SetParent(parent)
		self.textLine = MakeTextLine(self)
		self.textLine.SetText(localeInfo.UI_ITEM)
		
		self.listBox = self.ListBoxWithBoard("TOP_MOST")
		self.listBox.SetPickAlways()
		self.listBox.SetParent(self.dropdownParent if self.dropdownParent else self)

		self.listBox.SetEvent(__mem_func__(self.OnSelectItem))
		self.listBox.Hide()

		Window.SetPosition(self, self.x, self.y)
		Window.SetSize(self, self.width, self.height)
		self.textLine.UpdateRect()
		self.__ArrangeListBox()
		self.Show()
		
	def __del__(self):
		Window.__del__(self)
	
	def Destroy(self):
		self.textLine = None
		self.listBox = None
		self.imagebox = None
	
	def GetItemIndex(self, item):
		return self.listBox.GetItemIndex(item)
	
	def GetCurrentItem(self):
		return self.textLine.GetText()
	
	def SetPosition(self, x, y):
		Window.SetPosition(self, x, y)
		self.imagebox.SetPosition(x - self.borderLeft, y - self.borderTop)
		self.x = x
		self.y = y
		self.__ArrangeListBox()
	
	def SetSize(self, width, height):
		Window.SetSize(self, width, height)
		self.width = width
		self.height = height
		self.textLine.UpdateRect()
		self.__ArrangeListBox()
	
	def __ArrangeListBox(self):
		if self.dropdownParent:
			gx, gy = self.GetGlobalPosition()
			px, py = self.dropdownParent.GetGlobalPosition()
			self.listBox.SetPosition(gx - px, gy - py + self.height + 5)
		else:
			self.listBox.SetPosition(0, self.height + 5)
		self.listBox.SetWidth(self.width)

	def Enable(self):
		self.enable = True

	def Disable(self):
		self.enable = False
		self.textLine.SetText("")
		self.CloseListBox()

	def SetEvent(self, event):
		self.event = event
	
	def ClearItem(self):
		self.CloseListBox()
		self.listBox.ClearItem()
	
	def InsertItem(self, index, name):
		self.listBox.InsertItem(index, name)
		self.listBox.ArrangeItem()
	
	def SetCurrentItem(self, text):
		self.textLine.SetText(text)
	
	def SelectItem(self, key):
		self.listBox.SelectItem(key)
	
	def OnSelectItem(self, index, name):
		self.CloseListBox()
		self.event(index)
	
	def CloseListBox(self):
		self.isListOpened = False
		self.listBox.Hide()
	
	def OnMouseLeftButtonDown(self):
		if not self.enable:
			return
		self.isSelected = True

	def OnMouseLeftButtonUp(self):
		if not self.enable:
			return

		self.isSelected = False

		if self.isListOpened:
			self.CloseListBox()
		else:
			if self.listBox.GetItemCount() > 0:
				self.isListOpened = True
				self.__ArrangeListBox()
				self.listBox.Show()

	def OnSelectItem(self, index, name):
		self.CloseListBox()
		self.event(index)

	def OnUpdate(self):
		if not self.enable:
			return
		if self.IsIn():
			self.isOver = True
		else:
			self.isOver = False
	
	def OnRender(self):
		self.x, self.y = self.GetGlobalPosition()
		xRender = self.x
		yRender = self.y
		widthRender = self.width
		heightRender = self.height
		
		if self.isOver:
			grp.SetColor(HALF_WHITE_COLOR)
			# Render INSIDE the borders
			grp.RenderBar(xRender, yRender, widthRender, heightRender)
			
			if self.isSelected:
				grp.SetColor(WHITE_COLOR)
				grp.RenderBar(xRender, yRender, widthRender, heightRender)
	
	# Text customization methods
	def SetTextColor(self, color):
		"""Set text color using packed color format (0xAARRGGBB)"""
		self.textLine.SetPackedFontColor(color)
	
	def SetFontColor(self, r, g, b, a=1.0):
		"""Set text color using RGBA values (0.0-1.0)"""
		self.textLine.SetFontColor(r, g, b, a)
	
	def SetOutline(self, value=True):
		"""Enable/disable text outline"""
		if value:
			self.textLine.SetOutline()
	
	def SetFontName(self, fontName):
		"""Set font name (e.g., 'Tahoma:12')"""
		self.textLine.SetFontName(fontName)
	
	def SetHorizontalAlign(self, align):
		"""Set horizontal alignment: 'left', 'center', 'right'"""
		if align == "left":
			self.textLine.SetHorizontalAlignLeft()
		elif align == "center":
			self.textLine.SetHorizontalAlignCenter()
		elif align == "right":
			self.textLine.SetHorizontalAlignRight()
	
	def SetVerticalAlign(self, align):
		"""Set vertical alignment: 'top', 'center', 'bottom'"""
		if align == "top":
			self.textLine.SetVerticalAlignTop()
		elif align == "center":
			self.textLine.SetVerticalAlignCenter()
		elif align == "bottom":
			self.textLine.SetVerticalAlignBottom()

class MultiTextLineNew(Window):
	def __del__(self):
		Window.__del__(self)
	def Destroy(self):
		self.textRules = {}
	def __init__(self):
		Window.__init__(self)
		self.Destroy()
		self.AddFlag("not_pick")
		self.textRules["textRange"] = 15
		self.textRules["text"] = ""
		self.textRules["textType"] = ""
		self.textRules["fontName"] = ""
		self.textRules["hexColor"] = 0
		self.textRules["fontColor"] = 0
		self.textRules["outline"] = 0
	def SetTextType(self, textType):
		self.textRules["textType"] = textType
		self.Refresh()
	def SetTextRange(self, textRange):
		self.textRules["textRange"] = textRange
		self.Refresh()
	def SetOutline(self, outline):
		self.textRules["outline"] = outline
		self.Refresh()
	def SetPackedFontColor(self, hexColor):
		self.textRules["hexColor"] = hexColor
		self.Refresh()
	def SetFontColor(self, r, g, b):
		self.textRules["fontColor"] =[r, g, b]
		self.Refresh()
	def SetFontName(self, fontName):
		self.textRules["fontName"] = fontName
		self.Refresh()
	def SetText(self, newText):
		self.textRules["text"] = newText
		self.Refresh()
	def Refresh(self):
		textRules = self.textRules
		self.children=[]

		outline = textRules["outline"]
		fontColor = textRules["fontColor"]
		hexColor = textRules["hexColor"]
		yRange = textRules["textRange"]
		fontName = textRules["fontName"]
		textType = textRules["textType"].split("#")
		totalTextList = textRules["text"].split("#")

		(xPosition, yPosition) = (0, 0)

		for text in totalTextList:
			childText = TextLine()
			childText.SetParent(self)
			childText.AddFlag("not_pick")
			childText.SetPosition(xPosition, yPosition)
			if fontName != "":
				childText.SetFontName(fontName)
			if hexColor != 0:
				childText.SetPackedFontColor(hexColor)
			if fontColor != 0:
				childText.SetFontColor(*fontColor)
			if outline:
				childText.SetOutline()
			self.AddTextType(childText, textType)
			childText.SetText(str(text))
			childText.Show()
			self.children.append(childText)
			yPosition+=yRange
		self.CheckTexType(self.children, textType, yPosition)
	def AddTextType(self, text,  typeArg):
		if len(typeArg) != 2:
			return
		_typeDict = {
			"vertical":{
				"top":text.SetVerticalAlignTop,
				"bottom":text.SetVerticalAlignBottom,
				"center":text.SetVerticalAlignCenter,
			},
			"horizontal":{
				"top":text.SetHorizontalAlignLeft,
				"bottom":text.SetHorizontalAlignRight,
				"center":text.SetHorizontalAlignCenter,
			},
			"all_align":{
				"1" : [text.SetHorizontalAlignCenter,text.SetVerticalAlignCenter,text.SetWindowHorizontalAlignCenter,text.SetWindowVerticalAlignCenter],
			},
		}
		(firstToken, secondToken) = tuple(typeArg)
		if _typeDict.has_key(firstToken):
			textType = _typeDict[firstToken][secondToken] if _typeDict[firstToken].has_key(secondToken) else None
			if textType != None:
				if isinstance(textType, list):
					for rule in textType:
						rule()
				else:
					textType()
	def CheckTexType(self, textList,  typeArg, yMax):
		if len(typeArg) != 2:
			return
		elif typeArg[1] != "center":
			return
		width = 0
		for text in textList:
			if text.GetTextSize()[0] > width:
				width = text.GetTextSize()[0]
		centerWidth = width / 2
		#for text in textList:
		#	text.SetPosition(centerWidth, text.GetLocalPosition()[1])

		self.SetSize(width, yMax)
