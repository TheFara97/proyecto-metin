import item
import itemshop
import localeInfo
import net
import player
import ui
import uiToolTip
import uiCommon
import grp
import app
import dbg
import wndMgr
import uitooltip
import os
import chat
import constInfo
import emoji

try:
	import pickle
except ImportError:
	import cPickle as pickle

PATH = "d:/ymir work/ui/itemshop/"

LEFT = 0
RIGHT = 1

wheel_name = {
	0 : localeInfo.WHEEL_OF_FORTUNE_CAT_1,
	1 : localeInfo.WHEEL_OF_FORTUNE_CAT_2,
}

PREMIUM_DATA_DICT = {
	0 : (localeInfo.ITEMSHOP_PREMIUM_EXP, "d:/ymir work/ui/skill/common/affect/exp_bonus.sub",),
	1 : (localeInfo.ITEMSHOP_PREMIUM_ITEM, "d:/ymir work/ui/skill/common/affect/item_bonus.sub",),
	2 : (localeInfo.ITEMSHOP_PREMIUM_SAFEBOX, "d:/ymir work/ui/skill/common/affect/safebox.sub",),
	3 : (localeInfo.ITEMSHOP_PREMIUM_AUTOLOOT, "d:/ymir work/ui/skill/common/affect/autoloot.sub",),
	4 : (localeInfo.ITEMSHOP_PREMIUM_FISH_MIND, "d:/ymir work/ui/skill/common/affect/fishmind.sub",),
	5 : (localeInfo.ITEMSHOP_PREMIUM_MARRIAGE_FAST, "d:/ymir work/ui/skill/common/affect/marriage_fast.sub",),
	6 : (localeInfo.ITEMSHOP_PREMIUM_GOLD, "d:/ymir work/ui/skill/common/affect/gold_bonus.sub",),
}

POPUP_TEXT_DICT = {
	itemshop.ITEMSHOP_POPUP_ITEM_PROMO : localeInfo.ITEMSHOP_POPUP_ITEM_PROMO,
	itemshop.ITEMSHOP_POPUP_ITEM_LIMITED : localeInfo.ITEMSHOP_POPUP_ITEM_LIMITED,
	itemshop.ITEMSHOP_POPUP_NEW_ITEM : localeInfo.ITEMSHOP_POPUP_ITEM_NEW,
}

POPUP_TEXT_DICT_02 = {
	itemshop.ITEMSHOP_POPUP_ITEM_PROMO : localeInfo.ITEMSHOP_POPUP_ITEMS_PROMO,
	itemshop.ITEMSHOP_POPUP_ITEM_LIMITED : localeInfo.ITEMSHOP_POPUP_ITEMS_LIMITED,
	itemshop.ITEMSHOP_POPUP_NEW_ITEM : localeInfo.ITEMSHOP_POPUP_ITEMS_NEW,
}

class FILTER:
	DEFAULT = 0
	DISCOUNT = 1
	NEW = 2
	LIMITED = 3

def filterFunc(it, filter, text):
	if it["name"].startswith(text):
		if filter == FILTER.DEFAULT:
			return True
		elif filter == FILTER.DISCOUNT:
			if it["discount"] and it["discount_time"] >= app.GetGlobalTimeStamp():
				return True
		elif filter == FILTER.NEW:
			if it["add_time"] + 60 * 60 * 24 * 7 > app.GetGlobalTimeStamp():
				return True
		elif filter == FILTER.LIMITED:
			if it["limited_time"] >= app.GetGlobalTimeStamp():
				return True

	return False

class ItemShopItems:
	def __init__(self):
		self.__item = []
		self.__categoryDict = {}
		self.__dataTime = 0
		self.__LoadData()
		itemshop.SetWnd(self)

	def __del__(self):
		self.__SaveData()
		self.__item = []
		itemshop.SetWnd(None)

	def __LoadData(self):
		if os.path.exists("lib/itemshop_data.pcl"):
			file = open("lib/itemshop_data.pcl", "r")

			try:
				data = pickle.load(file)
				self.__item = data["item"]
				self.__categoryDict = data["category"]
				self.__dataTime = data["data_time"]
			except ValueError:
				pass

			file.close()

	def __SaveData(self):
		data = {
			"item": self.__item,
			"category": self.__categoryDict,
			"data_time": self.__dataTime,
		}

		file = open("lib/itemshop_data.pcl", "w")
		file.write(pickle.dumps(data))
		file.write("\n")
		file.close()

	def BINARY_ClearItem(self):
		self.__item = []
		self.__categoryDict = {}

	def BINARY_AddItem(self, data):
		item.SelectItem(data["vnum"])
		data["name"] = (item.GetItemName()).lower()

		add = True

		for it in self.__item:
			if it["id"] == data["id"]:
				index = self.__item.index(it)
				attr = it["attr"]
				self.__item[index]["attr"] = {
					it["bonus_type"]: attr,
					data["bonus_type"] : data["attr"]
				}
				add = False

		if add:
			self.__item.append(data)

	def BINARY_RemoveItem(self, index):
		for item in self.__item:
			if item["id"] == index:
				self.__item.remove(item)
				break

	def BINARY_UpdateItem(self, data):
		item.SelectItem(data["vnum"])
		data["name"] = (item.GetItemName()).lower()

		for it in self.__item:
			if it["id"] == data["id"]:
				index = self.__item.index(it)
				self.__item[index] = data
				break

	def BINARY_SetBuyStats(self, index, category, value):
		for item in self.__item:
			if item["id"] == index:
				item["buy"] = value

	def BINARY_RefreshLimitedCountItem(self, index, category, value):
		for it in self.__item:
			if it["id"] == index:
				i = self.__item.index(it)
				self.__item[i]["limited_buy_count"] = value
				break

	def BINARY_AppendCategory(self, index, name):
		self.__categoryDict[index] = name

	def GetItemDataByIndex(self, index):
		for item in self.__item:
			if item["id"] == index:
				return item

	def GetItemByCategory(self, category, filter, text):
		items = []
		item_list = list(self.__item)
		for it in item_list:
			if it["category"] == category:
				if filterFunc(it, filter, text):
					items.append(it["id"])

		return items

	def GetCategoryDict(self):
		return self.__categoryDict

	def SetDataTime(self, dataTime):
		self.__dataTime	= dataTime

	def GetDataTime(self):
		return self.__dataTime


class ItemShopWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.category = {}
		self.wndEditorWindow = None
		self.wndPopupDialog = None
		self.__isEditor = False
		self.editButton = None
		self.currentCategory = -1
		self.page = "ALL"
		self.__itemData = ItemShopItems()
		self.__wheelData = {}
		self.__wheelItemData = {}
		self.__wheelGroupButton = {}
		self.__wheelGroup = 0
		self.__wheelRefresh = False
		self.__itemToolTip = uiToolTip.ItemToolTip()
		self.__itemToolTip.HideToolTip()
		self.__itemDict = {}
		self.__scrollTo = 0
		self.__wheelAuto = False
		self.__InitWheel()
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __InitWheel(self):
		self.__isSpinning = False
		self.__lastSpin = 0.0
		self.__imageIndex = 1
		self.__spinSpeed = itemshop.TOMBOLA_SPIN_SPEED
		self.__toPos = 0
		self.__toSpin = 0
		self.__isBlinking = 0
		self.__lastBlink = 0
		self.__wheelTime = 0
		self.__blinkCount = itemshop.TOMBOLA_BLINK_COUNT

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/itemshopwindow.py")

		except:
			import exception
			exception.Abort("ItemShopWindow.__LoadScript.LoadObject")

		try:
			self.moneyValue = self.GetChild("money_value")

			mX, mY= self.moneyValue.GetLocalPosition()
			self.moneyValue.SetPosition(mX, mY)

			self.loadingImage = self.GetChild("item_loading")
			self.loadingImage.Hide()
			self.wheelStartButton = self.GetChild("wheel_start_button")
			self.wheelAnimationImage = self.GetChild("wheel_animation_image")
			self.wheelItemSlot = self.GetChild("wheel_item_slot")
			self.wheelPriceValue = self.GetChild("wheel_price_value")
			self.wheelAutoButton = self.GetChild("wheel_auto_button")

			self.noDiscountItemText = self.GetChild("no_discount_items_text")
			self.noDiscountItemText.Hide()

			self.exitbtn = self.GetChild("ExitButton")
			self.exitbtn.SetEvent(ui.__mem_func__(self.Close))

			self.wheelBtn = self.GetChild("wheel_button")
			self.wheelBtn.SetEvent(ui.__mem_func__(self.OpenKoloLosu))

			self.promoBtn = self.GetChild("VoucherButton")
			self.promoBtn.SetEvent(ui.__mem_func__(self.OpenVoucher))

			self.items_board = self.GetChild("items_board")

			self.promo_btn = self.GetChild("promo_btn")
			self.promo_btn.SetEvent(ui.__mem_func__(self.ClickAccept))
			self.codeframe = self.GetChild("CodeFrameSlotText")

			self.discount_button = self.GetChild("discount_button")
			self.discount_button.SetEvent(ui.__mem_func__(self.OpenDiscount))

			self.windowDict = {
				"ALL": self.GetChild("all_window"),
				"LIMITED": self.GetChild("all_window"),
				"WHEEL": self.GetChild("wheel_window"),
				"PROMO" : self.GetChild("promo_window"),
			}

			self.listBox = ui.NewListBox()
			self.listBox.SetParent(self.GetChild("items_board"))
			self.listBox.SetSize(400, 308)
			self.listBox.SetPosition(5, 9)
			self.listBox.itemStep = 0
			self.listBox.manualMode = True
			self.listBox.Show()

			self.scrollBar = ScrollBar()
			self.scrollBar.SetParent(self.windowDict["ALL"])
			self.scrollBar.SetScrollBarSize(325)
			self.scrollBar.SetPosition(615, 2)
			self.scrollBar.EnableSmoothMode()
			self.scrollBar.Hide()

			self.listBox.SetScrollBar(self.scrollBar)
			self.listBox.OnMouseWheel = self.scrollBar.OnMouseWheel

			self.listBoxCat = ui.NewListBox()
			self.listBoxCat.SetParent(self.GetChild("categories_board"))
			self.listBoxCat.SetSize(145, 260)
			self.listBoxCat.SetPosition(12, 43)
			self.listBoxCat.itemStep = 6
			self.listBoxCat.Show()

			self.scrollBarCat = ScrollBar()
			self.scrollBarCat.SetParent(self.windowDict["ALL"])
			self.scrollBarCat.SetScrollBarSize(325)
			self.scrollBarCat.SetPosition(185, 2)
			self.scrollBarCat.EnableSmoothMode()
			self.scrollBarCat.Show()

			self.listBoxCat.SetScrollBar(self.scrollBarCat)
			self.listBoxCat.OnMouseWheel = self.scrollBarCat.OnMouseWheel

			self.listBoxWheel = ui.NewListBox()
			self.listBoxWheel.SetParent(self.GetChild("wheel_item_list_bg"))
			self.listBoxWheel.SetSize(172, 9 * 32)
			self.listBoxWheel.SetPosition(11, 10)
			self.listBoxWheel.itemStep = 0
			self.listBoxWheel.Show()

			self.scrollBarWheel = ScrollBar()
			self.scrollBarWheel.SetParent(self.windowDict["WHEEL"])
			self.scrollBarWheel.SetScrollBarSize(290)
			self.scrollBarWheel.SetPosition(655, -10)
			self.scrollBarWheel.EnableSmoothMode()
			self.scrollBarWheel.Show()

			self.listBoxWheel.SetScrollBar(self.scrollBarWheel)
			self.listBoxWheel.OnMouseWheel = self.scrollBarWheel.OnMouseWheel

		except:
			import exception
			exception.Abort("ItemShopWindow.__LoadScript.BindObject")

		self.wheelStartButton.SetEvent(ui.__mem_func__(self.__OnClickWheelStartButton))
		self.wheelItemSlot.SetOverInItemEvent(ui.__mem_func__(self.__OnOverInWheelItem))
		self.wheelItemSlot.SetOverOutItemEvent(ui.__mem_func__(self.__OnOverOutWheelItem))
		self.wheelAutoButton.SetToggleUpEvent(ui.__mem_func__(self.__OnToggleUpWheelAutoButton))
		self.wheelAutoButton.SetToggleDownEvent(ui.__mem_func__(self.__OnToggleDownWheelAutoButton))

		for item in self.ElementDictionary.values():
			item.OnPressEscapeKey = ui.__mem_func__(self.Close)
			item.OnPressExitKey = ui.__mem_func__(self.Close)

		self.wndPopupDialog = PopupDialog()
		self.wndPopupDialog.Hide()

		self.__OnClickCategoryButton(0)
		self.__OnClickWheelGroupButton(1)
		self.SetCenterPosition()

		self.windowDict["WHEEL"].Hide()
		self.windowDict["PROMO"].Hide()
		self.windowDict["ALL"].Show()
		self.page = "ALL"

	def ClickAccept(self):
		promo_code = self.codeframe.GetText()
		self.codeframe.SetText("")

		net.SendChatPacket("/promo_code_system check_code %s" % promo_code)

	def OpenDiscount(self):
		self.page = "LIMITED"

		if self.windowDict["PROMO"].IsShow():
			self.windowDict["PROMO"].Hide()
		if not self.items_board.IsShow():
			self.items_board.Show()

		self.listBox.ClearItems()

		for index in xrange(14):
			for key, button in self.category.items():
				button.SetUp()
				if key == index:
					button.Down()

			self.listBoxCat.UpdateList()
			filter = FILTER.LIMITED

			text = ""
			itemPos = self.__itemData.GetItemByCategory(index, filter, text.lower())

			for pos in itemPos:
				data = self.__itemData.GetItemDataByIndex(pos)
				if data:
					try:
						item = self.__itemDict[index][pos]
					except KeyError:
						item = ItemShopItem(data)
						if not self.__itemDict.get(index, None):
							self.__itemDict[index] = {}
						self.__itemDict[index][pos] = item
					item.SetData(data)
					item.RefreshValue()
					item.Show()
					self.listBox.AppendItem(item)

		self.listBox.UpdateList()

		if self.listBox.IsEmpty():
			self.noDiscountItemText.Show()

		self.discount_button.Down()

	def OpenKoloLosu(self):
		self.discount_button.SetUp()
		if self.page == "ALL" or self.page == "LIMITED" or self.page == "PROMO":
			self.page = "WHEEL"
			self.windowDict["WHEEL"].Show()
			self.windowDict["PROMO"].Hide()
			self.windowDict["ALL"].Hide()
			self.__OnClickWheelGroupButton(self.__wheelGroup)
		elif self.page == "WHEEL":
			self.page = "ALL"
			self.windowDict["WHEEL"].Hide()
			self.windowDict["PROMO"].Hide()
			self.windowDict["ALL"].Show()
			if not self.items_board.IsShow():
				self.items_board.Show()
			if not self.scrollBar.IsShow():
				self.scrollBar.Show()
			self.__OnClickCategoryButton(0)

	def OpenVoucher(self):
		for index in xrange(14):
			for key, button in self.category.items():
				button.SetUp()
				if key == index:
					button.Down()

		self.wheelBtn.SetUp()
		self.discount_button.SetUp()
		if self.page == "ALL" or self.page == "LIMITED" or self.page == "WHEEL":
			self.page = "PROMO"
			self.windowDict["WHEEL"].Hide()
			self.windowDict["ALL"].Show()
			self.windowDict["PROMO"].Show()
			self.items_board.Hide()
			self.scrollBar.Hide()
		elif self.page == "PROMO":
			self.page = "ALL"
			self.windowDict["PROMO"].Hide()
			self.windowDict["ALL"].Show()
			if not self.scrollBar.IsShow():
				self.scrollBar.Show()
			if not self.items_board.IsShow():
				self.items_board.Show()
			self.__OnClickCategoryButton(0)

	def Open(self,dataTime):
		self.Show()
		self.__OnClickCategoryButton(0)
		self.page = "ALL"
		self.windowDict["WHEEL"].Hide()
		self.windowDict["PROMO"].Hide()
		self.windowDict["ALL"].Show()
		if not self.items_board.IsShow():
			self.items_board.Show()
		if not self.scrollBar.IsShow():
			self.scrollBar.Show()
		self.__itemData.SetDataTime(dataTime)
		self.loadingImage.Hide()

		categoryDict = self.__itemData.GetCategoryDict()

		if len(self.category) != len(categoryDict):
			for key, value in categoryDict.items():
				button = ItemShopCategoryButton()

				button.SetText(localeInfo.GetLocaleString(value), -4)

				button.SetEvent(ui.__mem_func__(self.__OnClickCategoryButton), key)
				self.category[key] = button
				self.listBoxCat.AppendItem(button)

			self.listBoxCat.UpdateList()

		if self.__scrollTo:
			self.__OnClickPopupDialog(*self.__scrollTo)

	def Close(self):
		self.__OnClickCategoryButton(0)
		self.Hide()
		return True

	def __OnClickCategoryButton(self, index):
		if not self.page == "ALL":
			self.page = "ALL"
		if self.windowDict["PROMO"].IsShow():
			self.windowDict["PROMO"].Hide()
		if not self.items_board.IsShow():
			self.items_board.Show()
		if not self.scrollBar.IsShow():
			self.scrollBar.Show()
			
		self.discount_button.SetUp()
		self.loadingImage.Show()
		self.listBox.ClearItems()
		self.noDiscountItemText.Hide()

		self.currentCategory = index

		for key, button in self.category.items():
			button.SetUp()
			if key == index:
				button.Down()

		self.listBoxCat.UpdateList()

		filter = FILTER.DEFAULT

		text = ""
		itemPos = self.__itemData.GetItemByCategory(index, filter, text.lower())

		for pos in itemPos:
			data = self.__itemData.GetItemDataByIndex(pos)
			if data:
				try:
					item = self.__itemDict[index][pos]
				except KeyError:
					item = ItemShopItem(data)
					if not self.__itemDict.get(index, None):
						self.__itemDict[index] = {}
					self.__itemDict[index][pos] = item
				item.SetData(data)
				item.RefreshValue()
				item.Show()
				self.listBox.AppendItem(item)
		self.listBox.UpdateList()


		self.scrollBar.Show()

		self.loadingImage.Hide()
		self.__scrollTo = 0

	def __OnClickWheelGroupButton(self, group):
		self.__wheelGroup = group

		self.loadingImage.Show()
		self.listBoxWheel.ClearItems()

		for key, button in self.__wheelGroupButton.items():
			button.SetUp()
			if key == group:
				button.Down()

		data = self.__wheelData.get(group)
		if not data:
			self.loadingImage.Hide()
			return

		for it in data["item"]:
			self.listBoxWheel.AppendItem(
				WheelItem(it["vnum"], it["count"], 100.0 * float(it["chance"]) / float(data["chance"]))
			)

		price = data["price"]
		text = ""
		if price["price_type"] == 0:
			text = localeInfo.NumberToMoneyString(price["price"], localeInfo.MONETARY_UNIT0)
		elif price["price_type"] == 1:
			text = localeInfo.NumberToMoneyString(price["price"], localeInfo.ITEMSHOP_CASH_UNIT)
		else:
			item.SelectItem(price["price_type"])
			text = str(price["price"]) + "x " + item.GetItemName()

		self.UpdateCoins()

		self.wheelPriceValue.SetText(localeInfo.ITEMSHOP_WHEEL_COST + text)
		self.loadingImage.Hide()

	def __OnClickWheelStartButton(self):
		net.SendTombolaStartPacket(self.__wheelGroup)

	def __OnOverInWheelItem(self, slotIndex):
		if self.__itemToolTip:
			self.__itemToolTip.SetItemToolTip(self.__wheelItemData[slotIndex]["vnum"])

	def __OnOverOutWheelItem(self):
		if self.__itemToolTip:
			self.__itemToolTip.HideToolTip()

	def __OnToggleUpWheelAutoButton(self):
		self.__wheelAuto = False

	def __OnToggleDownWheelAutoButton(self):
		self.__wheelAuto = True

	def ShowPopup(self, type, id, category):
		data = self.__itemData.GetItemDataByIndex(id)
		if data:
			item.SelectItem(data["vnum"])
			text = POPUP_TEXT_DICT.get(type, localeInfo.ITEMSHOP_POPUP_ITEM_NEW) % item.GetItemName()
		else:
			text = POPUP_TEXT_DICT_02.get(type, localeInfo.ITEMSHOP_POPUP_ITEMS_NEW)

		self.wndPopupDialog.Open(text, ui.__mem_func__(self.__OnClickPopupDialog), id, category)

	def __OnClickPopupDialog(self, id, category):
		data = self.__itemData.GetItemDataByIndex(id)

		if data:
			self.Open(self.__itemData.GetDataTime())

			self.__OnClickCategoryButton(data["category"])

			scroll = True

			item = None
			for it in self.__itemDict[data["category"]].values():
				if it.GetData()["id"] == id:
					item = it
					break

			while scroll:
				self.scrollBar.OnDown()

				(gX, gY) = item.GetLocalPosition()

				if gY + item.oHeight <= self.listBox.GetHeight():
					scroll = False
		elif category:
			self.Open(self.__itemData.GetDataTime())

			self.__OnClickCategoryButton(category)
		else:
			self.__scrollTo = (id, category)
			self.RequestOpen()

	def SetEditorFlag(self, flag):
		self.__isEditor = flag

		if flag:
			self.editButton = ui.Button()
			self.editButton.SetParent(self.GetChild("Board"))
			self.editButton.SetUpVisual("d:/ymir work/ui/btn_option01_extended.tga")
			self.editButton.SetOverVisual("d:/ymir work/ui/btn_option02_extended.tga")
			self.editButton.SetDownVisual("d:/ymir work/ui/btn_option03_extended.tga")
			self.editButton.SetPosition(668, 50)
			self.editButton.SetEvent(ui.__mem_func__(self.OpenEditorWindow))
			self.editButton.Show()

	def GetEditorFlag(self):
		return self.__isEditor

	def OpenEditorWindow(self):
		if not self.GetEditorFlag():
			return

		if not self.wndEditorWindow:
			self.wndEditorWindow = ItemShopEditorWindow()
			self.wndEditorWindow.SetData(self.__itemData)
			self.wndEditorWindow.Hide()

		self.wndEditorWindow.OpenWindow()

	def ClearItems(self):
		self.listBox.ClearItems()

	def Destroy(self):
		self.__itemData = None

		self.ClearDictionary()

		if self.wndEditorWindow:
			self.wndEditorWindow.Destroy()
			del self.wndEditorWindow

		listboxList = [self.listBox, self.listBoxCat, self.listBoxWheel]

		for listBox in listboxList:
			for item in listBox.items:
				item.ClearDictionary()
				item = 0

			listBox.ClearItems()

		self.moneyValue = 0

		self.loadingImage = 0
		self.wheelStartButton = 0
		self.wheelAnimationImage = 0
		self.wheelItemSlot = 0
		self.wheelPriceValue = 0
		
		self.noDiscountItemText = 0
		self.windowDict = {}

		self.listBox = 0
		self.scrollBar = 0
		self.listBoxCat = 0
		self.scrollBarCat = 0
		self.listBoxWheel = 0
		self.scrollBarWheel = 0

		self.category = {}

		self.__itemToolTip = 0

		if self.wndPopupDialog:
			self.wndPopupDialog.Destroy()
			del self.wndPopupDialog

		self.Hide()

	def OnPressExitKey(self):
		self.Hide()
		return True

	def OnPressEscapeKey(self):
		self.Hide()
		return True

	def RequestOpen(self):
		if self.IsShow():
			self.Hide()
			return
		self.Show()
		self.loadingImage.Show()
		net.SendItemShopOpenPacket(self.__itemData.GetDataTime())
		net.SendTombolaOpenPacket()

	def OpenWindow(self):
		if self.IsShow():
			self.Hide()
		else:
			self.Show()

	def Show(self):
		ui.ScriptWindow.Show(self)
		if self.__wheelRefresh:
			self.__OnClickWheelGroupButton(self.__wheelGroup)
			self.__wheelRefresh = False

	def UpdateCoins(self):
		currency = localeInfo.ITEMSHOP_CASH_UNIT
		self.moneyValue.SetText(str(localeInfo.NumberToGoldNotText(itemshop.GetCoins())+" "+str(currency)))

		data = self.__wheelData.get(self.__wheelGroup)
		if not data:
			return

		price = data["price"]

		text = ""
		if price["price_type"] == 0:
			text = localeInfo.NumberToMoneyString(player.GetElk(), "")
		elif price["price_type"] == 1:
			text = localeInfo.NumberToMoneyString(itemshop.GetCoins(), "")
		else:
			text = localeInfo.NumberToMoneyString(player.GetItemCountByVnum(price["price_type"]), "")

	def RefreshPage(self):
		scroll_pos = self.scrollBar.GetPos()
		self.__OnClickCategoryButton(self.currentCategory)
		self.scrollBar.SetPos(scroll_pos)
		if self.wndEditorWindow:
			if self.wndEditorWindow.IsShow():
				self.wndEditorWindow.RefreshList()

	def SetSpinningItem(self, index, vnum, count):
		self.wheelItemSlot.SetItemSlot(index, vnum, count)
		self.__wheelItemData[index] = {"vnum": vnum, "count": count}

	def SetPrice(self, group, price, price_type):
		if not self.__wheelData.get(group):
			self.__wheelData[group] = {}

		self.__wheelData[group]["price"] = {"price": price, "price_type": price_type}

	def SetItem(self, group, vnum, count, chance):
		if not self.__wheelData.get(group):
			self.__wheelData[group] = {}
		if not self.__wheelData[group].get("item"):
			self.__wheelData[group]["item"] = []

		if not self.__wheelGroupButton.get(group):
			index = len(self.__wheelGroupButton)
			button = ui.RadioButton()
			button.SetParent(self.windowDict["WHEEL"])
			button.SetPosition(76, 1 + index * 28)
			button.SetUpVisual(PATH + "category_btn.png")
			button.SetOverVisual(PATH + "category_btn1.png")
			button.SetDownVisual(PATH + "category_btn2.png")
			button.SetText("")
			button.ButtonText.SetOutline()
			#button.ButtonText.SetFontName("Tahoma:12")
			button.SetText(str(wheel_name[index]))
			button.SetEvent(ui.__mem_func__(self.__OnClickWheelGroupButton), group)
			button.Show()

			self.__wheelGroupButton[group] = button

		self.__wheelData[group]["item"].append({"vnum": vnum, "count": count, "chance": chance})
		if not self.__wheelData[group].get("chance"):
			self.__wheelData[group]["chance"] = 0
		self.__wheelData[group]["chance"] += chance

	def TombolaClear(self):
		self.__InitWheel()
		self.__wheelRefresh = True
		self.__wheelData = {}
		self.__wheelItemData = {}
		self.__wheelGroupButton = {}

		for item in self.listBox.items:
			item.ClearDictionary()
			item = 0

		self.listBox.ClearItems()

		self.__itemDict = {}
		self.listBoxCat.ClearItems()
		self.category = {}

	def StartSpinning(self, destPos, to_pos, to_spin, time):
		self.__InitWheel()
		self.__isSpinning = True
		self.__toPos = to_pos
		self.__toSpin = to_spin
		self.__wheelTime = app.GetTime() + time
		self.wheelStartButton.Disable()

	def OnUpdate(self):
		if self.__isSpinning:
			if (app.GetTime() - self.__lastSpin) >= self.__spinSpeed:
				self.__toPos -= 1

				if self.__toPos <= self.__toSpin:
					self.__spinSpeed += itemshop.TOMBOLA_SPIN_SPEED

				self.__imageIndex = 1 if self.__imageIndex == 16 else self.__imageIndex + 1

				if not self.wheelAnimationImage.IsShow():
					self.wheelAnimationImage.Show()

				self.wheelAnimationImage.LoadImage(PATH + "wheel/" + str(self.__imageIndex) + ".tga")

				self.__lastSpin = app.GetTime()

				if self.__toPos == 0 or self.__lastSpin > self.__wheelTime:
					self.__isSpinning = False
					self.__isBlinking = True

		if self.__isBlinking and (app.GetTime() - self.__lastBlink) >= 0.3:
			if self.__blinkCount > 0:
				if self.__blinkCount % 2:
					self.wheelAnimationImage.Hide()
				else:
					self.wheelAnimationImage.Show()
				self.__blinkCount -= 1
			else:
				self.__isBlinking = False
				self.__blinkCount = itemshop.TOMBOLA_BLINK_COUNT
				self.__lastBlink = 0
				self.wheelAnimationImage.Hide()
				self.__imageIndex = 1
				self.wheelAnimationImage.LoadImage(PATH + "wheel/" + str(self.__imageIndex) + ".tga")
				for i in xrange(16):
					self.wheelItemSlot.ClearSlot(i)
				self.wheelStartButton.Enable()

				if self.__wheelAuto:
					net.SendTombolaStartPacket(self.__wheelGroup)

			self.__lastBlink = app.GetTime()


class ItemShopCategoryButton(ui.NewListBoxItem):
	def __init__(self):
		ui.NewListBoxItem.__init__(self)
		self.__LoadWindow()
		self.OnRender = None

	def __LoadWindow(self):
		self.button = ui.ExpandedButton()
		self.button.SetParent(self)
		self.button.SetPosition(0, 0)
		self.button.SetUpVisual(PATH + "category_btn.png")
		self.button.SetOverVisual(PATH + "category_btn1.png")
		self.button.SetDownVisual(PATH + "category_btn2.png")
		self.button.SetText("")
		self.button.ButtonText.SetOutline()
		
		# Fix for ENABLE_DYNAMIC_FONTS
		if app.ENABLE_DYNAMIC_FONTS:
			self.button.ButtonText.SetDynamicFont(True)
			
		self.button.Show()

		self.RegisterComponent(self.button)
		self.RegisterComponent(self.button.ButtonText)

		self.SetSize(self.button.GetWidth(), self.button.GetHeight())

	def ClearDictionary(self):
		self.UnregisterComponent(self.button.ButtonText)
		self.UnregisterComponent(self.button)

		self.button.ButtonText = 0
		self.button = 0

		self.Hide()

	def SetEvent(self, event, *arg):
		self.button.SetEvent(event, *arg)

	def SetText(self, text, width=4):
		self.button.SetText(text, width)

	def SetUp(self):
		self.button.SetUp()

	def Down(self):
		self.button.Down()

	def __del__(self):
		ui.NewListBoxItem.__del__(self)


class ItemShopItem(ui.NewListBoxItem):
	def MakeImageBox(self, x, y, parent, image):
		wnd = ui.ExpandedImageBox()
		wnd.SetParent(parent)
		wnd.SetPosition(x, y)
		if image:
			wnd.LoadImage(image)
		wnd.Show()

		self.RegisterComponent(wnd)

		return wnd

	def MakeTextLine(self, x, y, parent, text):
	#def MakeTextLine(self, x, y, parent, text, fontname):
		wnd = ui.TextLine()
		wnd.SetParent(parent)
		wnd.SetPosition(x, y)
		#wnd.SetFontName(fontname)
		wnd.SetText(text)
		wnd.Show()
		
		self.RegisterComponent(wnd)

		return wnd

	def MakeButton(self, x, y, parent, upVisual, overVisual, downVisual):
		wnd = ui.Button()
		wnd.SetParent(parent)
		wnd.SetPosition(x, y)
		wnd.SetUpVisual(upVisual)
		wnd.SetOverVisual(overVisual)
		wnd.SetDownVisual(downVisual)
		wnd.Show()

		self.RegisterComponent(wnd)

		return wnd

	def __init__(self, data, size="big"):
		ui.NewListBoxItem.__init__(self)
		self.itemBuyQuestionDialog = None
		self.itemSelectAttributeDialog = None
		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.Hide()
		self.text_lines = []
		self.__updateEvent = None
		self.__nextUpdate = 0
		self.__timeUpdate = 0
		self.__addCount = 0
		self.oHeight = 0
		self.__textHasOutLine = False
		self.__data = data
		self.__LoadWindow(size)

		self.OnRender = None

	def SetData(self, data):
		self.__data = data

	def GetData(self):
		return self.__data

	def __del__(self):
		ui.NewListBoxItem.__del__(self)

	def Destroy(self):
		pass

	def CreateDescription(self, text):
		lines = uitooltip.SplitDescription(text, 40)
		if not lines:
			return

		for line in lines:
			self.AppendTextLine(line)

	def AppendTextLine(self, text):
		self.descText = ui.TextLine()
		self.descText.SetParent(self)
		self.descText.SetPackedFontColor(0xfff0e2ec)
		self.descText.SetText(text)
		self.descText.SetOutline()
		self.descText.SetHorizontalAlignCenter()
		self.descText.SetFeather(FALSE)
		self.descText.Show()

		self.descText.SetPosition(200, 35 + 14 * len(self.text_lines))

		self.text_lines.append(self.descText)
		self.RegisterComponent(self.descText)

		return self.descText

	def __LoadWindow(self, size):
		if size == "small":
			xAdd = 20
			xBButton = 330
			xDSlot = 228
			slotImageName = PATH+"item_bg.png"
		else:
			xAdd = 20
			xBButton = 330
			xDSlot = 228
			slotImageName = PATH+"item_bg.png"

		self.slotBar = self.MakeImageBox(0, 0, self, slotImageName)

		#self.nameText = self.MakeTextLine(180 + xAdd, 10, self.slotBar, "", "Tahoma:14b")
		self.nameText = self.MakeTextLine(180 + xAdd, 10, self.slotBar, "")
		self.nameText.SetPackedFontColor(0xfff0e2ec)
		self.nameText.SetHorizontalAlignCenter()
		self.nameText.SetOutline()

		self.priceslot = self.MakeImageBox(xBButton, 23, self.slotBar, PATH+"price_slot.png")

		#self.priceText = self.MakeTextLine(26,11, self.priceslot, "", "Tahoma:12")
		self.priceText = self.MakeTextLine(26,11, self.priceslot, "")
		self.priceText.SetHorizontalAlignCenter()
		self.priceText.SetVerticalAlignCenter()
		self.priceText.SetOutline()

		self.itemImage = self.MakeImageBox(25, 10, self.slotBar, "")
		self.itemImage.OnMouseOverIn = ui.__mem_func__(self.__OverInItem)
		self.itemImage.OnMouseOverOut = ui.__mem_func__(self.__OverOutItem)


		self.buyButton = self.MakeButton(xBButton, 78, self.slotBar,
									PATH + "buy_btn.png",
									PATH + "buy_btn1.png",
									PATH + "buy_btn2.png")
		self.buyButton.SetEvent(ui.__mem_func__(self.__OnClickBuyButton))

		self.countSlot = self.MakeImageBox(xBButton+18, 54, self.slotBar, PATH+"count_slot.png")
		#self.countText = self.MakeTextLine(8,7, self.countSlot, "1", "Tahoma:12")
		self.countText = self.MakeTextLine(8,7, self.countSlot, "1")
		self.countText.SetHorizontalAlignCenter()
		self.countText.SetVerticalAlignCenter()
		self.countText.SetOutline()
		self.minusButton = self.MakeButton(xBButton-3, 54, self.slotBar,
									  PATH + "minus_btn.png",
									  PATH + "minus_btn1.png",
									  PATH + "minus_btn2.png")
		self.minusButton.SetEvent(ui.__mem_func__(self.setMinusEvent))

		self.plusButton = self.MakeButton(xBButton+39, 54, self.slotBar,
									 PATH + "plus_btn.png",
									 PATH + "plus_btn1.png",
									 PATH + "plus_btn2.png")
		self.plusButton.SetEvent(ui.__mem_func__(self.setPlusEvent))

		self.discountSlot = self.MakeImageBox(xDSlot+137, 0, self.slotBar, PATH + "discount.png")

		self.rectangle = self.MakeImageBox(xAdd+65, 87, self.slotBar, PATH + "rectangle.png")
		self.rectangle.Hide()

		#self.discountTimeValue = self.MakeTextLine(180 + xAdd, 90, self.slotBar, "", "Tahoma:14")
		self.discountTimeValue = self.MakeTextLine(180 + xAdd, 90, self.slotBar, "")
		self.discountTimeValue.SetHorizontalAlignCenter()
		self.discountTimeValue.SetOutline()


		#self.limitedTimeValue = self.MakeTextLine(180 + xAdd, 90, self.slotBar, "", "Tahoma:14")
		self.limitedTimeValue = self.MakeTextLine(180 + xAdd, 90, self.slotBar, "")
		self.limitedTimeValue.SetHorizontalAlignCenter()
		self.limitedTimeValue.SetOutline()

		self.SetSize(self.slotBar.GetWidth() + 6, self.slotBar.GetHeight())
		self.oHeight = self.GetHeight()

		self.__SetItem(self.__data["vnum"], self.__data["count"], self.__data["is_vip"])

	def ClearDictionary(self):
		elements = [self.slotBar, self.nameText, self.descText, self.text_lines, self.priceText, self.itemImage, self.buyButton, self.countText,
					self.minusButton, self.plusButton, self.discountSlot, self.discountTimeValue, self.rectangle,
					self.limitedTimeValue]

		for element in elements:
			self.UnregisterComponent(element)
			element = 0

		if self.itemBuyQuestionDialog:
			self.itemBuyQuestionDialog.Destroy()
			del self.itemBuyQuestionDialog

		if self.itemSelectAttributeDialog:
			self.itemSelectAttributeDialog.Close()

		self.tooltipItem = 0

	def __SetItem(self, vnum, count, is_vip):
		if is_vip:
			item_data = PREMIUM_DATA_DICT.get(vnum)
			item.SelectItem(vnum)
			if item_data:
				self.nameText.SetText(item_data[0])

				if item.GetItemDescription() == "":
					self.AppendTextLine(self.CreateDescription(localeInfo.ITEMSHOP_MISSING_DESCRIPTION))
				else:
					self.AppendTextLine(self.CreateDescription(item.GetItemDescription()))

				iX, iY = self.itemImage.GetLocalPosition()
				self.itemImage.SetPosition(iX, iY + 32)

				self.itemImage.LoadImage(item_data[1])

			else:
				dbg.TraceError("Cannot find data for premium %d" % vnum)
		else:
			item.SelectItem(vnum)
			if count > 1:
				self.nameText.SetText("%d x %s" % (count, item.GetItemName()))
			else:
				self.nameText.SetText(item.GetItemName())

			_, height = item.GetItemSize()

			if item.GetItemDescription() == "":
				self.AppendTextLine(self.CreateDescription(localeInfo.ITEMSHOP_MISSING_DESCRIPTION))
			else:
				self.AppendTextLine(self.CreateDescription(item.GetItemDescription()))

			iX, iY = self.itemImage.GetLocalPosition()
			self.itemImage.SetPosition(iX, iY - 16 * (height - 3))

			self.itemImage.LoadImage(item.GetIconImageFileName())

	def minusEvent(self):
		value = int(self.countText.GetText())
		add = 1
		value -= add

		if value < 1:
			return
		if value > 20:
			return

		self.countText.SetText(str(value))

	def setMinusEvent(self):
		self.minusEvent()

	def plusEvent(self):
		value = int(self.countText.GetText())
		add = 1
		value += add
		
		if value > 20:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ITEMSHOP_CANNOT_BUY_MORE_THAN_20)
			return

		self.countText.SetText(str(value))

	def setPlusEvent(self):
		self.plusEvent()

	def clearEvent(self):
		self.__updateEvent = None
		self.__addCount = 0

	def __OnClickBuyButton(self):
		if self.__data["bonus_type"]:
			itemSelectAttributeDialog = ItemAttributeSelectDialog()
			itemSelectAttributeDialog.SetData(self.__data["attr"])
			itemSelectAttributeDialog.SetEvent(ui.__mem_func__(self.__OnSelectAttribute))
			itemSelectAttributeDialog.Show()
			self.itemSelectAttributeDialog = itemSelectAttributeDialog
		else:
			self.__AskBuyItem()

	def __AskBuyItem(self, bonus_type = itemshop.BONUS_NONE):
		itemPrice = self.__data["price"]
		itemIsFixedCount = self.__data["fixed_count"]
		itemCount = self.__data["count"]
		itemDiscount = self.__data["discount"]\
			if self.__data["discount_time"] >= app.GetGlobalTimeStamp() else 0
		itemCurrency = localeInfo.ITEMSHOP_CASH_UNIT

		if not itemIsFixedCount:
			itemCount = int(self.countText.GetText())
			itemPrice *= itemCount

		if itemDiscount:
			itemPrice *= float(100 - itemDiscount) / 100.0
			itemPrice = int(max(1, itemPrice))

		itemName = self.nameText.GetText()
		if bonus_type == itemshop.BONUS_PVP:
			itemName += " " + localeInfo.ITEMSHOP_ATTRIBUTE_PVP
		elif bonus_type == itemshop.BONUS_EXP:
			itemName += " " + localeInfo.ITEMSHOP_ATTRIBUTE_EXP

		itemBuyQuestionDialog = uiCommon.QuestionDialog()
		itemBuyQuestionDialog.SetText(localeInfo.ITEMSHOP_DO_YOU_BUY_ITEM(itemName,
																		  itemCount,
																		  localeInfo.NumberToMoneyString(itemPrice, itemCurrency)
																		  ))
		itemBuyQuestionDialog.SetAcceptEvent(lambda arg=True, arg2=bonus_type: self.__AnswerBuyItem(arg, arg2))
		itemBuyQuestionDialog.SetCancelEvent(lambda arg=False, arg2=bonus_type: self.__AnswerBuyItem(arg, arg2))
		itemBuyQuestionDialog.Open()
		self.itemBuyQuestionDialog = itemBuyQuestionDialog

	def __AnswerBuyItem(self, flag, bonus_type):
		if flag:
			net.SendItemShopBuyPacket(self.__data["id"], self.__data["category"],
									  1 if self.__data["fixed_count"] else int(self.countText.GetText()), bonus_type)

			if self.itemSelectAttributeDialog:
				self.itemSelectAttributeDialog.Close()
				self.itemSelectAttributeDialog = None

		self.itemBuyQuestionDialog.Close()
		self.itemBuyQuestionDialog = None

	def __OnSelectAttribute(self, key):
		if key == -1:
			self.itemBuyQuestionDialog = uiCommon.PopupDialog()
			self.itemBuyQuestionDialog.SetText(localeInfo.ITEMSHOP_SELECT_ATTRIBUTE_ERROR)
			self.itemBuyQuestionDialog.Open()
			return

		self.__AskBuyItem(key)

	def __GetTimeText(self, value):
		delta_time = value - app.GetGlobalTimeStamp()

		if delta_time <= 0:
			return localeInfo.ITEMSHOP_DISCOUNT_END

		text = ""

		d = delta_time // (24 * 3600)
		delta_time = delta_time % (24 * 3600)
		h = delta_time // 3600
		delta_time %= 3600
		m = delta_time // 60
		delta_time %= 60
		s = delta_time

		if d:
			text += "%dd " % d
		if h or m:
			text += "%dh " % h
		if text or m:
			text += "%dm " % m
		if text or s:
			text += "%ds" % s

		return text

	def RefreshValue(self):
		currency = localeInfo.ITEMSHOP_CASH_UNIT
		price = self.__data["price"]

		self.priceText.SetText(localeInfo.NumberToMoneyString(price, currency))

		itemPrice = price
		itemPrice *= float(100 - self.__data["discount"]) / 100.0
		itemPrice = int(max(1, itemPrice))

		if self.__data["discount"] and self.__data["discount_time"]:
			self.priceText.GetText()
			text = localeInfo.NumberToMoneyString(itemPrice, currency)
			self.priceText.SetText(text)

		if self.__data["bonus_type"]:

			self.UnregisterComponent(self.minusButton)
			self.UnregisterComponent(self.plusButton)
			self.UnregisterComponent(self.countText)
			self.UnregisterComponent(self.limitedTimeValue)
			self.UnregisterComponent(self.rectangle)
			self.UnregisterComponent(self.discountTimeValue)
			self.UnregisterComponent(self.discountSlot)
		else:
			showBanner = False

			if self.__data["limited_count"]:
				showBanner = True

				if self.__data["limited_count"] > 0 and self.__data["limited_count"] == self.__data["limited_buy_count"]:
					self.buyButton.Down()
			else:
				self.buyButton.SetUp()

			if self.__data["discount_time"] >= app.GetGlobalTimeStamp() and self.__data["discount"]:
				showBanner = True
				self.RegisterComponent(self.rectangle)
				self.RegisterComponent(self.discountTimeValue)
				self.rectangle.Show()
			else:
				self.UnregisterComponent(self.rectangle)
				self.UnregisterComponent(self.discountTimeValue)
				self.rectangle.Hide()

			if self.__data["limited_time"] >= app.GetGlobalTimeStamp():
				showBanner = True
				self.RegisterComponent(self.limitedTimeValue)
			else:
				self.UnregisterComponent(self.limitedTimeValue)

			if showBanner:
				self.RegisterComponent(self.rectangle)
				self.RegisterComponent(self.discountSlot)
				self.rectangle.Show()
			else:
				self.UnregisterComponent(self.rectangle)
				self.UnregisterComponent(self.discountSlot)
				self.rectangle.Hide()

			if self.__data["fixed_count"]:
				self.UnregisterComponent(self.minusButton)
				self.UnregisterComponent(self.plusButton)
				self.UnregisterComponent(self.countText)
			else:
				self.RegisterComponent(self.minusButton)
				self.RegisterComponent(self.plusButton)
				self.RegisterComponent(self.countText)

	def OnUpdate(self):
		if self.__updateEvent:
			if self.__nextUpdate <= app.GetTime() :
				self.__updateEvent()
				self.__nextUpdate = app.GetTime() + 0.1

		if self.__timeUpdate <= app.GetTime():
			if self.discountTimeValue.IsShow():
				text = self.__GetTimeText(self.__data["discount_time"])
				self.discountTimeValue.SetText(localeInfo.ITEMSHOP_DISCOUNT+"{} ".format(emoji.AppendEmoji("icon/emoji/image-example-007.png"))+text)

				self.priceText.SetOutline(self.__textHasOutLine)
				self.__textHasOutLine = not self.__textHasOutLine

			if self.limitedTimeValue.IsShow():
				text = self.__GetTimeText(self.__data["limited_time"])
				self.limitedTimeValue.SetText(localeInfo.ITEMSHOP_LIMITED_COUNT %
											   (self.__data["limited_count"] - self.__data["limited_buy_count"],
												self.__data["limited_count"])+" |r  "+"{}".format(emoji.AppendEmoji("icon/emoji/image-example-007.png"))+" "+text)
				if text == localeInfo.ITEMSHOP_DISCOUNT_END:
					self.buyButton.Down()

			self.__timeUpdate = app.GetTime() + 1.0

	def __OverInItem(self):
		if self.tooltipItem:
			self.tooltipItem.ClearToolTip()
			self.tooltipItem.ShowToolTip()
			if self.__data["is_vip"]:
				item_data = PREMIUM_DATA_DICT.get(self.__data["vnum"])
				if item_data:
					self.tooltipItem.SetTitle(item_data[0])
			else:
				socket = [0 for _ in xrange(player.METIN_SOCKET_MAX_NUM)]
				attr = [[0, 0] for _ in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]

				if not self.__data["bonus_type"]:
					socket = self.__data["socket"]
					attr = 0

				self.tooltipItem.AddItemData(self.__data["vnum"], socket, attr)

				item.SelectItem(self.__data["vnum"])


				if self.__data["bonus_type"]:
					self.tooltipItem.AppendDescription(localeInfo.ITEMSHOP_TOOLTIP_DETAILS, 26, 0xFF709558)


			if self.__data["additional_time"]:
				self.tooltipItem.AppendSpace(5)
				self.tooltipItem.AppendTextLine(localeInfo.ITEMSHOP_ADDITIONAL_TIME +" : "
												+ localeInfo.SecondToDHM(self.__data["additional_time"]),
												0xFF927b69)

	def __OverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()


class WheelItem(ui.NewListBoxItem):
	def __init__(self, vnum, count, chance):
		ui.NewListBoxItem.__init__(self)
		self.__LoadWindow(vnum, count)
		self.__vnum = vnum
		self.__chance = chance
		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.Hide()
		self.OnRender = None

	def __LoadWindow(self, vnum, count):
		item.SelectItem(vnum)

		self.itemImage = ui.ExpandedImageBox()
		self.itemImage.SetParent(self)
		self.itemImage.SetPosition(0, 0)
		self.itemImage.LoadImage(item.GetIconImageFileName())
		self.itemImage.Show()

		self.itemImage.OnMouseOverIn = ui.__mem_func__(self.__OverInItem)
		self.itemImage.OnMouseOverOut = ui.__mem_func__(self.__OverOutItem)

		self.RegisterComponent(self.itemImage)

		self.SetSize(208, 90)

	def ClearDictionary(self):
		self.UnregisterComponent(self.itemImage)

		self.tooltipItem = None
		self.itemImage = 0

		self.Hide()

	def __OverInItem(self):
		if self.tooltipItem:
			self.tooltipItem.SetItemToolTip(self.__vnum)
			if itemshop.ENABLE_TOMBOLA_PERCENTAGE:
				self.tooltipItem.AppendHorizontalLine()
				self.tooltipItem.AppendTextLine(localeInfo.ITEMSHOP_WHEEL_CHANCE % self.__chance)

	def __OverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def __del__(self):
		ui.NewListBoxItem.__del__(self)


WEAPON_SOCKETS = item.GetWeaponSockets()
BODY_SOCKETS = item.GetBodySockets()
ITEM_LIST = item.GetNames()


class ItemShopEditorWindow(ui.ScriptWindow):
	class Item(ui.Bar):
		def __init__(self):
			ui.Bar.__init__(self)
			self.__onClickEvent = None
			self.__textLine = None
			self.__CreateWindow()

		def __del__(self):
			ui.Bar.__del__(self)

		def __CreateWindow(self):
			self.__textLine = ui.TextLine()
			self.__textLine.SetParent(self)
			self.__textLine.SetPosition(32, 0)

			self.__textLine.Show()

			self.OnMouseLeftButtonDown = self.__OnClickButton
			self.__textLine.OnMouseLeftButtonDown = self.__OnClickButton

			self.OnMouseOverIn = self.__OnOverIn
			self.__textLine.OnMouseOverIn = self.__OnOverIn

			self.OnMouseOverOut = self.__OnOverOut
			self.__textLine.OnMouseOverOut = self.__OnOverOut

			self.SetSize(self.GetWidth(), self.GetHeight())
			self.SetColor(0x00000000)

		def __OnClickButton(self):
			if self.__onClickEvent:
				self.__onClickEvent()

			self.SetColor(grp.GenerateColor(0.0, 0.0, 0.5, 0.4))

		def SetText(self, text):
			self.__textLine.SetText(localeInfo.ITEMSHOP_EDITOR_ITEM_PREFIX % text)

		def SetEvent(self, event):
			self.__onClickEvent = event

		def GetWidth(self):
			return 206

		def GetHeight(self):
			return 20

		def __OnOverIn(self):
			self.SetColor(ui.SELECT_COLOR)

		def __OnOverOut(self):
			self.SetColor(0x00000000)

	class Expander(ui.Window):
		def __init__(self):
			ui.Window.__init__(self)
			self.__expandEvent = None
			self.__foldEvent = None
			self.__normalButton = None
			self.__expandedButton = None
			self.__isExpanded = False
			self.__CreateWindow()

		def __del__(self):
			ui.Window.__del__(self)

		def __CreateWindow(self):
			self.__normalButton = ui.Button()
			self.__normalButton.SetParent(self)
			self.__normalButton.SetUpVisual("d:/ymir work/ui/game/sphaera/list_expander_closed_normal.tga")
			self.__normalButton.SetOverVisual("d:/ymir work/ui/game/sphaera/list_expander_closed_hover.tga")
			self.__normalButton.SetDownVisual("d:/ymir work/ui/game/sphaera/list_expander_closed_hover.tga")
			self.__normalButton.SetEvent(ui.__mem_func__(self.__OnClickButton))
			self.__normalButton.Show()

			self.__expandedButton = ui.Button()
			self.__expandedButton.SetParent(self)
			self.__expandedButton.SetUpVisual("d:/ymir work/ui/game/sphaera/list_expander_open_normal.tga")
			self.__expandedButton.SetOverVisual("d:/ymir work/ui/game/sphaera/list_expander_open_hover.tga")
			self.__expandedButton.SetDownVisual("d:/ymir work/ui/game/sphaera/list_expander_open_hover.tga")
			self.__expandedButton.SetEvent(ui.__mem_func__(self.__OnClickButton))
			self.__expandedButton.Hide()

			self.__UpdateSize()

		def __OnClickButton(self):
			if self.__isExpanded:
				self.__expandedButton.Hide()
				self.__normalButton.Show()

				self.__isExpanded = not self.__isExpanded

				if self.__foldEvent:
					self.__foldEvent()
			else:
				self.__expandedButton.Show()
				self.__normalButton.Hide()

				self.__isExpanded = not self.__isExpanded

				if self.__expandEvent:
					self.__expandEvent()

			self.__UpdateSize()

		def __UpdateSize(self):
			width = self.GetWidth()
			height = self.GetHeight()

			self.SetSize(width, height)

		def SetText(self, text):
			self.__normalButton.SetText(text)
			self.__expandedButton.SetText(text)

		def SetExpandEvent(self, event):
			self.__expandEvent = event

		def SetFoldEvent(self, event):
			self.__foldEvent = event

		def IsExpanded(self):
			return self.__isExpanded

		def GetWidth(self):
			return self.__expandedButton.GetWidth() if self.__isExpanded else self.__normalButton.GetWidth()

		def GetHeight(self):
			return self.__expandedButton.GetHeight() if self.__isExpanded else self.__normalButton.GetHeight()

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__obj = {"expander": {}, "items": {}, "socket": {}, "socket_text": {},
					  "attr": {}, "attr_text": {}, "attr_value": {}, "attr_value_text": {}}
		self.__itemIndex = self.__itemCategory = -1
		self.itemToolTip = uiToolTip.ItemToolTip()
		self.itemToolTip.Hide()
		self.__data = None
		self.__LoadScript()

	def SetData(self, data):
		self.__data = data

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadScript(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/itemshopeditorwindow.py")
		except:
			import exception
			exception.Abort("ItemShopEditorWindow.__LoadScript.LoadObject")

		self.GetChild("Board").SetCloseEvent(ui.__mem_func__(self.OpenWindow))
		self.itemNameValue = self.GetChild("item_name_value")
		self.itemNameSlot = self.GetChild("item_name_slot")
		self.itemCountValue = self.GetChild("item_count_value")
		self.itemPriceValue = self.GetChild("item_price_value")
		self.itemSlot = self.GetChild("item_slot")
		self.unfixedCountCheckBox = self.GetChild("unfixed_count_checkbox")

		self.GetChild("item_count_slot").OnMouseLeftButtonDown = ui.__mem_func__(self.itemCountValue.SetFocus)
		self.GetChild("item_price_slot").OnMouseLeftButtonDown = ui.__mem_func__(self.itemPriceValue.SetFocus)

		self.itemNameSlot.OnMouseLeftButtonDown = ui.__mem_func__(self.__OnClickItemNameSlot)
		self.itemNameValue.OnMouseLeftButtonDown = ui.__mem_func__(self.__OnClickItemNameSlot)

		self.itemSlot.SetOverInItemEvent(ui.__mem_func__(self.__OverInItem))
		self.itemSlot.SetOverOutItemEvent(ui.__mem_func__(self.__OverOutItem))

		self.editButton = self.GetChild("edit_button")
		self.editButton.SetEvent(ui.__mem_func__(self.__OnClickEditButton))

		self.addButton = self.GetChild("add_button")
		self.addButton.SetEvent(ui.__mem_func__(self.__OnClickAddButton))

		self.deleteButton = self.GetChild("delete_button")
		self.deleteButton.SetEvent(ui.__mem_func__(self.__OnClickDeleteButton))

		self.unfoldListBox = self.GetChild("categories_listbox")

		text = [
			localeInfo.ITEMSHOP_CATEGORY_01,
			localeInfo.ITEMSHOP_CATEGORY_02,
			localeInfo.ITEMSHOP_CATEGORY_03,
			localeInfo.ITEMSHOP_CATEGORY_04,
			localeInfo.ITEMSHOP_CATEGORY_05,
			localeInfo.ITEMSHOP_CATEGORY_06,
			localeInfo.ITEMSHOP_CATEGORY_07,
			localeInfo.ITEMSHOP_CATEGORY_08,
			localeInfo.ITEMSHOP_CATEGORY_09,
			localeInfo.ITEMSHOP_CATEGORY_10,
			localeInfo.ITEMSHOP_CATEGORY_11,
			localeInfo.ITEMSHOP_CATEGORY_12,
			localeInfo.ITEMSHOP_CATEGORY_13,
			localeInfo.ITEMSHOP_CATEGORY_14,
		]

		for i in xrange(14):
			self.__obj["items"][i] = {}

			self.__obj["expander"][i] = self.Expander()
			self.__obj["expander"][i].SetText(text[i])
			self.__obj["expander"][i].Show()
			self.__obj["expander"][i].SetExpandEvent(lambda: self.RefreshList())
			self.__obj["expander"][i].SetFoldEvent(lambda: self.RefreshList())

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			self.__obj["socket"][i] = self.GetChild("socket_window_%02d" % (i+1))
			self.__obj["socket_text"][i] = self.GetChild("socket_text_%02d" % (i+1))

			self.__obj["socket"][i].OnMouseLeftButtonDown = lambda arg=i: self.__OnClickSocketWindow(arg)
			self.__obj["socket_text"][i].OnMouseLeftButtonDown = lambda arg=i: self.__OnClickSocketWindow(arg)

		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			self.__obj["attr"][i] = self.GetChild("attr_window_%02d" % (i+1))
			self.__obj["attr_text"][i] = self.GetChild("attr_text_%02d" % (i+1))

			self.__obj["attr_value"][i] = self.GetChild("attr_value_window_%02d" % (i+1))
			self.__obj["attr_value_text"][i] = self.GetChild("attr_value_text_%02d" % (i+1))

			self.__obj["attr"][i].OnMouseLeftButtonDown = lambda arg=i: self.__OnClickAttrWindow(arg)
			self.__obj["attr_text"][i].OnMouseLeftButtonDown = lambda arg=i: self.__OnClickAttrWindow(arg)

			self.__obj["attr_value"][i].OnMouseLeftButtonDown = lambda arg=i: self.__OnClickAttrValueWindow(arg)
			self.__obj["attr_value_text"][i].OnMouseLeftButtonDown = lambda arg=i: self.__OnClickAttrValueWindow(arg)

		self.__SetSlotData()

		self.RefreshList()
		self.wndSelector = ItemShopSelectListWindow()
		self.wndSelector.Hide()

	def RefreshList(self):
		self.unfoldListBox.ClearItem()
		for key, obj in self.__obj["expander"].iteritems():
			self.unfoldListBox.InsertItem(obj)
			if obj.IsExpanded():

				itemPos = self.__data.GetItemByCategory(key, FILTER.DEFAULT, "")

				for index in itemPos:
					data = self.__data.GetItemDataByIndex(index)

					if not data:
						continue

					item.SelectItem(data["vnum"])

					temp_item = self.Item()
					temp_item.SetText(item.GetItemName())
					temp_item.SetEvent(lambda arg=index, cat=key: self.__OnSelectItem(arg, cat))
					self.unfoldListBox.InsertItem(temp_item)

				temp_item = self.Item()
				temp_item.SetText(localeInfo.ITEMSHOP_EDITOR_ADD_ITEM)
				temp_item.SetEvent(lambda index=-1, cat=key: self.__OnSelectItem(index, cat))
				self.unfoldListBox.InsertItem(temp_item)
		self.__OnSelectItem(-1, -1)

	def __OnSelectItem(self, index, cat):

		self.__itemIndex = index
		self.__itemCategory = cat

		if index == -1:

			self.itemNameValue.SetText(localeInfo.ITEMSHOP_SELECT_ITEM)
			self.itemCountValue.SetText("1")
			self.itemPriceValue.SetText("1")

			self.itemSlot.ClearSlot(0)

			self.__SetSlotData()

			self.deleteButton.Hide()
			self.editButton.Hide()
			if cat == -1:
				self.addButton.Hide()
			else:
				self.addButton.Show()

		else:

			data = self.__data.GetItemDataByIndex(index)

			if not data:
				return

			item.SelectItem(data["vnum"])
			self.itemNameValue.SetText(item.GetItemName())
			self.itemNameValue.itemVnum = data["vnum"]
			self.itemCountValue.SetText(str(data["count"]))
			self.itemPriceValue.SetText(str(data["price"]))

			self.itemSlot.SetItemSlot(0, data["vnum"], 0)

			metinSlot = []
			for i in xrange(player.METIN_SOCKET_MAX_NUM):
				metinSlot.append(data["socket"][i])

			attrSlot = []
			for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				attrSlot.append(data["attr"][i])

			self.__SetSlotData(metinSlot, attrSlot)

			self.deleteButton.Show()
			self.editButton.Show()
			self.addButton.Hide()

	def __OnClickAttrWindow(self, slot):
		if self.__itemCategory == -1:
			return

		list = uiToolTip.ItemToolTip.AFFECT_DICT

		self.wndSelector.PrepareList("attr", list)
		self.wndSelector.Show()
		self.wndSelector.SetTop()
		self.wndSelector.SetSelectedSlot(self.__obj["attr_text"][slot])
		self.wndSelector.SetValueSlot(self.__obj["attr_value_text"][slot])

	def __OnClickSocketWindow(self, slot):
		if self.__itemCategory == -1:
			return

		data = self.__data.GetItemDataByIndex(slot)

		if not data:
			return

		item.SelectItem(data["vnum"])

		list = {}

		if item.GetItemType() == item.ITEM_TYPE_WEAPON and item.GetItemSubType() != item.WEAPON_ARROW:
			list = WEAPON_SOCKETS

		elif item.GetItemType() == item.ITEM_TYPE_ARMOR and item.GetItemSubType() == item.ARMOR_BODY:
			list = BODY_SOCKETS

		else:
			list = {}

		self.wndSelector.PrepareList("socket", list)
		self.wndSelector.Show()
		self.wndSelector.SetTop()
		self.wndSelector.SetSelectedSlot(self.__obj["socket_text"][slot])

	def __OnClickAttrValueWindow(self, slot):
		if self.__itemCategory == -1:
			return

		editLine = self.__obj["attr_value_text"][slot]
		editLine.SetFocus()

	def __OnClickEditButton(self):

		itemIndex = self.__itemIndex
		itemCategory = self.__itemCategory

		data = self.__data.GetItemDataByIndex(itemIndex)

		if not data:
			return

		itemVnum = data["vnum"]
		itemPrice = int(self.itemPriceValue.GetText())
		itemCount = int(self.itemCountValue.GetText())

		itemFixedCountFlag = bool(self.unfixedCountCheckBox.IsChecked())

		itemSocket = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			itemSocket.append(int(self.__obj["socket_text"][i].itemVnum))

		itemAttr = []
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			itemAttr.append([int(self.__obj["attr_text"][i].attrValue), int(self.__obj["attr_value_text"][i].GetText())])

		itemshop.SendItemShopPacket(itemIndex, itemCategory, itemVnum, itemPrice, itemCount, itemFixedCountFlag,
									itemSocket,
									itemAttr,
									itemshop.EDITOR_FLAG_EDIT)

	def __OnClickAddButton(self):
		itemIndex = 0
		itemCategory = self.__itemCategory

		itemVnum = self.itemNameValue.itemVnum
		itemPrice = int(self.itemPriceValue.GetText())
		itemCount = int(self.itemCountValue.GetText())

		itemFixedCountFlag = bool(self.unfixedCountCheckBox.IsChecked())

		itemSocket = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			itemSocket.append(int(self.__obj["socket_text"][i].itemVnum))

		itemAttr = []
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			itemAttr.append([int(self.__obj["attr_text"][i].attrValue), int(self.__obj["attr_value_text"][i].GetText())])

		itemshop.SendItemShopPacket(itemIndex, itemCategory, itemVnum, itemPrice, itemCount, itemFixedCountFlag,
									itemSocket,
									itemAttr,
									itemshop.EDITOR_FLAG_ADD)

		self.__OnSelectItem(-1, -1)

	def __OnClickDeleteButton(self):
		itemIndex = self.__itemIndex
		itemCategory = self.__itemCategory

		itemshop.SendItemShopPacket(itemIndex, itemCategory, 0, 0, 0, 0,
									[0 for i in xrange(player.METIN_SOCKET_MAX_NUM)],
									[(0, 0) for j in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)],
									itemshop.EDITOR_FLAG_DELETE)

		self.__itemIndex = -1
		self.__itemCategory = -1

	def __OnClickItemNameSlot(self):
		if self.__itemIndex == -1 and self.__itemCategory != -1:

			list = ITEM_LIST

			self.wndSelector.PrepareList("item", list)
			self.wndSelector.Show()
			self.wndSelector.SetTop()
			self.itemNameValue.itemVnum = 0
			self.wndSelector.SetSelectedSlot(self.itemNameValue)
			self.wndSelector.SetItemSlot(self.itemSlot)

	def __SetSlotData(self, sockets=None, attrs=None):
		if attrs is None:
			attrs = [(0, 0) for _ in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]

		if sockets is None:
			sockets = [0 for _ in xrange(player.METIN_SOCKET_MAX_NUM)]

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			name = localeInfo.ITEMSHOP_SELECT_SOCKET
			if sockets[i]:
				item.SelectItem(sockets[i])
				name = item.GetItemName()

			self.__obj["socket_text"][i].SetText(name)
			self.__obj["socket_text"][i].itemVnum = sockets[i]

		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			name = localeInfo.ITEMSHOP_SELECT_BONUS
			if attrs[i][0]:
				name = GetAffectString(attrs[i][0])

			self.__obj["attr_text"][i].SetText(name)
			self.__obj["attr_text"][i].attrValue = attrs[i][0]
			self.__obj["attr_value_text"][i].SetText(str(attrs[i][1]))

	def __OverInItem(self, slotPos):
		if self.itemToolTip:
			itemVnum = getattr(self.itemNameValue, "itemVnum", None)
			if itemVnum:

				itemSocket = []
				for i in xrange(player.METIN_SOCKET_MAX_NUM):
					itemSocket.append(int(self.__obj["socket_text"][i].itemVnum))

				itemAttr = []
				for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
					itemAttr.append([int(self.__obj["attr_text"][i].attrValue),
									 int(self.__obj["attr_value_text"][i].GetText())])

				self.itemToolTip.ClearToolTip()
				self.itemToolTip.AddRefineItemData(itemVnum, itemSocket, itemAttr)
				self.itemToolTip.ShowToolTip()


	def __OverOutItem(self):
		if self.itemToolTip:
			self.itemToolTip.HideToolTip()

	def OpenWindow(self):
		if self.IsShow():
			self.Hide()
		else:
			self.Show()
			self.SetCenterPosition()
			self.SetTop()

	def Destroy(self):
		self.Hide()
		self.ClearDictionary()

	def OnPressExitKey(self):
		self.Hide()
		return True

	def OnPressEscapeKey(self):
		self.Hide()
		return True

def GetAffectString(affectIndex):
	affectName = uiToolTip.ItemToolTip().GetAffectString(affectIndex, 99)
	affectName = affectName.replace("+99", "")
	affectName = affectName.replace("+99%", "")
	affectName = affectName.replace("99%", "")
	affectName = affectName.replace("99", "")
	affectName = affectName.replace("%", "")
	affectName = affectName.replace(":", "")
	return affectName

class ItemShopSelectListWindow(ui.ScriptWindow):
	class ItemShopSelectListItem(ui.ListBoxEx.Item):
		def __init__(self, text, index):
			ui.ListBoxEx.Item.__init__(self)
			self.__textLine = self.__CreateTextLine(text)
			self.__index = index

		def __del__(self):
			ui.ListBoxEx.Item.__del__(self)

		def SetSize(self, width, height):
			ui.ListBoxEx.Item.SetSize(self, 300, height)

		def __CreateTextLine(self, text):
			textLine = ui.TextLine()
			textLine.SetParent(self)
			textLine.SetPosition(0, 1)
			textLine.SetText(text)
			textLine.UpdateRect()
			textLine.Show()
			return textLine

		def GetItemIndex(self):
			return self.__index

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__selectedSlot = None
		self.__selectedValueSlot = None
		self.__selectedItemSlot = None
		self.__type = ""
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self):
		try:
			pythonScriptLoader = ui.PythonScriptLoader()
			pythonScriptLoader.LoadScriptFile(self, "uiscript/itemshopselectlist.py")
		except:
			import exception
			exception.Abort("ItemShopSelectListWindow.__LoadScript.LoadObject")

		try:
			self.listBox = self.GetChild("list_box_ex")
			self.scrollBar = self.GetChild("scroll_bar")
			self.board = self.GetChild("board")
			self.itemNameSlot = self.GetChild("item_name_slot")
			self.itemNameValue = self.GetChild("item_name_value")
			self.itemNameValue.OnIMEUpdate = self.__OnItemNameValueIMEUpdate
		except:

			import exception
			exception.Abort("ItemShopSelectListWindow.__LoadScript.BindObject")

		self.listBox.SetScrollBar(self.scrollBar)
		self.listBox.SetSelectEvent(ui.__mem_func__(self.__OnSelectItem))

		self.itemNameSlot.OnMouseLeftButtonDown = ui.__mem_func__(self.__OnClickItemNameSlot)

		self.board.SetCloseEvent(ui.__mem_func__(self.Hide))

	def PrepareList(self, type, objects, skipText=False):

		self.listBox.RemoveAllItems()

		self.listBox.AppendItem(self.ItemShopSelectListItem(localeInfo.ITEMSHOP_SELECT_EMPTY, 0))

		if type == "socket":

			self.listBox.SetViewItemCount(18)
			self.itemNameSlot.Hide()
			self.itemNameValue.Hide()

			for obj in objects:
				item.SelectItem(obj)
				self.listBox.AppendItem(self.ItemShopSelectListItem(item.GetItemName(), obj))
			self.board.SetTitleName(localeInfo.ITEMSHOP_SELECT_SOCKET)

		elif type == "attr":

			self.listBox.SetViewItemCount(18)
			self.itemNameSlot.Hide()
			self.itemNameValue.Hide()

			for obj in objects.keys():
				self.listBox.AppendItem(self.ItemShopSelectListItem(GetAffectString(obj), obj))

			self.board.SetTitleName(localeInfo.ITEMSHOP_SELECT_BONUS)

		if type == "item":

			self.listBox.SetViewItemCount(17)
			self.itemNameSlot.Show()
			self.itemNameValue.Show()
			self.itemNameValue.SetFocus()

			for obj in objects:
				if obj["name"]:
					self.listBox.AppendItem(self.ItemShopSelectListItem(obj["name"], obj["vnum"]))
			self.board.SetTitleName(localeInfo.ITEMSHOP_SELECT_ITEM)
			if not skipText:
				self.itemNameValue.SetText(localeInfo.ITEMSHOP_SELECT_ITEM)

		self.__type = type

	def __OnSelectItem(self, obj):
		if self.__selectedSlot:
			if self.__type == "socket":
				itemVnum = obj.GetItemIndex()
				if itemVnum == 0:
					self.__selectedSlot.SetText(localeInfo.ITEMSHOP_SELECT_EMPTY)
				else:
					item.SelectItem(itemVnum)
					self.__selectedSlot.SetText(item.GetItemName())
				self.__selectedSlot.itemVnum = itemVnum
			elif self.__type == "attr":
				attrValue = obj.GetItemIndex()
				if attrValue == 0:
					self.__selectedSlot.SetText(localeInfo.ITEMSHOP_SELECT_EMPTY)
				else:
					self.__selectedSlot.SetText(GetAffectString(attrValue))

				self.__selectedSlot.attrValue = attrValue
				self.__selectedValueSlot.SetFocus()

			elif self.__type == "item":
				itemVnum = obj.GetItemIndex()
				if itemVnum == 0:
					self.__selectedSlot.SetText(localeInfo.ITEMSHOP_SELECT_EMPTY)
				else:
					item.SelectItem(itemVnum)
					self.__selectedSlot.SetText(item.GetItemName())
				if self.__selectedItemSlot:
					self.__selectedItemSlot.SetItemSlot(0, itemVnum, 0)
				self.__selectedSlot.itemVnum = itemVnum

		self.Hide()

	def SetSelectedSlot(self, slot):
		self.__selectedSlot = slot

		self.__selectedValueSlot = None
		self.__selectedItemSlot = None

	def SetValueSlot(self, slot):
		self.__selectedValueSlot = slot

	def SetItemSlot(self, slot):
		self.__selectedItemSlot = slot

	def __OnItemNameValueIMEUpdate(self):
		ui.EditLine.OnIMEUpdate(self.itemNameValue)
		self.__Search(self.itemNameValue.GetText())

	def __Search(self, destName = ""):
		self.listBox.RemoveAllItems()

		list = []

		if destName:
			for item in ITEM_LIST:
				name = item["name"]

				if len(name):
					if name.lower().find(destName.lower()) != -1:
						list.append(item)
		else:
			list = ITEM_LIST

		self.PrepareList("item", list, True)

	def __OnClickItemNameSlot(self):
		from difflib import SequenceMatcher
		text = self.itemNameValue.GetText().lower()
		destText = localeInfo.ITEMSHOP_SELECT_ITEM.lower()
		if SequenceMatcher(None, text, destText).ratio() >= 0.75:
			self.itemNameValue.SetText("")

class ScrollBar(ui.Window):
	SCROLLBAR_WIDTH = 17
	SCROLLBAR_MIDDLE_HEIGHT = 9
	SCROLLBAR_BUTTON_HEIGHT = 0
	MIDDLE_BAR_POS = 8
	MIDDLE_BAR_UPPER_PLACE = 3
	MIDDLE_BAR_DOWNER_PLACE = 5
	TEMP_SPACE = MIDDLE_BAR_UPPER_PLACE + MIDDLE_BAR_DOWNER_PLACE
	if app.__BL_SMOOTH_SCROLL__:
		SMOOTH_RATIO = 1

	def __init__(self):
		ui.Window.__init__(self)

		self.pageSize = 1
		self.curPos = 0.0
		self.eventScroll = lambda *arg: None
		self.lockFlag = False
		self.scrollStep = 1
		if app.__BL_SMOOTH_SCROLL__:
			self.smooth_mode = False
			self.actual_pos = 0.0
			self.target_pos = 0.0

		self.CreateScrollBar()

	def __del__(self):
		ui.Window.__del__(self)

	def CreateScrollBar(self):
		barSlot = ui.ExpandedImageBox()
		barSlot.SetParent(self)
		barSlot.AddFlag("not_pick")
		barSlot.LoadImage(PATH+"scroll_slot.png")
		barSlot.Show()

		middleBar = ui.DragButton()
		middleBar.SetParent(self)
		middleBar.AddFlag("movable")
		middleBar.SetMoveEvent(ui.__mem_func__(self.OnMove))
		middleBar.SetUpVisual(PATH+"scroll_button.png")
		middleBar.SetOverVisual(PATH+"scroll_button.png")
		middleBar.SetDownVisual(PATH+"scroll_button.png")
		middleBar.Show()

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
		pass

	def SetScrollBarSize(self, height):
		self.pageSize = (height - self.SCROLLBAR_BUTTON_HEIGHT * 2) - self.SCROLLBAR_MIDDLE_HEIGHT - (self.TEMP_SPACE)
		self.SetSize(self.SCROLLBAR_WIDTH, height)
		self.middleBar.SetRestrictMovementArea(self.MIDDLE_BAR_POS,
											   self.SCROLLBAR_BUTTON_HEIGHT + self.MIDDLE_BAR_UPPER_PLACE,
											   self.MIDDLE_BAR_POS + 2,
											   height - self.SCROLLBAR_BUTTON_HEIGHT * 2 - self.TEMP_SPACE)
		self.middleBar.SetPosition(self.MIDDLE_BAR_POS, 0)

		self.UpdateBarSlot()

	def UpdateBarSlot(self):
		self.barSlot.SetScale(1.0, float(self.GetHeight()) / float(self.barSlot.GetHeight()))

	def GetPos(self):
		return self.curPos

	def SetPos(self, pos):
		pos = max(0.0, pos)
		pos = min(1.0, pos)

		newPos = float(self.pageSize) * pos
		self.middleBar.SetPosition(self.MIDDLE_BAR_POS,
								   int(newPos) + self.SCROLLBAR_BUTTON_HEIGHT + self.MIDDLE_BAR_UPPER_PLACE)
		self.OnMove()

	def SetScrollStep(self, step):
		self.scrollStep = step

	def GetScrollStep(self):
		return self.scrollStep

	def OnMouseWheel(self, nLen):
		if nLen > 0:
			self.OnUp()
			return True
		elif nLen < 0:
			self.OnDown()
			return True
		return False

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
		pickedPos = yMouseLocalPosition - self.SCROLLBAR_BUTTON_HEIGHT - self.SCROLLBAR_MIDDLE_HEIGHT / 2
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

class PopupDialog(ui.ScriptWindow):
	class DIR:
		UP = 0
		DOWN = 1

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__event = None
		self.__eventArg = None
		self.__posX = 100
		self.__posY = 0
		self.__nextUpdate = 0
		self.__dir = 0
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self):
		try:
			pythonScriptLoader = ui.PythonScriptLoader()
			pythonScriptLoader.LoadScriptFile(self, "uiscript/itemshoppopupdialog.py")
		except:
			import exception
			exception.Abort("PopupDialog.__LoadWindow.LoadObject")

		self.__text = self.GetChild("popup_text1")
		self.__text2 = self.GetChild("popup_text2")

		for item in self.ElementDictionary.values():
			item.OnMouseLeftButtonDown = self.__OnClick

	def Open(self, text, event, *event_arg):
		self.__event = event
		self.__eventArg = event_arg
		self.__text.SetText(text)
		self.__text2.SetText(emoji.AppendEmoji("icon/emoji/key_lclick.png")+localeInfo.ITEMSHOP_POPUP_SECOND_LINE)

		self.Show()

		self.__posY = wndMgr.GetScreenHeight()
		self.__dir = self.DIR.UP

	def __OnClick(self):
		if self.__event:
			self.__event(*self.__eventArg)

		self.__dir = self.DIR.DOWN
		self.__nextUpdate = 0.0

	def OnUpdate(self):
		if self.__posY != 0 and app.GetTime() >= self.__nextUpdate:
			if self.__dir == self.DIR.UP:
				self.__posY -= 1

				self.SetPosition(self.__posX, self.__posY)

				if self.__posY <= wndMgr.GetScreenHeight() - 150:
					self.__nextUpdate = app.GetTime() + 3.0
					self.__dir = self.DIR.DOWN

			elif self.__dir == self.DIR.DOWN:
				self.__posY += 1

				self.SetPosition(self.__posX, self.__posY)

				if self.__posY >= wndMgr.GetScreenHeight() + self.GetHeight():
					self.Hide()
					self.__posY = 0

class ItemAttributeSelectDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__text = []
		self.__data = {}
		self.__attr = -1
		self.__event = None
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __CreateTextLine(self, x, y, parent, text):
		textLine = ui.TextLine()
		textLine.SetParent(parent)
		textLine.SetPosition(x, y)
		textLine.SetHorizontalAlignCenter()
		textLine.SetWindowHorizontalAlignCenter()
		#textLine.SetFontName("Tahoma:13b")
		textLine.SetText(text)
		textLine.Show()
		return textLine

	def __LoadWindow(self):
		try:
			pythonScriptLoader = ui.PythonScriptLoader()
			pythonScriptLoader.LoadScriptFile(self, "uiscript/itemshopselectattributedialog.py")
		except:
			import exception
			exception.Abort("ItemSelectDialog.__LoadWindow.LoadObject")

		self.__buyButton = self.GetChild("buy_button")
		self.__attributeBoard = self.GetChild("attribute_board")

		self.__attributeButton = {
			itemshop.BONUS_PVP : self.GetChild("pvp_button"),
			itemshop.BONUS_EXP : self.GetChild("exp_button"),
		}

		self.__buyButton.SetEvent(ui.__mem_func__(self.__OnClickBuyButton))
		for key, value in self.__attributeButton.items():
			value.SetEvent(ui.__mem_func__(self.__OnClickAtrributeButton), key)

		self.RefreshText()

	def __OnClickBuyButton(self):
		if self.__event:
			self.__event(self.__attr)

	def __OnClickAtrributeButton(self, key):
		self.__attr = key

		for _, value in self.__attributeButton.items():
			value.SetUp()

		self.__attributeButton[key].Down()
		self.RefreshText()

	def SetData(self, data):
		self.__data = data

	def SetEvent(self, event):
		self.__event = event

	def RefreshText(self):
		for text in self.__text:
			text.Hide()
			text = None

		self.__text = []

		if self.__attr == -1:
			text = self.__CreateTextLine(0, 0, self.__attributeBoard, localeInfo.ITEMSHOP_SELECT_ATTRIBUTE)
			text.SetVerticalAlignCenter()
			text.SetWindowVerticalAlignCenter()

			self.__text.append(text)
		else:
			data = self.__data[self.__attr]
			data = filter(lambda x: x != [0, 0], data)

			y = (self.__attributeBoard.GetHeight() - len(data) * (17 + 2)) / 2

			for attr in data:
				text = self.__CreateTextLine(0, y, self.__attributeBoard,
											 uiToolTip.ItemToolTip().GetAffectString(attr[0], attr[1]))
				self.__text.append(text)
				y += 17 + 2

	def Close(self):
		self.Hide()
		self.ClearDictionary()

		for text in self.__text:
			text.Hide()
			text = None

		self.__text = []
		self.__event = None

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnPressExitKey(self):
		self.Close()
		return True

