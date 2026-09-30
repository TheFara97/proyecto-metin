import ui
import net
import mouseModule
import player
import snd
import localeInfo
import item
import grp
import uiScriptLocale
import interfaceModule
import itemWrapper

import cube
import uiToolTip
import util
import app

from ItemWrapper import ItemToolTipWrapper, ItemToolTipDummy, ItemToolTipOnlyTitleDummy, ItemContainer

ROOT_PATH = "Assets/ui/cube/{}"
ENABLE_MAKE_ALL_BUTTON = True

class Recipe:
	class Item:
		vnum = None
		count = None
	
	items = None
	reward = None
	price = None
	priceCheque = None
	priceAchievement = None
	chance = None
	category = None
	
	@staticmethod
	def CreateFromServer(packedRecipe):
		reward = Recipe.Item()
		reward.vnum = packedRecipe[0][0]
		reward.count = packedRecipe[0][1]
		
		items = []
		for packedRecipeItem in packedRecipe[1]:
			material = Recipe.Item()
			material.vnum = packedRecipeItem[0]
			material.count = packedRecipeItem[1]
			
			items.append(material)
		
		recipe = Recipe()
		recipe.items = items
		recipe.reward = reward
		recipe.price = packedRecipe[2]
		recipe.priceCheque = packedRecipe[3]
		recipe.priceAchievement = packedRecipe[4]
		recipe.chance = packedRecipe[5]
		recipe.category = packedRecipe[6]

		return recipe

class CraftingCalculator:
    """
    Strategy pattern implementation for calculating maximum possible crafts
    """
    class CraftingConstraint:
        def __init__(self, current_amount, required_amount):
            self.current = current_amount
            self.required = required_amount

        def get_possible_crafts(self):
            if self.required <= 0:
                return float('inf')
            return self.current // self.required

    def __init__(self, max_craft_limit=99):
        self.MAX_CRAFT_LIMIT = max_craft_limit
        self._constraints = []
        
    def add_money_constraint(self, player_money, price_per_craft):
        if price_per_craft > 0:
            self._constraints.append(
                self.CraftingConstraint(player_money, price_per_craft)
            )
        
    def add_material_constraint(self, available_count, required_count):
        if required_count > 0:
            self._constraints.append(
                self.CraftingConstraint(available_count, required_count)
            )
            
    def calculate(self):
        if not self._constraints:
            return self.MAX_CRAFT_LIMIT
            
        return min(
            min(constraint.get_possible_crafts() for constraint in self._constraints),
            self.MAX_CRAFT_LIMIT
        )

class CubeWindow(ui.ScriptWindow):
	SLOT_SIZEX = 32
	SLOT_SIZEY = 32
	
	ITEM_TITLE_COLOR = uiToolTip.ToolTip.POSITIVE_COLOR
	ITEM_TITLE_COLOR_MISSING = uiToolTip.ToolTip.NEGATIVE_COLOR
	
	MIN_CRAFT_COUNT = 1
	MAX_CRAFT_COUNT = 99

	class CraftingError(Exception):
		"""Base exception for crafting-related errors"""
		pass

	class InvalidRecipeError(CraftingError):
		"""Raised when recipe is invalid or not found"""
		pass

	class InsufficientResourcesError(CraftingError):
		"""Raised when not enough resources for crafting"""
		pass

	class SubCategory(ui.ListBoxEx.Item):
		def __init__(self, key, data, selectEv):
			ui.ListBoxEx.Item.__init__(self)

			self.key = key
			self.data = data
			#self.SetSelectedRenderColor(grp.GenerateColor(1.0, 1.0, 1.0, 0.05))
			self.SetSelectedRenderColor(None)
			self.selectEvent = selectEv
			self.__BuildObjects()

		def Destroy(self):
			self.key = None
			self.data = None
			self.Hide()

		def __BuildObjects(self):
			self.separator = ui.ExpandedImageBox()
			self.separator.SetParent(self)
			self.separator.AddFlag("not_pick")
			self.separator.LoadImage(ROOT_PATH.format("sm+-.png"))
			self.separator.Show()

			self.SetSize(*self.separator.GetSize())

			name = ui.TextLine()
			name.SetParent(self.separator)
			name.SetPosition(30, 0)
			name.SetWindowVerticalAlignCenter()
			name.SetVerticalAlignCenter()

			item.SelectItem(self.data.reward.vnum)
			name.SetText(item.GetItemName() + uiScriptLocale.CUBE_CRAFT_CHANCE.format(self.data.chance))
			name.SetPackedFontColor(0xFF676767)
			name.Show()
			self.name = name
			self.selected = False

		def GetData(self):
			return self.data

		def OnSelect(self):
			self.name.SetPackedFontColor(0xFF4f8aec)
			self.selected = True
			if self.selectEvent:
				self.selectEvent(self.key)
			self.SetFocus()


		def OnUnselect(self):
			self.name.SetPackedFontColor(0xFF676767)
			self.selected = False

		def OnUpdate(self):
			if self.IsInPosition():
				if not self.selected:
					self.name.SetPackedFontColor(0xFFcbcbcb)
			else:
				if self.selected:
					self.name.SetPackedFontColor(0xFF4f8aec)
				else:
					self.name.SetPackedFontColor(0xFF676767)

	class Category(ui.ListBoxEx.Item):
		def __init__(self, data, selectEv):
			ui.ListBoxEx.Item.__init__(self)

			self.eventFoldIn = lambda: None
			self.eventFoldOut = lambda: None
			self.isFoldOut = False
			self.selectEvent = selectEv

			self.data = data
			
			self.subCategories = []
			self.__BuildObjects()
			self.__RefreshState()
			
		def Destroy(self):
			self.subCategories = None
			self.data = None
			self.eventFoldIn = None
			self.eventFoldOut = None
			self.selectEvent = None
			self.Hide()

		def AddSubCategory(self, key, recipe):
			"""
			Add a new subcategory to the category
			
			Args:
				subCategoryData (dict): Dictionary containing subcategory information
									Should have at least a 'name' key
			"""
			subCategory = CubeWindow.SubCategory(key, recipe, self.selectEvent)
			subCategory.Hide()
			self.subCategories.append(subCategory)
			self.__RefreshState()

		def __BuildObjects(self):
			mainBtn = ui.ToggleButton()
			mainBtn.SetParent(self)
			mainBtn.SetUpVisual(ROOT_PATH.format("sm+-.png"))
			mainBtn.SetOverVisual(ROOT_PATH.format("sm+-.png"))
			mainBtn.SetDownVisual(ROOT_PATH.format("sm+-.png"))
			mainBtn.SetToggleUpEvent(self.__OnFoldTrigger)
			mainBtn.SetToggleDownEvent(self.__OnFoldTrigger)
			mainBtn.Show()
			self.mainBtn = mainBtn
			self.SetSize(mainBtn.GetWidth(), mainBtn.GetHeight())
			
			icon = ui.ImageBox()
			icon.AddFlag("not_pick")
			icon.SetParent(mainBtn)
			icon.SetPosition(13, 0)
			icon.SetWindowVerticalAlignCenter()
			icon.LoadImage(ROOT_PATH.format("plus_ico.png"))
			icon.Show()
			self.icon = icon
			
			name = ui.TextLine()
			name.AddFlag("not_pick")
			name.SetParent(mainBtn)
			name.SetPosition(45, 0)
			name.SetWindowVerticalAlignCenter()
			name.SetVerticalAlignCenter()
			name.SetText(getattr(localeInfo, "CATEGORY_{}".format(self.data.category), "CATEGORY_{}".format(self.data.category)))
			name.Show()
			self.name = name

		def __RefreshState(self):
			self.__RefreshText()

		def __RefreshText(self):
			if self.mainBtn.IsDown():
				self.name.SetPackedFontColor(0xFF4f8aec)
				self.icon.LoadImage(ROOT_PATH.format("minus_ico.png"))
			else:
				self.name.SetPackedFontColor(0xFF676767)
				self.icon.LoadImage(ROOT_PATH.format("plus_ico.png"))


		def OnUpdate(self):
			if self.mainBtn.IsInPosition():
				if not self.mainBtn.IsDown():
					self.name.SetPackedFontColor(0xFFcbcbcb)
			else:
				if self.mainBtn.IsDown():
					self.name.SetPackedFontColor(0xFF4f8aec)
					self.icon.LoadImage(ROOT_PATH.format("minus_ico.png"))
				else:
					self.name.SetPackedFontColor(0xFF676767)
					self.icon.LoadImage(ROOT_PATH.format("plus_ico.png"))

		def __OnFoldTrigger(self):
			self.isFoldOut = not self.isFoldOut
			self.OnMouseLeftButtonDown()

			if self.isFoldOut:
				self.eventFoldOut()
				#debug("XXXX")
				# if self.selectEvent:
				# 	self.selectEvent(self.data["itemtype"][0], self.data["itemtype"][1])
				self.SetFocus()
				self.Down()
			else:
				self.eventFoldIn()
				self.SetUp()

			self.__RefreshState()

		def SetUp(self):
			self.isFoldOut = False
			self.mainBtn.SetUp()
			self.__RefreshState()

		def Down(self):
			self.isFoldOut = True
			self.mainBtn.Down()
			self.__RefreshState()

		def GetData(self):
			return self.data

		def IsFoldOut(self):
			return self.isFoldOut

		def GetSubCategories(self):
			return self.subCategories

		def HideSubCategories(self):
			for subCat in self.subCategories:
				subCat.Hide()

		def ClearSubCategories(self):
			"""Clear all subcategories from this category"""
			for subCat in self.subCategories:
				subCat.Destroy()
		
			self.subCategories = []
			self.__RefreshState()

		def SetFoldInEvent(self, eventFoldIn):
			self.eventFoldIn = eventFoldIn

		def SetFoldOutEvent(self, eventFoldOut):
			self.eventFoldOut = eventFoldOut

		def OnRender(self):
			pass

	def __init__(self):
		self.__cubeSlot = None
		
		ui.ScriptWindow.__init__(self)
		
		self.__iRecipe = -1
		self.CatList = []
		self.__craftCount = self.MIN_CRAFT_COUNT
		self.__maxCraftCount = self.MIN_CRAFT_COUNT
		
		self.__LoadWindow()
		self.Reset()
	
	def __LoadWindow(self):
		if not self.LoadScript(self, "uiscript/cubewindow.py"):
			return
		
		self.GetChild("board").SetCloseEvent(self.OnClickCancel)
		
		self.__priceText = self.GetChild("NeedMoney")
		self.__chequeText = self.GetChild("NeedCheque")
		self.__achievementText = self.GetChild("NeedAchiev")
		self.__chanceText = self.GetChild("percentage")
		self.__cubeSlot = self.GetChild("CubeManager-RewardSlot")
		self.__materialSlots = [self.GetChild("material11"), self.GetChild("material12"), self.GetChild("material13"), self.GetChild("material14"), self.GetChild("material15"), self.GetChild("material16"), self.GetChild("material17")]
		self.__materialCounts = [self.GetChild("material_qty_text_1"), self.GetChild("material_qty_text_2"), self.GetChild("material_qty_text_3"), self.GetChild("material_qty_text_4"), self.GetChild("material_qty_text_5"), self.GetChild("material_qty_text_6"), self.GetChild("material_qty_text_7")]
		self.__craftCountText = self.GetChild("craft_count_text")

		self.__craftCountText.SetUpdateEvent(ui.__mem_func__(self._RunUpdateEvent))

		self.__decreaseButton = self.GetChild("decrease_button")
		self.__increaseButton = self.GetChild("increase_button")
		
		self.__decreaseButton.SetEvent(self.__OnDecreaseCraftCount)
		self.__increaseButton.SetEvent(self.__OnIncreaseCraftCount)

		self.__cubeSlot.SetOverInItemEvent(self.OnOverInItem)
		self.__cubeSlot.SetOverOutItemEvent(self.OnOverOutItem)
		
		for i in range(len(self.__materialSlots)):
			materialSlot = self.__materialSlots[i]
			materialSlot.SetOverInItemEvent(lambda slotIndex = 0, rowIndex = i: self.OnOverInMaterialItem(slotIndex, rowIndex))
			materialSlot.SetOverOutItemEvent(self.OnOverOutItem)
		
		self.CategoryList = self.GetChild("CubeManager-CategoryContainer")
		self.CategoryListScrollBar = self.GetChild("CubeManager-CategoryScrollbar")

		self.CategoryList.SetScrollBar(self.CategoryListScrollBar)
		self.CategoryList.OnMouseWheel = self.CategoryListScrollBar.OnMouseWheel
		
		self.__chanceSlot = self.GetChild("CubeManager-ChanceSlot")

		self.item = ItemContainer()
		self.item.SetOnSetItem(self.OnSetItem)

		self.__chanceSlot.SetSelectEmptySlotEvent(self.OnSelectEmptySlot)
		self.__chanceSlot.SetSelectItemSlotEvent(self.OnSelectItemSlot)

		self.GetChild("AcceptAllButton").SetEvent(self.OnClickAcceptAll)
		self.GetChild("AcceptButton").SetEvent(self.OnClickAccept)
		self.GetChild("AcceptButton").SetToolTipTextNew(uiScriptLocale.CUBE_TITLE_CRAFT)
		
		if not ENABLE_MAKE_ALL_BUTTON:
			self.GetChild("AcceptAllButton").Hide()

	def _RunUpdateEvent(self):
		"""
		Handles updates to the craft count when text input changes.
		
		Returns:
			bool: True if update was handled, False otherwise
		"""
		try:
			value = self._GetCraftCount(False)
			
			if value <= 0:
				return False
				
			if value > self.__maxCraftCount:
				self.__craftCount = self.__maxCraftCount
			else:
				self.__craftCount = value
				
			self.__UpdateCraftCount()
			return True
			
		except Exception:
			return False

	def __OnDecreaseCraftCount(self):
		self.__craftCount = max(1, self.__craftCount - 1)
		self.__UpdateCraftCount()
		
	def __OnIncreaseCraftCount(self):
		self.__craftCount = min(self.__maxCraftCount, self.__craftCount + 1)
		self.__UpdateCraftCount()

	def ValidateRecipe(self):
		"""
		Validates current recipe state
		
		Returns:
			Recipe: Valid recipe object
			
		Raises:
			InvalidRecipeError: If recipe is invalid or not found
		"""
		if self.__iRecipe < 0:
			raise self.InvalidRecipeError("No recipe selected")
			
		recipe = self.__recipes[self.__iRecipe]
		if not recipe:
			raise self.InvalidRecipeError("Recipe not found")
			
		return recipe

	def CalculateMaximumCraftCount(self):
		try:
			recipe = self.ValidateRecipe()
			
			calculator = CraftingCalculator(max_craft_limit=99)
			
			player_money = player.GetMoney()
			player_cheque = player.GetCheque()
			player_achievement = player.GetPktOsiag()
			calculator.add_money_constraint(player_money, recipe.price, recipe.priceCheque, recipe.priceAchievement)
			
			if recipe.price > 0 and player_money < recipe.price:
				raise self.InsufficientResourcesError(uiScriptLocale.CUBE_CRAFT_NOT_ENOUGH_YANG)
			elif recipe.priceCheque > 0 and player_cheque < recipe.priceCheque:
				raise self.InsufficientResourcesError(uiScriptLocale.CUBE_CRAFT_NOT_ENOUGH_WON)
			elif recipe.priceAchievement > 0 and player_achievement < recipe.priceAchievement:
				raise self.InsufficientResourcesError(uiScriptLocale.CUBE_CRAFT_NOT_ENOUGH_AP)

			for material in recipe.items:
				if not material:
					continue
					
				current_count = player.GetItemCountByVnum(material.vnum)
				
				if material.count > 0 and current_count < material.count:
					raise self.InsufficientResourcesError(
						"Insufficient material (vnum: {})".format(material.vnum)
					)
				
				calculator.add_material_constraint(current_count, material.count)
			
			max_possible = calculator.calculate()
			
			self.UpdateCraftingState(max_possible)
			
			return max_possible
			
		except Exception as e:
			self.UpdateCraftingState(1)
			return 1

	def UpdateCraftingState(self, max_possible):
		if max_possible < self.MIN_CRAFT_COUNT:
			self.__maxCraftCount = 0
			self.__craftCount = 0
			self.__UpdateCraftCount()
			return
			
		self.__maxCraftCount = min(max_possible, self.MAX_CRAFT_COUNT)
		
		if self.__craftCount < self.MIN_CRAFT_COUNT:
			self.__craftCount = self.MIN_CRAFT_COUNT
		elif self.__craftCount > self.__maxCraftCount:
			self.__craftCount = self.__maxCraftCount
			
		self.__UpdateCraftCount()

	def __UpdateCraftCount(self):
		self.__craftCountText.SetText(str(self.__craftCount))
		
		totalPrice = self.__price * self.__craftCount
		self.SetPrice(totalPrice, False)

		totalPrice = self.__priceCheque * self.__craftCount
		self.SetPriceCheque(totalPrice, False)

		totalPrice = self.__priceAchievement * self.__craftCount
		self.SetPriceAchievement(totalPrice, False)

		self.RegisterMaterials()

	def _GetCraftCount(self, returnAdjustValue=True):
		"""
		Gets the current craft count from the text input.
		
		Args:
			returnAdjustValue (bool): If True, adjusts value to be within valid range
			
		Returns:
			int: Current craft count value (0 if invalid)
		"""
		if not self.__craftCountText or not self.__craftCountText.GetText():
			return 0
			
		text = self.__craftCountText.GetText().strip()
		if not text.isdigit():
			return 0
		
		try:
			value = int(text)
			
			if value < self.MIN_CRAFT_COUNT:
				return self.MIN_CRAFT_COUNT
				
			if returnAdjustValue:
				return min(max(value, self.MIN_CRAFT_COUNT), self.__maxCraftCount)
				
			return value
			
		except ValueError:
			return 0

	def ClearCategories(self):
		for category in self.CatList:
			category.ClearSubCategories()
			category.Destroy()
		
		self.CatList = []
		self.CategoryList.RemoveAllItems()	

	def Refresh(self):
		self.ClearCategories()
		for key, recipe in enumerate(self.__recipes):
			item.SelectItem(recipe.reward.vnum)

			appendCat_ = True
			for category in self.CatList:
				if category.GetData().category == recipe.category:
					category.AddSubCategory(key, recipe)
					appendCat_ = False

			if (appendCat_):
				cat = self.Category(recipe, self.__Rebase)
				cat.SetFoldInEvent(self.__RefreshCategories)
				cat.SetFoldOutEvent(lambda arg=cat: self.__OnCategoryFoldOut(arg))
				cat.Hide()

				cat.AddSubCategory(key, recipe)

				self.CatList.append(cat)

		for cat in self.CatList:
			cat.SetUp()

		self.__RefreshCategories()

	def __Rebase(self, index):
		
	
		self.OnClearMaterials()

		self.__iRecipe = index

		recipe = self.__recipes[index]
		if not recipe: return

		self.__cubeSlot.SetItemSlot(0, recipe.reward.vnum, recipe.reward.count)
		
		
		self.SetPrice(recipe.price)
		self.SetPriceCheque(recipe.priceCheque)
		self.SetPriceAchievement(recipe.priceAchievement)
		self.CalculateMaximumCraftCount()

	def __OnCategoryScroll(self):
		scrollPos = self.CategoryListScrollBar.GetPos()
		basePos = int(scrollPos * self.CategoryList.GetScrollLen())
		if basePos != self.CategoryList.basePos:
			self.CategoryList.SetBasePos(basePos)

	def __OnCategoryFoldOut(self, foldOutCat):
		for cat in self.CatList:
			if cat != foldOutCat:
				cat.SetUp()

		self.__RefreshCategories()

	def __RefreshCategories(self):
		self.CategoryList.RemoveAllItems()

		for cat in self.CatList:
			self.CategoryList.AppendItem(cat)
			if cat.IsFoldOut():
				for subCat in cat.GetSubCategories():
					self.CategoryList.AppendItem(subCat)

			else:
				cat.SetUp()
				cat.HideSubCategories()

		if self.CategoryList.GetScrollLen() > 0:
			oldScrollPos = self.CategoryListScrollBar.GetPos()

			viewItemCount = self.CategoryList.GetViewItemCount()
			itemCount = self.CategoryList.GetItemCount()

			self.CategoryListScrollBar.Show()

			self.CategoryListScrollBar.SetPos(oldScrollPos)

	def SetItem(self, slotIndex, i):
		self.item.SetItem(slotIndex, i)
	
	def GetItem(self, slotIndex):
		return self.item.GetItem(slotIndex)
	
	def RefreshItems(self):
		self.__chanceSlot.RefreshItems(self.item.GetVnum, self.item.GetCount)

	def OnSelectEmptySlot(self, slotIndex):
		if mouseModule.mouseController.isAttached():
			slotType = mouseModule.mouseController.GetAttachedType()
			position = mouseModule.mouseController.GetAttachedSlotNumber()
			
			if not util.IsInventorySlotType(slotType):
				return
			
			windowType = mouseModule.SlotTypeToWindowType(slotType)
			newItem = itemWrapper.ItemToolTipWrapper(windowType, position)
			
			vnum = newItem.GetVnum()
			
			item.SelectItem(vnum)
			if vnum != 71084:
				return
			
			mouseModule.mouseController.DeattachObject()
			
			self.SetItem(slotIndex, newItem)

	def OnSelectItemSlot(self, slotIndex):
		self.SetItem(slotIndex, None)

	def OnSetItem(self, slotIndex):
		self.RefreshItems()

	def Hide(self, isSelf = False):
		if not isSelf:
			self.Close()
		else:
			ui.ScriptWindow.Hide(self)
	
	def Destroy(self):
		self.Reset()
		
		self.__priceText = None
		self.__chanceText = None
		self.__cubeSlot = None
		self.__materialSlots = None
		
		self.ClearDictionary()
	
	def Reset(self):
		if not self.__cubeSlot:
			return
		
		self.__recipes = []
		self.__price = 0
		self.__priceCheque = 0
		self.__priceAchievement = 0
		self.__chance = 0
		self.__craftCount = self.MIN_CRAFT_COUNT
		self.__maxCraftCount = self.MIN_CRAFT_COUNT
		self.__items = [None] * self.__cubeSlot.GetSlotCount()
		
		self.item.Clear()

		self.__startX = 0
		self.__startY = 0
			
	def Open(self):
		(self.__startX, self.__startY, z) = player.GetMainCharacterPosition()
		
		self.RefreshGrids()
		
		self.Show()
	
	def Close(self):
		self.Hide(True)
		self.Reset()
		
	def AddRecipe(self, recipe):
		self.__recipes.append(recipe)
	
	def SetPrice(self, price, update = True):
		if update:
			self.__price = price
		self.__priceText.SetText(localeInfo.FormatMoney(price))
	
	def SetPriceCheque(self, price, update = True):
		if update:
			self.__priceCheque = price

		self.__chequeText.SetText(localeInfo.NumberToCheque(price))
		
	def SetPriceAchievement(self, price, update = True):
		if update:
			self.__priceAchievement = price

		self.__achievementText.SetText(localeInfo.NumberToPktOsiagString(price))

	def OnClearMaterials(self):
		self.__cubeSlot.ClearSlot(0)
		for _ in range(len(self.__materialSlots)):
			materialSlot = self.__materialSlots[_]
			materialSlot.ClearSlot(0)

			self.__materialCounts[_].SetText("")

	def RegisterMaterials(self):
		if self.__iRecipe > -1:
			recipe = self.__recipes[self.__iRecipe]

			for _ in range(len(self.__materialSlots)):
				itemSlot = self.__materialSlots[_]
				
				if _ < len(recipe.items):
					material = recipe.items[_]
				else:
					itemSlot.ClearSlot(0)
					continue
				
				itemSlot.SetItemSlot(0, material.vnum, 0)

				colors = [grp.GenerateColor(0.5411, 0.7254, 0.5568, 1.0), grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0)]
				
				count = player.GetItemCountByVnum(material.vnum)
				self.__materialCounts[_].SetText("{}/{}".format(min(999, count), material.count * self.__craftCount))
				self.__materialCounts[_].SetPackedFontColor(colors[0] if count >= material.count * self.__craftCount else colors[1])
				itemSlot.Show()
	
	def RefreshGrids(self):
		self.__grids = []

	def OnOverInMaterialItem(self, _, row):
		recipe = self.__recipes[self.__iRecipe]

		if row >= len(recipe.items):
			return

		material = recipe.items[row]

		ItemToolTipOnlyTitleDummy(material.vnum).ShowToolTip()

	def OnOverInItem(self, slotIndex):
		recipe = self.__recipes[self.__iRecipe]

		ItemToolTipDummy(recipe.reward.vnum).ShowToolTip()
	
	def OnOverOutItem(self):
		uiToolTip.GetItemToolTipInstance().HideToolTip()
	
	def OnClickAccept(self):
		position = (0, 0) if not self.GetItem(0) else (self.GetItem(0).GetWindow(), self.GetItem(0).GetPosition())
		if self.__iRecipe != -1:
			cube.SendMakePacket(self.__iRecipe, self.__craftCount, position[0], position[1])
	
	def OnClickAcceptAll(self):
		position = (0, 0) if not self.GetItem(0) else (self.GetItem(0).GetWindow(), self.GetItem(0).GetPosition())
		if self.__iRecipe != -1:
			cube.SendMakePacket(self.__iRecipe, 100, position[0], position[1])
	
	def OnClickCancel(self):
		cube.SendClosePacket()
		self.Close()
	
	def OnUpdateInfo(self, price, priceCheque, priceAchievement,  chance):
		self.CalculateMaximumCraftCount()
		self.RefreshItems()
	
	def OnUpdate(self):
		USE_SHOP_LIMIT_RANGE = 2500
		
		(x, y, z) = player.GetMainCharacterPosition()
		if abs(x - self.__startX) > USE_SHOP_LIMIT_RANGE or abs(y - self.__startY) > USE_SHOP_LIMIT_RANGE:
			self.OnClickCancel()
	
	
	def OnPressEscapeKey(self):
		self.OnClickCancel()
		return True
