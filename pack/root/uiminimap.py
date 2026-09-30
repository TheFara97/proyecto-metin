#-*- coding: iso-8859-1 -*-
import ui
import uiScriptLocale
import wndMgr
import player
import miniMap
import localeInfo
import net
import app
import constInfo
import chr
import uiToolTip
import interfaceModule
if app.BL_KILL_BAR:
	import playersettingmodule
	import item

import time

#ifdef ENABLE_ULTIMATE_REGEN
def FormatTime(seconds):
	if seconds <= 0:
		return "0s"
	m, s = divmod(seconds, 60)
	h, m = divmod(m, 60)
	d, h = divmod(h, 24)
	text = ""
	if d > 0:
		text+= str(d)+"d "
	if h > 0:
		text+= str(h)+"h "
	if m > 0:
		text+= str(m)+"m "
	if s > 0:
		text+= str(s)+"s "
	return text
	
class MapTextToolTip(ui.Window):
	def __init__(self):
		ui.Window.__init__(self)

		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetHorizontalAlignCenter()
		textLine.SetOutline()
		textLine.SetHorizontalAlignRight()
		textLine.Show()
		self.textLine = textLine

	def __del__(self):
		ui.Window.__del__(self)

	def SetText(self, text):
		self.textLine.SetText(text)

	def SetTooltipPosition(self, PosX, PosY):
		if localeInfo.IsARABIC():
			w, h = self.textLine.GetTextSize()
			self.textLine.SetPosition(PosX - w - 5, PosY)
		else:
			self.textLine.SetPosition(PosX - 5, PosY)

	def SetTextColor(self, TextColor):
		self.textLine.SetPackedFontColor(TextColor)

	def SetHorizontalAlignLeft(self):
		if self.textLine:
			self.textLine.SetHorizontalAlignLeft()

	def GetTextSize(self):
		return self.textLine.GetTextSize()

class AtlasWindow(ui.ScriptWindow):

	class AtlasRenderer(ui.Window):
		def __init__(self):
			ui.Window.__init__(self)
			self.AddFlag("not_pick")

		def OnUpdate(self):
			miniMap.UpdateAtlas()

		def OnRender(self):
			(x, y) = self.GetGlobalPosition()
			fx = float(x)
			fy = float(y)
			miniMap.RenderAtlas(fx, fy)

		def HideAtlas(self):
			miniMap.HideAtlas()

		def ShowAtlas(self):
			miniMap.ShowAtlas()

	def __init__(self):
		self.tooltipInfo = MapTextToolTip()
		self.tooltipInfo.Hide()
		self.infoGuildMark = ui.MarkBox()
		self.infoGuildMark.Hide()
		self.AtlasMainWindow = None
		self.mapName = ""
		self.board = 0
		self.info = 0

		self.tooltipInfoEx = MapTextToolTip()
		self.tooltipInfoEx.Hide()
		self.bossRespawnDict = {}

		ui.ScriptWindow.__init__(self)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def UpdateBossRespawnTime(self, mobVnum, respawnTime):
		self.bossRespawnDict[mobVnum] = respawnTime
		
	@staticmethod
	def GetTimeRemaining(respawnTime):
		import app
		currentTime = app.GetGlobalTimeStamp()
		delta = respawnTime - currentTime
		
		if delta <= 0:
			return 0
		
		return delta
		
	def SetMapName(self, mapName):
		if 949==app.GetDefaultCodePage():
			try:
				self.board.SetTitleName(localeInfo.MINIMAP_ZONE_NAME_DICT[mapName])
			except:
				pass

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/AtlasWindow.py")
		except:
			import exception
			exception.Abort("AtlasWindow.LoadWindow.LoadScript")

		try:
			self.board = self.GetChild("board")
			self.info = self.GetChild("info")
			self.info_text1 = self.GetChild("info_text1")
			self.info_text2 = self.GetChild("info_text2")
			self.info_text3 = self.GetChild("info_text3")
			self.info_text4 = self.GetChild("info_text4")
			self.info_text5 = self.GetChild("info_text5")

		except:
			import exception
			exception.Abort("AtlasWindow.LoadWindow.BindObject")

		self.AtlasMainWindow = self.AtlasRenderer()
		self.board.SetCloseEvent(self.Hide)
		self.AtlasMainWindow.SetParent(self.board)
		self.AtlasMainWindow.SetPosition(7, 30)
		self.tooltipInfo.SetParent(self.board)
		self.tooltipInfoEx.SetParent(self.board)
		self.infoGuildMark.SetParent(self.board)
		if app.ENABLE_MINIMAP_TELEPORT:
			self.board.SetOnMouseLeftButtonUpEvent(ui.__mem_func__(self.OnMouseLeftButtonUpEvent))
		self.SetPosition(40, 120)
		self.Hide()

		miniMap.RegisterAtlasWindow(self)

	def Destroy(self):
		miniMap.UnregisterAtlasWindow()
		self.ClearDictionary()
		self.AtlasMainWindow = None
		self.tooltipAtlasClose = 0
		self.tooltipInfo = None
		self.tooltipInfoEx = None
		self.infoGuildMark = None
		self.board = None
		self.info = None

	def OnUpdate(self):
		if not self.tooltipInfo:
			return
	
		if not self.infoGuildMark:
			return
	
		self.infoGuildMark.Hide()
		self.tooltipInfo.Hide()
		self.tooltipInfoEx.Hide()
		self.info.Hide()
		self.info_text1.Hide()
		self.info_text2.Hide()
		self.info_text3.Hide()
		self.info_text4.Hide()
		self.info_text5.Hide()
	
		if False == self.board.IsIn():
			return
	
		(x, y) = self.GetGlobalPosition()
		(mouseX, mouseY) = wndMgr.GetMousePosition()
		(bFind, sName, iPosX, iPosY, dwTextColor, dwGuildID, iRegenInterval, nextRespTime, mobVnum) = miniMap.GetAtlasInfo(mouseX, mouseY)
	
		if False == bFind:
			return
	
		if "empty_guild_area" == sName:
			sName = localeInfo.GUILD_EMPTY_AREA
	
		self.info.Show()
	
		if iRegenInterval != 0 and mobVnum != 0:
			currentTime = app.GetGlobalTimeStamp()
			timeRemaining = nextRespTime - currentTime
			
			if nextRespTime == 0:
				cooldownText = "Alive"
			elif timeRemaining > 0:
				cooldownText = FormatTime(timeRemaining)
			else:
				cooldownText = "Alive"
			
			self.info_text1.SetText(localeInfo.ATLAS_WINDOW_NAME % sName)
			self.info_text1.Show()
			
			self.info_text2.SetText(localeInfo.ATLAS_WINDOW_COORDINATES % (iPosX, iPosY))
			self.info_text2.Show()
			
			self.info_text3.SetText(localeInfo.ATLAS_WINDOW_RESPAWN % FormatTime(iRegenInterval))
			self.info_text3.Show()
			
			self.info_text4.SetText(localeInfo.ATLAS_WINDOW_COOLDOWN % cooldownText)
			self.info_text4.Show()
			
			self.info_text5.SetText(localeInfo.ATLAS_WINDOW_WIKI)
			self.info_text5.Show()
			
			self.info.SetSize(self.board.GetWidth(), 110)
			
		else:
			self.info_text1.SetText(localeInfo.ATLAS_WINDOW_NAME % sName)
			self.info_text1.Show()
			
			self.info_text2.SetText(localeInfo.ATLAS_WINDOW_COORDINATES % (iPosX, iPosY))
			self.info_text2.Show()
			
			self.info.SetSize(self.board.GetWidth(), 56)
		
		if 0 != dwGuildID:
			textWidth, textHeight = self.tooltipInfo.GetTextSize()
			self.infoGuildMark.SetIndex(dwGuildID)
			self.infoGuildMark.SetPosition(mouseX - x - textWidth - 18 - 5, mouseY - y)
			#self.infoGuildMark.Show()

	def Hide(self):
		if self.AtlasMainWindow:
			self.AtlasMainWindow.HideAtlas()
			self.AtlasMainWindow.Hide()
		ui.ScriptWindow.Hide(self)

	def Show(self):
		if self.AtlasMainWindow:
			(bGet, iSizeX, iSizeY) = miniMap.GetAtlasSize()
			if bGet:
				self.SetSize(iSizeX + 15, iSizeY + 38)

				if localeInfo.IsARABIC():
					self.board.SetPosition(iSizeX+15, 0)

				self.board.SetSize(iSizeX + 15, iSizeY + 38)
				self.info.SetPosition(0, iSizeY + 45)
				self.info.SetSize(iSizeX + 15, 10)
				self.AtlasMainWindow.ShowAtlas()
				self.AtlasMainWindow.Show()
		ui.ScriptWindow.Show(self)
		self.SetTop()

	def SetCenterPositionAdjust(self, x, y):
		self.SetPosition((wndMgr.GetScreenWidth() - self.GetWidth()) / 2 + x, (wndMgr.GetScreenHeight() - self.GetHeight()) / 2 + y)

	if app.ENABLE_MINIMAP_TELEPORT:
		def OnMouseLeftButtonUpEvent(self):
			(mouseX, mouseY) = wndMgr.GetMousePosition()
			(bFind, sName, iPosX, iPosY, dwTextColor, dwGuildID, iRegenInterval, nextRespTime, mobVnum) = miniMap.GetAtlasInfo(mouseX, mouseY)
			if app.IsPressed(app.DIK_LALT):
				if iRegenInterval != 0 and mobVnum:
					if app.ENABLE_WIKI:
						if constInfo.GetInterfaceInstance().wndWiki:
							constInfo.GetInterfaceInstance().wndWiki.searchBossWithVnum(mobVnum)
			elif chr.IsGameMaster(0):
				net.SendChatPacket("/go %s %s" % (str(iPosX), str(iPosY)))

	def OnPressEscapeKey(self):
		self.Hide()
		return True

def __RegisterMiniMapColor(type, rgb):
	miniMap.RegisterColor(type, rgb[0], rgb[1], rgb[2])

class MiniMap(ui.ScriptWindow):

	CANNOT_SEE_INFO_MAP_DICT = {
		"metin2_map_monkeydungeon" : False,
		"metin2_map_monkeydungeon_02" : False,
		"metin2_map_monkeydungeon_03" : False,
		"metin2_map_devilsCatacomb" : False,
	}
	
	if app.BL_KILL_BAR:
		KILL_BAR_COOLTIME = 4.0
		KILL_BAR_MOVE_SPEED = 3.0
		KILL_BAR_MOVE_DISTANCE = 63.0
		KILL_BAR_MAX_ITEM = 5

		KILL_BAR_RACE = {
			playersettingmodule.RACE_WARRIOR_M: "|Ekill_bar/warrior_m|e",
			playersettingmodule.RACE_ASSASSIN_W	: "|Ekill_bar/assassin_w|e",
			playersettingmodule.RACE_SURA_M		: "|Ekill_bar/sura_m|e",
			playersettingmodule.RACE_SHAMAN_W	: "|Ekill_bar/shaman_w|e",
			playersettingmodule.RACE_WARRIOR_W	: "|Ekill_bar/warrior_w|e",
			playersettingmodule.RACE_ASSASSIN_M	: "|Ekill_bar/assassin_m|e",
			playersettingmodule.RACE_SURA_W		: "|Ekill_bar/sura_w|e",
			playersettingmodule.RACE_SHAMAN_M	: "|Ekill_bar/shaman_m|e",
		}

		KILL_BAR_WEAPON_TYPE = {
			"FIST": "|Ekill_bar/fist|e",
			item.WEAPON_SWORD: "|Ekill_bar/sword|e",
			item.WEAPON_DAGGER: "|Ekill_bar/dagger|e",
			item.WEAPON_BOW: "|Ekill_bar/bow|e",
			item.WEAPON_TWO_HANDED: "|Ekill_bar/twohand|e",
			item.WEAPON_BELL: "|Ekill_bar/bell|e",
			item.WEAPON_FAN: "|Ekill_bar/fan|e",
		}

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.__Initialize()

		miniMap.Create()
		miniMap.SetScale(2.0)

		self.AtlasWindow = AtlasWindow()
		self.AtlasWindow.LoadWindow()
		self.AtlasWindow.Hide()

		self.tooltipMiniMapOpen = MapTextToolTip()
		self.tooltipMiniMapOpen.SetText(localeInfo.MINIMAP)
		self.tooltipMiniMapOpen.Show()
		
		self.tooltipDungeonInfo = MapTextToolTip()
		self.tooltipDungeonInfo.SetText(localeInfo.MINIMAP_BUTTON_DUNGEON)
		
		self.tooltipWarpWindow = MapTextToolTip()
		self.tooltipWarpWindow.SetText(localeInfo.MINIMAP_BUTTON_WARP)
		
		self.tooltipMissions = MapTextToolTip()
		self.tooltipMissions.SetText(localeInfo.MINIMAP_BUTTON_MISSIONS)
		
		self.tooltipCollection = MapTextToolTip()
		self.tooltipCollection.SetText(localeInfo.MINIMAP_BUTTON_COLLECTION)
		
		self.tooltipTitle = MapTextToolTip()
		self.tooltipTitle.SetText(localeInfo.MINIMAP_BUTTON_TITLE)
		
		
		self.tooltipAtlasOpen = MapTextToolTip()
		self.tooltipInfo = MapTextToolTip()
		self.tooltipInfo.Show()

		if not miniMap.IsAtlas():
			self.tooltipAtlasOpen.SetText(localeInfo.MINIMAP_CAN_NOT_SHOW_AREAMAP)

		self.tooltipInfo = MapTextToolTip()
		self.tooltipInfo.Show()

		self.mapName = ""

		self.isLoaded = 0
		self.canSeeInfo = True

		self.imprisonmentDuration = 0
		self.imprisonmentEndTime = 0
		self.imprisonmentEndTimeText = ""


	def __del__(self):
		miniMap.Destroy()
		ui.ScriptWindow.__del__(self)


	def __Initialize(self):
		self.positionInfo = 0
		self.observerCount = 0

		self.OpenWindow = 0
		self.CloseWindow = 0
		self.ScaleUpButton = 0
		self.ScaleDownButton = 0
		self.MiniMapHideButton = 0
		self.MiniMapShowButton = 0
		self.AtlasShowButton = 0
		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			self.DungeonInfoShowButton = 0

		self.tooltipMiniMapOpen = 0
		self.tooltipDungeonInfo = 0
		self.tooltipWarpWindow = 0
		self.tooltipMissions = 0
		self.tooltipCollection = 0
		self.tooltipAtlasOpen = 0
		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			self.tooltipDungeonInfoOpen = 0
		self.tooltipInfo = None
		self.serverInfo = None
		if app.BL_KILL_BAR:
			self.KillList = list()

	def BindInterfaceClass(self, interface):
		self.interface = interface

	def SetMapName(self, mapName):
		self.mapName=mapName
		self.AtlasWindow.SetMapName(mapName)

		if self.CANNOT_SEE_INFO_MAP_DICT.has_key(mapName):
			self.canSeeInfo = False
			self.HideMiniMap()
			self.tooltipMiniMapOpen.SetText(localeInfo.MINIMAP_CANNOT_SEE)
		else:
			self.canSeeInfo = True
			self.ShowMiniMap()
			self.tooltipMiniMapOpen.SetText(localeInfo.MINIMAP)

	def SetImprisonmentDuration(self, duration):
		self.imprisonmentDuration = duration
		self.imprisonmentEndTime = app.GetGlobalTimeStamp() + duration

		self.__UpdateImprisonmentDurationText()

	def __UpdateImprisonmentDurationText(self):
		restTime = max(self.imprisonmentEndTime - app.GetGlobalTimeStamp(), 0)

		imprisonmentEndTimeText = localeInfo.SecondToDHM(restTime)
		if imprisonmentEndTimeText != self.imprisonmentEndTimeText:
			self.imprisonmentEndTimeText = imprisonmentEndTimeText
			self.serverInfo.SetText("%s: %s" % (uiScriptLocale.AUTOBAN_QUIZ_REST_TIME, self.imprisonmentEndTimeText))

	def Show(self):
		self.__LoadWindow()

		ui.ScriptWindow.Show(self)

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			if localeInfo.IsARABIC():
				pyScrLoader.LoadScriptFile(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "Minimap.py")
			else:
				pyScrLoader.LoadScriptFile(self, "UIScript/MiniMap.py")
		except:
			import exception
			exception.Abort("MiniMap.LoadWindow.LoadScript")

		try:
			self.OpenWindow = self.GetChild("OpenWindow")
			self.MiniMapWindow = self.GetChild("MiniMapWindow")
			self.ScaleUpButton = self.GetChild("ScaleUpButton")
			self.ScaleDownButton = self.GetChild("ScaleDownButton")
			self.MiniMapHideButton = self.GetChild("MiniMapHideButton")
			self.AtlasShowButton = self.GetChild("AtlasShowButton")
			self.DungeonButton = self.GetChild("DungeonButton")
			self.MissionButton = self.GetChild("MissionButton")
			self.TeleportButton = self.GetChild("TeleportButton")
			self.CollectionButton = self.GetChild("CollectionButton")
			self.TitleButton = self.GetChild("TitleButton")
			self.CloseWindow = self.GetChild("CloseWindow")
			self.MiniMapShowButton = self.GetChild("MiniMapShowButton")
			self.positionInfo = self.GetChild("PositionInfo")
			self.observerCount = self.GetChild("ObserverCount")
			self.serverInfo = self.GetChild("ServerInfo")

			#self.buttonChangeChannel = {
			#	1:self.GetChild("Channel1"),
			#	2:self.GetChild("Channel2"),
			#	3:self.GetChild("Channel3"),
			#	4:self.GetChild("Channel4"),
			#	5:self.GetChild("Channel5"),
			#	6:self.GetChild("Channel6")
			#}
		except:
			import exception
			exception.Abort("MiniMap.LoadWindow.Bind")

		if constInfo.MINIMAP_POSITIONINFO_ENABLE==0:
			self.positionInfo.Hide()

		self.serverInfo.SetText(net.GetServerInfo())
		self.ScaleUpButton.SetEvent(ui.__mem_func__(self.ScaleUp))
		self.ScaleDownButton.SetEvent(ui.__mem_func__(self.ScaleDown))
		self.MiniMapHideButton.SetEvent(ui.__mem_func__(self.HideMiniMap))
		self.MiniMapShowButton.SetEvent(ui.__mem_func__(self.ShowMiniMap))
		self.DungeonButton.SetEvent(ui.__mem_func__(self.ShowDungeonWindow))
		self.TeleportButton.SetEvent(ui.__mem_func__(self.ShowWarpWindow))
		self.MissionButton.SetEvent(ui.__mem_func__(self.ShowMissionWindow))
		self.CollectionButton.SetEvent(ui.__mem_func__(self.ShowColletionWindow))
		self.TitleButton.SetEvent(ui.__mem_func__(self.ShowTitleWindow))

		#self.buttonChangeChannel[1].SetEvent(ui.__mem_func__(self.ChangeChannel1))
		#self.buttonChangeChannel[2].SetEvent(ui.__mem_func__(self.ChangeChannel2))
		#self.buttonChangeChannel[3].SetEvent(ui.__mem_func__(self.ChangeChannel3))
		#self.buttonChangeChannel[4].SetEvent(ui.__mem_func__(self.ChangeChannel4))
		#self.buttonChangeChannel[5].SetEvent(ui.__mem_func__(self.ChangeChannel5))
		#self.buttonChangeChannel[6].SetEvent(ui.__mem_func__(self.ChangeChannel6))

		if miniMap.IsAtlas():
			self.AtlasShowButton.SetEvent(ui.__mem_func__(self.ToggleAtlasWindow))

		(ButtonPosX, ButtonPosY) = self.MiniMapShowButton.GetGlobalPosition()
		self.tooltipMiniMapOpen.SetTooltipPosition(ButtonPosX, ButtonPosY)




		(ButtonPosX, ButtonPosY) = self.AtlasShowButton.GetGlobalPosition()
		self.tooltipAtlasOpen.SetTooltipPosition(ButtonPosX, ButtonPosY)
		
		(ButtonPosX, ButtonPosY) = self.DungeonButton.GetGlobalPosition()
		self.tooltipDungeonInfo.SetTooltipPosition(ButtonPosX, ButtonPosY)
		
		(ButtonPosX, ButtonPosY) = self.TeleportButton.GetGlobalPosition()
		self.tooltipWarpWindow.SetTooltipPosition(ButtonPosX, ButtonPosY)
		
		(ButtonPosX, ButtonPosY) = self.MissionButton.GetGlobalPosition()
		self.tooltipMissions.SetTooltipPosition(ButtonPosX, ButtonPosY)
		
		(ButtonPosX, ButtonPosY) = self.CollectionButton.GetGlobalPosition()
		self.tooltipCollection.SetTooltipPosition(ButtonPosX, ButtonPosY)
		
		(ButtonPosX, ButtonPosY) = self.TitleButton.GetGlobalPosition()
		self.tooltipTitle.SetTooltipPosition(ButtonPosX, ButtonPosY)

		self.ShowMiniMap()

	def Destroy(self):
		self.HideMiniMap()

		self.AtlasWindow.Destroy()
		self.AtlasWindow = None
		if app.BL_KILL_BAR:
			self.KillList = None

		self.ClearDictionary()

		self.__Initialize()

	def UpdateObserverCount(self, observerCount):
		if observerCount>0:
			self.observerCount.Show()
		elif observerCount<=0:
			self.observerCount.Hide()

		self.observerCount.SetText(localeInfo.MINIMAP_OBSERVER_COUNT % observerCount)

	def ShowAtlasWhenReady(self):
		if miniMap.IsAtlas():
			self.ShowAtlas()
		else:
			self._pendingShowAtlas = True

	def OnUpdate(self):
		if getattr(self, '_pendingShowAtlas', False) and miniMap.IsAtlas():
			self._pendingShowAtlas = False
			self.ShowAtlas()

		(x, y, z) = player.GetMainCharacterPosition()
		miniMap.Update(x, y)

		self.GetChild("dayInfo").SetText(time.strftime("%d.%m.%y")+" - "+time.strftime("%H:%M:%S"))
		self.positionInfo.SetText("(%.0f, %.0f)" % (x/100, y/100))

		if self.tooltipInfo:
			if True == self.MiniMapWindow.IsIn():
				(mouseX, mouseY) = wndMgr.GetMousePosition()
				(bFind, sName, iPosX, iPosY, dwTextColor) = miniMap.GetInfo(mouseX, mouseY)
				if bFind == 0:
					self.tooltipInfo.Hide()
				elif not self.canSeeInfo:
					self.tooltipInfo.SetText("%s(%s)" % (sName, localeInfo.UI_POS_UNKNOWN))
					self.tooltipInfo.SetTooltipPosition(mouseX - 5, mouseY)
					self.tooltipInfo.SetTextColor(dwTextColor)
					self.tooltipInfo.Show()
				else:
					if localeInfo.IsARABIC() and sName[-1].isalnum():
						self.tooltipInfo.SetText("(%s)%d, %d" % (sName, iPosX, iPosY))
					else:
						self.tooltipInfo.SetText("%s(%d, %d)" % (sName, iPosX, iPosY))
					self.tooltipInfo.SetTooltipPosition(mouseX - 5, mouseY)
					self.tooltipInfo.SetTextColor(dwTextColor)
					self.tooltipInfo.Show()
			else:
				self.tooltipInfo.Hide()

			if self.imprisonmentDuration:
				self.__UpdateImprisonmentDurationText()

		if True == self.MiniMapShowButton.IsIn():
			self.tooltipMiniMapOpen.Show()
		else:
			self.tooltipMiniMapOpen.Hide()

		if True == self.AtlasShowButton.IsIn():
			self.tooltipAtlasOpen.Show()
		else:
			self.tooltipAtlasOpen.Hide()

		if True == self.DungeonButton.IsIn():
			self.tooltipDungeonInfo.Show()
		else:
			self.tooltipDungeonInfo.Hide()

		if True == self.TeleportButton.IsIn():
			self.tooltipWarpWindow.Show()
		else:
			self.tooltipWarpWindow.Hide()

		if True == self.MissionButton.IsIn():
			self.tooltipMissions.Show()
		else:
			self.tooltipMissions.Hide()

		if True == self.CollectionButton.IsIn():
			self.tooltipCollection.Show()
		else:
			self.tooltipCollection.Hide()

		if True == self.TitleButton.IsIn():
			self.tooltipTitle.Show()
		else:
			self.tooltipTitle.Hide()

		if app.BL_KILL_BAR:
			if self.KillList:
				self.KillList = filter(
					lambda obj: obj["CoolTime"] > app.GetTime(), self.KillList)
				for obj in self.KillList:
					(xLocal, yLocal) = obj["ThinBoard"].GetLocalPosition()
					if obj["MOVE_X"] > 0.0:
						obj["ThinBoard"].SetPosition(xLocal - MiniMap.KILL_BAR_MOVE_SPEED, yLocal)
						obj["MOVE_X"] -= MiniMap.KILL_BAR_MOVE_SPEED
					if obj["MOVE_Y"] > 0.0:
						obj["ThinBoard"].SetPosition(xLocal, yLocal + MiniMap.KILL_BAR_MOVE_SPEED)
						obj["MOVE_Y"] -= MiniMap.KILL_BAR_MOVE_SPEED

	def OnRender(self):
		(x, y) = self.GetGlobalPosition()
		fx = float(x)
		fy = float(y)
		miniMap.Render(fx+41.0, fy + 6.0)

	def Close(self):
		self.HideMiniMap()
		
	if app.BL_KILL_BAR:
		def RepositionKillBar(self, obj):
			obj["MOVE_Y"] += MiniMap.KILL_BAR_MOVE_DISTANCE
			return obj

		def AddKillInfo(self, killer, victim, killer_race, victim_race, weapon_type):
			if len(self.KillList) >= MiniMap.KILL_BAR_MAX_ITEM:
				self.KillList.sort(
					key=lambda obj: obj["CoolTime"], reverse=True)
				del self.KillList[-1]
			
			if self.KillList:
				self.KillList = map(self.RepositionKillBar, self.KillList)

			TBoard = ui.ThinBoard()
			TBoard.SetParent(self)
			TBoard.SetSize(185, 10)
			TBoard.SetPosition(15, 385)
			TBoard.Show()

			KillText = ui.TextLine()
			KillText.SetText("{} {} {} {} {}".format(MiniMap.KILL_BAR_RACE.get(int(killer_race), ""), killer, MiniMap.KILL_BAR_WEAPON_TYPE.get(
				int(weapon_type), MiniMap.KILL_BAR_WEAPON_TYPE.get("FIST")), victim, MiniMap.KILL_BAR_RACE.get(int(victim_race), "")))
			KillText.SetParent(TBoard)
			KillText.SetWindowHorizontalAlignCenter()
			KillText.SetWindowVerticalAlignCenter()
			KillText.SetHorizontalAlignCenter()
			KillText.SetVerticalAlignCenter()
			KillText.Show()

			KillDict = dict()
			KillDict["ThinBoard"] = TBoard
			KillDict["TextLine"] = KillText
			KillDict["CoolTime"] = app.GetTime() + MiniMap.KILL_BAR_COOLTIME
			KillDict["MOVE_X"] = MiniMap.KILL_BAR_MOVE_DISTANCE
			KillDict["MOVE_Y"] = 0.0

			self.KillList.append(KillDict)

	def HideMiniMap(self):
		miniMap.Hide()
		self.OpenWindow.Hide()
		self.CloseWindow.Show()

	#def ChangeChannel1(self):
	#	if self.buttonChangeChannel[1]:
	#		for i in xrange(1,7):
	#			self.buttonChangeChannel[i].SetUp()
	#			self.buttonChangeChannel[1].Down()
	#		net.MoveChannelGame(1)
	#
	#def ChangeChannel2(self):
	#	if self.buttonChangeChannel[2]:
	#		for i in xrange(1,7):
	#			self.buttonChangeChannel[i].SetUp()
	#			self.buttonChangeChannel[2].Down()
	#		net.MoveChannelGame(2)
	#
	#def ChangeChannel3(self):
	#	if self.buttonChangeChannel[3]:
	#		for i in xrange(1,7):
	#			self.buttonChangeChannel[i].SetUp()
	#			self.buttonChangeChannel[3].Down()
	#		net.MoveChannelGame(3)
	#
	#def ChangeChannel4(self):
	#	if self.buttonChangeChannel[4]:
	#		for i in xrange(1,7):
	#			self.buttonChangeChannel[i].SetUp()
	#			self.buttonChangeChannel[4].Down()
	#		net.MoveChannelGame(4)
	#
	#def ChangeChannel5(self):
	#	if self.buttonChangeChannel[5]:
	#		for i in xrange(1,7):
	#			self.buttonChangeChannel[i].SetUp()
	#			self.buttonChangeChannel[5].Down()
	#		net.MoveChannelGame(5)
	#
	#def ChangeChannel6(self):
	#	if self.buttonChangeChannel[6]:
	#		for i in xrange(1,7):
	#			self.buttonChangeChannel[i].SetUp()
	#			self.buttonChangeChannel[6].Down()
	#		net.MoveChannelGame(6)


	def ShowMiniMap(self):
		if not self.canSeeInfo:
			return

		miniMap.Show()
		self.OpenWindow.Show()
		self.CloseWindow.Hide()

	def isShowMiniMap(self):
		return miniMap.isShow()

	def ScaleUp(self):
		miniMap.ScaleUp()

	def ScaleDown(self):
		miniMap.ScaleDown()
		
	if app.BL_MOVE_CHANNEL:
		def RefreshServerInfo(self, channelNumber):
			#if net.GetChannelNumber() == 99:
			#	for i in xrange(1,7):
			#		self.buttonChangeChannel[i].Down()
			#elif net.GetChannelNumber() == 1:
			#	self.buttonChangeChannel[1].Down()
			#elif net.GetChannelNumber() == 2:
			#	self.buttonChangeChannel[2].Down()
			#elif net.GetChannelNumber() == 3:
			#	self.buttonChangeChannel[3].Down()
			#elif net.GetChannelNumber() == 4:
			#	self.buttonChangeChannel[4].Down()
			#elif net.GetChannelNumber() == 5:
			#	self.buttonChangeChannel[5].Down()
			#elif net.GetChannelNumber() == 6:
			#	self.buttonChangeChannel[6].Down()
				
			if net.GetChannelNumber() == 99:
				serverInfoStr = "|cFFc7a374Elvion|r - Global Channel"
			else:
				serverInfoStr = localeInfo.MINIMAP_SERVER_INFO_REGULAR.format(channelNumber)

			self.serverInfo.SetText(serverInfoStr)
			net.SetServerInfo(serverInfoStr)

	def ShowAtlas(self):
		if not miniMap.IsAtlas():
			return
		if not self.AtlasWindow.IsShow():
			self.AtlasWindow.Show()

	def ToggleAtlasWindow(self):
		if not miniMap.IsAtlas():
			return
		if self.AtlasWindow.IsShow():
			self.AtlasWindow.Hide()
		else:
			self.AtlasWindow.Show()
			
	def ShowDungeonWindow(self):
		interface = constInfo.GetInterfaceInstance()
		if interface != None:
			interface.OpenDungeonInfo()
			
	def ShowWarpWindow(self):
		interface = constInfo.GetInterfaceInstance()
		if interface != None:
			interface.OpenDungeonInfo()
			
	def ShowMissionWindow(self):
		interface = constInfo.GetInterfaceInstance()
		if interface != None:
			interface.ToggleCollectWindow()
			
	def ShowColletionWindow(self):
		interface = constInfo.GetInterfaceInstance()
		if interface != None:
			interface.ToggleCollectionWindow()
			
	def ShowTitleWindow(self):
		interface = constInfo.GetInterfaceInstance()
		if interface != None:
			interface.OpenTitleAchievement()

	if app.ENABLE_LOADING_PERFORMANCE:
		def IsShowingAtlas(self):
			return self.AtlasWindow.IsShow()
			