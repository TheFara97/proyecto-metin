import uiToolTip
import time
import event
import ui
import constInfo
import app
import renderTarget
import chat
import player
import nonplayer
import net
import emoji
import localeInfo

CHECKBOX_PATH = "elvion/game/warpwindow/dungeon_info/"

ITEM_LIST_BY_CAT = {
	0 : { 0 : 71036, 1: 72348 },
	1 : { 0 : 71037, 1: 72348 },
}

BLOCKED_CAT_LIST = [2, 3, 4]
RENDER_INDEX = 10
CAT_LIST = [localeInfo.MISSIONS_WINDOW_BIOLOG, localeInfo.MISSIONS_WINDOW_COLLECTOR, localeInfo.MISSIONS_WINDOW_HUNTING, localeInfo.MISSIONS_WINDOW_BOSSOLOGY, localeInfo.MISSIONS_WINDOW_STONEOLOGY]
LOCATION_BY_MOBID = {
	0:"0",
	101:localeInfo.WARP_WINDOW_MAP_1,
	502:localeInfo.WARP_WINDOW_MAP_2,
	636:localeInfo.WARP_WINDOW_MAP_7,691:localeInfo.WARP_WINDOW_MAP_7,
	2074:localeInfo.WARP_WINDOW_MAP_9, 2091:localeInfo.WARP_WINDOW_MAP_9, 2034:localeInfo.WARP_WINDOW_MAP_9, 2075:localeInfo.WARP_WINDOW_MAP_9,
	3190:localeInfo.WARP_WINDOW_MAP_10, 3102:localeInfo.WARP_WINDOW_MAP_10, 3103:localeInfo.WARP_WINDOW_MAP_10, 3104:localeInfo.WARP_WINDOW_MAP_10,
	8213:localeInfo.WARP_WINDOW_MAP_11, 9673:localeInfo.WARP_WINDOW_MAP_11, 9671:localeInfo.WARP_WINDOW_MAP_11,
	6775:localeInfo.WARP_WINDOW_MAP_12, 6781:localeInfo.WARP_WINDOW_MAP_12, 6779:localeInfo.WARP_WINDOW_MAP_12,
	6006:localeInfo.WARP_WINDOW_MAP_13, 34600:localeInfo.WARP_WINDOW_MAP_13, 6002:localeInfo.WARP_WINDOW_MAP_13,
	3302:localeInfo.WARP_WINDOW_MAP_14, 3304:localeInfo.WARP_WINDOW_MAP_14,
	591:localeInfo.WARP_WINDOW_MAP_2,3191:localeInfo.WARP_WINDOW_MAP_10,8214:localeInfo.WARP_WINDOW_MAP_11,
	6789:localeInfo.WARP_WINDOW_MAP_12,34601:localeInfo.WARP_WINDOW_MAP_13,3391:localeInfo.WARP_WINDOW_MAP_14,
	8007:localeInfo.WARP_WINDOW_MAP_2,8008:localeInfo.WARP_WINDOW_MAP_7,2095:localeInfo.WARP_WINDOW_MAP_9,8052:localeInfo.WARP_WINDOW_MAP_10,
	8210:localeInfo.WARP_WINDOW_MAP_11,6798:localeInfo.WARP_WINDOW_MAP_12,6801:localeInfo.WARP_WINDOW_MAP_13,6803:localeInfo.WARP_WINDOW_MAP_14,
	201014:localeInfo.WARP_WINDOW_MAP_18,201015:localeInfo.WARP_WINDOW_MAP_18,201012:localeInfo.WARP_WINDOW_MAP_18,201013:localeInfo.WARP_WINDOW_MAP_18,201018:localeInfo.WARP_WINDOW_MAP_18,201019:localeInfo.WARP_WINDOW_MAP_18
}
def _b(key, val):
	attr = getattr(localeInfo, key)
	if callable(attr):
		return attr(val)
	return attr % val

_NONE = "-"

BONUS_LIST = {
	0 : { 0 : {
			0: _b("MISSIONS_BONUS_ATTBONUS_MONSTER", 10),
			1: _b("MISSIONS_BONUS_ATT_GRADE_BONUS", 200),
			2: _b("MISSIONS_BONUS_ATTBONUS_RULER", 5),
			3: _b("MISSIONS_BONUS_TOTAL_DAMAGE", 5),
			4: _b("MISSIONS_BONUS_TOTAL_DAMAGE", 10),
			5: _b("MISSIONS_BONUS_ATTBONUS_DEVIL", 15),
			6: _b("MISSIONS_BONUS_SKILL_DAMAGE_PVM", 10),
			7: _NONE, 8: _NONE,
		}, 1 : {
			0: _NONE, 1: _NONE, 2: _NONE,
			3: _b("MISSIONS_BONUS_SKILL_DAMAGE_RESIST", 5),
			4: _b("MISSIONS_BONUS_AVG_DAMAGE", 10),
			5: _b("MISSIONS_BONUS_ATTBONUS_ORC", 15),
			6: _b("MISSIONS_BONUS_ATTBONUS_ELEMENT", 5),
			7: _NONE, 8: _NONE,
		}, 2 : {
			0: _NONE, 1: _NONE, 2: _NONE, 3: _NONE,
			4: _b("MISSIONS_BONUS_SKILL_DAMAGE", 5),
			5: _b("MISSIONS_BONUS_ATTBONUS_HUMAN", 30),
			6: _b("MISSIONS_BONUS_ELEMENT_RESIST", 5),
			7: _NONE, 8: _NONE,
		},
	},
	1 : { 0 : {
			0: _b("MISSIONS_BONUS_ATTBONUS_MONSTER", 10),
			1: _b("MISSIONS_BONUS_ATTBONUS_MONSTER", 15),
			2: _b("MISSIONS_BONUS_ATTBONUS_STONE", 10),
			3: _b("MISSIONS_BONUS_AVG_DAMAGE", 15),
			4: _b("MISSIONS_BONUS_ATTBONUS_MONSTER", 20),
			5: _b("MISSIONS_BONUS_DUNGEON_POWER", 10),
			6: _b("MISSIONS_BONUS_TOTAL_DAMAGE", 5),
			7: _b("MISSIONS_BONUS_ATTBONUS_RULER", 5),
			8: _b("MISSIONS_BONUS_ATTBONUS_RULER", 5),
		}, 1 : {
			0: _b("MISSIONS_BONUS_ATTBONUS_HUMAN", 10),
			1: _b("MISSIONS_BONUS_ATTBONUS_HUMAN", 15),
			2: _NONE, 3: _NONE,
			4: _b("MISSIONS_BONUS_ATTBONUS_HUMAN", 20),
			5: _NONE,
			6: _b("MISSIONS_BONUS_SKILL_DAMAGE", 10),
			7: _b("MISSIONS_BONUS_ATTBONUS_BOSS", 5),
			8: _b("MISSIONS_BONUS_ATTBONUS_BOSS", 5),
		}, 2 : {
			0: _NONE, 1: _NONE, 2: _NONE, 3: _NONE, 4: _NONE, 5: _NONE,
			6: _b("MISSIONS_BONUS_ATTBONUS_BOSS", 10),
			7: _b("MISSIONS_BONUS_ATTBONUS_STONE", 5),
			8: _b("MISSIONS_BONUS_ATTBONUS_STONE", 5),
		},
	},
	2 : { 0 : {
			0: _b("MISSIONS_BONUS_ATTBONUS_MONSTER", 1),
			1: _b("MISSIONS_BONUS_ATTBONUS_MONSTER", 2),
			2: _b("MISSIONS_BONUS_ATTBONUS_MONSTER", 3),
			3: _b("MISSIONS_BONUS_ATTBONUS_MONSTER", 4),
			4: _b("MISSIONS_BONUS_ATTBONUS_MONSTER", 5),
			5: _b("MISSIONS_BONUS_ATTBONUS_MONSTER", 6),
			6: _b("MISSIONS_BONUS_ATTBONUS_MONSTER", 8),
			7: _b("MISSIONS_BONUS_ATTBONUS_MONSTER", 10),
			8: _b("MISSIONS_BONUS_ATTBONUS_MONSTER", 15),
		}, 1 : {
			0: _NONE, 1: _NONE, 2: _NONE, 3: _NONE,
			4: _NONE, 5: _NONE, 6: _NONE, 7: _NONE, 8: _NONE,
		}, 2 : {
			0: _NONE, 1: _NONE, 2: _NONE, 3: _NONE,
			4: _NONE, 5: _NONE, 6: _NONE, 7: _NONE, 8: _NONE,
		},
	},
	3 : { 0 : {
			0: _b("MISSIONS_BONUS_ATTBONUS_BOSS", 1),
			1: _b("MISSIONS_BONUS_ATTBONUS_BOSS", 1),
			2: _b("MISSIONS_BONUS_ATTBONUS_BOSS", 1),
			3: _b("MISSIONS_BONUS_ATTBONUS_BOSS", 1),
			4: _b("MISSIONS_BONUS_ATTBONUS_BOSS", 1),
			5: _b("MISSIONS_BONUS_ATTBONUS_BOSS", 2),
			6: _b("MISSIONS_BONUS_ATTBONUS_BOSS", 3),
			7: _b("MISSIONS_BONUS_ATTBONUS_BOSS", 4),
			8: _b("MISSIONS_BONUS_ATTBONUS_BOSS", 5),
			9: _b("MISSIONS_BONUS_ATTBONUS_BOSS", 5),
		}, 1 : {
			0: _NONE, 1: _NONE, 2: _NONE, 3: _NONE,
			4: _NONE, 5: _NONE, 6: _NONE, 7: _NONE, 8: _NONE,
		}, 2 : {
			0: _NONE, 1: _NONE, 2: _NONE, 3: _NONE,
			4: _NONE, 5: _NONE, 6: _NONE, 7: _NONE, 8: _NONE,
		},
	},
	4 : { 0 : {
			0: _b("MISSIONS_BONUS_ATTBONUS_STONE", 1),
			1: _b("MISSIONS_BONUS_ATTBONUS_STONE", 1),
			2: _b("MISSIONS_BONUS_ATTBONUS_STONE", 1),
			3: _b("MISSIONS_BONUS_ATTBONUS_STONE", 1),
			4: _b("MISSIONS_BONUS_ATTBONUS_STONE", 1),
			5: _b("MISSIONS_BONUS_ATTBONUS_STONE", 2),
			6: _b("MISSIONS_BONUS_ATTBONUS_STONE", 3),
			7: _b("MISSIONS_BONUS_ATTBONUS_STONE", 4),
			8: _b("MISSIONS_BONUS_ATTBONUS_STONE", 5),
			9: _b("MISSIONS_BONUS_ATTBONUS_STONE", 5),
		}, 1 : {
			0: _NONE, 1: _NONE, 2: _NONE, 3: _NONE,
			4: _NONE, 5: _NONE, 6: _NONE, 7: _NONE, 8: _NONE,
		}, 2 : {
			0: _NONE, 1: _NONE, 2: _NONE, 3: _NONE,
			4: _NONE, 5: _NONE, 6: _NONE, 7: _NONE, 8: _NONE,
		},
	},
}

CATEGORY_BUTTON_NAME = ["Biologický výzkum", "Sbìratel", "Lov", "Bossologie", "Balvanologie"]
QUEST_COUNTER = {
	0 : 7,
	1 : 9,
	2 : 9,
	3 : 10,
	4 : 10,
}

class CollectWindow(ui.ScriptWindow):		
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		self.tooltipItem = uiToolTip.ItemToolTip()
		self.selectedWindow = 0
		self.realwindow = 0
		self.notificationEnabled = 0
		self.data = {i : {"ITEM_VNUM" : 0, "TIME" : -1, "COUNT" : 0, "COUNT_TOTAL" : 0, "TAKE_CHANCE" : 0, "RENDERTARGET_VNUM" : 0, "QUEST_INDEX" : 0, "REQUIRED_LEVEL" : 0, } for i in xrange(len(constInfo.CollectWindowQID))}
		self.progress = {i : {"COMPLETED" : 0} for i in xrange(len(constInfo.CollectWindowQID))}
		self.CategoryButtonList = []
		self.bonus_text = {}

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		
	def __LoadWindow(self):
		if self.isLoaded == 1:
			return
			
		self.isLoaded = 1
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/collectwindow.py")
		except:
			import exception
			exception.Abort("CollectWindow.LoadWindow.LoadObject")

		try:
			self.titleBar = self.GetChild("TitleBar")
			self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

			for i in xrange(3):
				self.bonus_text[i] = self.GetChild("bonustext_{}".format(i))


			for i in xrange(len(constInfo.CollectWindowQID)):
				self.CategoryButtonList.append(self.GetChild("CategoryButton_%d" % i))
				self.CategoryButtonList[i].SetEvent(ui.__mem_func__(self.__ChangeCategory), i)

			self.header_text_main = self.GetChild("header_text_main")
			self.time_left = self.GetChild("time_left")
			self.item_count = self.GetChild("item_count")
			self.chance_text = self.GetChild("chance_text")
			self.ItemSlot = self.GetChild("item_slot")
			self.ItemSlot.SetOverInItemEvent(ui.__mem_func__(self.__OverInItem))
			self.ItemSlot.SetOverOutItemEvent(ui.__mem_func__(self.__OverOutItem))
			self.blocked_slot = self.GetChild("blocked_slot")

			self.render = self.GetChild("RenderTarget")
			self.location_text = self.GetChild("location_text")
			self.mobname_text = self.GetChild("mobname_text")

			self.collect_button = self.GetChild("collect_button")
			self.collect_button.SetEvent(ui.__mem_func__(self.__ClickCollectButton))
			self.time_button1 = self.GetChild("time_button1")
			self.chance_button = self.GetChild("chance_button")
			self.chance_button.SetEvent(ui.__mem_func__(self.__useitem), 0)
			self.time_button1.SetEvent(ui.__mem_func__(self.__useitem), 1)

			self.PageText = self.GetChild("PageText")
			self.PrevPageBtn = self.GetChild("PrevPageBtn")
			self.NextPageBtn = self.GetChild("NextPageBtn")

			self.PrevPageBtn.SetEvent(ui.__mem_func__(self.SendCommand), 0)
			self.NextPageBtn.SetEvent(ui.__mem_func__(self.SendCommand), 1)

			self.progressbar = self.GetChild("progressbar")
			
			self.lowlevel_bg = self.GetChild("lowlevel_bg")
			self.lowlevel_text = self.GetChild("lowlevel_text")

			self.checkBox = self.GetChild("checkbox")
			self.checkBox.SetEvent(ui.__mem_func__(self.__ClickCheckBox))
		except:
			import exception
			exception.Abort("CollectWindow.LoadWindow.BindObject")

		self.__ChangeCategory(self.selectedWindow)
		self.SetCenterPosition()

	def Open(self):
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)

	def Show(self):
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()

	def __ChangeCategory(self, category):
		self.realwindow = category
		self.selectedWindow = category

		for obj in self.CategoryButtonList:
			obj.Enable()
			obj.SetUp()

		if (player.GetLevel() < self.data[self.selectedWindow]["REQUIRED_LEVEL"]):
			self.lowlevel_bg.Show()
			self.lowlevel_text.SetText(localeInfo.MISSIONS_REQUIRED_LEVEL+str(self.data[self.selectedWindow]["REQUIRED_LEVEL"]))
		else:
			self.lowlevel_bg.Hide()

		self.header_text_main.SetText(CAT_LIST[category])
		self.ItemSlot.SetItemSlot(0, self.data[category]["ITEM_VNUM"], 0)
		self.item_count.SetText(str(self.data[category]["COUNT"])+"/"+str(self.data[category]["COUNT_TOTAL"]))

		if category not in BLOCKED_CAT_LIST:
			self.chance_text.Show()
			self.time_left.Show()
			self.blocked_slot.Hide()

			self.collect_button.Show()
			self.chance_button.Show()
			self.time_button1.Show()
		else:
			self.chance_text.Hide()
			self.blocked_slot.Show()
			self.time_left.Hide()

			self.collect_button.Hide()
			self.chance_button.Hide()
			self.time_button1.Hide()

		renderTarget.SetBackground(RENDER_INDEX, "d:/ymir work/ui/game/myshop_deco/model_view_bg.sub")
		renderTarget.SetVisibility(RENDER_INDEX, True)

		if self.data[category]["RENDERTARGET_VNUM"] != 0:
			renderTarget.SelectModel(RENDER_INDEX, self.data[category]["RENDERTARGET_VNUM"])

			self.location_text.SetText(localeInfo.MISSIONS_WINDOW_LOCATION+LOCATION_BY_MOBID[self.data[category]["RENDERTARGET_VNUM"]])
			self.mobname_text.SetText(str(nonplayer.GetMonsterName(self.data[category]["RENDERTARGET_VNUM"])) + " |cFFa7ff33(Lv. "+str(nonplayer.GetMonsterLevel(self.data[category]["RENDERTARGET_VNUM"]))+")")
			for i in xrange(3):
				self.bonus_text[i].SetText(BONUS_LIST[category][i][self.data[category]["QUEST_INDEX"]])
		else:
			renderTarget.SelectModel(RENDER_INDEX, -1)
			
			self.location_text.SetText(localeInfo.MISSIONS_WINDOW_LOCATION)
			self.mobname_text.SetText("-")
			for i in xrange(3):
				self.bonus_text[i].SetText("-")


		self.PageText.SetText(str(self.data[category]["QUEST_INDEX"]))

		self.progressbar.SetPercentage(self.data[category]["COUNT"], self.data[category]["COUNT_TOTAL"])

		self.CategoryButtonList[category].Disable()
		self.CategoryButtonList[category].Down()

	def SendCommand(self, type):
		qid = constInfo.CollectWindowQID[self.selectedWindow]
		questid = self.data[self.selectedWindow]["QUEST_INDEX"]
		if self.selectedWindow == 1:
			if type == 0:
				if questid == 0 or questid < 0:
					return
				else:
					net.SendChatPacket("/choose_quest "+str(questid-1)+" "+str(qid))
					self.__ChangeCategory(self.selectedWindow)
			elif type == 1:
				if questid < 0 or questid == 8 or questid > 8:
					return
				else:
					net.SendChatPacket("/choose_quest "+str(questid+1)+" "+str(qid))
					self.__ChangeCategory(self.selectedWindow)
			self.PageText.SetText(str(self.data[self.selectedWindow]["QUEST_INDEX"]))
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MISSIONS_WINDOW_NOT_POSSIBLE)

	def	__OverInItem(self, slot_num):
		if self.tooltipItem:
			self.tooltipItem.ClearToolTip()
			self.tooltipItem.AddItemData(self.data[self.selectedWindow]["ITEM_VNUM"], 0, 0)
		
	def	__OverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.ClearToolTip()
			self.tooltipItem.HideToolTip()
		
	def AddData(self, windowType, time, count, itemVnum, countTotal, chance, rendertargetvnum, questindex, requiredLevel):
		self.data[windowType]["ITEM_VNUM"] = itemVnum
		self.data[windowType]["TIME"] = max(0, app.GetTime()+time)
		self.data[windowType]["COUNT"] = count
		self.data[windowType]["COUNT_TOTAL"] = countTotal
		self.data[windowType]["TAKE_CHANCE"] = chance
		self.data[windowType]["RENDERTARGET_VNUM"] = rendertargetvnum
		self.data[windowType]["QUEST_INDEX"] = questindex
		self.data[windowType]["REQUIRED_LEVEL"] = requiredLevel
		self.quest = questindex
		self.__ChangeCategory(self.realwindow)


	def OnUpdate(self):
		if self.selectedWindow not in BLOCKED_CAT_LIST:
			self.time_left.SetText(time.strftime("%H:%M:%S", time.gmtime(max(float(self.data[self.selectedWindow]["TIME"])-app.GetTime(), 0))))
			self.chance_text.SetText(str(self.data[self.selectedWindow]["TAKE_CHANCE"])+"%")

	def __ClickCollectButton(self):
		qid = constInfo.CollectWindowQID[self.selectedWindow]
		event.QuestButtonClick(qid, 1)

	def __useitem(self, value):
		item_map = {
			0: {1: 71037, 0: 71036},
			1: {1: 72349, 0: 72348}
		}

		item_to_search = item_map.get(value, {}).get(self.selectedWindow)
		if item_to_search is None:
			return

		inventory_ranges = [
			range(player.INVENTORY_PAGE_SIZE * player.INVENTORY_PAGE_COUNT),
			range(820, 954)
		]

		for inventory_range in inventory_ranges:
			for i in inventory_range:
				if player.GetItemIndex(i) == item_to_search:
					net.SendItemUsePacket(i)
					return

	def SendTime(self, val, time):
		val_conv = str(val)
		time_conv = float(time)
		if val_conv == str(0):
			self.data[0]["TIME"] = max(0, app.GetTime()+time_conv)
		elif val_conv == str(1):
			self.data[1]["TIME"] = max(0, app.GetTime()+time_conv)

	def SendChance(self, val, chance):
		chat.AppendChat(chat.CHAT_TYPE_INFO, str(val)+","+str(chance))
		chance_conv = str(chance)
		val_conv = str(val)
		if val_conv == str(0):
			self.data[0]["TAKE_CHANCE"] = chance_conv
		elif val_conv == str(1):
			self.data[1]["TAKE_CHANCE"] = chance_conv

	def __ClickCheckBox(self):
		net.SendChatPacket("/enable_mission_notifications")

	def SetNotificationState(self, state):
		self.notificationEnabled = state
		if state:
			checkImg = CHECKBOX_PATH + "checkbox_checked.png"
		else:
			checkImg = CHECKBOX_PATH + "checkbox_empty.png"
		self.checkBox.SetUpVisual(checkImg)
		self.checkBox.SetOverVisual(checkImg)
		self.checkBox.SetDownVisual(checkImg)

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnPressExitKey(self):
		self.Close()
		return True