#-*- coding: iso-8859-1 -*-
import os
import app
import dbg
import grp
import item
import background
import chr
import chrmgr
import player
import snd
import chat
import textTail
import snd
import net
import wndMgr
import acce
import systemSetting
import quest
import guild
import skill
import messenger
import localeInfo
import constInfo
import exchange
import ime
import time
import uiScriptLocale

import ui
import uiCommon
import uiPhaseCurtain
import uiMapNameShower
import uiAffectShower
import uiPlayerGauge
import uiCharacter
import uiTarget
import uiMount
import uiPrivateShopBuilder

import mouseModule
import consoleModule
import localeInfo
import interfaceModule
if app.ENABLE_SKILL_SELECT_FEATURE:
	import uiskillchoose
if app.ENABLE_OFFLINE_SHOP:
    import uiOfflineShop, offlineshop

import musicInfo
import debugInfo
import stringCommander
if app.ENABLE_KEYCHANGE_SYSTEM:
	import uiKeyChange
if app.ENABLE_OFFLINE_SHOP_SYSTEM:
	import uiOfflineShopBuilder
	import uiOfflineShop

if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
	import uiPrivateShop

import uiCube
import uisecurity
import uiRanking
import uiEnlightenment
import uiFishWiki
if app.ENABLE_ITEMSHOP:
	import uiItemShopNew

if app.ENABLE_IN_GAME_LOG_SYSTEM:
	import uiingamelog
	
import tween
import tweenCamera
import cinemachine

from _weakref import proxy

SCREENSHOT_CWDSAVE = True
SCREENSHOT_DIR = None

cameraDistance = 1550.0
cameraPitch = 27.0
cameraRotation = 0.0
cameraHeight = 100.0

testAlignment = 0

class GameWindow(ui.ScriptWindow):
	def __init__(self, stream):
		ui.ScriptWindow.__init__(self, "GAME")
		self.SetWindowName("game")
		net.SetPhaseWindow(net.PHASE_WINDOW_GAME, self)
		player.SetGameWindow(self)

		self.quickSlotPageIndex = 0
		self.lastPKModeSendedTime = 0
		self.pressNumber = None

		if app.ENABLE_BOT_CONTROL:
			self.botControlWnd = None

		if app.ENABLE_SKILL_SELECT_FEATURE:
			self.skillSelect = None

		self.guildWarQuestionDialog = None
		self.interface = None
		self.targetBoard = None
		self.console = None
		self.mapNameShower = None
		self.affectShower = None
		self.playerGauge = None

		if app.ENABLE_IN_GAME_LOG_SYSTEM:
			 self.wndInGameLog = uiingamelog.IngameLogWindow()
		if app.ENABLE_KEYCHANGE_SYSTEM:
			self.wndKeyChange = None

		if app.ENABLE_AUTO_SHOUT:
			self.lastShoutTime = 0

		self.stream=stream
		self.interface = interfaceModule.Interface()
		if app.ENABLE_EVENT_MANAGER:
			constInfo.SetInterfaceInstance(self.interface)
		interfaceModule.SetInstance(self.interface)
		self.interface.MakeInterface()
		self.interface.ShowDefaultWindows()
		constInfo.SetInterfaceInstance(self.interface)

		self.curtain = uiPhaseCurtain.PhaseCurtain()
		#self.curtain.speed = 0.03
		self.curtain.Hide()

		self.targetBoard = uiTarget.TargetBoard()
		self.targetBoard.SetWhisperEvent(ui.__mem_func__(self.interface.OpenWhisperDialog))
		self.targetBoard.Hide()

		if app.ENABLE_OFFLINE_SHOP:
			offlineshop.HideShopNames()
			
		if app.ENABLE_SKILL_SELECT_FEATURE:
			self.skillSelect = uiselectskill.SkillSelectWindow()
			self.skillSelect.Hide()

		self.console = consoleModule.ConsoleWindow()
		self.console.BindGameClass(self)
		self.console.SetConsoleSize(wndMgr.GetScreenWidth(), 200)
		self.console.Hide()

		self.mapNameShower = uiMapNameShower.MapNameShower()
		self.affectShower = uiAffectShower.AffectShower()

		self.enlightWindow = None
		
		self.rankingWindow = uiRanking.RankingWindow()
		self.weeklyRankingWindow = uiRanking.WeeklyRankingWindow()
		self.fishWikiWindow = uiFishWiki.FishingWikipediaWindow()


		self.playerGauge = uiPlayerGauge.PlayerGauge(self)
		self.playerGauge.Hide()
		
		self.cinemachineWindow = cinemachine.CinemachineWindow()
		self.cinemachineWindow.Hide()

		self.tweenMgr = tween.TweenManager()
		
		self.itemDropQuestionDialog = None

		self.__SetQuickSlotMode()

		if app.ENABLE_SKYBOX_SELECT:
			self.interface.wndgameOption.RefreshSkyBoxButtons()

		self.__ServerCommand_Build()
		self.__ProcessPreservedServerCommand()
		if app.ENABLE_KEYCHANGE_SYSTEM:
			self.wndKeyChange = uiKeyChange.KeyChangeWindow(self, self.interface)
			self.ADDKEYBUFFERCONTROL = player.KEY_ADDKEYBUFFERCONTROL
			self.ADDKEYBUFFERALT = player.KEY_ADDKEYBUFFERALT
			self.ADDKEYBUFFERSHIFT = player.KEY_ADDKEYBUFFERSHIFT

	def __del__(self):
		player.SetGameWindow(0)
		net.ClearPhaseWindow(net.PHASE_WINDOW_GAME, self)
		ui.ScriptWindow.__del__(self)

	def Open(self):
		app.SetFrameSkip(1)

		self.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())

		self.quickSlotPageIndex = 0
		self.PickingCharacterIndex = -1
		self.PickingItemIndex = -1
		self.consoleEnable = False
		self.isShowDebugInfo = False
		self.ShowNameFlag = False
		self.isCinemaMode = False;
		self.freeCameraSpeed = 1
		self.cameraFov = 30

		self.enableXMasBoom = False
		self.startTimeXMasBoom = 0.0
		self.indexXMasBoom = 0

		global cameraDistance, cameraPitch, cameraRotation, cameraHeight

		app.SetCamera(cameraDistance, cameraPitch, cameraRotation, cameraHeight)

		constInfo.SET_DEFAULT_CAMERA_MAX_DISTANCE()
		constInfo.SET_DEFAULT_CHRNAME_COLOR()
		constInfo.SET_DEFAULT_FOG_LEVEL()
		constInfo.SET_DEFAULT_CONVERT_EMPIRE_LANGUAGE_ENABLE()
		constInfo.SET_DEFAULT_USE_ITEM_WEAPON_TABLE_ATTACK_BONUS()
		constInfo.SET_DEFAULT_USE_SKILL_EFFECT_ENABLE()

		constInfo.SET_TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE()

		import event
		event.SetLeftTimeString(localeInfo.UI_LEFT_TIME)

		textTail.EnablePKTitle(constInfo.PVPMODE_ENABLE)

		if constInfo.PVPMODE_TEST_ENABLE:
			self.testPKMode = ui.TextLine()
			self.testPKMode.SetFontName(localeInfo.UI_DEF_FONT)
			self.testPKMode.SetPosition(0, 15)
			self.testPKMode.SetWindowHorizontalAlignCenter()
			self.testPKMode.SetHorizontalAlignCenter()
			self.testPKMode.SetFeather()
			self.testPKMode.SetOutline()
			self.testPKMode.Show()

			self.testAlignment = ui.TextLine()
			self.testAlignment.SetFontName(localeInfo.UI_DEF_FONT)
			self.testAlignment.SetPosition(0, 35)
			self.testAlignment.SetWindowHorizontalAlignCenter()
			self.testAlignment.SetHorizontalAlignCenter()
			self.testAlignment.SetFeather()
			self.testAlignment.SetOutline()
			self.testAlignment.Show()

		self.__BuildKeyDict()
		if app.ENABLE_KEYCHANGE_SYSTEM:
			pass
		self.__BuildDebugInfo()

		constInfo.IS_BONUS_CHANGER = False
		constInfo.IS_ACCE_WINDOW = False
		constInfo.IS_DRAGON_SOUL_OPEN = False

		uiPrivateShopBuilder.Clear()
		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			uiOfflineShopBuilder.Clear()

		exchange.InitTrading()

		if len(constInfo.lastSentenceStack) > 0:
			constInfo.lastSentencePos = 0

		snd.SetMusicVolume(systemSetting.GetMusicVolume()*net.GetFieldMusicVolume())
		snd.SetSoundVolume(systemSetting.GetSoundVolume())

		netFieldMusicFileName = net.GetFieldMusicFileName()
		if netFieldMusicFileName:
			snd.FadeInMusic("content/sound/BGM/" + netFieldMusicFileName)
		elif musicInfo.fieldMusic != "":
			snd.FadeInMusic("content/sound/BGM/" + musicInfo.fieldMusic)
		player.LoadBossNPC()
		self.__SetQuickSlotMode()
		self.__SelectQuickPage(self.quickSlotPageIndex)

		self.SetFocus()
		self.Show()
		app.ShowCursor()

		net.SendEnterGamePacket()

		try:
			self.StartGame()
		except:
			import exception
			exception.Abort("GameWindow.Open")

		if app.ENABLE_AUTO_SHOUT:
			self.lastShoutTime = app.GetTime() + constInfo.SHOUT_PER_SECOND

	def Close(self):
		self.Hide()

		global cameraDistance, cameraPitch, cameraRotation, cameraHeight
		(cameraDistance, cameraPitch, cameraRotation, cameraHeight) = app.GetCamera()

		if musicInfo.fieldMusic != "":
			snd.FadeOutMusic("content/sound/BGM/"+ musicInfo.fieldMusic)

		self.onPressKeyDict = None
		self.onClickKeyDict = None

		if app.ENABLE_MULTI_FARM_BLOCK:
			app.SetMultiFarmExeIcon(0)
		chat.Close()
		snd.StopAllSound()
		grp.InitScreenEffect()
		chr.Destroy()
		textTail.Clear()
		quest.Clear()
		background.Destroy()
		guild.Destroy()
		messenger.Destroy()
		if app.ENABLE_SKILL_SELECT_FEATURE and self.skillSelect:
			self.skillSelect.Destroy()
			self.skillSelect = None
		skill.ClearSkillData()
		wndMgr.Unlock()
		mouseModule.mouseController.DeattachObject()

		if self.guildWarQuestionDialog:
			self.guildWarQuestionDialog.Close()

		self.guildNameBoard = None
		self.partyRequestQuestionDialog = None
		self.partyInviteQuestionDialog = None
		self.guildInviteQuestionDialog = None
		self.guildWarQuestionDialog = None
		self.messengerAddFriendQuestion = None
		self.cinemachineWindow.Destroy()
		self.cinemachineWindow = None

		self.tweenMgr.Destroy()

		if app.ENABLE_IN_GAME_LOG_SYSTEM:
			if self.wndInGameLog:
				self.wndInGameLog.Destroy()
				self.wndInGameLog = None

		if app.ENABLE_BOT_CONTROL:
			if self.botControlWnd:
				self.botControlWnd.Constructor()
				self.botControlWnd = None

		self.itemDropQuestionDialog = None

		self.confirmDialog = None

		self.PrintCoord = None
		self.FrameRate = None
		self.Pitch = None
		self.Splat = None
		self.TextureNum = None
		self.ObjectNum = None
		self.ViewDistance = None
		self.PrintMousePos = None

		self.ClearDictionary()

		self.playerGauge = None
		self.mapNameShower = None
		self.affectShower = None

		if self.console:
			self.console.BindGameClass(0)
			self.console.Close()
			self.console=None

		if self.targetBoard:
			self.targetBoard.Destroy()
			self.targetBoard = None

		if self.interface:
			self.interface.HideAllWindows()
			self.interface.Close()
			self.interface=None

		if self.fishWikiWindow:
			self.fishWikiWindow.Close()
			self.fishWikiWindow = None
			
		if self.rankingWindow:
			self.rankingWindow.Close()
			self.rankingWindow = None

		if self.enlightWindow != None:
			self.enlightWindow.Close()
			self.enlightWindow = None

		if self.weeklyRankingWindow:
			self.weeklyRankingWindow.Close()
			self.weeklyRankingWindow = None
			
		interfaceModule.SetInstance(None)

		player.ClearSkillDict()
		player.ResetCameraRotation()

		self.KillFocus()
		if app.ENABLE_EVENT_MANAGER:
			constInfo.SetInterfaceInstance(None)
		constInfo.SetInterfaceInstance(None)
		app.HideCursor()
		if app.ENABLE_KEYCHANGE_SYSTEM:
			if self.wndKeyChange:
				self.wndKeyChange.KeyChangeWindowClose(True)
				self.wndKeyChange = None
				
		if app.ENABLE_VOTE4BUFF:
			constInfo.VOTE4BUFF_DATA["LOADED"] = False

		print "---------------------------------------------------------------------------- CLOSE GAME WINDOW"

	def ToggleCameraMode(self):
		self.__SetFreeCamera()

	def __SetFreeCamera(self):
		self.isCinemaMode = not self.isCinemaMode
		app.FreeCamera()

		if self.isCinemaMode:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "Cinematic mode ON!")
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "Cinematic mode OFF!")

	def __AddCameraFov(self):
		if not self.isCinemaMode:
			return

		self.cameraFov += 2
		app.SetFov(self.cameraFov)

	def __SubtractCameraFov(self):
		if not self.isCinemaMode:
			return

		self.cameraFov -= 2
		app.SetFov(self.cameraFov)

	def __AddFreeCameraSpeed(self, speed=0.05):
		if not self.isCinemaMode:
			return

		self.freeCameraSpeed += speed
		app.FreeCameraSpeed(self.freeCameraSpeed)

	def __SubtractFreeCameraSpeed(self, speed=0.05):
		if not self.isCinemaMode:
			return

		self.freeCameraSpeed -= speed
		app.FreeCameraSpeed(self.freeCameraSpeed)

	def CreateBezierPath(self, valueKey, num_points):
		import mathUtils

		waypoints = []
		n = len(constInfo.CAMERA_WAYPOINTS)
		for i in range(n):
			waypoints.append(constInfo.CAMERA_WAYPOINTS[i][valueKey])

		for i in range(num_points):
			t = i / float(num_points - 1)
			point = 0
			if valueKey == "rotation":
				point = mathUtils.bezier_rotation(t, *waypoints)
			else:
				point = mathUtils.bezier(t, *waypoints)
			if not constInfo.CAMERA_BEZIER_POINTS.has_key(valueKey):
				constInfo.CAMERA_BEZIER_POINTS[valueKey] = {}
			constInfo.CAMERA_BEZIER_POINTS[valueKey][i] = point

	def TestFreeCamera(self, num):
		(distance, pitch, rotation, height) = app.GetCamera()
		(x, y, z) = app.GetCameraPosition()

		if num == 1: # add waypoint
			(distance, pitch, rotation, height) = app.GetCamera()
			(x, y, z) = app.GetCameraPosition()

			waypointIndex = len(constInfo.CAMERA_WAYPOINTS)
			constInfo.CAMERA_WAYPOINTS[waypointIndex] = {
				"x" : x,
				"y" : y,
				"z" : z,
				"distance" : distance,
				"pitch" : pitch,
				"rotation" : rotation,
				"fov" : self.cameraFov,
			}

			chat.AppendChat(chat.CHAT_TYPE_INFO, "Added camera waypoint (%d) x: %d, y: %d, z: %d, distance: %d, pitch: %d, rotation: %d" %
							(waypointIndex, x, y, z, distance, pitch, rotation))

		elif num == 2:  # reset waypoints
			constInfo.CAMERA_WAYPOINTS = {}
			chat.AppendChat(chat.CHAT_TYPE_INFO, "Reset camera waypoints.")

		elif num == 3: # play waypoints bezier
			if len(constInfo.CAMERA_WAYPOINTS) < 1:
				return

			cameraTween = None
			if self.cinemachineWindow.cameraInterpolationMode == self.cinemachineWindow.MODE_BEZIER:
				num_points = 20

				isBezierRotation = False
				constInfo.CAMERA_BEZIER_POINTS = {}
				self.CreateBezierPath("x", num_points)
				self.CreateBezierPath("y", num_points)
				self.CreateBezierPath("z", num_points)
				self.CreateBezierPath("distance", num_points)
				self.CreateBezierPath("pitch", num_points)
				if self.cinemachineWindow.rotationInterpolationMode == self.cinemachineWindow.ROTATION_MODE_BEZIER:
					self.CreateBezierPath("rotation", num_points)
					isBezierRotation = True
				self.CreateBezierPath("fov", num_points)

				cameraTween = tweenCamera.CameraBezierTween(self.cinemachineWindow.TWEEN_DURATION, num_points, isBezierRotation)
			else:
				cameraTween = tweenCamera.CameraLerpTween(self.cinemachineWindow.TWEEN_DURATION)

			self.tweenMgr.CreateTween(cameraTween)
			cameraTween.StartTween()

		elif num == 4:
			self.cinemachineWindow.Toggle()

	def __BuildKeyDict(self):
		onPressKeyDict = {}


		onPressKeyDict[app.DIK_1]	= lambda : self.__PressNumKey(1)
		onPressKeyDict[app.DIK_2]	= lambda : self.__PressNumKey(2)
		onPressKeyDict[app.DIK_3]	= lambda : self.__PressNumKey(3)
		onPressKeyDict[app.DIK_4]	= lambda : self.__PressNumKey(4)
		onPressKeyDict[app.DIK_5]	= lambda : self.__PressNumKey(5)
		onPressKeyDict[app.DIK_6]	= lambda : self.__PressNumKey(6)
		onPressKeyDict[app.DIK_7]	= lambda : self.__PressNumKey(7)
		onPressKeyDict[app.DIK_8]	= lambda : self.__PressNumKey(8)
		onPressKeyDict[app.DIK_9]	= lambda : self.__PressNumKey(9)
		onPressKeyDict[app.DIK_F1]	= lambda : self.__PressQuickSlot(4)
		onPressKeyDict[app.DIK_F2]	= lambda : self.__PressQuickSlot(5)
		onPressKeyDict[app.DIK_F3]	= lambda : self.__PressQuickSlot(6)
		onPressKeyDict[app.DIK_F4]	= lambda : self.__PressQuickSlot(7)
		onPressKeyDict[app.DIK_F5]     = lambda: self.ToggleCameraMode()           # F5 (no key-change system)
		onPressKeyDict[app.DIK_END] = lambda: self.ToggleCameraMode()           # Insert (key-change system)
		#onPressKeyDict[app.DIK_PGUP]   = lambda: self.__AddFreeCameraSpeed()       # PageUp
		#onPressKeyDict[app.DIK_PGDN]   = lambda: self.__SubtractFreeCameraSpeed()  # PageDown
		onPressKeyDict[app.DIK_PGUP]   = lambda: self.__AddCameraFov()             # Home
		onPressKeyDict[app.DIK_PGDN]    = lambda: self.__SubtractCameraFov()        # End

		onPressKeyDict[app.DIK_RETURN]	= lambda : self.ChangeBonus()

		onPressKeyDict[app.DIK_F11]	= lambda : self.interface.ToggleCollectWindow()

		onPressKeyDict[app.DIK_F12] = lambda : self.interface.ToggleCollectionWindow()

		onPressKeyDict[app.DIK_P]	= lambda : self.interface.BuffNPCOpenWindow()
		if app.WJ_SPLIT_INVENTORY_SYSTEM:
			onPressKeyDict[app.DIK_U]		= lambda : self.__PressExtendedInventory()
			

		onPressKeyDict[app.DIK_TAB]			= lambda : 		self.ToggleMapWindow()

		onPressKeyDict[app.DIK_LALT]		= lambda : self.ShowName()
		onPressKeyDict[app.DIK_LCONTROL]	= lambda : self.ShowMouseImage()
		onPressKeyDict[app.DIK_SPACE]		= lambda : self.StartAttack()

		onPressKeyDict[app.DIK_UP]			= lambda : self.MoveUp()
		onPressKeyDict[app.DIK_DOWN]		= lambda : self.MoveDown()
		onPressKeyDict[app.DIK_LEFT]		= lambda : self.MoveLeft()
		onPressKeyDict[app.DIK_RIGHT]		= lambda : self.MoveRight()
		onPressKeyDict[app.DIK_W]			= lambda : self.MoveUp()
		onPressKeyDict[app.DIK_S]			= lambda : self.MoveDown()
		onPressKeyDict[app.DIK_A]			= lambda : self.MoveLeft()
		onPressKeyDict[app.DIK_D]			= lambda : self.MoveRight()

		onPressKeyDict[app.DIK_E]			= lambda: app.RotateCamera(app.CAMERA_TO_POSITIVE)
		onPressKeyDict[app.DIK_R]			= lambda: app.ZoomCamera(app.CAMERA_TO_NEGATIVE)
		onPressKeyDict[app.DIK_T]			= lambda: app.PitchCamera(app.CAMERA_TO_NEGATIVE)
		onPressKeyDict[app.DIK_G]			= self.__PressGKey
		onPressKeyDict[app.DIK_Q]			= self.__PressQKey

		onPressKeyDict[app.DIK_NUMPAD9]		= lambda: app.MovieResetCamera()
		onPressKeyDict[app.DIK_NUMPAD4]		= lambda: app.MovieRotateCamera(app.CAMERA_TO_NEGATIVE)
		onPressKeyDict[app.DIK_NUMPAD6]		= lambda: app.MovieRotateCamera(app.CAMERA_TO_POSITIVE)
		onPressKeyDict[app.DIK_PGUP]		= lambda: app.MovieZoomCamera(app.CAMERA_TO_NEGATIVE)
		onPressKeyDict[app.DIK_PGDN]		= lambda: app.MovieZoomCamera(app.CAMERA_TO_POSITIVE)
		onPressKeyDict[app.DIK_NUMPAD8]		= lambda: app.MoviePitchCamera(app.CAMERA_TO_NEGATIVE)
		onPressKeyDict[app.DIK_NUMPAD2]		= lambda: app.MoviePitchCamera(app.CAMERA_TO_POSITIVE)
		onPressKeyDict[app.DIK_GRAVE]		= lambda : self.PickUpItem()
		onPressKeyDict[app.DIK_Z]			= lambda : self.PickUpItem()
		onPressKeyDict[app.DIK_C]			= lambda state = "STATUS": self.interface.ToggleCharacterWindow(state)
		onPressKeyDict[app.DIK_V]			= lambda state = "SKILL": self.interface.ToggleCharacterWindow(state)
		onPressKeyDict[app.DIK_N]			= lambda state = "QUEST": self.interface.ToggleCharacterWindow(state)
		onPressKeyDict[app.DIK_I]			= lambda : self.interface.ToggleInventoryWindow()
		onPressKeyDict[app.DIK_O]			= lambda : self.interface.ToggleDragonSoulWindow()
		#if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
		#	onPressKeyDict[app.DIK_U]			= lambda : self.interface.TogglePrivateShopPanelWindowCheck()
		onPressKeyDict[app.DIK_M]			= lambda : self.interface.PressMKey()
		onPressKeyDict[app.DIK_ADD]			= lambda : self.interface.MiniMapScaleUp()
		onPressKeyDict[app.DIK_SUBTRACT]	= lambda : self.interface.MiniMapScaleDown()
		onPressKeyDict[app.DIK_L]			= lambda : self.interface.ToggleChatLogWindow()
		onPressKeyDict[app.DIK_COMMA]		= lambda : self.ShowConsole()
		onPressKeyDict[app.DIK_LSHIFT]		= lambda : self.__SetQuickPageMode()
		onPressKeyDict[app.DIK_J]			= lambda : self.__PressJKey()
		onPressKeyDict[app.DIK_H]			= lambda : self.__PressHKey()
		onPressKeyDict[app.DIK_B]			= lambda : self.__PressBKey()
		onPressKeyDict[app.DIK_F]			= lambda : self.__PressFKey()


		self.onPressKeyDict = onPressKeyDict

		onClickKeyDict = {}
		onClickKeyDict[app.DIK_UP] = lambda : self.StopUp()
		onClickKeyDict[app.DIK_DOWN] = lambda : self.StopDown()
		onClickKeyDict[app.DIK_LEFT] = lambda : self.StopLeft()
		onClickKeyDict[app.DIK_RIGHT] = lambda : self.StopRight()
		onClickKeyDict[app.DIK_SPACE] = lambda : self.EndAttack()

		onClickKeyDict[app.DIK_W] = lambda : self.StopUp()
		onClickKeyDict[app.DIK_S] = lambda : self.StopDown()
		onClickKeyDict[app.DIK_A] = lambda : self.StopLeft()
		onClickKeyDict[app.DIK_D] = lambda : self.StopRight()
		onClickKeyDict[app.DIK_Q] = lambda: app.RotateCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_E] = lambda: app.RotateCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_R] = lambda: app.ZoomCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_F] = lambda: app.ZoomCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_T] = lambda: app.PitchCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_G] = lambda: self.__ReleaseGKey()
		onClickKeyDict[app.DIK_NUMPAD4] = lambda: app.MovieRotateCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_NUMPAD6] = lambda: app.MovieRotateCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_PGUP] = lambda: app.MovieZoomCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_PGDN] = lambda: app.MovieZoomCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_NUMPAD8] = lambda: app.MoviePitchCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_NUMPAD2] = lambda: app.MoviePitchCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_LALT] = lambda: self.HideName()
		onClickKeyDict[app.DIK_LCONTROL] = lambda: self.HideMouseImage()
		onClickKeyDict[app.DIK_LSHIFT] = lambda: self.__SetQuickSlotMode()


		self.onClickKeyDict=onClickKeyDict

	def ChangeBonus(self):
		if constInfo.IS_BONUS_CHANGER:
			self.interface.wndChangerWindow.ChangeBonus()
		elif constInfo.IS_ACCE_WINDOW:
			acce.SendRefineRequest()
		elif constInfo.IS_DRAGON_SOUL_OPEN:
			self.interface.wndDragonSoulRefine.PressDoRefineButton()


	def __PressNumKey(self,num):
		if self.isCinemaMode:
			self.TestFreeCamera(num)
			return
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):

			if num >= 1 and num <= 9:
				if(chrmgr.IsPossibleEmoticon(-1)):
					chrmgr.SetEmoticon(-1,int(num)-1)
					net.SendEmoticon(int(num)-1)
		else:
			if num >= 1 and num <= 4:
				self.pressNumber(num-1)

	def __ClickBKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			return
		else:
			if constInfo.PVPMODE_ACCELKEY_ENABLE:
				self.ChangePKMode()
					
	def	__PressJKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			if player.IsMountingHorse():
				net.SendChatPacket("/unmount")
			else:
				if not uiPrivateShopBuilder.IsBuildingPrivateShop():
					for i in xrange(player.INVENTORY_PAGE_SIZE*player.INVENTORY_PAGE_COUNT):
						if player.GetItemIndex(i) in (71114, 71116, 71118, 71120):
							net.SendItemUsePacket(i)
							break
	def	__PressHKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			net.SendChatPacket("/user_horse_ride")
		else:
			if app.ENABLE_WIKI:
				self.interface.OpenWikiWindow()

	def	__PressBKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			net.SendChatPacket("/user_horse_back")
		else:
			state = "EMOTICON"
			self.interface.ToggleCharacterWindow(state)

	def	__PressFKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			net.SendChatPacket("/user_horse_feed")
		else:
			app.ZoomCamera(app.CAMERA_TO_POSITIVE)

	def __PressGKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			net.SendChatPacket("/user_horse_ride")
		else:
			if self.ShowNameFlag:
				self.interface.ToggleGuildWindow()
			else:
				app.PitchCamera(app.CAMERA_TO_POSITIVE)

	def	__ReleaseGKey(self):
		app.PitchCamera(app.CAMERA_STOP)

	def __PressQKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			if 0==interfaceModule.IsQBHide:
				interfaceModule.IsQBHide = 1
				self.interface.HideAllQuestButton()
			else:
				interfaceModule.IsQBHide = 0
				self.interface.ShowAllQuestButton()
		else:
			app.RotateCamera(app.CAMERA_TO_NEGATIVE)

	def __SetQuickSlotMode(self):
		self.pressNumber=ui.__mem_func__(self.__PressQuickSlot)

	def __SetQuickPageMode(self):
		self.pressNumber=ui.__mem_func__(self.__SelectQuickPage)

	def __PressQuickSlot(self, localSlotIndex):
		if localeInfo.IsARABIC():
			if 0 <= localSlotIndex and localSlotIndex < 4:
				player.RequestUseLocalQuickSlot(3-localSlotIndex)
			else:
				player.RequestUseLocalQuickSlot(11-localSlotIndex)
		else:
			player.RequestUseLocalQuickSlot(localSlotIndex)

	if app.ENABLE_KEYCHANGE_SYSTEM:
		def OpenKeyChangeWindow(self):
			self.wndKeyChange.Open()

		def OpenWindow(self, type, state):
			if type == player.KEY_OPEN_STATE:
				self.interface.ToggleCharacterWindow(state)
			elif type == player.KEY_OPEN_INVENTORY:
				self.interface.ToggleInventoryWindow()
			elif type == player.KEY_OPEN_DDS:
				self.interface.ToggleDragonSoulWindow()
			elif type == player.KEY_OPEN_MINIMAP:
				self.interface.ToggleMiniMap()
			elif type == player.KEY_OPEN_LOGCHAT:
				self.interface.ToggleChatLogWindow()
			elif type == player.KEY_OPEN_GUILD:
				self.interface.ToggleGuildWindow()
			elif type == player.KEY_OPEN_MESSENGER:
				self.interface.ToggleMessenger()
			elif type == player.KEY_OPEN_HELP:
				#if app.ENABLE_WIKI:
				self.interface.OpenWikiWindow()
			elif type == player.KEY_TP_MAP:
				self.interface.OpenDungeonInfo()

			elif type == player.KEY_OPEN_EXTENDED_INV:
				self.interface.ToggleExtendedInventoryWindow()
			elif type == player.KEY_OPEN_PET:
				self.interface.BuffNPCOpenWindow()
			elif type == player.KEY_OPEN_DOPY:
				self.interface.wndBlend.btnUse()
			elif type == player.KEY_OPEN_MARBLE:
				self.interface.wndCompanionWindow.Open()
			elif type == player.KEY_OPEN_SAVE_LOCATION:
				self.interface.OpenTitleAchievement()
			elif type == player.KEY_OPEN_RESP:
				self.interface.OpenEventCalendar()
				#if app.ENABLE_RESP_SYSTEM:
				#	self.interface.OpenRespWindow()
			elif type == player.KEY_OPEN_BUFF:
				self.OpenShopSearch()
			elif type == player.KEY_OPEN_MISSION:
				self.interface.ToggleCollectWindow()
			elif type == player.KEY_OPEN_COLLECTION:
				self.interface.ToggleCollectionWindow()
			elif type == player.KEY_RETURN:
				self.ChangeBonus()
			elif type == player.KEY_OPEN_DUNGEONS:
				self.interface.OpenDungeonInfo()

			elif type == player.KEY_CHANGECHANNEL1:
				net.MoveChannelGame(1)
			elif type == player.KEY_CHANGECHANNEL2:
				net.MoveChannelGame(2)
			elif type == player.KEY_CHANGECHANNEL3:
				net.MoveChannelGame(3)
			elif type == player.KEY_CHANGECHANNEL4:
				net.MoveChannelGame(4)
			elif type == player.KEY_CHANGECHANNEL5:
				net.MoveChannelGame(5)
			elif type == player.KEY_CHANGECHANNEL6:
				net.MoveChannelGame(6)
			elif type == player.KEY_OPEN_CHANNEL_DIALOG:
				if app.BL_MOVE_CHANNEL:
					self.interface.OpenMoveChannelDialog()
			elif type == player.KEY_CHANNEL_UP:
				if app.BL_MOVE_CHANNEL:
					self.interface.ChannelUp()
			elif type == player.KEY_CHANNEL_DOWN:
				if app.BL_MOVE_CHANNEL:
					self.interface.ChannelDown()

		def ScrollOnOff(self):
			if 0 == interfaceModule.IsQBHide:
				interfaceModule.IsQBHide = 1
				self.interface.HideAllQuestButton()
			else:
				interfaceModule.IsQBHide = 0
				self.interface.ShowAllQuestButton()

	def __SelectQuickPage(self, pageIndex):
		self.quickSlotPageIndex = pageIndex
		player.SetQuickPage(pageIndex)

	def ToggleDebugInfo(self):
		self.isShowDebugInfo = not self.isShowDebugInfo

		if self.isShowDebugInfo:
			self.PrintCoord.Show()
			self.FrameRate.Show()
			self.Pitch.Show()
			self.Splat.Show()
			self.TextureNum.Show()
			self.ObjectNum.Show()
			self.ViewDistance.Show()
			self.PrintMousePos.Show()
		else:
			self.PrintCoord.Hide()
			self.FrameRate.Hide()
			self.Pitch.Hide()
			self.Splat.Hide()
			self.TextureNum.Hide()
			self.ObjectNum.Hide()
			self.ViewDistance.Hide()
			self.PrintMousePos.Hide()

	def __BuildDebugInfo(self):
		self.PrintCoord = ui.TextLine()
		self.PrintCoord.SetFontName(localeInfo.UI_DEF_FONT)
		self.PrintCoord.SetPosition(wndMgr.GetScreenWidth() - 270, 0)

		self.FrameRate = ui.TextLine()
		self.FrameRate.SetFontName(localeInfo.UI_DEF_FONT)
		self.FrameRate.SetPosition(wndMgr.GetScreenWidth() - 270, 20)

		self.Pitch = ui.TextLine()
		self.Pitch.SetFontName(localeInfo.UI_DEF_FONT)
		self.Pitch.SetPosition(wndMgr.GetScreenWidth() - 270, 40)

		self.Splat = ui.TextLine()
		self.Splat.SetFontName(localeInfo.UI_DEF_FONT)
		self.Splat.SetPosition(wndMgr.GetScreenWidth() - 270, 60)

		self.PrintMousePos = ui.TextLine()
		self.PrintMousePos.SetFontName(localeInfo.UI_DEF_FONT)
		self.PrintMousePos.SetPosition(wndMgr.GetScreenWidth() - 270, 80)

		self.TextureNum = ui.TextLine()
		self.TextureNum.SetFontName(localeInfo.UI_DEF_FONT)
		self.TextureNum.SetPosition(wndMgr.GetScreenWidth() - 270, 100)

		self.ObjectNum = ui.TextLine()
		self.ObjectNum.SetFontName(localeInfo.UI_DEF_FONT)
		self.ObjectNum.SetPosition(wndMgr.GetScreenWidth() - 270, 120)

		self.ViewDistance = ui.TextLine()
		self.ViewDistance.SetFontName(localeInfo.UI_DEF_FONT)
		self.ViewDistance.SetPosition(0, 0)

	def __NotifyError(self, msg):
		chat.AppendChat(chat.CHAT_TYPE_INFO, msg)

	def ChangePKMode(self):

		if not app.IsPressed(app.DIK_LCONTROL):
			return

		if player.GetStatus(player.LEVEL)<constInfo.PVPMODE_PROTECTED_LEVEL:
			self.__NotifyError(localeInfo.OPTION_PVPMODE_PROTECT % (constInfo.PVPMODE_PROTECTED_LEVEL))
			return

		curTime = app.GetTime()
		if curTime - self.lastPKModeSendedTime < constInfo.PVPMODE_ACCELKEY_DELAY:
			return

		self.lastPKModeSendedTime = curTime

		curPKMode = player.GetPKMode()
		nextPKMode = curPKMode + 1
		if nextPKMode == player.PK_MODE_PROTECT:
			if 0 == player.GetGuildID():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_CANNOT_SET_GUILD_MODE)
				nextPKMode = 0
			else:
				nextPKMode = player.PK_MODE_GUILD

		elif nextPKMode == player.PK_MODE_MAX_NUM:
			nextPKMode = 0

		net.SendChatPacket("/PKMode " + str(nextPKMode))
		print "/PKMode " + str(nextPKMode)

	def OnChangePKMode(self):

		self.interface.OnChangePKMode()

		try:
			self.__NotifyError(localeInfo.OPTION_PVPMODE_MESSAGE_DICT[player.GetPKMode()])
		except KeyError:
			print "UNKNOWN PVPMode[%d]" % (player.GetPKMode())

		if constInfo.PVPMODE_TEST_ENABLE:
			curPKMode = player.GetPKMode()
			alignment, grade = chr.testGetPKData()
			self.pkModeNameDict = { 0 : "PEACE", 1 : "REVENGE", 2 : "FREE", 3 : "PROTECT", }
			self.testPKMode.SetText("Current PK Mode : " + self.pkModeNameDict.get(curPKMode, "UNKNOWN"))
			self.testAlignment.SetText("Current Alignment : " + str(alignment) + " (" + localeInfo.TITLE_NAME_LIST[grade] + ")")


	def StartGame(self):
		self.RefreshInventory()
		self.RefreshEquipment()
		self.RefreshCharacter()
		self.RefreshSkill()
		if constInfo.CURTAIN_ACTIVE == 1:
			constInfo.CURTAIN_ACTIVE = 0
		net.SendChatPacket("/language " + app.GetLocaleName())

	def CheckGameButton(self):
		if self.interface:
			self.interface.CheckGameButton()

	def RefreshAlignment(self):
		self.interface.RefreshAlignment()

	def RefreshStatus(self):
		if self.interface:
			self.interface.RefreshStatus()

		if self.playerGauge:
			self.playerGauge.RefreshGauge()

		self.CheckGameButton()

	def RefreshStamina(self):
		self.interface.RefreshStamina()

	def RefreshSkill(self):
		self.CheckGameButton()
		if self.interface:
			self.interface.RefreshSkill()

	def RefreshQuest(self):
		self.interface.RefreshQuest()

	def RefreshMessenger(self):
		self.interface.RefreshMessenger()

	def RefreshGuildInfoPage(self):
		self.interface.RefreshGuildInfoPage()

	def RefreshGuildDungeonInfoPage(self):
		self.interface.RefreshGuildDungeonInfoPage()

	def RefreshGuildDungeonBossDmgRankWindow(self):
		self.interface.RefreshGuildDungeonBossDmgRankWindow()

	def RefreshGuildBoardPage(self):
		self.interface.RefreshGuildBoardPage()

	def RefreshGuildMemberPage(self):
		self.interface.RefreshGuildMemberPage()

	def RefreshGuildMemberPageGradeComboBox(self):
		self.interface.RefreshGuildMemberPageGradeComboBox()

	def RefreshGuildSkillPage(self):
		self.interface.RefreshGuildSkillPage()

	def RefreshGuildGradePage(self):
		self.interface.RefreshGuildGradePage()

	def RefreshMobile(self):
		if self.interface:
			self.interface.RefreshMobile()

	def OnMobileAuthority(self):
		self.interface.OnMobileAuthority()

	def OnBlockMode(self, mode):
		self.interface.OnBlockMode(mode)

	def OpenQuestWindow(self, skin, idx):
		if constInfo.INPUT_IGNORE == 1:
			return
		else:
			self.interface.OpenQuestWindow(skin, idx)

	def AskGuildName(self):

		guildNameBoard = uiCommon.InputDialog()
		guildNameBoard.SetTitle(localeInfo.GUILD_NAME)
		guildNameBoard.SetAcceptEvent(ui.__mem_func__(self.ConfirmGuildName))
		guildNameBoard.SetCancelEvent(ui.__mem_func__(self.CancelGuildName))
		guildNameBoard.Open()

		self.guildNameBoard = guildNameBoard

	def ConfirmGuildName(self):
		guildName = self.guildNameBoard.GetText()
		if not guildName:
			return

		if net.IsInsultIn(guildName):
			self.PopupMessage(localeInfo.GUILD_CREATE_ERROR_INSULT_NAME)
			return

		net.SendAnswerMakeGuildPacket(guildName)
		self.guildNameBoard.Close()
		self.guildNameBoard = None
		return True

	def CancelGuildName(self):
		self.guildNameBoard.Close()
		self.guildNameBoard = None
		return True

	def PopupMessage(self, msg):
		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(msg, 0, localeInfo.UI_OK)

	def OpenRefineDialog(self, targetItemPos, nextGradeItemVnum, cost, cost2, cost3, prob, type=0, extraProb=0):
		self.interface.OpenRefineDialog(targetItemPos, nextGradeItemVnum, cost, cost2, cost3, prob, type, extraProb)

	def AppendMaterialToRefineDialog(self, vnum, count):
		self.interface.AppendMaterialToRefineDialog(vnum, count)

	def RunUseSkillEvent(self, slotIndex, coolTime):
		self.interface.OnUseSkill(slotIndex, coolTime)

	def ClearAffects(self):
		self.affectShower.ClearAffects()

	def SetAffect(self, affect):
		self.affectShower.SetAffect(affect)

	def ResetAffect(self, affect):
		self.affectShower.ResetAffect(affect)

	def BINARY_NEW_AddAffect(self, type, pointIdx, value, duration):
		self.affectShower.BINARY_NEW_AddAffect(type, pointIdx, value, duration)
		if chr.NEW_AFFECT_DRAGON_SOUL_DECK1 == type or chr.NEW_AFFECT_DRAGON_SOUL_DECK2 == type:
			self.interface.DragonSoulActivate(type - chr.NEW_AFFECT_DRAGON_SOUL_DECK1)
		elif chr.NEW_AFFECT_DRAGON_SOUL_QUALIFIED == type:
			self.BINARY_DragonSoulGiveQuilification()
		if app.ENABLE_VIP_SYSTEM and chr.NEW_AFFECT_VIP == type:
			import constInfo
			constInfo.IS_VIP = True

	def BINARY_NEW_RemoveAffect(self, type, pointIdx):
		self.affectShower.BINARY_NEW_RemoveAffect(type, pointIdx)
		if chr.NEW_AFFECT_DRAGON_SOUL_DECK1 == type or chr.NEW_AFFECT_DRAGON_SOUL_DECK2 == type:
			self.interface.DragonSoulDeactivate()
		if app.ENABLE_VIP_SYSTEM and chr.NEW_AFFECT_VIP == type:
			import constInfo
			constInfo.IS_VIP = False

	if app.ENABLE_AFFECT_FIX:
		def RefreshAffectWindow(self):
			self.affectShower.BINARY_NEW_RefreshAffect()



	def ActivateSkillSlot(self, slotIndex):
		if self.interface:
			self.interface.OnActivateSkill(slotIndex)

	def DeactivateSkillSlot(self, slotIndex):
		if self.interface:
			self.interface.OnDeactivateSkill(slotIndex)

	def RefreshEquipment(self):
		if self.interface:
			self.interface.RefreshInventory()

	def RefreshInventory(self):
		if self.interface:
			self.interface.RefreshInventory()

		if self.affectShower:
			self.affectShower.RefreshInventory()

	def RefreshCharacter(self):
		if self.interface:
			self.interface.RefreshCharacter()

	if app.RENEWAL_DEAD_PACKET:
		def OnGameOver(self, d_time):
			self.CloseTargetBoard()
			self.OpenRestartDialog(d_time)
	else:
		def OnGameOver(self):
			self.CloseTargetBoard()
			self.OpenRestartDialog()

	if app.RENEWAL_DEAD_PACKET:
		def OpenRestartDialog(self, d_time):
			self.interface.OpenRestartDialog(d_time)
	else:
		def OpenRestartDialog(self):
			self.interface.OpenRestartDialog()

	def ChangeCurrentSkill(self, skillSlotNumber):
		self.interface.OnChangeCurrentSkill(skillSlotNumber)

	def SetPCTargetBoard(self, vid, name):
		self.targetBoard.Open(vid, name)

		if app.IsPressed(app.DIK_LCONTROL):

			if not player.IsSameEmpire(vid):
				return

			if player.IsMainCharacterIndex(vid):
				return
			elif chr.INSTANCE_TYPE_BUILDING == chr.GetInstanceType(vid):
				return

			self.interface.OpenWhisperDialog(name)


	def RefreshTargetBoardByVID(self, vid):
		self.targetBoard.RefreshByVID(vid)

	def RefreshTargetBoardByName(self, name):
		self.targetBoard.RefreshByName(name)

	def __RefreshTargetBoard(self):
		self.targetBoard.Refresh()

	if app.ENABLE_VIEW_TARGET_DECIMAL_HP:
		def SetHPTargetBoard(self, vid, iMinHP, iMaxHP):
			if vid != self.targetBoard.GetTargetVID():
				self.targetBoard.ResetTargetBoard()
				self.targetBoard.SetEnemyVID(vid)
			
			self.targetBoard.SetHP(iMinHP, iMaxHP)
			self.targetBoard.Show()

	def CloseTargetBoardIfDifferent(self, vid):
		if vid != self.targetBoard.GetTargetVID():
			self.targetBoard.Close()

	def CloseTargetBoard(self):
		self.targetBoard.Close()

	def OpenEquipmentDialog(self, vid):
		self.interface.OpenEquipmentDialog(vid)

	def SetEquipmentDialogItem(self, vid, slotIndex, vnum, count):
		self.interface.SetEquipmentDialogItem(vid, slotIndex, vnum, count)

	def SetEquipmentDialogSocket(self, vid, slotIndex, socketIndex, value):
		self.interface.SetEquipmentDialogSocket(vid, slotIndex, socketIndex, value)

	def SetEquipmentDialogAttr(self, vid, slotIndex, attrIndex, type, value):
		self.interface.SetEquipmentDialogAttr(vid, slotIndex, attrIndex, type, value)

	def ShowMapName(self, mapName, x, y):

		if self.mapNameShower:
			self.mapNameShower.ShowMapName(mapName, x, y)

		if self.interface:
			self.interface.SetMapName(mapName)
    
	def BINARY_OpenAtlasWindow(self):
		self.interface.BINARY_OpenAtlasWindow()

	def BINARY_RefreshBossAtlasData(self):
		player.LoadBossNPC()
		if self.interface and hasattr(self.interface, 'wndResp') and self.interface.wndResp:
			wndResp = self.interface.wndResp
			if hasattr(wndResp, 'listBox') and wndResp.listBox:
				for item in wndResp.listBox.items:
					if hasattr(item, 'vnum'):
						net.SendRespFetchRespPacket(item.vnum)
						
	def OnRecvWhisper(self, mode, name, line):
		if mode == chat.WHISPER_TYPE_GM:
			self.interface.RegisterGameMasterName(name)

		append_text = str(time.strftime("[%d.%m %H:%M]"))+line
		chat.AppendWhisper(mode, name, append_text)
#ifdef WHISPER_HISTORY
		uiScriptLocale.SaveWhisper(player.GetName(), name, append_text)
#endif
		self.interface.RecvWhisper(name)

	def OnRecvWhisperSystemMessage(self, mode, name, line):
		append_text = str(time.strftime("[%d.%m %H:%M]"))+line
		chat.AppendWhisper(mode, name, append_text)
#ifdef WHISPER_HISTORY
		uiScriptLocale.SaveWhisper(player.GetName(), name, append_text)
#endif
		self.interface.RecvWhisper(name)

	def OnRecvWhisperError(self, mode, name, line):
		if localeInfo.WHISPER_ERROR.has_key(mode):
			chat.AppendWhisper(chat.WHISPER_TYPE_SYSTEM, name, localeInfo.WHISPER_ERROR[mode](name))
		else:
			chat.AppendWhisper(chat.WHISPER_TYPE_SYSTEM, name, "Whisper Unknown Error(mode=%d, name=%s)" % (mode, name))
		self.interface.RecvWhisper(name)

	def RecvWhisper(self, name):
		self.interface.RecvWhisper(name)

	def OnPickMoney(self, money):
		self.interface.OnPickMoneyNew(money)

	if app.ENABLE_CHEQUE_SYSTEM:
		def OnPickCheque(self, cheque):
			self.interface.OnPickChequeNew(cheque)

	def OnShopError(self, type):
		try:
			self.PopupMessage(localeInfo.SHOP_ERROR_DICT[type])
		except KeyError:
			self.PopupMessage(localeInfo.SHOP_ERROR_UNKNOWN % (type))

	def OnSafeBoxError(self):
		self.PopupMessage(localeInfo.SAFEBOX_ERROR)

	def OnFishingSuccess(self, isFish, fishName):
		chat.AppendChatWithDelay(chat.CHAT_TYPE_INFO, localeInfo.FISHING_SUCCESS(isFish, fishName), 2000)

	def OnFishingNotifyUnknown(self):
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.FISHING_UNKNOWN)

	def OnFishingWrongPlace(self):
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.FISHING_WRONG_PLACE)

	def OnFishingNotify(self, isFish, fishName):
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.FISHING_NOTIFY(isFish, fishName))

	def OnFishingFailure(self):
		chat.AppendChatWithDelay(chat.CHAT_TYPE_INFO, localeInfo.FISHING_FAILURE, 2000)

	def OnCannotPickItem(self):
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GAME_CANNOT_PICK_ITEM)

	def OnCannotMining(self):
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GAME_CANNOT_MINING)

	def OnCannotUseSkill(self, vid, type):
		if localeInfo.USE_SKILL_ERROR_TAIL_DICT.has_key(type):
			textTail.RegisterInfoTail(vid, localeInfo.USE_SKILL_ERROR_TAIL_DICT[type])

		if localeInfo.USE_SKILL_ERROR_CHAT_DICT.has_key(type):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_SKILL_ERROR_CHAT_DICT[type])

	def	OnCannotShotError(self, vid, type):
		textTail.RegisterInfoTail(vid, localeInfo.SHOT_ERROR_TAIL_DICT.get(type, localeInfo.SHOT_ERROR_UNKNOWN % (type)))

	def StartPointReset(self):
		self.interface.OpenPointResetDialog()

	def StartShop(self, vid):
		self.interface.OpenShopDialog(vid)

	def EndShop(self):
		self.interface.CloseShopDialog()

	def RefreshShop(self):
		self.interface.RefreshShopDialog()

	def SetShopSellingPrice(self, Price):
		pass

	if app.ENABLE_OFFLINE_SHOP_SYSTEM:
		def StartOfflineShop(self, vid):
			self.interface.OpenOfflineShopDialog(vid)
			
		def EndOfflineShop(self):
			self.interface.CloseOfflineShopDialog()
			
		def RefreshOfflineShop(self):
			self.interface.RefreshOfflineShopDialog()
        
	def StartExchange(self):
		self.interface.StartExchange()

	def EndExchange(self):
		self.interface.EndExchange()

	def RefreshExchange(self):
		self.interface.RefreshExchange()

	def RecvPartyInviteQuestion(self, leaderVID, leaderName):
		partyInviteQuestionDialog = uiCommon.QuestionDialog()
		partyInviteQuestionDialog.SetText(leaderName + localeInfo.PARTY_DO_YOU_JOIN)
		partyInviteQuestionDialog.SetAcceptEvent(lambda arg=True: self.AnswerPartyInvite(arg))
		partyInviteQuestionDialog.SetCancelEvent(lambda arg=False: self.AnswerPartyInvite(arg))
		partyInviteQuestionDialog.Open()
		partyInviteQuestionDialog.partyLeaderVID = leaderVID
		self.partyInviteQuestionDialog = partyInviteQuestionDialog

	def AnswerPartyInvite(self, answer):

		if not self.partyInviteQuestionDialog:
			return

		partyLeaderVID = self.partyInviteQuestionDialog.partyLeaderVID

		distance = player.GetCharacterDistance(partyLeaderVID)
		if distance < 0.0 or distance > 5000:
			answer = False

		net.SendPartyInviteAnswerPacket(partyLeaderVID, answer)

		self.partyInviteQuestionDialog.Close()
		self.partyInviteQuestionDialog = None

	def AddPartyMember(self, pid, name):
		self.interface.AddPartyMember(pid, name)

	def UpdatePartyMemberInfo(self, pid):
		self.interface.UpdatePartyMemberInfo(pid)

	def RemovePartyMember(self, pid):
		self.interface.RemovePartyMember(pid)
		self.__RefreshTargetBoard()

	def LinkPartyMember(self, pid, vid):
		self.interface.LinkPartyMember(pid, vid)

	def UnlinkPartyMember(self, pid):
		self.interface.UnlinkPartyMember(pid)

	def UnlinkAllPartyMember(self):
		self.interface.UnlinkAllPartyMember()

	def ExitParty(self):
		self.interface.ExitParty()
		self.RefreshTargetBoardByVID(self.targetBoard.GetTargetVID())

	def ChangePartyParameter(self, distributionMode):
		self.interface.ChangePartyParameter(distributionMode)

	def OnMessengerAddFriendQuestion(self, name):
		messengerAddFriendQuestion = uiCommon.QuestionDialog2()
		messengerAddFriendQuestion.SetText1(localeInfo.MESSENGER_DO_YOU_ACCEPT_ADD_FRIEND_1 % (name))
		messengerAddFriendQuestion.SetText2(localeInfo.MESSENGER_DO_YOU_ACCEPT_ADD_FRIEND_2)
		messengerAddFriendQuestion.SetAcceptEvent(ui.__mem_func__(self.OnAcceptAddFriend))
		messengerAddFriendQuestion.SetCancelEvent(ui.__mem_func__(self.OnDenyAddFriend))
		messengerAddFriendQuestion.Open()
		messengerAddFriendQuestion.name = name
		self.messengerAddFriendQuestion = messengerAddFriendQuestion

	def OnAcceptAddFriend(self):
		name = self.messengerAddFriendQuestion.name
		net.SendChatPacket("/messenger_auth y " + name)
		self.OnCloseAddFriendQuestionDialog()
		return True

	def OnDenyAddFriend(self):
		name = self.messengerAddFriendQuestion.name
		net.SendChatPacket("/messenger_auth n " + name)
		self.OnCloseAddFriendQuestionDialog()
		return True

	def OnCloseAddFriendQuestionDialog(self):
		self.messengerAddFriendQuestion.Close()
		self.messengerAddFriendQuestion = None
		return True

	def OpenSafeboxWindow(self, size):
		self.interface.OpenSafeboxWindow(size)

	def RefreshSafebox(self):
		self.interface.RefreshSafebox()

	def RefreshSafeboxMoney(self):
		self.interface.RefreshSafeboxMoney()

	def OpenMallWindow(self, size):
		self.interface.OpenMallWindow(size)

	def RefreshMall(self):
		self.interface.RefreshMall()

	def RecvGuildInviteQuestion(self, guildID, guildName):
		guildInviteQuestionDialog = uiCommon.QuestionDialog()
		guildInviteQuestionDialog.SetText(guildName + localeInfo.GUILD_DO_YOU_JOIN)
		guildInviteQuestionDialog.SetAcceptEvent(lambda arg=True: self.AnswerGuildInvite(arg))
		guildInviteQuestionDialog.SetCancelEvent(lambda arg=False: self.AnswerGuildInvite(arg))
		guildInviteQuestionDialog.Open()
		guildInviteQuestionDialog.guildID = guildID
		self.guildInviteQuestionDialog = guildInviteQuestionDialog

	def AnswerGuildInvite(self, answer):

		if not self.guildInviteQuestionDialog:
			return

		guildLeaderVID = self.guildInviteQuestionDialog.guildID
		net.SendGuildInviteAnswerPacket(guildLeaderVID, answer)

		self.guildInviteQuestionDialog.Close()
		self.guildInviteQuestionDialog = None


	def DeleteGuild(self):
		self.interface.DeleteGuild()

	def ShowClock(self, second):
		self.interface.ShowClock(second)

	def HideClock(self):
		self.interface.HideClock()

	def BINARY_ActEmotion(self, emotionIndex):
		if self.interface.wndCharacter:
			self.interface.wndCharacter.ActEmotion(emotionIndex)


	def CheckFocus(self):
		if False == self.IsFocus():
			if True == self.interface.IsOpenChat():
				self.interface.ToggleChat()

			self.SetFocus()

	def SaveScreen(self):
		return




	def ShowConsole(self):
		if debugInfo.IsDebugMode() or True == self.consoleEnable:
			player.EndKeyWalkingImmediately()
			self.console.OpenWindow()

	def ShowName(self):
		if app.ENABLE_OFFLINE_SHOP:
			offlineshop.ShowShopNames()
		self.ShowNameFlag = True
		self.playerGauge.EnableShowAlways()
		if not app.ENABLE_KEYCHANGE_SYSTEM:
			player.SetQuickPage(self.quickSlotPageIndex + 1)

	def __IsShowName(self):

		if systemSetting.IsAlwaysShowName():
			return True

		if self.ShowNameFlag:
			return True

		return False

	def HideName(self):
		if app.ENABLE_OFFLINE_SHOP:
			offlineshop.HideShopNames()
		self.ShowNameFlag = False
		self.playerGauge.DisableShowAlways()
		if not app.ENABLE_KEYCHANGE_SYSTEM:
			player.SetQuickPage(self.quickSlotPageIndex)

	def ShowMouseImage(self):
		self.interface.ShowMouseImage()

	def HideMouseImage(self):
		self.interface.HideMouseImage()

	def StartAttack(self):
		player.SetAttackKeyState(True)

	def EndAttack(self):
		player.SetAttackKeyState(False)

	def MoveUp(self):
		player.SetSingleDIKKeyState(app.DIK_UP, True)

	def MoveDown(self):
		player.SetSingleDIKKeyState(app.DIK_DOWN, True)

	def MoveLeft(self):
		player.SetSingleDIKKeyState(app.DIK_LEFT, True)

	def MoveRight(self):
		player.SetSingleDIKKeyState(app.DIK_RIGHT, True)

	def StopUp(self):
		player.SetSingleDIKKeyState(app.DIK_UP, False)

	def StopDown(self):
		player.SetSingleDIKKeyState(app.DIK_DOWN, False)

	def StopLeft(self):
		player.SetSingleDIKKeyState(app.DIK_LEFT, False)

	def StopRight(self):
		player.SetSingleDIKKeyState(app.DIK_RIGHT, False)
		
	def PickUpItem(self):
		player.PickCloseItem()
			

	def OnKeyDown(self, key):
		if self.interface.wndWeb and self.interface.wndWeb.IsShow():
			return

		if app.ENABLE_BOT_CONTROL:
			if self.isBotControlActive():
				return

		if key == app.DIK_ESC:
			self.RequestDropItem(False)
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

		try:
			if self.isCinemaMode:
				self.onPressKeyDict[key]()
			elif key in (app.DIK_INSERT, app.DIK_PGUP, app.DIK_PGDN, app.DIK_HOME, app.DIK_END):
				self.onPressKeyDict[key]()
			elif app.ENABLE_KEYCHANGE_SYSTEM:
				if self.wndKeyChange.IsOpen() == 1:
					if self.wndKeyChange.IsSelectKeySlot():
						if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
							if self.wndKeyChange.IsChangeKey(self.wndKeyChange.GetSelectSlotNumber()):
								self.wndKeyChange.ChangeKey(key + app.DIK_LCONTROL + self.ADDKEYBUFFERCONTROL)
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.KEYCHANGE_IMPOSSIBLE_CHANGE)
						elif app.IsPressed(app.DIK_LALT) or app.IsPressed(app.DIK_RALT):
							if self.wndKeyChange.IsChangeKey(self.wndKeyChange.GetSelectSlotNumber()):
								self.wndKeyChange.ChangeKey(key + app.DIK_LALT + self.ADDKEYBUFFERALT)
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.KEYCHANGE_IMPOSSIBLE_CHANGE)
						elif app.IsPressed(app.DIK_LSHIFT) or app.IsPressed(app.DIK_RSHIFT):
							if self.wndKeyChange.IsChangeKey(self.wndKeyChange.GetSelectSlotNumber()):
								self.wndKeyChange.ChangeKey(key + app.DIK_LSHIFT + self.ADDKEYBUFFERSHIFT)
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.KEYCHANGE_IMPOSSIBLE_CHANGE)
						else:
							self.wndKeyChange.ChangeKey(key)
				else:
					player.OnKeyDown(key)
			else:
				self.onPressKeyDict[key]()
		except KeyError:
			pass
		except:
			raise

		return True

	def OnKeyUp(self, key):
		if app.ENABLE_KEYCHANGE_SYSTEM:
			player.OnKeyUp(key)
		else:
			try:
				self.onClickKeyDict[key]()
			except KeyError:
				pass
			except:
				raise

	def OnMouseLeftButtonDown(self):
		if self.interface.BUILD_OnMouseLeftButtonDown():
			return

		if mouseModule.mouseController.isAttached():
			self.CheckFocus()
		else:
			hyperlink = ui.GetHyperlink()
			if hyperlink:
				return
			else:
				self.CheckFocus()
				player.SetMouseState(player.MBT_LEFT, player.MBS_PRESS);

		return True

	def OnMouseLeftButtonUp(self):

		if self.interface.BUILD_OnMouseLeftButtonUp():
			return

		if mouseModule.mouseController.isAttached():

			attachedType = mouseModule.mouseController.GetAttachedType()
			attachedItemIndex = mouseModule.mouseController.GetAttachedItemIndex()
			attachedItemSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemCount = mouseModule.mouseController.GetAttachedItemCount()
			if app.ENABLE_OFFLINE_SHOP:
				if uiOfflineShop.IsBuildingShop() and uiOfflineShop.IsSaleSlot(player.SlotTypeToInvenType(attachedType), attachedItemSlotPos):
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CANNOT_DOING_PRIVATE_SHOP_OPEN)
					return

			if player.SLOT_TYPE_QUICK_SLOT == attachedType:
				player.RequestDeleteGlobalQuickSlot(attachedItemSlotPos)

			elif player.SLOT_TYPE_INVENTORY == attachedType or player.SLOT_TYPE_SKILL_BOOK_INVENTORY == attachedType or player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == attachedType or player.SLOT_TYPE_STONE_INVENTORY == attachedType or player.SLOT_TYPE_BOX_INVENTORY == attachedType or player.SLOT_TYPE_EFSUN_INVENTORY == attachedType or player.SLOT_TYPE_CICEK_INVENTORY == attachedType:

				if player.ITEM_MONEY == attachedItemIndex:
					self.__PutMoney(attachedType, attachedItemCount, self.PickingCharacterIndex)
				elif player.ITEM_CHEQUE == attachedItemIndex and app.ENABLE_CHEQUE_SYSTEM:
					self.__PutCheque(attachedType, attachedItemCount, self.PickingCharacterIndex)
				else:
					self.__PutItem(attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount, self.PickingCharacterIndex)

			elif player.SLOT_TYPE_DRAGON_SOUL_INVENTORY == attachedType:
				self.__PutItem(attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount, self.PickingCharacterIndex)

			mouseModule.mouseController.DeattachObject()

		else:
			hyperlink = ui.GetHyperlink()
			if hyperlink:
				if app.IsPressed(app.DIK_LALT):
					link = chat.GetLinkFromHyperlink(hyperlink)
					ime.PasteString(link)
				else:
					self.interface.MakeHyperlinkTooltip(hyperlink)
				return
			else:
				player.SetMouseState(player.MBT_LEFT, player.MBS_CLICK)

		return True

	def __PutItem(self, attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount, dstChrID):
		if player.SLOT_TYPE_INVENTORY == attachedType or player.SLOT_TYPE_DRAGON_SOUL_INVENTORY == attachedType or player.SLOT_TYPE_SKILL_BOOK_INVENTORY == attachedType or player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == attachedType or player.SLOT_TYPE_STONE_INVENTORY == attachedType or player.SLOT_TYPE_BOX_INVENTORY == attachedType or player.SLOT_TYPE_EFSUN_INVENTORY == attachedType or player.SLOT_TYPE_CICEK_INVENTORY == attachedType:
			attachedInvenType = player.SlotTypeToInvenType(attachedType)
			if True == chr.HasInstance(self.PickingCharacterIndex) and player.GetMainCharacterIndex() != dstChrID:
				if player.IsEquipmentSlot(attachedItemSlotPos) and player.SLOT_TYPE_DRAGON_SOUL_INVENTORY != attachedType:
					self.stream.popupWindow.Close()
					self.stream.popupWindow.Open(localeInfo.EXCHANGE_FAILURE_EQUIP_ITEM, 0, localeInfo.UI_OK)
				else:
					if chr.IsNPC(dstChrID):
						if app.ENABLE_REFINE_RENEWAL:
							constInfo.AUTO_REFINE_TYPE = 2
							constInfo.AUTO_REFINE_DATA["NPC"][0] = dstChrID
							constInfo.AUTO_REFINE_DATA["NPC"][1] = attachedInvenType
							constInfo.AUTO_REFINE_DATA["NPC"][2] = attachedItemSlotPos
							constInfo.AUTO_REFINE_DATA["NPC"][3] = attachedItemCount
						net.SendGiveItemPacket(dstChrID, attachedInvenType, attachedItemSlotPos, attachedItemCount)
					else:
						net.SendExchangeStartPacket(dstChrID)
						net.SendExchangeItemAddPacket(attachedInvenType, attachedItemSlotPos, 0)
			else:
				self.__DropItem(attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount)

	def __PutMoney(self, attachedType, attachedMoney, dstChrID):
		if True == chr.HasInstance(dstChrID) and player.GetMainCharacterIndex() != dstChrID:
			net.SendExchangeStartPacket(dstChrID)
			net.SendExchangeElkAddPacket(attachedMoney)
		else:
			self.__DropMoney(attachedType, attachedMoney)

	def __DropMoney(self, attachedType, attachedMoney):
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
			return

		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			if (uiOfflineShopBuilder.IsBuildingOfflineShop()):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_OFFLINE_SHOP)
				return
			
			if (uiOfflineShop.IsEditingOfflineShop()):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_OFFLINE_SHOP)
				return

		if attachedMoney>=1000:
			self.stream.popupWindow.Close()
			self.stream.popupWindow.Open(localeInfo.DROP_MONEY_FAILURE_1000_OVER, 0, localeInfo.UI_OK)
			return

		itemDropQuestionDialog = uiCommon.QuestionDialog()
		itemDropQuestionDialog.SetText(localeInfo.DO_YOU_DROP_MONEY % (attachedMoney))
		itemDropQuestionDialog.SetAcceptEvent(lambda arg=True: self.RequestDropItem(arg))
		itemDropQuestionDialog.SetCancelEvent(lambda arg=False: self.RequestDropItem(arg))
		itemDropQuestionDialog.Open()
		itemDropQuestionDialog.dropType = attachedType
		itemDropQuestionDialog.dropCount = attachedMoney
		itemDropQuestionDialog.dropNumber = player.ITEM_MONEY
		self.itemDropQuestionDialog = itemDropQuestionDialog

	if app.ENABLE_CHEQUE_SYSTEM:
		def __PutCheque(self, attachedType, attachedMoney, dstChrID):
			if True == chr.HasInstance(dstChrID) and player.GetMainCharacterIndex() != dstChrID:
				net.SendExchangeStartPacket(dstChrID)
				net.SendExchangeChequeAddPacket(attachedMoney)
			else:
				self.__DropCheque(attachedType, attachedMoney)

		def __DropCheque(self, attachedType, attachedMoney):
			if uiPrivateShopBuilder.IsBuildingPrivateShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
				return
			
			if attachedMoney>=1000:
				self.stream.popupWindow.Close()
				self.stream.popupWindow.Open(localeInfo.DROP_CHEQUE_FAILURE_1000_OVER, 0, localeInfo.UI_OK)
				return

			itemDropQuestionDialog = uiCommon.QuestionDialog()
			itemDropQuestionDialog.SetText(localeInfo.DO_YOU_DROP_CHEQUE % (attachedMoney))
			itemDropQuestionDialog.SetAcceptEvent(lambda arg=True: self.RequestDropItem(arg))
			itemDropQuestionDialog.SetCancelEvent(lambda arg=False: self.RequestDropItem(arg))
			itemDropQuestionDialog.Open()
			itemDropQuestionDialog.dropType = attachedType
			itemDropQuestionDialog.dropCount = attachedMoney
			itemDropQuestionDialog.dropNumber = player.ITEM_CHEQUE
			self.itemDropQuestionDialog = itemDropQuestionDialog

	def __DropItem(self, attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount):
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
			return

		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			if (uiOfflineShopBuilder.IsBuildingOfflineShop()):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_OFFLINE_SHOP)
				return

			if (uiOfflineShop.IsEditingOfflineShop()):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_OFFLINE_SHOP)
				return

		if player.SLOT_TYPE_INVENTORY == attachedType and player.IsEquipmentSlot(attachedItemSlotPos):
			self.stream.popupWindow.Close()
			self.stream.popupWindow.Open(localeInfo.DROP_ITEM_FAILURE_EQUIP_ITEM, 0, localeInfo.UI_OK)

		else:
			if player.SLOT_TYPE_INVENTORY == attachedType or player.SLOT_TYPE_SKILL_BOOK_INVENTORY == attachedType or player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == attachedType or player.SLOT_TYPE_STONE_INVENTORY == attachedType or player.SLOT_TYPE_BOX_INVENTORY == attachedType or player.SLOT_TYPE_EFSUN_INVENTORY == attachedType or player.SLOT_TYPE_CICEK_INVENTORY == attachedType:
				dropItemIndex = player.GetItemIndex(attachedItemSlotPos)

				item.SelectItem(dropItemIndex)
				dropItemName = item.GetItemName()

				questionText = localeInfo.HOW_MANY_ITEM_DO_YOU_DROP(dropItemName, attachedItemCount)

				itemDropQuestionDialog = uiCommon.QuestionDialog()
				itemDropQuestionDialog.SetText(questionText)
				itemDropQuestionDialog.SetAcceptEvent(lambda arg=True: self.RequestDropItem(arg))
				itemDropQuestionDialog.SetCancelEvent(lambda arg=False: self.RequestDropItem(arg))
				itemDropQuestionDialog.Open()
				itemDropQuestionDialog.dropType = attachedType
				itemDropQuestionDialog.dropNumber = attachedItemSlotPos
				itemDropQuestionDialog.dropCount = attachedItemCount
				self.itemDropQuestionDialog = itemDropQuestionDialog

				constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)
			elif player.SLOT_TYPE_DRAGON_SOUL_INVENTORY == attachedType:
				dropItemIndex = player.GetItemIndex(player.DRAGON_SOUL_INVENTORY, attachedItemSlotPos)

				item.SelectItem(dropItemIndex)
				dropItemName = item.GetItemName()

				questionText = localeInfo.HOW_MANY_ITEM_DO_YOU_DROP(dropItemName, attachedItemCount)

				itemDropQuestionDialog = uiCommon.QuestionDialog()
				itemDropQuestionDialog.SetText(questionText)
				itemDropQuestionDialog.SetAcceptEvent(lambda arg=True: self.RequestDropItem(arg))
				itemDropQuestionDialog.SetCancelEvent(lambda arg=False: self.RequestDropItem(arg))
				itemDropQuestionDialog.Open()
				itemDropQuestionDialog.dropType = attachedType
				itemDropQuestionDialog.dropNumber = attachedItemSlotPos
				itemDropQuestionDialog.dropCount = attachedItemCount
				self.itemDropQuestionDialog = itemDropQuestionDialog

				constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def RequestDropItem(self, answer):
		if not self.itemDropQuestionDialog:
			return

		if answer:
			dropType = self.itemDropQuestionDialog.dropType
			dropCount = self.itemDropQuestionDialog.dropCount
			dropNumber = self.itemDropQuestionDialog.dropNumber

			if player.SLOT_TYPE_INVENTORY == dropType or player.SLOT_TYPE_SKILL_BOOK_INVENTORY == dropType or player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == dropType or player.SLOT_TYPE_STONE_INVENTORY == dropType or player.SLOT_TYPE_BOX_INVENTORY == dropType or player.SLOT_TYPE_EFSUN_INVENTORY == dropType or player.SLOT_TYPE_CICEK_INVENTORY == dropType:
				if dropNumber == player.ITEM_MONEY:
					net.SendGoldDropPacketNew(dropCount)
					snd.PlaySound("sound/ui/money.wav")
				elif app.ENABLE_CHEQUE_SYSTEM and dropNumber == player.ITEM_CHEQUE:
					net.SendGoldChequePacketNew(dropCount)
					snd.PlaySound("sound/ui/money.wav")
				else:
					self.__SendDropItemPacket(dropNumber, dropCount)
			elif player.SLOT_TYPE_DRAGON_SOUL_INVENTORY == dropType:
					self.__SendDropItemPacket(dropNumber, dropCount, player.DRAGON_SOUL_INVENTORY)
			elif app.WJ_SPLIT_INVENTORY_SYSTEM:
					if player.SLOT_TYPE_SKILL_BOOK_INVENTORY == dropType or player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == dropType or player.SLOT_TYPE_STONE_INVENTORY == dropType or player.SLOT_TYPE_BOX_INVENTORY == dropType or player.SLOT_TYPE_EFSUN_INVENTORY == dropType or player.SLOT_TYPE_CICEK_INVENTORY == dropType:
						self.__SendDropItemPacket(dropNumber, dropCount, player.SLOT_TYPE_SKILL_BOOK_INVENTORY or player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY or player.SLOT_TYPE_STONE_INVENTORY or player.SLOT_TYPE_BOX_INVENTORY or player.SLOT_TYPE_EFSUN_INVENTORY or player.SLOT_TYPE_CICEK_INVENTORY)

		self.itemDropQuestionDialog.Close()
		self.itemDropQuestionDialog = None

		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	def __SendDropItemPacket(self, itemVNum, itemCount, itemInvenType = player.INVENTORY):
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
			return

		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			if (uiOfflineShopBuilder.IsBuildingOfflineShop()):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_OFFLINE_SHOP)
				return
				
			if (uiOfflineShop.IsEditingOfflineShop()):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_OFFLINE_SHOP)
				return

		net.SendItemDropPacketNew(itemInvenType, itemVNum, itemCount)

	def OnMouseRightButtonDown(self):

		self.CheckFocus()

		if True == mouseModule.mouseController.isAttached():
			mouseModule.mouseController.DeattachObject()

		else:
			player.SetMouseState(player.MBT_RIGHT, player.MBS_PRESS)

		return True

	def OnMouseRightButtonUp(self):
		if True == mouseModule.mouseController.isAttached():
			return True

		player.SetMouseState(player.MBT_RIGHT, player.MBS_CLICK)
		return True

	def OnMouseMiddleButtonDown(self):
		player.SetMouseMiddleButtonState(player.MBS_PRESS)

	def OnMouseMiddleButtonUp(self):
		player.SetMouseMiddleButtonState(player.MBS_CLICK)

	def OnUpdate(self):
		app.UpdateGame()

		if app.IsPressed(app.DIK_Z):
			player.PickCloseItem()

		if self.isShowDebugInfo:
			self.UpdateDebugInfo()

		if self.enableXMasBoom:
			self.__XMasBoom_Update()

		if app.ENABLE_BOT_CONTROL:
			if app.GetTime() > constInfo.BOTCONTROL_TIME:
				if constInfo.BOTCONTROL_TIME != 0:
					player.SetBotControlState(True)
				self.BINARY_BotControlTimeUpdate()
			

		self.interface.BUILD_OnUpdate()

		if app.ENABLE_VOTE4BUFF:
			self.interface.Vote4Buff_OnUpdate()
			
		self.tweenMgr.OnUpdate()

		if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
			uiPrivateShop.UpdateTitleBoard()

		if app.ENABLE_AUTO_SHOUT:
			if constInfo.AUTO_SHOUT_ACTIVATED and len(constInfo.LAST_SHOUT_MESSAGE) != 0:
				if self.lastShoutTime <= app.GetTime():
					self.lastShoutTime = app.GetTime() + constInfo.SHOUT_PER_SECOND
					net.SendChatPacket(str(constInfo.LAST_SHOUT_MESSAGE), chat.CHAT_TYPE_SHOUT)

	def UpdateDebugInfo(self):
		(x, y, z) = player.GetMainCharacterPosition()
		nUpdateTime = app.GetUpdateTime()
		nUpdateFPS = app.GetUpdateFPS()
		nRenderFPS = app.GetRenderFPS()
		nFaceCount = app.GetFaceCount()
		fFaceSpeed = app.GetFaceSpeed()
		nST=background.GetRenderShadowTime()
		(fAveRT, nCurRT) =  app.GetRenderTime()
		(iNum, fFogStart, fFogEnd, fFarCilp) = background.GetDistanceSetInfo()
		(iPatch, iSplat, fSplatRatio, sTextureNum) = background.GetRenderedSplatNum()
		if iPatch == 0:
			iPatch = 1


		self.PrintCoord.SetText("Coordinate: %.2f %.2f %.2f ATM: %d" % (x, y, z, app.GetAvailableTextureMemory()/(1024*1024)))
		xMouse, yMouse = wndMgr.GetMousePosition()
		self.PrintMousePos.SetText("MousePosition: %d %d" % (xMouse, yMouse))

		self.FrameRate.SetText("UFPS: %3d UT: %3d FS %.2f" % (nUpdateFPS, nUpdateTime, fFaceSpeed))

		if fAveRT>1.0:
			self.Pitch.SetText("RFPS: %3d RT:%.2f(%3d) FC: %d(%.2f) " % (nRenderFPS, fAveRT, nCurRT, nFaceCount, nFaceCount/fAveRT))

		self.Splat.SetText("PATCH: %d SPLAT: %d BAD(%.2f)" % (iPatch, iSplat, fSplatRatio))
		self.ViewDistance.SetText("Num : %d, FS : %f, FE : %f, FC : %f" % (iNum, fFogStart, fFogEnd, fFarCilp))

	def OnRender(self):
		app.RenderGame()

		if self.console.Console.collision:
			background.RenderCollision()
			chr.RenderCollision()

		(x, y) = app.GetCursorPosition()

		textTail.UpdateAllTextTail()

		if True == wndMgr.IsPickedWindow(self.hWnd):

			self.PickingCharacterIndex = chr.Pick()

			if -1 != self.PickingCharacterIndex:
				textTail.ShowCharacterTextTail(self.PickingCharacterIndex)
			if 0 != self.targetBoard.GetTargetVID():
				textTail.ShowCharacterTextTail(self.targetBoard.GetTargetVID())

			if not self.__IsShowName():
				self.PickingItemIndex = item.Pick()
				if -1 != self.PickingItemIndex:
					textTail.ShowItemTextTail(self.PickingItemIndex)


		if self.__IsShowName():
			textTail.ShowAllTextTail()
			self.PickingItemIndex = textTail.Pick(x, y)

		textTail.UpdateShowingTextTail()
		textTail.ArrangeTextTail()
		if -1 != self.PickingItemIndex:
			textTail.SelectItemName(self.PickingItemIndex)

		grp.PopState()
		grp.SetInterfaceRenderState()

		textTail.Render()
		textTail.HideAllTextTail()

	def OnPressEscapeKey(self):

		if app.ENABLE_BOT_CONTROL:
			if self.isBotControlActive():
				return
		if app.TARGET == app.GetCursor():
			app.SetCursor(app.NORMAL)

		elif True == mouseModule.mouseController.isAttached():
			mouseModule.mouseController.DeattachObject()

		else:
			self.interface.OpenSystemDialog()

		return True

	def OnIMEReturn(self):
		if app.IsPressed(app.DIK_LSHIFT):
			self.interface.OpenWhisperDialogWithoutTarget()
		else:
			self.interface.ToggleChat()
		return True

	def OnPressExitKey(self):
		self.interface.ToggleSystemDialog()
		return True

	
	if app.WJ_ENABLE_TRADABLE_ICON:
		def BINARY_AddItemToExchange(self, inven_type, inven_pos, display_pos):
			if inven_type == player.INVENTORY:
				self.interface.CantTradableItemExchange(display_pos, inven_pos)

	def BINARY_LoverInfo(self, name, lovePoint):
		if self.interface.wndMessenger:
			self.interface.wndMessenger.OnAddLover(name, lovePoint)
		if self.affectShower:
			self.affectShower.SetLoverInfo(name, lovePoint)

	def BINARY_UpdateLovePoint(self, lovePoint):
		if self.interface.wndMessenger:
			self.interface.wndMessenger.OnUpdateLovePoint(lovePoint)
		if self.affectShower:
			self.affectShower.OnUpdateLovePoint(lovePoint)

	def BINARY_OnQuestConfirm(self, msg, timeout, pid):
		confirmDialog = uiCommon.QuestionDialogWithTimeLimit()
		confirmDialog.Open(msg, timeout)
		confirmDialog.SetAcceptEvent(lambda answer=True, pid=pid: net.SendQuestConfirmPacket(answer, pid) or self.confirmDialog.Hide())
		confirmDialog.SetCancelEvent(lambda answer=False, pid=pid: net.SendQuestConfirmPacket(answer, pid) or self.confirmDialog.Hide())
		self.confirmDialog = confirmDialog

	def Gift_Show(self):
		self.interface.ShowGift()

	def BINARY_AddToQueue(self, vnum, value):
		self.interface.AppendQueue(vnum, value)

	def BINARY_Highlight_Item(self, inven_type, inven_pos):
		if self.interface:
			self.interface.Highligt_Item(inven_type, inven_pos)

	def BINARY_Cards_UpdateInfo(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, hand_4, hand_4_v, hand_5, hand_5_v, cards_left, points):
		self.interface.UpdateCardsInfo(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, hand_4, hand_4_v, hand_5, hand_5_v, cards_left, points)
		
	def BINARY_Cards_FieldUpdateInfo(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points):
		self.interface.UpdateCardsFieldInfo(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points)
		
	def BINARY_Cards_PutReward(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points):
		self.interface.CardsPutReward(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points)
		
	def BINARY_Cards_ShowIcon(self):
		self.interface.CardsShowIcon()
		
	def BINARY_Cards_Open(self, safemode):
		self.interface.OpenCardsWindow(safemode)

	def BINARY_DragonSoulGiveQuilification(self):
		self.interface.DragonSoulGiveQuilification()

	if app.ENABLE_DS_CHANGE_ATTR:
		def BINARY_DragonSoulRefineWindow_Open(self, type):
			self.interface.OpenDragonSoulRefineWindow(type)
	else:
		def BINARY_DragonSoulRefineWindow_Open(self):
			self.interface.OpenDragonSoulRefineWindow()

	def BINARY_DragonSoulRefineWindow_RefineFail(self, reason, inven_type, inven_pos):
		self.interface.FailDragonSoulRefine(reason, inven_type, inven_pos)

	def BINARY_DragonSoulRefineWindow_RefineSucceed(self, inven_type, inven_pos):
		self.interface.SucceedDragonSoulRefine(inven_type, inven_pos)


	def BINARY_SetBigMessage(self, message):
		self.interface.bigBoard.SetTip(message)

	def BINARY_SetTipMessage(self, message):
		self.interface.tipBoard.SetTip(message)

	if app.ENABLE_DUNGEON_INFO_SYSTEM:
		def BINARY_DungeonInfoOpen(self):
			if self.interface:
				self.interface.DungeonInfoOpen()

		def BINARY_DungeonRankingRefresh(self):
			if self.interface:
				self.interface.DungeonRankingRefresh()

		def BINARY_DungeonInfoReload(self, onReset):
			if self.interface:
				self.interface.DungeonInfoReload(onReset)

	def BINARY_AppendNotifyMessage(self, type):
		if not type in localeInfo.NOTIFY_MESSAGE:
			return
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.NOTIFY_MESSAGE[type])

	def BINARY_Guild_EnterGuildArea(self, areaID):
		self.interface.BULID_EnterGuildArea(areaID)

	def BINARY_Guild_ExitGuildArea(self, areaID):
		self.interface.BULID_ExitGuildArea(areaID)

	def BINARY_GuildWar_OnSendDeclare(self, guildID):
		pass

	def BINARY_GuildWar_OnRecvDeclare(self, guildID, warType):
		mainCharacterName = player.GetMainCharacterName()
		masterName = guild.GetGuildMasterName()
		if mainCharacterName == masterName:
			self.__GuildWar_OpenAskDialog(guildID, warType)

	def BINARY_GuildWar_OnRecvPoint(self, gainGuildID, opponentGuildID, point):
		self.interface.OnRecvGuildWarPoint(gainGuildID, opponentGuildID, point)

	def BINARY_GuildWar_OnStart(self, guildSelf, guildOpp):
		self.interface.OnStartGuildWar(guildSelf, guildOpp)

	def BINARY_GuildWar_OnEnd(self, guildSelf, guildOpp):
		self.interface.OnEndGuildWar(guildSelf, guildOpp)

	def BINARY_BettingGuildWar_SetObserverMode(self, isEnable):
		self.interface.BINARY_SetObserverMode(isEnable)

	def BINARY_BettingGuildWar_UpdateObserverCount(self, observerCount):
		self.interface.wndMiniMap.UpdateObserverCount(observerCount)

	def __GuildWar_UpdateMemberCount(self, guildID1, memberCount1, guildID2, memberCount2, observerCount):
		guildID1 = int(guildID1)
		guildID2 = int(guildID2)
		memberCount1 = int(memberCount1)
		memberCount2 = int(memberCount2)
		observerCount = int(observerCount)

		self.interface.UpdateMemberCount(guildID1, memberCount1, guildID2, memberCount2)
		self.interface.wndMiniMap.UpdateObserverCount(observerCount)

	def __GuildWar_OpenAskDialog(self, guildID, warType):

		guildName = guild.GetGuildName(guildID)

		if "Noname" == guildName:
			return

		import uiGuild
		questionDialog = uiGuild.AcceptGuildWarDialog()
		questionDialog.SAFE_SetAcceptEvent(self.__GuildWar_OnAccept)
		questionDialog.SAFE_SetCancelEvent(self.__GuildWar_OnDecline)
		questionDialog.Open(guildName, warType)

		self.guildWarQuestionDialog = questionDialog

	def __GuildWar_CloseAskDialog(self):
		self.guildWarQuestionDialog.Close()
		self.guildWarQuestionDialog = None

	def __GuildWar_OnAccept(self):

		guildName = self.guildWarQuestionDialog.GetGuildName()

		net.SendChatPacket("/war " + guildName)
		self.__GuildWar_CloseAskDialog()

		return 1

	def __GuildWar_OnDecline(self):

		guildName = self.guildWarQuestionDialog.GetGuildName()

		net.SendChatPacket("/nowar " + guildName)
		self.__GuildWar_CloseAskDialog()

		return 1

	def __ServerCommand_Build(self):
		serverCommandList={
			"ConsoleEnable"			: self.__Console_Enable,
			"DayMode"				: self.__DayMode_Update,
			"PRESERVE_DayMode"		: self.__PRESERVE_DayMode_Update,
			"CloseRestartWindow"	: self.__RestartDialog_Close,
			"OpenPrivateShop"		: self.__PrivateShop_Open,
			"PartyHealReady"		: self.PartyHealReady,
			"ShowMeSafeboxPassword"	: self.AskSafeboxPassword,
			"CloseSafebox"			: self.CommandCloseSafebox,

			"CloseMall"				: self.CommandCloseMall,
			"ShowMeMallPassword"	: self.AskMallPassword,
			"item_mall"				: self.__ItemMall_Open,

			"RefineSuceeded"		: self.RefineSuceededMessage,
			"RefineFailed"			: self.RefineFailedMessage,
			"xmas_snow"				: self.__XMasSnow_Enable,
			"xmas_boom"				: self.__XMasBoom_Enable,
			"xmas_song"				: self.__XMasSong_Enable,
			"xmas_tree"				: self.__XMasTree_Enable,
			"newyear_boom"			: self.__XMasBoom_Enable,
			"PartyRequest"			: self.__PartyRequestQuestion,
			"PartyRequestDenied"	: self.__PartyRequestDenied,
			"horse_state"			: self.__Horse_UpdateState,
			"hide_horse_state"		: self.__Horse_HideState,
			"WarUC"					: self.__GuildWar_UpdateMemberCount,
			"test_server"			: self.__EnableTestServerFlag,
			"mall"			: self.__InGameShop_Show,


			"lover_login"			: self.__LoginLover,
			"lover_logout"			: self.__LogoutLover,
			"lover_near"			: self.__LoverNear,
			"lover_far"				: self.__LoverFar,
			"lover_divorce"			: self.__LoverDivorce,
			"PlayMusic"				: self.__PlayMusic,

			"MyShopPriceList"		: self.__PrivateShop_PriceList,

			"REV_CARD_REWARD"		: self.CardsReward,
			"REW_CARD_OPEN"			: self.CardsOpen,
			
			"get_input_start"		: self.GetInputOn,
			"get_input_end"			: self.GetInputOff,
			"AddCasketItem" : self.__RecvCasketItem,
			"SortCasketWindow" : self.__RecvCasketBuild,
			
			"load_server_rank"		: self.ServerRankLoadInfo,
			"load_server_rank_pos"	: self.ServerRankLoadMyPos,	
			"load_rank"				: self.RankLoadInfo,
			"load_rank_pos"			: self.RankLoadMyPos,	
			
			"enlight_o"				: self.EnlightenmentWindowOpen,
			"enlight_r"				: self.EnlightenmentWindowRefresh,
			"enlight_n"				: self.EnlightenmentUpgradeNotice,
			
			"my_ore"				: self.SetMyOreCount,
			"my_fish"				: self.SetMyFishCount,
			"title_progress_boss_killed"				: self.SetTitleBossCount,
			"title_progress_stones_killed"				: self.SetTitleStonesCount,
			"title_progress_enchant_used"				: self.SetTitleEnchantCount,
			"title_progress_refine_success"				: self.SetTitleRefineSuccessCount,
			"title_progress_refine_alchemy"				: self.SetTitleRefineAlchemyCount,
			"title_progress_refine_fail"				: self.SetTitleRefineFailCount,
			"title_progress_playtime"				: self.SetTitlePlaytimeCount,
			"title_progress_rare_count"				: self.SetTitleRareCount,
			"title_progress_relict_count"				: self.SetTitleRelictCount,
			"title_progress_hold_fastest_time"				: self.SetTitleDungeonFastestTime,
			"title_progress_killed_dungeon_bosses"				: self.SetTitleDungeonBossKilled,
			"title_progress_fastest_dungeon"				: self.SetTitleFastestDungeon,
			"title_progress_die_to_monster"				: self.SetTitleDieToMonster,
			"title_progress_dungeon_no_damage"				: self.SetTitleDungeonNoDamage,
			
			"my_ore_fish"			: self.SetMyOreFishCount,
			"fishw_new"				: self.FishWiki_NewFishUnlocked,
			"fishw_ustat"			: self.FishWiki_UnlockStatus,
			"fishw_fdata"			: self.FishWiki_FishData,
			"fishw_ranks"			: self.FishWiki_RankLoadStart,
			"fishw_rankd"			: self.FishWiki_RankLoadData,
			"fishw_ranke"			: self.FishWiki_RankLoadEnd,
			"fishw_loader"			: self.FishWiki_LoadError,
			"event": self.__ProcessServerEventFromCommander,

		}
		if app.ENABLE_COLLECTIONS_SYSTEM:
			serverCommandList["RECV_Collection"] = self.__RecvCollection
			serverCommandList["RECV_CollectionItem"] = self.__RecvCollectionItem
			serverCommandList["RECV_CollectionBuild"] = self.__RecvCollectionBuild
			serverCommandList["RECV_CollectionRefresh"] = self.__RecvCollectionRefresh
			serverCommandList["RECV_CollectionIncrease"] = self.__RecvCollectionIncrease

		if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
			serverCommandList["BuffNPCSummon"] = self.__SetBuffNPCSummon
			serverCommandList["BuffNPCUnsummon"] = self.__SetBuffNPCUnsummon
			serverCommandList["BuffNPCClear"] = self.__SetBuffNPCClear
			serverCommandList["BuffNPCBasicInfo"] = self.__SetBuffNPCBasicInfo
			serverCommandList["BuffNPCSkillInfo"] = self.__SetBuffNPCSkillInfo
			serverCommandList["BuffNPCSkillCooltime"] = self.__SetBuffNPCSkillSetSkillCooltime
			serverCommandList["BuffNPCCreatePopup"] = self.__SetBuffNPCCreatePopup

		if app.WORLD_BOSS_YUMA:
			serverCommandList["SendWorldbossNotification"] = self.WorldbossNotficiation

		if constInfo.GIFT_CODE_SYSTEM:
			serverCommandList.update({"OpenCodeWindow" : self.__OpenCodeWindow})
			
		if app.ENABLE_ITEMSHOP:
			serverCommandList.update({"SetWheelItemData" : self.interface.SetWheelItemData})
			serverCommandList.update({"OnSetWhell" : self.interface.OnSetWhell})
			serverCommandList.update({"GetWheelGiftData" : self.interface.GetWheelGiftData})
			serverCommandList.update({"SetDragonCoin" : self.ItemShopSetDragonCoin})
			serverCommandList.update({"ItemShopAppendLog" : self.ItemShopAppendLogEx})
			serverCommandList.update({"RefreshAccountMoney" : self.ItemShopSetDragonCoin})
			serverCommandList.update({"open_ishop" : self.BINARY_OpenItemShop})

		if app.ENABLE_AUTO_SELECT_SKILL:
			serverCommandList.update({"OpenAutoSkill" : self.interface.OpenAutoSkillWindow})

		serverCommandList.update({"getinputbegin" : self.__Inputget1 })
		serverCommandList.update({"getinputend" : self.__Inputget2 })

		if app.ENABLE_COLLECT_WINDOW:
			serverCommandList.update({"UpdateTime" : self.UpdateTime})
			serverCommandList.update({"UpdateChance" : self.UpdateChance})

		if app.ENABLE_DROP_INFO:
			serverCommandList.update({
				"DropInfoRefresh" : self.DropInfoRefresh,
			})

		if app.__DUNGEON_INFO__:
			serverCommandList.update({
				"dungeon_info_qid" : self.interface.DungeonInfoQuestIdx,
				"dungeon_info_cmd" : self.interface.DungeonInfoQuestCMD,
				"dungeon_info_cooldown" : self.interface.DungeonInfoCooldown,
				"dungeon_log_info" : self.interface.LoadServerData,
				})

		if app.ENABLE_HIDE_COSTUME_SYSTEM:
			serverCommandList.update({"SetBodyCostumeHidden" : self.SetBodyCostumeHidden })
			serverCommandList.update({"SetHairCostumeHidden" : self.SetHairCostumeHidden })
			serverCommandList.update({"SetAcceCostumeHidden" : self.SetAcceCostumeHidden })
			serverCommandList.update({"SetWeaponCostumeHidden" : self.SetWeaponCostumeHidden })
			serverCommandList.update({"SetAuraCostumeHidden" : self.SetAuraCostumeHidden })
			serverCommandList.update({"SetStoleCostumeHidden" : self.SetStoleCostumeHidden })

		if app.BL_MOVE_CHANNEL:
			serverCommandList["server_info"] = self.__SeverInfo

		if app.TEAM_MEMBER_STATUS:
			serverCommandList["UpdateTeamMemberStatus"] = self.__BINARY__UpdateTeamMemberStatus
			serverCommandList["ClearTeamMemberList"] = self.__BINARY__ClearTeamMemberList

		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			serverCommandList["OpenOfflineShop"] =self.__OfflineShop_Open
			serverCommandList["OpenOfflineShopPanel"] =self.OpenOfflineShopPanel
			serverCommandList["OpenOfflineShopLogs"]=self.OpenOfflineShopLogs
		if app.ENABLE_NEW_PET_SYSTEM:
			serverCommandList["pet_info"] = self.__pet_info
			serverCommandList["pet_info_update"] = self.__pet_info_update


		if app.ENABLE_ODLAMKI_SYSTEM:
			serverCommandList["OpenOdlamki"] = self.OpenOdlamki

		if app.ENABLE_COLLECT_WINDOW:
			serverCommandList["SetCollectWindowQID"] = self.__SetCollectWindowQID
			serverCommandList["OpenCollectWindow"] = self.OpenCollectWindow
			serverCommandList["SetMissionNotifications"] = self.__SetMissionNotifications
		
		if app.__AUTO_SKILL_READER__:
			serverCommandList["AutoSkillStatus"] = self.interface.AutoSkillStatus
			
		serverCommandList["OpenCrystalExchange"] = self.__CrystalExchange_Open

		if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
			serverCommandList["SetPrivateShopPremiumBuild"] = self.SetPrivateShopPremiumBuild
			
		if app.__SPIN_WHEEL__:
			serverCommandList.update({
				"SetSpinReward" : self.interface.SetSpinReward,
				"SetSpinWheel" : self.interface.SetSpinWheel,
				})
				
		if app.ENABLE_WHEEL_OF_FORTUNE:
			serverCommandList.update({"SetItemData" : self.interface.SetItemData})
			serverCommandList.update({"OnSetWhell" : self.interface.OnSetWhell})
			serverCommandList.update({"GetGiftData" : self.interface.GetGiftData})
			
		if app.ENABLE_PVP_RANKING:
			serverCommandList.update({
				"pvp_empty_window"		: self.PVP_Empty,
			})

		if app.ENABLE_MULTI_FARM_BLOCK:
			serverCommandList.update({"UpdateMultiFarmAffect" : self.UpdateMultiFarmAffect})
			serverCommandList.update({"UpdateMultiFarmPlayer" : self.UpdateMultiFarmPlayer})
			
		if app.__PENDANT__:
			serverCommandList.update({"SetPendantPageIdx" : self.interface.SetPendantPageIdx})
			
		if app.ENABLE_VOTE4BUFF:
			serverCommandList.update({
				"vote4buff_d"			: self.__Vote4BuffServerData,
				"vote4buff_a"			: self.__Vote4BuffVoteApi,
				"vote4buff_u"			: self.__Vote4BuffVoteURL,
				"vote4buff"				: self.__Vote4Buff,
			})
			
		self.serverCommander=stringCommander.Analyzer()
		for serverCommandItem in serverCommandList.items():
			self.serverCommander.SAFE_RegisterCallBack(
				serverCommandItem[0], serverCommandItem[1]
			)

		if app.ENABLE_SECONDARY_LEVEL:
			self.serverCommander.SAFE_RegisterCallBack("OpenSecondaryLevelWindow", self.__SecondaryLevel_Open)

		self.serverCommander.SAFE_RegisterCallBack("EnlightenmentWindowOpen", self.EnlightenmentWindowOpen)


	if app.ENABLE_HIDE_COSTUME_SYSTEM:
		def SetBodyCostumeHidden(self, hidden):
			constInfo.HIDDEN_BODY_COSTUME = int(hidden)
			self.interface.RefreshVisibleCostume()

		def SetHairCostumeHidden(self, hidden):
			constInfo.HIDDEN_HAIR_COSTUME = int(hidden)
			self.interface.RefreshVisibleCostume()

		def SetAcceCostumeHidden(self, hidden):
			constInfo.HIDDEN_ACCE_COSTUME = int(hidden)
			self.interface.RefreshVisibleCostume()
				
		def SetStoleCostumeHidden(self, hidden):
			constInfo.HIDDEN_STOLE_COSTUME = int(hidden)
			self.interface.RefreshVisibleCostume()

		def SetWeaponCostumeHidden(self, hidden):
			constInfo.HIDDEN_WEAPON_COSTUME = int(hidden)
			self.interface.RefreshVisibleCostume()

		def SetAuraCostumeHidden(self, hidden):
			constInfo.HIDDEN_AURA_COSTUME = int(hidden)
			self.interface.RefreshVisibleCostume()

	if app.ENABLE_COLLECTIONS_SYSTEM:
		def __RecvCollection(self, collectionIdx, name, isComplete):
			if self.interface and self.interface.wndCollections:
				self.interface.wndCollections.AddCollection(collectionIdx, name, isComplete)
		
		def __RecvCollectionItem(self, collectionIdx, itemIdx, iVnum, iCount, myCount):
			if self.interface and self.interface.wndCollections:
				self.interface.wndCollections.AddItem(collectionIdx, itemIdx, iVnum, iCount, myCount)
				
		def __RecvCollectionBuild(self):
			if self.interface and self.interface.wndCollections:
				self.interface.wndCollections.Build()
		
		def __RecvCollectionRefresh(self, collectionIdx, itemIdx, myCount):
			if self.interface and self.interface.wndCollections:
				self.interface.wndCollections.UpdateValue(collectionIdx, itemIdx, myCount)
		
		def __RecvCollectionIncrease(self, isIncreased):
			if self.interface and self.interface.wndCollections:
				self.interface.wndCollections.SetIncrease(isIncreased)

	if app.ENABLE_PUNKTY_OSIAGNIEC:
		def OnPickPktOsiag(self, pkt_osiag):
			self.interface.OnPickPktOsiagNew(pkt_osiag)

	if app.ENABLE_CUBE_RENEWAL_WORLDARD:
		def BINARY_CUBE_RENEWAL_OPEN(self):
			if self.interface:
				self.interface.BINARY_CUBE_RENEWAL_OPEN()

	def BINARY_ServerCommand_Run(self, line):
		line = line.strip()
		# Handle "event" commands specially
		if line.startswith("event "):
			# Extract everything after "event "
			event_data = line[6:]  # Skip "event "
			self.__ProcessServerEvent(event_data)
			return True

		# Handle all other commands normally
		try:
			return self.serverCommander.Run(line)
		except RuntimeError, msg:
			dbg.TraceError(msg)
			return 0

	def __ProcessPreservedServerCommand(self):
		try:
			command = net.GetPreservedServerCommand()
			while command:
				print " __ProcessPreservedServerCommand", command
				if command.startswith("event "):
					event_data = command[6:]  # Skip "event "
					self.__ProcessServerEvent(event_data)
				else:
					self.serverCommander.Run(command)
				command = net.GetPreservedServerCommand()
		except RuntimeError, msg:
			dbg.TraceError(msg)
			return 0

	def CardsOpen(self):
		import uicardslottery
		self.a = uicardslottery.CardsLottery()
		self.a.Open()
  
	def CardsReward(self, id):
		self.a.Reward(id)

	def PartyHealReady(self):
		self.interface.PartyHealReady()

	def AskSafeboxPassword(self):
		self.interface.AskSafeboxPassword()

	def AskMallPassword(self):
		self.interface.AskMallPassword()

	def __ItemMall_Open(self):
		self.interface.OpenItemMall();

	def CommandCloseMall(self):
		self.interface.CommandCloseMall()



		

	if app.ENABLE_ODLAMKI_SYSTEM:
		def OpenOdlamki(self):
			self.interface.OpenOdlamkiWindow()

		def BINARY_OdlamkiItemRefreshWindow(self):
			self.interface.wndOdlamki.ClearWindow()

	if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
		def __SetBuffNPCSummon(self):
			self.interface.BuffNPC_Summon()
			
		def __SetBuffNPCUnsummon(self):
			self.interface.BuffNPC_Unsummon()
			
		def __SetBuffNPCClear(self):
			self.interface.BuffNPC_Clear()

		def __SetBuffNPCBasicInfo(self, name, sex, intvalue):
			self.interface.BuffNPC_SetBasicInfo(str(name), int(sex), int(intvalue))

		def __SetBuffNPCSkillInfo(self, skill1, skill2, skill3):
			self.interface.BuffNPC_SetSkillInfo(skill1, skill2, skill3)
			
		def __SetBuffNPCSkillSetSkillCooltime(self, slot, timevalue):
			self.interface.BuffNPC_SetSkillCooltime(slot, timevalue)
			
		def __SetBuffNPCCreatePopup(self, type, value0, value1):
			self.interface.BuffNPC_CreatePopup(int(type), int(value0), int(value1))
			
		def BINARY_OpenCreateBuffWindow(self):
			self.interface.BuffNPC_OpenCreateWindow()

	def RefineSuceededMessage(self):
		self.PopupMessage(localeInfo.REFINE_SUCCESS)
		if app.ENABLE_REFINE_RENEWAL:
			self.interface.CheckRefineDialog(False)

	def RefineFailedMessage(self):
		self.PopupMessage(localeInfo.REFINE_FAILURE)
		if app.ENABLE_REFINE_RENEWAL:
			self.interface.CheckRefineDialog(True)

	def CommandCloseSafebox(self):
		self.interface.CommandCloseSafebox()

	def __PrivateShop_PriceList(self, itemVNum, itemPrice):
		uiPrivateShopBuilder.SetPrivateShopItemPrice(itemVNum, itemPrice)

	def __Horse_HideState(self):
		self.affectShower.SetHorseState(0, 0, 0)

	def __Horse_UpdateState(self, level, health, battery):
		self.affectShower.SetHorseState(int(level), int(health), int(battery))

	def __IsXMasMap(self):
		mapDict = ( "metin2_map_n_flame_01",
					"metin2_map_n_desert_01",
					"metin2_map_spiderdungeon",
					"metin2_map_deviltower1", )

		if background.GetCurrentMapName() in mapDict:
			return False

		return True

	def __XMasSnow_Enable(self, mode):

		self.__XMasSong_Enable(mode)

		if "1"==mode:

			if not self.__IsXMasMap():
				return

			print "XMAS_SNOW ON"
			background.EnableSnow(1)

		else:
			print "XMAS_SNOW OFF"
			background.EnableSnow(0)

	def __XMasBoom_Enable(self, mode):
		if "1"==mode:

			if not self.__IsXMasMap():
				return

			print "XMAS_BOOM ON"
			self.__DayMode_Update("dark")
			self.enableXMasBoom = True
			self.startTimeXMasBoom = app.GetTime()
		else:
			print "XMAS_BOOM OFF"
			self.__DayMode_Update("light")
			self.enableXMasBoom = False

	def __XMasTree_Enable(self, grade):

		print "XMAS_TREE ", grade
		background.SetXMasTree(int(grade))

	def __XMasSong_Enable(self, mode):
		if "1"==mode:
			print "XMAS_SONG ON"

			XMAS_BGM = "xmas.mp3"

			if app.IsExistFile("content/sound/BGM/" + XMAS_BGM)==1:
				if musicInfo.fieldMusic != "":
					snd.FadeOutMusic("content/sound/BGM/" + musicInfo.fieldMusic)

				musicInfo.fieldMusic=XMAS_BGM
				snd.FadeInMusic("content/sound/BGM/" + musicInfo.fieldMusic)

		else:
			print "XMAS_SONG OFF"

			if musicInfo.fieldMusic != "":
				snd.FadeOutMusic("content/sound/BGM/" + musicInfo.fieldMusic)

			musicInfo.fieldMusic=musicInfo.METIN2THEMA
			snd.FadeInMusic("content/sound/BGM/" + musicInfo.fieldMusic)

	def __RestartDialog_Close(self):
		self.interface.CloseRestartDialog()

	def __Console_Enable(self):
		constInfo.CONSOLE_ENABLE = True
		self.consoleEnable = True
		app.EnableSpecialCameraMode()
		ui.EnablePaste(True)

	def __PrivateShop_Open(self):
		self.interface.OpenPrivateShopInputNameDialog()

	def BINARY_PrivateShop_Appear(self, vid, text):
		self.interface.AppearPrivateShop(vid, text)

	def BINARY_PrivateShop_Disappear(self, vid):
		self.interface.DisappearPrivateShop(vid)


	def __PRESERVE_DayMode_Update(self, mode):
		if "light"==mode:
			background.SetEnvironmentData(0)
		elif "dark"==mode:

			if not self.__IsXMasMap():
				return

			background.RegisterEnvironmentData(1, constInfo.ENVIRONMENT_NIGHT)
			background.SetEnvironmentData(1)

	def __DayMode_Update(self, mode):
		if "light"==mode:
			self.curtain.SAFE_FadeOut(self.__DayMode_OnCompleteChangeToLight)
		elif "dark"==mode:

			if not self.__IsXMasMap():
				return

			self.curtain.SAFE_FadeOut(self.__DayMode_OnCompleteChangeToDark)

	def __DayMode_OnCompleteChangeToLight(self):
		background.SetEnvironmentData(0)
		self.curtain.FadeIn()

	def __DayMode_OnCompleteChangeToDark(self):
		background.RegisterEnvironmentData(1, constInfo.ENVIRONMENT_NIGHT)
		background.SetEnvironmentData(1)
		self.curtain.FadeIn()

	def __XMasBoom_Update(self):

		self.BOOM_DATA_LIST = ( (2, 5), (5, 2), (7, 3), (10, 3), (20, 5) )
		if self.indexXMasBoom >= len(self.BOOM_DATA_LIST):
			return

		boomTime = self.BOOM_DATA_LIST[self.indexXMasBoom][0]
		boomCount = self.BOOM_DATA_LIST[self.indexXMasBoom][1]

		if app.GetTime() - self.startTimeXMasBoom > boomTime:

			self.indexXMasBoom += 1

			for i in xrange(boomCount):
				self.__XMasBoom_Boom()

	def __XMasBoom_Boom(self):
		x, y, z = player.GetMainCharacterPosition()
		randX = app.GetRandom(-150, 150)
		randY = app.GetRandom(-150, 150)

		snd.PlaySound3D(x+randX, -y+randY, z, "sound/common/etc/salute.mp3")

	def __PartyRequestQuestion(self, vid):
		vid = int(vid)
		partyRequestQuestionDialog = uiCommon.QuestionDialog()
		partyRequestQuestionDialog.SetText(chr.GetNameByVID(vid) + localeInfo.PARTY_DO_YOU_ACCEPT)
		partyRequestQuestionDialog.SetAcceptText(localeInfo.UI_ACCEPT)
		partyRequestQuestionDialog.SetCancelText(localeInfo.UI_DENY)
		partyRequestQuestionDialog.SetAcceptEvent(lambda arg=True: self.__AnswerPartyRequest(arg))
		partyRequestQuestionDialog.SetCancelEvent(lambda arg=False: self.__AnswerPartyRequest(arg))
		partyRequestQuestionDialog.Open()
		partyRequestQuestionDialog.vid = vid
		self.partyRequestQuestionDialog = partyRequestQuestionDialog

	def __AnswerPartyRequest(self, answer):
		if not self.partyRequestQuestionDialog:
			return

		vid = self.partyRequestQuestionDialog.vid

		if answer:
			net.SendChatPacket("/party_request_accept " + str(vid))
		else:
			net.SendChatPacket("/party_request_deny " + str(vid))

		self.partyRequestQuestionDialog.Close()
		self.partyRequestQuestionDialog = None

	def __PartyRequestDenied(self):
		self.PopupMessage(localeInfo.PARTY_REQUEST_DENIED)

	def __EnableTestServerFlag(self):
		app.EnableTestServerFlag()
		
	if app.BL_KILL_BAR:
		def AddKillInfo(self, killer, victim, killer_race, victim_race, weapon_type):
			if self.interface:
				self.interface.AddKillInfo(killer, victim, killer_race, victim_race, weapon_type)

	def __InGameShop_Show(self, url):
		if constInfo.IN_GAME_SHOP_ENABLE:
			self.interface.OpenWebWindow(url)

	def __LoginLover(self):
		if self.interface.wndMessenger:
			self.interface.wndMessenger.OnLoginLover()

	def __LogoutLover(self):
		if self.interface.wndMessenger:
			self.interface.wndMessenger.OnLogoutLover()
		if self.affectShower:
			self.affectShower.HideLoverState()

	def __LoverNear(self):
		if self.affectShower:
			self.affectShower.ShowLoverState()

	def __LoverFar(self):
		if self.affectShower:
			self.affectShower.HideLoverState()

	def __LoverDivorce(self):
		if self.interface.wndMessenger:
			self.interface.wndMessenger.ClearLoverInfo()
		if self.affectShower:
			self.affectShower.ClearLoverState()

	def __PlayMusic(self, flag, filename):
		flag = int(flag)
		if flag:
			snd.FadeOutAllMusic()
			musicInfo.SaveLastPlayFieldMusic()
			snd.FadeInMusic("content/sound/BGM/" + filename)
		else:
			snd.FadeOutAllMusic()
			musicInfo.LoadLastPlayFieldMusic()
			snd.FadeInMusic("content/sound/BGM/" + musicInfo.fieldMusic)

	if app.ENABLE_LOADING_PERFORMANCE:
		def OpenWarpShowerWindow(self):
			if self.interface:
				self.interface.OpenWarpShowerWindow()

		def CloseWarpShowerWindow(self):
			if self.interface:
				self.interface.CloseWarpShowerWindow()

	def BINARY_RemoveItemRefreshWindow(self):
		self.interface.wndRemoveItem.ClearWindow()

	if app.ENABLE_RESP_SYSTEM:
		def BINARY_SetMobRespData(self, mobVnum, data):
			self.interface.wndResp.SetMobRespData(mobVnum, data)

		def BINARY_SetMobDropData(self, mobVnum, data):
			self.interface.wndResp.SetMobDropData(mobVnum, data)

		def BINARY_SetMapData(self, data, currentBossCount, maxBossCount, currentMetinCount, maxMetinCount):
			self.interface.wndResp.SetMapData(data, currentBossCount, maxBossCount, currentMetinCount, maxMetinCount)
			# Request resp data for all bosses on this map (for atlas window)
			for vnum in data:
				net.SendRespFetchRespPacket(vnum)

		def BINARY_RefreshResp(self, id, mobVnum, time, cord):
			self.interface.wndResp.RefreshRest(id, mobVnum, time, cord)

	def SendTitleActiveNow(self, id):
		if self.interface:
			self.interface.ActiveTileNow(id)

	def SendTitleEnable(self, id, val):
		if self.interface:
			self.interface.TitleEnable(id, val)

	def SendOfflineShopLogs(self, id, item, count, price, price2, date, action):
		if self.interface:
			self.interface.OfflineShopLogs(id, item, count, price, price2, date, action)

	def SendAntyExp(self, value):
		constInfo.ANTY_EXP_STATUS = value

	def SendWpadanie(self, value):
		constInfo.WPADANIE_DODATKOWE = value

	if app.WJ_SPLIT_INVENTORY_SYSTEM:
		def __PressExtendedInventory(self):
			if self.interface:
				self.interface.ToggleExtendedInventoryWindow()

	def SendWeeklyPage(self, page, active, season):
		if active == True:
			if self.interface:
				self.interface.SelectPage(page, season)

	def SendWeeklyInfo(self, pos, name, points, empire, job):
		if self.interface:
			self.interface.SendWeeklyInfo(pos, name, points, empire, job)

	if app.ENABLE_DROP_INFO:
		if app.ENABLE_DROP_INFO_PCT:
			def BINARY_DropInfoAppendItem(self, mob_vnum, vnum, min_count, max_count, percentage):
				if not constInfo.dropInfoDict.has_key(mob_vnum):
					constInfo.dropInfoDict[mob_vnum] = []

				constInfo.dropInfoDict[mob_vnum].append({"vnum": [vnum], "min_count": min_count, "max_count": max_count, "percentage": percentage})
		else:
			def BINARY_DropInfoAppendItem(self, mob_vnum, vnum, min_count, max_count):
				if not constInfo.dropInfoDict.has_key(mob_vnum):
					constInfo.dropInfoDict[mob_vnum] = []

				constInfo.dropInfoDict[mob_vnum].append({"vnum": [vnum], "min_count": min_count, "max_count": max_count})
				
		def BINARY_DropInfoRefresh(self, mob_vnum):
			self.targetBoard.DropInfoRefresh(mob_vnum)

		def DropInfoRefresh(self):
			constInfo.dropInfoDict = {}
			self.targetBoard.DropInfoClear()

	if app.ENABLE_ACCE_COSTUME_SYSTEM:
		def ActAcce(self, iAct, bWindow):
			if self.interface:
				self.interface.ActAcce(iAct, bWindow)

		def AlertAcce(self, bWindow):
			snd.PlaySound("sound/ui/make_soket.wav")

		def SendAcceMaterials(self, id, vnum, count):
			if self.interface:
				self.interface.AcceMaterials(id, vnum, count)

	if app.ENABLE_AURA_SYSTEM:
		def ActAura(self, iAct, bWindow):
			if self.interface:
				self.interface.ActAura(iAct, bWindow)

		def AlertAura(self, bWindow):
			snd.PlaySound("sound/ui/make_soket.wav")

	if app.BL_MOVE_CHANNEL:
		def __SeverInfo(self, channelNumber, mapIndex):

			_chNum	= int(channelNumber.strip())
			_mapIdx	= int(mapIndex.strip())

			if _chNum == 99 or _mapIdx >= 10000:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_CHANNEL_NOTICE % 0)
			else:
				if constInfo.PREV_CHANNEL and constInfo.PREV_CHANNEL != _chNum:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_CHANNEL_SWITCHED % (constInfo.PREV_CHANNEL, _chNum))
					constInfo.PREV_CHANNEL = 0
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_CHANNEL_NOTICE % _chNum)
				
			net.SetChannelName(_chNum)
			net.SetMapIndex(_mapIdx)
			self.interface.RefreshServerInfo(channelNumber)
	
	def GetInputOn(self):
		constInfo.INPUT_IGNORE = 1
		
	def GetInputOff(self):
		constInfo.INPUT_IGNORE = 0

	if app.ENABLE_NEW_PET_SYSTEM:	
		def __pet_info(self, *arg):
			try:
				full_arg = " ".join(arg) if isinstance(arg, (tuple, list)) else arg
				dat = full_arg.split("|")
				if dat[0] == "clear":
					constInfo.gui_pets["RACE"] = 0
					constInfo.gui_pets["NAME"] = ""
					return
				elif dat[0] == "state":
					constInfo.gui_pets["EXP"] = int(dat[1])
					constInfo.gui_pets["POINTS"] = int(dat[2])
					return

				if len(dat) < 6:
					return

				constInfo.gui_pets["NAME"] = str(dat[0].replace("+", " "))
				constInfo.gui_pets["RACE"] = int(dat[1])
				constInfo.gui_pets["LEVEL"] = int(dat[2])
				constInfo.gui_pets["EXP"] = int(dat[3])
				constInfo.gui_pets["EXP_NEED"] = int(dat[4])
				constInfo.gui_pets["POINTS"] = int(dat[5])
				
				if len(dat) >= 7:
					skill_data = dat[6].split("#")
					constInfo.gui_pets["SKILLS"] = []
					for skill in skill_data:
						if not len(skill):
							continue
					
						tmp = {}
						t = skill.split("&")
						if len(t) < 2:
							continue
							
						tmp["lv"] = int(t[0])
						tmp["affect"] = []
						for s in t[1].split("@"):
							tmp["affect"].append(s)
						
						constInfo.gui_pets["SKILLS"].append(tmp)
						
				self.interface.pet_refresh()
			except Exception as e:
				pass

		def __pet_info_update(self, arg):
			try:
				constInfo.gui_pets["ID"] = int(arg)
			except:
				pass

	if app.ENABLE_SWITCHBOT:
		def RefreshSwitchbotWindow(self):
			if self.interface:
				self.interface.RefreshSwitchbotWindow()
			
		def RefreshSwitchbotItem(self, slot):
			if self.interface:
				self.interface.RefreshSwitchbotItem(slot)

	def PotionsbalcudiaF(self):
		self.interface.wndBlend.OpenWindow()
		
	if app.ENABLE_MINIMAP_DUNGEONINFO:
		def BINARY_SetMiniMapDungeonInfoState(self, state):
			if self.interface:
				self.interface.SetMiniMapDungeonInfo(state)
		
		def BINARY_SetMiniMapDungeonInfoStage(self, cur_stage, max_stage):
			if self.interface:
				self.interface.SetMiniMapDungeonInfoStage(cur_stage, max_stage)
		
		def BINARY_SetMiniMapDungeonInfoGauge(self, gauge_type, value1, value2):
			if self.interface:
				self.interface.SetMiniMapDungeonInfoGauge(gauge_type, value1, value2)
		
		def BINARY_SetMiniMapDungeonInfoNotice(self, notice):
			if self.interface:
				self.interface.SetMiniMapDungeonInfoNotice(notice)

		def BINARY_SetMiniMapDungeonInfoButton(self, status):
			if self.interface:
				self.interface.SetMiniMapDungeonInfoButton(status)

		def BINARY_SetMiniMapDungeonInfoTimer(self, status, time):
			if self.interface:
				self.interface.SetMiniMapDungeonInfoTimer(status, time)

	if app.ENABLE_COLLECT_WINDOW:
		def BINARY_UpdateCollectWindow(self, windowType, time, count, itemVnum, countTotal, chance, rendertargetvnum, questindex, requiredlevel):
			if self.interface.wndCollectWindow:
				self.interface.wndCollectWindow.AddData(windowType, time, count, itemVnum, countTotal, chance, rendertargetvnum, questindex, requiredlevel)
				
		def UpdateTime(self, val, time):
			if self.interface.wndCollectWindow:
				self.interface.wndCollectWindow.SendTime(val, time)

		def UpdateChance(self, val, chance):
			if self.interface.wndCollectWindow:
				self.interface.wndCollectWindow.SendChance(val, chance)

		def __SetCollectWindowQID(self, window, value):
			constInfo.CollectWindowQID[int(window)] = int(value)

		def OpenCollectWindow(self):
			self.interface.ToggleCollectWindow()

		def __SetMissionNotifications(self, value):
			if self.interface.wndCollectWindow:
				self.interface.wndCollectWindow.SetNotificationState(int(value))
		
	if constInfo.GIFT_CODE_SYSTEM:
		def __OpenCodeWindow(self):
			self.interface.OpenGiftCodeWindow()

	if app.ENABLE_EVENT_MANAGER:
		def ClearEventManager(self):
			self.interface.ClearEventManager()
		def RefreshEventManager(self):
			self.interface.RefreshEventManager()
		def RefreshEventStatus(self, eventID, eventStatus, eventendTime, eventEndTimeText):
			self.interface.RefreshEventStatus(int(eventID), int(eventStatus), int(eventendTime), str(eventEndTimeText))
		def AppendEvent(self, dayIndex, eventID, eventIndex, startTime, endTime, empireFlag, channelFlag, value0, value1, value2, value3, startRealTime, endRealTime, isAlreadyStart):
			self.interface.AppendEvent(int(dayIndex),int(eventID), int(eventIndex), str(startTime), str(endTime), int(empireFlag), int(channelFlag), int(value0), int(value1), int(value2), int(value3), int(startRealTime), int(endRealTime), int(isAlreadyStart))
		def ClearPersonalActivatedEvents(self):
			self.interface.ClearPersonalActivatedEvents()
		def SetPersonalEventActivated(self, eventID, activationTime=0):
			self.interface.SetPersonalEventActivated(int(eventID), int(activationTime))
		def EventActivateResult(self, eventID, result):
			self.interface.EventActivateResult(int(eventID), int(result))

	if app.TAKE_LEGEND_DAMAGE_BOARD_SYSTEM:
		def SendLegendDamageData(self, vid, pos, name, level, race, empire, damage):
			if self.interface:
				self.interface.SendLegendDamageData(vid, pos, name, level, race, empire, damage)

	if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
		def OpenPrivateShopPanel(self):
			if self.interface:
				self.interface.OpenPrivateShopPanel()
			
		def ClosePrivateShopPanel(self):
			if self.interface:
				self.interface.ClosePrivateShopPanel()
			
		def RefreshPrivateShopWindow(self):
			if self.interface:
				self.interface.RefreshPrivateShopWindow()
			
		def OpenPrivateShopSearch(self, mode):
			self.interface.OpenPrivateShopSearch(mode)
			
		def PrivateShopSearchRefresh(self):
			if self.interface:
				self.interface.PrivateShopSearchRefresh()

		def PrivateShopSearchUpdate(self, index, state):
			if self.interface:
				self.interface.PrivateShopSearchUpdate(index, state)
			
		def AppendMarketItemPrice(self, gold, cheque):
			if self.interface:
				self.interface.AppendMarketItemPrice(gold, cheque)

		def AddPrivateShopTitleBoard(self, vid, text, type):
			if self.interface:
				self.interface.AddPrivateShopTitleBoard(vid, text, type)

		def RemovePrivateShopTitleBoard(self, vid):
			if self.interface:
				self.interface.RemovePrivateShopTitleBoard(vid)

		def SetPrivateShopPremiumBuild(self):
			if self.interface:
				self.interface.SetPrivateShopPremiumBuild()
				
		def PrivateShopStateUpdate(self):
			if self.interface:
				self.interface.PrivateShopStateUpdate()

	def BINARY_BattlePassInit(self):
		self.interface.wndBattlePass.RefreshGlobal()

	def BINARY_BattlePassUpdate(self):
		self.interface.wndBattlePass.RefreshLocal()
		
	if app.__ENABLE_POLYMORPH_SYSTEM__:
		def BINARY_PolySystem(self, polyStages, polySkins):
			self.interface.wndPolySystem.AppendStages(polyStages)
			self.interface.wndPolySystem.AppendSkins(polySkins)
			self.interface.wndPolySystem.Refresh()

		def BINARY_PolySkinOpen(self):
			self.interface.wndPolySystem.Open()

		def BINARY_PolySkinRefresh(self):
			self.interface.wndPolySystem.RefreshSkins()

		def BINARY_PolySkinForceClose(self):
			self.interface.wndPolySystem.CloseByServer()
		
	def OnCubeOpen(self, recipes):
		self.interface.OpenCubeWindow()
		
		cube = self.interface.GetCubeWindow()
		for recipe in recipes:
			cube.AddRecipe(uiCube.Recipe.CreateFromServer(recipe))
		
		cube.Refresh()
		

	def __Inputget1(self):
		constInfo.INPUT_IGNORE = 1 
	def __Inputget2(self):
		constInfo.INPUT_IGNORE = 0
	
	def OnCubeUpdate(self, gold, priceCheque, priceAchievement, chance):
		self.interface.GetCubeWindow().OnUpdateInfo(gold, priceCheque, priceAchievement, chance)
	
	def OnCubeClose(self):
		self.interface.CloseCubeWindow()

	def __RecvCasketItem(self, base_vnum, vnum, count):
		if self.interface and self.interface.wndCasketPreview:
			self.interface.wndCasketPreview.AddItem(int(base_vnum), int(vnum), int(count))

	def __RecvCasketBuild(self):
		if self.interface and self.interface.wndCasketPreview:
			self.interface.wndCasketPreview.Build()

	def EventItemQuickSlot(self, vnum):
		if self.interface:
			if self.interface.IsTeleportItem(vnum):
				self.interface.OpenDungeonInfo()
				
	if app.TEAM_MEMBER_STATUS:
		def __BINARY__UpdateTeamMemberStatus(self, sName, sLang, bStatus):
			print "Team member status: Name:", sName, "lang:", sLang, "status:", bStatus
			self.interface.wndMessenger.CheckoutTeamMemberStatus(sName, sLang, int(bStatus))

		def __BINARY__ClearTeamMemberList(self):
			print "Team member clear"
			self.interface.wndMessenger.ClearTeamMemberList()
			

	if app.ENABLE_SAVE_LOCATION_SYSTEM:
		def BINARY_UpdateSaveLocation(self, pos, name, x, y):
			self.interface.UpdateSaveLocation(pos, name, x, y)

		def BINARY_DelSaveLocation(self, pos):
			self.interface.DeleteSaveLocation(pos)

	if app.ENABLE_OFFLINE_SHOP:
		def OpenPrivateShopBuilder(self):
			if self.interface.wndShopOffline:
				if self.interface.wndShopOffline.IsShow() == False or self.interface.wndShopOffline.wBoard[3].IsShow():
					self.interface.wndShopOffline.Open()
				else:
					self.interface.wndShopOffline.Close()
	
		def OpenShopSearch(self):
			if self.interface.wndShopSearch:
				if not self.interface.wndShopSearch.IsShow():
					self.interface.wndShopSearch.Show()
				else:
					self.interface.wndShopSearch.Close()

	def OpenPrivateMessage(self, name):
		self.interface.OpenWhisperDialog(name)


	if app.ENABLE_BOT_CONTROL:
		def BINARY_BotControlOpen(self):
			self.botControlWnd = uisecurity.FindTheDifferentCaptcha()
			if self.botControlWnd:
				self.botControlWnd.Open()
	
		def BINARY_BotControlTimeUpdate(self):
			botcontroltime = app.GetRandom(1200, 1800)
			constInfo.BOTCONTROL_TIME = app.GetTime() + botcontroltime
	
		def BINARY_BotControlFail(self):
			self.EndExchange()
			self.interface.CommandCloseSafebox()
			net.SendChatPacket("/bot31_control_fail")
	
		def isBotControlActive(self):
			if self.botControlWnd:
				return self.botControlWnd.IsShow()
	
			return False
		

	def ServerRankLoadInfo(self, info):
		self.rankingWindow.RecvLoadInfo(info)

	def ServerRankLoadMyPos(self, catIdx, position, value):
		self.rankingWindow.RecvLoadMyPosition(int(position), int(value))

	def ServerRankLoadData(self, pName, level, raceWithskillGroup, empire, value):
		self.rankingWindow.RecvLoadData(pName, level, raceWithskillGroup, empire, value)

	def RankLoadInfo(self, info, sundayEndTime):
		self.weeklyRankingWindow.RecvLoadInfo(info, int(sundayEndTime))

	def RankLoadMyPos(self, catIdx, position, value):
		self.weeklyRankingWindow.RecvLoadMyPosition(int(position), int(value))

	def RankLoadData(self, pName, level, race, value):
		chat.AppendChat(1, "pName = %s, level = %d, race = %s, value = %d" % (pName, level, race, value))
		self.weeklyRankingWindow.RecvLoadData(pName, level, race, value)
			
	if app.ENABLE_SECONDARY_LEVEL:
		def __SecondaryLevel_Open(self):
			self.interface.ToggleSecondaryLevel()

	def EnlightenmentWindowOpen(self):
		if self.enlightWindow == None:
			self.enlightWindow = uiEnlightenment.EnlightenmentWindow()
		self.enlightWindow.OpenWindow()

	def EnlightenmentWindowRefresh(self):
		if self.enlightWindow == None:
			self.enlightWindow = uiEnlightenment.EnlightenmentWindow()
		self.enlightWindow.RefreshWindow()

	def EnlightenmentUpgradeNotice(self, name, level):
		if self.enlightWindow == None:
			self.enlightWindow = uiEnlightenment.EnlightenmentWindow()
		self.enlightWindow.OpenNoticeUpgradeBoard(name, int(level))
	
	def SetMyOreCount(self, count):
		constInfo.MY_ORE_COUNT = int(count)

	def SetMyFishCount(self, count):
		constInfo.MY_FISH_COUNT = int(count)

	def SetTitleBossCount(self, count):
		constInfo.TITLE_BOSS_COUNT = int(count)

	def SetTitleStonesCount(self, count):
		constInfo.TITLE_STONE_COUNT = int(count)
		
	def SetTitleRareCount(self, count):
		constInfo.TITLE_RARE_COUNT = int(count)
		
	def SetTitleRelictCount(self, count):
		constInfo.TITLE_RELICT_COUNT = int(count)
		
	def SetTitleDungeonFastestTime(self, count):
		constInfo.TITLE_DUNGEON_FASTEST_TIME = int(count)
		
	def SetTitleDungeonBossKilled(self, count):
		constInfo.TITLE_DUNGEON_DUNGEON_BOSS_KILLED = int(count)

	def SetTitleEnchantCount(self, count):
		constInfo.TITLE_ENCHANT_COUNT = int(count)

	def SetTitleRefineSuccessCount(self, count):
		constInfo.TITLE_REFINE_SUCCESS_COUNT = int(count)

	def SetTitleRefineAlchemyCount(self, count):
		constInfo.TITLE_REFINED_ALCHEMY_COUNT = int(count)

	def SetTitleRefineFailCount(self, count):
		constInfo.TITLE_REFINE_FAIL_COUNT = int(count)

	def SetTitlePlaytimeCount(self, count):
		constInfo.TITLE_PLAYTIME_COUNT = int(count)

	def SetTitleFastestDungeon(self, count):
		constInfo.TITLE_FASTEST_DUNGEON = int(count)

	def SetTitleDieToMonster(self, count):
		constInfo.TITLE_DIE_TO_MONSTER = int(count)

	def SetTitleDungeonNoDamage(self, count):
		constInfo.TITLE_DUNGEON_NO_DAMAGE = int(count)

	def SetMyOreFishCount(self, countOre, countFish):
		constInfo.MY_ORE_COUNT = int(countOre)
		constInfo.MY_FISH_COUNT = int(countFish)
		
	def FishWiki_NewFishUnlocked(self, fishVnum):
		self.fishWikiWindow.OnUnlockNewFish(int(fishVnum))

	def FishWiki_UnlockStatus(self, unlockStatus1, unlockStatus2):
		self.fishWikiWindow.OnLoadUnlockStatus(int(unlockStatus1), int(unlockStatus2))

	def FishWiki_FishData(self, fishVnum, myGetCount, bestLength, bestPrice, missionGivedCount):
		self.fishWikiWindow.OnLoadMyFishData(int(fishVnum), int(myGetCount), int(bestLength), int(bestPrice), int(missionGivedCount))

	def FishWiki_RankLoadStart(self):
		self.fishWikiWindow.OnLoadRankingStart()

	def FishWiki_RankLoadData(self, pos, name, length, rodLevel):
		self.fishWikiWindow.OnLoadRanking(int(pos), name, int(length), int(rodLevel))

	def FishWiki_RankLoadEnd(self):
		self.fishWikiWindow.OnLoadRankingEnd()

	def FishWiki_LoadError(self, status=""):
		self.fishWikiWindow.OnLoadError(status)	
		
	if app.ENABLE_NOTIFICATION_SYSTEM:
		def BINARY_SendNotification(self, category, value, additionalValue):
			if self.interface:
				self.interface.SendNotification(category, value, additionalValue)
		
	if app.WORLD_BOSS_YUMA:				
		def WorldbossNotficiation(self, szString):
			if self.interface:
				szString = szString.replace("_", " ")
				self.interface.WorldbossNotification(szString)

	if app.ENABLE_MOUNT_SYSTEM:
		def BINARY_SetMountInfo(self, vnum, name, level, exp, req_exp, summoned):
			self.interface.wndCompanionWindow.SetBasicInfo(vnum, name, level, exp, req_exp, summoned)
			constInfo.MOUNT_LEVEL = level
			
	if app.ENABLE_IN_GAME_LOG_SYSTEM:
		def BINARY_IN_GAME_LOG_OPEN(self,page):
			self.wndInGameLog.ChangePage(page)
	
		def OpenInGameLog(self):
			if not self.wndInGameLog:
				self.wndInGameLog = uiingamelog.IngameLogWindow()
	
			if self.wndInGameLog.IsShow():
				self.wndInGameLog.Close()
			else:
				self.wndInGameLog.Open()
	
		def BINARY_IN_GAME_LOG_TRADE_DETAILS(self,logID):
			self.wndInGameLog.UpdateLog(logID)
			
	if app.ENABLE_PVP_RANKING:
		def PVP_ClearPlayerInfo(self):
			self.interface.PVP_ClearPlayerInfo()
		if app.ENABLE_PVP_RANKING_WITH_EMPIRE:
			def PVP_AddPlayer(self, chName, chLevel, chRace, chEmpire, chKill, chDead, maxKill, IsRankEmpire, empire1, empire2, empire3):
				self.interface.PVP_AddPlayerInfo(chName, chLevel, chRace, chEmpire, chKill, chDead, maxKill, IsRankEmpire, empire1, empire2, empire3)
		else:
			def PVP_AddPlayer(self, chName, chLevel, chRace, chEmpire, chKill, chDead, maxKill):
				self.interface.PVP_AddPlayerInfo(chName, chLevel, chRace, chEmpire, chKill, chDead, maxKill)
		def PVP_Refresh(self):
			self.interface.PVP_RefreshWindow()
		def PVP_Empty(self):
			self.interface.PVP_EmptyWindow()
		def PVP_CloseWindow(self):
			self.interface.PVP_CloseWindow()
			
	if app.ENABLE_TITLE_ACHIEVEMENT_SYSTEM:
		def BINARY_ClearTitleAchievementList(self):
			"""Clear all title data - called when receiving new title data from server"""
			if self.interface.wndTitleAchievement:
				self.interface.wndTitleAchievement.BINARY_ClearTitleAchievementList()
		
		def BINARY_SetTitleUnlockStatus(self, titleIdx, unlocked):
			"""Set whether a specific title is unlocked or not"""
			if self.interface.wndTitleAchievement:
				self.interface.wndTitleAchievement.BINARY_SetTitleUnlockStatus(titleIdx, unlocked)
		
		def BINARY_SetCurrentActiveTitle(self, titleIdx):
			"""Set the currently active PRIMARY title"""
			if self.interface.wndTitleAchievement:
				self.interface.wndTitleAchievement.BINARY_SetCurrentActiveTitle(titleIdx)
		
		def BINARY_SetCurrentActiveTitlePremium(self, titleIdx):
			"""NEW: Set the currently active PREMIUM title (second slot)"""
			if self.interface.wndTitleAchievement:
				self.interface.wndTitleAchievement.BINARY_SetCurrentActiveTitlePremium(titleIdx)
		
		def BINARY_SetTitleEffect(self, effectIdx, affType, affValue):
			"""Set bonus effects for the PRIMARY title"""
			if self.interface.wndTitleAchievement:
				self.interface.wndTitleAchievement.BINARY_SetTitleEffect(effectIdx, affType, affValue)
		
		def BINARY_SetTitleEffectPremium(self, effectIdx, affType, affValue):
			"""NEW: Set bonus effects for the PREMIUM title"""
			if self.interface.wndTitleAchievement:
				self.interface.wndTitleAchievement.BINARY_SetTitleEffectPremium(effectIdx, affType, affValue)
		
		def BINARY_FinalizeTitleLoading(self):
			"""Called when all title data has been received - triggers UI update"""
			if self.interface.wndTitleAchievement:
				self.interface.wndTitleAchievement.BINARY_FinalizeTitleLoading()
			
	def ToggleMapWindow(self):
		self.interface.interfaceWindowList["teleport"].toggle()
		
	def __CrystalExchange_Open(self):
		self.interface.CrystalExchange_Open()
		
	def BINARY_CrystalExchangeOpen(self, *args):
		if len(args) >= 6:
			crystalCounts = list(args[:6])
			self.interface.CrystalExchange_SetCrystalCounts(crystalCounts)
			self.interface.CrystalExchange_Open()
	
	def BINARY_CrystalExchangeResult(self, result, itemVnum, itemsUsed, crystalsGained, crystalType, subheader):
		if result == 0:  # Success
			chat.AppendChat(chat.CHAT_TYPE_INFO, 
						"Successfully exchanged %d items for %d crystals!" % (itemsUsed, crystalsGained))
			self.interface.CrystalExchange_SucceedExchange()
		elif result == 1:  # General failure
			chat.AppendChat(chat.CHAT_TYPE_INFO, "Exchange failed!")
		elif result == 2:  # Insufficient items
			chat.AppendChat(chat.CHAT_TYPE_INFO, "You need %d items to exchange!" % itemsUsed)
		elif result == 3:  # Invalid item
			chat.AppendChat(chat.CHAT_TYPE_INFO, "This item cannot be exchanged!")
	
	def BINARY_CrystalExchangeRefresh(self, *args):
		if len(args) >= 6:
			crystalCounts = list(args[:6])
			self.interface.CrystalExchange_SetCrystalCounts(crystalCounts)
			self.interface.CrystalExchange_Refresh()
	
	def BINARY_CrystalExchangeRateUpdate(self, updateTime):
		chat.AppendChat(chat.CHAT_TYPE_INFO, "Crystal exchange rates updated!")
		self.interface.CrystalExchange_RateUpdate()

	def BINARY_Guild_NoticeDungeonOpen(self, dungeonIdx, isShowInfo):
		self.interface.OnRecvGuildOpenedDungeonIdx(dungeonIdx, isShowInfo)

	def BINARY_Guild_LogsRefresh(self):
		self.interface.OnRecvGuildRefreshLogs()
		
	if app.ENABLE_VOTE4BUFF:
		def __Vote4BuffServerData(self, data):
			constInfo.VOTE4BUFF_DATA["AVAILABLE_BONUSES"] = []
			constInfo.VOTE4BUFF_DATA["ITEM_REWARDS"] = []

			item_rewards, bonuses, change_bonus_price = data.split("#")

			constInfo.VOTE4BUFF_DATA["CHANGE_BONUS_PRICE"] = max(0, int(change_bonus_price))

			if item_rewards:
				for d1 in item_rewards.split("|"):
					constInfo.VOTE4BUFF_DATA["ITEM_REWARDS"].append(tuple(map(int, d1.split(":"))))

			if bonuses:
				for d1 in bonuses.split("|"):
					constInfo.VOTE4BUFF_DATA["AVAILABLE_BONUSES"].append([tuple(map(int, d2.split(":"))) for d2 in d1.split(",")])

		def __Vote4BuffVoteApi(self, api_url):
			constInfo.VOTE4BUFF_DATA["API_URL"] = api_url

		def __Vote4BuffVoteURL(self, url):
			constInfo.VOTE4BUFF_DATA["VOTE_URL"] = url

		def __Vote4Buff(self, data):
			self.interface.RefreshVote4BuffData(data)

	if app.ENABLE_ITEMSHOP:
		def ItemShopClear(self, updateTime):
			uiItemShopNew.ItemShopClear(int(updateTime))

		#USE_ITEMSHOP_RENEWED: @itemPriceJD
		def ItemShopUpdateItem(self, itemID, itemVnum, itemPrice, itemDiscount, itemOffertime, itemTopSelling, itemAddedTime, itemSellingCount, itemMaxSellingCount, itemPriceJD = 0):
			uiItemShopNew.ItemShopUpdateItem(int(itemID), int(itemVnum), long(itemPrice), int(itemDiscount), int(itemOffertime), int(itemTopSelling), int(itemAddedTime), long(itemSellingCount), int(itemMaxSellingCount), long(itemPriceJD))

			if self.interface:
				self.interface.ItemShopUpdateItem(int(itemID), int(itemMaxSellingCount))

		#USE_ITEMSHOP_RENEWED: @itemPriceJD
		def ItemShopAppendItem(self, categoryIndex, categorySubIndex, itemID, itemVnum, itemPrice, itemDiscount, itemOffertime, itemTopSelling, itemAddedTime, itemSellingCount, itemMaxSellingCount, itemPriceJD = 0):
			uiItemShopNew.ItemShopAppendItem(int(categoryIndex), int(categorySubIndex), int(itemID), int(itemVnum), long(itemPrice), int(itemDiscount), int(itemOffertime), int(itemTopSelling), int(itemAddedTime), long(itemSellingCount), int(itemMaxSellingCount), long(itemPriceJD))

		def ItemShopHideLoading(self):
			self.interface.ItemShopHideLoading()

		def ItemShopOpenMainPage(self):
			self.interface.OpenItemShopMainWindow()

		def ItemShopLogClear(self):
			uiItemShopNew.ItemShopLogClear()

		#USE_ITEMSHOP_RENEWED: @itemPriceJD
		def ItemShopAppendLog(self, dateText, dateTime, playerName, ipAdress, itemVnum, itemCount, itemPrice, itemPriceJD = 0):
			uiItemShopNew.ItemShopAppendLog(str(dateText), int(dateTime), str(playerName), str(ipAdress), int(itemVnum), int(itemCount), long(itemPrice))

		def ItemShopPurchasesWindow(self):
			self.interface.ItemShopPurchasesWindow()

		def ItemShopAppendLogEx(self, dateText, dateText2,dateTime, playerName, ipAdress, itemVnum, itemCount, itemPrice):
			uiItemShopNew.ItemShopAppendLog(str(dateText)+" "+str(dateText2), int(dateTime), str(playerName), str(ipAdress), int(itemVnum), int(itemCount), long(itemPrice))

		def BINARY_OpenItemShop(self):
			if self.interface:
				self.interface.OpenItemShopWindow()

		#USE_ITEMSHOP_RENEWED: @lldJCoins
		def ItemShopSetDragonCoin(self, lldCoins, lldJCoins = 0):
			if app.ENABLE_ITEMSHOP and self.interface:
				self.interface.ItemShopSetDragonCoin(long(lldCoins), long(lldJCoins))
		
	if app.ENABLE_MULTI_FARM_BLOCK:
		def UpdateMultiFarmPlayer(self, multiFarmPlayer):
			self.affectShower.SetMultiFarmPlayer(str(multiFarmPlayer))
		def UpdateMultiFarmAffect(self, multiFarmStatus, isNewStatus):
			self.affectShower.SetMultiFarmInfo(int(multiFarmStatus))
			if int(isNewStatus) == 1:
				if int(multiFarmStatus) == 1:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MULTI_FARM_ACTIVE_CHAT)
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MULTI_FARM_DEACTIVE_CHAT)
			app.SetMultiFarmExeIcon(int(multiFarmStatus))
			
	def __ProcessWorldBossRegisterDay(self, args):
		import dbg
		dbg.TraceError("[CMD] WORLD_BOSS_REGISTER_DAY received with %d args: %s" % (len(args), str(args)))
		
		if len(args) < 4:
			dbg.TraceError("[CMD] Not enough arguments!")
			return
		try:
			day_index = int(args[0])
			vnum = int(args[1])
			is_alive = int(args[2])
			next_spawn_time = int(args[3])
			
			dbg.TraceError("[CMD] Parsed: day=%d, vnum=%d, alive=%d, time=%d" % 
				(day_index, vnum, is_alive, next_spawn_time))
			
			import eventManager
			dbg.TraceError("[CMD] Sending event to EventManager")
			eventManager.EventManager().send_event("WORLD_BOSS_REGISTER_DAY", 
				day_index, vnum, is_alive, next_spawn_time)
			dbg.TraceError("[CMD] Event sent successfully")
		except Exception as e:
			import dbg
			import traceback
			dbg.TraceError("Error processing WORLD_BOSS_REGISTER_DAY: %s" % str(e))
			dbg.TraceError(traceback.format_exc())
	
	def __ProcessWorldBossRegisterRanking(self, args):
		if len(args) < 6:
			return
		try:
			day_index = int(args[0])
			vnum = int(args[1])
			rank = int(args[2])
			name = str(args[3])
			damage = int(args[4])
			timestamp = int(args[5])
			
			import eventManager
			eventManager.EventManager().send_event("WORLD_BOSS_REGISTER_RANKING",
				day_index, vnum, rank, name, damage, timestamp)
		except Exception as e:
			import dbg
			dbg.TraceError("Error processing WORLD_BOSS_REGISTER_RANKING: %s" % str(e))
	
	def __ProcessWorldBossRegisterPersonal(self, args):
		if len(args) < 5:
			return
		try:
			day_index = int(args[0])
			vnum = int(args[1])
			name = str(args[2])
			damage = int(args[3])
			timestamp = int(args[4])
			
			import eventManager
			eventManager.EventManager().send_event("WORLD_BOSS_REGISTER_PERSONAL",
				day_index, vnum, name, damage, timestamp)
		except Exception as e:
			import dbg
			dbg.TraceError("Error processing WORLD_BOSS_REGISTER_PERSONAL: %s" % str(e))
		
	def __ProcessServerEventFromCommander(self, args):
		line = " ".join(str(a) for a in args)
		self.__ProcessServerEvent(line)

	def __ProcessServerEvent(self, line=""):
		"""
		Process events received from the server and dispatch them to the EventManager.
		"""
		import dbg
		#dbg.TraceError("[ProcessServerEvent] Called with line: '%s'" % line)
		
		if not line:
			#dbg.TraceError("[ProcessServerEvent] Empty line received!")
			return False
		
		try:
			# Split the line into tokens
			tokens = line.split()
			if not tokens:
				return False
			
			event_name = tokens[0]
			args = tokens[1:]
			
			#dbg.TraceError("[ProcessServerEvent] Event: %s, Args: %s" % (event_name, str(args)))
			
			positional_args = []
			keyword_args = {}
			
			# Parse and convert arguments from strings to appropriate Python types
			for arg in args:
				try:
					if "=" in arg:
						# Handle keyword arguments (key=value format)
						key, value = arg.split("=", 1)
						keyword_args[key] = self.__ConvertArgValue(value)
					else:
						# Handle positional arguments
						converted = self.__ConvertArgValue(arg)
						positional_args.append(converted)
						#dbg.TraceError("[ProcessServerEvent] Converted: %s -> %s (%s)" % 
						#	(arg, str(converted), type(converted).__name__))
				except Exception as e:
					#dbg.TraceError("Error parsing argument '%s': %s" % (arg, str(e)))
					continue
			
			#dbg.TraceError("[ProcessServerEvent] Sending to EventManager: %s with %d args" % 
			#	(event_name, len(positional_args)))
			
			# Import event manager and dispatch the event
			import eventManager 
			eventManager.EventManager().send_event(event_name, *positional_args, **keyword_args)
			
			#dbg.TraceError("[ProcessServerEvent] Event dispatched successfully")
			return True
			
		except Exception as e:
			import traceback
			error_msg = traceback.format_exc()
			dbg.TraceError("Error processing server event: %s" % error_msg)
			return False

	def __ConvertArgValue(self, value_str):
		"""
		Convert a string value to an appropriate Python type.
		
		Args:
			value_str (str): The string value to convert
			
		Returns:
			The converted value (int, float, bool, or str)
		"""
		# Handle None/null values
		if value_str.lower() in ("none", "null"):
			return None
			
		# Handle boolean values
		if value_str.lower() == "true":
			return True
		if value_str.lower() == "false":
			return False
		
		# Handle numeric values
		try:
			# Try to convert to integer
			if value_str.isdigit() or (value_str[0] == '-' and value_str[1:].isdigit()):
				return int(value_str)
			
			# Try to convert to float
			if '.' in value_str:
				# Ensure there's only one decimal point and all other chars are digits
				parts = value_str.split('.')
				if len(parts) == 2 and (parts[0].isdigit() or (parts[0][0] == '-' and parts[0][1:].isdigit())) and parts[1].isdigit():
					return float(value_str)
		except (ValueError, IndexError):
			pass
		
		# If all else fails, return as string
		return value_str
			