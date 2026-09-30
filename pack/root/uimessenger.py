import app
import ui
import grp
import net
import guild
import messenger
import localeInfo
import constInfo
import uiToolTip
import uiCommon
import player
import os
from _weakref import proxy

FRIEND = 0
GUILD = 1
BLOCK = 2
TEAM = 3

IMG_DIR = "elvion/game/friendlist/"
FLAG_FORMULA = "elvion/game/friendlist/flags/{}.jpg"
FLAG_WIDTH = 16

# Race image dictionary
RACE_IMAGE_DICT = {
	0: "elvion/game/friendlist/races/warrior_m.png",
	1: "elvion/game/friendlist/races/assassin_w.png",
	2: "elvion/game/friendlist/races/sura_m.png",
	3: "elvion/game/friendlist/races/shaman_w.png",
	4: "elvion/game/friendlist/races/warrior_w.png",
	5: "elvion/game/friendlist/races/assassin_m.png",
	6: "elvion/game/friendlist/races/sura_w.png",
	7: "elvion/game/friendlist/races/shaman_m.png",
}
if app.ENABLE_WOLFMAN_CHARACTER:
	RACE_IMAGE_DICT[8] = "elvion/game/friendlist/races/wolfman_m.png"

class MessengerItem(ui.Window):
	def __init__(self, getParentEvent):
		ui.Window.__init__(self)

		self.SetParent(getParentEvent())
		self.AddFlag("float")

		self.name = ""

		self.image = ui.ImageBox()
		self.image.AddFlag("not_pick")
		self.image.SetParent(self)
		self.image.Show()
		self.text = ui.TextLine()
		self.text.SetParent(self)
		self.text.SetPosition(20, 2)
		self.text.Show()

		self.lovePoint = -1
		self.lovePointToolTip = None
		self.isSelected = FALSE
		self.getParentEvent = getParentEvent

	def SetName(self, name):
		self.name = name
		if name:
			self.text.SetText(name)
			self.SetSize(20 + 6*len(name) + 4, 16)

	def SetLovePoint(self, lovePoint):
		self.lovePoint = lovePoint

	def Select(self):
		self.isSelected = TRUE

	def UnSelect(self):
		self.isSelected = FALSE

	def GetName(self):
		return self.name

	def GetStepWidth(self):
		return 0

	def CanWhisper(self):
		return FALSE

	def IsOnline(self):
		return FALSE

	def IsMobile(self):
		return FALSE

	def CanRemove(self):
		return FALSE

	def OnRemove(self):
		return FALSE

	def CanWarp(self):
		return FALSE

	def OnMouseOverIn(self):
		if -1 != self.lovePoint:
			if not self.lovePointToolTip:
				self.lovePointToolTip = uiToolTip.ToolTip(100)
				self.lovePointToolTip.SetTitle(self.name)
				self.lovePointToolTip.AppendTextLine(localeInfo.AFF_LOVE_POINT % (self.lovePoint))
				self.lovePointToolTip.ResizeToolTip()
			self.lovePointToolTip.ShowToolTip()

	def OnMouseOverOut(self):
		if self.lovePointToolTip:
			self.lovePointToolTip.HideToolTip()

	def OnMouseLeftButtonDown(self):
		self.getParentEvent().OnSelectItem(self)

	def OnMouseLeftButtonDoubleClick(self):
		self.getParentEvent().OnDoubleClickItem(self)

	def OnRender(self):
		if self.isSelected:
			x, y = self.GetGlobalPosition()
			grp.SetColor(grp.GenerateColor(0.0, 0.0, 0.7, 0.7))
			grp.RenderBar(x+16, y, self.GetWidth()-16, self.GetHeight())

class MessengerMemberItem(MessengerItem):
	STATE_OFFLINE = 0
	STATE_ONLINE = 1
	STATE_MOBILE = 2

	IMAGE_FILE_NAME = {
		"ONLINE" : "d:/ymir work/ui/game/windows/messenger_list_online.sub",
		"OFFLINE" : "d:/ymir work/ui/game/windows/messenger_list_offline.sub",
		"MOBILE" : "d:/ymir work/ui/game/windows/messenger_list_mobile.sub",
	}

	def __init__(self, getParentEvent):
		MessengerItem.__init__(self, getParentEvent)
		self.key = None
		self.state = self.STATE_OFFLINE
		self.mobileFlag = FALSE
		self.Offline()

	def GetStepWidth(self):
		return 15

	def SetKey(self, key):
		self.key = key

	def IsSameKey(self, key):
		return self.key == key

	def IsOnline(self):
		return self.STATE_ONLINE == self.state

	def IsMobile(self):
		return self.STATE_MOBILE == self.state

	def Online(self):
		self.image.LoadImage(self.IMAGE_FILE_NAME["ONLINE"])
		self.state = self.STATE_ONLINE

	def Offline(self):
		if self.mobileFlag:
			self.image.LoadImage(self.IMAGE_FILE_NAME["MOBILE"])
			self.state = self.STATE_MOBILE
		else:
			self.image.LoadImage(self.IMAGE_FILE_NAME["OFFLINE"])
			self.state = self.STATE_OFFLINE

	def SetMobile(self, flag):
		self.mobileFlag = flag
		if not self.IsOnline():
			self.Offline()

	def CanWhisper(self):
		return self.IsOnline()

	def OnWhisper(self):
		if self.IsOnline():
			self.getParentEvent().whisperButtonEvent(self.GetName())

	def OnMobileMessage(self):
		return

	def Select(self):
		MessengerItem.Select(self)

class MessengerGroupItem(MessengerItem):
	IMAGE_FILE_NAME = {
		"OPEN" : "d:/ymir work/ui/game/windows/messenger_list_open.sub",
		"CLOSE" : "d:/ymir work/ui/game/windows/messenger_list_close.sub",
	}

	def __init__(self, getParentEvent):
		self.isOpen = FALSE
		self.memberList = []
		MessengerItem.__init__(self, getParentEvent)

	def AppendMember(self, member, key, name):
		member.SetKey(key)
		member.SetName(name)
		self.memberList.append(member)
		return member

	def RemoveMember(self, item):
		for i in xrange(len(self.memberList)):
			if item == self.memberList[i]:
				del self.memberList[i]
				return

	def ClearMember(self):
		self.memberList = []

	def FindMember(self, key):
		list = filter(lambda argMember, argKey=key: argMember.IsSameKey(argKey), self.memberList)
		return list[0] if list else None

	def GetLoginMemberList(self):
		return filter(MessengerMemberItem.IsOnline, self.memberList)

	def GetLogoutMemberList(self):
		return filter(lambda arg: not arg.IsOnline(), self.memberList)

	def IsOpen(self):
		return self.isOpen

	def Open(self):
		self.image.LoadImage(self.IMAGE_FILE_NAME["OPEN"])
		self.isOpen = TRUE

	def Close(self):
		self.image.LoadImage(self.IMAGE_FILE_NAME["CLOSE"])
		self.isOpen = FALSE
		map(ui.Window.Hide, self.memberList)

	def Select(self):
		if self.IsOpen():
			self.Close()
		else:
			self.Open()
		MessengerItem.Select(self)
		self.getParentEvent().OnRefreshList()

class MessengerFriendItem(MessengerMemberItem):
	def __init__(self, getParentEvent):
		MessengerMemberItem.__init__(self, getParentEvent)

	def CanRemove(self):
		return TRUE

	def OnRemove(self):
		messenger.RemoveFriend(self.key)
		net.SendMessengerRemovePacket(self.key, self.name)
		return TRUE

class MessengerGuildItem(MessengerMemberItem):
	def __init__(self, getParentEvent):
		MessengerMemberItem.__init__(self, getParentEvent)

	def CanWarp(self):
		return self.IsOnline()

	def OnWarp(self):
		net.SendGuildUseSkillPacket(155, self.key)

	def CanRemove(self):
		if guild.MainPlayerHasAuthority(guild.AUTH_REMOVE_MEMBER):
			if guild.IsMemberByName(self.name):
				return TRUE
		return FALSE

	def OnRemove(self):
		net.SendGuildRemoveMemberPacket(self.key)
		return TRUE

if app.ENABLE_MESSENGER_BLOCK:
	class MessengerBlockItem(MessengerMemberItem):
		def __init__(self, getParentEvent):
			MessengerMemberItem.__init__(self, getParentEvent)

		def CanRemove(self):
			return True

		def OnRemove(self):
			net.SendMessengerBlockRemovePacket(self.key, self.name)
			return True

class MessengerBoardItem(MessengerMemberItem):
	def __init__(self, getParentEvent):
		MessengerMemberItem.__init__(self, getParentEvent)
		self.flags = []

	def CanRemove(self):
		return False

	def OnRemove(self):
		return False

	def GetName(self):
		return self.key

	def AppendCharFlag(self, flags_string):
		self.text.SetPosition(4, 0)
		self.image.SetPosition(-10, 0)

		for flag in flags_string.split(","):
			flag_image = ui.ImageBox()
			flag_image.SetParent(self)

			text_size_x, text_size_y = self.text.GetTextSize()
			text_x, text_y = self.text.GetLocalPosition()
			x_pos = (text_size_x + text_x + 10) + len(self.flags) * FLAG_WIDTH
			flag_image.SetPosition(x_pos, 0)
			flag_image.SetWindowVerticalAlignCenter()

			try:
				flag_image.LoadImage(FLAG_FORMULA.format(flag))
				flag_image.Show()
				self.flags.append(flag_image)
			except:
				flag_image.Hide()
				break

	def SetName(self, name):
		MessengerMemberItem.SetName(self, name)
		self.text.SetPosition(4, 0)

	def OnRender(self):
		if self.isSelected:
			x, y = self.GetGlobalPosition()
			grp.SetColor(grp.GenerateColor(0.0, 0.0, 0.7, 0.7))
			grp.RenderBar(x+2, y, self.GetWidth()+6, self.GetHeight())

class MessengerBoardGroup(MessengerGroupItem):
	def __init__(self, getParentEvent):
		MessengerGroupItem.__init__(self, getParentEvent)
		self.SetName("TeamList")
		self.AddFlag("float")

	def AppendMember(self, key, name):
		item = MessengerBoardItem(self.getParentEvent)
		return MessengerGroupItem.AppendMember(self, item, key, name)

class MessengerFriendGroup(MessengerGroupItem):
	def __init__(self, getParentEvent):
		MessengerGroupItem.__init__(self, getParentEvent)
		self.SetName(localeInfo.MESSENGER_FRIEND)

	def AppendMember(self, key, name):
		item = MessengerFriendItem(self.getParentEvent)
		return MessengerGroupItem.AppendMember(self, item, key, name)

if app.ENABLE_MESSENGER_BLOCK:
	class MessengerBlockGroup(MessengerGroupItem):
		def __init__(self, getParentEvent):
			MessengerGroupItem.__init__(self, getParentEvent)
			self.SetName("Block")
			self.AddFlag("float")

		def AppendMember(self, key, name):
			item = MessengerBlockItem(self.getParentEvent)
			return MessengerGroupItem.AppendMember(self, item, key, name)

class MessengerGuildGroup(MessengerGroupItem):
	def __init__(self, getParentEvent):
		MessengerGroupItem.__init__(self, getParentEvent)
		self.SetName(localeInfo.MESSENGER_GUILD)
		self.AddFlag("float")

	def AppendMember(self, key, name):
		item = MessengerGuildItem(self.getParentEvent)
		return MessengerGroupItem.AppendMember(self, item, key, name)

class MessengerFamilyGroup(MessengerGroupItem):
	def __init__(self, getParentEvent):
		MessengerGroupItem.__init__(self, getParentEvent)
		self.SetName(localeInfo.MESSENGER_FAMILY)
		self.AddFlag("float")
		self.lover = None

	def AppendMember(self, key, name):
		item = MessengerGuildItem(self.getParentEvent)
		self.lover = item
		return MessengerGroupItem.AppendMember(self, item, key, name)

	def GetLover(self):
		return self.lover

class ExpandableSection(ui.Window):
	def __init__(self, parent, title, imageName):
		ui.Window.__init__(self)
		self.SetParent(parent)
		
		self.isExpanded = True
		self.groupIndex = -1
		self.parent = parent
		self.title = title
		
		self.backgroundImage = ui.ImageBox()
		self.backgroundImage.SetParent(self)
		self.backgroundImage.LoadImage(imageName)
		self.backgroundImage.SetPosition(0, 0)
		self.backgroundImage.OnMouseLeftButtonDown = ui.__mem_func__(self.ToggleExpand)
		self.backgroundImage.Show()
		
		self.arrowIcon = ui.ImageBox()
		self.arrowIcon.SetParent(self)
		self.arrowIcon.SetPosition(5, 6)
		self.arrowIcon.LoadImage(IMG_DIR + "expanded.png")
		self.arrowIcon.Show()
		
		# Title text
		self.titleText = ui.TextLine()
		self.titleText.SetParent(self)
		self.titleText.AddFlag("attach")
		self.titleText.AddFlag("not_pick")
		self.titleText.SetPosition(26, 4)
		self.titleText.SetFontName("Verdana:15b")
		self.titleText.SetText(title)
		self.titleText.SetPackedFontColor(0xFFc5b4a2)
		self.titleText.SetOutline()
		self.titleText.Show()
		
		# Opening parenthesis
		self.counterPrefix = ui.TextLine()
		self.counterPrefix.SetParent(self)
		self.counterPrefix.AddFlag("attach")
		self.counterPrefix.AddFlag("not_pick")
		self.counterPrefix.SetFontName("Tahoma:14")
		self.counterPrefix.SetPackedFontColor(0xFFc5b4a2)
		self.counterPrefix.SetOutline()
		self.counterPrefix.SetText("(")
		self.counterPrefix.Show()
		
		# Online count
		self.onlineText = ui.TextLine()
		self.onlineText.SetParent(self)
		self.onlineText.AddFlag("attach")
		self.onlineText.AddFlag("not_pick")
		self.onlineText.SetFontName("Tahoma:14")
		self.onlineText.SetPackedFontColor(0xFFb0dfb4)
		self.onlineText.SetOutline()
		self.onlineText.SetText("0")
		self.onlineText.Show()
		
		# Separator
		self.counterSeparator = ui.TextLine()
		self.counterSeparator.SetParent(self)
		self.counterSeparator.AddFlag("attach")
		self.counterSeparator.AddFlag("not_pick")
		self.counterSeparator.SetFontName("Tahoma:14")
		self.counterSeparator.SetPackedFontColor(0xFFc5b4a2)
		self.counterSeparator.SetOutline()
		self.counterSeparator.SetText(" / ")
		self.counterSeparator.Show()
		
		# Total count
		self.totalText = ui.TextLine()
		self.totalText.SetParent(self)
		self.totalText.AddFlag("attach")
		self.totalText.AddFlag("not_pick")
		self.totalText.SetFontName("Tahoma:14")
		self.totalText.SetPackedFontColor(0xFFe57976)
		self.totalText.SetOutline()
		self.totalText.SetText("0")
		self.totalText.Show()
		
		# Closing parenthesis
		self.counterSuffix = ui.TextLine()
		self.counterSuffix.SetParent(self)
		self.counterSuffix.AddFlag("attach")
		self.counterSuffix.AddFlag("not_pick")
		self.counterSuffix.SetFontName("Tahoma:14")
		self.counterSuffix.SetPackedFontColor(0xFFc5b4a2)
		self.counterSuffix.SetOutline()
		self.counterSuffix.SetText(")")
		self.counterSuffix.Show()
		
		self.UpdateCounterPosition()
		
		self.SetSize(204, 24)

	def SetGroupIndex(self, index):
		self.groupIndex = index

	def UpdateCounterPosition(self):
		titleWidth = self.titleText.GetTextSize()[0]
		baseX = 25 + titleWidth + 5
		
		self.counterPrefix.SetPosition(baseX, 4)
		
		prefixWidth = self.counterPrefix.GetTextSize()[0]
		self.onlineText.SetPosition(baseX + prefixWidth, 4)
		
		onlineWidth = self.onlineText.GetTextSize()[0]
		self.counterSeparator.SetPosition(baseX + prefixWidth + onlineWidth, 4)
		
		separatorWidth = self.counterSeparator.GetTextSize()[0]
		self.totalText.SetPosition(baseX + prefixWidth + onlineWidth + separatorWidth, 4)
		
		totalWidth = self.totalText.GetTextSize()[0]
		self.counterSuffix.SetPosition(baseX + prefixWidth + onlineWidth + separatorWidth + totalWidth, 4)

	def UpdateCounter(self, onlineCount, totalCount):
		self.onlineText.SetText(str(onlineCount))
		self.totalText.SetText(str(totalCount))
		self.UpdateCounterPosition()

	def ToggleExpand(self):
		self.isExpanded = not self.isExpanded
		if self.isExpanded:
			self.arrowIcon.LoadImage(IMG_DIR + "expanded.png")
		else:
			self.arrowIcon.LoadImage(IMG_DIR + "expand.png")
		self.parent.OnRefreshList()

	def IsExpanded(self):
		return self.isExpanded
		
	def OnMouseLeftButtonDown(self):
		self.ToggleExpand()
		
	def ApplyClippingMask(self, clipWindow):
		self.SetClippingMaskWindow(clipWindow)
		self.backgroundImage.SetClippingMaskWindow(clipWindow) 
		self.arrowIcon.SetClippingMaskWindow(clipWindow)
		self.titleText.SetClippingMaskWindow(clipWindow)
		self.counterPrefix.SetClippingMaskWindow(clipWindow)
		self.onlineText.SetClippingMaskWindow(clipWindow)
		self.counterSeparator.SetClippingMaskWindow(clipWindow)
		self.totalText.SetClippingMaskWindow(clipWindow)
		self.counterSuffix.SetClippingMaskWindow(clipWindow)

class ScrollBar(ui.Window):
	SCROLLBAR_WIDTH = 7
	SCROLL_BTN_XDIST = 0
	SCROLL_BTN_YDIST = 0

	class MiddleBar(ui.DragButton):
		def __init__(self):
			ui.DragButton.__init__(self)
			self.AddFlag("movable")
			self.SetWindowName("scrollbar_middlebar")
		
		def MakeImage(self):
			top = ui.ExpandedImageBox()
			top.SetParent(self)
			top.LoadImage("elvion/game/friendlist/scrollbar/scrollbar_top.tga")
			top.AddFlag("not_pick")
			top.Show()
			
			topScale = ui.ExpandedImageBox()
			topScale.SetParent(self)
			topScale.SetPosition(0, top.GetHeight())
			topScale.LoadImage("elvion/game/friendlist/scrollbar/scrollbar_scale.tga")
			topScale.AddFlag("not_pick")
			topScale.Show()
			
			bottom = ui.ExpandedImageBox()
			bottom.SetParent(self)
			bottom.LoadImage("elvion/game/friendlist/scrollbar/scrollbar_bottom.tga")
			bottom.AddFlag("not_pick")
			bottom.Show()
			
			bottomScale = ui.ExpandedImageBox()
			bottomScale.SetParent(self)
			bottomScale.LoadImage("elvion/game/friendlist/scrollbar/scrollbar_scale.tga")
			bottomScale.AddFlag("not_pick")
			bottomScale.Show()
			
			middle = ui.ExpandedImageBox()
			middle.SetParent(self)
			middle.LoadImage("elvion/game/friendlist/scrollbar/scrollbar_mid.tga")
			middle.AddFlag("not_pick")
			middle.Show()
			
			self.top = top
			self.topScale = topScale
			self.bottom = bottom
			self.bottomScale = bottomScale
			self.middle = middle
		
		def SetSize(self, height):
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

	def __init__(self):
		ui.Window.__init__(self)
		self.pageSize = 1
		self.curPos = 0.0
		self.eventScroll = None
		self.eventArgs = None
		self.lockFlag = False
		self.CreateScrollBar()
		self.SetScrollBarSize(0)
		self.scrollStep = 0.4
		self.SetWindowName("NONAME_ScrollBar")

	def __del__(self):
		ui.Window.__del__(self)

	def CreateScrollBar(self):
		topImage = ui.ExpandedImageBox()
		topImage.SetParent(self)
		topImage.AddFlag("not_pick")
		topImage.LoadImage("elvion/game/friendlist/scrollbar/scroll_top.tga")
		topImage.Show()
		
		bottomImage = ui.ExpandedImageBox()
		bottomImage.SetParent(self)
		bottomImage.AddFlag("not_pick")
		bottomImage.LoadImage("elvion/game/friendlist/scrollbar/scroll_bottom.tga")
		bottomImage.Show()
		
		middleImage = ui.ExpandedImageBox()
		middleImage.SetParent(self)
		middleImage.AddFlag("not_pick")
		middleImage.SetPosition(0, topImage.GetHeight())
		middleImage.LoadImage("elvion/game/friendlist/scrollbar/scroll_mid.tga")
		middleImage.Show()
		
		self.topImage = topImage
		self.bottomImage = bottomImage
		self.middleImage = middleImage
		
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
		middleImageScale = float((height - self.SCROLL_BTN_YDIST*2) - self.middleImage.GetHeight()) / float(self.middleImage.GetHeight())
		self.middleImage.SetRenderingRect(0, 0, 0, middleImageScale)
		self.bottomImage.SetPosition(0, height - self.bottomImage.GetHeight())
		self.middleBar.SetRestrictMovementArea(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST, self.middleBar.GetWidth(), height - self.SCROLL_BTN_YDIST * 2)
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST)

	def SetScrollStep(self, step):
		self.scrollStep = step

	def OnUp(self):
		self.SetPos(self.curPos-self.scrollStep)

	def OnDown(self):
		self.SetPos(self.curPos+self.scrollStep)

	def GetScrollStep(self):
		return self.scrollStep

	def GetPos(self):
		return self.curPos

	def SetPos(self, pos, moveEvent = True):
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

class MessengerWindow(ui.ScriptWindow):
	START_POSITION = 95
	ITEM_HEIGHT = 20  # Height of each member item
	SECTION_SPACING = 3  # Spacing between sections
	VISIBLE_HEIGHT = 190  # Height of the visible content area

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		messenger.SetMessengerHandler(self)

		self.board = None
		self.groupList = []
		self.showingItemList = []
		self.selectedItem = None
		self.whisperButtonEvent = lambda *arg: None
		self.familyGroup = None
		self.scrollBar = None
		self.guildButtonEvent = None

		self.showingPageSize = 0
		self.startLine = 0
		self.hasMobilePhoneNumber = TRUE
		self.isLoaded = 0

		self.expandSections = {}
		self.sectionsStartY = 109

		self.__AddGroup()
		messenger.RefreshGuildMember()

	def Show(self):
		if self.isLoaded == 0:
			self.isLoaded = 1
			self.__LoadWindow()
			self.__ReparentMembers()
			self.OnRefreshList()
			
		self.__RefreshCharacterInfo()
		ui.ScriptWindow.Show(self)

	def __LoadWindow(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "UIScript/MessengerWindow.py")

		try:
			self.board = self.GetChild("board")
			self.whisperButton = self.GetChild("WhisperButton")
			self.mobileButton = self.GetChild("MobileButton")
			self.removeButton = self.GetChild("RemoveButton")
			self.addFriendButton = self.GetChild("AddFriendButton")
			self.guildButton = self.GetChild("GuildButton")
			self.blockButton = self.GetChild("BlockButton")
			self.supportButton = self.GetChild("SupportButton")
			self.content_bg = self.GetChild("content_bg")
		except:
			import exception
			exception.Abort("MessengerWindow.__LoadWindow.__Bind")

		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		self.whisperButton.SetEvent(ui.__mem_func__(self.OnPressWhisperButton))
		self.mobileButton.SetEvent(ui.__mem_func__(self.OnPressMobileButton))
		self.removeButton.SetEvent(ui.__mem_func__(self.OnPressRemoveButton))
		self.addFriendButton.SetEvent(ui.__mem_func__(self.OnPressAddFriendButton))
		self.guildButton.SetEvent(ui.__mem_func__(self.OnPressGuildButton))
		self.blockButton.SetEvent(ui.__mem_func__(self.OnPressBlockButton))
		self.supportButton.SetEvent(ui.__mem_func__(self.OnPressSupportButton))
		
		self.contentClipWindow = ui.Window()
		self.contentClipWindow.SetParent(self.content_bg)
		self.contentClipWindow.SetPosition(-6, 3)
		self.contentClipWindow.SetSize(self.content_bg.GetWidth() + 3, self.content_bg.GetHeight() - 6)
		self.contentClipWindow.Show()

		self.scrollBar = ScrollBar()
		self.scrollBar.SetParent(self.board)
		self.scrollBar.SetPosition(219, 105)
		self.scrollBar.SetScrollBarSize(212)
		self.scrollBar.SetScrollEvent(ui.__mem_func__(self.OnScroll))
		self.scrollBar.Show()

		self.GetChild("search_btn").SetEvent(ui.__mem_func__(self.OnRefreshList))
		self.GetChild("clear_btn").SetEvent(ui.__mem_func__(self.__ClearSearch))
		self.GetChild("editline").OnPressEscapeKey = ui.__mem_func__(self.Close)
		self.GetChild("editline").OnIMEUpdate = ui.__mem_func__(self.OnIMEUpdate)

		friendSection = ExpandableSection(self, localeInfo.COMMUNITY_FRIEND_SECTION, IMG_DIR + "expand_friend.png")
		friendSection.SetGroupIndex(FRIEND)
		friendSection.Show()
		self.expandSections[FRIEND] = friendSection

		guildSection = ExpandableSection(self, localeInfo.COMMUNITY_GUILD_SECTION, IMG_DIR + "expand_favorite.png")
		guildSection.SetGroupIndex(GUILD)
		guildSection.Show()
		self.expandSections[GUILD] = guildSection

		if app.ENABLE_MESSENGER_BLOCK:
			blockSection = ExpandableSection(self, localeInfo.COMMUNITY_BLOCKED_SECTION, IMG_DIR + "expand_block.png")
			blockSection.SetGroupIndex(BLOCK)
			blockSection.Show()
			self.expandSections[BLOCK] = blockSection

		teamSection = ExpandableSection(self, localeInfo.COMMUNITY_TEAM_SECTION, IMG_DIR + "expand_team.png")
		teamSection.SetGroupIndex(TEAM)
		teamSection.Show()
		self.expandSections[TEAM] = teamSection
		
		for section in self.expandSections.values():
			section.SetParent(self.contentClipWindow)
			
		for section in self.expandSections.values():
			section.ApplyClippingMask(self.contentClipWindow)
	
		self.character_bg = self.GetChild("character_bg")
		
		self.charImage = ui.ImageBox()
		self.charImage.SetParent(self.character_bg)
		self.charImage.SetPosition(2, 2)
		self.charImage.Show()
		
		self.charLevelText = ui.TextLine()
		self.charLevelText.SetParent(self.character_bg)
		self.charLevelText.AddFlag("attach")
		self.charLevelText.AddFlag("not_pick")
		self.charLevelText.SetPosition(51, 19)
		self.charLevelText.SetPackedFontColor(0xFF90ed30)
		self.charLevelText.SetOutline()
		self.charLevelText.Show()
		
		self.charAlignmentText = ui.TextLine()
		self.charAlignmentText.SetParent(self.character_bg)
		self.charAlignmentText.SetFontName("Tahoma:12")
		self.charAlignmentText.SetOutline()
		self.charAlignmentText.Show()
		
		self.charNameText = ui.TextLine()
		self.charNameText.SetParent(self.character_bg)
		self.charNameText.SetFontName("Tahoma:12")
		self.charNameText.SetPackedFontColor(0xFFffd74c)
		self.charNameText.SetOutline()
		self.charNameText.Show()
		
		self.charGuildText = ui.TextLine()
		self.charGuildText.SetParent(self.character_bg)
		self.charGuildText.SetPosition(95, 5)
		self.charGuildText.SetFontName("Tahoma:12")
		self.charGuildText.SetPackedFontColor(0xFFefd3ff)
		self.charGuildText.SetOutline()
		self.charGuildText.Show()
		
		self.__RefreshCharacterInfo()

		self.mobileButton.Disable()
		self.mobileButton.Hide()
		self.whisperButton.Disable()
		self.removeButton.Disable()
		
	def __RefreshCharacterInfo(self):
		# Alignment data
		point, grade = player.GetAlignmentData()
		import colorInfo
		COLOR_DICT = {	0 : colorInfo.TITLE_RGB_GOOD_9,
						1 : colorInfo.TITLE_RGB_GOOD_8,
						2 : colorInfo.TITLE_RGB_GOOD_7,
						3 : colorInfo.TITLE_RGB_GOOD_6,
						4 : colorInfo.TITLE_RGB_GOOD_5,
						5 : colorInfo.TITLE_RGB_GOOD_4,
						6 : colorInfo.TITLE_RGB_GOOD_3,
						7 : colorInfo.TITLE_RGB_GOOD_2,
						8 : colorInfo.TITLE_RGB_GOOD_1,
						9 : colorInfo.TITLE_RGB_NORMAL,
						10 : colorInfo.TITLE_RGB_EVIL_1,
						11 : colorInfo.TITLE_RGB_EVIL_2,
						12 : colorInfo.TITLE_RGB_EVIL_3,
						13 : colorInfo.TITLE_RGB_EVIL_4, }
		colorList = COLOR_DICT.get(grade, colorInfo.TITLE_RGB_NORMAL)
		gradeColor = ui.GenerateColor(colorList[0], colorList[1], colorList[2])

		# Character race image
		race = net.GetMainActorRace()
		if race in RACE_IMAGE_DICT:
			try:
				self.charImage.LoadImage(RACE_IMAGE_DICT[race])
			except:
				pass
		
		# Character level
		level = player.GetStatus(player.LEVEL)
		self.charLevelText.SetText("Lv. %d" % level)
		
		# Calculate position for alignment text
		levelWidth = self.charLevelText.GetTextSize()[0]
		alignmentX = 50 + levelWidth + 3 # 3px after level text
		
		# Alignment
		self.charAlignmentText.SetText(localeInfo.TITLE_NAME_LIST[grade])
		self.charAlignmentText.SetPosition(alignmentX, 19)
		self.charAlignmentText.SetPackedFontColor(gradeColor)
		
		# Calculate position for name text
		alignmentWidth = self.charAlignmentText.GetTextSize()[0]
		nameX = alignmentX + alignmentWidth + 3 # 3px after alignment
		
		# Character name
		charName = player.GetName()
		self.charNameText.SetText(charName)
		self.charNameText.SetPosition(nameX, 19)
		
		# Guild name
		guildName = player.GetGuildName()
		if guildName:
			self.charGuildText.SetText(guildName)
		else:
			self.charGuildText.SetText(localeInfo.COMMUNITY_NO_GUILD)

	def OnPressBlockButton(self):
		blockNameBoard = uiCommon.InputDialog()
		blockNameBoard.SetTitle("Block")
		blockNameBoard.SetAcceptEvent(ui.__mem_func__(self.OnAddBlock))
		blockNameBoard.SetCancelEvent(ui.__mem_func__(self.OnCancelAddBlock))
		blockNameBoard.Open()
		self.blockNameBoard = blockNameBoard
		
	def OnPressSupportButton(self):
		os.system("start " + "https://discord.com/invite/")

	def OnAddBlock(self):
		text = self.blockNameBoard.GetText()
		if text:
			net.SendMessengerBlockAddByNamePacket(text)
		self.blockNameBoard.Close()
		self.blockNameBoard = None
		return True

	def OnCancelAddBlock(self):
		self.blockNameBoard.Close()
		self.blockNameBoard = None
		return True

	def __ClearSearch(self):
		self.GetChild("editline").SetText("")
		self.OnRefreshList()

	def OnIMEUpdate(self):
		editline = self.GetChild("editline")
		while True:
			if editline.GetTextSize()[0] >= 100:
				text = editline.GetText()
				editline.SetText(text[:len(text)-2])
			else:
				break
		ui.EditLine.OnIMEUpdate(editline)
		self.OnRefreshList()

	def __del__(self):
		messenger.SetMessengerHandler(None)
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.board = None
		self.scrollBar = None
		self.friendNameBoard = None
		self.questionDialog = None
		self.popupDialog = None
		self.inputDialog = None
		self.familyGroup = None
		self.whisperButton = None
		self.mobileButton = None
		self.removeButton = None
		self.expandSections = {}

	def OnCloseQuestionDialog(self):
		if self.questionDialog:
			self.questionDialog.Close()
		self.questionDialog = None
		return TRUE

	def Close(self):
		self.questionDialog = None
		self.Hide()

	def SetSize(self, width, height):
		ui.ScriptWindow.SetSize(self, width, height)
		if self.board:
			self.board.SetSize(width, height)
			
	def __LocateMember(self):
		if self.isLoaded == 0:
			return
	
		# Hide all items first
		map(ui.Window.Hide, self.showingItemList)
	
		# Calculate scroll offset
		scrollPos = self.scrollBar.GetPos()
		
		# Count total content height needed
		totalContentHeight = 0
		
		for groupIdx in [FRIEND, GUILD, BLOCK if app.ENABLE_MESSENGER_BLOCK else -1, TEAM]:
			if groupIdx == -1:
				continue
			
			if groupIdx >= len(self.groupList):
				continue
	
			if groupIdx not in self.expandSections:
				continue
				
			section = self.expandSections[groupIdx]
			totalContentHeight += section.GetHeight() + self.SECTION_SPACING
			
			if section.IsExpanded():
				group = self.groupList[groupIdx]
				groupMemberSet = set(group.memberList)
				for member in self.showingItemList:
					if member in groupMemberSet:
						totalContentHeight += self.ITEM_HEIGHT

		maxScroll = max(0, totalContentHeight - self.content_bg.GetHeight())
		scrollOffset = int(scrollPos * maxScroll)
		currentY = -scrollOffset
		
		for groupIdx in [FRIEND, GUILD, BLOCK if app.ENABLE_MESSENGER_BLOCK else -1, TEAM]:
			if groupIdx == -1:
				continue
			
			if groupIdx >= len(self.groupList):
				continue
	
			if groupIdx not in self.expandSections:
				continue
				
			section = self.expandSections[groupIdx]
			section.SetPosition(9, currentY)
			section.Show()
			
			currentY += section.GetHeight() + self.SECTION_SPACING
	
			if section.IsExpanded():
				group = self.groupList[groupIdx]
				groupMemberSet = set(group.memberList)
				for member in self.showingItemList:
					if member in groupMemberSet:
						member.SetPosition(5 + member.GetStepWidth(), currentY)
						member.Show()
						currentY += self.ITEM_HEIGHT
	
		# Update scrollbar
		if totalContentHeight > self.content_bg.GetHeight():
			visibleRatio = float(self.content_bg.GetHeight()) / float(totalContentHeight)
			self.scrollBar.LockScroll()
			self.scrollBar.SetMiddleBarSize(visibleRatio)
			self.scrollBar.UnlockScroll()
			self.scrollBar.Show()
		else:
			self.scrollBar.LockScroll()
			self.scrollBar.SetPos(0, False)
			self.scrollBar.UnlockScroll()
			self.scrollBar.Hide()

	def __AddGroup(self):
		member = MessengerFriendGroup(ui.__mem_func__(self.GetSelf))
		member.Open()
		member.Show()
		self.groupList.append(member)
	
		member = MessengerGuildGroup(ui.__mem_func__(self.GetSelf))
		member.Open()
		member.Show()
		self.groupList.append(member)
	
		if app.ENABLE_MESSENGER_BLOCK:
			member = MessengerBlockGroup(ui.__mem_func__(self.GetSelf))
			member.Open()
			member.Show()
			self.groupList.append(member)
	
		board_group = MessengerBoardGroup(ui.__mem_func__(self.GetSelf))
		board_group.Open()
		board_group.Show()
		self.groupList.append(board_group)
		
	def __ReparentMembers(self):
		for group in self.groupList:
			for member in group.memberList:
				member.SetParent(self.contentClipWindow)
				member.SetClippingMaskWindow(self.contentClipWindow)
		
		if self.familyGroup:
			for member in self.familyGroup.memberList:
				member.SetParent(self.contentClipWindow)
				member.SetClippingMaskWindow(self.contentClipWindow)

	def __AddFamilyGroup(self):
		member = MessengerFamilyGroup(ui.__mem_func__(self.GetSelf))
		member.Open()
		member.Show()
		self.familyGroup = member

	def ClearGuildMember(self):
		self.groupList[GUILD].ClearMember()

	def SetWhisperButtonEvent(self, event):
		self.whisperButtonEvent = event

	def SetGuildButtonEvent(self, event):
		self.guildButtonEvent = event

	def SendMobileMessage(self, name):
		return

	def OnPressGuildButton(self):
		self.guildButtonEvent()

	def OnPressAddFriendButton(self):
		friendNameBoard = uiCommon.InputDialog()
		friendNameBoard.SetTitle(localeInfo.MESSENGER_ADD_FRIEND)
		friendNameBoard.SetAcceptEvent(ui.__mem_func__(self.OnAddFriend))
		friendNameBoard.SetCancelEvent(ui.__mem_func__(self.OnCancelAddFriend))
		friendNameBoard.Open()
		self.friendNameBoard = friendNameBoard

	def OnAddFriend(self):
		text = self.friendNameBoard.GetText()
		if text:
			net.SendMessengerAddByNamePacket(text)
		if self.friendNameBoard:
			self.friendNameBoard.Close()
		self.friendNameBoard = None
		return TRUE

	def OnCancelAddFriend(self):
		self.friendNameBoard.Close()
		self.friendNameBoard = None
		return TRUE

	def OnPressWhisperButton(self):
		if self.selectedItem:
			interface = constInfo.GetInterfaceInstance()
			if interface:
				# Check if whisper window is already opened
				if hasattr(interface, 'whisperDialogDict') and interface.whisperDialogDict:
					# Get the first whisper window
					whisperDialog = None
					for dialog in interface.whisperDialogDict.values():
						whisperDialog = dialog
						break
					
					if whisperDialog and whisperDialog.IsShow():
						# Window is open, just switch to this target
						whisperDialog.OpenWithTarget(self.selectedItem.GetName())
					else:
						# Window not open, open it normally
						self.selectedItem.OnWhisper()
				else:
					# No whisper window exists, open normally
					self.selectedItem.OnWhisper()

	def OnPressMobileButton(self):
		if self.selectedItem:
			self.selectedItem.OnMobileMessage()

	def OnPressRemoveButton(self):
		if self.selectedItem:
			if self.selectedItem.CanRemove():
				self.questionDialog = uiCommon.QuestionDialog()
				self.questionDialog.SetText(localeInfo.MESSENGER_DO_YOU_DELETE)
				self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnRemove))
				self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
				self.questionDialog.Open()

	def OnRemove(self):
		if self.selectedItem:
			if self.selectedItem.CanRemove():
				map(lambda arg, argDeletingItem=self.selectedItem: arg.RemoveMember(argDeletingItem), self.groupList)
				self.selectedItem.OnRemove()
				self.selectedItem.UnSelect()
				self.selectedItem = None
				self.OnRefreshList()
		self.OnCloseQuestionDialog()

	def OnScroll(self):
		self.__LocateMember()
		scrollLineCount = 0
		
		for groupIdx in [FRIEND, GUILD, BLOCK if app.ENABLE_MESSENGER_BLOCK else -1, TEAM]:
			if groupIdx == -1:
				continue
			
			if groupIdx >= len(self.groupList):
				continue
			
			if groupIdx in self.expandSections:
				section = self.expandSections[groupIdx]
				if section.IsExpanded():
					group = self.groupList[groupIdx]
					searchText = self.GetChild("editline").GetText().lower() if self.IsChild("editline") else ""
					
					for member in group.memberList:
						if searchText == "" or member.name.lower().find(searchText) != -1:
							scrollLineCount += 1
		
		if scrollLineCount > 0:
			startLine = int(scrollLineCount * self.scrollBar.GetPos())
			
			maxStartLine = max(0, scrollLineCount - 13)
			if startLine > maxStartLine:
				startLine = maxStartLine
			if startLine < 0:
				startLine = 0

			if startLine != self.startLine:
				self.startLine = startLine
				self.__LocateMember()
				
	def OnRunMouseWheel(self, nLen):
		if nLen > 0:
			self.scrollBar.OnUp()
		else:
			self.scrollBar.OnDown()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

	def OnSelectItem(self, item):
		if self.selectedItem:
			if item != self.selectedItem:
				self.selectedItem.UnSelect()

		self.selectedItem = item

		if self.selectedItem:
			self.selectedItem.Select()

			if self.selectedItem.CanWhisper():
				self.whisperButton.Enable()
			else:
				self.whisperButton.Disable()

			if self.selectedItem.IsMobile():
				self.mobileButton.Enable()
			else:
				self.mobileButton.Disable()

			if self.selectedItem.CanRemove():
				self.removeButton.Enable()
			else:
				self.removeButton.Disable()

	def OnDoubleClickItem(self, item):
		if not self.selectedItem:
			return
		interface = constInfo.GetInterfaceInstance()
		if interface:
			# Check if whisper window is already open
			if hasattr(interface, 'whisperDialogDict') and interface.whisperDialogDict:
				# Get the first whisper window
				whisperDialog = None
				for dialog in interface.whisperDialogDict.values():
					whisperDialog = dialog
					break
				
				if whisperDialog and whisperDialog.IsShow():
					# Window is open, just switch to this target
					whisperDialog.OpenWithTarget(self.selectedItem.GetName())
				else:
					# Window not open, open it normally
					self.whisperButtonEvent(self.selectedItem.GetName())
			else:
				# No whisper window exists, open normally
				self.whisperButtonEvent(self.selectedItem.GetName())

	def GetSelf(self):
		return self

	def OnRefreshList(self):
		map(ui.Window.Hide, self.groupList)
		map(ui.Window.Hide, self.showingItemList)
		self.showingItemList = []
	
		if self.familyGroup:
			if self.familyGroup.GetLover():
				self.showingItemList.append(self.familyGroup.GetLover())
	
		searchText = self.GetChild("editline").GetText().lower() if self.IsChild("editline") else ""
		
		for groupIdx in [FRIEND, GUILD, BLOCK if app.ENABLE_MESSENGER_BLOCK else -1, TEAM]:
			if groupIdx == -1:
				continue
			
			if groupIdx >= len(self.groupList):
				continue
	
			if groupIdx in self.expandSections:
				section = self.expandSections[groupIdx]
	
				group = self.groupList[groupIdx]
				loginMemberList = group.GetLoginMemberList()
				logoutMemberList = group.GetLogoutMemberList()
				
				filteredLoginMembers = [m for m in loginMemberList if searchText == "" or m.name.lower().find(searchText) != -1]
				filteredLogoutMembers = [m for m in logoutMemberList if searchText == "" or m.name.lower().find(searchText) != -1]
				
				totalMembers = len(filteredLoginMembers) + len(filteredLogoutMembers)
				onlineMembers = len(filteredLoginMembers)
				section.UpdateCounter(onlineMembers, totalMembers)
	
				if section.IsExpanded():
					for member in filteredLoginMembers:
						self.showingItemList.append(member)
					
					for member in filteredLogoutMembers:
						self.showingItemList.append(member)
	
		self.startLine = 0
		if self.scrollBar:
			self.scrollBar.SetPos(0)
		self.__LocateMember()

	def RefreshMessenger(self):
		self.OnRefreshList()

	def __AddList(self, groupIndex, key, name):
		if groupIndex >= len(self.groupList):
			return None
		
		group = self.groupList[groupIndex]
		member = group.FindMember(key)
		if not member:
			member = group.AppendMember(key, name)
			if self.isLoaded:
				member.SetParent(self.contentClipWindow)
				member.SetClippingMaskWindow(self.contentClipWindow)
			self.OnSelectItem(None)
		return member

	def OnRemoveList(self, groupIndex, key):
		group = self.groupList[groupIndex]
		group.RemoveMember(group.FindMember(key))
		self.OnRefreshList()

	def OnRemoveAllList(self, groupIndex):
		group = self.groupList[groupIndex]
		group.ClearMember()
		self.OnRefreshList()

	def OnLogin(self, groupIndex, key, name=None):
		if not name:
			name = key
		group = self.groupList[groupIndex]
		member = self.__AddList(groupIndex, key, name)
		member.SetName(name)
		member.Online()
		self.OnRefreshList()

	def OnLogout(self, groupIndex, key, name=None):
		if groupIndex >= len(self.groupList):
			return
		
		group = self.groupList[groupIndex]
		member = self.__AddList(groupIndex, key, name)
		if not name:
			name = key
		member.SetName(name)
		member.Offline()
		self.OnRefreshList()

	def OnMobile(self, groupIndex, key, mobileFlag):
		group = self.groupList[groupIndex]
		member = group.FindMember(key)
		if not member:
			return
		member.SetMobile(mobileFlag)
		self.OnRefreshList()

	def OnAddLover(self, name, lovePoint):
		if not self.familyGroup:
			self.__AddFamilyGroup()

		member = self.familyGroup.AppendMember(0, name)
		member.SetName(name)
		member.SetLovePoint(lovePoint)
		member.Offline()
		self.OnRefreshList()

	def OnUpdateLovePoint(self, lovePoint):
		if not self.familyGroup:
			return
		lover = self.familyGroup.GetLover()
		if not lover:
			return
		lover.SetLovePoint(lovePoint)

	def OnLoginLover(self):
		if not self.familyGroup:
			return
		lover = self.familyGroup.GetLover()
		if not lover:
			return
		lover.Online()

	def OnLogoutLover(self):
		if not self.familyGroup:
			return
		lover = self.familyGroup.GetLover()
		if not lover:
			return
		lover.Offline()

	def ClearLoverInfo(self):
		if not self.familyGroup:
			return
		self.familyGroup.ClearMember()
		self.familyGroup = None
		self.OnRefreshList()

	def CheckoutTeamMemberStatus(self, name, lang, status):
		if TEAM >= len(self.groupList):
			return
		
		group = self.groupList[TEAM]
		member = group.FindMember(name)
	
		if not member:
			member = group.AppendMember(name, name)
			member.AppendCharFlag(lang)
			
			if self.isLoaded:
				member.SetParent(self.contentClipWindow)
				member.SetClippingMaskWindow(self.contentClipWindow)
			
			self.OnSelectItem(None)
	
		if status:
			member.Online()
		else:
			member.Offline()
	
		self.OnRefreshList()

	def ClearTeamMemberList(self):
		group = self.groupList[TEAM]
		group.ClearMember()
		self.OnRefreshList()

from operator import truediv
class ScrollBarSpecial(ui.Window):
	BASE_COLOR = grp.GenerateColor(0.0, 0.0, 0.0, 1.0)
	CORNERS_AND_LINES_COLOR = grp.GenerateColor(0.3411, 0.3411, 0.3411, 1.0)
	BAR_NUMB = 9
	SCROLL_WIDTH = 8

	class MiddleBar(ui.DragButton):
		MIDDLE_BAR_COLOR = grp.GenerateColor(78.0 / 255.0, 65.0 / 255.0, 51.0 / 255.0, 1.0)
		def __init__(self, horizontal_scroll):
			ui.DragButton.__init__(self)
			self.AddFlag("movable")
			self.horizontal_scroll = horizontal_scroll
			self.middle = ui.Bar()
			self.middle.SetParent(self)
			self.middle.AddFlag("attach")
			self.middle.AddFlag("not_pick")
			self.middle.SetColor(self.MIDDLE_BAR_COLOR)
			self.middle.SetSize(1, 1)
			self.middle.Show()

		def SetStaticScale(self, size):
			(base_width, base_height) = (self.middle.GetWidth(), self.middle.GetHeight())
			if not self.horizontal_scroll:
				ui.DragButton.SetSize(self, base_width, size)
				self.middle.SetSize(base_width, size)
			else:
				ui.DragButton.SetSize(self, size, base_height)
				self.middle.SetSize(size, base_height)

		def SetSize(self, selfSize, fullSize):
			(base_width, base_height) = (self.middle.GetWidth(), self.middle.GetHeight())
			
			if not self.horizontal_scroll:
				ui.DragButton.SetSize(self, base_width, truediv(int(selfSize), int(fullSize)) * selfSize)
				self.middle.SetSize(base_width, truediv(int(selfSize), int(fullSize)) * selfSize)
			else:
				ui.DragButton.SetSize(self, truediv(int(selfSize), int(fullSize)) * selfSize, base_height)
				self.middle.SetSize(truediv(int(selfSize), int(fullSize)) * selfSize, base_height)

		def SetStaticSize(self, size):
			size = max(2, size)
			
			if not self.horizontal_scroll:
				ui.DragButton.SetSize(self, size, self.middle.GetHeight())
				self.middle.SetSize(size, self.middle.GetHeight())
			else:
				ui.DragButton.SetSize(self, self.middle.GetWidth(), size)
				self.middle.SetSize(self.middle.GetWidth(), size)

	def __init__(self, horizontal_scroll = False):
		ui.Window.__init__(self)
		self.horizontal_scroll = horizontal_scroll
		self.scrollEvent = None
		self.scrollSpeed = 50
		self.sizeScale = 1.0
		self.bars = []
		for i in xrange(self.BAR_NUMB):
			br = ui.Bar()
			br.SetParent(self)
			br.AddFlag("attach")
			br.AddFlag("not_pick")
			br.SetColor([self.CORNERS_AND_LINES_COLOR, self.BASE_COLOR][i == (self.BAR_NUMB-1)])
			if not (i % 2 == 0): br.SetSize(1, 1)
			br.Show()
			self.bars.append(br)
		self.middleBar = self.MiddleBar(self.horizontal_scroll)
		self.middleBar.SetParent(self)
		self.middleBar.SetMoveEvent(ui.__mem_func__(self.OnScrollMove))
		self.middleBar.Show()

	def OnScrollMove(self):
		if not self.scrollEvent:
			return
		arg = float(self.middleBar.GetLocalPosition()[1] - 1) / float(self.GetHeight() - 2 - self.middleBar.GetHeight()) if not self.horizontal_scroll else\
				float(self.middleBar.GetLocalPosition()[0] - 1) / float(self.GetWidth() - 2 - self.middleBar.GetWidth())
		self.scrollEvent(arg)

	def SetScrollEvent(self, func):
		self.scrollEvent = func

	def SetScrollSpeed(self, speed):
		self.scrollSpeed = speed

	def OnMouseWheel(self, nLen):
		if self.content_bg and self.content_bg.IsIn():
			if self.scrollBar and self.scrollBar.IsShow():
				if nLen > 0:
					self.scrollBar.OnUp()
				else:
					self.scrollBar.OnDown()
				return True
		return False

	def GetPos(self):
		return float(self.middleBar.GetLocalPosition()[1] - 1) / float(self.GetHeight() - 2 - self.middleBar.GetHeight()) if not self.horizontal_scroll else float(self.middleBar.GetLocalPosition()[0] - 1) / float(self.GetWidth() - 2 - self.middleBar.GetWidth())

	def OnMouseLeftButtonDown(self):
		(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()
		if not self.horizontal_scroll:
			if xMouseLocalPosition == 0 or xMouseLocalPosition == self.GetWidth():
				return
			y_pos = (yMouseLocalPosition - self.middleBar.GetHeight() / 2)
			self.middleBar.SetPosition(1, y_pos)
		else:
			if yMouseLocalPosition == 0 or yMouseLocalPosition == self.GetHeight():
				return
			x_pos = (xMouseLocalPosition - self.middleBar.GetWidth() / 2)
			self.middleBar.SetPosition(x_pos, 1)
		self.OnScrollMove()

	def SetSize(self, w, h):
		(width, height) = (max(3, w), max(3, h))
		ui.Window.SetSize(self, width, height)
		self.bars[0].SetSize(1, (height - 2))
		self.bars[0].SetPosition(0, 1)
		self.bars[2].SetSize((width - 2), 1)
		self.bars[2].SetPosition(1, 0)
		self.bars[4].SetSize(1, (height - 2))
		self.bars[4].SetPosition((width - 1), 1)
		self.bars[6].SetSize((width - 2), 1)
		self.bars[6].SetPosition(1, (height - 1))
		self.bars[8].SetSize((width - 2), (height - 2))
		self.bars[8].SetPosition(1, 1)
		self.bars[1].SetPosition(0, 0)
		self.bars[3].SetPosition((width - 1), 0)
		self.bars[5].SetPosition((width - 1), (height - 1))
		self.bars[7].SetPosition(0, (height - 1))
		if not self.horizontal_scroll:
			self.middleBar.SetStaticSize(width - 2)
			self.middleBar.SetSize(12, self.GetHeight())
		else:
			self.middleBar.SetStaticSize(height - 2)
			self.middleBar.SetSize(12, self.GetWidth())
		self.middleBar.SetRestrictMovementArea(1, 1, width - 2, height - 2)

	def SetScale(self, selfSize, fullSize):
		self.sizeScale = float(selfSize)/float(fullSize)
		if self.sizeScale <= 0.0305:
			self.sizeScale = 0.05
		self.middleBar.SetSize(selfSize, fullSize)

	def SetStaticScale(self, r_size):
		self.middleBar.SetStaticScale(r_size)

	def SetPosScale(self, fScale):
		pos = (math.ceil((self.GetHeight() - 2 - self.middleBar.GetHeight()) * fScale) + 1) if not self.horizontal_scroll else (math.ceil((self.GetWidth() - 2 - self.middleBar.GetWidth()) * fScale) + 1)
		self.SetPos(pos)

	def SetPos(self, pos):
		wPos = (1, pos) if not self.horizontal_scroll else (pos, 1)
		self.middleBar.SetPosition(*wPos)