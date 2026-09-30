import ui
import player
import mouseModule
import net
import app
import chat
import snd
import item
import grp
import uiScriptLocale
import localeInfo
import ime
import grpText
import uiToolTip
import os

class changeequip(ui.ScriptWindow):

	def __init__(self):
		import exception
		ui.ScriptWindow.__init__(self)
		self.wndEquip = None
		self.tokens = None		
		self.activeslot = 0
		self.page = 0
		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.Hide()
		self.saveName = "lib/"+str(player.GetName())+"0.ddr"

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):			
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)
		
	def Close(self):
		self.Hide()	
			
	def Open(self):
		self.Show()	
		
	def __LoadWindow(self):
		try:			
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/fastequip.py")
		except:
			import exception
			exception.Abort("FastEquip.LoadWindow.LoadObject")

		self.wndEquip = self.GetChild("equipslot")
		self.bottone_change = self.GetChild("change_button")
		self.TitleBar = self.GetChild("TitleBar")		

		self.TitleBar.SetCloseEvent(ui.__mem_func__(self.Close))
		self.wndEquip.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectItemSlot))
		self.wndEquip.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot2))
		self.wndEquip.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.wndEquip.SetOverOutItemEvent(ui.__mem_func__(self.OnOverOutItem))
		self.bottone_change.SetEvent(ui.__mem_func__(self.__change_button))

		self.eq_btns = {
			0 : self.GetChild("eq0"),
			1 : self.GetChild("eq1"),
			2 : self.GetChild("eq2"),
			3 : self.GetChild("eq3"),
			4 : self.GetChild("eq4")
		}

		for i in xrange(5):
			self.eq_btns[i].SetEvent(ui.__mem_func__(self.__SetSlot), i)
			self.eq_btns[i].SetUp()
		self.eq_btns[self.page].Down()

		self.LoadData()
		self.SetTop()

	def __change_button(self):
			for i in range(1,18):
				if self.tokens[i-1] != "@":
					net.SendItemUsePacket(int(self.tokens[i-1]))

	def	LoadData(self):
		if os.path.exists(self.saveName):
			self.tokens = open(self.saveName, "r").read().split()
		else:
			open(self.saveName, "w").write("@\t@\t@\t@\t@\t@\t@\t@\t@\t@\t@\t@\t@\t@\t@\t@\t@")

	def __SetSlot(self, arg):
		self.saveName = "lib/"+str(player.GetName())+str(arg)+".ddr"

		for i in xrange(5):
			self.eq_btns[i].SetUp()
		self.eq_btns[arg].Down()
		self.page = arg

		self.LoadData()

	def OverInItem(self, slotNumber):
		if self.tooltipItem:
			self.tooltipItem.SetInventoryItem(int(self.tokens[slotNumber-1]))

	def OnOverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()
		
	def SelectItemSlot2(self, itemSlotIndex):
		self.tokens[itemSlotIndex-1] = "@"
		open(self.saveName, "w").write("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s" % tuple(self.tokens))

	def SelectItemSlot(self, itemSlotIndex):
		isAttached = mouseModule.mouseController.isAttached()
		if isAttached:
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			itemIndex = player.GetItemIndex(attachedSlotPos)
			itemCount = player.GetItemCount(attachedSlotPos)
			item.SelectItem(itemIndex)
			itemType = item.GetItemType()
			itemSubType = item.GetItemSubType()
			if itemSlotIndex == 1 and item.IsWearableFlag(item.WEARABLE_BODY):	
				self.tokens[itemSlotIndex-1] = attachedSlotPos
			elif itemSlotIndex == 2 and item.IsWearableFlag(item.WEARABLE_HEAD):	
				self.tokens[itemSlotIndex-1] = attachedSlotPos
			elif itemSlotIndex == 3 and item.IsWearableFlag(item.WEARABLE_FOOTS):	
				self.tokens[itemSlotIndex-1] = attachedSlotPos
			elif itemSlotIndex == 4 and item.IsWearableFlag(item.WEARABLE_WRIST):	
				self.tokens[itemSlotIndex-1] = attachedSlotPos
			elif itemSlotIndex == 5 and item.IsWearableFlag(item.WEARABLE_WEAPON):	
				self.tokens[itemSlotIndex-1] = attachedSlotPos	
			elif itemSlotIndex == 6 and item.IsWearableFlag(item.WEARABLE_NECK):		
				self.tokens[itemSlotIndex-1] = attachedSlotPos	
			elif itemSlotIndex == 7 and item.IsWearableFlag(item.WEARABLE_EAR):	
				self.tokens[itemSlotIndex-1] = attachedSlotPos
			elif itemSlotIndex == 8 and item.IsWearableFlag(item.WEARABLE_SHIELD):	
				self.tokens[itemSlotIndex-1] = attachedSlotPos
			elif itemSlotIndex == 9 and itemType == item.ITEM_TYPE_BELT:
				self.tokens[itemSlotIndex-1] = attachedSlotPos
			elif itemSlotIndex == 10 and itemType == item.ITEM_TYPE_ARMOR and itemSubType == item.ARMOR_GLOVE:
				self.tokens[itemSlotIndex-1] = attachedSlotPos
			elif itemSlotIndex == 11 and itemType == item.ITEM_TYPE_COSTUME and itemSubType == item.COSTUME_TYPE_WEAPON:
				self.tokens[itemSlotIndex-1] = attachedSlotPos
			elif itemSlotIndex == 12 and itemType == item.ITEM_TYPE_COSTUME and itemSubType == item.COSTUME_TYPE_HAIR:
				self.tokens[itemSlotIndex-1] = attachedSlotPos
			elif itemSlotIndex == 13 and itemType == item.ITEM_TYPE_COSTUME and itemSubType == item.COSTUME_TYPE_BODY:
				self.tokens[itemSlotIndex-1] = attachedSlotPos
			elif itemSlotIndex == 14 and itemType == item.ITEM_TYPE_COSTUME and itemSubType == item.COSTUME_TYPE_EMBLEMAT:
				self.tokens[itemSlotIndex-1] = attachedSlotPos
			elif itemSlotIndex == 15 and itemType == item.ITEM_TYPE_COSTUME and itemSubType == item.COSTUME_TYPE_ACCE:
				self.tokens[itemSlotIndex-1] = attachedSlotPos
			elif itemSlotIndex == 16 and itemType == item.ITEM_TYPE_COSTUME and itemSubType == item.COSTUME_TYPE_STOLE:
				self.tokens[itemSlotIndex-1] = attachedSlotPos
			elif itemSlotIndex == 17 and itemType == item.ITEM_TYPE_COSTUME and itemSubType == item.COSTUME_TYPE_AURA:
				self.tokens[itemSlotIndex-1] = attachedSlotPos
			else:		
				return

			open(self.saveName, "w").write("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s" % tuple(self.tokens))	

			mouseModule.mouseController.DeattachObject()

	def OnUpdate(self):	
		tokens = open(self.saveName, "r").read().split()		
		self.tokens = tokens
		for i in range(1,18):	
			if tokens[i-1] == "@":
				self.wndEquip.SetItemSlot(i, 0, 0)
			else:
				itemIndex = player.GetItemIndex(int(tokens[i-1]))
				if itemIndex != 0:
					item.SelectItem(itemIndex)
					itemType = item.GetItemType()
					itemSubType = item.GetItemSubType()
					if i == 1 and item.IsWearableFlag(item.WEARABLE_BODY):	
						self.wndEquip.SetItemSlot(i, itemIndex, 0)
					elif i == 2 and item.IsWearableFlag(item.WEARABLE_HEAD):	
						self.wndEquip.SetItemSlot(i, itemIndex, 0)
					elif i == 3 and item.IsWearableFlag(item.WEARABLE_FOOTS):	
						self.wndEquip.SetItemSlot(i, itemIndex, 0)
					elif i == 4 and item.IsWearableFlag(item.WEARABLE_WRIST):	
						self.wndEquip.SetItemSlot(i, itemIndex, 0)
					elif i == 5 and item.IsWearableFlag(item.WEARABLE_WEAPON):	
						self.wndEquip.SetItemSlot(i, itemIndex, 0)
					elif i == 6 and item.IsWearableFlag(item.WEARABLE_NECK):	
						self.wndEquip.SetItemSlot(i, itemIndex, 0)
					elif i == 7 and item.IsWearableFlag(item.WEARABLE_EAR):		
						self.wndEquip.SetItemSlot(i, itemIndex, 0)
					elif i == 8 and item.IsWearableFlag(item.WEARABLE_SHIELD):	
						self.wndEquip.SetItemSlot(i, itemIndex, 0)
					elif i == 9 and itemType == item.ITEM_TYPE_BELT:
						self.wndEquip.SetItemSlot(i, itemIndex, 0)
					elif i == 10 and itemType == item.ITEM_TYPE_ARMOR and itemSubType == item.ARMOR_GLOVE:
						self.wndEquip.SetItemSlot(i, itemIndex, 0)
					elif i == 11 and itemType == item.ITEM_TYPE_COSTUME and itemSubType == item.COSTUME_TYPE_WEAPON:
						self.wndEquip.SetItemSlot(i, itemIndex, 0)
					elif i == 12 and itemType == item.ITEM_TYPE_COSTUME and itemSubType == item.COSTUME_TYPE_HAIR:
						self.wndEquip.SetItemSlot(i, itemIndex, 0)
					elif i == 13 and itemType == item.ITEM_TYPE_COSTUME and itemSubType == item.COSTUME_TYPE_BODY:
						self.wndEquip.SetItemSlot(i, itemIndex, 0)
					elif i == 14 and itemType == item.ITEM_TYPE_COSTUME and itemSubType == item.COSTUME_TYPE_EMBLEMAT:
						self.wndEquip.SetItemSlot(i, itemIndex, 0)
					elif i == 15 and itemType == item.ITEM_TYPE_COSTUME and itemSubType == item.COSTUME_TYPE_ACCE:
						self.wndEquip.SetItemSlot(i, itemIndex, 0)
					elif i == 16 and itemType == item.ITEM_TYPE_COSTUME and itemSubType == item.COSTUME_TYPE_STOLE:
						self.wndEquip.SetItemSlot(i, itemIndex, 0)
					elif i == 17 and itemType == item.ITEM_TYPE_COSTUME and itemSubType == item.COSTUME_TYPE_AURA:
						self.wndEquip.SetItemSlot(i, itemIndex, 0)
					else:
						self.wndEquip.SetItemSlot(i, 0, 0)
						self.tokens[i-1] = "@"
						open(self.saveName, "w").write("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s" % tuple(self.tokens))
						continue
				else:
					self.wndEquip.SetItemSlot(i, 0, 0)				

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE
