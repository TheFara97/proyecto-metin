import ui, item, localeInfo, uiToolTip, app, constInfo, ingamelog, player
from _weakref import proxy

HISTORY_OFFLINESHOP, HISTORY_EXCHANGE = xrange(2)

PATH_ROOT = "offlineshop/lightwork/log/"
PATH_ROOT_SEARCH = "offlineshop/lightwork/searchshop/"

wndInGameLog = None

class IngameLogWindow(ui.Window):

	def __init__(self):
		ui.Window.__init__(self)
		self.ResetInformations()
		self.LoadPages()

		self.interface = constInfo.GetInterfaceInstance()
		self.toolTipInfo = uiToolTip.ToolTip()

	def __del__(self):
		ui.Window.__del__(self)
		self.ResetInformations()

	def LoadPages(self):
		self.AddFlag("movable")

		self.InitPage(550, 450, HISTORY_OFFLINESHOP, localeInfo.INGAME_LOG_TITLE_01)
		self.InitPage(550, 450, HISTORY_EXCHANGE, localeInfo.INGAME_LOG_TITLE_02)
		self.LoadButtons()

	def InitPage(self, width, height, page, title):
		self.wndPage[page] = ui.MakeBoardWithTitleBar(self, "not_pick", title, ui.__mem_func__(self.Close), width, height)
		self.SetSize(self.wndPage[page].GetWidth(), self.wndPage[page].GetHeight())
		self.SetCenterPosition()

		if page == HISTORY_OFFLINESHOP:
			self.wndPage[page].Show()

			self.border = ui.BorderA()
			self.border.SetParent(self.wndPage[page])
			self.border.SetPosition(10, 30)
			self.border.SetSize(530, 360)
			self.border.Show()

			self.titleBG = ui.ExpandedImageBox()
			self.titleBG.SetParent(self.wndPage[page])
			self.titleBG.LoadImage(PATH_ROOT + "titlebg.png")
			self.titleBG.SetPosition(15, 35)
			self.titleBG.Show()

			self.wndOfflineShopHistory = OfflineShopHistory()
			self.wndOfflineShopHistory.SetParent(self)
			self.wndOfflineShopHistory.SetSize(500, 316)
			self.wndOfflineShopHistory.SetPosition(15, 65)
			self.wndOfflineShopHistory.Hide()

			self.OfflineShopScrollbar = ui.ScrollBarAe()
			self.OfflineShopScrollbar.SetParent(self)
			self.OfflineShopScrollbar.SetScrollBarSize(226 + 87)
			# self.OfflineShopScrollbar.UpdateScrollWidth(40)
			self.OfflineShopScrollbar.SetPosition(386+84+4+50, 65)
			self.wndOfflineShopHistory.SetScrollBar(self.OfflineShopScrollbar)
			self.OfflineShopScrollbar.Hide()

			self.texts = [localeInfo.INGAME_LOG_OFFLINESHOP_ITEM_INFO_0,
						localeInfo.INGAME_LOG_OFFLINESHOP_ITEM_INFO_1,
						localeInfo.INGAME_LOG_OFFLINESHOP_ITEM_INFO_2,
						localeInfo.INGAME_LOG_OFFLINESHOP_ITEM_INFO_3]
			self.textList = []

			posList = [50, 180, 315, 440]

			for i in xrange(len(self.texts)):
				self.textList.append(i)
				self.textList[i] = ui.TextLine()
				self.textList[i].SetParent(self.titleBG)
				self.textList[i].SetPosition(posList[i], 5.5)
				self.textList[i].SetHorizontalAlignCenter()
				self.textList[i].SetText(self.texts[i])
				self.textList[i].Show()

		elif page == HISTORY_EXCHANGE:
			self.wndPage[page].Show()

			self.borderEx = ui.BorderA()
			self.borderEx.SetParent(self.wndPage[HISTORY_EXCHANGE])
			self.borderEx.SetPosition(10, 30)
			self.borderEx.SetSize(530, 360)
			self.borderEx.Show()

			self.titleBGEx = ui.ExpandedImageBox()
			self.titleBGEx.SetParent(self.wndPage[page])
			self.titleBGEx.LoadImage(PATH_ROOT + "titlebg.png")
			self.titleBGEx.SetPosition(15, 35)
			self.titleBGEx.Show()

			self.wndExchangeHistory = ExchangeHistory()
			self.wndExchangeHistory.SetParent(self)
			self.wndExchangeHistory.SetSize(500, 316)
			self.wndExchangeHistory.SetPosition(15, 65)
			self.wndExchangeHistory.Hide()

			self.ExchangeScrollbar = ui.ScrollBarAe()
			self.ExchangeScrollbar.SetParent(self)
			self.ExchangeScrollbar.SetScrollBarSize(226 + 87)
			# self.ExchangeScrollbar.UpdateScrollWidth(40)
			self.ExchangeScrollbar.SetPosition(386+84+4+50, 65)
			self.wndExchangeHistory.SetScrollBar(self.ExchangeScrollbar)
			self.ExchangeScrollbar.Hide()

			self.wndViewLogBG = ui.MakeExpandedImageBox(self, PATH_ROOT + "seeinglogbg.png", 11, 56)
			self.wndViewLogBG.OnMouseLeftButtonDown = ui.__mem_func__(self.OnSelectImageBox)
			self.wndViewLogBG.Hide()

			global wndInGameLog

			wndInGameLog = ExchangeWindow()
			wndInGameLog.SetParent(self.wndViewLogBG)
			wndInGameLog.SetPosition(0, 0)
			wndInGameLog.SetWindowVerticalAlignCenter()
			wndInGameLog.SetWindowHorizontalAlignCenter()
			wndInGameLog.Hide()

			self.excTexts = [localeInfo.INGAME_LOG_EXCHANGE_ITEM_INFO_0,
						localeInfo.INGAME_LOG_OFFLINESHOP_ITEM_INFO_3,
						localeInfo.INGAME_LOG_EXCHANGE_ITEM_INFO_1]
			self.excTextList = []

			for i in xrange(len(self.excTexts)):
				self.excTextList.append(i)
				self.excTextList[i] = ui.TextLine()
				self.excTextList[i].SetParent(self.titleBGEx)
				self.excTextList[i].SetPosition(70 + i * 170, 5.5)
				self.excTextList[i].SetHorizontalAlignCenter()
				self.excTextList[i].SetText(self.excTexts[i])
				self.excTextList[i].Show()

	def UpdateLog(self, logID):
		wndInGameLog.UpdateTradeInfo(logID)

	def LoadButtons(self):

		self.offShopLogButton = ui.RadioButton()
		self.offShopLogButton.SetParent(self)
		self.offShopLogButton.SetUpVisual(PATH_ROOT+"shop_norm.png")
		self.offShopLogButton.SetOverVisual(PATH_ROOT+"shop_hover.png")
		self.offShopLogButton.SetDownVisual(PATH_ROOT+"shop_down.png")
		self.offShopLogButton.SetPosition(10, 395)
		self.offShopLogButton.SetEvent(ui.__mem_func__(self.ChangePage), HISTORY_OFFLINESHOP)
		self.offShopLogButton.Show()

		self.exchangeLogButton = ui.RadioButton()
		self.exchangeLogButton.SetParent(self)
		self.exchangeLogButton.SetUpVisual(PATH_ROOT+"history_norm.png")
		self.exchangeLogButton.SetOverVisual(PATH_ROOT+"history_hover.png")
		self.exchangeLogButton.SetDownVisual(PATH_ROOT+"history_down.png")
		self.exchangeLogButton.SetPosition(277, 395)
		self.exchangeLogButton.SetEvent(ui.__mem_func__(self.ChangePage), HISTORY_EXCHANGE)
		self.exchangeLogButton.Show()

		self.InformationButton = ui.MakeButton(self, 25, 8, False, "d:/ymir work/ui/pattern/", "q_mark_01.tga", "q_mark_02.tga", "q_mark_01.tga")
		self.InformationButton.ShowToolTip = lambda arg=1: self.OverInButton()
		self.InformationButton.HideToolTip = lambda arg=1: self.OverOutButton()

		self.ChangePage(0)

	def OnSelectImageBox(self):
		self.wndViewLogBG.Hide()
		global wndInGameLog
		wndInGameLog.Hide()

	def SetPage(self, page):
		self.ChangePage(page)

	def ChangePage(self, page):
		if not ingamelog.OpenLog(page):
			return

		self.wndPage[page].Show()
		self.wndPage[HISTORY_OFFLINESHOP].Hide() if page == HISTORY_EXCHANGE else self.wndPage[HISTORY_EXCHANGE].Hide()

		if page == HISTORY_OFFLINESHOP:
			self.wndOfflineShopHistory.appendLogDatas()
			# if self.wndOfflineShopHistory.GetItemCount() > 6:
			self.OfflineShopScrollbar.Show()
			self.wndOfflineShopHistory.Show()
			self.ExchangeScrollbar.Hide()
			self.wndExchangeHistory.Hide()
			self.wndViewLogBG.Hide()
			wndInGameLog.Hide()

			self.offShopLogButton.Down()
			self.offShopLogButton.Disable()
			self.exchangeLogButton.SetUp()
			self.exchangeLogButton.Enable()

		else:
			self.wndExchangeHistory.appendLogDatas()
			# if self.wndExchangeHistory.GetItemCount() > 6:
			self.ExchangeScrollbar.Show()
			self.wndExchangeHistory.Show()
			self.OfflineShopScrollbar.Hide()
			self.wndOfflineShopHistory.Hide()
			self.wndViewLogBG.Hide()
			wndInGameLog.Hide()

			self.offShopLogButton.SetUp()
			self.offShopLogButton.Enable()

			self.exchangeLogButton.Down()
			self.exchangeLogButton.Disable()

		self.wndViewLogBG.Hide()

	def OnUpdate(self):
		if wndInGameLog.IsShow():
			self.wndViewLogBG.Show()
		else:
			self.wndViewLogBG.Hide()

	def Open(self):
		self.Show()
		self.SetTop()

		if self.wndPage[HISTORY_OFFLINESHOP].IsShow():
			if self.wndOfflineShopHistory.IsShow():
				self.wndOfflineShopHistory.appendLogDatas()

		elif self.wndPage[HISTORY_EXCHANGE].IsShow():
			if self.wndExchangeHistory.IsShow():
				self.wndExchangeHistory.appendLogDatas()
			if not wndInGameLog.IsShow():
				wndInGameLog.Show()

	def OverInButton(self):
		if self.InformationButton.IsIn() and self.toolTipInfo:
			self.toolTipInfo.ClearToolTip()
			self.toolTipInfo.AlignHorizonalCenter()
			self.toolTipInfo.AutoAppendNewTextLine(localeInfo.INGAME_LOG_INFORMATION)
			self.toolTipInfo.Show()

	def OverOutButton(self):
		if self.toolTipInfo:
			self.toolTipInfo.Hide()

	def Close(self):
		self.Hide()

		if wndInGameLog:
			wndInGameLog.Close()

		if self.toolTipInfo:
			self.toolTipInfo.Hide()

	def Destroy(self):
		self.ResetInformations()
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnPressExitKey(self):
		self.Close()
		return True

	def ResetInformations(self):
		self.OfflineShopScrollbar = None
		self.interface = None
		self.toolTipInfo = None
		self.wndPage = {}

	def OnRunMouseWheel(self, nLen):
		if nLen > 0:
			if self.wndPage[HISTORY_OFFLINESHOP].IsShow():
				self.OfflineShopScrollbar.OnUp()
			else:
				self.ExchangeScrollbar.OnUp()
		else:
			if self.wndPage[HISTORY_OFFLINESHOP].IsShow():
				self.OfflineShopScrollbar.OnDown()
			else:
				self.ExchangeScrollbar.OnDown()

class OfflineShopHistory(ui.Window):
	class NewItem(ui.Window):
		def __init__(self, index, data):
			ui.Window.__init__(self)
			self.background = None
			self.bSelected = False
			self.OnResetThings()
			self.Index = index
			self.Data = data

			item.SelectItem(data["vnum"])
			x, y = item.GetItemSize()

			self.background = ui.ExpandedImageBox()
			self.background.SetParent(self)
			# if self.Index != 0:
			self.background.LoadImage(PATH_ROOT + "background_image_{}.png".format(y))
			# else:
			# 	self.background.LoadImage(PATH_ROOT + "bar_complete.tga")
				
			self.background.OnMouseOverIn = ui.__mem_func__(self.OnMouseOverInImage)
			self.background.OnMouseOverOut = ui.__mem_func__(self.OnMouseOverOutImage)
			self.background.Show()
		
			self.SetSize(self.background.GetWidth(), self.background.GetHeight())
			_, self.itemSize = item.GetItemSize()
			self.ItemSlotBase = ui.ExpandedImageBox()
			self.ItemSlotBase.SetParent(self)
			self.ItemSlotBase.SetWindowHorizontalAlignLeft()
			self.ItemSlotBase.SetPosition(5, 1)
			self.ItemSlotBase.LoadImage("offlineshop/lightwork/slot_32x%d.tga" % (self.itemSize * 32))
			self.ItemSlotBase.SetWindowVerticalAlignCenter()
			self.ItemSlotBase.SetStringEvent("MOUSE_OVER_IN", ui.__mem_func__(self.OnMouseOverInImage))
			self.ItemSlotBase.SetStringEvent("MOUSE_OVER_OUT", ui.__mem_func__(self.OnMouseOverOutImage))
			self.ItemSlotBase.Show()
			if y > 2:
				self.ItemSlotBase.SetScale(0.95, 0.95)
				self.ItemSlotBase.SetPosition(5, 1)

			self.wndIcon = ui.ExpandedImageBox()
			self.wndIcon.SetParent(self.ItemSlotBase)
			self.wndIcon.AddFlag("not_pick")
			self.wndIcon.LoadImage(item.GetIconImageFileName())
			self.wndIcon.SetPosition(2, 1)
			self.wndIcon.Show()

			itemName = item.GetItemName()
			itemName = itemName if len(itemName) < 12 else itemName[:12] + "..."

			self.wndName = ui.MakeTextLineNew(self, 55, 0, self.HasAttrItem(data) + itemName)
			self.wndPrice = ui.MakeTextLineNew(self, -60, 0, localeInfo.NumberToMoneyString(data["price"]))
			self.wndPrice.SetWindowHorizontalAlignCenter()
			self.wndPrice.SetHorizontalAlignCenter()
			self.wndPrice.SetWindowVerticalAlignCenter()
			self.wndPrice.SetVerticalAlignCenter()
			self.wndName.SetWindowVerticalAlignCenter()
			self.wndName.SetVerticalAlignCenter()

			date = data["date"]

			self.wndDate = ui.MakeTextLineNew(self, 30, 0, date[5:-3] if date != "Updating" else date)
			self.wndDate.SetWindowHorizontalAlignRight()
			self.wndDate.SetHorizontalAlignRight()
			self.wndDate.SetHorizontalAlignRight()
			self.wndDate.SetWindowVerticalAlignCenter()
			self.wndDate.SetVerticalAlignCenter()

			buyerName = data["buyername"]
			self.wndBuyer = ui.MakeTextLineNew(self, 65, 0, buyerName)
			self.wndBuyer.SetWindowHorizontalAlignCenter()
			self.wndBuyer.SetHorizontalAlignCenter()
			self.wndBuyer.SetWindowVerticalAlignCenter()
			self.wndBuyer.SetVerticalAlignCenter()

			self.OnMouseOverOutImage()

		def HasAttrItem(self, Data):
			attrs	= [(Data["attr"][num]['type'], Data["attr"][num]['value']) for num in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]
			if attrs:
				for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
					apply = attrs[i][0]
					if 0 > apply:
						return "|cFFFFC700"

			return "|cFFF1E6C0"

		def __del__(self):
			ui.Window.__del__(self)

		def OnMouseOverInImage(self):
			if self.tooltipItem:
				sockets = [self.Data["socket"][num] for num in xrange(player.METIN_SOCKET_MAX_NUM)]
				attrs	= [(self.Data["attr"][num]['type'], self.Data["attr"][num]['value']) for num in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]
			
				self.tooltipItem.ClearToolTip()

				self.tooltipItem.AppendIcon(self.Data["vnum"], self.Data["count"])
				#self.tooltipItem.AddItemData(self.Data["vnum"], 0)
				self.tooltipItem.AddItemData(self.Data["vnum"], sockets, attrs)
				
				# self.tooltipItem.UpdateRect()
				
			self.background.SetAlpha(1.0)

		def OnMouseOverOutImage(self):
			if self.tooltipItem:
				self.tooltipItem.HideToolTip()
				
			self.background.SetAlpha(0.8)

		def SetItemToolTip(self, tooltip):
			self.tooltipItem = tooltip

		def OnResetThings(self):
			self.IsSelected = False
			self.bIsBlocked = False
			self.vnum = 0
			self.xBase = 0
			self.yBase = 0

			self.clickEvent = None
			if self.background != None:
				self.background.Hide()
			self.background = None
			self.wndIcon = None
			self.tooltipItem = None
			self.Data = None

		def SetParent(self, parent):
			ui.Window.SetParent(self, parent)
			self.parent = proxy(parent)

		def SetBasePosition(self, x, y):
			self.xBase = x
			self.yBase = y
			
		def GetBasePosition(self):
			return (self.xBase, self.yBase)
			
		def SetClickEvent(self, event):
			self.clickEvent = event
		
		def Destroy(self):
			self.OnResetThings()
		
		def OnRender(self):
			xList, yList = self.parent.GetGlobalPosition()
			widthList, heightList = self.parent.GetWidth(), self.parent.GetHeight()	

			images = [self.background, self.wndIcon, self.ItemSlotBase]
			# images = [self.background]
			for img in images:
				if img:
					img.SetClipRect(xList, yList, xList + widthList, yList + heightList)

			textList = [self.wndName, self.wndPrice, self.wndDate, self.wndBuyer]
			for text in textList:
				if text:
					text.SetClippingMaskWindow(self.parent)
					# xText, yText = text.GetGlobalPosition()

					# if yText < yList or yText + text.GetTextSize()[1] > yList + heightList:
					# 	text.Hide()
					# else:
					# 	text.Show()

	def __init__(self):
		ui.Window.__init__(self)

		self.OnResetThings()

		self.tooltipItem = uiToolTip.ItemToolTip()

	def __del__(self):
		ui.Window.__del__(self)
		self.OnResetThings()
		
	def Destroy(self):
		self.OnResetThings()
		
	def OnResetThings(self):
		self.SetFuncDown = None
		self.SelectIndexFunc = None
		self.itemList = []
		self.scrollBar = None

		self.selectEvent = None
		self.tooltipItem = None
	
	def SetEventSelect(self, event):
		self.SelectIndexFunc = event

	def SetScrollBar(self, scrollBar):
		scrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScroll))
		scrollBar.SetScrollStep(0.1)
		self.scrollBar = scrollBar

	def SetSelectEvent(self, event):
		self.selectEvent = event
		
	def __OnScroll(self):
		self.RefreshItemPosition(True)
			
	def GetTotalHeightItems(self):
		totalHeight = 0
		
		if self.itemList:
			for itemH in self.itemList:
				totalHeight += itemH.GetHeight()

		return totalHeight

	def GetItemCount(self):
		return len(self.itemList)

	def AppendItem(self, index, data):
		item = self.NewItem(index, data)
		item.SetParent(self)
		item.SetItemToolTip(self.tooltipItem)

		if len(self.itemList) == 0:
			item.SetBasePosition(0, 0)
		else:
			x, y = self.itemList[-1].GetLocalPosition()
			y += 2
			item.SetBasePosition(0, y + self.itemList[-1].GetHeight())

		item.Show()
		self.itemList.append(item)

		self.ResetScrollBarPos()
		self.ResizeScrollBar()	
		self.RefreshItemPosition()
		
		if len(self.itemList) <= 6:
			self.scrollBar.Hide()
		else:
			self.scrollBar.Show()	

	def ResizeScrollBar(self):
		totalHeight = float(self.GetTotalHeightItems())
		if totalHeight:
			scrollBarHeight = min(float(self.GetHeight() - 10) / totalHeight, 1.0)
		else:
			scrollBarHeight = 1.0
			
		self.scrollBar.SetMiddleBarSize(scrollBarHeight)

	def ResetScrollBarPos(self):
		self.scrollBar.SetPos(0)

	def RefreshItemPosition(self, bScroll = False):		
		scrollPos = self.scrollBar.GetPos()
		totalHeight = self.GetTotalHeightItems() - self.GetHeight()

		idx, CurIdx, yAccumulate = 0, 0, 0
		for item in self.itemList[idx:]:
			if bScroll:
				setPos = yAccumulate - int(scrollPos * totalHeight)
				item.SetPosition(0, setPos)
			else:
				item.SetPosition(0, yAccumulate)
				
			item.SetBasePosition(0, yAccumulate)

			CurIdx += 1
			yAccumulate += item.GetHeight()

	def Clear(self):
		range = len(self.itemList)
		
		if range > 0:
			for item in self.itemList:
				item.OnResetThings()
				item.Hide()
				del item

		self.itemList = []

	def appendLogDatas(self):
		if not ingamelog.IsLoadOfflineShop():
			return

		sold_item_count = ingamelog.SoldItemCount()
		if sold_item_count == 0:
			return

		self.Clear()

		for i in xrange(sold_item_count):
			offlineShopLog = {}
			offlineShopLog["vnum"] = int(ingamelog.SoldItemGetVnum(i))
			offlineShopLog["count"] = int(ingamelog.SoldItemGetCount(i))
			offlineShopLog["price"] =  long(ingamelog.SoldItemGetPrice(i))
			offlineShopLog["buyername"] = str(ingamelog.SoldItemGetBuyerName(i))
			offlineShopLog["date"] = str(ingamelog.SoldItemGetDate(i))
			
			offlineShopLog["attr"] = {}
			for z in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				offlineShopLog["attr"][z] = {}
				offlineShopLog["attr"][z]["type"] = int(ingamelog.SoldItemGetAttrTpye(i,z))
				offlineShopLog["attr"][z]["value"] = int(ingamelog.SoldItemGetAttrValue(i,z))

			offlineShopLog["socket"] = {}
			for y in xrange(player.METIN_SOCKET_MAX_NUM):
				offlineShopLog["socket"][y] = int(ingamelog.SoldItemGetSocket(i,y))

			self.AppendItem(i, offlineShopLog)

class ExchangeHistory(ui.Window):
	def __init__(self):
		ui.Window.__init__(self)

		self.OnResetThings()

	def __del__(self):
		ui.Window.__del__(self)
		self.OnResetThings()
		
	def Destroy(self):
		self.OnResetThings()
		
	def OnResetThings(self):
		self.SetFuncDown = None
		self.SelectIndexFunc = None
		self.itemList = []
		self.scrollBar = None

		self.selectEvent = None
	
	def SetEventSelect(self, event):
		self.SelectIndexFunc = event

	def SetScrollBar(self, scrollBar):
		scrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScroll))
		scrollBar.SetScrollStep(0.1)
		self.scrollBar = scrollBar

		self.appendLogDatas()

	def SetSelectEvent(self, event):
		self.selectEvent = event
		
	def __OnScroll(self):
		self.RefreshItemPosition(True)
			
	def GetTotalHeightItems(self):
		totalHeight = 0
		
		if self.itemList:
			for itemH in self.itemList:
				totalHeight += itemH.GetHeight()
			
		return totalHeight

	def GetItemCount(self):
		return len(self.itemList)

	def AppendItem(self, index, data):
		item = self.NewItem(index, data)
		item.SetParent(self)

		if len(self.itemList) == 0:
			item.SetBasePosition(0, 0)
		else:
			x, y = self.itemList[-1].GetLocalPosition()
			y += 2
			item.SetBasePosition(0, y + self.itemList[-1].GetHeight())

		item.Show()
		self.itemList.append(item)

		self.ResetScrollBarPos()
		self.ResizeScrollBar()	
		self.RefreshItemPosition()
		
		if len(self.itemList) <= 10:
			self.scrollBar.Hide()
		else:
			self.scrollBar.Show()	

	def ResizeScrollBar(self):
		totalHeight = float(self.GetTotalHeightItems())
		if totalHeight:
			scrollBarHeight = min(float(self.GetHeight() - 10) / totalHeight, 1.0)
		else:
			scrollBarHeight = 1.0
			
		self.scrollBar.SetMiddleBarSize(scrollBarHeight)

	def ResetScrollBarPos(self):
		self.scrollBar.SetPos(0)

	def RefreshItemPosition(self, bScroll = False):		
		scrollPos = self.scrollBar.GetPos()
		totalHeight = self.GetTotalHeightItems() - self.GetHeight()

		idx, CurIdx, yAccumulate = 0, 0, 0
		for item in self.itemList[idx:]:
			if bScroll:
				setPos = yAccumulate - int(scrollPos * totalHeight)
				item.SetPosition(0, setPos)
			else:
				item.SetPosition(0, yAccumulate)
				
			item.SetBasePosition(0, yAccumulate)

			CurIdx += 1
			yAccumulate += item.GetHeight()

	def Clear(self):
		range = len(self.itemList)
		
		if range > 0:
			for item in self.itemList:
				item.OnResetThings()
				item.Hide()
				del item

		self.itemList = []

	def appendLogDatas(self):
		if not ingamelog.IsLoadedTradeLog():
			return

		tradeLogCount = ingamelog.TradeLogInfoCount()
		if tradeLogCount == 0:
			return

		self.Clear()

		for i in xrange(tradeLogCount):
			exchangeLog = {}
			exchangeLog["logid"] = int(ingamelog.GetTradeLogId(i))
			exchangeLog["victimname"] = str(ingamelog.GetTradeVictimName(i))
			exchangeLog["date"] =  str(ingamelog.GetTradeDate(i))
			self.AppendItem(i, exchangeLog)

	class NewItem(ui.Window):
		def __init__(self, index, data):
			ui.Window.__init__(self)
			self.background = None
			self.bSelected = False
			self.OnResetThings()
			self.Index = index
			self.Data = data

			self.background = ui.ExpandedImageBox()
			self.background.SetParent(self)
			# if self.Index != 0:
			self.background.LoadImage(PATH_ROOT + "background_image_1.png")
			# else:
			# 	self.background.LoadImage(PATH_ROOT + "bar_complete.tga")
			self.background.Show()
		
			self.SetSize(self.background.GetWidth(), self.background.GetHeight())

			buyerName = data["victimname"]
			self.wndTradedWith = ui.MakeTextLineNew(self, -170, 0, buyerName)
			self.wndTradedWith.SetWindowHorizontalAlignCenter()
			self.wndTradedWith.SetHorizontalAlignCenter()
			self.wndTradedWith.SetWindowVerticalAlignCenter()
			self.wndTradedWith.SetVerticalAlignCenter()

			date = data["date"]
			self.wndDate = ui.MakeTextLineNew(self, -10, 0, date[6:-3] if date != "Updating" else date)
			self.wndDate.SetWindowHorizontalAlignCenter()
			self.wndDate.SetHorizontalAlignCenter()
			self.wndDate.SetWindowVerticalAlignCenter()
			self.wndDate.SetVerticalAlignCenter()

			self.logID = data["logid"] # Log indexi buraya gelecek amk comarı

			self.btnExchange = ui.MakeButton(self, 376, 7, False, PATH_ROOT, "btn_info.png", "btn_info2.png", "btn_info3.png")
			self.btnExchange.SetEvent(lambda arg = self.logID : self.OnClickExchangeLog(arg))

			global wndInGameLog

		def __del__(self):
			ui.Window.__del__(self)

		def OnResetThings(self):
			self.IsSelected = False
			self.bIsBlocked = False
			self.xBase = 0
			self.yBase = 0

			self.clickEvent = None
			if self.background != None:
				self.background.Hide()
			self.background = None
			self.wndIcon = None
			self.tooltipItem = None
			self.Data = None

		def OnClickExchangeLog(self, logID):
			ingamelog.SendTradeDetails(logID)
			wndInGameLog.Show()

		def SetParent(self, parent):
			ui.Window.SetParent(self, parent)
			self.parent = proxy(parent)

		def SetBasePosition(self, x, y):
			self.xBase = x
			self.yBase = y
			
		def GetBasePosition(self):
			return (self.xBase, self.yBase)
			
		def SetClickEvent(self, event):
			self.clickEvent = event
		
		def Destroy(self):
			self.OnResetThings()
		
		def OnRender(self):
			xList, yList = self.parent.GetGlobalPosition()
			widthList, heightList = self.parent.GetWidth(), self.parent.GetHeight()	
			self.background.SetClipRect(xList, yList, xList + widthList, yList + heightList)

			textList = [self.wndTradedWith, self.wndDate]
			for text in textList:
				if text:
					text.SetClippingMaskWindow(self.parent)
					# xText, yText = text.GetGlobalPosition()

					# if yText < yList or yText + text.GetTextSize()[1] > yList + heightList:
					# 	text.Hide()
					# else:
					# 	text.Show()

				xBtn, yBtn = self.btnExchange.GetGlobalPosition()

				if yBtn < yList or yBtn + text.GetTextSize()[1] > yList + heightList:
					self.btnExchange.Hide()
				else:
					self.btnExchange.Show()

class ExchangeWindow(ui.BorderA):
	def __init__(self):
		ui.BorderA.__init__(self)
		self.ResetDatas()

		self.SetSize(442, 195)
		self.tooltipItem = uiToolTip.ItemToolTip()
		self.LoadWindow()

		# self.AddFlag("movable")

	def __del__(self):
		ui.BorderA.__del__(self)

	def LoadWindow(self):
		self.CreateGridExchange()

		self.MoneyInformations()
		self.CreatePlayerInformations(localeInfo.INGAME_TRADELOG_YOU, localeInfo.INGAME_TRADELOG_PLAYERNAME)

		# if app.ENABLE_WINDOW_ANIMATION:
		# 	self.SetWindowAnimation()

	def CreateGridExchange(self):
		self.itemSlots = []
		for i in xrange(2):
			self.itemSlots.append(i)
			self.itemSlots[i] = ui.GridSlotWindow()
			self.itemSlots[i].SetParent(self)
			self.itemSlots[i].SetPosition(15 + i * 225, 30)
			self.itemSlots[i].ArrangeSlot(0, 6, 4, 32, 32, 0, 0)
			self.itemSlots[i].SetSlotBaseImage("d:/ymir work/ui/public/slot_base.sub", 1.0, 1.0, 1.0, 1.0)
			self.itemSlots[i].SetGridSpecial(6, 4)
			self.itemSlots[i].SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			self.itemSlots[i].Show()

		self.itemSlots[0].SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.itemSlots[1].SetOverInItemEvent(ui.__mem_func__(self.OverInItem2))
		

	def MoneyInformations(self, ownerMoney = 0, targetMoney = 0):
		self.ownerMoneyBG = ui.MakeImageBox(self, "offlineshop/lightwork/searchshop/searchbg.png", 42, 168)
		self.targetMoneyBG = ui.MakeImageBox(self, "offlineshop/lightwork/searchshop/searchbg.png", 278, 168)
		self.moneyAmount =  [ownerMoney, targetMoney]

		self.moneyTexts = []
		for j in xrange(2):
			self.moneyTexts.append(j)
			self.moneyTexts[j] = ui.TextLine()
			self.moneyTexts[j].SetParent(self.ownerMoneyBG if j == 0 else self.targetMoneyBG)
			self.moneyTexts[j].SetPosition(0, 0)
			self.moneyTexts[j].SetHorizontalAlignCenter()
			self.moneyTexts[j].SetVerticalAlignCenter()
			self.moneyTexts[j].SetWindowHorizontalAlignCenter()
			self.moneyTexts[j].SetWindowVerticalAlignCenter()
			self.moneyTexts[j].SetText(localeInfo.NumberToDecimalString(str(self.moneyAmount[j])))
			self.moneyTexts[j].Show()

	def CreatePlayerInformations(self, ownerName, targetName):
		self.ownerName = ownerName
		self.targetName = targetName

		self.ownerNameText = ui.TextLine()
		self.ownerNameText.SetParent(self)
		self.ownerNameText.SetPosition(95, 13)
		self.ownerNameText.SetHorizontalAlignCenter()
		self.ownerNameText.SetVerticalAlignCenter()
		self.ownerNameText.SetText(self.ownerName)
		self.ownerNameText.Show()

		self.targetNameText = ui.TextLine()
		self.targetNameText.SetParent(self)
		self.targetNameText.SetPosition(330, 13)
		self.targetNameText.SetHorizontalAlignCenter()
		self.targetNameText.SetVerticalAlignCenter()
		self.targetNameText.SetText(self.targetName)
		self.targetNameText.Show()

	def OverInItem(self, slotIndex):
		if self.tooltipItem:
			self.tooltipItem.SetItemToolTip(slotIndex)

		self.tooltipItem.ClearToolTip()

		for i in xrange(len(self.itemInfo)):
			if self.itemInfo[i]["isOwner"] == TRUE and self.itemInfo[i]["pos"] == slotIndex:
				attr = {}
				for z in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
					attr[z] = {}
					attr[z][0] = self.itemInfo[i]["attr"][z]["type"]
					attr[z][1] = self.itemInfo[i]["attr"][z]["value"]
				socket = {}
				for y in xrange(player.METIN_SOCKET_MAX_NUM):
					socket[y] = self.itemInfo[i]["socket"][y]

				self.tooltipItem.AddItemData(self.itemInfo[i]["vnum"], socket, attr)

	def OverInItem2(self, slotIndex):
		if self.tooltipItem:
			self.tooltipItem.SetItemToolTip(slotIndex)

		self.tooltipItem.ClearToolTip()
		for i in xrange(len(self.itemInfo)):
			if self.itemInfo[i]["isOwner"] == FALSE and self.itemInfo[i]["pos"] == slotIndex:
				attr = {}
				for z in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
					attr[z] = {}
					attr[z][0] = self.itemInfo[i]["attr"][z]["type"]
					attr[z][1] = self.itemInfo[i]["attr"][z]["value"]

				socket = {}
				for y in xrange(player.METIN_SOCKET_MAX_NUM):
					socket[y] = self.itemInfo[i]["socket"][y]
				self.tooltipItem.AddItemData(self.itemInfo[i]["vnum"], socket, attr)

	def OverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def Open(self):
		self.Show()

	def Close(self):
		self.Hide()

		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def Destroy(self):
		self.ResetDatas()

	def ResetDatas(self):
		self.tooltipItem = None

		self.itemSlots = []
		self.moneyTexts = []
		self.moneyAmount = []
		self.itemInfo = []

	def UpdateTradeInfo(self,logID):
		self.itemInfo = []

		owner,victim = ingamelog.GetTradeDetailsGold(logID)
		self.moneyTexts[0].SetText(localeInfo.NumberToDecimalString(str(owner)))
		self.moneyTexts[1].SetText(localeInfo.NumberToDecimalString(str(victim)))
		self.targetNameText.SetText(str(ingamelog.GetTradeDetailsVictimName(logID)))

		details_item_count = ingamelog.TradeDetailsItemCount(logID)
		for y in xrange(24):
			self.itemSlots[0].SetItemSlot(y, 0, 0) # Itemi gride koymak icin # 1 grid index, item vnum, item count
			self.itemSlots[1].SetItemSlot(y, 0, 0) # Itemi gride koymak icin # 1 grid index, item vnum, item count
		if details_item_count == 0:
			return

		for i in xrange(details_item_count):
			self.itemInfo.append({})
			self.itemInfo[i]["vnum"] = int(ingamelog.TradeDetailsGetVnum(logID,i))
			self.itemInfo[i]["count"] = int(ingamelog.TradeDetailsGetCount(logID,i))
			self.itemInfo[i]["pos"] = int(ingamelog.TradeDetailsGetPos(logID,i))
			self.itemInfo[i]["isOwner"] = int(ingamelog.TradeDetailsIsOwner(logID,i))

			self.itemInfo[i]["attr"] = {}
			for z in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				self.itemInfo[i]["attr"][z] = {}
				self.itemInfo[i]["attr"][z]["type"] = int(ingamelog.TradeDetailsGetAttrTpye(logID,i,z))
				self.itemInfo[i]["attr"][z]["value"] = int(ingamelog.TradeDetailsGetAttrValue(logID,i,z))

			self.itemInfo[i]["socket"] = {}
			for x in xrange(player.METIN_SOCKET_MAX_NUM):
				self.itemInfo[i]["socket"][x] = int(ingamelog.TradeDetailsGetSocket(logID,i,x))

		for j in xrange(details_item_count):
			if self.itemInfo[j]["isOwner"] == TRUE:
				self.itemSlots[0].SetItemSlot(self.itemInfo[j]["pos"], self.itemInfo[j]["vnum"], self.itemInfo[j]["count"])
			else:
				self.itemSlots[1].SetItemSlot(self.itemInfo[j]["pos"], self.itemInfo[j]["vnum"], self.itemInfo[j]["count"])