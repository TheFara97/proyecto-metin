import ui
import item
import wndMgr
import player 
import uiToolTip
import net
import constInfo
import event 
import chat
import uiCommon
import localeInfo
import app

# Crystal Types matching the C++ enum
CRYSTAL_TYPE_GENERAL = 0        # Ogólna
CRYSTAL_TYPE_BASIC_MAP = 1      # Mapy Podstawowe  
CRYSTAL_TYPE_DUNGEON_BASIC = 2  # Dungeon Podstawa
CRYSTAL_TYPE_AMASIA_MAPS = 3    # Mapy Amasia
CRYSTAL_TYPE_DUNGEON_AMASIA = 4 # Dungeon Amasia
CRYSTAL_TYPE_SZLATUKI = 5       # Szlatuki
MAX_CRYSTAL_TYPES = 6

CRYSTAL_TYPE_NAMES = [
	"Ogólna",
	"Mapy Podstawowe", 
	"Dungeon Podstawa",
	"Mapy Amasia",
	"Dungeon Amasia",
	"Szlatuki"
]

class CrystalExchangeWindow(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		
		self.LoadWindow()
		
		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.Hide()

		self.MessageBox = uiCommon.QuestionDialog()
		
		# Crystal counts for each type
		self.crystalCounts = [0] * MAX_CRYSTAL_TYPES
		
		# Current selected crystal type
		self.selectedCrystalType = CRYSTAL_TYPE_GENERAL
		
		# Exchange items data from server
		self.exchangeItems = []  # Format: [(vnum, current_rate, crystal_reward, crystal_type), ...]
		
		# Inventory management
		self.slot_inventario, self.slot_gui, self.slotPos, self.slot_select = [], [], 0, 0

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadWindow(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/crystalexchangewindow.py")
		except:
			import exception
			exception.Abort("CrystalExchangeWindow.LoadDialog.LoadObject")

		try:
			GetObject = self.GetChild
			self.board = GetObject("board")
			self.titleBar = GetObject("TitleBar") 
			self.btnExit = GetObject("ExitButton")
			
			# Crystal type buttons (left side)
			self.crystalButtons = []
			self.crystalCountTexts = []
			for i in xrange(MAX_CRYSTAL_TYPES):
				self.crystalButtons.append(GetObject("crystal_button_%d" % i))
				self.crystalCountTexts.append(GetObject("crystal_count_%d" % i))
			
			# Exchange section
			self.itemSlot = GetObject("ItemSlot")
			self.refreshButton = GetObject("RefreshButton")
			
		except:
			import exception
			exception.Abort("CrystalExchangeWindow.LoadDialog.BindObject")

		# Set events
		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
		self.btnExit.SetEvent(ui.__mem_func__(self.Close))
		
		# Crystal type selection buttons
		for i in xrange(MAX_CRYSTAL_TYPES):
			self.crystalButtons[i].SetEvent(ui.__mem_func__(self.SelectCrystalType), i)
		
		# Item slot events
		self.itemSlot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
		self.itemSlot.SAFE_SetButtonEvent("LEFT", "EXIST", self.SelectItemSlot)
		self.itemSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.itemSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		
		# Refresh button
		self.refreshButton.SetEvent(ui.__mem_func__(self.RequestRefresh))

	def Close(self):
		wndMgr.OnceIgnoreMouseLeftButtonUpEvent()
		self.Hide()
		# Send close packet to server
		net.SendCrystalExchangeAction(1, 0, 0)  # SUBHEADER_CRYSTAL_EXCHANGE_CLOSE

	def Open(self):
		self.Clear()
		self.Show()
		# Request data from server when opening
		net.SendCrystalExchangeAction(0, 0, 0)  # SUBHEADER_CRYSTAL_EXCHANGE_OPEN
		self.RefreshSlot(False)
		self.UpdateCrystalCounts()

	def Clear(self):
		self.slot_inventario, self.slot_gui, self.slotPos, self.slot_select = [], [], 0, 0

	def SelectCrystalType(self, crystalType):
		if crystalType < 0 or crystalType >= MAX_CRYSTAL_TYPES:
			return
			
		self.selectedCrystalType = crystalType
		
		# Update button states (highlight selected)
		for i in xrange(MAX_CRYSTAL_TYPES):
			if i == crystalType:
				self.crystalButtons[i].Down()
			else:
				self.crystalButtons[i].SetUp()
		
		# Refresh items for selected crystal type
		self.RefreshSlot(True)

	def SelectItemSlot(self, slotPos):
		wndMgr.OnceIgnoreMouseLeftButtonUpEvent()
		
		if slotPos >= len(self.slot_inventario):
			return
			
		SlotInventario = self.slot_inventario[slotPos]
		self.slot_select = slotPos
		
		itemVNum = player.GetItemIndex(SlotInventario)
		itemCount = player.GetItemCount(SlotInventario)
		
		if itemVNum == 0:
			return
		
		# Get exchange info for this item
		itemsNeeded, crystalsGiven = self.GetExchangeRate(itemVNum)
		
		if itemsNeeded == 0 or crystalsGiven == 0:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "This item cannot be exchanged.")
			return
			
		# Check if player has enough items
		if itemCount < itemsNeeded:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "You need %d of this item to exchange." % itemsNeeded)
			return
		
		# Show confirmation dialog
		confirmText = "Exchange %d items for %d crystal shards?\nRate: %d:%d" % (itemsNeeded, crystalsGiven, itemsNeeded, crystalsGiven)
		self.MessageBox.SetText(confirmText)
		self.MessageBox.SetAcceptEvent(lambda: self.ConfirmExchange(itemVNum))
		self.MessageBox.SetCancelEvent(lambda: self.MessageBox.Close())
		self.MessageBox.Open()

	def ConfirmExchange(self, itemVNum):
		self.MessageBox.Close()
		# Send exchange packet to server
		net.SendCrystalExchangeAction(2, itemVNum, 1)  # SUBHEADER_CRYSTAL_EXCHANGE_SELL

	def RequestRefresh(self):
		# Send refresh request to server
		net.SendCrystalExchangeAction(3, 0, 0)  # SUBHEADER_CRYSTAL_EXCHANGE_REFRESH

	def SetTableSize(self, size):
		SLOT_X_COUNT = 5
		self.itemSlot.ArrangeSlot(0, SLOT_X_COUNT, size, 32, 32, 0, 0)
		self.itemSlot.RefreshSlot()
		self.itemSlot.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)
		self.board.SetSize(self.board.GetWidth(), 76 + 32*size)
		self.SetSize(self.board.GetWidth(), 76 + 32*size)
		self.UpdateRect()

	def RefreshSlot(self, refresh=False):
		getItemVNum, getItemCount, setItemVNum = player.GetItemIndex, player.GetItemCount, self.itemSlot.SetItemSlot

		if not refresh:
			self.slot_inventario = []
			self.slot_gui = []
			self.slotPos = 0
			
			# Get configured items for current crystal type
			configuredItems = self.GetConfiguredItemsForCategory(self.selectedCrystalType)
			
			for i in xrange(player.INVENTORY_PAGE_SIZE):
				slotNumber = i
				itemVNum = getItemVNum(slotNumber)

				if 0 == itemVNum:
					continue

				# Check if this item is configured for exchange in current category
				if itemVNum not in configuredItems:
					continue

				self.slot_inventario.append(i)
				self.slot_gui.append(self.slotPos)

				self.slotPos += 1
				if self.slotPos > 54:
					break

		itemCount = len(self.slot_inventario)
		if itemCount < 15:
			self.SetTableSize(3)
		else:
			lineCount = 3
			lineCount += (itemCount - 15) / 5
			if itemCount % 5:
				lineCount += 1
			self.SetTableSize(lineCount)

		count = 0
		for inventoryPos in self.slot_inventario:
			itemVNum = getItemVNum(inventoryPos)
			itemCount = getItemCount(inventoryPos)

			if itemCount <= 1:
				itemCount = 0

			setItemVNum(count, itemVNum, itemCount)
			count += 1
	
		self.itemSlot.RefreshSlot()
		if refresh:
			self.tooltipItem.Hide()

	def GetConfiguredItemsForCategory(self, crystalType):
		"""Get all item vnums configured for a specific crystal category"""
		configuredItems = set()
		
		for exchangeItem in self.exchangeItems:
			# exchangeItem format: [vnum, current_rate, crystal_reward, crystal_type]
			if len(exchangeItem) >= 4 and exchangeItem[3] == crystalType:
				configuredItems.add(exchangeItem[0])  # Add vnum to set
		
		return configuredItems

	def GetExchangeRate(self, itemVNum):
		"""Get exchange rate for an item (items needed : crystals given)"""
		for exchangeItem in self.exchangeItems:
			if exchangeItem[0] == itemVNum:
				itemsNeeded = exchangeItem[1]  # current rate
				crystalsGiven = exchangeItem[2]  # crystal reward count
				return (itemsNeeded, crystalsGiven)
		return (0, 0)

	def UpdateCrystalCounts(self):
		"""Update crystal count displays"""
		for i in xrange(MAX_CRYSTAL_TYPES):
			self.crystalCountTexts[i].SetText("Kryształy: %d szt." % self.crystalCounts[i])

	def OverOutItem(self):
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OverInItem(self, slotIndex):
		if None != self.tooltipItem:
			if slotIndex < len(self.slot_inventario):
				inventorySlotPos = self.slot_inventario[slotIndex]
				itemVNum = player.GetItemIndex(inventorySlotPos)
				
				# Show normal tooltip
				self.tooltipItem.SetInventoryItem(inventorySlotPos)
				
				# Add exchange rate information
				itemsNeeded, crystalsGiven = self.GetExchangeRate(itemVNum)
				if itemsNeeded > 0 and crystalsGiven > 0:
					# Add separator line
					self.tooltipItem.AppendTextLine("─────────────────", 0xFFFFFFFF)
					
					# Add exchange rate info
					rateText = "Exchange Rate: %d : %d crystals" % (itemsNeeded, crystalsGiven)
					self.tooltipItem.AppendTextLine(rateText, 0xFFFFE3AD)
					
					# Add crystal type info
					crystalTypeName = CRYSTAL_TYPE_NAMES[self.selectedCrystalType]
					typeText = "Crystal Type: %s" % crystalTypeName
					self.tooltipItem.AppendTextLine(typeText, 0xFF87CEEB)

	def SucceedExchange(self):
		"""Called when exchange succeeds"""
		if self.slot_select < len(self.slot_inventario):
			del self.slot_inventario[self.slot_select]
			del self.slot_gui[self.slot_select]
		self.RefreshSlot(True)
		self.UpdateCrystalCounts()

	# Server communication methods
	def SetCrystalCounts(self, counts):
		"""Update crystal counts from server"""
		if len(counts) >= MAX_CRYSTAL_TYPES:
			self.crystalCounts = counts[:MAX_CRYSTAL_TYPES]
			self.UpdateCrystalCounts()

	def SetExchangeItems(self, items):
		"""Set available exchange items from server"""
		self.exchangeItems = items
		self.RefreshSlot(True)

	def OnRateUpdate(self):
		"""Called when exchange rates are updated"""
		chat.AppendChat(chat.CHAT_TYPE_INFO, "Exchange rates updated! (24h cycle)")
		self.RefreshSlot(True)