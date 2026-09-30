import constInfo
import systemSetting
import wndMgr
import chat
import app
import player
import uiTaskBar
import net
import uiCharacter
import uiInventory
import uiBonusChanger
import uiDragonSoul
import uiChat
import uiMessenger
import guild
import snd
if app.ENABLE_AUTO_SELECT_SKILL:
	import uiAutoSkill
if app.ENABLE_VOTE4BUFF:
	import uivote4buff
if app.ENABLE_AURA_SYSTEM:
	import uiaura
if app.ENABLE_EVENT_MANAGER:
	import uiEventCalendar

if app.ENABLE_ODLAMKI_SYSTEM:
	import uiOdlamki

if app.ENABLE_LOADING_PERFORMANCE:
	import uiWarpShower

if app.ENABLE_LINK_IN_CHAT:
	import webbrowser

if app.ENABLE_VS_SHOP_SEARCH:
	import uiShopSearch
	
if app.ENABLE_OFFLINE_SHOP:
	import  uiOfflineShop, uiShopSearch
if app.ENABLE_PVP_RANKING:
	import uipvpranking

import uiIkonki

if app.TAKE_LEGEND_DAMAGE_BOARD_SYSTEM:
	import uiLegendDamageWindow
if app.__SPIN_WHEEL__:
	import uiSpinWheel
if app.ENABLE_WHEEL_OF_FORTUNE:
	import uiWheelofFortune
if app.ENABLE_ITEMSHOP:
	import uiItemShopNew
import uiCasket

import ui
import uiHelp
import uiWhisper
import uiPointReset
import grp
if constInfo.GIFT_CODE_SYSTEM:
	import uigiftcodewindow
import uiShop
import uiBonus
import uiExchange
import uiSystem
import uiRestart
import uiToolTip
import uiMiniMap
import uiParty
import uiSafebox
import uiGuild
import uiWonExchange
import uiQuest
import uiPrivateShopBuilder
import uiCommon
import uiRefine
import uiEquipmentDialog
import uiGameButton
import uiTip
import uiCube
import miniMap
import uiSelectItem
import uiScriptLocale
import event
import localeInfo
import uiremoveitem
import uiCollectWindow
import uiBlend

if app.ENABLE_NOTIFICATION_SYSTEM:
	import uiNotificationmanager
if app.ENABLE_MOUNT_COSTUME_SYSTEM:
	import uiMount
if app.ENABLE_RESP_SYSTEM:
	import uiresp
if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
	import uiBuffNPC
import uiGameOption
import uicrystalexchange
if app.__DUNGEON_INFO__:
	import uiDungeonTrack

if app.ENABLE_OFFLINE_SHOP_SYSTEM:
	import uiOfflineShopBuilder
	import uiOfflineShop
if app.ENABLE_ACCE_COSTUME_SYSTEM:
	import uiacce
if app.ENABLE_SWITCHBOT:
	import uiSwitchbot
import new_uiWeeklyRank
if app.ENABLE_DUNGEON_INFO_SYSTEM:
	import uiDungeonInfo
if app.ENABLE_NEW_PET_SYSTEM:
	import uiCompanion
if app.ENABLE_ARTEFAKT_SYSTEM:
	import uiartefaktsystem
if app.ENABLE_MINIMAP_DUNGEONINFO:
	import uiminimapdungeoninfo
import uiemeral_gauge
import uigorgon_gauge
import uicrimson_gauge
if app.__BL_CHEST_DROP_INFO__:
	import uiChestDropInfo
import uiCards
if app.ENABLE_COLLECTIONS_SYSTEM:
	import uiCollections
if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
	import uiPrivateShop
	import uiPrivateShopSearch
import chr
if app.ENABLE_SAVE_LOCATION_SYSTEM:
	import uiSaveLocation

import emoji
import uiBattlePass
if app.__ENABLE_POLYMORPH_SYSTEM__:
	import uiPolySystem
import uiRanking
if app.ENABLE_SECONDARY_LEVEL:
	import uiSecondaryLevel

if app.WORLD_BOSS_YUMA:
	import uiworldbosstext
if app.ENABLE_TITLE_ACHIEVEMENT_SYSTEM:
	import uititleachievement
if app.ENABLE_WIKI:
	import uiWiki
if app.__AUTO_SKILL_READER__:
	import uiAutoSkillReader

import uiteleport
import uiWorldBoss

IsQBHide = 0
class Interface(object):
	CHARACTER_STATUS_TAB = 1
	CHARACTER_SKILL_TAB = 2

	class NewGoldChat(ui.Window):
		def __init__(self, parent = None, x = 0, y = 0):
			ui.Window.__init__(self)
			self.texts = {}
			self.before_close = app.GetTime() + 5
			self.parent = parent
			self.SpaceBet = 14
			self.maxY = 0
			self.x = x-745
			self.y = y+1
			self.ColorValue = 0xFFFFFFFF
			self.Show()
			
		def GetMaxY(self):
			return self.maxY

		def AddGoldValue(self, text):
			for i in xrange(len(self.texts)):
				if len(self.texts) == 5 and i == 0:
					self.texts[i].Hide()
				x, y = self.texts[i].GetLocalPosition()
				self.texts[i].SetPosition(x, y-self.SpaceBet)

			i = 0
			if len(self.texts) == 5:
				for i in xrange(len(self.texts)-1):
					self.texts[i] = self.texts[i+1]
				i = 4
			else:
				i = len(self.texts)
			
			self.texts[i] = ui.TextLine()
			if self.parent != None:
				self.texts[i].SetParent(self.parent)
			self.texts[i].SetPosition(self.x, self.y)
			self.texts[i].SetPackedFontColor(grp.GenerateColor(255./255, 215./255, 76./255, 1))
			self.texts[i].SetHorizontalAlignLeft()
			self.texts[i].SetOutline(TRUE)
			self.texts[i].SetText(text)
			
			self.texts[i].Show()
			self.before_close = app.GetTime() + 10


		def ClearAll(self):
			self.Hide()
			self.texts = {}

		def OnRender(self):
			if len(self.texts) > 0:
				x, y = self.texts[0].GetGlobalPosition()
				w, h = self.texts[0].GetTextSize()
				grp.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.5))
				self.BOARD_START_COLOR = grp.GenerateColor(0.0, 0.0, 0.0, 0.0)
				self.BOARD_END_COLOR = grp.GenerateColor(0.0, 0.0, 0.0, 0.8)
				grp.RenderGradationBar(x, y+h-50, 150, h*8, self.BOARD_START_COLOR, self.BOARD_END_COLOR)
				
		def OnUpdate(self):
			if app.GetTime() >= self.before_close:
				for i in xrange(len(self.texts)):
					if len(self.texts) > 0 and i == 0:
						self.texts[i].Hide()
						self.texts = {}
					if i == 0:
						return
					x, y = self.texts[i].GetLocalPosition()
					self.texts[i].SetPosition(x, y-self.SpaceBet)

				i = 0
				if len(self.texts) == 5:
					for i in xrange(len(self.texts)-1):
						self.texts[i] = self.texts[i+1]
					i = 4
				else:
					i = len(self.texts)
				


	def __init__(self):
		systemSetting.SetInterfaceHandler(self)
		self.windowOpenPosition = 0
		if app.__SPIN_WHEEL__:
			self.wndSpinWheel = None
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.onTopWindow = player.ON_TOP_WND_NONE
		if app.__DUNGEON_INFO__:
			self.wndDungeonTrack = None
			self.wndDungeonManager = None

		if app.__AUTO_SKILL_READER__:
			self.wndAutoSkillReader = None
		self.dlgWhisperWithoutTarget = None
		self.inputDialog = None
		self.tipBoard = None
		self.bigBoard = None
		if app.ENABLE_WHEEL_OF_FORTUNE:
			self.wndWheelofFortune=None
		if app.ENABLE_WIKI:
			self.wndWiki = None
		
		if app.ENABLE_ITEMSHOP:
			self.wndItemShop=None

		self.mallPageDlg = None
		if app.ENABLE_AUTO_SELECT_SKILL:
			self.wndAutoSkill = None
		if app.ENABLE_EVENT_MANAGER:
			self.wndEventManager = None
			self.wndEventIcon = None
			self.wndPersonalEventTracker = None
		if app.ENABLE_TITLE_ACHIEVEMENT_SYSTEM:
			self.wndTitleAchievement = None
		self.wndWeb = None
		if app.ENABLE_CUBE_RENEWAL_WORLDARD:
			self.wndCubeRenewal = None
		self.wndTaskBar = None
		self.wndCharacter = None
		self.wndInventory = None
		self.wndExpandedTaskBar = None
		self.wndDragonSoul = None
		self.wndDragonSoulRefine = None
		if app.ENABLE_PVP_RANKING:
			self.wndPvPRanking = None
			self.wndPvPRankingAdmin = None
		self.wndChat = None
		self.yangText = None
		self.wndMessenger = None
		self.wndMiniMap = None
		self.wndGuild = None
		if app.ENABLE_SECONDARY_LEVEL:
			self.wndSecondaryLevel = None
		if app.__BL_CHEST_DROP_INFO__:
			self.wndChestDropInfo = None
		if app.ENABLE_COLLECTIONS_SYSTEM:
			self.wndCollections = None
		self.wndGuildBuilding = None
		
		if app.ENABLE_VOTE4BUFF:
			self.btnVote4Buff = uivote4buff.Vote4BuffButton(self)

			self.wndVote4Buff = uivote4buff.Vote4BuffWindow()
			self.wndVote4Buff.BindInterface(self)

		if app.ENABLE_OFFLINE_SHOP:
			self.wndShopOffline = None
			self.wndShopSearch = None
			self.wndShopNotification = None
		self.wndCasketPreview = None
		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			self.wndDungeonInfo = None
		self.wndRemoveItem = None
		if app.WJ_SPLIT_INVENTORY_SYSTEM:
			self.wndExtendedInventory = None
		if app.ENABLE_RESP_SYSTEM:
			self.wndResp = None
		if app.__ENABLE_POLYMORPH_SYSTEM__:
			self.wndPolySystem = None

		self.wndBonus = None
		self.wndBlend = None
		self.wndCollectWindow = None
		self.wndWeeklyRankWindow_New = None
		if app.ENABLE_SWITCHBOT:
			self.wndSwitchbot = None
		self.wndgameOption = None

		if app.ENABLE_LINK_IN_CHAT:
			self.OpenLinkQuestionDialog = None
			
		self.wndBattlePassButton = None
		self.wndBattlePass = None
		

		if app.ENABLE_ODLAMKI_SYSTEM:
			self.wndOdlamki = None
		if app.TAKE_LEGEND_DAMAGE_BOARD_SYSTEM:
			self.wndLegendDamageWindow = None

		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			self.wndOfflineShopAdminPanel = None
			self.wndOfflineShopLogPanel = None
			self.offlineShopAdvertisementBoardDict = {}

		if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
			self.wndPrivateShopPanel		= None
			self.wndPrivateShopSearch		= None
			self.privateShopTitleBoardDict = {}
		self.listGMName = {}
		self.wndQuestWindow = {}
		self.wndQuestWindowNewKey = 0
		if app.ENABLE_MINIMAP_DUNGEONINFO:
			self.wndMiniMapDungeonInfo = None
		self.wndEmeraldGauge = None
		self.wndGorgonGauge = None
		self.wndCrimsonGauge = None
		if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
			self.wndBuffNPCWindow = None
			self.wndBuffNPCCreateWindow = None
			
		self.wndCrystalExchangeWindow = None

		if app.ENABLE_VS_SHOP_SEARCH:
			self.wndOfflineShopSearch = None

		self.privateShopAdvertisementBoardDict = {}
		self.guildScoreBoardDict = {}
		self.equipmentDialogDict = {}
		if app.ENABLE_LOADING_PERFORMANCE:
			self.wndWarpShower = None
		
		self.messenger_bg = None
		self.messenger = None
		
		if app.WORLD_BOSS_YUMA:
			self.WorldbossHwnd = {}

		if app.__AUTO_SKILL_READER__ and self.wndAutoSkillReader:
			self.wndAutoSkillReader.Hide()
			self.wndAutoSkillReader.Destroy()
			self.wndAutoSkillReader = None

		if app.__DUNGEON_INFO__:
			if self.wndDungeonTrack:
				self.wndDungeonTrack.Close()
				self.wndDungeonTrack.Destroy()
				self.wndDungeonTrack = None
			if self.wndDungeonManager:
				self.wndDungeonManager.Destroy()
				self.wndDungeonManager = None
		
		if constInfo.ENABLE_EXPANDED_MONEY_TASKBAR:
			self.wndExpandedMoneyTaskBar = None
		
		if app.ENABLE_NOTIFICATION_SYSTEM:
			self.wndNotificationManager = None

		event.SetInterfaceWindow(self)
		
		self.interfaceWindowList = {}

	def __del__(self):
		systemSetting.DestroyInterfaceHandler()
		event.SetInterfaceWindow(None)

	def AppendQueue(self, vnum, value):
		name = chr.GetNameByVID(vnum)
		if value == 1:
			self.yangText.AddGoldValue("|cFFe4ec98"+emoji.AppendEmoji("icon/emoji/sell.png")+" "+str(name))
		elif value == 0:
			self.yangText.AddGoldValue("|cFFf8a6a6"+emoji.AppendEmoji("icon/emoji/buy.png")+" "+str(name))

	def __MakeUICurtain(self):
		wndUICurtain = ui.Bar("TOP_MOST")
		wndUICurtain.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		wndUICurtain.SetColor(0x77000000)
		wndUICurtain.Hide()
		self.wndUICurtain = wndUICurtain

	def __MakeMessengerWindow(self):
		self.wndMessenger = uiMessenger.MessengerWindow()

		from _weakref import proxy
		self.wndMessenger.SetWhisperButtonEvent(lambda n,i=proxy(self):i.OpenWhisperDialog(n))
		self.wndMessenger.SetGuildButtonEvent(ui.__mem_func__(self.ToggleGuildWindow))

	def __MakeGuildWindow(self):
		self.wndGuild = uiGuild.GuildWindow()

	def __MakeChatWindow(self):

		wndChat = uiChat.ChatWindow()

		wndChat.SetSize(wndChat.CHAT_WINDOW_WIDTH, 0)
		wndChat.SetPosition(wndMgr.GetScreenWidth()/2 - wndChat.CHAT_WINDOW_WIDTH/2, wndMgr.GetScreenHeight() - wndChat.EDIT_LINE_HEIGHT - 50)
		wndChat.SetHeight(200)
		wndChat.Refresh()
		wndChat.Show()

		yangText = self.NewGoldChat(None, wndMgr.GetScreenWidth()/2 - wndChat.CHAT_WINDOW_WIDTH/2 + 540, wndMgr.GetScreenHeight() - wndChat.EDIT_LINE_HEIGHT - 50)
		self.yangText = yangText

		self.wndChat = wndChat
		self.wndChat.BindInterface(self)
		self.wndChat.SetSendWhisperEvent(ui.__mem_func__(self.OpenWhisperDialogWithoutTarget))
		self.wndChat.SetOpenChatLogEvent(ui.__mem_func__(self.ToggleChatLogWindow))
		self.wndChat.SetOpenChatEvent(ui.__mem_func__(self.__OnChatOpen))
		self.wndChat.SetCloseChatEvent(ui.__mem_func__(self.__OnChatClose))
		self.wndChat.SetChatResizeEvent(ui.__mem_func__(self.__OnChatOpen))

		if app.ENABLE_AUTO_SHOUT:
			self.wndChat.SetAutoShoutEvent(ui.__mem_func__(self.AutoShoutButton))

	def OnPickMoneyNew(self, money):
		self.yangText.AddGoldValue("|cFFffd74c"+emoji.AppendEmoji("icon/emoji/money_icon_small.png")+" "+"+%s"%(localeInfo.NumberToGoldNotText(money)))

	def OnPickChequeNew(self, money):
		self.yangText.AddGoldValue("|cFFb8b8b8"+emoji.AppendEmoji("icon/emoji/cheque_icon.png")+" "+"+%s"%(localeInfo.NumberToGoldNotText(money)))

	def OnPickPktOsiagNew(self, money):
		self.yangText.AddGoldValue("|cFFbfa66a"+emoji.AppendEmoji("icon/emoji/achievement_point.tga")+" "+"+%s"%(localeInfo.NumberToGoldNotText(money)))

	def __MakeTaskBar(self):
		wndTaskBar = uiTaskBar.TaskBar()
		wndTaskBar.LoadWindow()
		self.wndTaskBar = wndTaskBar
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_CHARACTER, ui.__mem_func__(self.ToggleCharacterWindowStatusPage))
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_INVENTORY, ui.__mem_func__(self.ToggleInventoryWindow))
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_MESSENGER, ui.__mem_func__(self.ToggleMessenger))
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_SYSTEM, ui.__mem_func__(self.ToggleSystemDialog))
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_ANTIEXP, ui.__mem_func__(self.ToggleAntyExp))
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_ITEM_SHOP, ui.__mem_func__(self.OpenItemShopMainWindow))
		#if uiTaskBar.TaskBar.IS_EXPANDED:
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_EXPAND, ui.__mem_func__(self.ToggleExpandedButton))
		self.wndExpandedTaskBar = uiTaskBar.ExpandedTaskBar()
		self.wndExpandedTaskBar.LoadWindow()
		self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_POTIONS, ui.__mem_func__(self.OpenBlendWindow))
		self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_OFFLINESHOP, ui.__mem_func__(self.OpenOfflineShopWindow))
		self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_COMPANION, ui.__mem_func__(self.pet_window_open))
		self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_SHOPSEARCH, ui.__mem_func__(self.ToggleSearchShop))
		self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_BONUSES, ui.__mem_func__(self.ToggleBonusWindow))
		self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_SWITCHBOT, ui.__mem_func__(self.ToggleSwitchbotWindow))
		self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_AUTOBUFF, ui.__mem_func__(self.BuffNPCOpenWindow))
		self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_RANKING, ui.__mem_func__(self.ToggleWeeklyRankWindow))
		self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_BIN, ui.__mem_func__(self.OpenRemoveItem))
		self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_RUNE, ui.__mem_func__(self.OpenRuneWindow))
		self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_ARTEFAKT, ui.__mem_func__(self.OpenArtefaktWindow))
		self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_CHANGEEQUIPMENT, ui.__mem_func__(self.OpenChangeEquipmentWindow))
		self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_STORAGE, ui.__mem_func__(self.OpenStorageWindow))
		self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_SAVELOCATION, ui.__mem_func__(self.ToggleSaveLocationWindow))
		self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_WIKI, ui.__mem_func__(self.OpenWikiWindow))

		#else:
		#	self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_CHAT, ui.__mem_func__(self.ToggleChat))

		self.wndEnergyBar = None
		import app
		if app.ENABLE_ENERGY_SYSTEM:
			wndEnergyBar = uiTaskBar.EnergyBar()
			wndEnergyBar.LoadWindow()
			self.wndEnergyBar = wndEnergyBar
			
		if constInfo.ENABLE_EXPANDED_MONEY_TASKBAR:
			self.wndExpandedMoneyTaskBar = uiTaskBar.ExpandedMoneyTaskBar()
			self.wndExpandedMoneyTaskBar.LoadWindow()
			if self.wndInventory:
				self.wndInventory.SetExpandedMoneyBar(self.wndExpandedMoneyTaskBar)

		if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
			self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_OFFLINE_SHOP, ui.__mem_func__(self.TogglePrivateShopPanelWindowCheck))

	def __MakeParty(self):
		wndParty = uiParty.PartyWindow()
		wndParty.Hide()
		self.wndParty = wndParty

	def __MakeGameButtonWindow(self):
		wndGameButton = uiGameButton.GameButtonWindow()
		wndGameButton.SetTop()
		wndGameButton.Show()
		wndGameButton.SetButtonEvent("STATUS", ui.__mem_func__(self.__OnClickStatusPlusButton))
		wndGameButton.SetButtonEvent("QUEST", ui.__mem_func__(self.__OnClickQuestButton))
		wndGameButton.SetButtonEvent("BUILD", ui.__mem_func__(self.__OnClickBuildButton))

		self.wndGameButton = wndGameButton

	def __IsChatOpen(self):
		return True

	def __MakeWindows(self):
		wndCharacter = uiCharacter.CharacterWindow()
		wndInventory = uiInventory.InventoryWindow()
		wndInventory.BindInterfaceClass(self)
		if app.WJ_SPLIT_INVENTORY_SYSTEM:
			wndExtendedInventory = uiInventory.ExtendedInventoryWindow()
			wndExtendedInventory.BindInterfaceClass(self)
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			wndDragonSoul = uiDragonSoul.DragonSoulWindow()
			wndDragonSoul.BindInterfaceClass(self)
			wndDragonSoulRefine = uiDragonSoul.DragonSoulRefineWindow()
		else:
			wndDragonSoul = None
			wndDragonSoulRefine = None

		wndMiniMap = uiMiniMap.MiniMap()
		wndSafebox = uiSafebox.SafeboxWindow()
		if app.WJ_ENABLE_TRADABLE_ICON:
			wndSafebox.BindInterface(self)
		wndSafebox.BindInterface(self)
		if app.ENABLE_SWITCHBOT:
			self.wndSwitchbot = uiSwitchbot.SwitchbotWindow()
		self.wndCasketPreview = uiCasket.CasketDialog()
		self.wndBonus = uiBonus.BonusWindow()
		self.wndWeeklyRankWindow_New = new_uiWeeklyRank.WeeklyRankWindow()
		self.wndCollectWindow = uiCollectWindow.CollectWindow()
		self.wndBlend = uiBlend.AutoDopalacze()
		wndMall = uiSafebox.MallWindow()
		self.wndMall = wndMall

		self.wndWonExchange = uiWonExchange.WonExchangeWindow()
		self.wndWonExchange.BindInterface(self)
		


		if app.ENABLE_ODLAMKI_SYSTEM:
			wndOdlamki = uiOdlamki.OdlamkiWindow()
			wndOdlamki.BindInterfaceClass(self)
			self.wndOdlamki = wndOdlamki

		wndChatLog = uiChat.ChatLogWindow()
		wndChatLog.BindInterface(self)

		if app.ENABLE_OFFLINE_SHOP:
			self.wndShopOffline = uiOfflineShop.OfflineShopWindow()
			self.wndShopOffline.Hide()

			self.wndShopSearch = uiShopSearch.ShopSearch()
			self.wndShopSearch.Hide()

			self.wndShopNotification = uiOfflineShop.SoldNotification()
			self.wndShopNotification.Hide()

		self.wndCharacter = wndCharacter
		self.wndInventory = wndInventory
		self.wndDragonSoul = wndDragonSoul
		self.wndDragonSoulRefine = wndDragonSoulRefine
		if app.WJ_SPLIT_INVENTORY_SYSTEM:
			self.wndExtendedInventory = wndExtendedInventory
		self.wndMiniMap = wndMiniMap
		self.wndSafebox = wndSafebox
		self.wndChatLog = wndChatLog
		if app.ENABLE_NOTIFICATION_SYSTEM:
			self.wndNotificationManager = uiNotificationmanager.NotificationWindow()
		self.wndWorldBoss = uiWorldBoss.WorldBossController()
		if app.__ENABLE_POLYMORPH_SYSTEM__:
			self.wndPolySystem = uiPolySystem.PolySystemWindow()
		if app.ENABLE_LOADING_PERFORMANCE:
			self.wndWarpShower = uiWarpShower.WarpShowerWindow()
		if app.ENABLE_COLLECTIONS_SYSTEM:
			self.wndCollections = uiCollections.CollectionsWindow()
		if app.__BL_CHEST_DROP_INFO__:
			self.wndChestDropInfo = uiChestDropInfo.ChestDropInfoWindow()
		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			self.wndDungeonInfo = uiDungeonInfo.DungeonInfoWindow()
			self.wndMiniMap.BindInterfaceClass(self)

		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.SetDragonSoulRefineWindow(self.wndDragonSoulRefine)
			self.wndDragonSoulRefine.SetInventoryWindows(self.wndInventory, self.wndDragonSoul)
			self.wndInventory.SetDragonSoulRefineWindow(self.wndDragonSoulRefine)

		if app.ENABLE_NEW_PET_SYSTEM:
			self.wndCompanionWindow = uiCompanion.CompanionWindow(self.wndInventory)
			self.wndCompanionWindow.Hide()

		if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
			self.wndBuffNPCWindow = uiBuffNPC.BuffNPCWindow()
			self.wndBuffNPCCreateWindow = uiBuffNPC.BuffNPCCreateWindow()
			
		self.wndCrystalExchangeWindow = uicrystalexchange.CrystalExchangeWindow()

		if app.ENABLE_VS_SHOP_SEARCH:
			self.wndOfflineShopSearch = uiShopSearch.SearchWindow()
			
		if app.ENABLE_PVP_RANKING:
			self.wndPvPRanking = uipvpranking.RankingWindow()
			self.wndPvPRankingAdmin = uipvpranking.AdminWindow()
			
		self.wndgameOption = uiGameOption.OptionDialog()

		if app.ENABLE_ARTEFAKT_SYSTEM:
			self.wndArtefaktWindow = uiartefaktsystem.ArtefaktWindow()
			self.wndArtefaktWindow.Hide()

		if app.TAKE_LEGEND_DAMAGE_BOARD_SYSTEM:
			self.wndLegendDamageWindow = uiLegendDamageWindow.LegendDamageWindow()
			self.wndLegendDamageWindow.Hide()
			self.wndLegendDamageWindow.BindInterfaceClass(self)
			
		if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
			self.wndPrivateShopPanel = uiPrivateShop.PrivateShopPanel()
			self.wndPrivateShopPanel.BindInterfaceClass(self)
			self.wndPrivateShopPanel.BindInventoryClass(self.wndInventory)
			self.wndPrivateShopPanel.BindDragonSoulInventoryClass(self.wndDragonSoul)

			self.wndDragonSoul.BindPrivateShopClass(self.wndPrivateShopPanel)
			self.wndDragonSoul.BindPrivateShopSearchClass(self.wndPrivateShopSearch)
			
			self.wndPrivateShopSearch = uiPrivateShopSearch.PrivateShopSeachWindow()
			self.wndPrivateShopSearch.BindInterfaceClass(self)
			
			self.wndInventory.BindWindow(self.wndPrivateShopPanel)
			self.wndInventory.BindPrivateShopClass(self.wndPrivateShopPanel)
			self.wndInventory.BindPrivateShopSearchClass(self.wndPrivateShopSearch)

	if app.ENABLE_NEW_PET_SYSTEM:
		def pet_refresh(self):
			if self.wndCompanionWindow:
				self.wndCompanionWindow.PetReload()
				
		def pet_window_open(self):
			_obj = self.wndCompanionWindow
			if not _obj:
				return
				
			_obj.Show() if not _obj.IsShow() else _obj.Close()

	def __MakeDialogs(self):
		self.dlgExchange = uiExchange.ExchangeDialog()
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.dlgExchange.BindInterface(self)
			self.dlgExchange.SetInven(self.wndInventory)
			self.wndInventory.BindWindow(self.dlgExchange)
		self.dlgExchange.LoadDialog()
		self.dlgExchange.SetCenterPosition()
		self.dlgExchange.Hide()

		self.dlgPointReset = uiPointReset.PointResetDialog()
		self.dlgPointReset.LoadDialog()
		self.dlgPointReset.Hide()

		self.dlgShop = uiShop.ShopDialog()
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.dlgShop.BindInterface(self)
		self.dlgShop.LoadDialog()
		self.dlgShop.BindInterface(self)
		self.dlgShop.Hide()
		
		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			self.dlgOfflineShop = uiOfflineShop.OfflineShopDialog()
			self.dlgOfflineShop.LoadDialog()
			self.dlgOfflineShop.BindInterfaceClass(self)
			self.dlgOfflineShop.Hide()

			self.offlineShopBuilder = uiOfflineShopBuilder.OfflineShopBuilder()
			if app.WJ_ENABLE_TRADABLE_ICON:
				self.offlineShopBuilder.BindInterface(self)
				self.offlineShopBuilder.SetInven(self.wndInventory)
				self.wndInventory.BindWindow(self.offlineShopBuilder)
			self.offlineShopBuilder.Hide()	

		if app.ENABLE_MINIMAP_DUNGEONINFO:
			self.wndMiniMapDungeonInfo = uiminimapdungeoninfo.MiniMapDungeonInfo()

		self.dlgRestart = uiRestart.RestartDialog()
		self.dlgRestart.LoadDialog()
		self.dlgRestart.Hide()

		self.dlgSystem = uiSystem.SystemDialog()
		self.dlgSystem.LoadDialog()
		self.dlgSystem.SetOpenHelpWindowEvent(ui.__mem_func__(self.OpenHelpWindow))

		self.dlgSystem.Hide()

		self.dlgPassword = uiSafebox.PasswordDialog()
		self.dlgPassword.Hide()
		
		if app.ENABLE_SECONDARY_LEVEL:
			self.wndSecondaryLevel = uiSecondaryLevel.SecondaryLevelWindow()

		self.hyperlinkItemTooltip = uiToolTip.HyperlinkItemToolTip()
		self.hyperlinkItemTooltip.Hide()

		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.Hide()

		self.tooltipSkill = uiToolTip.SkillToolTip()
		self.tooltipSkill.Hide()

		self.privateShopBuilder = uiPrivateShopBuilder.PrivateShopBuilder()
		self.privateShopBuilder.Hide()

		self.dlgRefineNew = uiRefine.RefineDialogNew()
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.dlgRefineNew.SetInven(self.wndInventory)
			self.wndInventory.BindWindow(self.dlgRefineNew)
		self.dlgRefineNew.Hide()
		
		if app.ENABLE_TITLE_ACHIEVEMENT_SYSTEM:
			self.wndTitleAchievement = ui.wndTitleAchievement = uititleachievement.TitleAchievementWindow()
		
		self.wndRemoveItem = uiremoveitem.RemoveItemDialog()
		self.wndRemoveItem.Hide()
		
		if app.ENABLE_RESP_SYSTEM:
			self.wndResp = uiresp.RespDialog()
			self.wndResp.Hide()

		self.wndBattlePass = uiBattlePass.BattlePassWindow()
		self.wndBattlePass.Hide()
		
		self.wndRankingWindow = uiRanking.RankingWindow()
		self.wndRankingWindowWeekly = uiRanking.WeeklyRankingWindow()

	def __MakeHelpWindow(self):
		self.wndHelp = uiHelp.HelpWindow()
		self.wndHelp.LoadDialog()
		self.wndHelp.SetCloseEvent(ui.__mem_func__(self.CloseHelpWindow))
		self.wndHelp.Hide()

	def __MakeTipBoard(self):
		self.tipBoard = uiTip.TipBoard()
		self.tipBoard.Hide()

		self.bigBoard = uiTip.BigBoard()
		self.bigBoard.Hide()

	def __MakeWebWindow(self):
		if constInfo.IN_GAME_SHOP_ENABLE:
			import uiWeb
			self.wndWeb = uiWeb.WebWindow()
			self.wndWeb.LoadWindow()
			self.wndWeb.Hide()

	def __MakeCubeWindow(self):
		self.wndCube = uiCube.CubeWindow()
		self.wndCube.Hide()

	if constInfo.GIFT_CODE_SYSTEM:
		def __MakeGiftCodeWindow(self):
			self.wndGiftCodeWindow = uigiftcodewindow.UiGiftCodeWindow()
			self.wndGiftCodeWindow.LoadWindow()
			self.wndGiftCodeWindow.Hide()
		
	def __MakeChangerWindow(self):
		self.wndChangerWindow = uiBonusChanger.ChangerWindow()
		self.wndChangerWindow.LoadWindow()
		self.wndChangerWindow.Hide()

	def __MakeCardsInfoWindow(self):
		self.wndCardsInfo = uiCards.CardsInfoWindow()
		self.wndCardsInfo.LoadWindow()
		self.wndCardsInfo.Hide()
		
	def __MakeCardsWindow(self):
		self.wndCards = uiCards.CardsWindow()
		self.wndCards.LoadWindow()
		self.wndCards.Hide()
		
	def __MakeCardsIconWindow(self):
		self.wndCardsIcon = uiCards.IngameWindow()
		self.wndCardsIcon.LoadWindow()
		self.wndCardsIcon.Hide()

	def __MakeIsIconWindow(self):
		self.wndIsIcon = uiIkonki.IsIngameIcon()
		self.wndIsIcon.BindInterfaceClass(self)
		self.wndIsIcon.LoadWindow()
		self.wndIsIcon.Show()

	if app.ENABLE_ACCE_COSTUME_SYSTEM:
		def __MakeAcceWindow(self):
			self.wndAcceCombine = uiacce.CombineWindow()
			self.wndAcceCombine.LoadWindow()
			self.wndAcceCombine.Hide()

			self.wndAcceAbsorption = uiacce.AbsorbWindow()
			self.wndAcceAbsorption.LoadWindow()
			self.wndAcceAbsorption.Hide()

			if self.wndInventory:
				self.wndInventory.SetAcceWindow(self.wndAcceCombine, self.wndAcceAbsorption)

		def AcceMaterials(self, id, vnum, count):
			self.wndAcceCombine.SendMaterials(id, vnum, count)

	if app.ENABLE_AURA_SYSTEM:
		def __MakeAuraWindow(self):
			self.wndAuraRefine = uiaura.RefineWindow(self.wndInventory, self)
			self.wndAuraRefine.LoadWindow()
			self.wndAuraRefine.Hide()
	
			self.wndAuraAbsorption = uiaura.AbsorbWindow(self.wndInventory, self)
			self.wndAuraAbsorption.LoadWindow()
			self.wndAuraAbsorption.Hide()
			
			if self.wndInventory:
				self.wndInventory.SetAuraWindow(self.wndAuraAbsorption, self.wndAuraRefine)

	def __MakeItemSelectWindow(self):
		self.wndItemSelect = uiSelectItem.SelectItemWindow()
		self.wndItemSelect.Hide()

	if app.ENABLE_CUBE_RENEWAL_WORLDARD:
		def __MakeCubeRenewal(self):
			import uicuberenewal
			self.wndCubeRenewal = uicuberenewal.CubeRenewalWindows()
			self.wndCubeRenewal.Hide()

	if app.ENABLE_HIDE_COSTUME_SYSTEM:
		def RefreshVisibleCostume(self):
			self.wndInventory.RefreshVisibleCostume()

	def MakeInterface(self):
		self.__MakeMessengerWindow()
		self.__MakeGuildWindow()
		self.__MakeChatWindow()
		self.__MakeParty()
		
		self.__MakeIsIconWindow()

		self.__MakeWindows()
		self.__MakeDialogs()

		self.__MakeUICurtain()
		self.__MakeTaskBar()
		self.__MakeGameButtonWindow()
		self.__MakeHelpWindow()
		self.__MakeTipBoard()


		if app.ENABLE_OFFLINE_SHOP:
			self.wndShopOffline.SetItemToolTip(self.tooltipItem)
			self.wndShopOffline.BindInterface(self)
			self.wndShopSearch.SetItemToolTip(self.tooltipItem)
			self.wndShopSearch.BindInterface(self)
		if app.ENABLE_CUBE_RENEWAL_WORLDARD:
			self.__MakeCubeRenewal()
		self.__MakeWebWindow()
		self.__MakeCubeWindow()
		if app.ENABLE_WIKI:
			self.__MakeWiki()
		if constInfo.GIFT_CODE_SYSTEM:
			self.__MakeGiftCodeWindow()

		if app.ENABLE_SAVE_LOCATION_SYSTEM:
			self.__MakeSaveLocationWindow()
		self.__MakeChangerWindow()
		self.__MakeCardsInfoWindow()
		self.__MakeCardsWindow()
		self.__MakeCardsIconWindow()
		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			self.__MakeAcceWindow()
		if app.ENABLE_AURA_SYSTEM:
			self.__MakeAuraWindow()

		self.__MakeItemSelectWindow()

		if app.ENABLE_OFFLINE_SHOP:
			self.wndShopOffline.SetItemToolTip(self.tooltipItem)
			self.wndShopOffline.BindInterface(self)
			self.wndShopSearch.SetItemToolTip(self.tooltipItem)
			self.wndShopSearch.BindInterface(self)

		self.questButtonList = []
		self.whisperButtonList = []
		self.whisperDialogDict = {}
		self.privateShopAdvertisementBoardDict = {}

		self.wndInventory.SetItemToolTip(self.tooltipItem)
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.SetItemToolTip(self.tooltipItem)
			self.wndDragonSoulRefine.SetItemToolTip(self.tooltipItem)
		self.wndSafebox.SetItemToolTip(self.tooltipItem)
		if app.ENABLE_SWITCHBOT:
			self.wndSwitchbot.SetItemToolTip(self.tooltipItem)

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			self.wndAcceCombine.SetItemToolTip(self.tooltipItem)
			self.wndAcceAbsorption.SetItemToolTip(self.tooltipItem)

		if app.ENABLE_AURA_SYSTEM:
			self.wndAuraAbsorption.SetItemToolTip(self.tooltipItem)
			self.wndAuraRefine.SetItemToolTip(self.tooltipItem)
		if app.ENABLE_SECONDARY_LEVEL:
			self.wndSecondaryLevel.SetItemToolTip(self.tooltipItem)

		self.wndMall.SetItemToolTip(self.tooltipItem)

		self.wndCharacter.SetSkillToolTip(self.tooltipSkill)
		self.wndTaskBar.SetItemToolTip(self.tooltipItem)
		self.wndTaskBar.SetSkillToolTip(self.tooltipSkill)
		self.wndGuild.SetSkillToolTip(self.tooltipSkill)
		self.wndGuild.SetItemToolTip(self.tooltipItem)

		if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
			self.wndBuffNPCWindow.SetSkillToolTip(self.tooltipSkill)

		self.wndItemSelect.SetItemToolTip(self.tooltipItem)
		if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
			self.wndPrivateShopPanel.SetItemToolTip(self.tooltipItem)
			self.wndPrivateShopSearch.SetItemToolTip(self.tooltipItem)
			self.privateShopTitleBoardDict = {}
		self.dlgShop.SetItemToolTip(self.tooltipItem)
		self.dlgExchange.SetItemToolTip(self.tooltipItem)
		self.privateShopBuilder.SetItemToolTip(self.tooltipItem)
		self.wndRemoveItem.SetItemToolTip(self.tooltipItem)
		if app.__ENABLE_POLYMORPH_SYSTEM__:
			self.wndPolySystem.SetToolTip(self.tooltipItem)
		if app.ENABLE_TITLE_ACHIEVEMENT_SYSTEM:
			self.wndTitleAchievement.SetToolTip(self.tooltipItem)
		if app.ENABLE_ODLAMKI_SYSTEM:
			self.wndOdlamki.SetItemToolTip(self.tooltipItem)
		if app.WJ_SPLIT_INVENTORY_SYSTEM:
			self.wndExtendedInventory.SetItemToolTip(self.tooltipItem)
		if app.ENABLE_RESP_SYSTEM:
			self.wndResp.SetItemToolTip(self.tooltipItem)
		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			self.dlgOfflineShop.SetItemToolTip(self.tooltipItem)
			self.offlineShopBuilder.SetItemToolTip(self.tooltipItem)

		self.__InitWhisper()

		self.messenger_bg = ui.ImageBox()
		self.messenger_bg.LoadImage("zaris/whisper/message_button.png")
		self.messenger_bg.SetPosition(wndMgr.GetScreenWidth() - 75, 278)
		self.messenger_bg.Hide()

		self.messenger = uiWhisper.WhisperButton()
		if constInfo.MESSENGER_NEWS_LIST:
			self.messenger.SetUpVisual("zaris/whisper/new_message_alert.png")
			self.messenger.SetOverVisual("zaris/whisper/new_message_alert.png")
			self.messenger.SetDownVisual("zaris/whisper/new_message_alert.png")
			#if app.FLASH_IN_TASKBAR:
			#	app.FlashApplication()
		else:
			self.SetMessengerReaded()
		self.messenger.SetToolTipText(localeInfo.WHISPER_BUTTON_OPEN)
		self.messenger.ToolTipText.SetHorizontalAlignCenter()
		self.messenger.SetEvent(ui.__mem_func__(self.OpenWhisperDialogWithoutTarget))
		self.messenger.SetPosition(wndMgr.GetScreenWidth() - 75, 278)
		self.messenger.Show()
		
		self.__InitializeWindow()
  
	def __InitializeWindow(self):
     
		import uiteleport
		self.wndTeleport = uiteleport.TeleportMap()
		self.wndTeleport.Hide()
  
		self.AppendInterfaceWindow("teleport", self.wndTeleport)
  
	def AppendInterfaceWindow(self, name, window):
		self.interfaceWindowList[name] = window
  
	def ToggleMapWindow(self):
		self.interfaceWindowList["teleport"].toggle()
 
	def MakeHyperlinkTooltip(self, hyperlink):
		tokens = hyperlink.split(":")
		if tokens and len(tokens):
			type = tokens[0]
			if "item" == type:
				self.hyperlinkItemTooltip.SetHyperlinkItem(tokens)
			elif "szept" == type:	
				self.OpenWhisperDialog(str(tokens[1]))

			elif app.ENABLE_LINK_IN_CHAT and "web" == type and (tokens[1].startswith("httpXxX") or tokens[1].startswith("httpsXxX")):
					url = tokens[1].replace("XxX", "://")
					if url.startswith("http://") or url.startswith("https://"):
						OpenLinkQuestionDialog = uiCommon.QuestionDialog2()
						OpenLinkQuestionDialog.SetText1(localeInfo.CHAT_OPEN_LINK_DANGER)
						OpenLinkQuestionDialog.SetText2(localeInfo.CHAT_OPEN_LINK)
						OpenLinkQuestionDialog.SetAcceptEvent(lambda arg=True: self.AnswerOpenLink(arg))
						OpenLinkQuestionDialog.SetCancelEvent(lambda arg=False: self.AnswerOpenLink(arg))
						constInfo.link = url
						OpenLinkQuestionDialog.Open()
						self.OpenLinkQuestionDialog = OpenLinkQuestionDialog
			elif app.ENABLE_LINK_IN_CHAT and "sysweb" == type:
				url = tokens[1].replace("XxX", "://")
				if url.startswith("http://") or url.startswith("https://"):
					webbrowser.open(url)


	if app.ENABLE_LINK_IN_CHAT:
		def AnswerOpenLink(self, answer):
			if not self.OpenLinkQuestionDialog:
				return

			self.OpenLinkQuestionDialog.Close()
			self.OpenLinkQuestionDialog = None

			if not answer:
				return

			link = constInfo.link
			webbrowser.open(link)


	def Close(self):
		if app.ENABLE_WHEEL_OF_FORTUNE:
			if self.wndWheelofFortune:
				self.wndWheelofFortune.Hide()
				self.wndWheelofFortune.Destroy()
				self.wndWheelofFortune=None
		if app.__SPIN_WHEEL__:
			if self.wndSpinWheel:
				self.wndSpinWheel.Close()
				self.wndSpinWheel.Destroy()
				self.wndSpinWheel = None
		if app.ENABLE_EVENT_MANAGER:
			if self.wndEventManager:
				self.wndEventManager.Hide()
				self.wndEventManager.Destroy()
				self.wndEventManager = None

			if self.wndEventIcon:
				self.wndEventIcon.Hide()
				self.wndEventIcon.Destroy()
				self.wndEventIcon = None

			if self.wndPersonalEventTracker:
				self.wndPersonalEventTracker.Hide()
				self.wndPersonalEventTracker.Destroy()
				self.wndPersonalEventTracker = None

		if app.ENABLE_AUTO_SELECT_SKILL:
			if self.wndAutoSkill:
				self.wndAutoSkill.Hide()
				self.wndAutoSkill.Destroy()
				self.wndAutoSkill=None

		if app.ENABLE_SECONDARY_LEVEL:
			if self.wndSecondaryLevel:
				self.wndSecondaryLevel.Destroy()
	
		if app.ENABLE_ITEMSHOP:
			if self.wndItemShop:
				self.wndItemShop.Hide()
				self.wndItemShop.Destroy()
				self.wndItemShop = None
				
		if self.wndRankingWindow:
			self.wndRankingWindow.Destroy()

		if self.wndRankingWindowWeekly:
			self.wndRankingWindowWeekly.Destroy()
				
		if self.dlgWhisperWithoutTarget:
			self.dlgWhisperWithoutTarget.Destroy()
			del self.dlgWhisperWithoutTarget

		if uiQuest.QuestDialog.__dict__.has_key("QuestCurtain"):
			uiQuest.QuestDialog.QuestCurtain.Close()

		if self.wndQuestWindow:
			for key, eachQuestWindow in self.wndQuestWindow.items():
				eachQuestWindow.nextCurtainMode = -1
				eachQuestWindow.CloseSelf()
				eachQuestWindow = None
		self.wndQuestWindow = {}
		
		if app.ENABLE_WIKI:
			if self.wndWiki:
				self.wndWiki.Close()
				self.wndWiki.Destroy()
				self.wndWiki=None

		if self.wndChat:
			self.wndChat.hide_btnChatSizing()
			self.wndChat.Destroy()

		if self.wndTaskBar:
			self.wndTaskBar.Destroy()

		if self.yangText:
			self.yangText.ClearAll()

		if self.wndExpandedTaskBar:
			self.wndExpandedTaskBar.Destroy()

		if self.wndEnergyBar:
			self.wndEnergyBar.Destroy()

		if self.wndCharacter:
			self.wndCharacter.Destroy()

		if self.wndInventory:
			self.wndInventory.Destroy()

		if app.WJ_SPLIT_INVENTORY_SYSTEM:
			if self.wndExtendedInventory:
				self.wndExtendedInventory.Destroy()

		if self.wndDragonSoul:
			self.wndDragonSoul.Destroy()

		if self.wndDragonSoulRefine:
			self.wndDragonSoulRefine.Destroy()

		if self.dlgExchange:
			self.dlgExchange.Destroy()

		if self.dlgPointReset:
			self.dlgPointReset.Destroy()

		if self.dlgShop:
			self.dlgShop.Destroy()

		if self.dlgRestart:
			self.dlgRestart.Destroy()

		if self.dlgSystem:
			self.dlgSystem.Destroy()

		if self.dlgPassword:
			self.dlgPassword.Destroy()

		if app.ENABLE_SAVE_LOCATION_SYSTEM and self.wndSaveLocation:
			self.wndSaveLocation.Destroy()

		if self.wndMiniMap:
			self.wndMiniMap.Destroy()

		if self.messenger_bg:
			self.messenger_bg.Destroy()

		if self.messenger:
			self.messenger.Destroy()

		if self.wndSafebox:
			self.wndSafebox.Destroy()

		if self.wndWeb:
			self.wndWeb.Destroy()
			self.wndWeb = None

		if self.wndMall:
			self.wndMall.Destroy()

		if self.wndParty:
			self.wndParty.Destroy()
			
		if app.ENABLE_TITLE_ACHIEVEMENT_SYSTEM:
			if self.wndTitleAchievement:
				self.wndTitleAchievement.Destroy()
				del self.wndTitleAchievement

		if self.wndHelp:
			self.wndHelp.Destroy()

		if self.wndCardsInfo:
			self.wndCardsInfo.Destroy()

		if self.wndCards:
			self.wndCards.Destroy()

		if self.wndCardsIcon:
			self.wndCardsIcon.Destroy()
				
		if app.ENABLE_ARTEFAKT_SYSTEM:
			if self.wndArtefaktWindow:
				self.wndArtefaktWindow.Destroy()
				del self.wndArtefaktWindow

		if self.wndCube:
			self.wndCube.Destroy()

		if constInfo.GIFT_CODE_SYSTEM:
			if self.wndGiftCodeWindow:
				self.wndGiftCodeWindow.Destroy()

		if app.ENABLE_ACCE_COSTUME_SYSTEM and self.wndAcceCombine:
			self.wndAcceCombine.Destroy()

		if app.ENABLE_ACCE_COSTUME_SYSTEM and self.wndAcceAbsorption:
			self.wndAcceAbsorption.Destroy()

		if app.ENABLE_AURA_SYSTEM:
			if self.wndAuraAbsorption:
				self.wndAuraAbsorption.Destroy()

			if self.wndAuraRefine:
				self.wndAuraRefine.Destroy()

		if app.ENABLE_LOADING_PERFORMANCE:
			if self.wndWarpShower:
				self.wndWarpShower.Destroy()
				del self.wndWarpShower

		if self.wndMessenger:
			self.wndMessenger.Destroy()

		if self.wndGuild:
			self.wndGuild.Destroy()
		
		if app.ENABLE_NEW_PET_SYSTEM and self.wndCompanionWindow:
			self.wndCompanionWindow.Destroy()

		if self.privateShopBuilder:
			self.privateShopBuilder.Destroy()

		if self.wndChangerWindow:
			self.wndChangerWindow.Destroy()

		if self.dlgRefineNew:
			self.dlgRefineNew.Destroy()
		

		if app.ENABLE_COLLECTIONS_SYSTEM:
			if self.wndCollections:
				self.wndCollections.Destroy()

		if self.wndGuildBuilding:
			self.wndGuildBuilding.Destroy()

		if app.__ENABLE_POLYMORPH_SYSTEM__:
			if self.wndPolySystem:
				self.wndPolySystem.Destroy()
		
		if self.wndWorldBoss:
			self.wndWorldBoss.Destroy()

		if self.wndgameOption:
			self.wndgameOption.Hide()
			self.wndgameOption.Destroy()
			del self.wndgameOption

		if app.ENABLE_PVP_RANKING:
			if self.wndPvPRanking:
				self.wndPvPRanking.Destroy()
			if self.wndPvPRankingAdmin:
				self.wndPvPRankingAdmin.Destroy()

		if self.wndGameButton:
			self.wndGameButton.Destroy()
		if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
			if self.wndPrivateShopPanel:
				self.wndPrivateShopPanel.Hide()
				self.wndPrivateShopPanel.Destroy()
				
			if self.wndPrivateShopSearch:
				self.wndPrivateShopSearch.Hide()
				self.wndPrivateShopSearch.Destroy()

			del self.wndPrivateShopPanel
			del self.wndPrivateShopSearch
			self.privateShopTitleBoardDict = {}
			
		if app.__BL_CHEST_DROP_INFO__:
			if self.wndChestDropInfo:
				del self.wndChestDropInfo


			
		if self.wndIsIcon:
			self.wndIsIcon.Destroy()


		if self.mallPageDlg:
			self.mallPageDlg.Destroy()

		if app.ENABLE_OFFLINE_SHOP:
			if self.wndShopOffline:
				self.wndShopOffline.Hide()
				self.wndShopOffline.Destroy()
				del self.wndShopOffline	

			if self.wndShopSearch:
				self.wndShopSearch.Hide()
				self.wndShopSearch.Destroy()
				del self.wndShopSearch

			if self.wndShopNotification:
				self.wndShopNotification.Hide()
				self.wndShopNotification.Destroy()
				del self.wndShopNotification

		if self.wndItemSelect:
			self.wndItemSelect.Destroy()

		if app.ENABLE_MINIMAP_DUNGEONINFO:
			if self.wndMiniMapDungeonInfo:
				self.wndMiniMapDungeonInfo.Destroy()
		if self.wndEmeraldGauge:
			self.wndEmeraldGauge.Close()
			self.wndEmeraldGauge = None
		if self.wndGorgonGauge:
			self.wndGorgonGauge.Close()
			self.wndGorgonGauge = None
		if self.wndCrimsonGauge:
			self.wndCrimsonGauge.Close()
			self.wndCrimsonGauge = None

		if self.wndRemoveItem:
			self.wndRemoveItem.Destroy()
			del self.wndRemoveItem

		if app.ENABLE_CUBE_RENEWAL_WORLDARD:
			if self.wndCubeRenewal:
				self.wndCubeRenewal.Destroy()
				self.wndCubeRenewal.Close()

		if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
			if self.wndBuffNPCWindow:
				self.wndBuffNPCWindow.Destroy()
			if self.wndBuffNPCCreateWindow:
				self.wndBuffNPCCreateWindow.Destroy()
				
		if self.wndCrystalExchangeWindow:
			self.wndCrystalExchangeWindow.Destroy()

		if app.ENABLE_VS_SHOP_SEARCH:
			if self.wndOfflineShopSearch:
				self.wndOfflineShopSearch.Destroy()

		if app.ENABLE_RESP_SYSTEM:
			if self.wndResp:
				self.wndResp.Destroy()

				del self.wndResp

		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			if (self.dlgOfflineShop):
				self.dlgOfflineShop.Destroy()
				
			if (self.offlineShopBuilder):
				self.offlineShopBuilder.Destroy()

			if (self.wndOfflineShopAdminPanel):
				self.wndOfflineShopAdminPanel.Destroy()
			
			if (self.wndOfflineShopLogPanel):
				self.wndOfflineShopLogPanel.Destroy()


		if app.ENABLE_ODLAMKI_SYSTEM:
			if self.wndOdlamki:
				self.wndOdlamki.Destroy()
		
		if self.wndCasketPreview:
			self.wndCasketPreview.Destroy()
			del self.wndCasketPreview

		if app.ENABLE_SWITCHBOT:
			if self.wndSwitchbot:
				self.wndSwitchbot.Destroy()

		if app.WORLD_BOSS_YUMA:
			if self.WorldbossHwnd:
				self.WorldbossHwnd = {}

		if self.wndBonus:
			self.wndBonus.Destroy()
		
		if self.wndCollectWindow:
			self.wndCollectWindow.Destroy()

		if self.wndWeeklyRankWindow_New:
			self.wndWeeklyRankWindow_New.Destroy()

		if self.wndBlend:
			self.wndBlend.Destroy()

		if constInfo.ENABLE_EXPANDED_MONEY_TASKBAR:
			if self.wndExpandedMoneyTaskBar:
				self.wndExpandedMoneyTaskBar.Destroy()

		if app.TAKE_LEGEND_DAMAGE_BOARD_SYSTEM:
			if self.wndLegendDamageWindow:
				self.wndLegendDamageWindow.Destroy()
				del self.wndLegendDamageWindow

		if self.wndBattlePassButton:
			self.wndBattlePassButton.Hide(True)
			self.wndBattlePassButton.Destroy()
			del self.wndBattlePassButton
			
		if self.wndBattlePass:
			self.wndBattlePass.Hide()
			self.wndBattlePass.Destroy()
			del self.wndBattlePass

		self.wndChatLog.Destroy()
		
		if app.ENABLE_NOTIFICATION_SYSTEM:
			if self.wndNotificationManager:
				self.wndNotificationManager.Destroy()
				
		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			if self.wndDungeonInfo:
				self.wndDungeonInfo.Destroy()
				del self.wndDungeonInfo
				
		if app.ENABLE_VOTE4BUFF:
			if self.btnVote4Buff:
				self.btnVote4Buff = None

			if self.wndVote4Buff:
				self.wndVote4Buff.Destroy()
				self.wndVote4Buff = None

		for btn in self.questButtonList:
			btn.SetEvent(0)
		for btn in self.whisperButtonList:
			btn.SetEvent(0)
		for dlg in self.whisperDialogDict.itervalues():
			dlg.Destroy()
		for brd in self.guildScoreBoardDict.itervalues():
			brd.Destroy()
		for dlg in self.equipmentDialogDict.itervalues():
			dlg.Destroy()

		del self.mallPageDlg
		
		if app.ENABLE_MINIMAP_DUNGEONINFO:
			del self.wndMiniMapDungeonInfo

		del self.wndGuild
		del self.wndMessenger
		del self.wndUICurtain
		del self.wndChat
		del self.yangText
		del self.wndTaskBar
		del self.wndWonExchange
		if self.wndExpandedTaskBar:
			del self.wndExpandedTaskBar
		del self.wndEnergyBar
		if app.ENABLE_CUBE_RENEWAL_WORLDARD:
			del self.wndCubeRenewal
			
		del self.wndCharacter
		del self.wndInventory
		if app.__ENABLE_POLYMORPH_SYSTEM__:
			del self.wndPolySystem
		if self.wndDragonSoul:
			del self.wndDragonSoul
		if self.wndDragonSoulRefine:
			del self.wndDragonSoulRefine
		del self.wndWorldBoss
		del self.dlgExchange
		del self.dlgPointReset
		del self.dlgShop
		del self.dlgRestart
		del self.dlgSystem
		del self.dlgPassword
		del self.hyperlinkItemTooltip
		del self.tooltipItem
		del self.tooltipSkill
		del self.wndMiniMap
		del self.messenger_bg
		del self.messenger
		del self.wndSafebox
		del self.wndMall
		del self.wndParty
		del self.wndHelp

		del self.wndCardsInfo
		del self.wndCards
		del self.wndCardsIcon

		if app.ENABLE_SAVE_LOCATION_SYSTEM:
			del self.wndSaveLocation

		del self.wndCube
		if constInfo.GIFT_CODE_SYSTEM:
			del self.wndGiftCodeWindow
		del self.privateShopBuilder
		del self.inputDialog
		del self.wndChatLog
		del self.dlgRefineNew
		del self.wndGuildBuilding
		if app.ENABLE_NEW_PET_SYSTEM and self.wndCompanionWindow:
			del self.wndCompanionWindow
		del self.wndGameButton
		del self.wndIsIcon
		del self.tipBoard
		del self.bigBoard
		if app.ENABLE_PVP_RANKING:
			del self.wndPvPRanking
			del self.wndPvPRankingAdmin
		del self.wndItemSelect
		if app.ENABLE_NOTIFICATION_SYSTEM:
			if self.wndNotificationManager:
				del self.wndNotificationManager
		if app.ENABLE_COLLECTIONS_SYSTEM:
			del self.wndCollections
		del self.wndChangerWindow
		if app.WJ_SPLIT_INVENTORY_SYSTEM:
			if self.wndExtendedInventory:
				del self.wndExtendedInventory
		if app.ENABLE_SWITCHBOT:
			del self.wndSwitchbot	
		del self.wndBonus
		del self.wndCollectWindow
		del self.wndWeeklyRankWindow_New
		del self.wndBlend

		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			del self.dlgOfflineShop
			del self.wndOfflineShopAdminPanel
			del self.wndOfflineShopLogPanel
			self.offlineShopAdvertisementBoardDict = {}


		if app.ENABLE_ODLAMKI_SYSTEM:
			del self.wndOdlamki

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			del self.wndAcceCombine
			del self.wndAcceAbsorption

		if app.ENABLE_AURA_SYSTEM:
			del self.wndAuraAbsorption
			del self.wndAuraRefine
			
		if app.ENABLE_SECONDARY_LEVEL:
			del self.wndSecondaryLevel
			
		del self.wndRankingWindow

		del self.wndRankingWindowWeekly

		if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
			del self.wndBuffNPCWindow
			del self.wndBuffNPCCreateWindow
		
		del self.wndCrystalExchangeWindow

		if app.ENABLE_VS_SHOP_SEARCH:
			del self.wndOfflineShopSearch

		if constInfo.ENABLE_EXPANDED_MONEY_TASKBAR:
			if self.wndExpandedMoneyTaskBar:
				del self.wndExpandedMoneyTaskBar

		self.questButtonList = []
		self.whisperButtonList = []
		self.whisperDialogDict = {}
		self.privateShopAdvertisementBoardDict = {}
		self.guildScoreBoardDict = {}
		self.equipmentDialogDict = {}

		uiChat.DestroyChatInputSetWindow()
		
		for window in self.interfaceWindowList.itervalues():
			window.Destroy()
		self.interfaceWindowList.clear()	
  
	def OnUseSkill(self, slotIndex, coolTime):
		self.wndCharacter.OnUseSkill(slotIndex, coolTime)
		self.wndTaskBar.OnUseSkill(slotIndex, coolTime)
		self.wndGuild.OnUseSkill(slotIndex, coolTime)

	def OnActivateSkill(self, slotIndex):
		self.wndCharacter.OnActivateSkill(slotIndex)
		self.wndTaskBar.OnActivateSkill(slotIndex)

	def OnDeactivateSkill(self, slotIndex):
		self.wndCharacter.OnDeactivateSkill(slotIndex)
		self.wndTaskBar.OnDeactivateSkill(slotIndex)

	def OnChangeCurrentSkill(self, skillSlotNumber):
		self.wndTaskBar.OnChangeCurrentSkill(skillSlotNumber)

	def SelectMouseButtonEvent(self, dir, event):
		self.wndTaskBar.SelectMouseButtonEvent(dir, event)

	def RefreshAlignment(self):
		self.wndCharacter.RefreshAlignment()

	def RefreshStatus(self):
		self.wndTaskBar.RefreshStatus()
		self.wndCharacter.RefreshStatus()
		self.wndInventory.RefreshStatus()
		if self.wndEnergyBar:
			self.wndEnergyBar.RefreshStatus()
		if app.WJ_SPLIT_INVENTORY_SYSTEM:
			self.wndExtendedInventory.RefreshStatus()
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.RefreshStatus()
		if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
			if self.wndPrivateShopPanel.IsShow():
				self.wndPrivateShopPanel.Refresh()
		if app.__AUTO_SKILL_READER__ and self.wndAutoSkillReader and self.wndAutoSkillReader.IsShow():
			self.wndAutoSkillReader.Refresh()
			
	def RefreshStamina(self):
		self.wndTaskBar.RefreshStamina()

	def RefreshSkill(self):
		self.wndCharacter.RefreshSkill()
		self.wndTaskBar.RefreshSkill()

	def RefreshInventory(self):
		self.wndTaskBar.RefreshQuickSlot()
		self.wndInventory.RefreshItemSlot()
		if constInfo.IS_BONUS_CHANGER:
			self.UpdateBonusChanger()
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.RefreshItemSlot()
		if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
			self.wndBuffNPCWindow.RefreshEquipSlotWindow()
		if app.WJ_SPLIT_INVENTORY_SYSTEM:
			self.wndExtendedInventory.RefreshItemSlot()
		if app.ENABLE_ARTEFAKT_SYSTEM:
			self.wndArtefaktWindow.RefreshSlot()
		if app.__AUTO_SKILL_READER__ and self.wndAutoSkillReader and self.wndAutoSkillReader.IsShow():
			self.wndAutoSkillReader.Refresh()

	def RefreshCharacter(self):
		self.wndCharacter.RefreshCharacter()
		self.wndTaskBar.RefreshQuickSlot()

	def RefreshQuest(self):
		self.wndCharacter.RefreshQuest()

	def RefreshSafebox(self):
		self.wndSafebox.RefreshSafebox()

	def RefreshMall(self):
		self.wndMall.RefreshMall()

	if app.ENABLE_ARTEFAKT_SYSTEM:
		def OpenArtefaktWindow(self):
			if self.wndArtefaktWindow.IsShow():
				self.wndArtefaktWindow.Hide()
			else:
				self.wndArtefaktWindow.Open()

	if app.ENABLE_AUTO_SELECT_SKILL:
		def MakeAutoSkillWindow(self):
			if self.wndAutoSkill == None:
				self.wndAutoSkill = uiAutoSkill.AutoSkillSelect()
		def OpenAutoSkillWindow(self):
			self.MakeAutoSkillWindow()
			self.wndAutoSkill.Show()
			
	if app.ENABLE_WIKI:
		def OpenWikiWindow(self):
			try:
				if self.wndWiki == None:
					self.__MakeWiki()
				
				if self.wndWiki.IsShow():
					self.wndWiki.Close()
				else:
					self.wndWiki.Open()
					
			except Exception as e:
				sys_err("Interface: Error in OpenWikiWindow(): {}".format(str(e)))
		
		def __MakeWiki(self):
			try:
				if self.wndWiki == None:
					self.wndWiki = uiWiki.EncyclopediaofGame()
			except Exception as e:
				sys_err("Interface: Error in __MakeWiki(): {}".format(str(e)))
				
		def WikiSearchItem(self, itemVnum):
			if self.wndWiki:
				if not self.wndWiki.IsShow():
					self.wndWiki.Show()
				self.wndWiki.SetTop()
				self.wndWiki.SearchItem(itemVnum)

	def OpenItemMall(self):
		if not self.mallPageDlg:
			self.mallPageDlg = uiShop.MallPageDialog()

		self.mallPageDlg.Open()

	def RefreshMessenger(self):
		self.wndMessenger.RefreshMessenger()

	def RefreshGuildInfoPage(self):
		self.wndGuild.RefreshGuildInfoPage()

	def RefreshGuildDungeonInfoPage(self):
		self.wndGuild.RefreshGuildDungeonInfoPage()

	def RefreshGuildDungeonBossDmgRankWindow(self):
		pass
		#if self.wndDungeonBossDmgRankWindow == None:
		#	self.wndDungeonBossDmgRankWindow = uiGuild.GuildDungeonBossDmgRankWindow()
		#	self.wndDungeonBossDmgRankWindow.Open()
		#self.wndDungeonBossDmgRankWindow.Refresh()

	def RefreshGuildBoardPage(self):
		self.wndGuild.RefreshGuildBoardPage()

	def RefreshGuildMemberPage(self):
		self.wndGuild.RefreshGuildMemberPage()

	def RefreshGuildMemberPageGradeComboBox(self):
		self.wndGuild.RefreshGuildMemberPageGradeComboBox()

	def RefreshGuildSkillPage(self):
		self.wndGuild.RefreshGuildSkillPage()

	def RefreshGuildGradePage(self):
		self.wndGuild.RefreshGuildGradePage()

	def DeleteGuild(self):
		self.wndMessenger.ClearGuildMember()
		self.wndGuild.DeleteGuild()

	def RefreshMobile(self):
		self.dlgSystem.RefreshMobile()

	def OnMobileAuthority(self):
		self.dlgSystem.OnMobileAuthority()

	def OnBlockMode(self, mode):
		self.dlgSystem.OnBlockMode(mode)

	def OpenPointResetDialog(self):
		self.dlgPointReset.Show()
		self.dlgPointReset.SetTop()

	def ClosePointResetDialog(self):
		self.dlgPointReset.Close()

	def OpenShopDialog(self, vid):
		self.wndInventory.Show()
		self.wndInventory.SetTop()
		self.dlgShop.Open(vid)
		self.dlgShop.SetTop()

	def CloseShopDialog(self):
		self.dlgShop.Close()

	def RefreshShopDialog(self):
		self.dlgShop.Refresh()

	if app.ENABLE_OFFLINE_SHOP_SYSTEM:
		def OpenOfflineShopDialog(self, vid):
			self.wndInventory.Show()
			self.wndInventory.SetTop()
			self.dlgOfflineShop.Open(vid)
			self.dlgOfflineShop.SetTop()

			if self.wndOfflineShopAdminPanel:
				if self.wndOfflineShopAdminPanel.wndOfflineShopAddItem:
					self.wndOfflineShopAdminPanel.wndOfflineShopAddItem.Close()
					
				if self.wndOfflineShopAdminPanel.wndOfflineShopRemoveItem:
					self.wndOfflineShopAdminPanel.wndOfflineShopRemoveItem.Close()
			
		def CloseOfflineShopDialog(self):	
			self.dlgOfflineShop.Close()
			
		def RefreshOfflineShopDialog(self):
			self.dlgOfflineShop.Refresh()


		def ToggleOfflineShopLogWindow(self):
			self.wndOfflineShopLogPanel.Open()

		def OpenOfflineShopInputNameDialog(self):
			inputDialog = uiOfflineShop.OfflineShopInputDialog()
			inputDialog.SetAcceptEvent(ui.__mem_func__(self.OpenOfflineShopBuilder))
			inputDialog.SetCancelEvent(ui.__mem_func__(self.CloseOfflineShopInputNameDialog))
			inputDialog.Open()
			self.inputDialog = inputDialog
			
		def CloseOfflineShopInputNameDialog(self):
			self.inputDialog = None
			return True
		
		def OpenOfflineShopBuilder(self):	
			self.offlineShopBuilder.Open(player.GetName())
			self.CloseOfflineShopInputNameDialog()
			return True
		
		def AppearOfflineShop(self, vid, text):
			board = uiOfflineShopBuilder.OfflineShopAdvertisementBoard()
			board.Open(vid, text)
			
			self.offlineShopAdvertisementBoardDict[vid] = board
			
		def DisappearOfflineShop(self, vid):
			if (not self.offlineShopAdvertisementBoardDict.has_key(vid)):
				return
				
			del self.offlineShopAdvertisementBoardDict[vid]
			uiOfflineShopBuilder.DeleteADBoard(vid)

	if app.ENABLE_VS_SHOP_SEARCH:
		def ToggleOfflineShopSearch(self):
			if self.wndOfflineShopSearch.IsShow():
				self.wndOfflineShopSearch.Close()
			else:
				self.wndOfflineShopSearch.Open()
				self.wndOfflineShopSearch.SetTop()
				

	def ToggleSearchShop(self):
		if self.wndShopSearch:
			if not self.wndShopSearch.IsShow():
				self.wndShopSearch.Show()
			else:
				self.wndShopSearch.Close()

	def OpenCharacterWindowQuestPage(self):
		self.wndCharacter.Show()
		self.wndCharacter.SetState("QUEST")

	def OpenQuestWindow(self, skin, idx):

		wnds = ()

		q = uiQuest.QuestDialog(skin, idx)
		q.SetWindowName("QuestWindow" + str(idx))
		q.Show()
		if skin:
			q.Lock()
			wnds = self.__HideWindows()

			q.AddOnDoneEvent(lambda tmp_self, args=wnds: self.__ShowWindows(args))

		if skin:
			q.AddOnCloseEvent(q.Unlock)
		q.AddOnCloseEvent(lambda key = self.wndQuestWindowNewKey:ui.__mem_func__(self.RemoveQuestDialog)(key))
		self.wndQuestWindow[self.wndQuestWindowNewKey] = q

		self.wndQuestWindowNewKey = self.wndQuestWindowNewKey + 1


	def RemoveQuestDialog(self, key):
		del self.wndQuestWindow[key]

	def StartExchange(self):
		self.dlgExchange.OpenDialog()
		self.dlgExchange.Refresh()

	def EndExchange(self):
		self.dlgExchange.CloseDialog()

	def RefreshExchange(self):
		self.dlgExchange.Refresh()

	if app.WJ_ENABLE_TRADABLE_ICON:
		def CantTradableItemExchange(self, dstSlotIndex, srcSlotIndex):
			self.dlgExchange.CantTradableItem(dstSlotIndex, srcSlotIndex)

	def AddPartyMember(self, pid, name):
		self.wndParty.AddPartyMember(pid, name)

		self.__ArrangeQuestButton()

	def UpdatePartyMemberInfo(self, pid):
		self.wndParty.UpdatePartyMemberInfo(pid)

	def RemovePartyMember(self, pid):
		self.wndParty.RemovePartyMember(pid)

		self.__ArrangeQuestButton()

	def LinkPartyMember(self, pid, vid):
		self.wndParty.LinkPartyMember(pid, vid)

	def UnlinkPartyMember(self, pid):
		self.wndParty.UnlinkPartyMember(pid)

	def UnlinkAllPartyMember(self):
		self.wndParty.UnlinkAllPartyMember()

	def ExitParty(self):
		self.wndParty.ExitParty()

		self.__ArrangeQuestButton()

	def PartyHealReady(self):
		self.wndParty.PartyHealReady()

	def ChangePartyParameter(self, distributionMode):
		self.wndParty.ChangePartyParameter(distributionMode)

	def AskSafeboxPassword(self):
		if self.wndSafebox.IsShow():
			return

		self.dlgPassword.SetTitle(localeInfo.PASSWORD_TITLE)
		self.dlgPassword.SetSendMessage("/safebox_password ")

		self.dlgPassword.ShowDialog()

	def OpenSafeboxWindow(self, size):
		self.dlgPassword.CloseDialog()
		self.wndSafebox.ShowWindow(size)

	def RefreshSafeboxMoney(self):
		self.wndSafebox.RefreshSafeboxMoney()

	def CommandCloseSafebox(self):
		self.wndSafebox.CommandCloseSafebox()

	def AskMallPassword(self):
		if self.wndMall.IsShow():
			return
		self.dlgPassword.SetTitle(localeInfo.MALL_PASSWORD_TITLE)
		self.dlgPassword.SetSendMessage("/mall_password ")
		self.dlgPassword.ShowDialog()

	def OpenMallWindow(self, size):
		self.dlgPassword.CloseDialog()
		self.wndMall.ShowWindow(size)

	def CommandCloseMall(self):
		self.wndMall.CommandCloseMall()

	def OnStartGuildWar(self, guildSelf, guildOpp):
		self.wndGuild.OnStartGuildWar(guildSelf, guildOpp)

		guildWarScoreBoard = uiGuild.GuildWarScoreBoard()
		guildWarScoreBoard.Open(guildSelf, guildOpp)
		guildWarScoreBoard.Show()
		self.guildScoreBoardDict[uiGuild.GetGVGKey(guildSelf, guildOpp)] = guildWarScoreBoard

	def OnEndGuildWar(self, guildSelf, guildOpp):
		self.wndGuild.OnEndGuildWar(guildSelf, guildOpp)

		key = uiGuild.GetGVGKey(guildSelf, guildOpp)

		if not self.guildScoreBoardDict.has_key(key):
			return

		self.guildScoreBoardDict[key].Destroy()
		del self.guildScoreBoardDict[key]

	def UpdateMemberCount(self, gulidID1, memberCount1, guildID2, memberCount2):
		key = uiGuild.GetGVGKey(gulidID1, guildID2)

		if not self.guildScoreBoardDict.has_key(key):
			return

		self.guildScoreBoardDict[key].UpdateMemberCount(gulidID1, memberCount1, guildID2, memberCount2)

	def OnRecvGuildWarPoint(self, gainGuildID, opponentGuildID, point):
		key = uiGuild.GetGVGKey(gainGuildID, opponentGuildID)
		if not self.guildScoreBoardDict.has_key(key):
			return

		guildBoard = self.guildScoreBoardDict[key]
		guildBoard.SetScore(gainGuildID, opponentGuildID, point)

	def OnChangePKMode(self):
		self.wndCharacter.RefreshAlignment()
		self.dlgSystem.OnChangePKMode()

	def OpenRefineDialog(self, targetItemPos, nextGradeItemVnum, cost, cost2, cost3, prob, type, probExtra):
		self.dlgRefineNew.Open(targetItemPos, nextGradeItemVnum, cost, cost2, cost3, prob, type, probExtra)

	def AppendMaterialToRefineDialog(self, vnum, count):
		self.dlgRefineNew.AppendMaterial(vnum, count)

	def ShowDefaultWindows(self):
		self.wndTaskBar.Show()
		if constInfo.ENABLE_EXPANDED_MONEY_TASKBAR:
			if self.wndExpandedMoneyTaskBar:
				self.wndExpandedMoneyTaskBar.Show()
		self.wndMiniMap.Show()
		self.wndMiniMap.ShowMiniMap()
		if self.wndEnergyBar:
			self.wndEnergyBar.Show()
		if constInfo.ZAPAMIETAJ_OKNO_ZAPISU == True:
			self.wndLocationWindow.Show()
		if constInfo.ZAPAMIETAJ_OKNO_TPMETKIBOSSY == True:
			self.wndResp.Show()

	def ShowAllWindows(self):
		self.wndTaskBar.Show()
		self.wndCharacter.Show()
		self.wndInventory.Show()
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.Show()
			self.wndDragonSoulRefine.Show()
		self.wndChat.Show()
		self.yangText.Show()
		self.wndMiniMap.Show()
		if self.wndEnergyBar:
			self.wndEnergyBar.Show()
		if self.wndWonExchange:
			self.wndWonExchange.Show()
		if self.wndExpandedTaskBar:
			self.wndExpandedTaskBar.Show()
			self.wndExpandedTaskBar.SetTop()
		if constInfo.ENABLE_EXPANDED_MONEY_TASKBAR:
			if self.wndExpandedMoneyTaskBar:
				self.wndExpandedMoneyTaskBar.Show()
				self.wndExpandedMoneyTaskBar.SetTop()

	def HideAllWindows(self):
		if self.wndTaskBar:
			self.wndTaskBar.Hide()

		if app.ENABLE_COLLECTIONS_SYSTEM:
			if self.wndCollections:
				self.wndCollections.Hide()

		if self.wndEnergyBar:
			self.wndEnergyBar.Hide()

		if self.wndCharacter:
			self.wndCharacter.Hide()

		if self.wndWonExchange:
			self.wndWonExchange.Hide()
			
		if app.ENABLE_SECONDARY_LEVEL:
			if self.wndSecondaryLevel:
				self.wndSecondaryLevel.Hide()
				
		if self.wndRankingWindow:
			self.wndRankingWindow.Hide()
		
		if self.wndRankingWindowWeekly:
			self.wndRankingWindowWeekly.Hide()

		if self.wndInventory:
			self.wndInventory.Hide()

		if constInfo.ENABLE_EXPANDED_MONEY_TASKBAR:
			if self.wndExpandedMoneyTaskBar:
				self.wndExpandedMoneyTaskBar.Hide()

		if app.WJ_SPLIT_INVENTORY_SYSTEM:
			if self.wndExtendedInventory:
				self.wndExtendedInventory.Hide()

		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.Hide()
			self.wndDragonSoulRefine.Hide()
		
		if self.wndChat:
			self.wndChat.Hide()

		if app.ENABLE_SAVE_LOCATION_SYSTEM and self.wndSaveLocation:
			self.wndSaveLocation.Hide()

		if self.yangText:
			self.yangText.Hide()

		if self.wndMiniMap:
			self.wndMiniMap.Hide()

		if self.wndMessenger:
			self.wndMessenger.Hide()
			
		if self.dlgWhisperWithoutTarget:
			self.dlgWhisperWithoutTarget.Hide()
			
		if app.ENABLE_MINIMAP_DUNGEONINFO:
			if self.wndMiniMapDungeonInfo:
				self.wndMiniMapDungeonInfo.Hide()

		if self.wndGuild:
			self.wndGuild.Hide()

		if app.__ENABLE_POLYMORPH_SYSTEM__:
			self.wndPolySystem.Hide()
		
		self.wndWorldBoss.Hide()
			
		if app.ENABLE_TITLE_ACHIEVEMENT_SYSTEM:
			if self.wndTitleAchievement:
				self.wndTitleAchievement.Hide()

		if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
			self.wndPrivateShopPanel.Hide()
			self.wndPrivateShopSearch.Hide()

		if self.wndExpandedTaskBar:
			self.wndExpandedTaskBar.Hide()
			
		if app.ENABLE_NOTIFICATION_SYSTEM:
			if self.wndNotificationManager:
				self.wndNotificationManager.Hide()
			
		if app.__BL_CHEST_DROP_INFO__:
			if self.wndChestDropInfo:
				self.wndChestDropInfo.Hide()
			
		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			if self.wndDungeonInfo:
				self.wndDungeonInfo.Hide()

		if app.__DUNGEON_INFO__:
			if self.wndDungeonTrack:
				self.wndDungeonTrack.Close()

		if self.wndRemoveItem:
			self.wndRemoveItem.Hide()

		if app.ENABLE_RESP_SYSTEM:
			if self.wndResp:
				self.wndResp.Hide()
		
		if self.wndCasketPreview:
			self.wndCasketPreview.Hide()
			
		if app.ENABLE_PVP_RANKING:
			if self.wndPvPRanking:
				self.wndPvPRanking.Hide()
			if self.wndPvPRankingAdmin:
				self.wndPvPRankingAdmin.Hide()

		if app.ENABLE_SWITCHBOT:
			if self.wndSwitchbot:
				self.wndSwitchbot.Hide()
		
		if self.messenger_bg:
			self.messenger_bg.Hide()
		
		if self.messenger:
			self.messenger.Hide()

		if self.wndBonus:
			self.wndBonus.Hide()

		if self.wndCollectWindow:
			self.wndCollectWindow.Hide()

		if self.wndWeeklyRankWindow_New:
			self.wndWeeklyRankWindow_New.Hide()

		if app.TAKE_LEGEND_DAMAGE_BOARD_SYSTEM:
			if self.wndLegendDamageWindow:
				self.wndLegendDamageWindow.Close()

		if self.wndBlend:
			self.wndBlend.Hide()

		if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
			if self.wndBuffNPCWindow:
				self.wndBuffNPCWindow.Hide()
			if self.wndBuffNPCCreateWindow:
				self.wndBuffNPCCreateWindow.Hide()
				
		if self.wndCrystalExchangeWindow:
			self.wndCrystalExchangeWindow.Hide()

		if app.ENABLE_VS_SHOP_SEARCH:
			if self.wndOfflineShopSearch:
				self.wndOfflineShopSearch.Hide()
    
		for window in self.interfaceWindowList.itervalues():
			window.Close()
			
	def IsShowDlgQuestionWindow(self):
		if self.wndDragonSoul.IsDlgQuestionShow():
			return True
		elif self.dlgShop.IsDlgQuestionShow():
			return True
		elif self.wndWonExchange.IsDlgQuestionShow():
			return True
		else:
			return False

	def CloseDlgQuestionWindow(self):
		if self.wndDragonSoul.IsDlgQuestionShow():
			self.wndDragonSoul.ExternQuestionDialog_Close()
		if self.dlgShop.IsDlgQuestionShow():
			self.dlgShop.ExternQuestionDialog_Close()
		if self.wndWonExchange.IsDlgQuestionShow():
			self.wndWonExchange.ExternQuestionDialog_Close()

	def ShowMouseImage(self):
		self.wndTaskBar.ShowMouseImage()

	def HideMouseImage(self):
		self.wndTaskBar.HideMouseImage()

	def ToggleChat(self):
		if True == self.wndChat.IsEditMode():
			self.wndChat.CloseChat()
		else:
			if self.wndWeb and self.wndWeb.IsShow():
				pass
			else:
				self.wndChat.OpenChat()

	def IsOpenChat(self):
		return self.wndChat.IsEditMode()

	def SetChatFocus(self):
		self.wndChat.SetChatFocus()

	if app.RENEWAL_DEAD_PACKET:
		def OpenRestartDialog(self, d_time):
			self.dlgRestart.OpenDialog(d_time)
			self.dlgRestart.SetTop()
	else:
		def OpenRestartDialog(self):
			self.dlgRestart.OpenDialog()
			self.dlgRestart.SetTop()

	def CloseRestartDialog(self):
		self.dlgRestart.Close()

	def ToggleSystemDialog(self):
		if False == self.dlgSystem.IsShow():
			self.dlgSystem.OpenDialog()
			self.dlgSystem.SetTop()
		else:
			self.dlgSystem.Close()

	def OpenSystemDialog(self):
		self.dlgSystem.OpenDialog()
		self.dlgSystem.SetTop()

	if app.BL_MOVE_CHANNEL:
		def OpenMoveChannelDialog(self):
			if net.GetChannelNumber() == 99:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_CHANNEL_NOT_MOVE)
				return
			if net.GetMapIndex() >= 10000:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_CHANNEL_NOT_MOVE)
				return
			if getattr(self.dlgSystem, "moveChannelDlg", None):
				self.dlgSystem.moveChannelDlg.Open()
			else:
				if hasattr(uiSystem, "MoveChannelDialog"):
					moveChannelDlg = uiSystem.MoveChannelDialog()
					moveChannelDlg.Open()
					self.dlgSystem.moveChannelDlg = moveChannelDlg

		def __CanChangeChannel(self):
			if net.GetChannelNumber() == 99:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_CHANNEL_NOT_MOVE)
				return False
			if net.GetMapIndex() >= 10000:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_CHANNEL_NOT_MOVE)
				return False
			return True

		def ChannelUp(self):
			if not self.__CanChangeChannel():
				return
			MAX_CHANNELS = 6
			currentCh = net.GetChannelNumber()
			nextCh = currentCh % MAX_CHANNELS + 1
			constInfo.PREV_CHANNEL = currentCh
			net.MoveChannelGame(nextCh)

		def ChannelDown(self):
			if not self.__CanChangeChannel():
				return
			MAX_CHANNELS = 6
			currentCh = net.GetChannelNumber()
			nextCh = (currentCh - 2) % MAX_CHANNELS + 1
			constInfo.PREV_CHANNEL = currentCh
			net.MoveChannelGame(nextCh)

	def ToggleMessenger(self):
		if self.wndMessenger.IsShow():
			self.wndMessenger.Hide()
		else:
			self.wndMessenger.SetTop()
			self.wndMessenger.Show()

	if app.ENABLE_MINIMAP_DUNGEONINFO:
		def SetMiniMapDungeonInfo(self, state):
			import dbg
			if state == 1:
				self.wndMiniMap.HideMiniMap()
				self.wndMiniMap.Hide()
				self.wndMiniMapDungeonInfo.Show()
			else:
				self.wndMiniMap.Show()
				self.wndMiniMap.ShowMiniMap()
				self.wndMiniMapDungeonInfo.Hide()
	
		def SetMiniMapDungeonInfoStage(self, cur_stage, max_stage):
			self.wndMiniMapDungeonInfo.SetStage(cur_stage, max_stage)
		
		def SetMiniMapDungeonInfoGauge(self, gauge_type, value1, value2):
			if gauge_type == 3:
				# Emerald dungeon custom gauge: value1 = tasks done (0-4)
				if value1 < 0:
					# Negative: hide gauge
					if self.wndEmeraldGauge:
						self.wndEmeraldGauge.Close()
				else:
					if not self.wndEmeraldGauge:
						self.wndEmeraldGauge = uiemeral_gauge.EmeraldGauge()
					self.wndEmeraldGauge.Open(value1)
				return
			if gauge_type == 4:
				# Gorgon dungeon oxygen gauge: value1 = oxygen percent (0-100)
				if value1 < 0:
					# Negative: hide gauge
					if self.wndGorgonGauge:
						self.wndGorgonGauge.Close()
				else:
					if not self.wndGorgonGauge:
						self.wndGorgonGauge = uigorgon_gauge.GorgonGauge()
					self.wndGorgonGauge.Open(value1)
				return
			if gauge_type == 5:
				# Crimson dungeon dual-color gauge: value1 = red kills (0-20), value2 = blue kills (0-20)
				if value1 < 0:
					# Negative: hide gauge
					if self.wndCrimsonGauge:
						self.wndCrimsonGauge.Close()
				else:
					if not self.wndCrimsonGauge:
						self.wndCrimsonGauge = uicrimson_gauge.CrimsonGauge()
					self.wndCrimsonGauge.Open(value1, value2)
				return
			self.wndMiniMapDungeonInfo.SetGauge(gauge_type, value1, value2)
		
		def SetMiniMapDungeonInfoNotice(self, notice):
			self.wndMiniMapDungeonInfo.SetNotice(notice)

		def SetMiniMapDungeonInfoButton(self, status):
			self.wndMiniMapDungeonInfo.SetButton(status)

		def SetMiniMapDungeonInfoTimer(self, status, time):
			self.wndMiniMapDungeonInfo.SetTimer(status, time)

	def ToggleMiniMap(self):
		if app.IsPressed(app.DIK_LSHIFT) or app.IsPressed(app.DIK_RSHIFT):
			if False == self.wndMiniMap.isShowMiniMap():
				self.wndMiniMap.ShowMiniMap()
				self.wndMiniMap.SetTop()
			else:
				self.wndMiniMap.HideMiniMap()

		else:
			self.wndMiniMap.ToggleAtlasWindow()

	def PressMKey(self):
		if app.IsPressed(app.DIK_LALT) or app.IsPressed(app.DIK_RALT):
			self.ToggleMessenger()

		else:
			self.ToggleMiniMap()

	def SetMapName(self, mapName):
		self.wndMiniMap.SetMapName(mapName)

	def MiniMapScaleUp(self):
		self.wndMiniMap.ScaleUp()

	def MiniMapScaleDown(self):
		self.wndMiniMap.ScaleDown()

	def ToggleCharacterWindow(self, state):
		if False == player.IsObserverMode():
			if False == self.wndCharacter.IsShow():
				self.OpenCharacterWindowWithState(state)
			else:
				if state == self.wndCharacter.GetState():
					self.wndCharacter.OverOutItem()
					self.wndCharacter.Hide()
				else:
					self.wndCharacter.SetState(state)

	def OpenCharacterWindowWithState(self, state):
		if False == player.IsObserverMode():
			self.wndCharacter.SetState(state)
			self.wndCharacter.Show()
			self.wndCharacter.SetTop()

	def ToggleCharacterWindowStatusPage(self):
		self.ToggleCharacterWindow("STATUS")
		
	if app.BL_KILL_BAR:
		def AddKillInfo(self, killer, victim, killer_race, victim_race, weapon_type):
			self.wndMiniMap.AddKillInfo(killer, victim, killer_race, victim_race, weapon_type)

	def ToggleInventoryWindow(self):
		if False == player.IsObserverMode():
			if False == self.wndInventory.IsShow():
				self.wndInventory.Show()
				self.wndInventory.SetTop()
				if 1 == constInfo.EnvanterAcilsinmi:
					if not self.wndExtendedInventory.IsShow():
						self.wndExtendedInventory.Show()
			else:
				self.wndInventory.OverOutItem()
				self.wndInventory.Close()
				if 1 == constInfo.EnvanterAcilsinmi:
					if self.wndExtendedInventory.IsShow():
						self.wndExtendedInventory.Close()





	if app.ENABLE_ODLAMKI_SYSTEM:
		def OpenOdlamkiWindow(self):
			if self.wndOdlamki.IsShow():
				self.wndOdlamki.Hide()
			else:
				self.wndOdlamki.Open()


	if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
		def BuffNPC_OpenCreateWindow(self):
			if self.wndBuffNPCWindow:
				if False == self.wndBuffNPCCreateWindow.IsShow():
					self.wndBuffNPCCreateWindow.Show()
					self.wndBuffNPCCreateWindow.SetTop()
				
		def BuffNPCOpenWindow(self):
			if self.wndBuffNPCWindow:
				if False == self.wndBuffNPCWindow.IsShow():
					self.wndBuffNPCWindow.Show()
					self.wndBuffNPCWindow.SetTop()
				else:
					self.wndBuffNPCWindow.Close()
				
		def BuffNPC_Summon(self):
			if self.wndBuffNPCWindow:
				self.wndBuffNPCWindow.SetSummon()
				
		def BuffNPC_Unsummon(self):
			if self.wndBuffNPCWindow:
				self.wndBuffNPCWindow.SetUnsummon()
				
		def BuffNPC_Clear(self):
			if self.wndBuffNPCWindow:
				self.wndBuffNPCWindow.SetClear()
				
		def BuffNPC_SetBasicInfo(self, name, sex, intvalue):
			if self.wndBuffNPCWindow:
				self.wndBuffNPCWindow.SetBasicInfo(name, sex, intvalue)
				
		def BuffNPC_SetSkillInfo(self, skill1, skill2, skill3):
			if self.wndBuffNPCWindow:
				self.wndBuffNPCWindow.SetSkillInfo(skill1, skill2, skill3)
				
		def BuffNPC_SetSkillCooltime(self, slot, timevalue):
			if self.wndBuffNPCWindow:
				self.wndBuffNPCWindow.SetSkillCooltime(slot, timevalue)
		
		def BuffNPC_CreatePopup(self, type, value0, value1):
			if self.wndBuffNPCWindow:
				self.wndBuffNPCWindow.CreatePopup(type, value0, value1)

	if app.WJ_SPLIT_INVENTORY_SYSTEM:
		def ToggleExtendedInventoryWindow(self):
			if FALSE == player.IsObserverMode():
				if self.wndExtendedInventory.IsShow():
					self.wndExtendedInventory.OverOutItem()
					self.wndExtendedInventory.Close()
				else:
					self.wndExtendedInventory.Show()

	def ToggleExpandedButton(self):
		if constInfo.ExpandedTaskBarExpander == 1:
			self.wndExpandedTaskBar.Hide()
			constInfo.ExpandedTaskBarExpander = 0
		else:
			self.wndExpandedTaskBar.Show()
			constInfo.ExpandedTaskBarExpander = 1

	EXPANDED_TASKBAR_Y_DEFAULT = -94
	EXPANDED_TASKBAR_BAR_HEIGHT = 37

	def __OnChatOpen(self):
		if self.wndExpandedTaskBar and constInfo.ExpandedTaskBarExpander == 1:
			chatTop = self.wndChat.GetChatAreaTop()
			(x, _y, _w, _h) = self.wndExpandedTaskBar.GetRect()
			self.wndExpandedTaskBar.SetPosition(x, chatTop - self.EXPANDED_TASKBAR_BAR_HEIGHT - 2)

	def __OnChatClose(self):
		if self.wndExpandedTaskBar and constInfo.ExpandedTaskBarExpander == 1:
			(x, _y, _w, _h) = self.wndExpandedTaskBar.GetRect()
			self.wndExpandedTaskBar.SetPosition(x, wndMgr.GetScreenHeight() + self.EXPANDED_TASKBAR_Y_DEFAULT)

	if constInfo.ENABLE_EXPANDED_MONEY_TASKBAR:
		def ToggleExpandedMoneyButton(self):
			if False == self.wndExpandedMoneyTaskBar.IsShow():
				self.wndExpandedMoneyTaskBar.Show()
				self.wndExpandedMoneyTaskBar.SetTop()
			else:
				self.wndExpandedMoneyTaskBar.Close()

	def DragonSoulActivate(self, deck):
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.ActivateDragonSoulByExtern(deck)

	def DragonSoulDeactivate(self):
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.DeactivateDragonSoul()

	def Highligt_Item(self, inven_type, inven_pos):
		if player.DRAGON_SOUL_INVENTORY == inven_type:
			if app.ENABLE_DRAGON_SOUL_SYSTEM:
				self.wndDragonSoul.HighlightSlot(inven_pos)

		elif app.ENABLE_HIGHLIGHT_NEW_ITEM and player.SLOT_TYPE_INVENTORY == inven_type:
			self.wndInventory.HighlightSlot(inven_pos)
			self.wndExtendedInventory.HighlightSlot(inven_pos)

	def DragonSoulGiveQuilification(self):
		self.DRAGON_SOUL_IS_QUALIFIED = True
		#if (self.wndExpandedTaskBar):
		#	self.wndExpandedTaskBar.SetToolTipText(uiTaskBar.ExpandedTaskBar.BUTTON_DRAGON_SOUL, uiScriptLocale.TASKBAR_DRAGON_SOUL)

	def ToggleDragonSoulWindow(self):
		if False == player.IsObserverMode():
			if app.ENABLE_DRAGON_SOUL_SYSTEM:
				if False == self.wndDragonSoul.IsShow():
					self.wndDragonSoul.Show()
				else:
					self.wndDragonSoul.Close()

	def FailDragonSoulRefine(self, reason, inven_type, inven_pos):
		if False == player.IsObserverMode():
			if app.ENABLE_DRAGON_SOUL_SYSTEM:
				if True == self.wndDragonSoulRefine.IsShow():
					self.wndDragonSoulRefine.RefineFail(reason, inven_type, inven_pos)

	def SucceedDragonSoulRefine(self, inven_type, inven_pos):
		if False == player.IsObserverMode():
			if app.ENABLE_DRAGON_SOUL_SYSTEM:
				if True == self.wndDragonSoulRefine.IsShow():
					self.wndDragonSoulRefine.RefineSucceed(inven_type, inven_pos)

	if app.ENABLE_DS_CHANGE_ATTR:
		def OpenDragonSoulRefineWindow(self, type):
			if False == player.IsObserverMode():
				if app.ENABLE_DRAGON_SOUL_SYSTEM:
					if False == self.wndDragonSoulRefine.IsShow():
						self.wndDragonSoulRefine.SetWindowType(type)
						self.wndDragonSoulRefine.Show()
						if None != self.wndDragonSoul:
							if False == self.wndDragonSoul.IsShow():
								self.wndDragonSoul.Show()
	else:
		def OpenDragonSoulRefineWindow(self):
			if False == player.IsObserverMode():
				if app.ENABLE_DRAGON_SOUL_SYSTEM:
					if False == self.wndDragonSoulRefine.IsShow():
						self.wndDragonSoulRefine.Show()
						if None != self.wndDragonSoul:
							if False == self.wndDragonSoul.IsShow():
								self.wndDragonSoul.Show()

	def CloseDragonSoulRefineWindow(self):
		if False == player.IsObserverMode():
			if app.ENABLE_DRAGON_SOUL_SYSTEM:
				if True == self.wndDragonSoulRefine.IsShow():
					self.wndDragonSoulRefine.Close()


	def ToggleGuildWindow(self):
		if not self.wndGuild.IsShow():
			if self.wndGuild.CanOpen():
				self.wndGuild.Open()
			else:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GUILD_YOU_DO_NOT_JOIN)
		else:
			self.wndGuild.OverOutItem()
			self.wndGuild.Hide()

	def ToggleChatLogWindow(self):
		if self.wndChatLog.IsShow():
			self.wndChatLog.Hide()
		else:
			self.wndChatLog.Show()

	if app.ENABLE_AUTO_SHOUT:
		def AutoShoutButton(self):
			if constInfo.AUTO_SHOUT_ACTIVATED:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AUTO_SHOUT_INFO2)
				constInfo.AUTO_SHOUT_ACTIVATED = 0
			else:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AUTO_SHOUT_INFO)
				constInfo.AUTO_SHOUT_ACTIVATED = 1
				if constInfo.LAST_SHOUT_MESSAGE == "":
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AUTO_SHOUT_INFO3)

	def ToggleMarmurShopWindow(self):
		if self.wndPolySystem.IsShow():
			self.wndPolySystem.Close()
		else:
			net.SendPolyOpen()

	def ToggleBonusWindow(self):
		if self.wndBonus.IsShow():
			self.wndBonus.Close()
		else:
			self.wndBonus.Open()

	def OpenBlendWindow(self):
		if self.wndBlend.IsShow():
			self.wndBlend.Close()
		else:
			self.wndBlend.Open()

	def ToggleCollectWindow(self):
		if self.wndCollectWindow.IsShow():
			self.wndCollectWindow.Close()
		else:
			self.wndCollectWindow.Open()

	def ToggleWeeklyRankWindow(self):
		if self.wndWeeklyRankWindow_New.IsShow():
			self.wndWeeklyRankWindow_New.Close()
		else:
			self.wndWeeklyRankWindow_New.Open()
			
	def OpenRuneWindow(self):
		if self.wndInventory:
			self.wndInventory.ClickRuneButton()
			
	def OpenChangeEquipmentWindow(self):
		if self.wndInventory:
			self.wndInventory.ClickFastEquipButton()
	
	def OpenStorageWindow(self):
		if self.wndInventory:
			self.wndInventory.DepoIsToggle()
			
	if app.ENABLE_SWITCHBOT:
		def ToggleSwitchbotWindow(self):
			if self.wndSwitchbot.IsShow():
				self.wndSwitchbot.Close()
			else:
				self.wndSwitchbot.Open()
				
		def RefreshSwitchbotWindow(self):
			if self.wndSwitchbot and self.wndSwitchbot.IsShow():
				self.wndSwitchbot.RefreshSwitchbotWindow()

		def RefreshSwitchbotItem(self, slot):
			if self.wndSwitchbot and self.wndSwitchbot.IsShow():
				self.wndSwitchbot.RefreshSwitchbotItem(slot)

	def OpenRemoveItem(self):
		if self.wndRemoveItem.IsShow():
			self.wndRemoveItem.Hide()
		else:
			self.wndRemoveItem.OpenWindow(self.wndInventory)

	def CheckGameButton(self):
		if self.wndGameButton:
			self.wndGameButton.CheckGameButton()

	def __OnClickStatusPlusButton(self):
		self.ToggleCharacterWindow("STATUS")


	def __OnClickQuestButton(self):
		self.ToggleCharacterWindow("QUEST")

	if app.ENABLE_KEYCHANGE_SYSTEM:
		def ToggleHelpWindow(self):
			if self.wndHelp.IsShow():
				self.CloseHelpWindow()
			else:
				self.OpenHelpWindow()


	def __OnClickBuildButton(self):
		self.BUILD_OpenWindow()

	def OpenHelpWindow(self):
		self.wndUICurtain.Show()
		self.wndHelp.Open()

	def CloseHelpWindow(self):
		self.wndUICurtain.Hide()
		self.wndHelp.Close()

	def OpenWebWindow(self, url):
		self.wndWeb.Open(url)

		self.wndChat.CloseChat()

	if constInfo.GIFT_CODE_SYSTEM:
		def OpenGiftCodeWindow(self):
			if self.wndGiftCodeWindow.IsShow():
				self.wndGiftCodeWindow.Hide()
			else:
				self.wndGiftCodeWindow.Show()

	def ShowGift(self):
		self.wndTaskBar.ShowGift()

	def CloseWbWindow(self):
		self.wndWeb.Close()
		
	def OpenCardsInfoWindow(self):
		self.wndCardsInfo.Open()
		
	def OpenCardsWindow(self, safemode):
		self.wndCards.Open(safemode)
		
	def UpdateCardsInfo(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, hand_4, hand_4_v, hand_5, hand_5_v, cards_left, points):
		self.wndCards.UpdateCardsInfo(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, hand_4, hand_4_v, hand_5, hand_5_v, cards_left, points)
		
	def UpdateCardsFieldInfo(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points):
		self.wndCards.UpdateCardsFieldInfo(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points)
		
	def CardsPutReward(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points):
		self.wndCards.CardsPutReward(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points)
		
	def CardsShowIcon(self):
		self.wndCardsIcon.Show()

	if app.ENABLE_CUBE_RENEWAL_WORLDARD:
		def BINARY_CUBE_RENEWAL_OPEN(self):
			self.wndCubeRenewal.Show()

	def OpenCubeWindow(self):
		self.wndCube.Open()

		if False == self.wndInventory.IsShow():
			self.wndInventory.Show()

	def CloseCubeWindow(self):
		self.wndCube.Close()
	
	def GetCubeWindow(self):
		return self.wndCube

	if app.ENABLE_ACCE_COSTUME_SYSTEM:
		def ActAcce(self, iAct, bWindow):
			board = (self.wndAcceAbsorption,self.wndAcceCombine)[int(bWindow)]
			if iAct == 1:
				self.ActAcceOpen(board)
			elif iAct == 2:
				self.ActAcceClose(board)
			elif iAct == 3 or iAct == 4:
				self.ActAcceRefresh(board, iAct)

		def ActAcceOpen(self,board):
			if not board.IsOpened():
				board.Open()
			if not self.wndInventory.IsShow():
				self.wndInventory.Show()
			self.wndInventory.RefreshBagSlotWindow()

		def ActAcceClose(self,board):
			if board.IsOpened():
				board.Close()
			self.wndInventory.RefreshBagSlotWindow()

		def ActAcceRefresh(self,board,iAct):
			if board.IsOpened():
				board.Refresh(iAct)
			self.wndInventory.RefreshBagSlotWindow()

	if app.ENABLE_AURA_SYSTEM:
		def ActAura(self, iAct, bWindow):
			if iAct == 1:
				if bWindow == True:
					if not self.wndAuraRefine.IsOpened():
						self.wndAuraRefine.Open()
				else:
					if not self.wndAuraAbsorption.IsOpened():
						self.wndAuraAbsorption.Open()
				
				self.wndInventory.RefreshBagSlotWindow()
			elif iAct == 2:
				if bWindow == True:
					if self.wndAuraRefine.IsOpened():
						self.wndAuraRefine.Close()
				else:
					if self.wndAuraAbsorption.IsOpened():
						self.wndAuraAbsorption.Close()
				
				self.wndInventory.RefreshBagSlotWindow()
			elif iAct == 3 or iAct == 4:
				if bWindow == True:
					if self.wndAuraRefine.IsOpened():
						self.wndAuraRefine.Refresh(iAct)
				else:
					if self.wndAuraAbsorption.IsOpened():
						self.wndAuraAbsorption.Refresh(iAct)
				
				self.wndInventory.RefreshBagSlotWindow()

	if app.ENABLE_RESP_SYSTEM:
		def OpenRespWindow(self):
			#pass
			if self.wndResp.IsShow():
				self.wndResp.Hide()
			else:
				self.wndResp.Show()

		def ClearLocationWindow(self):
			self.wndLocationWindow.ClearData()

		def UpdateLocationWindowPos(self, position, index, posx, posy):
			self.wndLocationWindow.AppendPosition(position, index, posx, posy)

		def UpdateLocationWindowName(self, name):
			self.wndLocationWindow.AppendName(name)

	if app.ENABLE_DUNGEON_INFO_SYSTEM:
		def ToggleDungeonInfoWindow(self):
			#if False == player.IsObserverMode():
			if False == self.wndDungeonInfo.IsShow():
				self.wndDungeonInfo.Open()
			else:
				self.wndDungeonInfo.Close()

		def DungeonInfoOpen(self):
			if self.wndDungeonInfo:
				self.wndDungeonInfo.OnOpen()

		def DungeonRankingRefresh(self):
			if self.wndDungeonInfo:
				self.wndDungeonInfo.OnRefreshRanking()

		def DungeonInfoReload(self, onReset):
			if self.wndDungeonInfo:
				self.wndDungeonInfo.OnReload(onReset)

	def __HideWindows(self):
		hideWindows = self.wndTaskBar,\
						self.wndCharacter,\
						self.wndInventory,\
						self.wndMiniMap,\
						self.wndGuild,\
						self.wndMessenger,\
						self.yangText,\
						self.wndChat,\
						self.wndParty,\
						self.wndGameButton,

		if self.wndWonExchange:
			hideWindows += self.wndWonExchange,
			
		if self.wndRankingWindow:
			hideWindows += self.wndRankingWindow,
			
		if app.ENABLE_SECONDARY_LEVEL:
			hideWindows += self.wndSecondaryLevel,
			
		if self.wndRankingWindowWeekly:
			hideWindows += self.wndRankingWindowWeekly,

		if self.wndEnergyBar:
			hideWindows += self.wndEnergyBar,
		
		if self.wndCasketPreview:
			hideWindows += self.wndCasketPreview,

		if self.wndExpandedTaskBar:
			hideWindows += self.wndExpandedTaskBar,

		if app.ENABLE_COLLECTIONS_SYSTEM:
			hideWindows += self.wndCollections,

		if self.wndRemoveItem:
			hideWindows += self.wndRemoveItem,

		if app.ENABLE_ODLAMKI_SYSTEM:
			if self.wndOdlamki:
				hideWindows += self.wndOdlamki,

		if app.ENABLE_OFFLINE_SHOP:
			if self.wndShopOffline:
				hideWindows += self.wndShopOffline,
			if self.wndShopSearch:
				hideWindows += self.wndShopSearch,
			if self.wndShopNotification:
				hideWindows += self.wndShopNotification,
		

		if app.__ENABLE_POLYMORPH_SYSTEM__ and self.wndPolySystem:
			hideWindows += self.wndPolySystem,

		if app.WJ_SPLIT_INVENTORY_SYSTEM:
			if self.wndExtendedInventory:
				hideWindows += self.wndExtendedInventory,

		if app.ENABLE_RESP_SYSTEM and self.wndResp:
			hideWindows += self.wndResp,

		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			hideWindows += self.wndDragonSoul,\
						self.wndDragonSoulRefine,

		for index, window in enumerate(self.interfaceWindowList):
			hideWindows += self.interfaceWindowList[window],
   
		if app.ENABLE_SWITCHBOT and self.wndSwitchbot:
			hideWindows += self.wndSwitchbot,
			
		if self.messenger_bg:
			hideWindows += self.messenger_bg,
		if self.messenger:
			hideWindows += self.messenger,

		if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
			if self.wndBuffNPCWindow:
				hideWindows += self.wndBuffNPCWindow,
			if self.wndBuffNPCCreateWindow:
				hideWindows += self.wndBuffNPCCreateWindow,

		if app.ENABLE_VS_SHOP_SEARCH:
			if self.wndOfflineShopSearch:
				hideWindows += self.wndOfflineShopSearch,

		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			if self.wndDungeonInfo:
				hideWindows += self.wndDungeonInfo,
				
		if app.ENABLE_MINIMAP_DUNGEONINFO:
			if self.wndMiniMapDungeonInfo:
				hideWindows += self.wndMiniMapDungeonInfo,

		if app.ENABLE_SAVE_LOCATION_SYSTEM:
			hideWindows += self.wndSaveLocation,

		if constInfo.ENABLE_EXPANDED_MONEY_TASKBAR:
			if self.wndExpandedMoneyTaskBar:
				hideWindows += self.wndExpandedMoneyTaskBar,
				
		if self.wndgameOption:
			hideWindows += self.wndgameOption,

		if app.TAKE_LEGEND_DAMAGE_BOARD_SYSTEM:
			if self.wndLegendDamageWindow:
				hideWindows += self.wndLegendDamageWindow,

		if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
			hideWindows += self.wndPrivateShopPanel,\
						self.wndPrivateShopSearch

		if app.__AUTO_SKILL_READER__ and self.wndAutoSkillReader and self.wndAutoSkillReader.IsShow():
			hideWindows += self.wndAutoSkillReader,

		hideWindows = filter(lambda x:x.IsShow(), hideWindows)
		map(lambda x:x.Hide(), hideWindows)

		self.HideAllQuestButton()
		self.HideAllWhisperButton()

		if self.wndChat.IsEditMode():
			self.wndChat.CloseChat()

		return hideWindows

	def ToggleWonExchangeWindow(self):
		if player.IsObserverMode():
			return

		if False == self.wndWonExchange.IsShow():
			self.wndWonExchange.SetPage(uiWonExchange.WonExchangeWindow.PAGE_SELL)
			self.wndWonExchange.Show()
			self.wndWonExchange.SetTop()
		else:
			self.wndWonExchange.Hide()

	def __ShowWindows(self, wnds):
		map(lambda x:x.Show(), wnds)
		global IsQBHide
		if not IsQBHide:
			self.ShowAllQuestButton()
		else:
			self.HideAllQuestButton()

		self.ShowAllWhisperButton()

	def BINARY_OpenAtlasWindow(self):
		if self.wndMiniMap:
			self.wndMiniMap.ShowAtlas()

	if app.ENABLE_COLLECTIONS_SYSTEM:
		def ToggleCollectionWindow(self):
			if self.wndCollections.IsShow():
				self.wndCollections.Hide()
			else:
				self.wndCollections.Show()

	def BINARY_SetObserverMode(self, flag):
		self.wndGameButton.SetObserverMode(flag)

	if app.ENABLE_REFINE_RENEWAL:
		def CheckRefineDialog(self, isFail):
			self.dlgRefineNew.CheckRefine(isFail)

	def BINARY_OpenSelectItemWindow(self):
		self.wndItemSelect.Open()


	def OpenPrivateShopInputNameDialog(self):

		inputDialog = uiCommon.InputDialog()
		inputDialog.SetTitle(localeInfo.PRIVATE_SHOP_INPUT_NAME_DIALOG_TITLE)
		inputDialog.SetMaxLength(32)
		inputDialog.SetAcceptEvent(ui.__mem_func__(self.OpenPrivateShopBuilder))
		inputDialog.SetCancelEvent(ui.__mem_func__(self.ClosePrivateShopInputNameDialog))
		inputDialog.Open()
		self.inputDialog = inputDialog

	def ClosePrivateShopInputNameDialog(self):
		self.inputDialog = None
		return True

	def OpenPrivateShopBuilder(self):

		if not self.inputDialog:
			return True

		if not len(self.inputDialog.GetText()):
			return True

		self.privateShopBuilder.Open(self.inputDialog.GetText())
		self.ClosePrivateShopInputNameDialog()
		return True

	def AppearPrivateShop(self, vid, text):

		board = uiPrivateShopBuilder.PrivateShopAdvertisementBoard()
		board.Open(vid, text)

		self.privateShopAdvertisementBoardDict[vid] = board

	def DisappearPrivateShop(self, vid):

		if not self.privateShopAdvertisementBoardDict.has_key(vid):
			return

		del self.privateShopAdvertisementBoardDict[vid]
		uiPrivateShopBuilder.DeleteADBoard(vid)

	def UpdateBonusChanger(self):
		if self.wndChangerWindow:
			self.wndChangerWindow.OnUpdate()
	
	def AddToBonusChange(self, item1, item2):
		if self.wndChangerWindow:
			self.wndChangerWindow.AddItems(item1, item2)


	def OpenEquipmentDialog(self, vid):
		dlg = uiEquipmentDialog.EquipmentDialog()
		dlg.SetItemToolTip(self.tooltipItem)
		dlg.SetCloseEvent(ui.__mem_func__(self.CloseEquipmentDialog))
		dlg.Open(vid)

		self.equipmentDialogDict[vid] = dlg

	def SetEquipmentDialogItem(self, vid, slotIndex, vnum, count):
		if not vid in self.equipmentDialogDict:
			return
		self.equipmentDialogDict[vid].SetEquipmentDialogItem(slotIndex, vnum, count)

	def SetEquipmentDialogSocket(self, vid, slotIndex, socketIndex, value):
		if not vid in self.equipmentDialogDict:
			return
		self.equipmentDialogDict[vid].SetEquipmentDialogSocket(slotIndex, socketIndex, value)

	def SetEquipmentDialogAttr(self, vid, slotIndex, attrIndex, type, value):
		if not vid in self.equipmentDialogDict:
			return
		self.equipmentDialogDict[vid].SetEquipmentDialogAttr(slotIndex, attrIndex, type, value)

	def CloseEquipmentDialog(self, vid):
		if not vid in self.equipmentDialogDict:
			return
		del self.equipmentDialogDict[vid]


	def BINARY_ClearQuest(self, index):
		btn = self.__FindQuestButton(index)
		if 0 != btn:
			self.__DestroyQuestButton(btn)

	def RecvQuest(self, index, name):
		self.BINARY_RecvQuest(index, name, "file", localeInfo.GetLetterImageName())

	def BINARY_RecvQuest(self, index, name, iconType, iconName):

		btn = self.__FindQuestButton(index)
		if 0 != btn:
			self.__DestroyQuestButton(btn)

		btn = uiWhisper.WhisperButton()

		import item
		if "item"==iconType:
			item.SelectItem(int(iconName))
			buttonImageFileName=item.GetIconImageFileName()
		else:
			buttonImageFileName=iconName

		if iconName and (iconType not in ("item", "file")):
			btn.SetUpVisual("d:/ymir work/ui/game/quest/questicon/%s" % (iconName.replace("open", "close")))
			btn.SetOverVisual("d:/ymir work/ui/game/quest/questicon/%s" % (iconName))
			btn.SetDownVisual("d:/ymir work/ui/game/quest/questicon/%s" % (iconName))
		else:
			if localeInfo.IsEUROPE():
				btn.SetUpVisual(localeInfo.GetLetterCloseImageName())
				btn.SetOverVisual(localeInfo.GetLetterOpenImageName())
				btn.SetDownVisual(localeInfo.GetLetterOpenImageName())
			else:
				btn.SetUpVisual(buttonImageFileName)
				btn.SetOverVisual(buttonImageFileName)
				btn.SetDownVisual(buttonImageFileName)
				btn.Flash()

		listOfTypes = iconType.split(",")
		if "blink" in listOfTypes:
			btn.Flash()

		listOfColors = {
			"golden":	0xFFffa200,
			"green":	0xFF00e600,
			"blue":		0xFF0099ff,
			"purple":	0xFFcc33ff,

			"fucsia":	0xFFcc0099,
			"aqua":		0xFF00ffff,
		}
		for k,v in listOfColors.iteritems():
			if k in listOfTypes:
				btn.ToolTipText.SetPackedFontColor(v)

		if not app.ENABLE_QUEST_RENEWAL:
			if localeInfo.IsARABIC():
				btn.SetToolTipText(name, -20, 55)
				btn.ToolTipText.SetHorizontalAlignRight()
			else:
				btn.SetToolTipText(name, -20, 55)
				btn.ToolTipText.SetHorizontalAlignLeft()

			btn.SetEvent(ui.__mem_func__(self.__StartQuest), btn)
			btn.Show()
		else:
			btn.SetEvent(ui.__mem_func__(self.__StartQuest), btn)
			
		btn.Show()

		btn.index = index
		btn.name = name

		self.questButtonList.insert(0, btn)
		self.__ArrangeQuestButton()

	def __ArrangeQuestButton(self):

		screenWidth = wndMgr.GetScreenWidth()
		screenHeight = wndMgr.GetScreenHeight()

		if self.wndParty.IsShow():
			xPos = 100 + 30
		else:
			xPos = 20

		if localeInfo.IsARABIC():
			xPos = xPos + 15

		yPos = 170 * screenHeight / 600
		yCount = (screenHeight - 330) / 63

		count = 0
		for btn in self.questButtonList:
		
			if app.ENABLE_QUEST_RENEWAL:
				btn.SetToolTipText(str(len(self.questButtonList)))
				btn.ToolTipText.SetPosition(13, 37)


			btn.SetPosition(xPos + (int(count/yCount) * 100), yPos + (count%yCount * 63))
			count += 1
			global IsQBHide
			if IsQBHide:
				btn.Hide()
			else:
				if app.ENABLE_QUEST_RENEWAL and count > 0:
					btn.Hide()
				else:
					btn.Show()

	def __StartQuest(self, btn):
		if app.ENABLE_QUEST_RENEWAL:
			self.__OnClickQuestButton()
			self.HideAllQuestButton()
		else:
			event.QuestButtonClick(btn.index)
			self.__DestroyQuestButton(btn)

	def __FindQuestButton(self, index):
		for btn in self.questButtonList:
			if btn.index == index:
				return btn

		return 0

	def __DestroyQuestButton(self, btn):
		btn.SetEvent(0)
		self.questButtonList.remove(btn)
		self.__ArrangeQuestButton()

	def HideAllQuestButton(self):
		for btn in self.questButtonList:
			btn.Hide()

	def ShowAllQuestButton(self):
		for btn in self.questButtonList:
			btn.Show()
			if app.ENABLE_QUEST_RENEWAL:
				break


	def __InitWhisper(self):
		chat.InitWhisper(self)

	def OpenWhisperDialogWithoutTarget(self):
		if not self.dlgWhisperWithoutTarget:
			dlgWhisper = uiWhisper.WhisperDialog(self.MinimizeWhisperDialog, self.CloseWhisperDialog)
			dlgWhisper.BindInterface(self)
			dlgWhisper.LoadDialog()
			dlgWhisper.OpenWithoutTarget(self.RegisterTemporaryWhisperDialog)
			dlgWhisper.SetPosition(30,30)
			dlgWhisper.Show()
			self.dlgWhisperWithoutTarget = dlgWhisper

			self.windowOpenPosition = (self.windowOpenPosition+1) % 5

		else:
			self.dlgWhisperWithoutTarget.SetTop()
			self.dlgWhisperWithoutTarget.textRenderer.SetTargetName("")
			self.dlgWhisperWithoutTarget.OpenWithoutTarget(self.RegisterTemporaryWhisperDialog)

	def RegisterTemporaryWhisperDialog(self, name):
		if not self.dlgWhisperWithoutTarget:
			return

		self.whisperDialogDict[name] = self.dlgWhisperWithoutTarget
		self.dlgWhisperWithoutTarget.OpenWithTarget(name)
		self.__CheckGameMaster(name)

	def OpenWhisperDialog(self, name):
		if not self.dlgWhisperWithoutTarget:
			dlgWhisper = uiWhisper.WhisperDialog(self.MinimizeWhisperDialog, self.CloseWhisperDialog)
			dlgWhisper.BindInterface(self)
			dlgWhisper.LoadDialog()
			dlgWhisper.OpenWithTarget(name)
			dlgWhisper.SetPosition(30,30)
			dlgWhisper.Show()
			self.dlgWhisperWithoutTarget = dlgWhisper

			self.windowOpenPosition = (self.windowOpenPosition+1) % 5

		else:
			self.dlgWhisperWithoutTarget.SetTop()
			self.dlgWhisperWithoutTarget.textRenderer.SetTargetName("")
			self.dlgWhisperWithoutTarget.OpenWithTarget(name)

	def RecvWhisper(self, name):
		if not name in constInfo.MESSENGER_NEWS_LIST:
			constInfo.MESSENGER_NEWS_LIST.append(name)

		self.messenger.SetUpVisual("zaris/whisper/new_message_alert.png")
		self.messenger.SetOverVisual("zaris/whisper/new_message_alert.png")
		self.messenger.SetDownVisual("zaris/whisper/new_message_alert.png")
		self.messenger.EnableFlash()
		self.messenger_bg.Show()
		#if app.FLASH_IN_TASKBAR:
		#	app.FlashApplication()

		if self.dlgWhisperWithoutTarget:
			self.dlgWhisperWithoutTarget.LoadMessengerList()
			self.dlgWhisperWithoutTarget.CheckGameMaster()

		snd.PlaySound("sound/ui/new_message.wav")
	
	def SetMessengerReaded(self):
		self.messenger_bg.Hide()
		self.messenger.DisableFlash()
		self.messenger.SetUpVisual("zaris/whisper/message_button.png")
		self.messenger.SetOverVisual("zaris/whisper/message_button.png")
		self.messenger.SetDownVisual("zaris/whisper/message_button.png")

	def MakeWhisperButton(self, name):
		return

	def ShowWhisperDialog(self, btn):
		try:
			self.__MakeWhisperDialog(btn.name)
			dlgWhisper = self.whisperDialogDict[btn.name]
			dlgWhisper.OpenWithTarget(btn.name)
			dlgWhisper.Show()
			self.__CheckGameMaster(btn.name)
		except:
			import dbg
			dbg.TraceError("interface.ShowWhisperDialog - Failed to find key")

		self.__DestroyWhisperButton(btn)

	def MinimizeWhisperDialog(self, name):
		self.CloseWhisperDialog(name)

	def CloseWhisperDialog(self, name):

		if self.dlgWhisperWithoutTarget:
			self.dlgWhisperWithoutTarget.Destroy()
			self.dlgWhisperWithoutTarget = None

		return

	def __ArrangeWhisperButton(self):

		screenWidth = wndMgr.GetScreenWidth()
		screenHeight = wndMgr.GetScreenHeight()

		xPos = screenWidth - 70
		yPos = 170 * screenHeight / 600
		yCount = (screenHeight - 330) / 63

		count = 0
		for button in self.whisperButtonList:

			button.SetPosition(xPos + (int(count/yCount) * -50), yPos + (count%yCount * 63))
			count += 1

	def __FindWhisperButton(self, name):
		for button in self.whisperButtonList:
			if button.name == name:
				return button

		return 0

	def __MakeWhisperDialog(self, name):
		dlgWhisper = uiWhisper.WhisperDialog(self.MinimizeWhisperDialog, self.CloseWhisperDialog)
		dlgWhisper.BindInterface(self)
		dlgWhisper.LoadDialog()
		dlgWhisper.SetPosition(self.windowOpenPosition*30,self.windowOpenPosition*30)
		self.whisperDialogDict[name] = dlgWhisper

		self.windowOpenPosition = (self.windowOpenPosition+1) % 5

		return dlgWhisper

	def __DestroyWhisperButton(self, button):
		button.SetEvent(0)
		self.whisperButtonList.remove(button)
		self.__ArrangeWhisperButton()

	def HideAllWhisperButton(self):
		for btn in self.whisperButtonList:
			btn.Hide()

	def ShowAllWhisperButton(self):
		for btn in self.whisperButtonList:
			btn.Show()

	def __CheckGameMaster(self, name):
		if not self.listGMName.has_key(name):
			return
		if self.whisperDialogDict.has_key(name):
			dlg = self.whisperDialogDict[name]
			dlg.SetGameMasterLook()

	def RegisterGameMasterName(self, name):
		if self.listGMName.has_key(name):
			return
		self.listGMName[name] = "GM"

	def IsGameMasterName(self, name):
		if self.listGMName.has_key(name):
			return True
		else:
			return False

	if app.TAKE_LEGEND_DAMAGE_BOARD_SYSTEM:
		def OpenLegendDamageWindow(self, vid):
			#import dbg
			#dbg.TraceError("OpenLegendDamageWindow: %d ", vid)
			self.wndLegendDamageWindow.Show(vid)

		def SendLegendDamageData(self, vid, pos, name, level, race, empire, damage):
			if vid not in constInfo.LEGEND_DAMAGE_DATA:
				constInfo.LEGEND_DAMAGE_DATA[vid] = {
					"NAME": [None] * 15,
					"LEVEL": [None] * 15,
					"RACE": [None] * 15,
					"EMPIRE": [None] * 15,
					"DAMAGE": [None] * 15
				}

			damage_data = constInfo.LEGEND_DAMAGE_DATA[vid]
			damage_data["NAME"][pos] = name
			damage_data["LEVEL"][pos] = level
			damage_data["RACE"][pos] = race
			damage_data["EMPIRE"][pos] = empire
			damage_data["DAMAGE"][pos] = damage



	def BUILD_OpenWindow(self):
		self.wndGuildBuilding = uiGuild.BuildGuildBuildingWindow()
		self.wndGuildBuilding.Open()
		self.wndGuildBuilding.wnds = self.__HideWindows()
		self.wndGuildBuilding.SetCloseEvent(ui.__mem_func__(self.BUILD_CloseWindow))

	def BUILD_CloseWindow(self):
		self.__ShowWindows(self.wndGuildBuilding.wnds)
		self.wndGuildBuilding = None

	def BUILD_OnUpdate(self):
		if not self.wndGuildBuilding:
			return

		if self.wndGuildBuilding.IsPositioningMode():
			import background
			x, y, z = background.GetPickingPoint()
			self.wndGuildBuilding.SetBuildingPosition(x, y, z)

	def BUILD_OnMouseLeftButtonDown(self):
		if not self.wndGuildBuilding:
			return

		if self.wndGuildBuilding.IsPositioningMode():
			self.wndGuildBuilding.SettleCurrentPosition()
			return True
		elif self.wndGuildBuilding.IsPreviewMode():
			pass
		else:
			return True
		return False

	def BUILD_OnMouseLeftButtonUp(self):
		if not self.wndGuildBuilding:
			return

		if not self.wndGuildBuilding.IsPreviewMode():
			return True

		return False

	def BULID_EnterGuildArea(self, areaID):
		mainCharacterName = player.GetMainCharacterName()
		masterName = guild.GetGuildMasterName()

		if mainCharacterName != masterName:
			return

		if areaID != player.GetGuildID():
			return

		self.wndGameButton.ShowBuildButton()

	def BULID_ExitGuildArea(self, areaID):
		self.wndGameButton.HideBuildButton()
		
	if app.__BL_CHEST_DROP_INFO__:
		def OpenChestDropWindow(self, itemVnum, isMain):
			if self.wndChestDropInfo:
				self.wndChestDropInfo.Open(itemVnum, isMain)

	def IsEditLineFocus(self):
		if self.ChatWindow.chatLine.IsFocus():
			return 1

		if self.ChatWindow.chatToLine.IsFocus():
			return 1

		return 0

	def ActiveTileNow(self, id):
		self.wndWeeklyRankWindow_New.Active(id)

	def GetActiveTitleNow(self):
		self.wndWeeklyRankWindow_New.GetActiveNow()

	def TitleEnable(self, id, val):
		self.wndWeeklyRankWindow_New.Enable(id, val)

	def OfflineShopLogs(self, id, item, count, price, price2, date, action):
		self.wndOfflineShopLogPanel.SendLogs(id, item, count, price, price2, date, action)

	def SelectPage(self, page, season):
		self.wndWeeklyRankWindow_New.PutPage(page, season)

	def SendWeeklyInfo(self, pos, name, points, empire, job):
		self.wndWeeklyRankWindow_New.LoadPage(pos, name, points, empire, job)


	if app.BL_MOVE_CHANNEL:
		def RefreshServerInfo(self, channelNumber):
			if self.wndMiniMap:
				self.wndMiniMap.RefreshServerInfo(channelNumber)

	def UseDSSButtonEffect(self, enable):
		if self.wndInventory:
			self.wndInventory.UseDSSButtonEffect(enable)
	
	def EmptyFunction(self):
		pass

	def GetInventoryPageIndex(self):
		if self.wndInventory:
			return self.wndInventory.GetInventoryPageIndex()
		else:
			return -1

	if app.WJ_ENABLE_TRADABLE_ICON:
		def SetOnTopWindow(self, onTopWnd):
			self.onTopWindow = onTopWnd

		def GetOnTopWindow(self):
			return self.onTopWindow

		def RefreshMarkInventoryBag(self):
			self.wndInventory.RefreshMarkSlots()

	def MouseSlotEventClear(self):
		if self.wndInventory:
			self.wndInventory.MouseSlotEventClear()
			self.RefreshMarkInventoryBag()
		
		if self.wndDragonSoul:
			self.wndDragonSoul.MouseSlotEventClear()
			self.RefreshMarkInventoryBag()
		
		if self.wndExtendedInventory:
			self.wndExtendedInventory.MouseSlotEventClear()
			self.RefreshMarkInventoryBag()
					
	if app.ENABLE_LOADING_PERFORMANCE:
		def OpenWarpShowerWindow(self):
			self._atlasWasOpen = self.wndMiniMap and self.wndMiniMap.IsShowingAtlas()
			if self._atlasWasOpen:
				self.wndMiniMap.ToggleAtlasWindow()

			if self.dlgSystem:
				self.dlgSystem.Close()
				self.dlgSystem.Destroy()

			self.HideAllQuestButton()
			self.HideAllWhisperButton()

			self.HideAllWindows()

			self.wndWarpShower.Open()

		def CloseWarpShowerWindow(self):
			if self.wndWarpShower:
				self.wndWarpShower.Close()
			self.ShowDefaultWindows()
			if getattr(self, "_atlasWasOpen", False):
				self._atlasWasOpen = False
				if self.wndMiniMap:
					self.wndMiniMap.ShowAtlasWhenReady()

	if app.ENABLE_EVENT_MANAGER:
		def MakeEventIcon(self):
			if self.wndEventIcon == None:
				self.wndEventIcon = uiEventCalendar.MovableImage()
				self.wndEventIcon.Show()
		def MakeEventCalendar(self):
			if self.wndEventManager == None:
				self.wndEventManager = uiEventCalendar.EventCalendarWindow()
		def OpenEventCalendar(self):
			self.MakeEventCalendar()
			if self.wndEventManager.IsShow():
				self.wndEventManager.Close()
			else:
				self.wndEventManager.Open()
		def RefreshEventStatus(self, eventID, eventStatus, eventendTime, eventEndTimeText):
			if eventendTime != 0:
				eventendTime += app.GetGlobalTimeStamp()
			uiEventCalendar.SetEventStatus(eventID, eventStatus, eventendTime, eventEndTimeText)
			self.RefreshEventManager()
		def ClearEventManager(self):
			uiEventCalendar.server_event_data={}
		def RefreshEventManager(self):
			if self.wndEventManager:
				self.wndEventManager.Refresh()
			if self.wndEventIcon:
				self.wndEventIcon.Refresh()
		def AppendEvent(self, dayIndex, eventID, eventIndex, startTime, endTime, empireFlag, channelFlag, value0, value1, value2, value3, startRealTime, endRealTime, isAlreadyStart):
			self.MakeEventCalendar()
			self.MakeEventIcon()
			if startRealTime != 0:
				startRealTime += app.GetGlobalTimeStamp()
			if endRealTime != 0:
				endRealTime += app.GetGlobalTimeStamp()
			uiEventCalendar.SetServerData(dayIndex, eventID, eventIndex, startTime, endTime, empireFlag, channelFlag, value0, value1, value2, value3, startRealTime, endRealTime, isAlreadyStart)

		def _show_personal_tracker(self):
			if self.wndPersonalEventTracker is None:
				self.wndPersonalEventTracker = uiEventCalendar.PersonalEventTracker()
				saved = uiEventCalendar._tracker_saved_pos
				if saved:
					self.wndPersonalEventTracker.SetPosition(saved[0], saved[1])
				else:
					self.wndPersonalEventTracker.SetPosition(wndMgr.GetScreenWidth() - 220, 200)
			self.wndPersonalEventTracker.Refresh()

		def ClearPersonalActivatedEvents(self):
			uiEventCalendar.personal_activated_events.clear()
			if self.wndEventManager:
				self.wndEventManager.RefreshDayPanel()
			if self.wndPersonalEventTracker:
				self.wndPersonalEventTracker.Hide()

		def SetPersonalEventActivated(self, eventID, activationTime=0):
			import time
			ts = activationTime if activationTime > 0 else int(time.time())
			uiEventCalendar.personal_activated_events[eventID] = ts
			if self.wndEventManager:
				self.wndEventManager.RefreshDayPanel()
			# Show tracker if the event is still within its active window.
			# This handles both rewarp and login with currently-running events.
			duration = uiEventCalendar.EVENT_PERSONAL_DURATION
			for day_events in uiEventCalendar.server_event_data.values():
				if eventID in day_events:
					val0 = day_events[eventID][uiEventCalendar.EVENT_VALUE0]
					if val0 > 0:
						duration = val0
					break
			if (time.time() - ts) < duration:
				self._show_personal_tracker()

		def EventActivateResult(self, eventID, result):
			import time
			if result == 0:  # EVENT_ACTIVATE_SUCCESS
				uiEventCalendar.personal_activated_events[eventID] = int(time.time())
			if self.wndEventManager:
				self.wndEventManager.OnEventActivateResult(eventID, result)
			# Only show tracker when player manually activates an event
			if result == 0:
				self._show_personal_tracker()

	if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
		def OpenPrivateShopPanel(self):
			if self.wndPrivateShopPanel:
				self.wndPrivateShopPanel.Open()
				
			if not self.wndInventory.IsShow():
				self.wndInventory.Show()
				
		def ClosePrivateShopPanel(self):
			if self.wndPrivateShopPanel:
				self.wndPrivateShopPanel.Close(False)
				
		def RefreshPrivateShopWindow(self):
			if self.wndPrivateShopPanel:
				self.wndPrivateShopPanel.Refresh()
				self.wndPrivateShopPanel.RefreshWindow()
				
		def TogglePrivateShopPanelWindow(self):
			if False == player.IsObserverMode():
				if not self.wndPrivateShopPanel.RequestOpen():
					self.wndPrivateShopPanel.Close()
					
		def TogglePrivateShopPanelWindowCheck(self):
			if False == player.IsObserverMode():
				if not self.wndPrivateShopPanel.RequestOpen(True):
					self.wndPrivateShopPanel.Close()

		def OpenPrivateShopSearch(self, mode):
			if self.wndPrivateShopSearch:
				self.wndPrivateShopSearch.Open(mode)
				
		def PrivateShopSearchUpdate(self, index, state):
			if self.wndPrivateShopSearch:
				self.wndPrivateShopSearch.UpdateResult(index, state)

		def PrivateShopSearchRefresh(self):
			if self.wndPrivateShopSearch:
				self.wndPrivateShopSearch.RefreshPage()
				
		def AppendMarketItemPrice(self, gold, cheque):
			if self.wndPrivateShopPanel and self.wndPrivateShopPanel.IsShow():
				self.wndPrivateShopPanel.AppendMarketItemPrice(gold, cheque)
				
			elif self.self.privateShopBuilder and self.self.privateShopBuilder.IsShow():
				self.privateShopBuilder.AppendMarketItemPrice(gold, cheque)
				
		def AddPrivateShopTitleBoard(self, vid, text, type):

			board = uiPrivateShop.PrivateShopTitleBoard(type)
			board.Open(vid, text)

			self.privateShopAdvertisementBoardDict[vid] = board

		def RemovePrivateShopTitleBoard(self, vid):

			if not self.privateShopAdvertisementBoardDict.has_key(vid):
				return

			del self.privateShopAdvertisementBoardDict[vid]
			uiPrivateShop.DeleteTitleBoard(vid)
			
		def SetPrivateShopPremiumBuild(self):
			if self.wndPrivateShopPanel:
				self.wndPrivateShopPanel.SetPremiumBuildMode()
				self.wndPrivateShopPanel.RefreshWindow()
				
		def PrivateShopStateUpdate(self):
			if self.wndPrivateShopPanel:
				self.wndPrivateShopPanel.OnStateUpdate()
				
	def ToggleBattlePass(self):
		if False == player.IsObserverMode():
			if False == self.wndBattlePass.IsShow():
				self.wndBattlePass.Open()
				self.wndBattlePass.SetTop()
			else:
				self.wndBattlePass.Close()

	def EmptyFunction(self):
		pass

	def GetWindowByType(self, type):
		windows = {
			player.INVENTORY: self.wndInventory,
			player.DRAGON_SOUL_INVENTORY: self.wndDragonSoul
		}
		
		return windows.get(type, None)

	def GetInventory(self):
		return self.wndInventory
		
	if app.ENABLE_SAVE_LOCATION_SYSTEM:
		def __MakeSaveLocationWindow(self):
			self.wndSaveLocation = uiSaveLocation.SaveLocationWindow()
			if self.wndSaveLocation.IsShow():
				self.wndSaveLocation.Show()

		def ToggleSaveLocationWindow(self):
			if not self.wndSaveLocation:
				return

			if player.IsObserverMode():
				return

			if not self.wndSaveLocation.IsShow():
				self.wndSaveLocation.Show()
				self.wndSaveLocation.SetTop()
			else:
				self.wndSaveLocation.Close()

		def UpdateSaveLocation(self, pos, name, x, y):
			if self.wndSaveLocation:
				self.wndSaveLocation.UpdateSaveLocation(pos, name, x, y)

		def DeleteSaveLocation(self, pos):
			if self.wndSaveLocation:
				self.wndSaveLocation.DeleteSaveLocation(pos)

	def ToggleRankingWindow(self):
		if self.wndRankingWindow.IsShow():
			self.wndRankingWindow.Close()
		else:
			self.wndRankingWindow.OpenWindow()

	def ToggleAntyExp(self):
		net.SendChatPacket("/toggle_antyexp")
			
	if app.ENABLE_SECONDARY_LEVEL:
		def ToggleSecondaryLevel(self):
			if self.wndSecondaryLevel.IsShow():
				self.wndSecondaryLevel.Close()
			else:
				self.wndSecondaryLevel.Open()

	def ToggleRankingWindowWeekly(self):
		if self.wndRankingWindowWeekly.IsShow():
			self.wndRankingWindowWeekly.Close()
		else:
			self.wndRankingWindowWeekly.OpenWindow()

	if app.WORLD_BOSS_YUMA:
		def WorldbossNotification(self, szString):
			for i in range(len(constInfo.WORLD_BOSS_TEXT_POSITION)):
				if constInfo.WORLD_BOSS_TEXT_POSITION[i] == 0:
					constInfo.WORLD_BOSS_TEXT_POSITION[i] = 1
					self.WorldbossHwnd[i] = uiworldbosstext.WorldbossNotification(szString, i)
					break
					
	if app.__PENDANT__:
		def SetPendantPageIdx(self, pageIdx, secondOpened):
			self.wndInventory.SetPendantPageIdx(int(pageIdx), int(secondOpened))
					
	if app.__SPIN_WHEEL__:
		def SetSpinReward(self, selectedItemIdx, selectedItemCount, cmdData):
			if self.wndSpinWheel:
				self.wndSpinWheel.SetSpinReward(selectedItemIdx, selectedItemCount, cmdData)
		def SetSpinWheel(self, count, show):
			if self.wndSpinWheel == None:
				self.wndSpinWheel = uiSpinWheel.Window()
			self.wndSpinWheel.SetSpinData(int(count))
			if int(show):
				self.wndSpinWheel.Open()
			else:
				self.wndSpinWheel.Close()
				
	if app.ENABLE_TITLE_ACHIEVEMENT_SYSTEM:
		def OpenTitleAchievement(self):
			if self.wndTitleAchievement.IsShow():
				self.wndTitleAchievement.Close()
			else:
				self.wndTitleAchievement.Open()


	def OpenWorldboss(self):
		if self.wndWorldBoss.IsShow():
			self.wndWorldBoss.Close()
		else:
			self.wndWorldBoss.Open()

	if app.ENABLE_WHEEL_OF_FORTUNE:
		def MakeWheelofFortune(self):
			if self.wndWheelofFortune == None:
				self.wndWheelofFortune = uiWheelofFortune.WheelofFortune()
		def OpenWheelofFortune(self):
			self.MakeWheelofFortune()
			if self.wndWheelofFortune.IsShow():
				self.wndWheelofFortune.Close()
			else:
				self.wndWheelofFortune.Open()
		def SetItemData(self, cmd):
			self.MakeWheelofFortune()
			self.wndWheelofFortune.SetItemData(str(cmd))
		def OnSetWhell(self, giftIndex):
			self.MakeWheelofFortune()
			self.wndWheelofFortune.OnSetWhell(int(giftIndex))
		def GetGiftData(self, itemVnum, itemCount):
			self.MakeWheelofFortune()
			self.wndWheelofFortune.GetGiftData(int(itemVnum), int(itemCount))

	if app.ENABLE_PVP_RANKING:
		def PVP_ClearPlayerInfo(self):
			self.wndPvPRanking.ClearPlayerInfo()
		
		if app.ENABLE_PVP_RANKING_WITH_EMPIRE:
			def PVP_AddPlayerInfo(self, chName, chLevel, chRace, chEmpire, chKill, chDead, maxKill, IsRankEmpire, empire1, empire2, empire3):
				self.wndPvPRanking.AddPlayerInfo(chName, chLevel, chRace, chEmpire, chKill, chDead, maxKill, IsRankEmpire, empire1, empire2, empire3)
		else:
			def PVP_AddPlayerInfo(self, chName, chLevel, chRace, chEmpire, chKill, chDead, maxKill):
				self.wndPvPRanking.AddPlayerInfo(chName, chLevel, chRace, chEmpire, chKill, chDead, maxKill)
		def PVP_RefreshWindow(self):
			self.wndPvPRanking.RefreshWindow()
		def PVP_EmptyWindow(self):
			self.wndPvPRanking.EmptyWindow()
		def PVP_CloseWindow(self):
			self.wndPvPRanking.Close(),
		def OpenPvpAdminWindow(self):
			if self.wndPvPRankingAdmin.IsShow():
				self.wndPvPRankingAdmin.Hide()	
			else:
				if chr.IsGameMaster(player.GetMainCharacterIndex()):
					self.wndPvPRankingAdmin.OpenDialog()
				else:
					pass
					
	def CrystalExchange_Open(self):
		if self.wndCrystalExchangeWindow:
			self.wndCrystalExchangeWindow.Open()
	
	def CrystalExchange_SetCrystalCounts(self, crystalCounts):
		if self.wndCrystalExchangeWindow:
			self.wndCrystalExchangeWindow.SetCrystalCounts(crystalCounts)
	
	def CrystalExchange_SucceedExchange(self):
		if self.wndCrystalExchangeWindow:
			self.wndCrystalExchangeWindow.SucceedExchange()
	
	def CrystalExchange_Refresh(self):
		if self.wndCrystalExchangeWindow:
			self.wndCrystalExchangeWindow.RefreshSlot(True)
	
	def CrystalExchange_RateUpdate(self):
		if self.wndCrystalExchangeWindow and self.wndCrystalExchangeWindow.IsShow():
			self.wndCrystalExchangeWindow.RefreshSlot(True)
				
	def AttachItemFromSafebox(self, slotIndex, itemIndex):
		if self.wndInventory and self.wndInventory.IsShow():
			self.wndInventory.AttachItemFromSafebox(slotIndex, itemIndex)

		return True

	def AttachInvenItemToOtherWindowSlot(self, slotWindow, slotIndex):
		if self.wndSafebox and self.wndSafebox.IsShow():
			return self.wndSafebox.AttachItemFromInventory(slotWindow, slotIndex)

		return False

	def OnRecvGuildOpenedDungeonIdx(self, openedDungeonIdx, isShowInfo):
		self.wndGuild.OnRecv_OpenedDungeonIdx(openedDungeonIdx, isShowInfo)

	def OnRecvGuildRefreshLogs(self):
		self.wndGuild.RefreshLogs()

	if app.ENABLE_NOTIFICATION_SYSTEM:
		def SendNotification(self, category, value, additionalValue):
			if self.wndNotificationManager:
				self.wndNotificationManager.AddToQueue(category, value, additionalValue)


	if app.ENABLE_OFFLINE_SHOP:
		def OpenOfflineShopWindow(self):
			if self.wndShopOffline:
				if self.wndShopOffline.IsShow() == False or self.wndShopOffline.wBoard[3].IsShow():
					self.wndShopOffline.Open()
				else:
					self.wndShopOffline.Close()

	if app.__DUNGEON_INFO__:
		def DungeonInfoQuestIdx(self, questIdx):
			constInfo.dungeonInfoQuestIdx = int(questIdx)
		def DungeonInfoQuestCMD(self):
			net.SendQuestInputStringPacket(constInfo.dungeonInfoCMD)
			constInfo.dungeonInfoCMD = ""
		def IsTeleportItem(self, itemVnum):
			return uiDungeonTrack.IsWarpItems(itemVnum)
		def OpenDungeonInfo(self):
			if self.wndDungeonTrack != None:
				if self.wndDungeonTrack.IsShow():
					self.wndDungeonTrack.Close()
				else:
					self.wndDungeonTrack.Open()
		def OpenWarpWindow(self):
			if self.wndDungeonTrack != None:
				if self.wndDungeonTrack.IsShow():
					self.wndDungeonTrack.Close()
				else:
					self.wndDungeonTrack.OpenOnMapPage()

		def DungeonInfoCooldown(self, clean, data):
			dungeonData = uiDungeonTrack.dungeonData
			if int(clean):
				for mobIdx, dungeonInfo in dungeonData.items():
					dungeonInfo["server_cooldown"] = 0
					dungeonInfo["server_completed"] = 0
					dungeonInfo["server_fastest"] = 0
					dungeonInfo["server_damage"] = 0
					dungeonInfo["server_daily_count"] = 0
					dungeonInfo["whisper_info"] = False
			
			if data != "" and data != "-":
				dungeonList = data[:len(data)-1].split("#")
				for dungeonInfo in dungeonList:
					dungeonSplitData = dungeonInfo.split("|")
					if len(dungeonSplitData) >= 5:
						bossIdx = int(dungeonSplitData[0])
						if dungeonData.has_key(bossIdx):
							if dungeonData[bossIdx]["server_cooldown"] != app.GetGlobalTimeStamp() + int(dungeonSplitData[1]):
								dungeonData[bossIdx]["whisper_info"] = False

							dungeonData[bossIdx]["server_cooldown"] = 0 if int(dungeonSplitData[1]) <= 0 else app.GetGlobalTimeStamp() + int(dungeonSplitData[1])
							dungeonData[bossIdx]["server_completed"] = int(dungeonSplitData[2])
							dungeonData[bossIdx]["server_fastest"] = int(dungeonSplitData[3])
							dungeonData[bossIdx]["server_damage"] = int(dungeonSplitData[4])
							dungeonData[bossIdx]["server_daily_count"] = int(dungeonSplitData[5]) if len(dungeonSplitData) >= 6 else 0
						dungeonData[bossIdx]["reset_flag"] = int(dungeonSplitData[6]) if len(dungeonSplitData) >= 7 else 0
						if len(dungeonSplitData) >= 8 and int(dungeonSplitData[7]) > 0:
							dungeonData[bossIdx]["daily_limit"] = int(dungeonSplitData[7]) - 1
			if self.wndDungeonManager == None:
				self.wndDungeonManager = uiDungeonTrack.DungeonManager()
			if self.wndDungeonTrack == None:
				self.wndDungeonTrack = uiDungeonTrack.DungeonTrack()
			self.wndDungeonTrack.Refresh()
		def LoadServerData(self, mobIdx, rankIdx, cmd):
			if self.wndDungeonTrack != None:
				self.wndDungeonTrack.LoadServerData(int(mobIdx), int(rankIdx), cmd)

	if app.ENABLE_ITEMSHOP:
		def MakeItemShopWindow(self):
			if not self.wndItemShop:
				self.wndItemShop = uiItemShopNew.ItemShopWindow()

		def OpenItemShopWindow(self):
			if not self.wndItemShop:
				self.wndItemShop = uiItemShopNew.ItemShopWindow()

			if self.wndItemShop.IsShow():
				self.wndItemShop.Close()
			else:
				self.wndItemShop.Open()

		def OpenItemShopMainWindow(self):
			self.MakeItemShopWindow()
			self.wndItemShop.Open()
			self.wndItemShop.LoadFirstOpening()
		def OpenItemShopToWheel(self):
			self.MakeItemShopWindow()
			self.wndItemShop.Open()
			self.wndItemShop.HotItems()
		def ItemShopHideLoading(self):
			self.MakeItemShopWindow()
			self.wndItemShop.Open()
			self.wndItemShop.CloseLoading()
		def ItemShopPurchasesWindow(self):
			self.MakeItemShopWindow()
			self.wndItemShop.Open()
			self.wndItemShop.OpenPurchasesWindow()
		def ItemShopUpdateItem(self, itemID, itemMaxSellingCount):
			self.MakeItemShopWindow()
			self.wndItemShop.UpdateItem(itemID, itemMaxSellingCount)

		#USE_ITEMSHOP_RENEWED: @lldJCoins
		def ItemShopSetDragonCoin(self, lldCoins, lldJCoins = 0):
			if not self.wndItemShop:
				self.wndItemShop = uiItemShopNew.ItemShopWindow()

			self.wndItemShop.SetDragonCoin(lldCoins)

			if app.USE_ITEMSHOP_RENEWED:
				self.wndItemShop.SetDragonJeton(lldJCoins)

		def SetWheelItemData(self, cmd):
			self.MakeItemShopWindow()
			self.wndItemShop.SetWheelItemData(str(cmd))
		def OnSetWhell(self, giftIndex):
			self.MakeItemShopWindow()
			self.wndItemShop.OnSetWhell(int(giftIndex))
		def GetWheelGiftData(self, itemVnum, itemCount):
			self.MakeItemShopWindow()
			self.wndItemShop.GetWheelGiftData(int(itemVnum), int(itemCount))
			
	if app.__AUTO_SKILL_READER__:
		def OpenAutoSkillReader(self):
			if not self.wndAutoSkillReader:
				self.wndAutoSkillReader = uiAutoSkillReader.Window()

			if self.wndAutoSkillReader.IsShow():
				self.wndAutoSkillReader.Hide()
			else:
				self.wndAutoSkillReader.Show()

		def AutoSkillStatus(self, status):
			if self.wndAutoSkillReader:
				self.wndAutoSkillReader.ServerSetStatus(status)

	if app.ENABLE_VOTE4BUFF:
		def ToggleVote4Buff(self):
			if not self.wndVote4Buff.IsShow():
				self.wndVote4Buff.Open()
			else:
				self.wndVote4Buff.Close()

		def OpenVote4Buff(self):
			self.wndVote4Buff.Open()

		def RefreshVote4BuffData(self, data):
			tokens = data.split(':')
			
			constInfo.VOTE4BUFF_DATA["LOADED"] = True
			constInfo.VOTE4BUFF_DATA["LAST_VOTE_TIME"] = int(tokens[0])
			if uivote4buff.IsVoteValid():
				constInfo.VOTE4BUFF_DATA["SELECTED_BONUS"]  = int(tokens[1])
				constInfo.VOTE4BUFF_DATA["CHANGE_BONUS_CD"] = int(tokens[2])
			else:
				constInfo.VOTE4BUFF_DATA["SELECTED_BONUS"]  = 0
				constInfo.VOTE4BUFF_DATA["CHANGE_BONUS_CD"] = 0

			if self.wndVote4Buff.IsShow():
				self.wndVote4Buff.RefreshInfo()

			self.RefreshVote4BuffButton()
		
		def RefreshVote4BuffButton(self):
			if uivote4buff.IsBonusSelected() and uivote4buff.HIDE_BIG_BUTTON_ON_SUCCESSFUL_VOTE:
				self.btnVote4Buff.Hide()
			else:
				self.btnVote4Buff.Show()

		def Vote4Buff_OnUpdate(self):
			if self.wndVote4Buff:
				self.wndVote4Buff.Update()
				
_instance = None

def GetInstance():
	global _instance
	return _instance

def SetInstance(instance):
	global _instance
	
	if _instance:
		del _instance
	
	_instance = instance

