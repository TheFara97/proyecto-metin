import ui, localeInfo, net, dbg
import player
import colorInfo
import constInfo
from uiToolTip import ItemToolTip
import app
import chat
import uiScriptLocale

TITLES_COUNT = 24
TITLE_PATH = "elvion/game/title_achievement/{}"

_TITLE_ACHIEVEMENT = {}

for x in xrange(TITLES_COUNT):
	title_id = x + 1
	_TITLE_ACHIEVEMENT[title_id] = {}
	
	locale_index = title_id
	
	_TITLE_ACHIEVEMENT[title_id]["name"] = eval("localeInfo.TITLE_ACHIEVEMENT_%d" % locale_index)
	_TITLE_ACHIEVEMENT[title_id]["task"] = eval("localeInfo.TITLE_ACHIEVEMENT_INFO_OBJECT_%d" % locale_index)
	_TITLE_ACHIEVEMENT[title_id]["bonus"] = eval("localeInfo.TITLE_ACHIEVEMENT_INFO_BONUS_%d" % locale_index)
	_TITLE_ACHIEVEMENT[title_id]["color"] = eval("colorInfo.TITLE_ACHIEVEMENT_COLOR_%d" % locale_index)

class Edit2(ui.EditLine):
	def __init__(self, parent, text, x, y, width, height):
		ui.EditLine.__init__(self)
		self.imageSlot = ui.ImageBox()
		self.imageSlot.SetParent(parent)
		self.imageSlot.SetPosition(x, y)
		self.imageSlot.LoadImage("elvion/game/title_achievement/title_search.png")
		self.imageSlot.Show()
		self.SetText(text)
		self.main = text
		self.SetEscapeEvent(self.OnPressEscapeKey)
		self.SetMax(15)
		self.SetUserMax(15)
		self.SetParent(self.imageSlot)
		
		self.SetSize(width, height)
		self.SetPosition(8, 8)
		self.Show()
	
	def OnPressEscapeKey(self):
		pass
	
	def __del__(self):
		ui.EditLine.__del__(self)

class TitleAchievementWindow(ui.ScriptWindow):
	FILTER_ALL = 0
	FILTER_OWNED = 1
	FILTER_FAVOURITE = 2

	def __init__(self):
		super(TitleAchievementWindow, self).__init__()
		self.Initialize()
		self.LoadWindow()

	def __del__(self):
		try:
			ui.ScriptWindow.__del__(self)
		except:
			pass

	def Destroy(self):
		self.ClearDictionary()
		self.ClearList()
		self.Initialize()

	def Initialize(self):
		self.selectedIdx = -1
		self.window_dict = {}
		self.tooltip = None
		self.titleItems = []
		self.unlockedTitles = [0] * 25
		self.favouriteTitles = [0] * 25
		self.currentActiveTitle = 0
		self.currentActiveTitlePremium = 0
		self.currentAffects = []
		self.currentAffectsPremium = []
		self.currentFilter = self.FILTER_ALL
		self.searchText = ""
		self.scrollBar = None
		self.contentContainer = None
		self.clippingWindow = None
		self.searchTextEdit = None
		self.searchTextCover = None
		self.favouritesFilePath = ""

	def SetToolTip(self, tooltip):
		from _weakref import proxy
		self.tooltip = proxy(tooltip)

	def Open(self):
		net.SendChatPacket("/load_title_achievement")
		self.LoadFavourites()
		self.SetTop()
		self.SetCenterPosition()
		self.Show()

	def Close(self):
		self.SaveFavourites()
		self.Hide()
		if self.tooltip:
			self.tooltip.HideToolTip()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def ClearList(self):
		if self.contentContainer:
			for item in self.titleItems:
				item.Hide()
		self.titleItems = []

	def BINARY_ClearTitleAchievementList(self):
		self.unlockedTitles = [0] * 25
		self.currentActiveTitle = 0
		self.currentActiveTitlePremium = 0
		self.currentAffects = []
		self.currentAffectsPremium = []
		self.ClearList()

	def BINARY_SetTitleUnlockStatus(self, titleIdx, unlocked):
		if titleIdx < len(self.unlockedTitles):
			self.unlockedTitles[titleIdx] = unlocked

	def BINARY_SetCurrentActiveTitle(self, titleIdx):
		self.currentActiveTitle = titleIdx if titleIdx != 255 else 0
		self.currentAffects = []
		self.UpdateActiveGlow()
	
	def BINARY_SetCurrentActiveTitlePremium(self, titleIdx):
		self.currentActiveTitlePremium = titleIdx if titleIdx != 255 else 0
		self.currentAffectsPremium = []
		self.UpdateActiveGlow()
	
	def UpdateActiveGlow(self):
		for item in self.titleItems:
			if hasattr(item, 'titleIdx'):
				if hasattr(item, 'glowImage'):
					if item.titleIdx == self.currentActiveTitle:
						item.glowImage.Show()
					else:
						item.glowImage.Hide()
				
				if hasattr(item, 'glowImagePremium'):
					if item.titleIdx == self.currentActiveTitlePremium:
						item.glowImagePremium.Show()
					else:
						item.glowImagePremium.Hide()

	def BINARY_SetTitleEffect(self, effectIdx, affType, affValue):
		while len(self.currentAffects) <= effectIdx:
			self.currentAffects.append((0, 0))
		self.currentAffects[effectIdx] = (affType, affValue)
	
	def BINARY_SetTitleEffectPremium(self, effectIdx, affType, affValue):
		while len(self.currentAffectsPremium) <= effectIdx:
			self.currentAffectsPremium.append((0, 0))
		self.currentAffectsPremium[effectIdx] = (affType, affValue)

	def BINARY_FinalizeTitleLoading(self):
		self.LoadTitleList()

	def __OnValueUpdate(self):
		ui.EditLine.OnIMEUpdate(self.searchTextEdit)
		val = uiScriptLocale.RemoveDiacritic(self.searchTextEdit.GetText())
		
		if len(val) <= 0:
			self.searchTextCover.Show()
			self.searchText = ""
			self.LoadTitleList()
		else:
			self.searchTextCover.Hide()
			self.searchText = val
			self.LoadTitleList()

	def LoadTitleList(self):
		self.ClearList()
		
		if not self.contentContainer:
			return
		
		start_x = 10
		start_y = 5
		spacing_y = 41
		title_height = 60
		
		visible_count = 0
		current_y = start_y
		
		for idx in range(1, TITLES_COUNT + 1):
			if not self.ShouldShowTitle(idx):
				continue
			
			title_data = _TITLE_ACHIEVEMENT.get(idx, {})
			is_unlocked = idx < len(self.unlockedTitles) and self.unlockedTitles[idx] == 1
			is_favourite = idx < len(self.favouriteTitles) and self.favouriteTitles[idx] == 1
			
			titleItem = self.CreateTitleItem(idx, title_data, is_unlocked, is_favourite)
			titleItem.SetParent(self.contentContainer)
			titleItem.SetPosition(start_x, current_y)
			titleItem.Show()
			
			self.titleItems.append(titleItem)
			
			visible_count += 1
			current_y += spacing_y
		
		if self.scrollBar:
			total_height = visible_count * spacing_y if visible_count > 0 else 0
			self.scrollBar.SetContentHeight(total_height)
			self.scrollBar.SetPos(0)

	def ShouldShowTitle(self, idx):
		title_data = _TITLE_ACHIEVEMENT.get(idx, {})
		
		if self.searchText:
			title_name = title_data.get("name", "").lower()
			search_lower = self.searchText.lower()
			
			title_name_clean = uiScriptLocale.RemoveDiacritic(title_name)
			
			if search_lower not in title_name_clean:
				return False
		
		if self.currentFilter == self.FILTER_OWNED:
			if not (idx < len(self.unlockedTitles) and self.unlockedTitles[idx] == 1):
				return False
		elif self.currentFilter == self.FILTER_FAVOURITE:
			if not (idx < len(self.favouriteTitles) and self.favouriteTitles[idx] == 1):
				return False
		
		return True

	def CreateTitleItem(self, idx, title_data, is_unlocked, is_favourite):
		container = ui.ImageBox()
		container.SetParent(self.contentContainer)
		container.LoadImage(TITLE_PATH.format("title_bg.png"))
		container.titleIdx = idx
		container.SetClippingMaskWindow(self.clippingWindow)
		container.SetEvent(ui.__mem_func__(self.OnClickTitle), "mouse_click", idx)
		container.OnMouseRightButtonUp = lambda arg=idx: self.OnRightClickTitle(arg)
		
		glowImage = ui.ImageBox()
		glowImage.SetParent(container)
		glowImage.LoadImage(TITLE_PATH.format("title_glow.png"))
		glowImage.SetPosition(0, 0)
		glowImage.SetClippingMaskWindow(self.clippingWindow)
		if idx == self.currentActiveTitle:
			glowImage.Show()
		else:
			glowImage.Hide()
		container.glowImage = glowImage
		
		glowImagePremium = ui.ImageBox()
		glowImagePremium.SetParent(container)
		glowImagePremium.LoadImage(TITLE_PATH.format("title_glow_premium.png"))
		glowImagePremium.SetPosition(0, 0)
		glowImagePremium.SetClippingMaskWindow(self.clippingWindow)
		if idx == self.currentActiveTitlePremium:
			glowImagePremium.Show()
		else:
			glowImagePremium.Hide()
		container.glowImagePremium = glowImagePremium

		if not is_unlocked:
			lockImage = ui.ImageBox()
			lockImage.SetParent(container)
			lockImage.LoadImage(TITLE_PATH.format("locked.png"))
			lockImage.SetPosition(388, 6)
			lockImage.SetClippingMaskWindow(self.clippingWindow)
			lockImage.Show()
			container.lockImage = lockImage
		
		if is_favourite:
			starImage = ui.ImageBox()
			starImage.SetParent(container)
			starImage.LoadImage(TITLE_PATH.format("favorite_btn_norm.png"))
			starImage.SetPosition(420, 6)
			starImage.SetClippingMaskWindow(self.clippingWindow)
			starImage.Show()
			container.starImage = starImage
		
		titleText = ui.TextLine()
		titleText.SetParent(container)
		titleText.SetPosition(42, 4)
		titleText.SetOutline()
		titleText.SetFontName("Tahoma:12")
		titleText.SetClippingMaskWindow(self.clippingWindow)
		
		color = title_data.get("color", [255, 255, 255])
		name = title_data.get("name", "Unknown")
		
		if is_unlocked:
			titleText.SetText(name)
			titleText.SetPackedFontColor(ui.GenerateColor(color[0], color[1], color[2]))
		else:
			titleText.SetText(name)
			titleText.SetPackedFontColor(ui.GenerateColor(136, 136, 136))
		
		titleText.Show()
		container.titleText = titleText
		
		bonusText = ui.TextLine()
		bonusText.SetParent(container)
		bonusText.SetPosition(42, 19)
		bonusText.SetOutline()
		bonusText.SetFontName("Tahoma:12")
		bonusText.SetClippingMaskWindow(self.clippingWindow)
		bonusText.SetText(title_data.get("bonus", ""))
		bonusText.SetPackedFontColor(0xFFb3b2b1)
		bonusText.Show()
		container.bonusText = bonusText
		
		taskText = ui.TextLine()
		taskText.SetParent(container)
		taskText.SetPosition(238, 12)
		taskText.SetOutline()
		taskText.SetFontName("Tahoma:12")
		taskText.SetClippingMaskWindow(self.clippingWindow)
		
		if not is_unlocked:
			current = self.GetTitleProgress(idx)
			required = self.GetTitleRequirement(idx)
			current_formatted = self.GetFormattedCount(current)
			required_formatted = self.GetFormattedCount(required)
			taskText.SetText(localeInfo.TITLE_ACHIEVEMENT_PROGRESS % (current_formatted, required_formatted))
			taskText.SetPackedFontColor(0xFFb3b2b1)
		else:
			taskText.SetText(localeInfo.TITLE_ACHIEVEMENT_UNLOCKED)
			taskText.SetPackedFontColor(0xFF64ff64)
		
		taskText.Show()
		container.taskText = taskText
		
		container.titleIdx = idx
		container.titleData = title_data
		container.isUnlocked = is_unlocked
		
		return container
		
	def OnUpdate(self):
		import wndMgr
		
		for item in self.titleItems:
			if not hasattr(item, 'titleIdx') or not hasattr(item, 'taskText'):
				continue
			
			idx = item.titleIdx
			is_unlocked = idx < len(self.unlockedTitles) and self.unlockedTitles[idx] == 1
			
			if not is_unlocked:
				current = self.GetTitleProgress(idx)
				required = self.GetTitleRequirement(idx)
				current_formatted = self.GetFormattedCount(current)
				required_formatted = self.GetFormattedCount(required)
				item.taskText.SetText(localeInfo.TITLE_ACHIEVEMENT_PROGRESS % (current_formatted, required_formatted))
				item.taskText.SetPackedFontColor(0xFFb3b2b1)
			else:
				item.taskText.SetText(localeInfo.TITLE_ACHIEVEMENT_UNLOCKED)
				item.taskText.SetPackedFontColor(0xFF64ff64)
		
		if not self.tooltip:
			return
		
		mouseX, mouseY = wndMgr.GetMousePosition()
		clipX, clipY = self.clippingWindow.GetGlobalPosition()
		clipW, clipH = self.clippingWindow.GetWidth(), self.clippingWindow.GetHeight()
		
		tooltipShown = False
		
		for item in self.titleItems:
			if not hasattr(item, 'titleIdx'):
				continue
			
			itemX, itemY = item.GetGlobalPosition()
			itemW, itemH = item.GetWidth(), item.GetHeight()
			
			if mouseX >= itemX and mouseX <= itemX + itemW:
				if mouseY >= itemY and mouseY <= itemY + itemH:
					if mouseY >= clipY and mouseY < clipY + clipH:
						self.OnMouseOverIn(item.titleIdx)
						tooltipShown = True
						break
		
		if not tooltipShown:
			self.tooltip.Hide()

	def OnClickTitle(self, event_type, idx):
		if idx < len(self.unlockedTitles) and self.unlockedTitles[idx] == 1:
			if idx == self.currentActiveTitlePremium:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.TITLE_ACHIEVEMENT_TITLE_ACTIVE_IN_PREMIUM)
				return
			
			if self.currentActiveTitlePremium > 0 and not self.CanActivateTogether(idx, self.currentActiveTitlePremium):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.TITLE_ACHIEVEMENT_TITLE_CANNOT_ACTIVATE_TOGETHER)
				return
			
			net.SendChatPacket("/change_title_achievement {}".format(idx))
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.TITLE_ACHIEVEMENT_NOT_UNLOCKED_YET)

	def OnRightClickTitle(self, idx):
		if app.IsPressed(app.DIK_LCONTROL):
			if idx < len(self.favouriteTitles):
				self.favouriteTitles[idx] = 0 if self.favouriteTitles[idx] == 1 else 1
				self.SaveFavourites()
				self.LoadTitleList()
				if self.favouriteTitles[idx] == 1:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.TITLE_ACHIEVEMENT_ADDED_TO_FAVOURITES)
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.TITLE_ACHIEVEMENT_REMOVED_FROM_FAVOURITES)
		else:
			if idx < len(self.unlockedTitles) and self.unlockedTitles[idx] == 1:
				if idx == self.currentActiveTitle:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.TITLE_ACHIEVEMENT_TITLE_ACTIVE_IN_PRIMARY)
					return
				
				if self.currentActiveTitle > 0 and not self.CanActivateTogether(self.currentActiveTitle, idx):
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.TITLE_ACHIEVEMENT_TITLE_CANNOT_ACTIVATE_TOGETHER_PRIMARY)
					return
				
				net.SendChatPacket("/change_title_achievement_premium {}".format(idx))
			else:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.TITLE_ACHIEVEMENT_NOT_UNLOCKED_YET)
	
	def CanActivateTogether(self, title1, title2):
		if title1 == 0 or title2 == 0 or title1 == title2:
			return False
		
		effects1 = self.GetTitleEffectTypes(title1)
		effects2 = self.GetTitleEffectTypes(title2)
		
		for eff1 in effects1:
			if eff1 in effects2:
				return False
		
		return True
	
	def GetTitleEffectTypes(self, titleIdx):
		if titleIdx not in _TITLE_ACHIEVEMENT:
			return []
		
		effect_types = []
		
		if titleIdx in [1, 2, 3]:
			effect_types.append("ATTBONUS_STONE")
		
		elif titleIdx in [4, 5, 6]:
			effect_types.append("ATTBONUS_BOSS")
		
		elif titleIdx == 10:
			effect_types.append("RELIC_BONUS")
		
		elif titleIdx == 12:
			effect_types.append("ALCHEMY_REFINE")
		
		elif titleIdx in [13, 14, 15]:
			effect_types.append("MINING_SUCCESS")
		
		elif titleIdx in [16, 17, 18]:
			effect_types.append("FISHING_SUCCESS")
		
		elif titleIdx in [11, 19, 21]:
			effect_types.append("REFINE_FAIL_BONUS")
		
		elif titleIdx == 20:
			effect_types.append("DEATH_BONUS")
		
		elif titleIdx == 22:
			effect_types.append("RECORD_BONUS")
		
		elif titleIdx == 23:
			effect_types.append("PLAYTIME_BONUS")
		
		elif titleIdx == 24:
			effect_types.append("NO_DAMAGE_BONUS")
		
		return effect_types

	def OnMouseOverIn(self, idx):
		if not self.tooltip or idx not in _TITLE_ACHIEVEMENT:
			return
		
		title_data = _TITLE_ACHIEVEMENT[idx]
		color = title_data["color"]
		is_unlocked = idx < len(self.unlockedTitles) and self.unlockedTitles[idx] == 1
		
		self.tooltip.ClearToolTip()
		self.tooltip.AutoAppendTextLine(title_data["name"], ui.GenerateColor(color[0], color[1], color[2]))
		self.tooltip.AutoAppendTextLine(title_data["task"])
		self.tooltip.AutoAppendTextLine("Bonus: |cffebddbe" + title_data["bonus"] + "|r")
		
		if is_unlocked:
			self.tooltip.AutoAppendTextLine(localeInfo.TITLE_ACHIEVEMENT_UNLOCKED_STATUS, 0xff00ff00)
		else:
			current = self.GetTitleProgress(idx)
			required = self.GetTitleRequirement(idx)
			current_formatted = self.GetFormattedCount(current)
			required_formatted = self.GetFormattedCount(required)
			self.tooltip.AutoAppendTextLine(localeInfo.TITLE_ACHIEVEMENT_PROGRESS % (current_formatted, required_formatted), 0xffffff00)
		
		if idx == self.currentActiveTitle:
			self.tooltip.AutoAppendTextLine("", 0xffffffff)
			self.tooltip.AutoAppendTextLine(localeInfo.TITLE_ACHIEVEMENT_ACTIVATED_PRIMARY, 0xff00ff00)
		if idx == self.currentActiveTitlePremium:
			self.tooltip.AutoAppendTextLine("", 0xffffffff)
			self.tooltip.AutoAppendTextLine(localeInfo.TITLE_ACHIEVEMENT_ACTIVATED_PREMIUM, 0xffffaa00)
		
		self.tooltip.AutoAppendTextLine("", 0xffffffff)
		self.tooltip.AutoAppendTextLine(localeInfo.TITLE_ACHIEVEMENT_TOOLTIP_ACTIVATE, 0xffebd6be)
		self.tooltip.AutoAppendTextLine(localeInfo.TITLE_ACHIEVEMENT_TOOLTIP_ACTIVATE_PREMIUM, 0xffebd6be)
		self.tooltip.AutoAppendTextLine(localeInfo.TITLE_ACHIEVEMENT_TOOLTIP_ADD_FAVOURITE, 0xffebd6be)
		
		self.tooltip.AlignHorizonalCenter()
		self.tooltip.Show()
	
	def OnMouseOverOut(self, idx):
		if self.tooltip:
			self.tooltip.Hide()

	def GetFormattedCount(self, count):
		if count <= 0:
			return "0"
		
		count_str = str(count)
		formatted = '.'.join([
			i-3<0 and count_str[:i] or count_str[i-3:i] 
			for i in range(len(count_str)%3, len(count_str)+1, 3) 
			if i
		])
		return formatted

	def GetTitleProgress(self, titleIdx):
		if titleIdx == 1:
			return int(constInfo.TITLE_STONE_COUNT)
		elif titleIdx == 2:
			return int(constInfo.TITLE_STONE_COUNT)
		elif titleIdx == 3:
			return int(constInfo.TITLE_STONE_COUNT)
		elif titleIdx == 4:
			return int(constInfo.TITLE_BOSS_COUNT)
		elif titleIdx == 5:
			return int(constInfo.TITLE_BOSS_COUNT)
		elif titleIdx == 6:
			return int(constInfo.TITLE_BOSS_COUNT)
		elif titleIdx == 7:
			return int(constInfo.TITLE_DUNGEON_FASTEST_TIME)
		elif titleIdx == 8:
			return int(constInfo.TITLE_ENCHANT_COUNT)
		elif titleIdx == 9:
			return int(constInfo.TITLE_ENCHANT_COUNT)
		elif titleIdx == 10:
			return int(constInfo.TITLE_RELICT_COUNT)
		elif titleIdx == 11:
			return int(constInfo.TITLE_REFINE_SUCCESS_COUNT)
		elif titleIdx == 12:
			return int(constInfo.TITLE_REFINED_ALCHEMY_COUNT)
		elif titleIdx == 13:
			return int(constInfo.MY_ORE_COUNT)
		elif titleIdx == 14:
			return int(constInfo.MY_ORE_COUNT)
		elif titleIdx == 15:
			return int(constInfo.MY_ORE_COUNT)
		elif titleIdx == 16:
			return int(constInfo.MY_FISH_COUNT)
		elif titleIdx == 17:
			return int(constInfo.MY_FISH_COUNT)
		elif titleIdx == 18:
			return int(constInfo.MY_FISH_COUNT)
		elif titleIdx == 21:
			return int(constInfo.TITLE_REFINE_FAIL_COUNT)
		elif titleIdx == 22:
			return int(constInfo.TITLE_DUNGEON_DUNGEON_BOSS_KILLED)
		elif titleIdx == 23:
			return int(constInfo.TITLE_PLAYTIME_COUNT)
		return 0

	def GetTitleRequirement(self, titleIdx):
		requirements = {
			1: 1000, 2: 20000, 3: 100000,
			4: 1000, 5: 2000, 6: 20000,
			7: 1, 8: 50000, 9: 500000,
			10: 1000, 11: 1000, 12: 1000,
			13: 20000, 14: 200000, 15: 1000000,
			16: 1000, 17: 10000, 18: 100000,
			19: 999, 20: 999, 21: 30,
			22: 1000, 23: 2000, 24: 1,
		}
		return requirements.get(titleIdx, 100)

	def SetFilter(self, filterType):
		self.currentFilter = filterType
		self.LoadTitleList()

	def LoadWindow(self):
		try:
			ui.PythonScriptLoader().LoadScriptFile(self, "uiscript/titleachievement.py")
		except Exception, msg:
			dbg.TraceError("TitleAchievementWindow LoadWindow - %s" % str(msg))
			return

		try:
			self.__BindObjects()
		except Exception, msg:
			dbg.TraceError("TitleAchievementWindow BindObjects - %s" % str(msg))
			return

		try:
			self.__BindEvents()
		except Exception, msg:
			dbg.TraceError("TitleAchievementWindow BindEvents - %s" % str(msg))
			return

	def __BindObjects(self):
		child = self.GetChild
		self.window_dict = {}
		
		try:
			self.window_dict['TitleBar'] = child("TitleBar")
			self.scrollBar = child("titlesScrollbar")
			
			self.clippingWindow = ui.Window()
			self.clippingWindow.SetParent(self)
			self.clippingWindow.SetPosition(10, 96)
			self.clippingWindow.SetSize(464, 254)
			self.clippingWindow.Show()
			
			self.contentContainer = ui.Window()
			self.contentContainer.SetParent(self.clippingWindow)
			self.contentContainer.SetPosition(0, 0)
			self.contentContainer.SetSize(454, 256)
			self.contentContainer.Show()
			
			self.clippingWindow.OnMouseWheel = self.scrollBar.OnMouseWheel
			
			self.window_dict['allTitlesBtn'] = child("allTitlesBtn")
			self.window_dict['ownedTitlesButton'] = child("ownedTitlesButton")
			self.window_dict['favouriteButton'] = child("favouriteButton")
			self.clippingWindow.SetWindowName("TitleClippingWindow")
			
			self.clippingWindow.OnMouseLeftButtonUp = ui.__mem_func__(self.OnClickTitleInClipWindow)
			self.clippingWindow.OnMouseRightButtonUp = ui.__mem_func__(self.OnRightClickTitleInClipWindow)
			
			searchContainer = child("SearchFieldContainer")
			self.searchTextEdit = Edit2(searchContainer, "", 0, 0, 120, 25)
			self.searchTextEdit.OnIMEUpdate = ui.__mem_func__(self.__OnValueUpdate)
			
			self.searchTextCover = ui.MakeTextLine(self.searchTextEdit)
			self.searchTextCover.SetPosition(-24, -7)
			self.searchTextCover.SetText(localeInfo.TITLE_ACHIEVEMENT_SEARCH_TITLES)
			
		except Exception, e:
			dbg.TraceError("Failed to bind objects: %s" % str(e))

	def __BindEvents(self):
		if 'TitleBar' in self.window_dict and self.window_dict['TitleBar']:
			self.window_dict['TitleBar'].SetCloseEvent(self.Close)
	
		if 'allTitlesBtn' in self.window_dict and self.window_dict['allTitlesBtn']:
			self.window_dict['allTitlesBtn'].SetEvent(ui.__mem_func__(self.SetFilter), self.FILTER_ALL)
		
		if 'ownedTitlesButton' in self.window_dict and self.window_dict['ownedTitlesButton']:
			self.window_dict['ownedTitlesButton'].SetEvent(ui.__mem_func__(self.SetFilter), self.FILTER_OWNED)
		
		if 'favouriteButton' in self.window_dict and self.window_dict['favouriteButton']:
			self.window_dict['favouriteButton'].SetEvent(ui.__mem_func__(self.SetFilter), self.FILTER_FAVOURITE)
		
		if self.scrollBar:
			self.scrollBar.SetScrollEvent(ui.__mem_func__(self.OnScroll))

	def OnClickTitleInClipWindow(self):
		import wndMgr
		mouseX, mouseY = wndMgr.GetMousePosition()
		
		clipX, clipY = self.clippingWindow.GetGlobalPosition()
		clipW, clipH = self.clippingWindow.GetWidth(), self.clippingWindow.GetHeight()
		
		for item in self.titleItems:
			if not hasattr(item, 'titleIdx'):
				continue
			
			itemX, itemY = item.GetGlobalPosition()
			itemW, itemH = item.GetWidth(), item.GetHeight()
			
			if mouseX >= itemX and mouseX <= itemX + itemW:
				if mouseY >= itemY and mouseY <= itemY + itemH:
					if mouseY >= clipY and mouseY < clipY + clipH:
						self.OnClickTitle("mouse_click", item.titleIdx)
						return True
		
		return False
	
	def OnRightClickTitleInClipWindow(self):
		import wndMgr
		mouseX, mouseY = wndMgr.GetMousePosition()
		
		clipX, clipY = self.clippingWindow.GetGlobalPosition()
		clipW, clipH = self.clippingWindow.GetWidth(), self.clippingWindow.GetHeight()
		
		for item in self.titleItems:
			if not hasattr(item, 'titleIdx'):
				continue
			
			itemX, itemY = item.GetGlobalPosition()
			itemW, itemH = item.GetWidth(), item.GetHeight()
			
			visibleTop = max(itemY, clipY)
			visibleBottom = min(itemY + itemH, clipY + clipH)
			
			if mouseX >= itemX and mouseX <= itemX + itemW:
				if mouseY >= visibleTop and mouseY < visibleBottom:
					self.OnRightClickTitle(item.titleIdx)
					return True
		
		return False
	
	def OnScroll(self):
		if not self.scrollBar or not self.contentContainer:
			return
		
		pos = self.scrollBar.GetPos()
		contentHeight = self.scrollBar.contentHeight if hasattr(self.scrollBar, 'contentHeight') else 0
		visibleHeight = 256
		
		maxScroll = max(0, contentHeight - visibleHeight)
		
		scrollOffset = int(maxScroll * pos)
		self.contentContainer.SetPosition(0, 0 - scrollOffset)

	def OnRunMouseWheel(self, a):
		if not self.scrollBar or not self.scrollBar.IsShow():
			return False
		if not self.IsInPosition():
			return False
		
		if a > 0:
			self.scrollBar.OnUp()
		else:
			self.scrollBar.OnDown()
		return True

	def RefreshTitles(self):
		self.ClearList()
		self.LoadTitleList()
		net.SendChatPacket("/load_title_achievement")
	
	def GetFavouritesFilePath(self):
		import os
		
		playerName = player.GetName()
		if not playerName:
			return ""
		
		directory = "content/userdata/favourite_titles/"
		
		if not os.path.exists(directory):
			try:
				os.makedirs(directory)
			except:
				dbg.TraceError("Failed to create directory: %s" % directory)
				return ""
		
		return directory + playerName + ".txt"
	
	def SaveFavourites(self):
		filePath = self.GetFavouritesFilePath()
		if not filePath:
			return
		
		try:
			favourites = []
			for idx in range(len(self.favouriteTitles)):
				if self.favouriteTitles[idx] == 1:
					favourites.append(str(idx))
			
			with open(filePath, "w") as f:
				f.write(",".join(favourites))
			
		except Exception, e:
			dbg.TraceError("Failed to save favourites: %s" % str(e))
	
	def LoadFavourites(self):
		filePath = self.GetFavouritesFilePath()
		if not filePath:
			return
		
		try:
			import os
			if not os.path.exists(filePath):
				return
			
			f = open(filePath, "r")
			content = f.readlines()
			f.close()	
			
			for content in reversed(content):
				content = content.strip()
				if not content:
					continue
				
				self.favouriteTitles = [0] * 25
				
				favourites = content.split(",")
				for idxStr in favourites:
					try:
						idx = int(idxStr)
						if 0 <= idx < len(self.favouriteTitles):
							self.favouriteTitles[idx] = 1
					except:
						pass
		
		except Exception, e:
			dbg.TraceError("Failed to load favourites: %s" % str(e))