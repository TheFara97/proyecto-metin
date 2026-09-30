import dbg
import interfaceModule
import item
import player
import safebox
import uiToolTip
import exchange

class ItemWrapper:
	def __init__(self, window, position):
		self.__isActivated = False
		
		self.__window = window
		self.__position = position
	
	def __del__(self):
		if self.IsActivated():
			self.Deactivate()
	
	def GetVnum(self):
		if self.GetWindow() in (player.INVENTORY,
								player.DRAGON_SOUL_INVENTORY):
			return player.GetItemIndex(self.GetWindow(), self.GetPosition())
		elif self.GetWindow() == player.SAFEBOX:
			return safebox.GetItemID(self.GetPosition())
		elif self.GetWindow() == player.MALL:
			return safebox.GetMallItemID(self.GetPosition())
	
	def GetCount(self):
		if self.GetWindow() in (player.INVENTORY,
								player.DRAGON_SOUL_INVENTORY):
			return player.GetItemCount(self.GetWindow(), self.GetPosition())
		elif self.GetWindow() == player.SAFEBOX:
			return safebox.GetItemCount(self.GetPosition())
		elif self.GetWindow() == player.MALL:
			return safebox.GetMallItemCount(self.GetPosition())
	
	def GetWindow(self):
		return self.__window
	
	def SetWindow(self, window):
		self.__window = window
	
	def GetPosition(self):
		return self.__position
	
	def SetPosition(self, position):
		self.__position = position
	
	def IsActivated(self):
		return self.__isActivated
	
	def Activate(self, r = 1.0, g = 1.0, b = 1.0, a = 1.0):
		window = interfaceModule.GetInstance().GetWindowByType(self.GetWindow())
		if not window:
			return
		
		try:
			window.ActivateSlot(self.GetPosition(), r, g, b, a)
			self.__isActivated = True
		except:
			dbg.TraceError("Not implemented method ActivateSlot for window {}.".format(self.GetWindow()))
	
	def Deactivate(self):
		window = interfaceModule.GetInstance().GetWindowByType(self.GetWindow())
		if not window:
			return
		
		try:
			window.DeactivateSlot(self.GetPosition())
			self.__isActivated = False
		except:
			dbg.TraceError("Not implemented method DeactivateSlot for window {}.".format(self.GetWindow()))

class ItemToolTipWrapper(ItemWrapper):
	def __init__(self, window, position):
		ItemWrapper.__init__(self, window, position)
		
		self.__toolTip = uiToolTip.GetItemToolTipInstance()
	
	def GetToolTip(self):
		return self.__toolTip
	
	def ShowToolTip(self):
		if not self.GetToolTip():
			return
		
		if self.GetWindow() in (player.INVENTORY,
								player.DRAGON_SOUL_INVENTORY):
			self.GetToolTip().SetInventoryItem(self.GetPosition(), self.GetWindow())
		elif self.GetWindow() == player.SAFEBOX:
			self.GetToolTip().SetSafeBoxItem(self.GetPosition())
		elif self.GetWindow() == player.MALL:
			self.GetToolTip().SetMallItem(self.GetPosition())
		else:
			raise Exception("Window type {} not supported.".format(self.GetWindow()))
	
	def HideToolTip(self):
		if not self.GetToolTip():
			return
		
		self.GetToolTip().HideToolTip()
	
	def AttachOnAddItemData(self, event):
		if not self.GetToolTip():
			return
		
		self.GetToolTip().AttachOnAddItemData(event)
	
	def DetachOnAddItemData(self, event):
		if not self.GetToolTip():
			return
		
		self.GetToolTip().DetachOnAddItemData(event)

class ItemToolTipDummy:
	def __init__(self, vnum, metinSlot = None, attrSlot = None, forceUseableColor = False):
		self.__vnum = vnum
		self.__metinSlot = metinSlot
		self.__attrSlot = attrSlot
		self.__forceUseableColor = forceUseableColor
		
		if not self.__metinSlot:
			self.__metinSlot = [0] * player.METIN_SOCKET_MAX_NUM
		
		if not self.__attrSlot:
			self.__attrSlot = [(0, 0)] * player.ATTRIBUTE_SLOT_MAX_NUM
		
		self.__toolTip = uiToolTip.GetItemToolTipInstance()
		
		self.__shortcuts = []
	
	def __del__(self):
		self.__metinSlot = None
		self.__attrSlot = None
		
		self.__toolTip = None
		
		self.__shortcuts = None
	
	def GetVnum(self):
		return self.__vnum
	
	def SetVnum(self, vnum):
		self.__vnum = vnum
	
	def GetMetinSlot(self):
		return self.__metinSlot
	
	def SetMetinSlot(self, metinSlot):
		self.__metinSlot = metinSlot
	
	def GetAttrSlot(self):
		return self.__attrSlot
	
	def SetAttrSlot(self, attrSlot):
		self.__attrSlot = attrSlot
	
	def IsForceUseableColor(self):
		return self.__forceUseableColor
	
	def SetForceUseableColor(self, forceUseableColor):
		self.__forceUseableColor = forceUseableColor
	
	def SetShortcuts(self, shortcuts):
		self.__shortcuts = shortcuts
	
	def GetToolTip(self):
		return self.__toolTip
	
	def ShowToolTip(self):
		if not self.GetToolTip():
			return
		
		self.GetToolTip().ClearToolTip()
		self.GetToolTip().SetCannotUseItemForceSetDisableColor(not self.IsForceUseableColor())
		self.GetToolTip().AddItemData(self.GetVnum(), self.GetMetinSlot(), self.GetAttrSlot())
		
		if len(self.__shortcuts) > 0:
			self.GetToolTip().AppendSpace(5)
			
			for shortcut in self.__shortcuts:
				self.GetToolTip().AppendShortcut(*shortcut)
		
		self.GetToolTip().ShowToolTip()
	
	def HideToolTip(self):
		if not self.GetToolTip():
			return
		
		self.GetToolTip().HideToolTip()

class ItemToolTipOnlyTitleDummy:
	def __init__(self, vnum, color = uiToolTip.ToolTip.TITLE_COLOR, postfix = ""):
		self.__vnum = vnum
		self.__color = color
		self.__postfix = postfix
		
		self.__toolTip = uiToolTip.GetItemToolTipInstance()
	
	def GetVnum(self):
		return self.__vnum
	
	def SetVnum(self, vnum):
		self.__vnum = vnum
	
	def GetColor(self):
		return self.__color
	
	def SetColor(self, color):
		self.__color = color
	
	def GetPostfix(self):
		return self.__postfix
	
	def SetPostfix(self, postfix):
		self.__postfix = postfix
	
	def GetToolTip(self):
		return self.__toolTip
	
	def ShowToolTip(self):
		if not self.GetToolTip():
			return
		
		item.SelectItem(self.GetVnum())
		
		self.GetToolTip().ClearToolTip()
		self.GetToolTip().AppendTextLine(item.GetItemName() + self.GetPostfix(), self.GetColor())
		self.GetToolTip().ShowToolTip()
	
	def HideToolTip(self):
		if not self.GetToolTip():
			return
		
		self.GetToolTip().HideToolTip()

class ItemGridWrapper(ItemWrapper):
	def __init__(self, window, position):
		ItemWrapper.__init__(self, window, position)
		
		item.SelectItem(self.GetVnum())
		_, self.__size = item.GetItemSize()
	
	def GetSize(self):
		return self.__size

class ItemContainer(object):
	def __init__(self):
		self.items = {}
		self.onSetItem = None
	
	def __del__(self):
		self.items = {}
		self.onSetItem = None
	
	def Clear(self):
		slotIndices = self.items.keys()
		
		self.items = {}
		
		for slotIndex in slotIndices:
			self.OnSetItem(slotIndex)
	
	def SetItem(self, slotIndex, i):
		if not i and slotIndex in self.items:
			del self.items[slotIndex]
		else:
			self.items[slotIndex] = i
		
		self.OnSetItem(slotIndex)
	
	def GetItem(self, slotIndex):
		return self.items.get(slotIndex, None)
	
	def GetItemCount(self):
		return len(self.items)
	
	def GetVnum(self, slotIndex):
		i = self.GetItem(slotIndex)
		if not i:
			return 0
		
		return i.GetVnum()
	
	def GetCount(self, slotIndex):
		i = self.GetItem(slotIndex)
		if not i:
			return 0
		
		return i.GetCount()
	
	def SetOnSetItem(self, event):
		self.onSetItem = event
	
	def OnSetItem(self, slotIndex):
		if self.onSetItem:
			self.onSetItem(slotIndex)
	
	def __iter__(self):
		return self.items.iteritems()
