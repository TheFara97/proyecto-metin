import ui, app, item, net, os, constInfo, localeInfo, uiToolTip, math, snd, wndMgr
#import webbrowser
from _weakref import proxy

itemshopData = {}
playerLog = []

UPDATE_TIME = -1

def ItemShopLogClear():
	global playerLog
	playerLog = []

def ItemShopAppendLog(dateText, dateTime, playerName, ipAdress, itemVnum, itemCount, itemPrice):
	dateTimeReal = "None"
	dateSplit = dateText.split(" ")
	if len(dateSplit) > 1:
		dateFirst = dateSplit[0].split("-")
		if len(dateFirst) == 3:
			dateTimeReal = dateFirst[2]+"/"+dateFirst[1]+"/"+dateFirst[0]
			dateTimeReal+=" "
			dateSecond = dateSplit[1].split(":")
			if len(dateSecond) == 3:
				dateTimeReal += dateSecond[0]+":"+dateSecond[1]

	global playerLog
	playerLog.append([dateTimeReal,dateTime,playerName,ipAdress,itemVnum,itemCount,itemPrice])

def ItemShopClear(time):
	global itemshopData
	global UPDATE_TIME
	if time != UPDATE_TIME:
		itemshopData.clear()
		UPDATE_TIME = time

#USE_ITEMSHOP_RENEWED: @itemPriceJD
def ItemShopAppendItem(categoryIndex, categorySubIndex, itemID, itemVnum, itemPrice, itemDiscount, itemOffertime, itemTopSelling, itemAddedTime, itemSellingCount, itemMaxSellingCount, itemPriceJD = 0):
	if not itemshopData.has_key(categoryIndex):
		subCategoryList = []

		for x in xrange(10):
			subCategoryList.append([])

		itemshopData[categoryIndex] = subCategoryList

	itemshopData[categoryIndex][categorySubIndex].append([itemID, itemVnum, itemPrice, itemDiscount, itemOffertime, itemTopSelling, itemAddedTime, itemSellingCount, itemMaxSellingCount, itemPriceJD])

def ItemShopUpdateItem(itemID, itemVnum, itemPrice, itemDiscount, itemOffertime, itemTopSelling, itemAddedTime, itemSellingCount, itemMaxSellingCount, itemPriceJD = 0):
	global itemshopData

	for key, items in itemshopData.items():
		for subCategory in items:
			for shopItem in subCategory:
				if shopItem[0] == itemID:
					shopItem = [itemID, itemVnum, itemPrice, itemDiscount, itemOffertime, itemTopSelling, itemAddedTime, itemSellingCount, itemMaxSellingCount, itemPriceJD]
					return

buyQuestionDialog = None

LIST_ITEM_ID = 0
LIST_ITEM_VNUM = LIST_ITEM_ID + 1
LIST_ITEM_PRICE = LIST_ITEM_VNUM + 1
LIST_ITEM_DISCOUNT = LIST_ITEM_PRICE + 1
LIST_ITEM_TIME = LIST_ITEM_DISCOUNT + 1
LIST_ITEM_TOP_SELLING_INDEX = LIST_ITEM_TIME + 1
LIST_ITEM_ADDED_TIME = LIST_ITEM_TOP_SELLING_INDEX + 1
LIST_ITEM_SELLING_COUNT = LIST_ITEM_ADDED_TIME + 1
LIST_ITEM_MAX_SELLING_COUNT = LIST_ITEM_SELLING_COUNT + 1

if app.USE_ITEMSHOP_RENEWED:
	LIST_ITEM_JPRICE = LIST_ITEM_MAX_SELLING_COUNT + 1

LOG_DATE_TEXT = 0
LOG_DATE = 1
LOG_ACCOUNT = 2
LOG_IP = 3
LOG_ITEM = 4
LOG_COUNT = 5
LOG_PRICE = 6


HOT_ITEM=1
NEW_ITEM=2
OFFER_ITEM=3

#Config

WEBSITE_LINK      = "https://elvion.mentra.gg/"
NOWPAYMENTS_LINK  = "https://elvion.cc/shop/coins/buy"

eventCategory = [
	# [[FILENAMES],[CATEGORY, SUB_CATEGORY]]
	[["costume_banner_0.png","costume_banner_1.png"],[0,0]],
]

IMG_DIR ="elvion/game/itemshop/"

SPECIAL_OFFER_CATEGORY    = 99   # DB categoryType value for special offer items
SPECIAL_OFFER_SHOW_TIME   = 5.0  # seconds each item is fully visible
SPECIAL_OFFER_FADE_TIME   = 0.6  # seconds for each fade-in / fade-out transition

BANNER_SHOW_TIME = 4.0  # seconds each banner image is fully visible
BANNER_FADE_TIME = 0.6  # seconds for each banner fade-in / fade-out

WHEEL_DIR ="elvion/game/itemshop/wheel/default/"

wheel_christmas_items = [\
	[80460,5],[80450,5],[80458,5],[30502,5],[30505,5],[85004,1],[79016,2],[100700,2],[23000,1],[90029,2],[34000,2],[31150,5],[31151,5],[31152,5],[31153,5],[31154,5]\
	,[31155,5],[203032,5],[91232,1],[91233,1],[91234,1],[91235,1]
]

wheel_random_items = [\
	[50166,1],[50167,1],[80040,1],[203000,3],[203001,1],[203002,1],[203003,1],[80032,3],[10151,1],[10156,1],[10161,1],[61400,1],[79016,2],[79016,3],[79016,4],[100700,2]\
	,[100700,3],[100700,4],[31150,3],[31151,3],[31152,3],[31153,3],[31154,3],[31155,3],[90029,1],[203034,1],[71180,1],[56004,1]
]

wheel_cheque_items = [\
	[91236,1],[91237,1],[91238,1],[91239,1],[91240,1],[91241,1],[91242,1],[91243,1],[91244,1],[201000,100],[115100,1],[125100,1],[135100,1],[145100,1],[155100,1],[165100,1]\
	,[100700,2],[100700,3],[79016,5],[79016,7],[79016,10],[71052,10],[71052,20],[79012,10],[79012,20],[203034,1],[71180,1],[203012,1],[52959,1],[53947,1]
]

itemShopCategoryData = {
	0:{
		"name":localeInfo.CATEGORY_0_NAME,
		"subCategory":[],
		},
	1:{
		"name":localeInfo.CATEGORY_1_NAME,
		"subCategory":[],
		},
	2:{
		"name":localeInfo.CATEGORY_2_NAME,
		"subCategory":[localeInfo.CATEGORY_2_SUB_0_NAME,localeInfo.CATEGORY_2_SUB_1_NAME,localeInfo.CATEGORY_2_SUB_2_NAME,localeInfo.CATEGORY_2_SUB_3_NAME],
		},
	3:{
		"name":localeInfo.CATEGORY_3_NAME,
		"subCategory":[localeInfo.CATEGORY_3_SUB_0_NAME,localeInfo.CATEGORY_3_SUB_2_NAME],
		},
	4:{
		"name":localeInfo.CATEGORY_8_NAME,
		"subCategory":[],
		},
	5:{
		"name":localeInfo.CATEGORY_5_NAME,
		"subCategory":[],
		},
}

def calculateRect(curValue, maxValue):
	try:
		return -1.0 + float(curValue) / float(maxValue)
	except:
		return 0.0

def RenderWindow(child, _wx, _wy, _height):
	(_x, _y) = child.GetGlobalPosition()
	childHeight = child.GetHeight()
	
	TOP_MARGIN = 0
	BOTTOM_MARGIN = 35
	
	if _y + childHeight < (_wy - TOP_MARGIN) or _y > (_wy + _height + BOTTOM_MARGIN):
		if child.IsShow():
			child.Hide()
		HideChildElements(child)
		return
	
	if isinstance(child, ui.ExpandedImageBox):
		itsRendered = False
		topRender = 0
		bottomRender = 0
		
		if _y < (_wy + TOP_MARGIN):
			topRender = (_wy + TOP_MARGIN) - _y
			itsRendered = True
		
		if _y + childHeight > (_wy + _height - BOTTOM_MARGIN):
			bottomRender = (_wy + _height - BOTTOM_MARGIN) - (_y + childHeight)
			itsRendered = True
		
		if itsRendered:
			child.SetRenderingRect(
				0, 
				calculateRect(childHeight - abs(topRender), childHeight), 
				0, 
				calculateRect(childHeight - abs(bottomRender), childHeight)
			)
			HideChildElements(child)
		else:
			child.SetRenderingRect(0, 0, 0, 0)
			ShowChildElements(child)
	else:
		if _y < (_wy + TOP_MARGIN) or _y + childHeight > (_wy + _height - BOTTOM_MARGIN):
			if child.IsShow():
				child.Hide()
			HideChildElements(child)
			return
		else:
			ShowChildElements(child)
	
	if not child.IsShow():
		child.Show()

class Grid:
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

class WheelRewardPreviewWindow(ui.BoardWithTitleBar):
	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.SetSize(460, 350)
		self.SetTitleName("Wheel Rewards")
		self.SetCloseEvent(self.Close)
		self.AddFlag("movable")
		self.AddFlag("attach")
		self.AddFlag("float")
		
		wndItem = ui.GridSlotWindow()
		wndItem.SetParent(self)
		wndItem.SetPosition(15, 35)
		wndItem.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
		wndItem.SetOverInItemEvent(ui.__mem_func__(self.__OverIn))
		wndItem.SetOverOutItemEvent(ui.__mem_func__(self.__OverOut))
		wndItem.ArrangeSlot(0, 12, 8, 32, 32, 0, 0)
		wndItem.RefreshSlot()
		wndItem.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)
		wndItem.Show()
		wndItem.itemDataDict = {}
		self.wndItem = wndItem
		
	def __OverIn(self, index=-1):
		interface = constInfo.GetInterfaceInstance()
		if not interface:
			return
		if index >= 0 and self.wndItem.itemDataDict.has_key(index):
			interface.tooltipItem.SetItemToolTip(self.wndItem.itemDataDict[index])

	def __OverOut(self, index=-1):
		interface = constInfo.GetInterfaceInstance()
		if not interface:
			return
		interface.tooltipItem.HideToolTip()
	
	def LoadRewardData(self, itemList):
		itemDataDict = {}
		for j in xrange(12 * 8):
			self.wndItem.ClearSlot(j)
		grid = Grid(12, 8)
		for itemData in itemList:
			item.SelectItem(itemData[0])
			itemSize = item.GetItemSize()
			pos = grid.find_blank(itemSize[0], itemSize[1])
			if pos == -1:
				continue
			grid.put(pos, itemSize[0], itemSize[1])
			itemDataDict[pos] = itemData[0]
			self.wndItem.SetItemSlot(pos, itemData[0], 0 if itemData[1] == 1 else itemData[1])
		self.wndItem.itemDataDict = itemDataDict
		self.wndItem.RefreshSlot()
	
	def Open(self):
		self.SetCenterPosition()
		self.SetTop()
		self.Show()
	
	def Close(self):
		self.Hide()
	
	def OnPressEscapeKey(self):
		self.Close()
		return True

def HideChildElements(parent):
	if hasattr(parent, 'children'):
		for key, childElement in parent.children.items():
			if childElement and childElement.IsShow():
				childElement.Hide()

def ShowChildElements(parent):
	if hasattr(parent, 'children'):
		for key, childElement in parent.children.items():
			if childElement and not childElement.IsShow():
				childElement.Show()

class CategoryList(ui.Window):
	def __del__(self):
		ui.Window.__del__(self)
	
	def Destroy(self):
		self.scrollBar = None
		self.selectedItem = None
		self.itemList = []
		self.basePos = 0
	
	def __init__(self):
		ui.Window.__init__(self)
		self.Destroy()
	
	def SetScrollBar(self, scrollBar):
		scrollBar.SetScrollEvent(ui.__mem_func__(self.RefreshList))
		self.scrollBar = scrollBar
	
	def OnMouseWheel(self, nLen):
		if self.scrollBar:
			if nLen > 0:
				self.scrollBar.OnUp()
			else:
				self.scrollBar.OnDown()
			return True
		return False
	
	def GetSelectedItem(self):
		return self.selectedItem
	
	def SelectItem(self, selectedItem):
		self.selectedItem = selectedItem
		if selectedItem != None and selectedItem.GetSubCategory() != 1:
			for item in self.itemList:
				if item != selectedItem and item.IsExpanded():
					item.Collapse()
			
			if selectedItem.IsExpanded():
				selectedItem.Collapse()
			else:
				selectedItem.Expand()
				selectedItem.SetTop()
				if hasattr(selectedItem, 'itemList'):
					for subItem in selectedItem.itemList:
						subItem.SetTop()
		
		self.RefreshList()
	
	def SetBasePos(self, basePos):
		self.basePos = basePos
		self.RefreshList()
	
	def ClearItem(self):
		self.selectedItem = None
		for j in xrange(len(self.itemList)):
			if hasattr(self.itemList[j], 'itemList'):
				itemList = self.itemList[j].itemList
				for x in xrange(len(itemList)):
					itemList[x].Hide()
					itemList[x].Destroy()
				itemList = []
		self.itemList = []
		if self.scrollBar:
			self.scrollBar.SetPos(0)
		self.SetBasePos(0)
		
	def RemoveAllItems(self):
		self.ClearItem()
	
	def AppendItemList(self, categoryData):
		self.ClearItem()
		for categoryList in categoryData:
			categoryBtn = None
			if categoryList.has_key("item"):
				categoryBtn = categoryList["item"]
				categoryBtn.SetParent(self)
				if categoryList.has_key("children"):
					for _x in categoryList["children"]:
						childItems = _x["item"]
						childItems.SetParent(self)
						categoryBtn.itemList.append(childItems)
			if categoryBtn != None:
				self.itemList.append(categoryBtn)
		self.RefreshList()
	
	def RefreshDynamicPosition(self):
		x_pos = 0
		y_pos = 0
		max_height = 0
		button_width = 150
		button_spacing = 7
		
		for j in xrange(len(self.itemList)):
			categoryBtn = self.itemList[j]
			categoryBtn.SetPosition(x_pos, 0, True)
			
			if categoryBtn.IsExpanded() and hasattr(categoryBtn, 'itemList'):
				sub_y = 36  # Start position for subcategories
				for x in xrange(len(categoryBtn.itemList)):
					childItem = categoryBtn.itemList[x]
					childItem.SetPosition(x_pos + childItem.GetOffset(), sub_y, True)
					sub_y += 34
				
				if sub_y > max_height:
					max_height = sub_y
			
			x_pos += button_width + button_spacing
	
		return max(40, max_height) if max_height > 0 else 40
	
	def RefreshList(self):
		screenHeight = self.RefreshDynamicPosition()
		self.SetSize(self.GetWidth(), screenHeight)

		if self.scrollBar:
			self.scrollBar.Hide()

		(_wx, _wy) = self.GetGlobalPosition()
		_height = self.GetHeight()
		
		for j in xrange(len(self.itemList)):
			categoryBtn = self.itemList[j]
			categoryBtn.Show()
			
			if categoryBtn.IsExpanded() and hasattr(categoryBtn, 'itemList'):
				for x in xrange(len(categoryBtn.itemList)):
					categoryBtn.itemList[x].Show()
			else:
				if hasattr(categoryBtn, 'itemList'):
					for x in xrange(len(categoryBtn.itemList)):
						categoryBtn.itemList[x].Hide()

class CategoryItemNew(ui.ExpandedImageBox):
	def __del__(self):
		ui.ExpandedImageBox.__del__(self)
	
	def __init__(self, text, categoryIndex):
		ui.ExpandedImageBox.__init__(self)
		self.children = {}
		self.categoryIndex = categoryIndex
		self.expanded = False
		self.event = None
		self.onCollapseEvent = None
		self.onExpandEvent = None
		self.isSubCategory = 0
		self.itemList = []
		self.offset = 7
		
		self.LoadImage(IMG_DIR + "button2_0.png")
		self.SetSize(154, 39)
		
		hasSubCategories = False
		if itemShopCategoryData.has_key(categoryIndex):
			categoryData = itemShopCategoryData[categoryIndex]["subCategory"]
			hasSubCategories = len(categoryData) > 0
		
		if hasSubCategories:
			arrowIcon = ui.ExpandedImageBox()
			arrowIcon.SetParent(self)
			arrowIcon.AddFlag("not_pick")
			arrowIcon.SetPosition(65, 36)
			arrowIcon.LoadImage(IMG_DIR + "arrow_down.png")
			arrowIcon.Show()
			self.children["arrowIcon"] = arrowIcon
		
		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.AddFlag("not_pick")
		textLine.SetPosition(79, 11)
		textLine.SetHorizontalAlignCenter()
		textLine.SetFontName("Verdana:12b")
		textLine.SetText(text)
		textLine.Show()
		self.children["textLine"] = textLine
		
		self.SetOnExpandEvent(self.ExpandEvent)
		self.SetOnCollapseEvent(self.CollapseEvent)
	
	def SetParent(self, parent):
		ui.ExpandedImageBox.SetParent(self, parent)
		self.parent = ui.proxy(parent)
	
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
	
	def SetSubCategory(self, flag):
		self.isSubCategory = flag
	
	def GetSubCategory(self):
		return self.isSubCategory
	
	def SetOnExpandEvent(self, event):
		self.onExpandEvent = event
	
	def SetOnCollapseEvent(self, event):
		self.onCollapseEvent = event
	
	def SetEvent(self, event):
		self.event = event
	
	def SetOffset(self, offset):
		self.offset = offset
	
	def GetOffset(self):
		return self.offset
	
	def OnSelect(self):
		if self.event:
			self.event()
		if self.GetSubCategory() != 1 and hasattr(self, 'parent'):
			self.parent.SelectItem(self)
	
	def OnMouseLeftButtonDown(self):
		self.OnSelect()
	
	def CollapseEvent(self):
		self.LoadImage(IMG_DIR + "button2_0.png")
		if self.children.has_key("arrowIcon"):
			self.children["arrowIcon"].LoadImage(IMG_DIR + "arrow_down.png")
			self.children["arrowIcon"].SetPosition(65, 36)
		
		if hasattr(self, 'itemList'):
			delay = 0.0
			for subItem in self.itemList:
				if hasattr(subItem, 'StartCollapseAnimation'):
					subItem.StartCollapseAnimation(delay)
				delay += 0.03
	
	def ExpandEvent(self):
		self.LoadImage(IMG_DIR + "button2_1.png")
		if self.children.has_key("arrowIcon"):
			self.children["arrowIcon"].LoadImage(IMG_DIR + "arrow_up.png")
			self.children["arrowIcon"].SetPosition(65, 24)
		
		if hasattr(self, 'itemList'):
			delay = 0.0
			for subItem in self.itemList:
				if hasattr(subItem, 'StartPopAnimation'):
					subItem.StartPopAnimation(delay)
				delay += 0.05
	
	def Hide(self):
		ui.ExpandedImageBox.Hide(self)
		HideChildElements(self)
	
	def Show(self):
		ui.ExpandedImageBox.Show(self)
		ShowChildElements(self)

class CategorySubItemNew(ui.ExpandedImageBox):
	def __del__(self):
		ui.ExpandedImageBox.__del__(self)
	
	def __init__(self, text, categoryIndex, categorySubIndex):
		ui.ExpandedImageBox.__init__(self)
		self.children = {}
		self.categoryIndex = categoryIndex
		self.categorySubIndex = categorySubIndex
		self.expanded = False
		self.event = None
		self.onCollapseEvent = None
		self.onExpandEvent = None
		self.isSubCategory = 1
		self.itemList = []
		self.offset = 3
		
		self.isAnimating = False
		self.animStartTime = 0.0
		self.animDuration = 0.2
		self.animationType = "none"
		self.targetScale = 1.0
		self.startScale = 1.0
		self.currentScale = 1.0
		
		self.LoadImage(IMG_DIR + "subcategory_btn_norm.png")
		self.SetSize(149, 34)
		
		# Add subcategory arrow icon
		#subCategoryIcon = ui.ExpandedImageBox()
		#subCategoryIcon.SetParent(self)
		#subCategoryIcon.AddFlag("not_pick")
		#subCategoryIcon.SetPosition(10, 13)
		#try:
		#	subCategoryIcon.LoadImage(IMG_DIR + "arrow_subcategory.png")
		#except:
		#	pass
		#subCategoryIcon.Show()
		#self.children["subCategoryIcon"] = subCategoryIcon
		
		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.AddFlag("not_pick")
		textLine.SetPosition(72, 9)
		textLine.SetHorizontalAlignCenter()
		textLine.SetFontName("Verdana:12b")
		textLine.SetText("  " + text)
		textLine.Show()
		self.children["textLine"] = textLine
		
		self.SetOnExpandEvent(self.ExpandEvent)
		self.SetOnCollapseEvent(self.CollapseEvent)
	
	def StartPopAnimation(self, delay=0.0):
		self.isAnimating = True
		self.animationType = "expand"
		self.animStartTime = app.GetTime() + delay
		self.startScale = 0.5
		self.targetScale = 1.0
		self.currentScale = self.startScale
		
		self.SetAlpha(0.3)
		self.SetScale(self.startScale, self.startScale)
	
	def StartCollapseAnimation(self, delay=0.0):
		self.isAnimating = True
		self.animationType = "collapse"
		self.animStartTime = app.GetTime() + delay
		self.animDuration = 0.15
		self.startScale = 1.0
		self.targetScale = 0.3
		self.currentScale = self.startScale
	
	def OnUpdate(self):
		if not self.isAnimating:
			return
		
		now = app.GetTime()
		
		if now < self.animStartTime:
			return
		
		elapsed = now - self.animStartTime
		
		if elapsed >= self.animDuration:
			self.isAnimating = False
			
			if self.animationType == "expand":
				self.currentScale = self.targetScale
				self.SetScale(1.0, 1.0)
				self.SetAlpha(1.0)
			elif self.animationType == "collapse":
				self.SetAlpha(0.0)
				self.SetScale(0.3, 0.3)
			return
		
		t = elapsed / self.animDuration
		
		if self.animationType == "expand":
			if t < 0.5:
				easedT = self.__EaseOutBack(t * 2.0) * 0.5
			else:
				easedT = 0.5 + self.__EaseOutQuad((t - 0.5) * 2.0) * 0.5
			
			self.currentScale = self.startScale + (self.targetScale - self.startScale) * easedT
			
			overshoot = 1.0
			if t < 0.6:
				overshoot = 1.0 + (0.15 * (1.0 - t / 0.6))
			
			finalScale = self.currentScale * overshoot
			self.SetScale(finalScale, finalScale)
			
			alpha = min(1.0, t * 2.5)
			self.SetAlpha(alpha)
			
		elif self.animationType == "collapse":
			easedT = self.__EaseInQuad(t)
			
			self.currentScale = self.startScale + (self.targetScale - self.startScale) * easedT
			self.SetScale(self.currentScale, self.currentScale)
			
			alpha = 1.0 - (t * 1.5)
			alpha = max(0.0, alpha)
			self.SetAlpha(alpha)
	
	def __EaseOutBack(self, t):
		c1 = 1.70158
		c3 = c1 + 1.0
		return 1.0 + c3 * pow(t - 1.0, 3.0) + c1 * pow(t - 1.0, 2.0)
	
	def __EaseOutQuad(self, t):
		return 1.0 - (1.0 - t) * (1.0 - t)
	
	def __EaseInQuad(self, t):
		return t * t
	
	def SetParent(self, parent):
		ui.ExpandedImageBox.SetParent(self, parent)
		self.parent = ui.proxy(parent)
	
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
	
	def SetSubCategory(self, flag):
		self.isSubCategory = flag
	
	def GetSubCategory(self):
		return self.isSubCategory
	
	def SetOnExpandEvent(self, event):
		self.onExpandEvent = event
	
	def SetOnCollapseEvent(self, event):
		self.onCollapseEvent = event
	
	def SetEvent(self, event):
		self.event = event
	
	def SetOffset(self, offset):
		self.offset = offset
	
	def GetOffset(self):
		return self.offset
	
	def OnSelect(self):
		if self.event:
			self.event()
	
	def OnMouseLeftButtonDown(self):
		self.OnSelect()
	
	def CollapseEvent(self):
		self.LoadImage(IMG_DIR + "subcategory_btn_norm.png")
	
	def ExpandEvent(self):
		self.LoadImage(IMG_DIR + "subcategory_btn_hover.png")
	
	def Hide(self):
		ui.ExpandedImageBox.Hide(self)
		HideChildElements(self)
		self.isAnimating = False
	
	def Show(self):
		if not self.isAnimating:
			self.SetScale(1.0, 1.0)
			self.SetAlpha(1.0)
		
		ui.ExpandedImageBox.Show(self)
		ShowChildElements(self)

class ScrollBarNew(ui.Window):
	SCROLLBAR_WIDTH = 7
	SCROLL_BTN_XDIST = 0
	SCROLL_BTN_YDIST = 0
	
	class MiddleBar(ui.DragButton):
		def __init__(self):
			ui.DragButton.__init__(self)
			self.AddFlag("movable")
			self.SetWindowName("scrollbar_middlebar")
		
		def MakeImage(self):
			try:
				top = ui.ExpandedImageBox()
				top.SetParent(self)
				top.LoadImage(IMG_DIR+"scrollbar_ex/scrollbar_top.tga")
				top.AddFlag("not_pick")
				top.Show()
				
				topScale = ui.ExpandedImageBox()
				topScale.SetParent(self)
				topScale.SetPosition(0, top.GetHeight())
				topScale.LoadImage(IMG_DIR+"scrollbar_ex/scrollbar_scale.tga")
				topScale.AddFlag("not_pick")
				topScale.Show()
				
				bottom = ui.ExpandedImageBox()
				bottom.SetParent(self)
				bottom.LoadImage(IMG_DIR+"scrollbar_ex/scrollbar_bottom.tga")
				bottom.AddFlag("not_pick")
				bottom.Show()
				
				bottomScale = ui.ExpandedImageBox()
				bottomScale.SetParent(self)
				bottomScale.LoadImage(IMG_DIR+"scrollbar_ex/scrollbar_scale.tga")
				bottomScale.AddFlag("not_pick")
				bottomScale.Show()
				
				middle = ui.ExpandedImageBox()
				middle.SetParent(self)
				middle.LoadImage(IMG_DIR+"scrollbar_ex/scrollbar_mid.tga")
				middle.AddFlag("not_pick")
				middle.Show()
				
				self.top = top
				self.topScale = topScale
				self.bottom = bottom
				self.bottomScale = bottomScale
				self.middle = middle
			except:
				simple = ui.Bar()
				simple.SetParent(self)
				simple.SetColor(0x77000000)
				simple.Show()
				self.simple = simple
		
		def SetSize(self, height):
			if hasattr(self, 'top'):
				minHeight = self.top.GetHeight() + self.bottom.GetHeight() + self.middle.GetHeight()
				height = max(minHeight, height)
				ui.DragButton.SetSize(self, 10, height)
				scale = (height - minHeight) / 2
				extraScale = 0
				if (height - minHeight) % 2 == 1:
					extraScale = 1
				self.topScale.SetRenderingRect(0, 0, 0, scale - 1)
				self.middle.SetPosition(0, self.top.GetHeight() + scale)
				self.bottomScale.SetPosition(0, self.middle.GetBottom())
				self.bottomScale.SetRenderingRect(0, 0, 0, scale - 1 + extraScale)
				self.bottom.SetPosition(0, height - self.bottom.GetHeight())
			else:
				ui.DragButton.SetSize(self, 10, height)
				if hasattr(self, 'simple'):
					self.simple.SetSize(10, height)
	
	def __init__(self):
		ui.Window.__init__(self)
		self.pageSize = 1
		self.curPos = 0.0
		self.eventScroll = None
		self.eventArgs = None
		self.lockFlag = False
		self.CreateScrollBar()
		self.SetScrollBarSize(0)
		self.scrollStep = 0.2
		self.SetWindowName("NONAME_ScrollBar")
	
	def __del__(self):
		ui.Window.__del__(self)
	
	def CreateScrollBar(self):
		try:
			topImage = ui.ExpandedImageBox()
			topImage.SetParent(self)
			topImage.AddFlag("not_pick")
			topImage.LoadImage(IMG_DIR+"scrollbar_ex/scroll_top.tga")
			topImage.Show()
			
			bottomImage = ui.ExpandedImageBox()
			bottomImage.SetParent(self)
			bottomImage.AddFlag("not_pick")
			bottomImage.LoadImage(IMG_DIR+"scrollbar_ex/scroll_bottom.tga")
			bottomImage.Show()
			
			middleImage = ui.ExpandedImageBox()
			middleImage.SetParent(self)
			middleImage.AddFlag("not_pick")
			middleImage.SetPosition(0, topImage.GetHeight())
			middleImage.LoadImage(IMG_DIR+"scrollbar_ex/scroll_mid.tga")
			middleImage.Show()
			
			self.topImage = topImage
			self.bottomImage = bottomImage
			self.middleImage = middleImage
		except:
			self.topImage = None
			self.bottomImage = None
			self.middleImage = None
		
		middleBar = self.MiddleBar()
		middleBar.SetParent(self)
		middleBar.SetMoveEvent(ui.__mem_func__(self.OnMove))
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
		
		if self.middleImage:
			middleImageScale = float((height - self.SCROLL_BTN_YDIST*2) - self.middleImage.GetHeight()) / float(self.middleImage.GetHeight())
			self.middleImage.SetRenderingRect(0, 0, 0, middleImageScale)
		if self.bottomImage:
			self.bottomImage.SetPosition(0, height - self.bottomImage.GetHeight())
		
		self.middleBar.SetRestrictMovementArea(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST, \
			self.middleBar.GetWidth(), height - self.SCROLL_BTN_YDIST * 2)
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST)
	
	def SetScrollStep(self, step):
		self.scrollStep = step
	
	def OnUp(self):
		self.SetPos(self.curPos - self.scrollStep)
	
	def OnDown(self):
		self.SetPos(self.curPos + self.scrollStep)
	
	def GetScrollStep(self):
		return self.scrollStep
	
	def GetPos(self):
		return self.curPos
	
	def SetPos(self, pos, moveEvent=True):
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

class ItemShopWindow(ui.Window):
	def __del__(self):
		ui.Window.__del__(self)
	def Destroy(self):
		ItemShopLogClear()
		if self.children.has_key("categoryListBox"):
			self.children["categoryListBox"].RemoveAllItems()
		if self.children.has_key("itemListBox"):
			itemListBox = self.children["itemListBox"]
			if hasattr(itemListBox, 'children'):
				for child in itemListBox.children.values():
					if hasattr(child, 'Destroy'):
						child.Destroy()
					child.Hide()
				itemListBox.children.clear()
		if self.children.has_key("PurchasesListBox"):
			purchasesListBox = self.children["PurchasesListBox"]
			if hasattr(purchasesListBox, 'children'):
				for child in purchasesListBox.children.values():
					if hasattr(child, 'Destroy'):
						child.Destroy()
					child.Hide()
				purchasesListBox.children.clear()
		self.CloseWeb()
		
		if self.promoCodeWindow:
			self.promoCodeWindow.Close()
			self.promoCodeWindow = None
	
		global buyQuestionDialog
		if buyQuestionDialog != None:
			buyQuestionDialog.Hide()
			buyQuestionDialog.Destroy()
			buyQuestionDialog=None
		if self.wheelRewardPreview:
			self.wheelRewardPreview.Close()
			self.wheelRewardPreview = None

		self.RealCoin = -1
		self.NewCoin = 0
		self.CoinRange = 0
		self.CoinFlag = False
		self.CoinSize = 0

		if app.USE_ITEMSHOP_RENEWED:
			self.RealJeton = -1
			self.NewJeton = 0
			self.JetonRange = 0
			self.JetonFlag = False
			self.JetonSize = 0

		self.children = {}

	def __init__(self):
		ui.Window.__init__(self)
		self.children = {}
		self.promoCodeWindow = None
		self.moneyInitialized = False
		self.lastMoney = 0
		self.wheelRewardPreview = None
		self.isAnimatingMoney = False
		self.animStart = 0.0
		self.animFrom = 0
		self.animTo = 0
		self.animDuration = 0.4
	
		# Add pulse state variables
		self.coinSlotPulse = False
		self.coinPulseStart = 0.0
		if app.USE_ITEMSHOP_RENEWED:
			# Smooth Jeton Animation state
			self.jetonInitialized = False
			self.lastJeton = 0
			self.isAnimatingJeton = False
			self.animStartJeton = 0.0
			self.animFromJeton = 0
			self.animToJeton = 0
			self.animDurationJeton = 0.4
		
			# Add jeton pulse state variables
			self.jetonSlotPulse = False
			self.jetonPulseStart = 0.0

		# Special Offer cycling state
		self.specialOfferItems = []
		self.specialOfferIndex = 0
		self.specialOfferTimer = 0.0
		self.specialOfferState = "idle"  # "idle" | "show" | "fading_out" | "fading_in"
		self.specialOfferAlpha = 0.0
		self.specialOfferCurrentData = None
		self.specialOfferNavDir = 1  # +1 = next, -1 = prev

		self.Destroy()
		self.LoadWindow()

	def LoadWindow(self):
		self.AddFlag("movable")
		self.AddFlag("attach")
		self.AddFlag("float")

		boardImage = ui.ImageBox()
		boardImage.SetParent(self)
		boardImage.AddFlag("attach")
		boardImage.LoadImage(IMG_DIR+"bg.png")
		self.SetSize(boardImage.GetWidth(),boardImage.GetHeight())
		boardImage.SetPosition(0,0)
		boardImage.Show()
		self.children["boardImage"] = boardImage

		closeBtn = ui.Button()
		closeBtn.SetParent(boardImage)
		closeBtn.SetUpVisual(IMG_DIR+"close_0.png")
		closeBtn.SetOverVisual(IMG_DIR+"close_1.png")
		closeBtn.SetDownVisual(IMG_DIR+"close_2.png")
		closeBtn.SetEvent(self.Close)
		closeBtn.SetPosition(boardImage.GetWidth()-closeBtn.GetWidth()-5,5)
		closeBtn.Show()
		self.children["closeBtn"] = closeBtn
		
		dragonCoinsPlaceholder = ui.TextLine()
		dragonCoinsPlaceholder.SetParent(boardImage)
		dragonCoinsPlaceholder.SetPosition(21, 12)
		dragonCoinsPlaceholder.SetFontName("Tahoma:14")
		dragonCoinsPlaceholder.SetPackedFontColor(0xFFc3c3c3)
		dragonCoinsPlaceholder.SetText("Dragon Coins")
		dragonCoinsPlaceholder.Show()
		self.children["dragonCoinsPlaceholder"] = dragonCoinsPlaceholder

		if app.USE_ITEMSHOP_RENEWED:
			dragonCoinText = ui.TextLine()
		else:
			dragonCoinText = MultiTextLine()

		dragonCoinText.SetParent(boardImage)

		dragonCoinText.SetPosition(56, 34)

		if not app.USE_ITEMSHOP_RENEWED:
			#dragonCoinText.SetTextRange(17)
			dragonCoinText.SetTextType("horizontal#left")

		dragonCoinText.SetFontName("Tahoma:12")

		if not app.USE_ITEMSHOP_RENEWED:
			dragonCoinText.SetOutline(True)

		dragonCoinText.Show()
		self.children["dragonCoinText"] = dragonCoinText

		if app.USE_ITEMSHOP_RENEWED:
			if app.USE_ITEMSHOP_RENEWED:
				dragonJCoinText = ui.TextLine()
			else:
				dragonJCoinText = MultiTextLine()

			dragonJCoinText.SetParent(boardImage)

			dragonJCoinText.SetPosition(51, 38)

			dragonJCoinText.SetFontName("Tahoma:12")
			dragonJCoinText.Hide()
			self.children["dragonJCoinText"] = dragonJCoinText

		buyCoinBtn = ui.Button()
		buyCoinBtn.SetParent(boardImage)
		buyCoinBtn.SetUpVisual(IMG_DIR+"home_btn_norm.png")
		buyCoinBtn.SetOverVisual(IMG_DIR+"home_btn_hover.png")
		buyCoinBtn.SetDownVisual(IMG_DIR+"home_btn_down.png")
		buyCoinBtn.SetEvent(self.LandingPage)
		buyCoinBtn.SetPosition(335, 14)
		buyCoinBtn.Show()
		self.children["buyCoinBtn"] = buyCoinBtn

		buyCoinBtnText = MultiTextLine()
		buyCoinBtnText.SetParent(buyCoinBtn)
		#buyCoinBtnText.SetTextRange(17)
		buyCoinBtnText.SetPosition(buyCoinBtn.GetWidth()/2,10)
		buyCoinBtnText.SetTextType("horizontal#center")
		buyCoinBtnText.SetFontName("Verdana:12")
		buyCoinBtnText.SetText(localeInfo.ITEMSHOP_BUY_COIN)
		buyCoinBtnText.Show()
		buyCoinBtn.multiText = buyCoinBtnText

		purchasesBtn = ui.Button()
		purchasesBtn.SetParent(boardImage)
		purchasesBtn.SetUpVisual(IMG_DIR+"purchases_btn_norm.png")
		purchasesBtn.SetOverVisual(IMG_DIR+"purchases_btn_hover.png")
		purchasesBtn.SetDownVisual(IMG_DIR+"purchases_btn_down.png")
		purchasesBtn.SetEvent(self.PurchaseBtn)
		purchasesBtn.SetPosition(442, 14)
		purchasesBtn.Show()
		self.children["purchasesBtn"] = purchasesBtn

		purchasesBtnText = MultiTextLine()
		purchasesBtnText.SetParent(purchasesBtn)
		#purchasesBtnText.SetTextRange(17)
		purchasesBtnText.SetPosition(purchasesBtn.GetWidth()/2,10)
		purchasesBtnText.SetTextType("horizontal#center")
		purchasesBtnText.SetFontName("Verdana:12")
		purchasesBtnText.SetText(localeInfo.ITEMSHOP_PURCHASES)
		purchasesBtnText.Show()
		purchasesBtn.multiText = purchasesBtnText

		wheelBtn = ui.Button()
		wheelBtn.SetParent(boardImage)
		wheelBtn.SetUpVisual(IMG_DIR+"buy_coins_0.png")
		wheelBtn.SetOverVisual(IMG_DIR+"buy_coins_1.png")
		wheelBtn.SetDownVisual(IMG_DIR+"buy_coins_2.png")
		wheelBtn.SetEvent(self.BuyDragonCoin2)
		wheelBtn.SetPosition(549, 14)
		wheelBtn.Show()
		self.children["wheelBtn"] = wheelBtn

		wheelBtnText = MultiTextLine()
		wheelBtnText.SetParent(wheelBtn)
		#wheelBtnText.SetTextRange(17)
		wheelBtnText.SetPosition(wheelBtn.GetWidth()/2,10)
		wheelBtnText.SetTextType("horizontal#center")
		wheelBtnText.SetFontName("Verdana:12")
		wheelBtnText.SetText(localeInfo.ITEMSHOP_WHEEL)
		wheelBtnText.Show()
		wheelBtn.multiText = wheelBtnText

		hotItemsBtn = ui.Button()
		hotItemsBtn.SetParent(boardImage)
		hotItemsBtn.SetUpVisual(IMG_DIR+"hot_btn_norm.png")
		hotItemsBtn.SetOverVisual(IMG_DIR+"hot_btn_hover.png")
		hotItemsBtn.SetDownVisual(IMG_DIR+"hot_btn_down.png")
		hotItemsBtn.SetEvent(self.HotItems)
		hotItemsBtn.SetPosition(656, 14)
		hotItemsBtn.Show()
		self.children["hotItemsBtn"] = hotItemsBtn

		hotItemsBtnText = MultiTextLine()
		hotItemsBtnText.SetParent(hotItemsBtn)
		#hotItemsBtnText.SetTextRange(17)
		hotItemsBtnText.SetPosition(hotItemsBtn.GetWidth()/2,10)
		hotItemsBtnText.SetTextType("horizontal#center")
		hotItemsBtnText.SetFontName("Verdana:12")
		hotItemsBtnText.SetText(localeInfo.ITEMSHOP_HOTITEMS)
		hotItemsBtnText.Show()
		hotItemsBtn.multiText = hotItemsBtnText

		promoBtn = ui.Button()
		promoBtn.SetParent(boardImage)
		promoBtn.SetUpVisual(IMG_DIR+"offers_btn_norm.png")
		promoBtn.SetOverVisual(IMG_DIR+"offers_btn_hover.png")
		promoBtn.SetDownVisual(IMG_DIR+"offers_btn_down.png")
		promoBtn.SetEvent(ui.__mem_func__(self.PromoCode))
		promoBtn.SetPosition(763, 14)
		promoBtn.Show()
		self.children["promoBtn"] = promoBtn

		promoBtnText = MultiTextLine()
		promoBtnText.SetParent(promoBtn)
		#promoBtnText.SetTextRange(17)
		promoBtnText.SetPosition(promoBtn.GetWidth()/2+15,10)
		promoBtnText.SetTextType("horizontal#center")
		promoBtnText.SetFontName("Verdana:12b")
		promoBtnText.SetText(localeInfo.ITEMSHOP_PROMOCODE)
		promoBtnText.Show()
		promoBtn.multiText = promoBtnText

		# offersBtn = ui.Button()
		# offersBtn.SetParent(boardImage)
		# offersBtn.SetUpVisual(IMG_DIR+"button2_0.tga")
		# offersBtn.SetOverVisual(IMG_DIR+"button2_1.tga")
		# offersBtn.SetDownVisual(IMG_DIR+"button2_2.tga")
		# offersBtn.SetEvent(self.OfferItems)
		# offersBtn.SetPosition(100+((buyCoinBtn.GetWidth()+7)*4),12) #offers
		# offersBtn.Show()
		# self.children["offersBtn"] = offersBtn

		# offersBtnText = MultiTextLine()
		# offersBtnText.SetParent(offersBtn)
		# offersBtnText.SetTextRange(17)
		# offersBtnText.SetPosition(offersBtn.GetWidth()/2,10)
		# offersBtnText.SetTextType("horizontal#center")
		# offersBtnText.SetFontName("Tahoma:16")
		# offersBtnText.SetText(localeInfo.ITEMSHOP_OFFERS)
		# offersBtnText.Show()
		# offersBtn.multiText = offersBtnText

		# --- Special Offers left-side panel ---
		specialOfferBg = ui.ExpandedImageBox()
		specialOfferBg.SetParent(boardImage)
		specialOfferBg.LoadImage(IMG_DIR + "special_offer.png")
		specialOfferBg.SetPosition(16, 266)
		specialOfferBg.Show()
		self.children["specialOfferBg"] = specialOfferBg

		specialOfferTitle = ui.TextLine()
		specialOfferTitle.SetParent(specialOfferBg)
		specialOfferTitle.SetFontName("Verdana:12b")
		specialOfferTitle.SetPackedFontColor(0xFFf7dd83)
		specialOfferTitle.SetText("Special Offers")
		specialOfferTitle.SetHorizontalAlignCenter()
		specialOfferTitle.SetPosition(specialOfferBg.GetWidth() / 2, 8)
		specialOfferTitle.Show()
		self.children["specialOfferTitle"] = specialOfferTitle

		soIcon = ui.ExpandedImageBox()
		soIcon.SetParent(specialOfferBg)
		soIcon.SetPosition((specialOfferBg.GetWidth() - 32) / 2, 38)
		soIcon.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__SpecialOfferOverIn)
		soIcon.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__SpecialOfferOverOut)
		soIcon.Hide()
		self.children["soIcon"] = soIcon

		soName = ui.TextLine()
		soName.SetParent(specialOfferBg)
		soName.SetFontName("Verdana:12")
		soName.SetPackedFontColor(0xFFa993be)
		soName.SetHorizontalAlignCenter()
		soName.SetPosition(specialOfferBg.GetWidth() / 2, 121)
		soName.AddFlag("not_pick")
		soName.Hide()
		self.children["soName"] = soName

		soPrice = ui.TextLine()
		soPrice.SetParent(specialOfferBg)
		soPrice.SetFontName("Verdana:14b")
		soPrice.SetPackedFontColor(0xFFf7dd83)
		soPrice.SetHorizontalAlignCenter()
		soPrice.SetPosition(specialOfferBg.GetWidth() / 2, 184)
		soPrice.AddFlag("not_pick")
		soPrice.Hide()
		self.children["soPrice"] = soPrice

		soBuyBtn = ui.ExpandedImageBox()
		soBuyBtn.SetParent(specialOfferBg)
		soBuyBtn.LoadImage(IMG_DIR + "buy_0.png")
		soBuyBtn.SetPosition((specialOfferBg.GetWidth() - soBuyBtn.GetWidth()) / 2, 210)
		soBuyBtn.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__SpecialOfferBuyIn)
		soBuyBtn.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__SpecialOfferBuyOut)
		soBuyBtn.SetEvent(ui.__mem_func__(self.__SpecialOfferBuy), "mouse_click")
		soBuyBtn.Hide()
		self.children["soBuyBtn"] = soBuyBtn

		soBuyBtnText = ui.TextLine()
		soBuyBtnText.SetParent(soBuyBtn)
		soBuyBtnText.AddFlag("not_pick")
		soBuyBtnText.SetPosition(0, 7)
		soBuyBtnText.SetHorizontalAlignCenter()
		soBuyBtnText.SetWindowHorizontalAlignCenter()
		soBuyBtnText.SetPackedFontColor(0xFFf7bd48)
		soBuyBtnText.SetText(localeInfo.ITEMSHOP_BUY)
		soBuyBtnText.Show()
		self.children["soBuyBtnText"] = soBuyBtnText

		ARROW_Y = 98
		soArrowLeft = ui.ExpandedImageBox()
		soArrowLeft.SetParent(specialOfferBg)
		soArrowLeft.LoadImage(IMG_DIR + "count_left.png")
		soArrowLeft.SetPosition(8 + 9, ARROW_Y)
		soArrowLeft.SetEvent(ui.__mem_func__(self.__SpecialOfferPrev), "mouse_click")
		soArrowLeft.Hide()
		self.children["soArrowLeft"] = soArrowLeft

		soCounter = ui.TextLine()
		soCounter.SetParent(specialOfferBg)
		soCounter.SetFontName("Tahoma:11b")
		soCounter.SetPackedFontColor(0xFF888888)
		soCounter.SetText("1/1")
		soCounter.SetHorizontalAlignCenter()
		soCounter.SetPosition(specialOfferBg.GetWidth() / 2, ARROW_Y + 4)
		soCounter.AddFlag("not_pick")
		soCounter.Hide()
		self.children["soCounter"] = soCounter

		soArrowRight = ui.ExpandedImageBox()
		soArrowRight.SetParent(specialOfferBg)
		soArrowRight.LoadImage(IMG_DIR + "count_right.png")
		soArrowRight.SetPosition(specialOfferBg.GetWidth() - 8 - soArrowRight.GetWidth() - 9, ARROW_Y)
		soArrowRight.SetEvent(ui.__mem_func__(self.__SpecialOfferNext), "mouse_click")
		soArrowRight.Hide()
		self.children["soArrowRight"] = soArrowRight
		# --- End Special Offers panel ---

		itemSearchBg = ui.ExpandedImageBox()
		itemSearchBg.SetParent(boardImage)
		itemSearchBg.LoadImage(IMG_DIR + "search.png")

		if app.USE_ITEMSHOP_RENEWED:
			itemSearchBg.SetScale(0.98, 1.0)

		itemSearchBg.SetPosition(132, 16)

		itemSearchBg.Show()
		self.children["itemSearchBg"] = itemSearchBg

		itemSearch = ui.EditLine()
		itemSearch.SetParent(itemSearchBg)
		itemSearch.SetMax(40)
		itemSearch.SetPosition(23, 9)
		itemSearch.SetSize(itemSearchBg.GetWidth()-16, itemSearchBg.GetHeight()-10)
		itemSearch.OnIMEUpdate = ui.__mem_func__(self.__OnValueUpdate)
		itemSearch.OnPressEscapeKey = ui.__mem_func__(self.Close)
		itemSearch.Show()
		self.children["itemSearch"] = itemSearch

		itemSearchText = MultiTextLine()
		itemSearchText.SetParent(itemSearch)
		itemSearchText.SetTextRange(20)
		itemSearchText.SetPosition(6, 0)
		itemSearchText.SetFontColor(128,128,128)
		itemSearchText.SetTextType("horizontal#left")
		itemSearchText.SetFontName("Tahoma:12")
		itemSearchText.SetText(localeInfo.ITEMSHOP_ITEMSEARCH_DEFAULT_TEXT)
		itemSearchText.Show()
		itemSearch.backText = itemSearchText

		if len(eventCategory) <= 3:
			bannerContainer = ui.Window()
			bannerContainer.SetParent(boardImage)
			bannerContainer.SetPosition(16, 132 - 64 + 49)
			bannerContainer.SetSize(750, 142)
			bannerContainer.Show()
			self.children["bannerContainer"] = bannerContainer
			
			bannerList = []
			for banner in xrange(len(eventCategory)):
				xBanner = ItemShopBanner()
				xBanner.SetParent(bannerContainer)
				xBanner.SetPosition(banner * 250, 0)
				xBanner.LoadData(banner)
				xBanner.SetEvent(self.ClickBanner, "mouse_click", eventCategory[banner][1][0], eventCategory[banner][1][1])
				xBanner.Show()
				bannerList.append(xBanner)
			self.children["bannerList"] = bannerList
				
		categoryScrollBar = ScrollBarNew()
		categoryScrollBar.SetParent(boardImage)
		categoryScrollBar.SetPosition(174,69)
		categoryScrollBar.SetScrollBarSize(444)
		categoryScrollBar.SetScrollStep(0.20)
		categoryScrollBar.Hide()
		self.children["categoryScrollBar"] = categoryScrollBar
		
		categoryListBox = CategoryList()
		categoryListBox.SetParent(boardImage)
		categoryListBox.SetPosition(11,68)
		categoryListBox.SetSize(961,140)
		categoryListBox.SetScrollBar(categoryScrollBar)
		categoryListBox.Show()
		self.children["categoryListBox"] = categoryListBox

		itemListBoxClipWindow = ui.Window()
		itemListBoxClipWindow.SetParent(boardImage)
		itemListBoxClipWindow.SetPosition(193, 266)
		itemListBoxClipWindow.SetSize(756, 247)
		itemListBoxClipWindow.Show()
		self.children["itemListBoxClipWindow"] = itemListBoxClipWindow
		
		itemListBox = ui.Window()
		itemListBox.SetParent(itemListBoxClipWindow)
		itemListBox.SetPosition(0, 0)
		itemListBox.SetSize(750, 260)
		itemListBox.children = {}
		itemListBox.Show()
		self.children["itemListBox"] = itemListBox
		
		itemScrollBar = ui.ModernScrollBar()
		itemScrollBar.SetParent(boardImage)
		itemScrollBar.SetPosition(200+750+11, 111)
		itemScrollBar.SetScrollBarSize(409)
		itemScrollBar.SetScrollStep(0.07)
		itemScrollBar.SetScrollEvent(ui.__mem_func__(self.OnScrollItemList))
		itemScrollBar.Show()
		self.children["itemScrollBar"] = itemScrollBar
		
		if app.__BL_SMOOTH_SCROLL__:
			itemScrollBar.EnableSmoothMode()
		
		itemListBoxClipWindow.OnMouseWheel = itemScrollBar.OnMouseWheel

		purchasesHeader = ui.Window()
		purchasesHeader.SetParent(boardImage)
		purchasesHeader.SetPosition(16, 75)
		purchasesHeader.SetSize(927, 35)
		purchasesHeader.Hide()
		self.children["purchasesHeader"] = purchasesHeader
		
		headerBg = ui.Bar()
		headerBg.SetParent(purchasesHeader)
		headerBg.SetPosition(0, -3)
		headerBg.SetSize(927, 35)
		headerBg.SetColor(0x77000000)
		headerBg.Show()
		
		headerTexts = [
			[75, 8, localeInfo.PURCHASES_DATE],
			[260, 8, localeInfo.PURCHASES_ACCOUNTID],
			[400, 8, localeInfo.PURCHASES_IP],
			[600, 8, localeInfo.PURCHASES_ITEMBOUGHT],
			[800, 8, localeInfo.PURCHASES_PRICE],
		]
		
		purchasesHeader.headerTextsList = []
		for j in xrange(len(headerTexts)):
			text = headerTexts[j]
			headerText = ui.TextLine()
			headerText.SetParent(purchasesHeader)
			headerText.SetFontName("Verdana:14b")
			headerText.SetPackedFontColor(0xFFf7bd48)
			headerText.SetText(text[2])
			headerText.SetOutline()
			headerText.SetHorizontalAlignCenter()
			headerText.SetPosition(text[0], text[1])
			headerText.Show()
			purchasesHeader.headerTextsList.append(headerText)
		
		purchasesClipWindow = ui.Window()
		purchasesClipWindow.SetParent(boardImage)
		purchasesClipWindow.SetPosition(15, 112)
		purchasesClipWindow.SetSize(945, 407)
		purchasesClipWindow.Hide()
		self.children["purchasesClipWindow"] = purchasesClipWindow
		
		purchasesListBox = ui.Window()
		purchasesListBox.SetParent(purchasesClipWindow)
		purchasesListBox.SetPosition(0, 0)
		purchasesListBox.SetSize(945, 407)
		purchasesListBox.children = {}
		purchasesListBox.Hide()
		self.children["PurchasesListBox"] = purchasesListBox
		
		PurchasesScrollBar = ui.ModernScrollBar()
		PurchasesScrollBar.SetParent(boardImage)
		PurchasesScrollBar.SetPosition(200+750+14, 111)
		PurchasesScrollBar.SetScrollBarSize(409)
		PurchasesScrollBar.SetScrollStep(0.07)
		PurchasesScrollBar.SetScrollEvent(ui.__mem_func__(self.OnScrollPurchasesList))
		PurchasesScrollBar.Hide()
		self.children["PurchasesScrollBar"] = PurchasesScrollBar
		
		if app.__BL_SMOOTH_SCROLL__:
			PurchasesScrollBar.EnableSmoothMode()
		
		purchasesClipWindow.OnMouseWheel = PurchasesScrollBar.OnMouseWheel

		LoadingImage = ui.ExpandedImageBox()
		LoadingImage.SetParent(boardImage)
		LoadingImage.SetPosition(boardImage.GetWidth()/2,boardImage.GetHeight()/2)
		LoadingImage.LoadImage(IMG_DIR+"load_.tga")
		self.children["LoadingImage"] = LoadingImage

		self.children["LoadingImageRotation"] = 0

		wheelBg = ui.ImageBox()
		wheelBg.SetParent(self)
		wheelBg.AddFlag("attach")
		wheelBg.LoadImage(WHEEL_DIR+"wallpaper.tga")
		wheelBg.SetPosition(2,61)
		wheelBg.Hide()
		self.children["wheelBg"] = wheelBg

		wheelBgDesign = ui.ImageBox()
		wheelBgDesign.AddFlag("attach")
		wheelBgDesign.SetParent(wheelBg)
		wheelBgDesign.LoadImage(WHEEL_DIR+"wheel_bg.tga")
		wheelBgDesign.SetAlpha(0.5)
		wheelBgDesign.Show()
		self.children["wheelBgDesign"] = wheelBgDesign

		wheelSpinCheck = ui.ToggleButton()
		wheelSpinCheck.SetParent(wheelBg)
		wheelSpinCheck.SetUpVisual(WHEEL_DIR+"no_selected.tga")
		wheelSpinCheck.SetOverVisual(WHEEL_DIR+"no_selected.tga")
		wheelSpinCheck.SetDownVisual(WHEEL_DIR+"selected.tga")
		wheelSpinCheck.SetPosition(27,24)
		wheelSpinCheck.SetToggleUpEvent(self.SetSpintAnimation,False)
		wheelSpinCheck.SetToggleDownEvent(self.SetSpintAnimation,True)
		wheelSpinCheck.Show()
		self.children["wheelSpinCheck"] = wheelSpinCheck

		wheelSpinCheckText = MultiTextLine()
		wheelSpinCheckText.SetParent(wheelBg)
		wheelSpinCheckText.SetPosition(52,25)
		wheelSpinCheckText.SetTextRange(17)
		wheelSpinCheckText.SetTextType("horizontal#left")
		wheelSpinCheckText.SetFontName("Tahoma:12")
		wheelSpinCheckText.SetOutline(True)
		wheelSpinCheckText.Show()
		self.children["wheelSpinCheckText"] = wheelSpinCheckText

		backToItemShop = ui.Button()
		backToItemShop.SetParent(wheelBg)
		backToItemShop.SetUpVisual(WHEEL_DIR+"back_0.tga")
		backToItemShop.SetOverVisual(WHEEL_DIR+"back_1.tga")
		backToItemShop.SetDownVisual(WHEEL_DIR+"back_2.tga")
		backToItemShop.SetPosition(19,425)
		backToItemShop.SetEvent(self.BackToItemShop)
		backToItemShop.Show()
		self.children["backToItemShop"] = backToItemShop
		
		backToItemShopText = MultiTextLine()
		backToItemShopText.SetParent(backToItemShop)
		backToItemShopText.SetPosition(backToItemShop.GetWidth()+10,4)
		backToItemShopText.SetTextRange(17)
		backToItemShopText.SetTextType("horizontal#left")
		backToItemShopText.SetFontName("Verdana:12b")
		backToItemShopText.SetOutline(True)
		backToItemShopText.SetText(localeInfo.ITEMSHOP_WHEEL_BACK_ISHOP)
		backToItemShopText.Show()
		backToItemShop.backToItemShopText = backToItemShopText
		
		wheelPreviewBtn = ui.Button()
		wheelPreviewBtn.SetParent(wheelBg)
		wheelPreviewBtn.SetUpVisual(WHEEL_DIR+"reward_btn_norm.png")
		wheelPreviewBtn.SetOverVisual(WHEEL_DIR+"reward_btn_hover.png")
		wheelPreviewBtn.SetDownVisual(WHEEL_DIR+"reward_btn_down.png")
		wheelPreviewBtn.SetPosition(200, 383)
		wheelPreviewBtn.SetEvent(ui.__mem_func__(self.ShowWheelRewards))
		wheelPreviewBtn.Show()
		self.children["wheelPreviewBtn"] = wheelPreviewBtn

		wheelPreviewBtnText = MultiTextLine()
		wheelPreviewBtnText.SetParent(wheelPreviewBtn)
		wheelPreviewBtnText.SetPosition(wheelPreviewBtn.GetWidth()+10, 46)
		wheelPreviewBtnText.SetTextRange(17)
		wheelPreviewBtnText.SetTextType("horizontal#left")
		wheelPreviewBtnText.SetFontName("Verdana:12b")
		wheelPreviewBtnText.SetOutline(True)
		#wheelPreviewBtnText.SetText("View All Rewards")
		wheelPreviewBtnText.Show()
		wheelPreviewBtn.previewBtnText = wheelPreviewBtnText

		whellTicketBtn = ui.RadioButton()
		whellTicketBtn.SetParent(wheelBg)
		whellTicketBtn.SetUpVisual(WHEEL_DIR+"no_selected.tga")
		whellTicketBtn.SetOverVisual(WHEEL_DIR+"no_selected.tga")
		whellTicketBtn.SetDownVisual(WHEEL_DIR+"selected.tga")
		whellTicketBtn.SetPosition(797,400)
		whellTicketBtn.SetEvent(self.SetTicketMode, 0)
		whellTicketBtn.Show()
		self.children["whellTicketBtn"] = whellTicketBtn

		whellTicketBtnText = MultiTextLine()
		whellTicketBtnText.SetParent(whellTicketBtn)
		whellTicketBtnText.SetPosition(29,-1)
		whellTicketBtnText.SetTextRange(17)
		whellTicketBtnText.SetTextType("horizontal#left")
		whellTicketBtnText.SetFontName("Tahoma:12")
		whellTicketBtnText.SetOutline(True)
		whellTicketBtnText.SetText(localeInfo.ITEMSHOP_WHEEL_TICKET_TEXT)
		whellTicketBtnText.Show()
		whellTicketBtn.whellTicketBtnText = whellTicketBtnText

		whellCoinBtn = ui.RadioButton()
		whellCoinBtn.SetParent(wheelBg)
		whellCoinBtn.SetUpVisual(WHEEL_DIR+"no_selected.tga")
		whellCoinBtn.SetOverVisual(WHEEL_DIR+"no_selected.tga")
		whellCoinBtn.SetDownVisual(WHEEL_DIR+"selected.tga")
		whellCoinBtn.SetPosition(797,430)
		whellCoinBtn.SetEvent(self.SetTicketMode,1)
		whellCoinBtn.Show()
		self.children["whellCoinBtn"] = whellCoinBtn
		
		whellCoinBtnText = MultiTextLine()
		whellCoinBtnText.SetParent(whellCoinBtn)
		whellCoinBtnText.SetPosition(29,-1)
		whellCoinBtnText.SetTextRange(17)
		whellCoinBtnText.SetTextType("horizontal#left")
		whellCoinBtnText.SetFontName("Tahoma:12")
		whellCoinBtnText.SetOutline(True)
		whellCoinBtnText.SetText(localeInfo.ITEMSHOP_WHEEL_COIN_TEXT)
		whellCoinBtnText.Show()
		whellTicketBtn.whellCoinBtnText = whellCoinBtnText

		whellChequeBtn = ui.RadioButton()
		whellChequeBtn.SetParent(wheelBg)
		whellChequeBtn.SetUpVisual(WHEEL_DIR+"no_selected.tga")
		whellChequeBtn.SetOverVisual(WHEEL_DIR+"no_selected.tga")
		whellChequeBtn.SetDownVisual(WHEEL_DIR+"selected.tga")
		whellChequeBtn.SetPosition(797,370)
		whellChequeBtn.SetEvent(self.SetTicketMode,2)
		whellChequeBtn.Show()
		self.children["whellChequeBtn"] = whellChequeBtn

		whellChequeBtnText = MultiTextLine()
		whellChequeBtnText.SetParent(whellChequeBtn)
		whellChequeBtnText.SetPosition(29,-1)
		whellChequeBtnText.SetTextRange(17)
		whellChequeBtnText.SetTextType("horizontal#left")
		whellChequeBtnText.SetFontName("Tahoma:12")
		whellChequeBtnText.SetOutline(True)
		whellChequeBtnText.SetText(localeInfo.ITEMSHOP_WHEEL_CHEQUE_TEXT)
		whellChequeBtnText.Show()
		whellChequeBtn.whellChequeBtnText = whellChequeBtnText

		fortuneImg = ui.ImageBox()
		fortuneImg.SetParent(wheelBg)
		fortuneImg.AddFlag("not_pick")
		fortuneImg.LoadImage(WHEEL_DIR+"fortune.tga")
		fortuneImg.SetPosition(308,43)
		fortuneImg.Show()
		self.children["fortuneImg"] = fortuneImg

		fortuneSingle = ui.ExpandedImageBox()
		fortuneSingle.SetParent(fortuneImg)
		fortuneSingle.AddFlag("not_pick")
		fortuneSingle.LoadImage(WHEEL_DIR+"single.tga")
		fortuneSingle.SetPosition(125,6)#0 index.
		fortuneSingle.Show()
		self.children["fortuneSingle"] = fortuneSingle

		wheelCircle = ui.ExpandedImageBox()
		wheelCircle.SetParent(fortuneImg)
		wheelCircle.LoadImage(WHEEL_DIR+"circle.tga")
		wheelCircle.AddFlag("not_pick")
		wheelCircle.SetPosition(125,111)
		wheelCircle.Show()
		self.children["wheelCircle"] = wheelCircle

		wheelStartBtn = ui.Button()
		wheelStartBtn.SetParent(fortuneImg)
		wheelStartBtn.SetUpVisual(WHEEL_DIR+"wheel_0.tga")
		wheelStartBtn.SetOverVisual(WHEEL_DIR+"wheel_1.tga")
		wheelStartBtn.SetDownVisual(WHEEL_DIR+"wheel_2.tga")
		wheelStartBtn.SetPosition(130,130)
		wheelStartBtn.SetEvent(ui.__mem_func__(self.StartWheel))
		wheelStartBtn.Show()
		self.children["wheelStartBtn"] = wheelStartBtn

		itemPosition = [[175,40], [245, 65], [290, 135], [290, 210], [245, 265], [175, 295], [100, 270], [60, 210], [55, 135], [100, 65]]
		for j in xrange(len(itemPosition)):
			wheelItem = ImageBoxSpecial()
			wheelItem.SetParent(fortuneImg)
			wheelItem.AddFlag("movable")
			wheelItem.SetPosition(itemPosition[j][0],itemPosition[j][1])
			wheelItem.SetSize(32,32)
			wheelItem.Show()
			wheelItem.index = j
			self.children["wheelItem%d"%j] = wheelItem

		self.SetTrailerMode()
		self.SetTicketMode(1)

		self.SetDragonCoin(0)

		if app.USE_ITEMSHOP_RENEWED:
			self.SetDragonJeton(0)

		self.SetCenterPosition()
		self.LoadShopCategoryNew()


	def PromoCode(self):
		if not self.promoCodeWindow:
			self.promoCodeWindow = PromoCodeWindow()
			self.promoCodeWindow.SetAcceptEvent(ui.__mem_func__(self.__OnPromoCodeSubmit))
		
		self.promoCodeWindow.Open()
	
	def __OnPromoCodeSubmit(self, promoCode):
		net.SendChatPacket("/promo_code_system check_code %s" % promoCode)

	def SetTrailerMode(self):
		self.children["wheelStep"] = 0
		self.children["circleRotation"] = 0
		self.children["wheelRotation"] = 0
		
		coinType = self.children.get("wheelCointType", 1)
		if coinType == 0:
			itemSource = wheel_christmas_items
		elif coinType == 2:
			itemSource = wheel_cheque_items
		else:
			itemSource = wheel_random_items
		
		for j in xrange(10):
			for x in xrange(10):
				itemData = itemSource[app.GetRandom(0, len(itemSource)-1)]
				item.SelectItem(itemData[0])
				self.children["wheelItem%d"%j].LoadImage(item.GetIconImageFileName(), itemData[0], itemData[1])

	def UpdateSingleFortune(self):
		single_pos = [[125,6,0], [193,33,35], [230,92,70], [228,161,110], [185,215,142], [117,237,180], [54,216,215], [11,159,250], [11,86,285], [53,26,324]]
		index = self.GetRotationToIndex(self.children["circleRotation"])
		self.children["fortuneSingle"].SetRotation(single_pos[index][2])
		self.children["fortuneSingle"].SetPosition(single_pos[index][0],single_pos[index][1])

	def OnSetWhell(self, giftIndex):
		self.giftIndex = giftIndex
		self.children["wheelRotation"] = self.GetGiftRotation(giftIndex)

		if self.children["wheelSpinCheckFlag"] == False:
			self.children["wheelStep"] = app.GetRandom(10, 15)
		else:
			self.children["wheelStep"] = 0
			self.children["circleRotation"] = self.children["wheelRotation"]
			self.children["wheelCircle"].SetRotation(self.children["circleRotation"])
			self.UpdateSingleFortune()
			self.SendServerDone()

	def SetWheelItemData(self, data):
		for j in xrange(10):
			self.children["wheelItem%d"%j].Clear()
		index = 0
		splitData = data.split("#")
		for splitText in splitData:
			splitItem = splitText.split("|")
			if len(splitItem) == 2:
				item.SelectItem(int(splitItem[0]))
				self.children["wheelItem%d"%index].LoadImage(item.GetIconImageFileName(), int(splitItem[0]), int(splitItem[1]))
				index += 1

	def StartWheel(self):
		net.SendChatPacket("/ishop wheel start %d"% self.children["wheelCointType"])

	def SendServerDone(self):
		net.SendChatPacket("/ishop wheel done")

	def CloseMessageWindow(self):
		if self.children.has_key("message_window"):
			msgWindow = self.children["message_window"]
			if msgWindow != None:
				msgWindow.Hide()
				msgWindow = None

	def GetWheelGiftData(self, itemVnum, itemCount):
		self.CloseMessageWindow()
		self.children["message_window"] = MessageWindow()
		self.children["message_window"].LoadMessage(itemVnum, itemCount)

	def SetTicketMode(self, index):
		if not self.children.has_key("wheelSpinCheckFlag"):
			self.children["wheelSpinCheckFlag"] = False
		self.children["wheelCointType"] = index
		self.__ClickRadioButton([self.children["whellTicketBtn"],self.children["whellCoinBtn"],self.children["whellChequeBtn"]],index)
		self.SetSpintAnimation(self.children["wheelSpinCheckFlag"])

		if self.children.has_key("wheelBgDesign"):
			if index == 0:
				try:
					self.children["wheelBgDesign"].LoadImage(WHEEL_DIR+"wheel_bg_christmas.tga")
				except:
					self.children["wheelBgDesign"].LoadImage(WHEEL_DIR+"wheel_bg.tga")
			elif index == 2:
				try:
					self.children["wheelBgDesign"].LoadImage(WHEEL_DIR+"wheel_bg_cheque.tga")
				except:
					self.children["wheelBgDesign"].LoadImage(WHEEL_DIR+"wheel_bg.tga")
			else:
				self.children["wheelBgDesign"].LoadImage(WHEEL_DIR+"wheel_bg.tga")

		self.SetTrailerMode()

		if self.children.has_key("wheelPreviewBtn"):
			wheelPreviewBtn = self.children["wheelPreviewBtn"]
			if hasattr(wheelPreviewBtn, 'previewBtnText'):
				if index == 0:
					wheelPreviewBtn.previewBtnText.SetText("View Easter Rewards")
				elif index == 2:
					wheelPreviewBtn.previewBtnText.SetText("View Cheque Rewards")
				else:
					wheelPreviewBtn.previewBtnText.SetText("View All Rewards")

		if self.wheelRewardPreview and self.wheelRewardPreview.IsShow():
			if index == 0:
				self.wheelRewardPreview.LoadRewardData(wheel_christmas_items)
				self.wheelRewardPreview.SetTitleName("Easter Wheel Rewards")
			elif index == 2:
				self.wheelRewardPreview.LoadRewardData(wheel_cheque_items)
				self.wheelRewardPreview.SetTitleName("Cheque Wheel Rewards")
			else:
				self.wheelRewardPreview.LoadRewardData(wheel_random_items)
				self.wheelRewardPreview.SetTitleName("Wheel Rewards")

	def WheelBtn(self):
		pass
		#webbrowser.open_new("https://elvion.mentra.gg/")
		# self.children["wheelBg"].Show()

	def BackToItemShop(self):
		self.children["wheelBg"].Hide()
		
	def OnScrollItemList(self):
		itemScrollBar = self.children.get("itemScrollBar")
		itemListBox = self.children.get("itemListBox")
		
		if not itemScrollBar or not itemListBox:
			return
		
		scrollPos = itemScrollBar.GetPos()
		
		totalHeight = itemListBox.GetHeight()
		visibleHeight = 260
		scrollableHeight = max(0, totalHeight - visibleHeight)
		
		offset = int(scrollPos * scrollableHeight)
		itemListBox.SetPosition(0, -offset)
		
	def OnScrollPurchasesList(self):
		PurchasesScrollBar = self.children.get("PurchasesScrollBar")
		purchasesListBox = self.children.get("PurchasesListBox")
		
		if not PurchasesScrollBar or not purchasesListBox:
			return
		
		scrollPos = PurchasesScrollBar.GetPos()
		
		totalHeight = purchasesListBox.GetHeight()
		visibleHeight = 407
		scrollableHeight = max(0, totalHeight - visibleHeight)
		
		offset = int(scrollPos * scrollableHeight)
		purchasesListBox.SetPosition(0, -offset)
		
	def ShowWheelRewards(self):
		if not self.wheelRewardPreview:
			self.wheelRewardPreview = WheelRewardPreviewWindow()

		coinType = self.children.get("wheelCointType", 1)
		if coinType == 0:
			self.wheelRewardPreview.LoadRewardData(wheel_christmas_items)
			self.wheelRewardPreview.SetTitleName("Easter Wheel Rewards")
		elif coinType == 2:
			self.wheelRewardPreview.LoadRewardData(wheel_cheque_items)
			self.wheelRewardPreview.SetTitleName("Cheque Wheel Rewards")
		else:
			self.wheelRewardPreview.LoadRewardData(wheel_random_items)
			self.wheelRewardPreview.SetTitleName("Wheel Rewards")
		
		self.wheelRewardPreview.Open()

	def SetSpintAnimation(self, flag):
		self.children["wheelSpinCheckFlag"] = flag
		text = localeInfo.ITEMSHOP_WHEEL_SKIP_ANIMATION + "\n"
		coinType = self.children["wheelCointType"]
		if coinType == 0:
			text += localeInfo.ITEMSHOP_WHEEL_TICKET_TEXT_INFO
		elif coinType == 2:
			text += localeInfo.ITEMSHOP_WHEEL_CHEQUE_TEXT_INFO
		else:
			text += localeInfo.ITEMSHOP_WHEEL_COIN_TEXT_INFO
		self.children["wheelSpinCheckText"].SetText(text)

	def GetGiftRotation(self, giftIndex):
		if giftIndex == 0:
			if app.GetRandom(0,1) == 0:
				return 341+app.GetRandom(4,18)
			else:
				return app.GetRandom(4,19)
		else:
			listofIndex = [0,20,58,94,126,160,196,232,268,304,341]
			return listofIndex[giftIndex]+app.GetRandom(4,28)

	def GetRotationToIndex(self, rotation):
		if rotation >= 341 and rotation <= 360 or rotation <= 20:
			return 0
		elif rotation >= 305 and rotation <= 305+38:
			return 9
		elif rotation >= 269 and rotation <= 269+38:
			return 8
		elif rotation >= 233 and rotation <= 233+38:
			return 7
		elif rotation >= 197 and rotation <= 197+38:
			return 6
		elif rotation >= 161 and rotation <= 161+38:
			return 5
		elif rotation >= 127 and rotation <= 127+38:
			return 4
		elif rotation >= 95 and rotation <= 95+38:
			return 3
		elif rotation >= 59 and rotation <= 59+38:
			return 2
		elif rotation >= 21 and rotation <= 21+38:
			return 1
		return 0

	def OnUpdate(self):
		now = app.GetTime()
		self.UpdateMoneyAnimation(now)
		self.UpdateSpecialOffer(now)
		if app.USE_ITEMSHOP_RENEWED:
			self.UpdateJetonAnimation(now)

		for xBanner in self.children.get("bannerList", []):
			xBanner.OnUpdate()

		if self.children.has_key("categoryListBox"):
			categoryListBox = self.children["categoryListBox"]
			if hasattr(categoryListBox, 'itemList'):
				for categoryItem in categoryListBox.itemList:
					if hasattr(categoryItem, 'itemList'):
						for subItem in categoryItem.itemList:
							if hasattr(subItem, 'OnUpdate'):
								subItem.OnUpdate()
			
		if self.children["wheelStep"] > 0:
			increaseValue = 0.5 * float((self.children["wheelStep"]*2))
			self.children["circleRotation"] += increaseValue
			self.children["wheelCircle"].SetRotation(self.children["circleRotation"])
			self.UpdateSingleFortune()
			if self.children["circleRotation"] >= 360:
				if self.children["wheelStep"] > 1:
					self.children["wheelStep"]-= 1
				self.children["circleRotation"]= 0
			if self.children["wheelStep"] == 1 and self.children["wheelRotation"] == self.children["circleRotation"]:
				self.children["wheelStep"] -= 1
				self.SendServerDone()
				return

		if self.children.has_key("itemListBox"):
			itemListBox = self.children["itemListBox"]
			if hasattr(itemListBox, 'children'):
				for itemPointer in itemListBox.children.values():
					if hasattr(itemPointer, 'OnUpdate'):
						itemPointer.OnUpdate()
		if self.children.has_key("LoadingImage"):
			LoadingImage = self.children["LoadingImage"]
			if LoadingImage.IsShow():
				self.children["LoadingImageRotation"] += 15
				LoadingImage.SetRotation(self.children["LoadingImageRotation"])

		if self.CoinFlag == True:
			if self.CoinSize >= 0 and self.CoinSize <= 15:
				if self.NewCoin > self.RealCoin:
					self.RealCoin += ((self.CoinRange) / 15)
					self.SetDragonCoinReal(self.RealCoin)
				else:
					self.RealCoin -= ((self.CoinRange) / 15)
					self.SetDragonCoinReal(self.RealCoin)

				self.CoinSize += 1

				if self.CoinSize == 15:
					self.RealCoin = self.NewCoin
					self.SetDragonCoinReal(self.RealCoin)
					self.CoinFlag = False

		if app.USE_ITEMSHOP_RENEWED:
			if self.JetonFlag == True:
				if self.JetonSize >= 0 and self.JetonSize <= 15:
					if self.NewJeton > self.RealJeton:
						self.RealJeton += ((self.JetonRange) / 15)
						self.SetDragonJetonReal(self.RealJeton)
					else:
						self.RealJeton -= ((self.JetonRange) / 15)
						self.SetDragonJetonReal(self.RealJeton)

					self.JetonSize += 1

					if self.JetonSize == 15:
						self.RealJeton = self.NewJeton
						self.SetDragonJetonReal(self.RealJeton)
						self.JetonFlag = False

	def __OnValueUpdate(self):
		itemSearch = self.children["itemSearch"]
		ui.EditLine.OnIMEUpdate(itemSearch)

		self.ClearItemList()
		searchItemList = []
		searchText = itemSearch.GetText().lower()
		if len(searchText) > 0:
			for key, data in itemshopData.items():
				for itemList in data:
					for itemData in itemList:
						item.SelectItem(itemData[LIST_ITEM_VNUM])
						if item.GetItemName().lower().find(searchText) != -1:
							searchItemList.append(itemData)
			self.RefreshItemList(searchItemList)
		else:
			self.LoadFirstOpening()

	def LoadShopCategoryNew(self):
		self.categoryIndex = -1
		self.categorySubIndex = -1
		categoryListBox = self.children["categoryListBox"]
		
		categoryData = []
		for key, data in itemShopCategoryData.items():
			categoryItem = CategoryItemNew(data["name"], key)
			categoryItem.SetEvent(lambda idx=key: self.ClickCategoryNew("", idx, -1))
			
			categoryInfo = {"item": categoryItem}
			
			if len(data["subCategory"]) > 0:
				categoryInfo["children"] = []
				for j, subCatName in enumerate(data["subCategory"]):
					subCategoryItem = CategorySubItemNew(subCatName, key, j)
					subCategoryItem.SetEvent(lambda idx=key, subIdx=j: self.ClickCategoryNew("", idx, subIdx))
					categoryInfo["children"].append({"item": subCategoryItem})
			
			categoryData.append(categoryInfo)
		
		categoryListBox.AppendItemList(categoryData)
	
	def ClickCategoryNew(self, emptyArg, categoryIndex, categorySubIndex=-1):
		self.categoryIndex = categoryIndex
		self.categorySubIndex = categorySubIndex
		self.RefreshItem()
		
	def AdjustItemListBoxLayout(self, showBanner):
		itemListBox = self.children["itemListBox"]
		itemScrollBar = self.children["itemScrollBar"]
		
		if showBanner:
			itemListBox.SetPosition(195, 264)
			itemListBox.SetSize(750, 260)
			itemScrollBar.SetPosition(200+750+11, 111)
			itemScrollBar.SetScrollBarSize(409)
		else:
			itemListBox.SetPosition(195, 65)
			itemListBox.SetSize(750, 464)
			itemScrollBar.SetPosition(200+750+11, 111)
			itemScrollBar.SetScrollBarSize(409)

	def RefreshItem(self):
		if self.children.has_key("bannerContainer"):
			self.children["bannerContainer"].Show()
		
		self.AdjustItemListBoxLayout(True)
		
		self.ClearItemList()
		itemListBox = self.children["itemListBox"]
		if itemshopData.has_key(self.categoryIndex):
			itemData = itemshopData[self.categoryIndex]
			if self.categorySubIndex == -1:
				allList = []
				for itemList in itemData:
					for shopItem in itemList:
						allList.append(shopItem)
				self.RefreshItemList(allList)
			else:
				if self.categorySubIndex >= len(itemData):
					return
				itemList = itemData[self.categorySubIndex]
				self.RefreshItemList(itemList)

	def SortList(self, itemDataList):
		sortData = []
		for itemData in itemDataList:
			sortItem = SortClass(itemData)
			sortData.append(sortItem)
		sortData.sort()
		itemDataListEx = []
		for shortItem in sortData:
			itemDataListEx.append(shortItem.arg)
		return itemDataListEx

	def ClearItemList(self):
		if not self.children.has_key("itemListBox"):
			return
		
		itemListBox = self.children["itemListBox"]
		
		if hasattr(itemListBox, 'children'):
			for child in itemListBox.children.values():
				if hasattr(child, 'Destroy'):
					child.Destroy()
				child.Hide()
			itemListBox.children.clear()
		
		if self.children.has_key("itemScrollBar"):
			itemScrollBar = self.children["itemScrollBar"]
			itemScrollBar.SetPos(0)
		
		itemListBox.SetPosition(0, 0)

	def RefreshItemList(self, itemDataList):
		self.ClosePurchasesWindow()
		if len(itemDataList) == 0:
			return
		
		itemListBox = self.children["itemListBox"]
		itemScrollBar = self.children["itemScrollBar"]
		itemListBoxClipWindow = self.children["itemListBoxClipWindow"]
		
		self.ClearItemList()
		
		if not hasattr(itemListBox, 'children'):
			itemListBox.children = {}
		
		itemDataList = self.SortList(itemDataList)
		
		itemsPerRow = 3
		itemWidth = 240
		itemHeight = 122
		xSpacing = 14
		ySpacing = 8
		
		xPos = 0
		yPos = 0
		itemCount = 0
		
		for shopItem in itemDataList:
			xItem = ItemShopItem()
			xItem.SetParent(itemListBox)
			xItem.SetPosition(xPos, yPos)
			xItem.LoadData(shopItem)
			xItem.SetClippingMaskWindow(itemListBoxClipWindow)
			xItem.Show()
			
			itemListBox.children[len(itemListBox.children)] = xItem
			
			itemCount += 1
			xPos += itemWidth + xSpacing
			
			if itemCount % itemsPerRow == 0:
				xPos = 0
				yPos += itemHeight + ySpacing
		
		totalRows = (len(itemDataList) + itemsPerRow - 1) / itemsPerRow
		totalContentHeight = totalRows * (itemHeight + ySpacing)
		
		itemListBox.SetSize(750, max(260, totalContentHeight))
		
		itemScrollBar.SetContentHeight(totalContentHeight)
		
		if totalContentHeight > 260:
			itemScrollBar.Show()
		else:
			itemScrollBar.Hide()
			itemListBox.SetPosition(0, 0)

	def InsertItemWithType(self, itemDataList, insertType):
		currentTime = app.GetGlobalTimeStamp()
		for key, data in itemshopData.items():
			if key == SPECIAL_OFFER_CATEGORY:
				continue
			for categoryItems in data:
				for itemData in categoryItems:
					if insertType == LIST_ITEM_DISCOUNT:
						if itemData[LIST_ITEM_DISCOUNT] > 0 or itemData[LIST_ITEM_TIME] > 0:
							if not itemData in itemDataList:
								itemDataList.append(itemData)
					elif insertType == LIST_ITEM_TOP_SELLING_INDEX:
						if itemData[LIST_ITEM_TOP_SELLING_INDEX] >= 0 and itemData[LIST_ITEM_TOP_SELLING_INDEX] <= 10:
							if not itemData in itemDataList:
								itemDataList.append(itemData)
					elif insertType == LIST_ITEM_ADDED_TIME:
						if itemData[LIST_ITEM_ADDED_TIME] > currentTime-(60*60*24*3):
							if not itemData in itemDataList:
								itemDataList.append(itemData)
					else:
						itemDataList.append(itemData)

		return itemDataList

	def OfferItems(self):
		self.ClearItemList()
		itemDataList = []
		itemDataList = self.InsertItemWithType(itemDataList, LIST_ITEM_DISCOUNT)
		self.RefreshItemList(itemDataList)

	def HotItems(self):
		self.children["wheelBg"].Show()
		# Initialize to coin mode if not set, otherwise keep current mode
		if not self.children.has_key("wheelCointType"):
			self.SetTicketMode(1)
		else:
			# Refresh the current mode (updates background, items, etc.)
			self.SetTicketMode(self.children["wheelCointType"])

	def LoadFirstOpening(self):
		self.categoryIndex = -1
		self.categorySubIndex = -1
		self.ClearItemList()
		
		if self.children.has_key("bannerContainer"):
			self.children["bannerContainer"].Show()
		
		self.AdjustItemListBoxLayout(True)
		
		itemDataList = []
		itemDataList = self.InsertItemWithType(itemDataList, LIST_ITEM_TOP_SELLING_INDEX)
		itemDataList = self.InsertItemWithType(itemDataList, LIST_ITEM_DISCOUNT)
		itemDataList = self.InsertItemWithType(itemDataList, LIST_ITEM_ADDED_TIME)
		if len(itemDataList) == 0:
			itemDataList = self.InsertItemWithType(itemDataList, 99)
		itemDataList = self.SortList(itemDataList)
		self.RefreshItemList(itemDataList)
		self.LoadSpecialOffers()

	def ClickBanner(self, emptyArg, categoryIndex, categorySubIndex):
		self.categoryIndex = categoryIndex
		self.categorySubIndex = categorySubIndex
		self.RefreshItem()

	def LandingPage(self):
		self.LoadFirstOpening()

	def BuyDragonCoin2(self):
		if not self.children.get("paymentWindow"):
			self.children["paymentWindow"] = PaymentWindow()
		pw = self.children["paymentWindow"]
		pw.SetCenterPosition()
		pw.SetTop()
		pw.Show()

	def OpenWeb(self, url):
		pass

	def CloseWeb(self):
		pass

	def __SpecialOfferOverIn(self):
		if not self.specialOfferCurrentData:
			return
		interface = constInfo.GetInterfaceInstance()
		if not interface:
			return
		if interface.tooltipItem:
			interface.tooltipItem.SetItemToolTip(self.specialOfferCurrentData[LIST_ITEM_VNUM])

	def __SpecialOfferOverOut(self):
		interface = constInfo.GetInterfaceInstance()
		if not interface:
			return
		if interface.tooltipItem:
			interface.tooltipItem.HideToolTip()

	def __SpecialOfferBuyIn(self):
		soBuyBtn = self.children.get("soBuyBtn")
		if soBuyBtn:
			soBuyBtn.LoadImage(IMG_DIR + "buy_1.png")

	def __SpecialOfferBuyOut(self):
		soBuyBtn = self.children.get("soBuyBtn")
		if soBuyBtn:
			soBuyBtn.LoadImage(IMG_DIR + "buy_0.png")

	def __SpecialOfferBuy(self, _emptyArg=None):
		if not self.specialOfferCurrentData or self.specialOfferState not in ("show", "fading_in"):
			return
		data = self.specialOfferCurrentData

		global buyQuestionDialog
		if not buyQuestionDialog:
			buyQuestionDialog = BuyWindow()

		priceIsJetons = False
		if app.USE_ITEMSHOP_RENEWED and data[LIST_ITEM_JPRICE] > 0:
			priceIsJetons = True
			price = data[LIST_ITEM_JPRICE]
		else:
			price = data[LIST_ITEM_PRICE]

		discount = data[LIST_ITEM_DISCOUNT]
		if discount > 0:
			price = int(float(price) * (100 - discount) / 100.0)

		item.SelectItem(data[LIST_ITEM_VNUM])
		text = localeInfo.ITEMSHOP_BUY_TEXT_PRICE % (1, item.GetItemName())
		text += "\n"
		if discount > 0:
			text += (localeInfo.ITEMSHOP_BUY_PRICE_JETONS_WITH_DISCOUNT if priceIsJetons else localeInfo.ITEMSHOP_BUY_PRICE_COINS_WITH_DISCOUNT) % (price, discount)
		else:
			text += (localeInfo.ITEMSHOP_BUY_PRICE_JETONS if priceIsJetons else localeInfo.ITEMSHOP_BUY_PRICE_COINS) % (price)

		buyQuestionDialog.SetText(text)
		buyQuestionDialog.SetAcceptEvent(self.__SpecialOfferAcceptBuy)
		buyQuestionDialog.SetCancelEvent(self.__SpecialOfferCancelBuy)
		buyQuestionDialog.SetCenterPosition()
		buyQuestionDialog.SetTop()
		buyQuestionDialog.Show()

	def __SpecialOfferAcceptBuy(self):
		if self.specialOfferCurrentData:
			net.SendChatPacket("/ishop buy %d 1" % self.specialOfferCurrentData[LIST_ITEM_ID])
		self.__SpecialOfferCancelBuy()

	def __SpecialOfferCancelBuy(self):
		global buyQuestionDialog
		if buyQuestionDialog:
			buyQuestionDialog.Hide()
			buyQuestionDialog.Destroy()
			buyQuestionDialog = None

	def __UpdateSpecialOfferCounter(self):
		total = len(self.specialOfferItems)
		soCounter = self.children.get("soCounter")
		hasMultiple = total > 1
		if soCounter:
			soCounter.SetText("%d/%d" % (self.specialOfferIndex + 1, total))
			if hasMultiple:
				soCounter.Show()
			else:
				soCounter.Hide()
		for key in ("soArrowLeft", "soArrowRight"):
			wnd = self.children.get(key)
			if wnd:
				if hasMultiple:
					wnd.Show()
				else:
					wnd.Hide()

	def __SpecialOfferPrev(self, _emptyArg=None):
		if len(self.specialOfferItems) <= 1:
			return
		self.specialOfferNavDir = -1
		if self.specialOfferState in ("show", "fading_in"):
			self.specialOfferState = "fading_out"
			self.specialOfferTimer = app.GetTime()

	def __SpecialOfferNext(self, _emptyArg=None):
		if len(self.specialOfferItems) <= 1:
			return
		self.specialOfferNavDir = 1
		if self.specialOfferState in ("show", "fading_in"):
			self.specialOfferState = "fading_out"
			self.specialOfferTimer = app.GetTime()

	def LoadSpecialOffers(self):
		newItems = []
		if itemshopData.has_key(SPECIAL_OFFER_CATEGORY):
			for subList in itemshopData[SPECIAL_OFFER_CATEGORY]:
				for itemData in subList:
					newItems.append(itemData)  # full item data, not just vnum

		self.specialOfferItems = newItems

		if len(self.specialOfferItems) == 0:
			for key in ("soIcon", "soName", "soPrice", "soBuyBtn", "soArrowLeft", "soCounter", "soArrowRight"):
				wnd = self.children.get(key)
				if wnd:
					wnd.Hide()
			self.specialOfferState = "idle"
			return

		# Only start cycling if not already running (called once per category during data load)
		if self.specialOfferState == "idle":
			self.specialOfferIndex = 0
			self.specialOfferNavDir = 1
			self.specialOfferCurrentData = self.specialOfferItems[0]
			self.__RefreshSpecialOfferDisplay(self.specialOfferCurrentData)
			self.__ApplySpecialOfferAlpha(0.0)
			self.specialOfferState = "fading_in"
			self.specialOfferTimer = app.GetTime()

	def __RefreshSpecialOfferDisplay(self, itemData):
		vnum = itemData[LIST_ITEM_VNUM]
		item.SelectItem(vnum)
		# Capture all item data before any LoadImage call — LoadImage may
		# trigger C++ code that resets the engine's selected-item state.
		iconPath = item.GetIconImageFileName()
		name = item.GetItemName()
		if len(name) > 18:
			name = name[:17] + "..."

		soIcon = self.children.get("soIcon")
		if soIcon:
			soIcon.LoadImage(iconPath)
			soIcon.SetAlpha(0.0)  # LoadImage resets diffuse alpha — caller sets final alpha
			soIcon.Show()

		soName = self.children.get("soName")
		if soName:
			soName.SetText(name)
			soName.Show()

		soPrice = self.children.get("soPrice")
		if soPrice:
			price = itemData[LIST_ITEM_PRICE]
			discount = itemData[LIST_ITEM_DISCOUNT]
			if app.USE_ITEMSHOP_RENEWED and itemData[LIST_ITEM_JPRICE] > 0:
				price = itemData[LIST_ITEM_JPRICE]
			if discount > 0:
				price = int(float(price) * (100 - discount) / 100.0)
				soPrice.SetText("%d DC  (-%d%%)" % (price, discount))
			else:
				soPrice.SetText("%d DC" % price)
			soPrice.Show()

		soBuyBtn = self.children.get("soBuyBtn")
		if soBuyBtn:
			soBuyBtn.LoadImage(IMG_DIR + "buy_0.png")
			soBuyBtn.Show()

		self.specialOfferCurrentData = itemData
		self.__UpdateSpecialOfferCounter()

	def __ApplySpecialOfferAlpha(self, alpha):
		alpha_byte = max(0, min(255, int(alpha * 255)))
		soIcon = self.children.get("soIcon")
		if soIcon:
			soIcon.SetAlpha(alpha)
		soName = self.children.get("soName")
		if soName:
			soName.SetPackedFontColor((alpha_byte << 24) | 0xFFFFFF)
		soPrice = self.children.get("soPrice")
		if soPrice:
			soPrice.SetPackedFontColor((alpha_byte << 24) | 0xf7dd83)
		soBuyBtn = self.children.get("soBuyBtn")
		if soBuyBtn:
			soBuyBtn.SetAlpha(alpha)
		soBuyBtnText = self.children.get("soBuyBtnText")
		if soBuyBtnText:
			soBuyBtnText.SetPackedFontColor((alpha_byte << 24) | 0xf7bd48)
		soCounter = self.children.get("soCounter")
		if soCounter:
			soCounter.SetPackedFontColor((alpha_byte << 24) | 0x888888)
		soArrowLeft = self.children.get("soArrowLeft")
		if soArrowLeft:
			soArrowLeft.SetAlpha(alpha)
		soArrowRight = self.children.get("soArrowRight")
		if soArrowRight:
			soArrowRight.SetAlpha(alpha)
		self.specialOfferAlpha = alpha

	def UpdateSpecialOffer(self, now):
		if len(self.specialOfferItems) == 0:
			return

		if self.specialOfferState == "show":
			if len(self.specialOfferItems) > 1 and now - self.specialOfferTimer >= SPECIAL_OFFER_SHOW_TIME:
				self.specialOfferNavDir = 1
				self.specialOfferState = "fading_out"
				self.specialOfferTimer = now

		elif self.specialOfferState == "fading_out":
			t = (now - self.specialOfferTimer) / SPECIAL_OFFER_FADE_TIME
			if t >= 1.0:
				self.__ApplySpecialOfferAlpha(0.0)
				self.specialOfferIndex = (self.specialOfferIndex + self.specialOfferNavDir) % len(self.specialOfferItems)
				self.specialOfferNavDir = 1
				self.specialOfferCurrentData = self.specialOfferItems[self.specialOfferIndex]
				self.__RefreshSpecialOfferDisplay(self.specialOfferCurrentData)
				self.__ApplySpecialOfferAlpha(0.0)
				self.specialOfferState = "fading_in"
				self.specialOfferTimer = now
			else:
				self.__ApplySpecialOfferAlpha(1.0 - t)

		elif self.specialOfferState == "fading_in":
			t = (now - self.specialOfferTimer) / SPECIAL_OFFER_FADE_TIME
			if t >= 1.0:
				self.__ApplySpecialOfferAlpha(1.0)
				self.specialOfferState = "show"
				self.specialOfferTimer = now
			else:
				self.__ApplySpecialOfferAlpha(t)

	def PurchaseBtn(self):
		self.OpenPurchasesWindow()

	def SetDragonCoinReal(self, lldCoins):
		extra = ""

		if app.USE_ITEMSHOP_RENEWED:
			#extra = " " + (localeInfo.ITEMSHOP_CURRENCY_COIN if lldCoins == 1 else localeInfo.ITEMSHOP_CURRENCY_COINS)
			extra = "\n|cffF8BE47"
		else:
			#extra = "\n|cffF8BE47" + localeInfo.ITEMSHOP_DRAGONCOINT
			extra = "\n|cffF8BE47"

		self.children["dragonCoinText"].SetText(NumberToMoneyFormat(lldCoins) + extra)

	if app.USE_ITEMSHOP_RENEWED:
		def SetDragonJetonReal(self, lldJCoins):
			extra = " " + (localeInfo.ITEMSHOP_CURRENCY_JETON if lldJCoins == 1 else localeInfo.ITEMSHOP_CURRENCY_JETONS)

			self.children["dragonJCoinText"].SetText(NumberToMoneyFormat(lldJCoins) + extra)

	def UpdateItem(self, itemID, itemMaxSellingCount):
		if not self.children.has_key("itemListBox"):
			return

		itemListBox = self.children["itemListBox"]
		if hasattr(itemListBox, "children"):
			childrenValues = itemListBox.children.values() if isinstance(itemListBox.children, dict) else itemListBox.children
			for shopItem in childrenValues:
				if isinstance(shopItem, ItemShopItem) and hasattr(shopItem, "data") and shopItem.data:
					if shopItem.data[LIST_ITEM_ID] == itemID:
						shopItem.data[LIST_ITEM_MAX_SELLING_COUNT] = itemMaxSellingCount
						shopItem.LoadData(shopItem.data)

	def SetDragonCoin(self, lldCoins):
		if not self.moneyInitialized:
			self.RealCoin = lldCoins
			self.lastMoney = lldCoins
			self.animFrom = lldCoins
			self.animTo = lldCoins
			self.SetDragonCoinReal(lldCoins)
			self.moneyInitialized = True
		else:
			if lldCoins != self.lastMoney:
				self.AnimateMoneyChange(lldCoins)
				self.lastMoney = lldCoins

	if app.USE_ITEMSHOP_RENEWED:
		def SetDragonJeton(self, lldJCoins):
			if not self.jetonInitialized:
				self.RealJeton = lldJCoins
				self.lastJeton = lldJCoins
				self.animFromJeton = lldJCoins
				self.animToJeton = lldJCoins
				self.SetDragonJetonReal(lldJCoins)
				self.jetonInitialized = True
			else:
				if lldJCoins != self.lastJeton:
					self.AnimateJetonChange(lldJCoins)
					self.lastJeton = lldJCoins
		
		def AnimateJetonChange(self, newValue):
			now = app.GetTime()
			
			if self.isAnimatingJeton:
				elapsed = now - self.animStartJeton
				t = min(1.0, elapsed / self.animDurationJeton)
				baseFrom = int(round(self.animFromJeton + (self.animToJeton - self.animFromJeton) * t))
			else:
				baseFrom = self.lastJeton
			
			self.animFromJeton = baseFrom
			self.animToJeton = newValue
			self.animStartJeton = now
			self.animDurationJeton = 0.4
			self.isAnimatingJeton = True
			
			if newValue < baseFrom:
				snd.PlaySound("sound/ui/coin_gain.wav")
		
		def UpdateJetonAnimation(self, now):
			if self.isAnimatingJeton:
				t = (now - self.animStartJeton) / self.animDurationJeton
				if t >= 1.0:
					self.isAnimatingJeton = False
					self.SetDragonJetonReal(self.animToJeton)
					self.RealJeton = self.animToJeton
					return
				
				t = max(0.0, min(1.0, t))
				delta = self.animToJeton - self.animFromJeton
				cur = self.animFromJeton + int(delta * t)
				
				self.SetDragonJetonReal(cur)
				self.RealJeton = cur
					
	def AnimateMoneyChange(self, newValue):
		now = app.GetTime()
		
		if self.isAnimatingMoney:
			elapsed = now - self.animStart
			t = min(1.0, elapsed / self.animDuration)
			baseFrom = int(round(self.animFrom + (self.animTo - self.animFrom) * t))
		else:
			baseFrom = self.lastMoney
		
		self.animFrom = baseFrom
		self.animTo = newValue
		self.animStart = now
		self.animDuration = 0.4
		self.isAnimatingMoney = True
		
		self.coinSlotPulse = True
		self.coinPulseStart = now
	
		if newValue < baseFrom:
			snd.PlaySound("sound/ui/coin_gain.wav")

	def UpdateMoneyAnimation(self, now):
		if self.isAnimatingMoney:
			t = (now - self.animStart) / self.animDuration
			if t >= 1.0:
				self.isAnimatingMoney = False
				self.SetDragonCoinReal(self.animTo)
				self.RealCoin = self.animTo
				return
			
			t = max(0.0, min(1.0, t))
			delta = self.animTo - self.animFrom
			cur = self.animFrom + int(delta * t)
			
			self.SetDragonCoinReal(cur)
			self.RealCoin = cur
		
		if self.coinSlotPulse:
			elapsed = now - self.coinPulseStart
			if elapsed < 0.2:
				offset = int(2 * math.sin(elapsed * 20))
			
	def Open(self):
		if self.children.has_key("sendPacketData"):
			if not self.IsShow():
				net.SendChatPacket("/ishop currency")
		else:
			self.children["sendPacketData"] = 1
			self.children["LoadingImage"].Show()
			self.LoadFirstOpening()

			global UPDATE_TIME
			net.SendChatPacket("/ishop data %d" % UPDATE_TIME)

		self.Show()
		self.SetTop()

	def CloseLoading(self):
		self.children["LoadingImage"].Hide()

	def OpenPurchasesWindow(self):
		self.BackToItemShop()

		if self.children.has_key("bannerContainer"):
			self.children["bannerContainer"].Hide()

		self.children["itemListBox"].Hide()
		self.children["itemScrollBar"].Hide()
		self.children["itemListBoxClipWindow"].Hide()
		self.children["categoryListBox"].Hide()
		self.children["categoryScrollBar"].Hide()

		if self.children.has_key("specialOfferBg"):
			self.children["specialOfferBg"].Hide()
	
		if self.children.has_key("purchasesHeader"):
			self.children["purchasesHeader"].Show()
		
		if self.children.has_key("purchasesClipWindow"):
			self.children["purchasesClipWindow"].Show()
		
		if self.children.has_key("PurchasesScrollBar"):
			self.children["PurchasesScrollBar"].Show()
		
		if self.children.has_key("PurchasesListBox"):
			self.children["PurchasesListBox"].Show()
	
		if not self.children.has_key("sendPacketDataLog"):
			self.children["sendPacketDataLog"] = 1
			self.children["LoadingImage"].Show()
			net.SendChatPacket("/ishop log")
		else:
			self.RefreshPurchasesWindow()
	
	def ClosePurchasesWindow(self):
		self.BackToItemShop()
		
		if self.children.has_key("purchasesHeader"):
			self.children["purchasesHeader"].Hide()
		
		if self.children.has_key("purchasesClipWindow"):
			self.children["purchasesClipWindow"].Hide()
		
		if self.children.has_key("PurchasesScrollBar"):
			self.children["PurchasesScrollBar"].Hide()
		
		if self.children.has_key("PurchasesListBox"):
			self.children["PurchasesListBox"].Hide()
	
		self.children["itemListBox"].Show()
		self.children["itemScrollBar"].Show()
		self.children["itemListBoxClipWindow"].Show()
		self.children["categoryListBox"].Show()

		if self.children.has_key("specialOfferBg"):
			self.children["specialOfferBg"].Show()

		if self.children.has_key("bannerContainer"):
			self.children["bannerContainer"].Show()

	def SortPurchasesWindow(self):
		global playerLog
		sortData = []
		for logData in playerLog:
			sortLog = SortClass(logData,True)
			sortData.append(sortLog)
		sortData.sort()
		logDataList = []
		for sortLog in sortData:
			logDataList.append(sortLog.arg)
		return logDataList

	def RefreshPurchasesWindow(self):
		purchasesListBox = self.children["PurchasesListBox"]
		PurchasesScrollBar = self.children["PurchasesScrollBar"]
		purchasesClipWindow = self.children["purchasesClipWindow"]
		
		if hasattr(purchasesListBox, 'children'):
			for child in purchasesListBox.children.values():
				if hasattr(child, 'Destroy'):
					child.Destroy()
				child.Hide()
			purchasesListBox.children.clear()
		
		if not hasattr(purchasesListBox, 'children'):
			purchasesListBox.children = {}
		
		global playerLog
		playerLog = self.SortPurchasesWindow()
		
		logHeight = 35
		yPos = 0
		
		for j in xrange(len(playerLog)):
			logItem = PurchasesLog(purchasesListBox)
			logItem.SetParent(purchasesListBox)
			logItem.LoadData(j)
			logItem.SetPosition(0, yPos)
			logItem.SetClippingMaskWindow(purchasesClipWindow)
			logItem.Show()
			
			purchasesListBox.children[len(purchasesListBox.children)] = logItem
			
			yPos += logHeight
		
		totalContentHeight = len(playerLog) * logHeight
		
		purchasesListBox.SetSize(927, max(407, totalContentHeight))
		
		PurchasesScrollBar.SetContentHeight(totalContentHeight)
		
		if totalContentHeight > 407:
			PurchasesScrollBar.Show()
		else:
			PurchasesScrollBar.Hide()
			purchasesListBox.SetPosition(0, 0)

	def Close(self):
		self.CloseMessageWindow()
		if self.children["wheelStep"] > 0:
			self.children["wheelStep"] = 0
			self.children["circleRotation"] = self.children["wheelRotation"]
			self.children["wheelCircle"].SetRotation(self.children["circleRotation"])
			self.UpdateSingleFortune()
			self.SendServerDone()

		self.CloseWeb()

		global buyQuestionDialog
		if buyQuestionDialog != None:
			buyQuestionDialog.Hide()
			buyQuestionDialog.Destroy()
			buyQuestionDialog=None

		self.Hide()
	def OnPressEscapeKey(self):
		self.Close()
		return True
	def __ClickRadioButton(self, buttonList, buttonIndex):
		try:
			btn=buttonList[buttonIndex]
		except IndexError:
			return
		for eachButton in buttonList:
			eachButton.SetUp()
		btn.Down()

class PurchasesLog(ui.ExpandedImageBox):
	def __del__(self):
		ui.ExpandedImageBox.__del__(self)
	
	def Destroy(self):
		self.childrenImages = {}
		self.childrenTextLine = {}
		self.parent = None
	
	def __init__(self, parent):
		ui.ExpandedImageBox.__init__(self)
		self.Destroy()
		self.parent = proxy(parent)
	
	def LoadData(self, index):
		self.childrenImages["self"] = self
		self.LoadImage(IMG_DIR+"purch_img.png")
		
		try:
			logData = playerLog[index]
		except:
			return
		
		dateText = logData[LOG_DATE_TEXT]
		accountText = logData[LOG_ACCOUNT]
		#ipText = logData[LOG_IP]
		ipText = "-"
		
		if logData[LOG_ITEM] == 1:
			itemText = localeInfo.ITEMSHOP_WHEEL_PURCHASE
		else:
			item.SelectItem(logData[LOG_ITEM])
			if logData[LOG_COUNT] > 1:
				itemText = "|cff9e9d9d%sx%d|r" % (item.GetItemName(), logData[LOG_COUNT])
			else:
				itemText = "|cff9e9d9d%s|r" % item.GetItemName()
		
		priceText = localeInfo.PURCHASES_PRICE_TEXT % NumberToMoneyFormat(logData[LOG_PRICE])
		
		textList = [
			[75, 8, dateText],
			[260, 8, accountText],
			[400, 8, ipText],
			[600, 8, itemText],
			[800, 8, priceText],
		]
		
		for j in xrange(len(textList)):
			text = textList[j]
			logText = ui.TextLine()
			logText.SetParent(self)
			logText.SetFontName("Verdana:12b")
			logText.SetText(text[2])
			logText.SetHorizontalAlignCenter()
			logText.SetPosition(text[0], text[1])
			logText.Show()
			self.childrenTextLine["text%d" % j] = logText

class ItemShopBoard(ui.Window):
	CORNER_WIDTH = 7
	CORNER_HEIGHT = 7
	LINE_WIDTH = 7
	LINE_HEIGHT = 7
	LT = 0
	LB = 1
	RT = 2
	RB = 3
	L = 0
	R = 1
	T = 2
	B = 3
	def __init__(self):
		ui.Window.__init__(self)
		self.MakeBoard()
		self.MakeBase()
	def MakeBoard(self):
		cornerPath = IMG_DIR+"board/corner_"
		linePath = IMG_DIR+"board/"
		CornerFileNames = [ cornerPath+dir+".tga" for dir in ("left_top", "left_bottom", "right_top", "right_bottom") ]
		LineFileNames = [ linePath+dir+".tga" for dir in ("left", "right", "top", "bottom") ]
		self.Corners = []
		for fileName in CornerFileNames:
			Corner = ui.ExpandedImageBox()
			Corner.AddFlag("not_pick")
			Corner.LoadImage(fileName)
			Corner.SetParent(self)
			Corner.SetPosition(0, 0)
			Corner.Show()
			self.Corners.append(Corner)
		self.Lines = []
		for fileName in LineFileNames:
			Line = ui.ExpandedImageBox()
			Line.AddFlag("not_pick")
			Line.LoadImage(fileName)
			Line.SetParent(self)
			Line.SetPosition(0, 0)
			Line.Show()
			self.Lines.append(Line)
		self.Lines[self.L].SetPosition(0, self.CORNER_HEIGHT)
		self.Lines[self.T].SetPosition(self.CORNER_WIDTH, 0)
	def MakeBase(self):
		self.Base = ui.ExpandedImageBox()
		self.Base.AddFlag("not_pick")
		self.Base.LoadImage(IMG_DIR+"board/base.tga")
		self.Base.SetParent(self)
		self.Base.SetPosition(self.CORNER_WIDTH, self.CORNER_HEIGHT)
		self.Base.Show()
	def __del__(self):
		ui.Window.__del__(self)
	def Destroy(self):
		self.Base=0
		self.Corners=0
		self.Lines=0
		self.CORNER_WIDTH = 0
		self.CORNER_HEIGHT = 0
		self.LINE_WIDTH = 0
		self.LINE_HEIGHT = 0
		self.LT = 0
		self.LB = 0
		self.RT = 0
		self.RB = 0
		self.L = 0
		self.R = 0
		self.T = 0
		self.B = 0
	def SetSize(self, width, height):
		width = max(self.CORNER_WIDTH*2, width)
		height = max(self.CORNER_HEIGHT*2, height)
		ui.Window.SetSize(self, width, height)
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

#class WebWindow(ItemShopBoard):
#	def __del__(self):
#		ItemShopBoard.__del__(self)
#	def Destroy(self):
#		self.oldPos=(0,0)
#		self.closeBtn=None
#		self.parent = None
#	def __init__(self, parent):
#		ItemShopBoard.__init__(self)
#		self.Destroy()
#		self.parent = proxy(parent)
#		self.LoadWindow()
#	def LoadWindow(self):
#		self.AddFlag("attach")
#		self.AddFlag("float")
#		#self.AddFlag("movable")
#		self.SetSize(450*2,450)
#		closeBtn = ui.Button()
#		closeBtn.SetParent(self)
#		closeBtn.SetUpVisual(IMG_DIR+"close_0.tga")
#		closeBtn.SetOverVisual(IMG_DIR+"close_1.tga")
#		closeBtn.SetDownVisual(IMG_DIR+"close_2.tga")
#		closeBtn.SetPosition(self.GetWidth()-closeBtn.GetWidth(),0)
#		closeBtn.SetEvent(self.parent.CloseWeb)
#		closeBtn.Show()
#		self.closeBtn=closeBtn
#
#	def Close(self):
#		app.HideWebPage()
#		self.Hide()
#
#	def Open(self, url):
#		self.SetSize(450*2,450)
#		self.Show()
#		self.SetTop()
#		self.SetCenterPosition()
#
#		x, y = self.GetGlobalPosition()
#		sx, sy = x + 10, y + 30
#		ex, ey = sx + self.GetWidth() - 20, sy + self.GetHeight() - 40
#		app.ShowWebPage(url, (sx, sy, ex, ey))
#
#	def OnUpdate(self):
#		newPos = self.GetGlobalPosition()
#		if newPos == self.oldPos:
#			return
#		self.oldPos = newPos
#		x, y = newPos
#		sx, sy = x + 10, y + 30
#		ex, ey = sx + self.GetWidth() - 20, sy + self.GetHeight() - 40
#		app.MoveWebPage((sx, sy, ex, ey))


class PromoCodeWindow(ItemShopBoard):
	def __del__(self):
		ItemShopBoard.__del__(self)
	
	def __init__(self):
		ItemShopBoard.__init__(self)
		self.AddFlag("attach")
		self.AddFlag("float")
		self.Destroy()
		self.LoadWindow()
	
	def Destroy(self):
		self.children = {}
		self.acceptEvent = None
		self.toolTip = None
	
	def LoadWindow(self):
		self.SetSize(350, 180)
		
		#questionIcon = ui.ExpandedImageBox()
		#questionIcon.SetParent(self)
		#questionIcon.LoadImage("elvion/game/itemshop/question_mark.png")
		#questionIcon.SetPosition(self.GetWidth() - questionIcon.GetWidth() - 10, 10)
		#questionIcon.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__OnQuestionOverIn)
		#questionIcon.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__OnQuestionOverOut)
		#questionIcon.Show()
		#self.children["questionIcon"] = questionIcon
		
		titleText = ui.TextLine()
		titleText.SetParent(self)
		titleText.SetPosition(self.GetWidth()/2, 25)
		titleText.SetHorizontalAlignCenter()
		titleText.SetFontName("Verdana:14b")
		titleText.SetText(localeInfo.ITEMSHOP_PROMOCODE)
		titleText.Show()
		self.children["titleText"] = titleText
		
		descText = MultiTextLine()
		descText.SetParent(self)
		descText.SetTextRange(25)
		descText.SetPosition(self.GetWidth()/2, 55)
		descText.SetTextType("horizontal#center")
		descText.SetFontName("Verdana:12b")
		descText.SetText(localeInfo.ITEMSHOP_PROMOCODE_DESC)
		descText.Show()
		self.children["descText"] = descText
		
		inputBg = ui.ExpandedImageBox()
		inputBg.SetParent(self)
		inputBg.LoadImage(IMG_DIR + "voucher_bg.png")
		inputBg.SetPosition(72, 85)
		inputBg.Show()
		self.children["inputBg"] = inputBg
		
		promoInput = ui.EditLine()
		promoInput.SetParent(inputBg)
		promoInput.SetMax(30)
		promoInput.SetPosition(10, 10)
		promoInput.SetSize(inputBg.GetWidth() - 20, inputBg.GetHeight() - 10)
		promoInput.Show()
		self.children["promoInput"] = promoInput
		
		placeholderText = ui.TextLine()
		placeholderText.SetParent(promoInput)
		placeholderText.SetPosition(5, 0)
		placeholderText.SetFontColor(128/255.0, 128/255.0, 128/255.0)
		placeholderText.SetFontName("Tahoma:12")
		placeholderText.SetText(localeInfo.ITEMSHOP_PROMOCODE_PLACEHOLDER)
		placeholderText.Show()
		promoInput.placeholderText = placeholderText
		
		promoInput.OnIMEUpdate = ui.__mem_func__(self.__OnPromoInputUpdate)
		promoInput.OnPressEscapeKey = ui.__mem_func__(self.Close)
		
		confirmBtn = ui.Button()
		confirmBtn.SetParent(self)
		confirmBtn.SetUpVisual(IMG_DIR+"buy_0.png")
		confirmBtn.SetOverVisual(IMG_DIR+"buy_1.png")
		confirmBtn.SetDownVisual(IMG_DIR+"buy_2.png")
		confirmBtn.SetText(localeInfo.ITEMSHOP_PROMOCODE_CONFIRM)
		confirmBtn.SetPosition((self.GetWidth()/2) - confirmBtn.GetWidth() - 10, 136)
		confirmBtn.SetEvent(ui.__mem_func__(self.__OnConfirm))
		confirmBtn.Show()
		self.children["confirmBtn"] = confirmBtn
		
		cancelBtn = ui.Button()
		cancelBtn.SetParent(self)
		cancelBtn.SetUpVisual(IMG_DIR+"buy_0.png")
		cancelBtn.SetOverVisual(IMG_DIR+"buy_1.png")
		cancelBtn.SetDownVisual(IMG_DIR+"buy_2.png")
		cancelBtn.SetText(localeInfo.UI_CANCEL)
		cancelBtn.SetPosition((self.GetWidth()/2) + 10, 136)
		cancelBtn.SetEvent(ui.__mem_func__(self.Close))
		cancelBtn.Show()
		self.children["cancelBtn"] = cancelBtn
	
	def __OnQuestionOverIn(self):
		pass
		#if not self.toolTip:
		#	self.toolTip = uiToolTip.ToolTip()
		#
		#self.toolTip.ClearToolTip()
		#self.toolTip.SetThinBoardSize(300,50)
		#self.toolTip.AutoAppendTextLineItemshop(localeInfo.ITEMSHOP_PROMOCODE_TOOLTIP_TITLE, 0xFFd6c8a9)
		#self.toolTip.AppendSpace(5)
		#self.toolTip.AutoAppendTextLineItemshop(localeInfo.ITEMSHOP_PROMOCODE_TOOLTIP_LINE1, 0xFFb6a786, TRUE)
		#self.toolTip.AppendSpace(2)
		#self.toolTip.AutoAppendTextLineItemshop(localeInfo.ITEMSHOP_PROMOCODE_TOOLTIP_LINE2, 0xFFb6a786, TRUE)
		#self.toolTip.AppendSpace(2)
		#self.toolTip.AutoAppendTextLineItemshop(localeInfo.ITEMSHOP_PROMOCODE_TOOLTIP_LINE3, 0xFFb6a786, TRUE)
		#self.toolTip.AppendSpace(2)
		#self.toolTip.AutoAppendTextLineItemshop(localeInfo.ITEMSHOP_PROMOCODE_TOOLTIP_LINE4, 0xFFb6a786, TRUE)
		#self.toolTip.AppendSpace(2)
		#self.toolTip.AutoAppendTextLineItemshop(localeInfo.ITEMSHOP_PROMOCODE_TOOLTIP_LINE5, 0xFFb6a786, TRUE)
		#self.toolTip.AppendSpace(2)
		#self.toolTip.AutoAppendTextLineItemshop(localeInfo.ITEMSHOP_PROMOCODE_TOOLTIP_LINE6, 0xFFb6a786, TRUE)
		#self.toolTip.AppendSpace(2)
		#self.toolTip.ShowToolTip()
	
	def __OnQuestionOverOut(self):
		if self.toolTip:
			self.toolTip.HideToolTip()
	
	def __OnPromoInputUpdate(self):
		promoInput = self.children["promoInput"]
		ui.EditLine.OnIMEUpdate(promoInput)
		
		if len(promoInput.GetText()) > 0:
			promoInput.placeholderText.Hide()
		else:
			promoInput.placeholderText.Show()
	
	def __OnConfirm(self):
		promoCode = self.children["promoInput"].GetText()
		if len(promoCode) == 0:
			return
		
		if self.acceptEvent:
			self.acceptEvent(promoCode)
		
		self.Close()
	
	def SetAcceptEvent(self, event):
		self.acceptEvent = event
	
	def Open(self):
		self.children["promoInput"].SetText("")
		self.children["promoInput"].SetFocus()
		self.children["promoInput"].placeholderText.Show()
		self.SetCenterPosition()
		self.SetTop()
		self.Show()
	
	def Close(self):
		if self.toolTip:
			self.toolTip.HideToolTip()
		self.Hide()
	
	def OnPressEscapeKey(self):
		self.Close()
		return True

class BuyWindow(ItemShopBoard):
	def __del__(self):
		ItemShopBoard.__del__(self)
	def __init__(self):
		ItemShopBoard.__init__(self)
		self.AddFlag("attach")
		self.AddFlag("float")
		self.Destroy()
		#popupShadow = ui.ImageBox()
		#popupShadow.SetParent(self)
		#popupShadow.LoadImage(IMG_DIR+"popup_shadow.png")
		#popupShadow.SetPosition(-315,-83)
		#popupShadow.Show()
		#self.children["popup_shadow"] = popupShadow
		buyText = MultiTextLine()
		buyText.SetParent(self)
		buyText.SetTextRange(25)
		buyText.SetPosition(self.GetWidth()/2,28)
		buyText.SetTextType("horizontal#center")
		buyText.SetFontName("Verdana:12b")
		buyText.Show()
		self.children["buyText"] = buyText
		yesBtn = ui.Button()
		yesBtn.SetParent(self)
		yesBtn.SetUpVisual(IMG_DIR+"buy_popup_norm.png")
		yesBtn.SetOverVisual(IMG_DIR+"buy_popup_hover.png")
		yesBtn.SetDownVisual(IMG_DIR+"buy_popup_down.png")
		yesBtn.SetText(localeInfo.ITEMSHOP_WHEEL)
		yesBtn.Show()
		self.children["yesBtn"] = yesBtn
		cancelBtn = ui.Button()
		cancelBtn.SetParent(self)
		cancelBtn.SetUpVisual(IMG_DIR+"cancel_btn_norm.png")
		cancelBtn.SetOverVisual(IMG_DIR+"cancel_btn_hover.png")
		cancelBtn.SetDownVisual(IMG_DIR+"cancel_btn_down.png")
		cancelBtn.SetText(localeInfo.UI_CANCEL)
		cancelBtn.Show()
		self.children["cancelBtn"] = cancelBtn
	def Destroy(self):
		self.children={}
	def SetText(self, text):
		buyText = self.children["buyText"]
		buyText.SetText(text)
		(x, y) = buyText.GetTextSize()
		(minWidth,minHeight) = (256,142)
		if x+40 > minWidth:
			minWidth = x+40
		if not ((minWidth%2) == 0):
			minWidth+=1
		self.SetSize(minWidth,minHeight)
		buyText.SetPosition(self.GetWidth()/2,28)
		self.children["yesBtn"].SetPosition((self.GetWidth()/2)+15,94)
		self.children["cancelBtn"].SetPosition((self.GetWidth()/2)-self.children["cancelBtn"].GetWidth()-15,94)
	def SetCancelEvent(self, func):
		self.children["cancelBtn"].SetEvent(func)
	def SetAcceptEvent(self, func):
		self.children["yesBtn"].SetEvent(func)
	def OnPressEscapeKey(self):
		self.children["cancelBtn"].CallEvent()
		return True

class PaymentWindow(ItemShopBoard):
	def __del__(self):
		ItemShopBoard.__del__(self)
	def __init__(self):
		ItemShopBoard.__init__(self)
		self.AddFlag("attach")
		self.AddFlag("float")
		self.children = {}

		mentraBtn = ui.Button()
		mentraBtn.SetParent(self)
		mentraBtn.SetUpVisual(IMG_DIR + "mentra_norm.png")
		mentraBtn.SetOverVisual(IMG_DIR + "mentra_hover.png")
		mentraBtn.SetDownVisual(IMG_DIR + "mentra_down.png")
		mentraBtn.SetEvent(ui.__mem_func__(self.__OnMentra))
		mentraBtn.Show()
		self.children["mentraBtn"] = mentraBtn

		nowBtn = ui.Button()
		nowBtn.SetParent(self)
		nowBtn.SetUpVisual(IMG_DIR + "nowpayments_norm.png")
		nowBtn.SetOverVisual(IMG_DIR + "nowpayments_hover.png")
		nowBtn.SetDownVisual(IMG_DIR + "nowpayments_down.png")
		nowBtn.SetEvent(ui.__mem_func__(self.__OnNowPayments))
		nowBtn.Show()
		self.children["nowBtn"] = nowBtn

		# Compute window size from button dimensions
		PADDING  = 20
		GAP      = 12
		LABEL_H  = 18
		btnY     = 46 + LABEL_H
		totalBtnW = mentraBtn.GetWidth() + GAP + nowBtn.GetWidth()
		winW = max(260, totalBtnW + PADDING * 2)
		winH = btnY + max(mentraBtn.GetHeight(), nowBtn.GetHeight()) + 18

		self.SetSize(winW, winH)

		title = ui.TextLine()
		title.SetParent(self)
		title.SetFontName("Verdana:12b")
		title.SetPackedFontColor(0xFFf7dd83)
		title.SetText("Select Payment Method")
		title.SetHorizontalAlignCenter()
		title.SetPosition(winW / 2, 18)
		title.AddFlag("not_pick")
		title.Show()
		self.children["title"] = title

		# Centre the two buttons horizontally
		startX = (winW - totalBtnW) / 2
		mentraBtn.SetPosition(startX, btnY)
		nowBtn.SetPosition(startX + mentraBtn.GetWidth() + GAP, btnY)

		mentraLabel = ui.TextLine()
		mentraLabel.SetParent(self)
		mentraLabel.SetFontName("Verdana:12b")
		mentraLabel.SetPackedFontColor(0xFF8a4a9e)
		mentraLabel.SetText("Pay via Mentra.gg")
		mentraLabel.SetOutline()
		mentraLabel.SetHorizontalAlignCenter()
		mentraLabel.SetPosition(startX + mentraBtn.GetWidth() / 2, btnY - LABEL_H + 2)
		mentraLabel.AddFlag("not_pick")
		mentraLabel.Show()
		self.children["mentraLabel"] = mentraLabel

		nowLabel = ui.TextLine()
		nowLabel.SetParent(self)
		nowLabel.SetFontName("Verdana:12b")
		nowLabel.SetPackedFontColor(0xFF64acff)
		nowLabel.SetText("Pay via Crypto")
		nowLabel.SetOutline()
		nowLabel.SetHorizontalAlignCenter()
		nowLabel.SetPosition(startX + mentraBtn.GetWidth() + GAP + nowBtn.GetWidth() / 2, btnY - LABEL_H + 2)
		nowLabel.AddFlag("not_pick")
		nowLabel.Show()
		self.children["nowLabel"] = nowLabel

		closeBtn = ui.Button()
		closeBtn.SetParent(self)
		closeBtn.SetUpVisual(IMG_DIR + "close_0.png")
		closeBtn.SetOverVisual(IMG_DIR + "close_1.png")
		closeBtn.SetDownVisual(IMG_DIR + "close_2.png")
		closeBtn.SetEvent(ui.__mem_func__(self.Hide))
		closeBtn.SetPosition(winW - closeBtn.GetWidth() - 4, 4)
		closeBtn.Show()
		self.children["closeBtn"] = closeBtn

	def Destroy(self):
		self.children = {}

	def __OnMentra(self):
		os.system("start \"\" " + WEBSITE_LINK)
		self.Hide()

	def __OnNowPayments(self):
		os.system("start \"\" " + NOWPAYMENTS_LINK)
		self.Hide()

	def OnPressEscapeKey(self):
		self.Hide()
		return True

class SortClass:
	def __init__(self, arg, isLog = False):
		self.arg = arg
		self.isLog = isLog
	def __del__(self):
		self.arg = []
	def __lt__(self, other):
		if self.isLog:
			return self.arg[LOG_DATE] >= other.arg[LOG_DATE]
		else:
			currentTime = app.GetGlobalTimeStamp()
			# hot item
			if self.arg[LIST_ITEM_TOP_SELLING_INDEX] <= 10 or other.arg[LIST_ITEM_TOP_SELLING_INDEX] <= 10:
				return self.arg[LIST_ITEM_TOP_SELLING_INDEX] <= other.arg[LIST_ITEM_TOP_SELLING_INDEX]
			# time offer
			if self.arg[LIST_ITEM_TIME] != 0 or other.arg[LIST_ITEM_TIME] != 0:
				if self.arg[LIST_ITEM_TIME] > 0 and other.arg[LIST_ITEM_TIME] > 0:
					return ((self.arg[LIST_ITEM_TIME]-currentTime) <= (other.arg[LIST_ITEM_TIME]-currentTime))
				else:
					return self.arg[LIST_ITEM_TIME] >= other.arg[LIST_ITEM_TIME]
			# discount
			if self.arg[LIST_ITEM_DISCOUNT] != 0 or other.arg[LIST_ITEM_DISCOUNT] != 0:
				return self.arg[LIST_ITEM_DISCOUNT] >= other.arg[LIST_ITEM_DISCOUNT]
			# added time
			if self.arg[LIST_ITEM_ADDED_TIME] > currentTime-(60*60*24*3) or other.arg[LIST_ITEM_ADDED_TIME] > currentTime-(60*60*24*3):
				return self.arg[LIST_ITEM_ADDED_TIME] >= other.arg[LIST_ITEM_ADDED_TIME]
			return self.arg[LIST_ITEM_ID] >= other.arg[LIST_ITEM_ID]

class ItemShopBanner(ui.ExpandedImageBox):
	def __del__(self):
		ui.ExpandedImageBox.__del__(self)
	def Destroy(self):
		self.bannerIndex = -1
		self.childrenImages = {}
		self.bannerImageIndex = 0
		self.bannerState = "show"
		self.bannerTimer = 0.0
	def __init__(self):
		ui.ExpandedImageBox.__init__(self)
		self.Destroy()
	def LoadData(self, bannerIndex):
		self.bannerIndex = bannerIndex
		if bannerIndex >= len(eventCategory):
			return
		self.LoadImage(IMG_DIR + eventCategory[bannerIndex][0][0])
		self.childrenImages["self"] = self
		self.bannerImageIndex = 0
		self.bannerState = "show"
		self.bannerTimer = 0.0  # initialized on first OnUpdate call
	def OnUpdate(self):
		if self.bannerIndex < 0 or self.bannerIndex >= len(eventCategory):
			return
		images = eventCategory[self.bannerIndex][0]
		if len(images) <= 1:
			return
		now = app.GetTime()
		if self.bannerTimer == 0.0:
			self.bannerTimer = now
			return
		if self.bannerState == "show":
			if now - self.bannerTimer >= BANNER_SHOW_TIME:
				self.bannerState = "fading_out"
				self.bannerTimer = now
		elif self.bannerState == "fading_out":
			t = (now - self.bannerTimer) / BANNER_FADE_TIME
			if t >= 1.0:
				self.bannerImageIndex = (self.bannerImageIndex + 1) % len(images)
				self.LoadImage(IMG_DIR + images[self.bannerImageIndex])
				self.SetAlpha(0.0)  # LoadImage resets diffuse alpha — reapply after
				self.bannerState = "fading_in"
				self.bannerTimer = now
			else:
				self.SetAlpha(1.0 - t)
		elif self.bannerState == "fading_in":
			t = (now - self.bannerTimer) / BANNER_FADE_TIME
			if t >= 1.0:
				self.SetAlpha(1.0)
				self.bannerState = "show"
				self.bannerTimer = now
			else:
				self.SetAlpha(t)
	def OnRender(self, parent):
		listboxHeight = parent.GetHeight()
		(pgx,pgy) = parent.GetGlobalPosition()
		for key, image in self.childrenImages.items():
			(jgx,jgy) = image.GetGlobalPosition()
			imageHeight = image.GetHeight()
			if jgy < pgy:
				image.SetRenderingRect(0,ui.calculateRect(imageHeight-abs(jgy-pgy),imageHeight),0,0)
			elif jgy+imageHeight > (pgy+listboxHeight-8):
				calculate = (pgy+listboxHeight-8) - (jgy+imageHeight)
				if calculate == 0:
					return
				image.SetRenderingRect(0.0,0.0,0.0,ui.calculateRect(imageHeight-abs(calculate),imageHeight))
			else:
				image.SetRenderingRect(0,0,0,0)

class ItemShopItem(ui.ExpandedImageBox):
	def __del__(self):
		ui.ExpandedImageBox.__del__(self)
	def __init__(self):
		ui.ExpandedImageBox.__init__(self)
		self.Destroy()
		self.buyCount=1
		self.LoadImage(IMG_DIR+"item_box.png")
		self.childrenImages["self"] = self
	def SetClippingMaskWindow(self, window):
		self.clippingWindow = window
		ui.Window.SetClippingMaskWindow(self, window)
		
		if hasattr(self, 'slot'):
			self.slot.SetClippingMaskWindow(window)
		if hasattr(self, 'buyButton'):
			self.buyButton.SetClippingMaskWindow(window)
		if hasattr(self, 'itemNameText'):
			self.itemNameText.SetClippingMaskWindow(window)
		if hasattr(self, 'itemPriceText'):
			self.itemPriceText.SetClippingMaskWindow(window)
		if hasattr(self, 'itemCountText'):
			self.itemCountText.SetClippingMaskWindow(window)
	def Destroy(self):
		self.data = []
		self.buyCount=0
		self.butButtonSet = False
		self.childrenImages = {}
		self.childrenTextLine = {}

	def LoadData(self, itemData):
		self.data = itemData
		self.SetItem(itemData[LIST_ITEM_VNUM])
		self.SetCount()
		self.SetItemName()
		self.SetBuyButton(itemData[LIST_ITEM_TIME], itemData[LIST_ITEM_MAX_SELLING_COUNT])

		if app.USE_ITEMSHOP_RENEWED:
			self.SetPrice(itemData[LIST_ITEM_PRICE], itemData[LIST_ITEM_DISCOUNT], itemData[LIST_ITEM_JPRICE])
		else:
			self.SetPrice(itemData[LIST_ITEM_PRICE], itemData[LIST_ITEM_DISCOUNT])

		self.SetTime(itemData[LIST_ITEM_TIME])
		self.SetLastSellCount(itemData[LIST_ITEM_MAX_SELLING_COUNT])

		if itemData[LIST_ITEM_TOP_SELLING_INDEX] >= 0 and itemData[LIST_ITEM_TOP_SELLING_INDEX] <= 10:
			self.SetItemType(1)
		elif itemData[LIST_ITEM_TIME] > 0 or itemData[LIST_ITEM_DISCOUNT] > 0:
			self.SetItemType(3)
		elif itemData[LIST_ITEM_ADDED_TIME] > app.GetGlobalTimeStamp()-(60*60*24*3):
			self.SetItemType(2)

	def SetLastSellCount(self, maxSellingCount):
		if maxSellingCount != -1:
			self.ClearChildren("maxSelling")
			maxSelling = MultiTextLine()
			maxSelling.SetParent(self)
			maxSelling.SetTextRange(20)
			maxSelling.SetTextType("horizontal#left")
			maxSelling.SetFontName("Verdana:12b")
			maxSelling.SetText(localeInfo.ITEMSHOP_LAST_COUNT%maxSellingCount)
			maxSelling.SetPosition(110, 60)
			maxSelling.Show()
			self.childrenTextLine["maxSelling"] = maxSelling

	def ClearChildren(self, childrenName):
		if self.childrenTextLine.has_key(childrenName):
			self.childrenTextLine[childrenName].Hide()
			self.childrenTextLine[childrenName].Destroy()
			del self.childrenTextLine[childrenName]
		if self.childrenImages.has_key(childrenName):
			self.childrenImages[childrenName].Hide()
			self.childrenImages[childrenName].Destroy()
			del self.childrenImages[childrenName]

	def FormatTime(self, seconds):
		if seconds <= 0:
			if self.butButtonSet == False:
				self.SetBuyButton(self.data[LIST_ITEM_TIME])
			return "|cffFF7B7B0h 0m 0s"
		m, s = divmod(seconds, 60)
		h, m = divmod(m, 60)
		d, h = divmod(h, 24)
		if d > 0:
			return "|cffFF7B7B%dd %dh %dm %ds"%(d,h,m,s)
		return "|cffFF7B7B%dh %dm %ds" % (h,m,s)

	def OnUpdate(self):
		if self.childrenTextLine.has_key("timeText"):
			self.childrenTextLine["timeText"].SetText(self.FormatTime(self.data[LIST_ITEM_TIME]-app.GetGlobalTimeStamp()))

	def SetTime(self, time):
		self.ClearChildren("timeText")
		if time != 0:
			self.butButtonSet = False
			
			timeText = ui.TextLine()
			timeText.SetParent(self)
			timeText.AddFlag("not_pick")
			timeText.SetPosition(186,65)
			timeText.SetWindowHorizontalAlignLeft()
			timeText.SetFontName("Tahoma:13")
			timeText.Show()
			self.childrenTextLine["timeText"] = timeText

	def SetBuyButton(self, time, maxSellingCount = -1):
		self.ClearChildren("buyButton")
		self.butButtonSet = False
		buyButton = ui.ExpandedImageBox()
		buyButton.SetParent(self)
		if((time != 0 and app.GetGlobalTimeStamp() > time) or maxSellingCount == 0):
			buyButton.LoadImage(IMG_DIR+"button_buy_disabled.png")
			self.butButtonSet = True
		else:
			buyButton.LoadImage(IMG_DIR+"buy_0.png")
			buyButton.SAFE_SetStringEvent("MOUSE_OVER_IN",self.OverInBuyBtn)
			buyButton.SAFE_SetStringEvent("MOUSE_OVER_OUT",self.OverOutBuyBtn)
			buyButton.SetEvent(ui.__mem_func__(self.ClickBuyBtn),"mouse_click")
		buyButton.SetPosition(97,84)
		buyButton.Show()
		self.childrenImages["buyButton"] = buyButton

		buyBtnText = ui.TextLine()
		buyBtnText.SetParent(buyButton)
		buyBtnText.AddFlag("not_pick")
		buyBtnText.SetPosition(0,7)
		buyBtnText.SetHorizontalAlignCenter()
		buyBtnText.SetWindowHorizontalAlignCenter()
		buyBtnText.SetPackedFontColor(0xFFf7bd48)
		#buyBtnText.SetWindowVerticalAlignCenter()
		buyBtnText.SetText(localeInfo.ITEMSHOP_BUY)
		buyBtnText.Show()
		self.childrenTextLine["buyBtnText"] = buyBtnText

	def ClickBuyBtn(self, emptyArg):
		time = self.data[LIST_ITEM_TIME]
		if time != 0 and app.GetGlobalTimeStamp() > time:
			return
		
		global buyQuestionDialog
		if not buyQuestionDialog:
			buyQuestionDialog = BuyWindow()


		priceIsJetons = False

		if app.USE_ITEMSHOP_RENEWED and self.data[LIST_ITEM_JPRICE] > 0:
			priceIsJetons = True
			price = self.data[LIST_ITEM_JPRICE] * self.buyCount
		else:
			price = self.data[LIST_ITEM_PRICE] * self.buyCount

		discount = self.data[LIST_ITEM_DISCOUNT]

		if discount > 0:
			price = int((float(price) / 100.0) * float(100 - discount))

		item.SelectItem(self.data[LIST_ITEM_VNUM])

		text = localeInfo.ITEMSHOP_BUY_TEXT_PRICE % (self.buyCount, item.GetItemName())
		text += "\n"

		if discount > 0:
			text += (localeInfo.ITEMSHOP_BUY_PRICE_JETONS_WITH_DISCOUNT if priceIsJetons else localeInfo.ITEMSHOP_BUY_PRICE_COINS_WITH_DISCOUNT) % (price, discount)
		else:
			text += (localeInfo.ITEMSHOP_BUY_PRICE_JETONS if priceIsJetons else localeInfo.ITEMSHOP_BUY_PRICE_COINS) % (price)

		buyQuestionDialog.SetText(text)
		buyQuestionDialog.SetAcceptEvent(self.AcceptBuy)
		buyQuestionDialog.SetCancelEvent(self.CloseQuestionDialog)
		buyQuestionDialog.SetCenterPosition()
		buyQuestionDialog.SetTop()
		buyQuestionDialog.Show()

	def CloseQuestionDialog(self):
		global buyQuestionDialog
		if buyQuestionDialog!= None:
			buyQuestionDialog.Hide()
			buyQuestionDialog.Destroy()
			buyQuestionDialog=None

	def AcceptBuy(self):
		net.SendChatPacket("/ishop buy %d %d"%(self.data[LIST_ITEM_ID],self.buyCount))
		self.CloseQuestionDialog()

	def EmptyFunction(self):
		pass

	def OverOutBuyBtn(self):
		if self.childrenImages.has_key("buyButton"):
			self.childrenImages["buyButton"].LoadImage(IMG_DIR+"buy_0.png")

	def OverInBuyBtn(self):
		if self.childrenImages.has_key("buyButton"):
			self.childrenImages["buyButton"].LoadImage(IMG_DIR+"buy_1.png")

	def SetPrice(self, price, discount, itemPriceJD = 0):
		self.ClearChildren("lineImage")
		self.ClearChildren("itemPriceEx")
		self.ClearChildren("itemPrice")

		itemPrice = MultiTextLine()
		itemPrice.AddFlag("not_pick")
		itemPrice.SetParent(self)
		itemPrice.SetTextRange(20)
		itemPrice.SetPosition(140, 38)
		itemPrice.SetTextType("horizontal#left")
		itemPrice.SetFontName("Tahoma:13")

		priceIsJetons = False

		if app.USE_ITEMSHOP_RENEWED and itemPriceJD > 0:
			priceIsJetons = True
			price = itemPriceJD * self.buyCount
		else:
			price = price * self.buyCount

		text = ""

		if discount > 0:
			newPrice = int((float(price) / 100.0) * float(100 - discount))

			text = (localeInfo.ITEMSHOP_PRICE_JUST_JETONS_1 if priceIsJetons else localeInfo.ITEMSHOP_PRICE_JUST_COINS_1) % NumberToMoneyFormat(price)

			itemPriceEx = ui.TextLine()
			itemPriceEx.SetParent(self)
			itemPriceEx.SetText(text)
			itemPriceEx.Hide()
			(xSize, ySize) = itemPriceEx.GetTextSize()

			self.childrenTextLine["itemPriceEx"] = itemPriceEx

			text = (localeInfo.ITEMSHOP_PRICE_JETONS if priceIsJetons else localeInfo.ITEMSHOP_PRICE_COINS) % (NumberToMoneyFormat(price))
			text += "\n"
			text += (localeInfo.ITEMSHOP_PRICE_JETONS_WITH_DISCOUNT if priceIsJetons else localeInfo.ITEMSHOP_PRICE_COINS_WITH_DISCOUNT) % (NumberToMoneyFormat(newPrice), discount)

			lineImage = ui.ExpandedImageBox()
			lineImage.SetParent(itemPrice)
			lineImage.AddFlag("not_pick")
			lineImage.LoadImage(IMG_DIR + "line.tga")
			lineImage.SetDiffuseColor(1.0, 0.0, 0.0, 0.9)
			scaleCalc = float(xSize) * (1.0 / 10.0)
			lineImage.SetScale(scaleCalc, 1.0)
			lineImage.SetPosition(17, (ySize / 2) + 1)
			lineImage.Show()

			self.childrenImages["lineImage"] = lineImage

			self.SetItemType(3)
		else:
			text = (localeInfo.ITEMSHOP_PRICE_JETONS_1 if priceIsJetons else localeInfo.ITEMSHOP_PRICE_COINS_1) % (NumberToMoneyFormat(price))

		itemPrice.SetText(text)
		itemPrice.Show()

		self.childrenTextLine["itemPrice"] = itemPrice

	def SetItemName(self):
		self.ClearChildren("itemName")
		itemName = ui.TextLine()
		itemName.SetParent(self)
		itemName.AddFlag("not_pick")
		itemName.SetHorizontalAlignCenter()
		itemName.SetWindowHorizontalAlignCenter()
		itemName.SetPackedFontColor(0xFFffffff)
		itemName.SetFontName("Tahoma:12")
		itemName.SetPosition(43,14)
		itemName.SetText(item.GetItemName())
		#itemName.SetFontColor(247,189,72)
		itemName.Show()
		self.childrenTextLine["itemName"] = itemName

	def SetCount(self):
		self.ClearChildren("CountBg")
		self.ClearChildren("CountText")
		self.ClearChildren("CountLeft")
		self.ClearChildren("CountRight")
		if not item.IsFlag(item.ITEM_FLAG_STACKABLE):
			return

		CountLeft = ui.ExpandedImageBox()
		CountLeft.SetParent(self)
		CountLeft.LoadImage(IMG_DIR+"count_left.png")
		CountLeft.SetPosition(25,95)
		CountLeft.SetEvent(ui.__mem_func__(self.ClickCount),"mouse_click",0)
		CountLeft.Show()
		self.childrenImages["CountLeft"] = CountLeft

		CountBg = ui.ExpandedImageBox()
		CountBg.SetParent(self)
		CountBg.AddFlag("not_pick")
		CountBg.LoadImage(IMG_DIR+"count_back.png")
		CountBg.SetPosition(25+CountLeft.GetWidth(),95)
		CountBg.Show()
		self.childrenImages["CountBg"] = CountBg

		CountText = ui.TextLine()
		CountText.SetParent(CountBg)
		CountText.AddFlag("not_pick")
		CountText.SetHorizontalAlignCenter()
		CountText.SetVerticalAlignCenter()
		CountText.SetWindowHorizontalAlignCenter()
		CountText.SetWindowVerticalAlignCenter()
		CountText.SetText("1")
		CountText.Show()
		self.childrenTextLine["CountText"] = CountText

		CountRight = ui.ExpandedImageBox()
		CountRight.SetParent(self)
		CountRight.LoadImage(IMG_DIR+"count_right.png")
		CountRight.SetPosition(25+CountLeft.GetWidth()+CountBg.GetWidth(),95)
		CountRight.SetEvent(ui.__mem_func__(self.ClickCount),"mouse_click",1)
		CountRight.Show()
		self.childrenImages["CountRight"] = CountRight

	def ClickCount(self,emptyArg, btnType):
		value = 1
		if app.IsPressed(app.DIK_LCONTROL):
			value += 4

		buyCount = self.buyCount

		if btnType == 0:
			buyCount -= value

			if buyCount <= 0:
				buyCount = 1
		else:
			buyCount += value

			if buyCount <= 0:
				buyCount = 1
			elif buyCount > 20:
				buyCount = 20

		self.buyCount = buyCount

		if self.childrenTextLine.has_key("CountText"):
			self.childrenTextLine["CountText"].SetText(str(self.buyCount))

		itemData = self.data

		if app.USE_ITEMSHOP_RENEWED:
			self.SetPrice(itemData[LIST_ITEM_PRICE], itemData[LIST_ITEM_DISCOUNT], itemData[LIST_ITEM_JPRICE])
		else:
			self.SetPrice(itemData[LIST_ITEM_PRICE], itemData[LIST_ITEM_DISCOUNT])

	def SetItem(self, itemVnum):
		self.ClearChildren("itemImage")
		self.ClearChildren("itemCount")
		item.SelectItem(itemVnum)
		(width,height) = item.GetItemSize()
		(xPos, yPos) = (36,46)
		if height == 1:
			xPos = 36
			yPos += 10
		elif height == 2:
			yPos -= 7
		elif height == 3:
			yPos -= 30
		itemImage = ui.ExpandedImageBox()
		itemImage.SetParent(self)
		itemImage.LoadImage(item.GetIconImageFileName())
		itemImage.SetPosition(xPos,yPos)
		itemImage.SAFE_SetStringEvent("MOUSE_OVER_IN",self.OverInItem,itemVnum)
		itemImage.SAFE_SetStringEvent("MOUSE_OVER_OUT",self.OverOutItem)
		itemImage.Show()
		self.childrenImages["itemImage"] = itemImage

	def OverInItem(self, itemVnum):
		interface = constInfo.GetInterfaceInstance()
		if interface:
			if interface.tooltipItem:
				interface.tooltipItem.SetItemToolTip(itemVnum)
	def OverOutItem(self):
		interface = constInfo.GetInterfaceInstance()
		if interface:
			if interface.tooltipItem:
				interface.tooltipItem.HideToolTip()
	def SetItemType(self, itemType):
		self.ClearChildren("itemType")
		typeImages = ["hot.png","new.png","offer.png"]
		if itemType == 0:
			return
		itemType-=1
		if itemType >= len(typeImages):
			return
		typeImage = ui.ExpandedImageBox()
		typeImage.AddFlag("not_pick")
		typeImage.SetParent(self)
		typeImage.LoadImage(IMG_DIR+typeImages[itemType])
		typeImage.SetPosition(4,4)
		typeImage.Show()
		self.childrenImages["typeImage"] = typeImage

	def OnRender(self, parent):
		listboxHeight = parent.GetHeight()
		(pgx,pgy) = parent.GetGlobalPosition()
		for key, textline in self.childrenTextLine.items():
			(igx,igy) = textline.GetGlobalPosition()
			if igy < pgy:
				textline.Hide()
			elif igy > (pgy+listboxHeight-25):
				textline.Hide()
			else:
				textline.Show()
		for key, image in self.childrenImages.items():
			(jgx,jgy) = image.GetGlobalPosition()
			imageHeight = image.GetHeight()
			if jgy < pgy:
				image.SetRenderingRect(0,ui.calculateRect(imageHeight-abs(jgy-pgy),imageHeight),0,0)
			elif jgy+imageHeight > (pgy+listboxHeight-8):
				calculate = (pgy+listboxHeight-8) - (jgy+imageHeight)
				if calculate == 0:
					return
				image.SetRenderingRect(0.0,0.0,0.0,ui.calculateRect(imageHeight-abs(calculate),imageHeight))
			else:
				image.SetRenderingRect(0,0,0,0)

class ItemWindow(ui.Window):
	def __del__(self):
		ui.Window.__del__(self)
	def __init__(self, parent):
		ui.Window.__init__(self)
		self.children=[]
		self.Destroy()
		self.parent=proxy(parent)
		self.SetSize(750,122)
	def OnUpdate(self):
		for children in self.children:
			children.OnUpdate()
	def Destroy(self):
		for children in self.children:
			children.Destroy()
			children=None
		self.children=[]
		self.parent = None
	def AppendWindow(self, window):
		self.children.append(window)
		self.OnRender()
	def OnRender(self):
		for children in self.children:
			children.OnRender(self.parent)
class CategoryItem(ui.ExpandedImageBox):
	def __del__(self):
		ui.ExpandedImageBox.__del__(self)
	def __init__(self):
		ui.ExpandedImageBox.__init__(self)
		self.Destroy()
		self.categoryIndex=-1
		self.LoadImage(IMG_DIR+"button2_0.png")
	
	def SetData(self, categoryIndex, categoryName, categoryListBox):
		self.categoryIndex = categoryIndex
		self.parent = proxy(categoryListBox)
		self.isExpanded = False
		self.hasSubCategories = False
		
		if itemShopCategoryData.has_key(categoryIndex):
			categoryData = itemShopCategoryData[categoryIndex]["subCategory"]
			self.hasSubCategories = len(categoryData) > 0
		
		isSubCategory = categoryName.startswith("  ")
		
		categoryText = MultiTextLine()
		categoryText.SetParent(self)
		categoryText.SetPosition(35, 12)
		categoryText.SetTextRange(15)
		categoryText.SetTextType("horizontal#left")
		categoryText.SetFontName("Tahoma:12")
		
		if isSubCategory:
			categoryText.SetFontColor(180/255.0, 160/255.0, 140/255.0)
		else:
			categoryText.SetFontColor(214/255.0, 200/255.0, 169/255.0)
		
		categoryText.SetText(categoryName)
		categoryText.Show()
		self.categoryText = categoryText
		
		if not isSubCategory and self.hasSubCategories:
			self.arrowImage = ui.ImageBox()
			self.arrowImage.SetParent(self)
			self.arrowImage.LoadImage(IMG_DIR + "arrow_up.png")
			self.arrowImage.SetPosition(self.GetWidth() - 25, 12)
			self.arrowImage.Show()
	
	def SetExpanded(self, expanded):
		self.isExpanded = expanded
		if hasattr(self, 'arrowImage') and self.arrowImage and self.hasSubCategories:
			if expanded:
				self.arrowImage.LoadImage(IMG_DIR + "arrow_down.png")
			else:
				self.arrowImage.LoadImage(IMG_DIR + "arrow_up.png")
				
	def Destroy(self):
		if not hasattr(self, 'arrowImage'):
			self.arrowImage = None
			
		if self.arrowImage:
			self.arrowImage.Hide()
			self.arrowImage = None
		self.parent = None
		self.categoryIndex = 0
		self.categoryText = None
		
	def OnRender(self):
		parent = self.parent
		if parent == None:
			return
		listboxHeight = parent.GetHeight()
		(pgx,pgy) = parent.GetGlobalPosition()
		(igx,igy) = self.categoryText.GetGlobalPosition()
		if igy < pgy:
			self.categoryText.Hide()
		elif igy > (pgy+listboxHeight-25):
			self.categoryText.Hide()
		else:
			self.categoryText.Show()
			
		if hasattr(self, 'arrowImage') and self.arrowImage:
			(agx,agy) = self.arrowImage.GetGlobalPosition()
			if agy < pgy:
				self.arrowImage.Hide()
			elif agy > (pgy+listboxHeight-25):
				self.arrowImage.Hide()
			else:
				self.arrowImage.Show()
				
		(jgx,jgy) = self.GetGlobalPosition()
		imageHeight = self.GetHeight()
		if jgy < pgy:
			self.SetRenderingRect(0,ui.calculateRect(imageHeight-abs(jgy-pgy),imageHeight),0,0)
		elif jgy+imageHeight > (pgy+listboxHeight-4):
			calculate = (pgy+listboxHeight-4) - (jgy+imageHeight)
			if calculate == 0:
				return
			self.SetRenderingRect(0.0,0.0,0.0,ui.calculateRect(imageHeight-abs(calculate),imageHeight))
		else:
			self.SetRenderingRect(0,0,0,0)
			
class ScrollBar(ui.Window):
	SCROLLBAR_WIDTH = 10
	SCROLL_BTN_XDIST = 2
	SCROLL_BTN_YDIST = 3

	class MiddleBar(ui.DragButton):
		def __init__(self):
			ui.DragButton.__init__(self)
			self.AddFlag("movable")
			self.SetWindowName("scrollbar_middlebar")
		
		def MakeImage(self):
			slider = ui.ExpandedImageBox()
			slider.SetParent(self)
			slider.LoadImage(IMG_DIR+"scroll_slider.png")
			slider.AddFlag("not_pick")
			slider.Show()
			self.slider = slider
		
		def SetSize(self, height):
			minHeight = self.slider.GetHeight()
			height = max(minHeight, height)
			ui.DragButton.SetSize(self, self.slider.GetWidth(), height)
			
			if height > minHeight:
				self.slider.SetPosition(0, (height - minHeight) // 2)
			else:
				self.slider.SetPosition(0, 0)

	def __init__(self):
		ui.Window.__init__(self)

		self.pageSize = 1
		self.curPos = 0.0
		self.eventScroll = None
		self.eventArgs = None
		self.lockFlag = False

		self.CreateScrollBar()
		self.SetScrollBarSize(0)

		self.scrollStep = 0.03
		self.SetWindowName("NONAME_ScrollBar")

	def __del__(self):
		ui.Window.__del__(self)

	def CreateScrollBar(self):
		backgroundImage = ui.ExpandedImageBox()
		backgroundImage.SetParent(self)
		backgroundImage.AddFlag("not_pick")
		backgroundImage.LoadImage(IMG_DIR+"scroll_empty.png")
		backgroundImage.Show()
		self.backgroundImage = backgroundImage

		middleBar = self.MiddleBar()
		middleBar.SetParent(self)
		middleBar.SetMoveEvent(ui.__mem_func__(self.OnMove))
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
		totalHeight = self.GetHeight() - self.SCROLL_BTN_YDIST * 2
		sliderHeight = max(20, int(pageScale * totalHeight)) 
		self.middleBar.SetSize(sliderHeight)
		realHeight = totalHeight - sliderHeight
		self.pageSize = realHeight

	def SetScrollBarSize(self, height):
		self.SetSize(self.SCROLLBAR_WIDTH, height)
		self.backgroundImage.SetPosition(1, 2)
		
		self.pageSize = height - self.SCROLL_BTN_YDIST * 2 - self.middleBar.GetHeight()
		
		self.middleBar.SetRestrictMovementArea(
			self.SCROLL_BTN_XDIST, 
			self.SCROLL_BTN_YDIST, 
			self.middleBar.GetWidth(), 
			height - self.SCROLL_BTN_YDIST * 2
		)
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST)

	def SetScrollStep(self, step):
		self.scrollStep = step

	def GetScrollStep(self):
		return self.scrollStep

	def GetPos(self):
		return self.curPos

	def OnUp(self):
		self.SetPos(self.curPos - self.scrollStep)

	def OnDown(self):
		self.SetPos(self.curPos + self.scrollStep)

	def SetPos(self, pos, moveEvent=True):
		pos = max(0.0, pos)
		pos = min(1.0, pos)
		newPos = float(self.pageSize) * pos
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, int(newPos) + self.SCROLL_BTN_YDIST)
		if moveEvent:
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
		sliderHeight = self.middleBar.GetHeight()
		availableHeight = self.GetHeight() - self.SCROLL_BTN_YDIST * 2
		clickPos = yMouseLocalPosition - self.SCROLL_BTN_YDIST - (sliderHeight // 2)
		newPos = float(clickPos) / float(self.pageSize)
		self.SetPos(newPos)

	def LockScroll(self):
		self.lockFlag = True

	def UnlockScroll(self):
		self.lockFlag = False
#class ScrollBar(ui.Window):
#	SCROLLBAR_WIDTH = 12
#	MIDDLE_BAR_POS = 5
#	MIDDLE_BAR_UPPER_PLACE = 4
#	MIDDLE_BAR_DOWNER_PLACE = 4
#	MIDDLE_BAR_PER_HEIGHT = 27
#	TEMP_SPACE = MIDDLE_BAR_UPPER_PLACE + MIDDLE_BAR_DOWNER_PLACE
#	class MiddleBar(ui.DragButton):
#		MIDDLE_BAR_PER_HEIGHT = 27
#		MIDDLE_BAR_WIDTH = 6
#		def __init__(self):
#			ui.DragButton.__init__(self)
#			self.AddFlag("movable")
#		def MakeImage(self):
#			top = ui.ImageBox()
#			top.SetParent(self)
#			top.LoadImage(IMG_DIR+"scroll_top.tga")
#			top.SetPosition(0, 0)
#			top.AddFlag("not_pick")
#			top.Show()
#			bottom = ui.ImageBox()
#			bottom.SetParent(self)
#			bottom.LoadImage(IMG_DIR+"scroll_bottom.tga")
#			bottom.AddFlag("not_pick")
#			bottom.Show()
#			middle = ui.ExpandedImageBox()
#			middle.SetParent(self)
#			middle.LoadImage(IMG_DIR+"scroll_mid.tga")
#			middle.SetPosition(0, self.MIDDLE_BAR_PER_HEIGHT)
#			middle.AddFlag("not_pick")
#			middle.Show()
#			self.top = top
#			self.bottom = bottom
#			self.middle = middle
#		def SetSize(self, height):
#			height = max(self.MIDDLE_BAR_PER_HEIGHT * 3, height)
#			ui.DragButton.SetSize(self, 10, height)
#			self.bottom.SetPosition(0, height-self.MIDDLE_BAR_PER_HEIGHT)
#			height -= self.MIDDLE_BAR_PER_HEIGHT*3
#			self.middle.SetRenderingRect(0, 0, 0, float(height)/float(self.MIDDLE_BAR_PER_HEIGHT))
#	def __init__(self):
#		ui.Window.__init__(self)
#		self.pageSize = 1
#		self.curPos = 0.0
#		self.eventScroll = lambda *arg: None
#		self.lockFlag = FALSE
#		self.scrollStep = 0.20
#		self.CreateScrollBar()
#	def __del__(self):
#		ui.Window.__del__(self)
#	def CreateScrollBar(self):
#		barSlot = ui.Bar3D()
#		barSlot.SetParent(self)
#		barSlot.AddFlag("not_pick")
#		barSlot.Show()
#		middleBar = self.MiddleBar()
#		middleBar.SetParent(self)
#		middleBar.SetMoveEvent(ui.__mem_func__(self.OnMove))
#		middleBar.Show()
#		middleBar.MakeImage()
#		middleBar.SetSize(60)
#		self.middleBar = middleBar
#		self.barSlot = barSlot
#		self.SCROLLBAR_MIDDLE_HEIGHT = self.middleBar.GetHeight()
#	def Destroy(self):
#		self.middleBar = None
#		self.eventScroll = lambda *arg: None
#	def SetScrollEvent(self, event):
#		self.eventScroll = event
#	def SetMiddleBarSize(self, pageScale):
#		realHeight = self.GetHeight()
#		self.SCROLLBAR_MIDDLE_HEIGHT = int(pageScale * float(realHeight))
#		self.middleBar.SetSize(self.SCROLLBAR_MIDDLE_HEIGHT)
#		self.pageSize = self.GetHeight() - self.SCROLLBAR_MIDDLE_HEIGHT - (self.TEMP_SPACE)
#	def SetScrollBarSize(self, height):
#		self.pageSize = height - self.SCROLLBAR_MIDDLE_HEIGHT - (self.TEMP_SPACE)
#		self.SetSize(self.SCROLLBAR_WIDTH, height)
#		self.middleBar.SetRestrictMovementArea(self.MIDDLE_BAR_POS, self.MIDDLE_BAR_UPPER_PLACE, self.MIDDLE_BAR_POS+2, height - self.TEMP_SPACE)
#		self.middleBar.SetPosition(self.MIDDLE_BAR_POS, 0)
#		self.UpdateBarSlot()
#	def UpdateBarSlot(self):
#		self.barSlot.SetPosition(0, 2)
#		self.barSlot.SetSize(self.GetWidth() - 2, self.GetHeight() - 2)
#	def GetPos(self):
#		return self.curPos
#	def SetPos(self, pos):
#		pos = max(0.0, pos)
#		pos = min(1.0, pos)
#		newPos = float(self.pageSize) * pos
#		self.middleBar.SetPosition(self.MIDDLE_BAR_POS, int(newPos) + self.MIDDLE_BAR_UPPER_PLACE)
#		self.OnMove()
#	def SetScrollStep(self, step):
#		self.scrollStep = step
#	def GetScrollStep(self):
#		return self.scrollStep
#	def OnUp(self):
#		self.SetPos(self.curPos-self.scrollStep)
#	def OnDown(self):
#		self.SetPos(self.curPos+self.scrollStep)
#	def OnMove(self):
#		if self.lockFlag:
#			return
#		if 0 == self.pageSize:
#			return
#		(xLocal, yLocal) = self.middleBar.GetLocalPosition()
#		self.curPos = float(yLocal - self.MIDDLE_BAR_UPPER_PLACE) / float(self.pageSize)
#		self.eventScroll()
#	def OnMouseLeftButtonDown(self):
#		(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()
#		pickedPos = yMouseLocalPosition - self.SCROLLBAR_MIDDLE_HEIGHT/2
#		newPos = float(pickedPos) / float(self.pageSize)
#		self.SetPos(newPos)
#	def LockScroll(self):
#		self.lockFlag = TRUE
#	def UnlockScroll(self):
#		self.lockFlag = FALSE
def NumberToMoneyFormat(n):
	if n <= 0 :
		return "|cfffaf8f50|r"
	return "|cfffaf8f5%s|r" % ('.'.join([ i-3<0 and str(n)[:i] or str(n)[i-3:i] for i in range(len(str(n))%3, len(str(n))+1, 3) if i ]))
class MultiTextLine(ui.Window):
	def __del__(self):
		ui.Window.__del__(self)
	def Destroy(self):
		self.children = []
		self.rangeText = 0
	def __init__(self):
		ui.Window.__init__(self)
		self.children = []
		self.rangeText = 15
		self.textType = ""
		self.fontName = ""
		self.outline = False
		self.rgb = (0,0,0)
	def SetFontColor(self, r, g, b):
		self.rgb = (r, g, b)
		for text in self.children:
			if self.rgb[0] != 0:
				text.SetFontColor(self.rgb[0],self.rgb[1],self.rgb[2])
	def SetOutline(self, flag):
		self.outline = flag
		for text in self.children:
			if self.outline:
				text.SetOutline()
	def SetTextType(self, textType):
		self.textType = textType
		for text in self.children:
			self.AddTextType(self.textType.split("#"),text)
	def SetFontName(self, fontName):
		self.fontName = fontName
		for text in self.children:
			text.SetFontName(fontName)
	def GetTextSize(self, index = 0):
		if len(self.children) == 0:
			return (0,0)
		elif index >= len(self.children):
			return (0,0)
		return self.children[index].GetTextSize()

	def SetTextRange(self, range):
		self.rangeText = range
		yPosition = 0
		for text in self.children:
			text.SetPosition(0,yPosition)
			yPosition+=self.rangeText
	def AddTextType(self, typeArg, text):
		if len(typeArg) > 1:
			if typeArg[0] == "vertical":
				if typeArg[1] == "top":
					text.SetVerticalAlignTop()
				elif typeArg[1] == "bottom":
					text.SetVerticalAlignBottom()
				elif typeArg[1] == "center":
					text.SetVerticalAlignCenter()
			elif typeArg[0] == "horizontal":
				if typeArg[1] == "left":
					text.SetHorizontalAlignLeft()
				elif typeArg[1] == "right":
					text.SetHorizontalAlignRight()
				elif typeArg[1] == "center":
					text.SetHorizontalAlignCenter()
	def SetText(self, cmd):
		if len(self.children) > 1:
			self.children=[]
		multi_arg = cmd.split("\n")
		yPosition = 0
		for text in multi_arg:
			childText = ui.TextLine()
			childText.SetParent(self)
			childText.SetPosition(0,yPosition)
			if self.rgb[0] != 0:
				childText.SetFontColor(self.rgb[0],self.rgb[1],self.rgb[2])
			if self.textType != "":
				self.AddTextType(self.textType.split("#"),childText)
			if self.fontName != "":
				childText.SetFontName(self.fontName)
			if self.outline:
				childText.SetOutline()
			childText.SetText(str(text))
			childText.Show()
			self.children.append(childText)
			yPosition+=self.rangeText
class ListBox(ui.Window):
	def __init__(self, isHorizontal = False):
		ui.Window.__init__(self)
		self.viewItemCount=10
		self.basePos=0
		self.itemHeight=16
		self.itemStep=20
		self.itemList=[]
		self.itemWidth=100
		self.scrollBar=None
		self.isHorizontal= isHorizontal
	def __del__(self):
		ui.Window.__del__(self)
	def __GetItemCount(self):
		return len(self.itemList)
	def SetItemStep(self, itemStep):
		self.itemStep=itemStep
	def SetItemSize(self, itemWidth, itemHeight):
		self.itemWidth=itemWidth
		self.itemHeight=itemHeight
	def SetViewItemCount(self, viewItemCount):
		self.viewItemCount=viewItemCount
	def RemoveAllItems(self):
		for item in self.itemList:
			item.Destroy()
		self.itemList=[]
		if self.scrollBar:
			self.scrollBar.SetPos(0)
	def GetItems(self):
		return self.itemList
	def RemoveItem(self, delItem):
		self.itemList.remove(delItem)
	def AppendItem(self, newItem):
		self.itemList.append(newItem)
		if app.ENABLE_MOUSEWHEEL_EVENT:
			newItem.SetMouseWheelScrollEvent(self.OnMouseWheelScroll_ScrollBar)
	def SetScrollBar(self, scrollBar):
		scrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScroll))
		self.scrollBar=scrollBar
		if app.ENABLE_MOUSEWHEEL_EVENT:
			self.SetMouseWheelScrollEvent(self.OnMouseWheelScroll_ScrollBar)
	if app.ENABLE_MOUSEWHEEL_EVENT:
		def OnMouseWheelScroll_ScrollBar(self,mode):
			if self.scrollBar:
				if mode == "UP":
					self.scrollBar.OnUp()
				else:
					self.scrollBar.OnDown()
	def OnMouseWheel(self, nLen):
		if self.scrollBar:
			if self.scrollBar.IsShow():
				if nLen > 0:
					self.scrollBar.OnUp()
				else:
					self.scrollBar.OnDown()
				return True
		return False
	def __OnScroll(self):
		self.SetBasePos(int(self.scrollBar.GetPos()*self.__GetScrollLen()))
	def __GetScrollLen(self):
		if self.__GetItemCount() == 0:
			return 0
		(lx,ly) = self.itemList[len(self.itemList)-1].exPos
		return ly
	def Render(self,basePos):
		for item in self.itemList:
			(ex,ey) = item.exPos
			if self.isHorizontal:
				item.SetPosition(ex-(basePos),ey)
			else:
				item.SetPosition(ex,ey-(basePos))
			item.OnRender()
	def SetBasePos(self, basePos):
		if self.basePos == basePos:
			return
		self.Render(basePos)
		self.basePos=basePos

class ImageBoxSpecial(ui.Window):
	def __del__(self):
		ui.Window.__del__(self)
	def Destroy(self):
		self.imageList=[]
		self.waitingTime = 0.0
		self.sleepTime = 0.0
		self.alphaValue = 0.0
		self.increaseValue = 0.0
		self.minAlpha = 0.0
		self.maxAlpha = 0.0
		self.alphaStatus = False
		self.imageIndex = 0
		self.img = None
		self.numberText = None

	def __init__(self):
		ui.Window.__init__(self)
		self.Destroy()
		self.waitingTime = 2.0
		self.alphaValue = 0.3
		self.increaseValue = 0.05
		self.minAlpha = 0.3
		self.maxAlpha = 1.0
	def SetImage(self, folder, itemVnum, itemCount):
		(x, y) = (0,0)
		if self.img != None:
			(x, y) = self.img.GetLocalPosition()
		if self.img == None:
			img = ui.ImageBox()
			img.SetParent(self)
			img.SAFE_SetStringEvent("MOUSE_OVER_OUT",self.OverOutItem)
			img.SAFE_SetStringEvent("MOUSE_OVER_IN",self.OverInItem)
			self.img = img
		if self.numberText == None:
			numberText = ui.NumberLine()
			numberText.SetParent(self.img)
			numberText.SetPosition(20,20)
			self.numberText = numberText

		self.numberText.SetNumber(str(itemCount))
		self.numberText.Show()

		self.img.LoadImage(folder)
		self.img.SetPosition(x, y)
		self.img.Show()

	def OverOutItem(self):
		interface = constInfo.GetInterfaceInstance()
		if interface:
			if interface.tooltipItem:
				interface.tooltipItem.HideToolTip()
	def OverInItem(self):
		interface = constInfo.GetInterfaceInstance()
		if interface:
			if interface.tooltipItem:
				interface.tooltipItem.SetItemToolTip(self.imageList[self.imageIndex][1])
	def Clear(self):
		self.img = None
		self.numberText = None
		self.imageList = []
	def LoadImage(self, imageFolder, itemVnum, itemCount):
		item_list = [imageFolder, itemVnum, itemCount]
		if item_list in self.imageList:
			return
		self.imageList.append(item_list)
		if self.img == None:
			self.SetImage(imageFolder, itemVnum, itemCount)
	def GetNextImage(self, listIndex):
		if listIndex >= len(self.imageList):
			if len(self.imageList) > 0:
				return (0,self.imageList[0][0],self.imageList[0][1],self.imageList[0][2])
			return (0,"",0,0)
		return (listIndex, self.imageList[listIndex][0],self.imageList[listIndex][0], self.imageList[listIndex][2])
	def OnUpdate(self):
		if len(self.imageList) <= 1:
			self.imageIndex=0
			return
		elif self.sleepTime > app.GetTime():
			return
		if self.alphaStatus == True:
			self.alphaValue -= self.increaseValue
			if self.alphaValue < self.minAlpha:
				self.alphaValue = self.minAlpha
				self.alphaStatus = False
				(imageIndex, imageFolder, itemVnum, itemCount) = self.GetNextImage(self.imageIndex+1)
				if imageFolder != "":
					self.SetImage(imageFolder, itemVnum, itemCount)
				self.imageIndex = imageIndex
		else:
			self.alphaValue += self.increaseValue
			if self.alphaValue > self.maxAlpha:
				self.alphaStatus = True
				self.sleepTime = app.GetTime()+self.waitingTime
		if self.img != None:
			self.img.SetAlpha(self.alphaValue)

class MessageWindow(ui.BoardWithTitleBar):
	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)
	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.Destroy()
	def Destroy(self):
		self.children = {}
	def LoadMessage(self, itemVnum, itemCount):
		self.SetTitleName(localeInfo.WHEEL_REWARD_TITLE)
		self.SetCloseEvent(self.Close)
		self.SetSize(210,180)
		self.AddFlag("attach")
		self.AddFlag("movable")
		self.AddFlag("float")
		self.SetCenterPosition()

		item.SelectItem(itemVnum)

		multiText = MultiTextLine()
		multiText.SetParent(self)
		multiText.SetTextType("horizontal#center")
		multiText.SetPosition(95,45)
		if itemCount > 1:
			text = "%s\nx%d%s\n%s"%(localeInfo.WHEEL_REWARD_TEXT, itemCount,item.GetItemName(), localeInfo.WHEEL_REWARD_TEXT_1)
		else:
			text = "%s\n%s\n%s"%(localeInfo.WHEEL_REWARD_TEXT,item.GetItemName(), localeInfo.WHEEL_REWARD_TEXT_1)
		multiText.SetText(text)
		multiText.Show()
		self.children["multiText"] = multiText

		itemIcon = ui.ImageBox()
		itemIcon.SetParent(self)
		itemIcon.LoadImage(item.GetIconImageFileName())
		itemIcon.SetPosition(105-16,95)
		itemIcon.Show()
		self.children["itemIcon"] = itemIcon

		itemCountNumber = ui.NumberLine()
		itemCountNumber.SetParent(itemIcon)
		itemCountNumber.SetNumber(str(itemCount))
		itemCountNumber.SetPosition(20,20)
		itemCountNumber.Show()
		self.children["itemCount"] = itemCountNumber

		okBtn = ui.Button()
		okBtn.SetParent(self)
		okBtn.SetUpVisual("d:/ymir work/ui/public/middle_button_01.sub")
		okBtn.SetOverVisual("d:/ymir work/ui/public/middle_button_02.sub")
		okBtn.SetDownVisual("d:/ymir work/ui/public/middle_button_03.sub")
		okBtn.SetText(localeInfo.UI_OK)
		okBtn.SetEvent(ui.__mem_func__(self.Close))
		okBtn.SetPosition(105-(okBtn.GetWidth()/2),135)
		okBtn.Show()
		self.children["okBtn"] = okBtn

		self.Show()

	def Close(self):
		self.Destroy()
		self.Hide()
	def OnPressEscapeKey(self):
		self.Close()
		return True
