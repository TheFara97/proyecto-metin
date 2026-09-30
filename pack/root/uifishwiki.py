import ui
import chat
import item
import localeInfo
import uiScriptLocale
import net
import app
import wndMgr

FISHES = [
	[27802, 27803, 27804],
	[27805, 27806, 27807],
	[27808, 27809, 27810],
	[27811, 27812, 27813],
	[27814, 27815, 27816],
	[27817, 27818, 27819],
	[27820, 27821, 27822],
	[27823, 27824, 27825],
	[27826, 27827, 27828],
	[27829, 27830, 27831],
	[27832, 0, 0],
]

FISH_NEED_RODS = {
	27802 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27803 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27804 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27805 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27806 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27807 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27808 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27809 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27810 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27811 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27812 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27813 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27814 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27815 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27816 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27817 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27818 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27819 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27820 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27821 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27822 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27823 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27824 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27825 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27826 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27827 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27828 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27829 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27830 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27831 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
	27832 : localeInfo.FISHWIKI_FISHINGPOLE_GRADE,
}

FISH_MISSION_DATA = {
	27802 : [25, 18, 80, localeInfo.FISHWIKI_FISH_27802],
	27803 : [25, 40, 80, localeInfo.FISHWIKI_FISH_27803],
	27804 : [25, 9, 80, localeInfo.FISHWIKI_FISH_27804],
	27805 : [25, 50, 80, localeInfo.FISHWIKI_FISH_27805],
	27806 : [25, 105, 80, localeInfo.FISHWIKI_FISH_27806],
	27807 : [25, 180, 70, localeInfo.FISHWIKI_FISH_27807],
	27808 : [25, 180, 70, localeInfo.FISHWIKI_FISH_27808],
	27809 : [25, 95, 70, localeInfo.FISHWIKI_FISH_27809],
	27810 : [25, 85, 70, localeInfo.FISHWIKI_FISH_27810],
	27811 : [25, 60, 70, localeInfo.FISHWIKI_FISH_27811],
	27812 : [20, 100, 60, localeInfo.FISHWIKI_FISH_27812],
	27813 : [20, 50, 60, localeInfo.FISHWIKI_FISH_27813],
	27814 : [20, 100, 60, localeInfo.FISHWIKI_FISH_27814],
	27815 : [20, 65, 60, localeInfo.FISHWIKI_FISH_27815],
	27816 : [20, 313, 60, localeInfo.FISHWIKI_FISH_27816],
	27817 : [20, 50, 60, localeInfo.FISHWIKI_FISH_27817],
	27818 : [20, 82, 50, localeInfo.FISHWIKI_FISH_27818],
	27819 : [20, 66, 50, localeInfo.FISHWIKI_FISH_27819],
	27820 : [20, 34, 50, localeInfo.FISHWIKI_FISH_27820],
	27821 : [15, 175, 50, localeInfo.FISHWIKI_FISH_27821],
	27822 : [15, 106, 50, localeInfo.FISHWIKI_FISH_27822],
	27823 : [15, 106, 50, localeInfo.FISHWIKI_FISH_27823],
	27824 : [15, 18, 50, "x"],
	27825 : [10, 37, 50, "x"],
	27826 : [10, 58, 50, "x"],
	27827 : [10, 18, 50, "x"],
	27828 : [10, 135, 50, "x"],
	27829 : [10, 37, 50, "x"],
	27830 : [10, 230, 50, "x"],
	27831 : [10, 17, 50, "x"],
	27832 : [10, 17, 50, "x"],
}

def GetFishMissionData(fishVnum):
	return FISH_MISSION_DATA.get(fishVnum, None)

class ScrollBar(ui.Window):
	MIDDLE_BAR_POS = 0
	MIDDLE_BAR_UPPER_PLACE = 6
	MIDDLE_BAR_DOWNER_PLACE = 5
	TEMP_SPACE = MIDDLE_BAR_UPPER_PLACE + MIDDLE_BAR_DOWNER_PLACE

	class MiddleBar(ui.DragButton):
		def __init__(self):
			ui.DragButton.__init__(self)
			self.AddFlag("movable")
			ui.DragButton.SetSize(self, 20, 22)

		def MakeImage(self):
			middle = ui.ExpandedImageBox()
			middle.SetParent(self)
			middle.LoadImage("zaris/fish_wiki/scroll_middle.png")
			middle.SetPosition(-3, 0)
			middle.AddFlag("not_pick")
			middle.Show()
			self.middle = middle

		def SetSize(self, height):
			ui.DragButton.SetSize(self, 20, 22)

	def __init__(self):
		ui.Window.__init__(self)

		self.pageSize = 1
		self.curPos = 0.0
		self.eventScroll = lambda *arg: None
		self.lockFlag = False
		self.scrollStep = 0.20

		self.CreateScrollBar()
		self.SetScrollBarSize(256)

	def __del__(self):
		ui.Window.__del__(self)

	def CreateScrollBar(self):
		barSlot = ui.ExpandedImageBox()
		barSlot.SetParent(self)
		barSlot.LoadImage("zaris/fish_wiki/scroll_base.png")
		barSlot.SetPosition(0, 0)
		barSlot.AddFlag("not_pick")
		barSlot.Show()

		middleBar = self.MiddleBar()
		middleBar.SetParent(self)
		middleBar.SetMoveEvent(ui.__mem_func__(self.OnMove))
		middleBar.Show()
		middleBar.MakeImage()

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
		realHeight = self.GetHeight()
		self.SCROLLBAR_MIDDLE_HEIGHT = int(pageScale * float(realHeight))
		self.middleBar.SetSize(self.SCROLLBAR_MIDDLE_HEIGHT)
		self.pageSize = self.GetHeight() - self.SCROLLBAR_MIDDLE_HEIGHT - (self.TEMP_SPACE)

	def SetScrollBarSize(self, height):
		height = 266
		self.pageSize = height - self.SCROLLBAR_MIDDLE_HEIGHT - (self.TEMP_SPACE)
		self.SetSize(self.SCROLLBAR_WIDTH, height)
		self.middleBar.SetRestrictMovementArea(self.MIDDLE_BAR_POS, self.MIDDLE_BAR_UPPER_PLACE, self.MIDDLE_BAR_POS+2, height - self.TEMP_SPACE)
		self.middleBar.SetPosition(self.MIDDLE_BAR_POS, 0)

		self.UpdateBarSlot()

	def UpdateBarSlot(self):
		self.barSlot.SetPosition(0, 0)
		self.barSlot.SetSize(self.GetWidth() - 2, self.GetHeight() - 2)

	def GetPos(self):
		return self.curPos

	def SetPos(self, pos):
		pos = max(0.0, pos)
		pos = min(1.0, pos)

		newPos = float(self.pageSize) * pos
		self.middleBar.SetPosition(self.MIDDLE_BAR_POS, int(newPos) + self.MIDDLE_BAR_UPPER_PLACE)
		self.OnMove()

	def SetScrollStep(self, step):
		self.scrollStep = step

	def GetScrollStep(self):
		return self.scrollStep

	def OnUp(self):
		self.SetPos(self.curPos-self.scrollStep)

	def OnDown(self):
		self.SetPos(self.curPos+self.scrollStep)

	def OnMove(self):
		if self.lockFlag:
			return

		if 0 == self.pageSize:
			return

		(xLocal, yLocal) = self.middleBar.GetLocalPosition()
		self.curPos = float(yLocal - self.MIDDLE_BAR_UPPER_PLACE) / float(self.pageSize)
		self.eventScroll()

	def OnMouseLeftButtonDown(self):
		(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()
		pickedPos = yMouseLocalPosition - self.SCROLLBAR_MIDDLE_HEIGHT/2
		newPos = float(pickedPos) / float(self.pageSize)
		self.SetPos(newPos)

	def LockScroll(self):
		self.lockFlag = True

	def UnlockScroll(self):
		self.lockFlag = False

class FishesListItem(ui.Window):
	def __init__(self, idx, fishVnums, clickEvent):
		ui.Window.__init__(self)

		self.idx = idx
		self.fishVnums = fishVnums
		self.clickEvent = clickEvent
		
		self.isUnlocked = [False, False, False]
		
		self.BuildWindow()

	def __del__(self):
		ui.Window.__del__(self)

	def BuildWindow(self):
		count = 0
		self.fishBtns = {}
		self.fishImgs = {}
		self.fishLockedImgs = {}
		for i in xrange(3):
			if self.fishVnums[i] == 0:
				break
			self.fishBtns[i] = ui.Button()
			self.fishBtns[i].SetParent(self)
			self.fishBtns[i].SetUpVisual("zaris/fish_wiki/slot_norm.png")
			self.fishBtns[i].SetOverVisual("zaris/fish_wiki/slot_hover.png")
			self.fishBtns[i].SetDownVisual("zaris/fish_wiki/slot_down.png")
			self.fishBtns[i].SetDisableVisual("zaris/fish_wiki/slot_disabled.png")
			self.fishBtns[i].SAFE_SetEvent(self.OnClickFish, i)
			self.fishBtns[i].SetPosition(43*i, 0)
			self.fishBtns[i].Show()
			
			item.SelectItem(self.fishVnums[i])
			self.fishImgs[i] = ui.ImageBox()
			self.fishImgs[i].SetParent(self.fishBtns[i])
			self.fishImgs[i].LoadImage(item.GetIconImageFileName())
			self.fishImgs[i].SetPosition(0, 0)
			self.fishImgs[i].AddFlag("not_pick")
			self.fishImgs[i].SetWindowHorizontalAlignCenter()
			self.fishImgs[i].SetWindowVerticalAlignCenter()
			self.fishImgs[i].SetAlpha(0.4)
			self.fishImgs[i].Show()

			self.fishLockedImgs[i] = ui.ImageBox()
			self.fishLockedImgs[i].SetParent(self.fishImgs[i])
			self.fishLockedImgs[i].LoadImage("zaris/fish_wiki/not_unlocked.png")
			self.fishLockedImgs[i].SetPosition(0, 0)
			self.fishLockedImgs[i].AddFlag("not_pick")
			self.fishLockedImgs[i].SetWindowHorizontalAlignCenter()
			self.fishLockedImgs[i].SetWindowVerticalAlignCenter()
			self.fishLockedImgs[i].Show()

			count += 1
		
		startX = 43
		if count == 2:
			startX -= 43/2
		elif count == 3:
			startX = 0

		for i in xrange(count):
			self.fishBtns[i].SetPosition(startX+43*i, 0)

		self.SetSize(43*3, 47)

	def SetIsUnlocked(self, fishIdx, isUnlocked):
		if self.isUnlocked[fishIdx] == isUnlocked:
			return

		self.isUnlocked[fishIdx] = isUnlocked
		if isUnlocked:
			self.fishLockedImgs[fishIdx].Hide()
			self.fishImgs[fishIdx].SetAlpha(1.0)
		else:
			self.fishLockedImgs[fishIdx].Show()
			self.fishImgs[fishIdx].SetAlpha(0.4)

	def OnClickFish(self, i):
		self.clickEvent(self.idx, i)

class FishingWikipediaWindow(ui.Window):
	def __init__(self):
		ui.Window.__init__(self)
		
		self.MY_FISH_DATA = {}
		for vnum in FISH_NEED_RODS.keys():
			self.MY_FISH_DATA[vnum] = [0, 0, 0, 0, False]

		self.RANKING_DATA = {}
		self.RANKING_DATA_LAST_LOAD_TIMES = {}
		
		self.selectedFishCatIdx = -1
		self.selectedFishInCatIdx = -1
		
		self.needCloseBlackBoard = False
		self.blackBoardCloseTime = 0
		
		self.newFishUnlockWindow = None
		
		self.BuildWindow()

	def __del__(self):
		ui.Window.__del__(self)
	
	def BuildWindow(self):
		self.AddFlag('float')
		
		self.Board = ui.BoardWithTitleBar()
		self.Board.SetParent(self)
		self.Board.SetSize(228, 386)
		self.Board.SetPosition(0, 0)
		self.Board.SetTitleName(localeInfo.FISHWIKI_MAIN_WINDOW)
		self.Board.SetCloseEvent(ui.__mem_func__(self.CloseByPlayer))
		self.Board.Show()
		
		self.bg = ui.ImageBox()
		self.bg.SetParent(self.Board)
		self.bg.SetPosition(20, 50)
		self.bg.LoadImage("zaris/fish_wiki/bg_simple.png")
		self.bg.Show()

		self.scrollBar = ScrollBar()
		self.scrollBar.SetParent(self.bg)
		self.scrollBar.SetScrollBarSize(256)
		self.scrollBar.SetPosition(160, 24)
		self.scrollBar.Show()

		self.fishListBox = ui.ListBoxEx()
		self.fishListBox.SetParent(self.bg)
		self.fishListBox.SetPosition(16, 20)
		self.fishListBox.SetSize(129, 45*6)
		self.fishListBox.SetItemSize(129, 45)
		self.fishListBox.SetItemStep(46)
		self.fishListBox.SetViewItemCount(6)
		self.fishListBox.SetScrollBar(self.scrollBar)
		self.fishListBox.Show()
		
		self.fishListItems = {}
		for i in xrange(len(FISHES)):
			self.fishListItems[i] = FishesListItem(i, FISHES[i], ui.__mem_func__(self.OnClickFish))
			self.fishListItems[i].SetParent(self.fishListBox)
			self.fishListBox.AppendItem(self.fishListItems[i])
		
		self.bigBoards = {}
		for i in xrange(3):
			self.bigBoards[i] = ui.Window()
			self.bigBoards[i].SetParent(self.bg)
			self.bigBoards[i].SetSize(348, 320)
			self.bigBoards[i].SetPosition(190, 0)
			self.bigBoards[i].Hide()
			
		self.bw0_bg = ui.ImageBox()
		self.bw0_bg.SetParent(self.bigBoards[0])
		self.bw0_bg.SetPosition(12, 10)
		self.bw0_bg.LoadImage("zaris/fish_wiki/unknown_fish_board.png")
		self.bw0_bg.Show()

		self.bw0_hideBigWindowBtn = ui.Button()
		self.bw0_hideBigWindowBtn.SetParent(self.bigBoards[0])
		self.bw0_hideBigWindowBtn.SetPosition(116, 284)
		self.bw0_hideBigWindowBtn.SetUpVisual("zaris/fish_wiki/btn_normal.png")
		self.bw0_hideBigWindowBtn.SetOverVisual("zaris/fish_wiki/btn_hover.png")
		self.bw0_hideBigWindowBtn.SetDownVisual("zaris/fish_wiki/btn_down.png")
		self.bw0_hideBigWindowBtn.SAFE_SetEvent(self.SetSimpleBoard)
		self.bw0_hideBigWindowBtn.SetText(localeInfo.FISHWIKI_HIDE_WINDOW)
		self.bw0_hideBigWindowBtn.Show()

		self.bw0_needRodTxt = ui.TextLine("Tahoma:12")
		self.bw0_needRodTxt.SetParent(self.bw0_bg)
		self.bw0_needRodTxt.SetPackedFontColor(0xFF79fc79)
		self.bw0_needRodTxt.SetPosition(-1, 170)
		self.bw0_needRodTxt.SetHorizontalAlignCenter()
		self.bw0_needRodTxt.SetWindowHorizontalAlignCenter()
		self.bw0_needRodTxt.SetOutline()
		self.bw0_needRodTxt.SetText("")
		self.bw0_needRodTxt.Show()
			
		self.bw1_bg = ui.ImageBox()
		self.bw1_bg.SetParent(self.bigBoards[1])
		self.bw1_bg.SetPosition(6, 6)
		self.bw1_bg.LoadImage("zaris/fish_wiki/mission_board.png")
		self.bw1_bg.Show()

		self.bw1_fishNameTxt = ui.TextLine("Tahoma:14")
		self.bw1_fishNameTxt.SetParent(self.bw1_bg)
		self.bw1_fishNameTxt.SetPackedFontColor(0xFFD0D0D0)
		self.bw1_fishNameTxt.SetPosition(16, 3)
		self.bw1_fishNameTxt.SetOutline()
		self.bw1_fishNameTxt.SetText("")
		self.bw1_fishNameTxt.Show()

		self.bw1_fishGetCountTxt = ui.TextLine("Tahoma:14")
		self.bw1_fishGetCountTxt.SetParent(self.bw1_bg)
		self.bw1_fishGetCountTxt.SetPackedFontColor(0xFFD0D0D0)
		self.bw1_fishGetCountTxt.SetPosition(16, 40)
		self.bw1_fishGetCountTxt.SetOutline()
		self.bw1_fishGetCountTxt.SetText(localeInfo.FISHWIKI_CATCHED_FISHES)
		self.bw1_fishGetCountTxt.Show()

		self.bw1_fishMyBestLengthTxt = ui.TextLine("Tahoma:14")
		self.bw1_fishMyBestLengthTxt.SetParent(self.bw1_bg)
		self.bw1_fishMyBestLengthTxt.SetPackedFontColor(0xFFD0D0D0)
		self.bw1_fishMyBestLengthTxt.SetPosition(16, 60)
		self.bw1_fishMyBestLengthTxt.SetOutline()
		self.bw1_fishMyBestLengthTxt.SetText(localeInfo.FISHWIKI_LONGEST_FISH)
		self.bw1_fishMyBestLengthTxt.Show()

		self.bw1_fishMyBestPriceTxt = ui.TextLine("Tahoma:14")
		self.bw1_fishMyBestPriceTxt.SetParent(self.bw1_bg)
		self.bw1_fishMyBestPriceTxt.SetPackedFontColor(0xFFD0D0D0)
		self.bw1_fishMyBestPriceTxt.SetPosition(16, 80)
		self.bw1_fishMyBestPriceTxt.SetOutline()
		self.bw1_fishMyBestPriceTxt.SetText(localeInfo.FISHWIKI_MOST_EXPENSIVE_FISH)
		self.bw1_fishMyBestPriceTxt.Show()

		self.bw1_showRankingBtn = ui.Button()
		self.bw1_showRankingBtn.SetParent(self.bigBoards[1])
		self.bw1_showRankingBtn.SetPosition(284, 56)
		self.bw1_showRankingBtn.SetUpVisual("zaris/fish_wiki/rank_btn_norm.png")
		self.bw1_showRankingBtn.SetOverVisual("zaris/fish_wiki/rank_btn_hover.png")
		self.bw1_showRankingBtn.SetDownVisual("zaris/fish_wiki/rank_btn_down.png")
		self.bw1_showRankingBtn.SAFE_SetEvent(self.OnClickRankingButton)
		self.bw1_showRankingBtn.SetText("")
		self.bw1_showRankingBtn.SetToolTipText(localeInfo.FISHWIKI_SHOW_RANKING, 0, -10)
		self.bw1_showRankingBtn.Show()

		self.bw1_missionInfoTxt = ui.TextLine("Tahoma:12")
		self.bw1_missionInfoTxt.SetParent(self.bw1_bg)
		self.bw1_missionInfoTxt.SetPackedFontColor(0xFFD0D0D0)
		self.bw1_missionInfoTxt.SetPosition(16, 172)
		self.bw1_missionInfoTxt.SetOutline()
		self.bw1_missionInfoTxt.SetText(localeInfo.FISHWIKI_GIVE_10_FISHES)
		self.bw1_missionInfoTxt.Show()

		self.bw1_missionChanceInfoTxt = ui.TextLine("Tahoma:12")
		self.bw1_missionChanceInfoTxt.SetParent(self.bw1_bg)
		self.bw1_missionChanceInfoTxt.SetPackedFontColor(0xFFD0D0D0)
		self.bw1_missionChanceInfoTxt.SetPosition(16, 190)
		self.bw1_missionChanceInfoTxt.SetOutline()
		self.bw1_missionChanceInfoTxt.SetText(localeInfo.FISHWIKI_GIVE_CHANCE)
		self.bw1_missionChanceInfoTxt.Show()

		self.bw1_missionRewardInfoTxt = ui.TextLine("Tahoma:12")
		self.bw1_missionRewardInfoTxt.SetParent(self.bw1_bg)
		self.bw1_missionRewardInfoTxt.SetPackedFontColor(0xFFD0D0D0)
		self.bw1_missionRewardInfoTxt.SetPosition(16, 208)
		self.bw1_missionRewardInfoTxt.SetOutline()
		self.bw1_missionRewardInfoTxt.SetText(localeInfo.FISHWIKI_REWARD_FOR_MISSION)
		self.bw1_missionRewardInfoTxt.Show()

		self.bw1_missionProgressInfoTxt = ui.TextLine("Tahoma:12")
		self.bw1_missionProgressInfoTxt.SetParent(self.bw1_bg)
		self.bw1_missionProgressInfoTxt.SetPackedFontColor(0xFFD0D0D0)
		self.bw1_missionProgressInfoTxt.SetPosition(311, 227)
		self.bw1_missionProgressInfoTxt.SetOutline()
		self.bw1_missionProgressInfoTxt.SetText(localeInfo.FISH_PROGRESS_INFO)
		self.bw1_missionProgressInfoTxt.SetHorizontalAlignCenter()
		self.bw1_missionProgressInfoTxt.Show()

		self.bw1_giveMissionFishBtn = ui.Button()
		self.bw1_giveMissionFishBtn.SetParent(self.bigBoards[1])
		self.bw1_giveMissionFishBtn.SetPosition(114, 284)
		self.bw1_giveMissionFishBtn.SetUpVisual("zaris/fish_wiki/btn_normal.png")
		self.bw1_giveMissionFishBtn.SetOverVisual("zaris/fish_wiki/btn_hover.png")
		self.bw1_giveMissionFishBtn.SetDownVisual("zaris/fish_wiki/btn_down.png")
		self.bw1_giveMissionFishBtn.SetDisableVisual("zaris/fish_wiki/btn_disable.png")
		self.bw1_giveMissionFishBtn.SAFE_SetEvent(self.OnClickGiveFishBtn)
		self.bw1_giveMissionFishBtn.SetText(localeInfo.FISH_GIVE_MISSION_BTN)
		self.bw1_giveMissionFishBtn.Show()
			
		self.bw2_bg = ui.ImageBox()
		self.bw2_bg.SetParent(self.bigBoards[2])
		self.bw2_bg.SetPosition(22, 6)
		self.bw2_bg.LoadImage("zaris/fish_wiki/rank_board.png")
		self.bw2_bg.Show()
		
		self.rankingPos = ui.TextLine()
		self.rankingPos.SetParent(self.bigBoards[2])
		self.rankingPos.SetText("Pos.")
		self.rankingPos.SetPosition(-133, 36)
		self.rankingPos.SetPackedFontColor(0xFFe2c79a)
		self.rankingPos.SetOutline()
		self.rankingPos.SetFontName("Verdana:12b")
		self.rankingPos.SetWindowHorizontalAlignCenter()
		self.rankingPos.SetHorizontalAlignCenter()
		self.rankingPos.Show()
		
		self.rankingName = ui.TextLine()
		self.rankingName.SetParent(self.bigBoards[2])
		self.rankingName.SetText("Name")
		self.rankingName.SetPosition(-71, 36)
		self.rankingName.SetPackedFontColor(0xFFe2c79a)
		self.rankingName.SetOutline()
		self.rankingName.SetFontName("Verdana:12b")
		self.rankingName.SetWindowHorizontalAlignCenter()
		self.rankingName.SetHorizontalAlignCenter()
		self.rankingName.Show()
		
		self.rankingLenght = ui.TextLine()
		self.rankingLenght.SetParent(self.bigBoards[2])
		self.rankingLenght.SetText("Fish Lenght")
		self.rankingLenght.SetPosition(20, 36)
		self.rankingLenght.SetPackedFontColor(0xFFe2c79a)
		self.rankingLenght.SetOutline()
		self.rankingLenght.SetFontName("Verdana:12b")
		self.rankingLenght.SetWindowHorizontalAlignCenter()
		self.rankingLenght.SetHorizontalAlignCenter()
		self.rankingLenght.Show()
		
		self.rankingRod = ui.TextLine()
		self.rankingRod.SetParent(self.bigBoards[2])
		self.rankingRod.SetText("Fishing Rod")
		self.rankingRod.SetPosition(107, 36)
		self.rankingRod.SetPackedFontColor(0xFFe2c79a)
		self.rankingRod.SetOutline()
		self.rankingRod.SetFontName("Verdana:12b")
		self.rankingRod.SetWindowHorizontalAlignCenter()
		self.rankingRod.SetHorizontalAlignCenter()
		self.rankingRod.Show()

		self.bw2_fishRankNameTxt = ui.TextLine("Tahoma:12")
		self.bw2_fishRankNameTxt.SetParent(self.bw2_bg)
		self.bw2_fishRankNameTxt.SetPackedFontColor(0xFFD0D0D0)
		self.bw2_fishRankNameTxt.SetPosition(12, 6)
		self.bw2_fishRankNameTxt.SetOutline()
		self.bw2_fishRankNameTxt.SetText(localeInfo.FISHWIKI_LENGTH_RANKING)
		self.bw2_fishRankNameTxt.Show()

		self.bw1_backToFishStatsBtn = ui.Button()
		self.bw1_backToFishStatsBtn.SetParent(self.bigBoards[2])
		self.bw1_backToFishStatsBtn.SetPosition(115, 284)
		self.bw1_backToFishStatsBtn.SetUpVisual("zaris/fish_wiki/btn_normal.png")
		self.bw1_backToFishStatsBtn.SetOverVisual("zaris/fish_wiki/btn_hover.png")
		self.bw1_backToFishStatsBtn.SetDownVisual("zaris/fish_wiki/btn_down.png")
		self.bw1_backToFishStatsBtn.SAFE_SetEvent(self.OnClickReturnToStats)
		self.bw1_backToFishStatsBtn.SetText(uiScriptLocale.CREATE_PREV)
		self.bw1_backToFishStatsBtn.Show()
		
		self.rankTxts = {}
		for i in xrange(10):
			rankIdxTxt = ui.TextLine("Tahoma:12")
			rankIdxTxt.SetParent(self.bw2_bg)
			rankIdxTxt.SetPackedFontColor(0xFFD0D0D0)
			rankIdxTxt.SetPosition(18, 51+i*21)
			rankIdxTxt.SetOutline()
			rankIdxTxt.SetText("%d."%(i+1))
			rankIdxTxt.SetHorizontalAlignCenter()
			rankIdxTxt.Show()
			playerNameTxt = ui.TextLine("Tahoma:12")
			playerNameTxt.SetParent(self.bw2_bg)
			playerNameTxt.SetPackedFontColor(0xFFD0D0D0)
			playerNameTxt.SetPosition(41, 51+i*21)
			playerNameTxt.SetOutline()
			playerNameTxt.SetText("---")
			playerNameTxt.Show()
			fishLengthTxt = ui.TextLine("Tahoma:12")
			fishLengthTxt.SetParent(self.bw2_bg)
			fishLengthTxt.SetPackedFontColor(0xFFD0D0D0)
			fishLengthTxt.SetPosition(133, 51+i*21)
			fishLengthTxt.SetOutline()
			fishLengthTxt.SetText(localeInfo.FISH_LENGTH_DEFAULT)
			fishLengthTxt.Show()
			rodLevelTxt = ui.TextLine("Tahoma:12")
			rodLevelTxt.SetParent(self.bw2_bg)
			rodLevelTxt.SetPackedFontColor(0xFFD0D0D0)
			rodLevelTxt.SetPosition(222, 51+i*21)
			rodLevelTxt.SetOutline()
			rodLevelTxt.SetText(localeInfo.FISHWIKI_ROD_LEVEL_0)
			rodLevelTxt.Show()
			self.rankTxts[i] = [rankIdxTxt, playerNameTxt, fishLengthTxt, rodLevelTxt]

		self.blackBoardLoading = ui.ImageBox()
		self.blackBoardLoading.SetParent(self.bg)
		self.blackBoardLoading.SetPosition(0, 0)
		self.blackBoardLoading.LoadImage("zaris/fish_wiki/blackboard_bg.png")
		self.blackBoardLoading.Hide()

		self.SetSize(228, 386)
		self.SetCenterPosition()
		self.Hide()

	def OnUnlockNewFish(self, fishVnum):
		if self.newFishUnlockWindow == None:
			self.newFishUnlockWindow = NewFishFoundWindow()
		self.newFishUnlockWindow.OnCatchNewFish(fishVnum)

	def OnLoadUnlockStatus(self, unlockData1, unlockData2):
		unlockData = [int(unlockData1), int(unlockData2)]
		catIdx = 0
		inCatIdx = 0
		unlockIdx = 0
		dataIdx = 0
		for i in xrange(len(FISHES)*3):
			if FISHES[catIdx][inCatIdx] == 0:
				continue

			if unlockData[unlockIdx] & (1 << dataIdx):
				self.fishListItems[catIdx].SetIsUnlocked(inCatIdx, True)
			else:
				self.fishListItems[catIdx].SetIsUnlocked(inCatIdx, False)

			inCatIdx += 1
			if inCatIdx == 3:
				inCatIdx = 0
				catIdx += 1
			
			if dataIdx >= 30:
				dataIdx = 0
				unlockIdx += 1
			else:
				dataIdx += 1

		if not self.IsShow():
			self.Show()

	def OnUpdate(self):	
		if self.needCloseBlackBoard:
			if app.GetGlobalTime() >= self.blackBoardCloseTime:
				self.needCloseBlackBoard = False
				self.blackBoardLoading.Hide()

	def OnLoadError(self, status):
		self.needCloseBlackBoard = True

	def OnLoadMyFishData(self, loadedFishVnum, myGetCount, bestLength, bestPrice, missionGivedCount):
		self.MY_FISH_DATA[loadedFishVnum] = [myGetCount, bestLength, bestPrice, missionGivedCount, True]
		if loadedFishVnum == FISHES[self.selectedFishCatIdx][self.selectedFishInCatIdx]:
			item.SelectItem(loadedFishVnum)
			self.RefreshMyFishData(loadedFishVnum)
		self.needCloseBlackBoard = True

	def OnLoadRankingStart(self):
		fishVnum = FISHES[self.selectedFishCatIdx][self.selectedFishInCatIdx]
		self.RANKING_DATA_LAST_LOAD_TIMES[fishVnum] = app.GetGlobalTimeStamp()
		self.RANKING_DATA[fishVnum] = {}
		for i in xrange(10):
			self.RANKING_DATA[fishVnum][i] = ["-----", 0, 0]

	def OnLoadRanking(self, pos, name, length, rodLevel):
		fishVnum = FISHES[self.selectedFishCatIdx][self.selectedFishInCatIdx]
		self.RANKING_DATA[fishVnum][pos] = [name, length, rodLevel]

	def OnLoadRankingEnd(self):
		fishVnum = FISHES[self.selectedFishCatIdx][self.selectedFishInCatIdx]
		for i in xrange(10):
			if self.RANKING_DATA[fishVnum][i][1] == 0:
				self.rankTxts[i][1].SetText("-----")
				self.rankTxts[i][2].SetText(localeInfo.FISH_LENGTH_DEFAULT)
				self.rankTxts[i][3].SetText("")
			else:
				self.rankTxts[i][1].SetText(self.RANKING_DATA[fishVnum][i][0])
				self.rankTxts[i][2].SetText("%.2f cm."%(float(self.RANKING_DATA[fishVnum][i][1]) / 100.0))
				self.rankTxts[i][3].SetText(localeInfo.FISHWIKI_ROD_LEVEL % self.RANKING_DATA[fishVnum][i][2])
		self.needCloseBlackBoard = True

	def RefreshMyFishData(self, fishVnum):
		self.bw1_fishGetCountTxt.SetText(localeInfo.FISHWIKI_CATCHED_FISHES_COUNT % self.MY_FISH_DATA[fishVnum][0])
		self.bw1_fishMyBestLengthTxt.SetText(localeInfo.FISHWIKI_LONGEST_FISH_CM % (float(self.MY_FISH_DATA[fishVnum][1]) / 100.0))
		self.bw1_fishMyBestPriceTxt.SetText(localeInfo.FISHWIKI_MOST_EXPENSIVE_FISH_YANG % localeInfo.NumberToString(self.MY_FISH_DATA[fishVnum][2]))

		self.bw1_missionInfoTxt.SetText(localeInfo.FISHWIKI_GIVE_FISHES % (FISH_MISSION_DATA[fishVnum][0], item.GetItemName(), FISH_MISSION_DATA[fishVnum][1]))
		self.bw1_missionChanceInfoTxt.SetText(localeInfo.FISHWIKI_GIVE_CHANCE_SUCCESS % FISH_MISSION_DATA[fishVnum][2])
		self.bw1_missionRewardInfoTxt.SetText(localeInfo.FISHWIKI_REWARD_FOR_MISSION_FISH % FISH_MISSION_DATA[fishVnum][3])
		self.bw1_missionProgressInfoTxt.SetText(localeInfo.FISHWIKI_PROGRESS_STATE % (self.MY_FISH_DATA[fishVnum][3], FISH_MISSION_DATA[fishVnum][0]))

		if self.MY_FISH_DATA[fishVnum][3] < FISH_MISSION_DATA[fishVnum][0]:
			self.bw1_giveMissionFishBtn.Enable()
		else:
			self.bw1_giveMissionFishBtn.Disable()

	def OnClickReturnToStats(self):
		self.OnClickFish(self.selectedFishCatIdx, self.selectedFishInCatIdx)

	def OnClickRankingButton(self):
		fishVnum = FISHES[self.selectedFishCatIdx][self.selectedFishInCatIdx]
		item.SelectItem(fishVnum)
		self.bw2_fishRankNameTxt.SetText(localeInfo.FISHWIKI_LENGTH_RANKING_BEST % item.GetItemName())
		self.SetBigBoard(2)
		
		if (fishVnum in self.RANKING_DATA_LAST_LOAD_TIMES and app.GetGlobalTimeStamp() >= self.RANKING_DATA_LAST_LOAD_TIMES[fishVnum]+15*60) or fishVnum not in self.RANKING_DATA_LAST_LOAD_TIMES:
			self.blackBoardLoading.Show()
			self.needCloseBlackBoard = False
			self.blackBoardCloseTime = app.GetGlobalTime() + 400
			net.SendChatPacket("/fish_wiki 4 %d"%fishVnum)
			chat.AppendChat(1, "[TEST] Load ranking from server")
		else:
			self.OnLoadRankingEnd()
			chat.AppendChat(1, "[TEST] Load ranking from memory")

	def OnClickGiveFishBtn(self):
		fishVnum = FISHES[self.selectedFishCatIdx][self.selectedFishInCatIdx]
		net.SendChatPacket("/fish_wiki 3 %d"%fishVnum)

	def OnClickFish(self, catIdx, idx):
		if self.selectedFishCatIdx != -1:
			self.fishListItems[self.selectedFishCatIdx].fishBtns[self.selectedFishInCatIdx].Enable()
		self.fishListItems[catIdx].fishBtns[idx].Disable()

		self.selectedFishCatIdx = catIdx
		self.selectedFishInCatIdx = idx
		
		fishVnum = FISHES[catIdx][idx]

		if self.fishListItems[catIdx].isUnlocked[idx]:
			item.SelectItem(fishVnum)
			self.bw1_fishNameTxt.SetText(item.GetItemName())
			
			self.RefreshMyFishData(fishVnum)
			if not self.MY_FISH_DATA[fishVnum][4]:
				self.blackBoardLoading.Show()
				self.needCloseBlackBoard = False
				self.blackBoardCloseTime = app.GetGlobalTime() + 400
				net.SendChatPacket("/fish_wiki 2 %d"%fishVnum)
				chat.AppendChat(1, "[TEST] Load fish data from server")

			self.SetBigBoard(1)
		else:
			self.bw0_needRodTxt.SetText(FISH_NEED_RODS[fishVnum])

			self.SetBigBoard(0)

	def SetBigBoard(self, bigBoardIdx):
		self.bg.LoadImage("zaris/fish_wiki/bg.png")
		self.Board.SetSize(576, 386)
		self.SetSize(576, 386)
		self.SetCenterPosition()
		
		for i in xrange(3):
			self.bigBoards[i].Hide()
		self.bigBoards[bigBoardIdx].Show()

	def SetSimpleBoard(self):
		if self.selectedFishCatIdx != -1:
			self.fishListItems[self.selectedFishCatIdx].fishBtns[self.selectedFishInCatIdx].Enable()
		self.selectedFishCatIdx = -1
		self.selectedFishInCatIdx = -1

		self.bg.LoadImage("zaris/fish_wiki/bg_simple.png")
		self.Board.SetSize(228, 386)
		self.SetSize(228, 386)
		self.SetCenterPosition()
		
		for i in xrange(3):
			self.bigBoards[i].Hide()

	def CloseByPlayer(self):
		net.SendChatPacket("/fish_wiki 1")
		self.Close()

	def Close(self):
		self.Hide()
		self.SetSimpleBoard()

	def OnPressEscapeKey(self):
		if self.IsShow():
			self.CloseByPlayer()
			return TRUE
		return FALSE

class NewFishFoundWindow(ui.Window):
	def __init__(self):
		ui.Window.__init__(self, "TOP_MOST")
		
		self.closeTime = 0
		
		self.BuildWindow()

	def __del__(self):
		ui.Window.__del__(self)
	
	def BuildWindow(self):
		self.AddFlag('float')

		self.boardBg = ui.ImageBox()
		self.boardBg.SetParent(self)
		self.boardBg.SetPosition(0, 0)
		self.boardBg.LoadImage("zaris/notification/background_item.png")
		self.boardBg.Show()

		self.fishText = ui.TextLine()
		self.fishText.SetParent(self)
		self.fishText.SetText(localeInfo.FISHWIKI_NOTIFICATION)
		self.fishText.SetPosition(0, 56)
		self.fishText.SetOutline()
		self.fishText.SetFontName("Verdana:12b")
		self.fishText.SetWindowHorizontalAlignCenter()
		self.fishText.SetHorizontalAlignCenter()
		self.fishText.Show()

		self.fishText1 = ui.TextLine()
		self.fishText1.SetParent(self)
		self.fishText1.SetText(localeInfo.FISHWIKI_NOTIFICATION_TEXT)
		self.fishText1.SetPosition(0, 82)
		self.fishText1.SetPackedFontColor(0xFFe2c79a)
		self.fishText1.SetOutline()
		self.fishText1.SetFontName("Verdana:12b")
		self.fishText1.SetWindowHorizontalAlignCenter()
		self.fishText1.SetHorizontalAlignCenter()
		self.fishText1.Show()

		self.fishItemImg = ui.ImageBox()
		self.fishItemImg.SetParent(self)
		self.fishItemImg.SetPosition(115, 16)
		self.fishItemImg.Show()

		self.SetSize(264, 113)
		self.SetPosition(wndMgr.GetScreenWidth()/2-264/2, 30)
		self.Hide()

	def OnUpdate(self):
		if app.GetGlobalTimeStamp() >= self.closeTime:
			self.Hide()

	def OnCatchNewFish(self, fishVnum):
		self.closeTime = app.GetGlobalTimeStamp() + 5
		item.SelectItem(fishVnum)
		self.fishItemImg.LoadImage(item.GetIconImageFileName())
		self.Show()
