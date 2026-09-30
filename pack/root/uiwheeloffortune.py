import ui, app, item, net, constInfo, localeInfo, wndMgr, chat
import math

IMG_DIR = "zaris/whell_of_fortune/"
# Add separate image directories for different modes
IMG_DIR_ITEM = "zaris/whell_of_fortune/item/"
IMG_DIR_GOLD = "zaris/whell_of_fortune/gold/"

INFO_MODE = 0

# Item wheel rewards
item_list = [\
	[40335,1],[40336,1],[40337,1],[40338,1],[40339,1],[40340,1],[40341,1],[41738,1],[41739,1],[46034,1],[46035,1],[85559,1],[201231,1],[80040,1],[201229,1],[201224,1],[201225,1]\
	,[201226,1],[201227,1],[201228,1],[201222,20],[201223,5],[201221,1],[80032,10],[52940,1],[52941,1],[53930,1],[53931,1],[203000,3],[203001,3],[203002,3],[203003,3]
]

# Gold wheel rewards (different items for gold mode)
gold_item_list = [\
	[50166,1],[50167,1],[80040,1],[203000,5],[203001,5],[203002,5],[203003,5],[80032,30],[10151,1],[10156,1],[10161,1],[61400,1],[79016,2],[79016,3],[79016,4],[100700,2]\
	,[100700,3],[100700,4],[80008,20000],[31150,35],[31151,35],[31152,35],[31153,35],[31154,35],[31155,35],[90029,3],[203034,1],[71180,1]
]

class Grid: # from KeN
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
	def clear(self, pos, width, height):
		if pos < 0 or pos >= (self.width * self.height):
			return
		for row in range(height):
			start = pos + (row * self.width)
			self.grid[start] = True
			col = 1
			while col < width:
				self.grid[start + col] = False
				col += 1
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
	def SetTextType(self, textType):
		self.textType = textType
		for text in self.children:
			self.AddTextType(self.textType.split("#"),text)
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
			if self.textType != "":
				self.AddTextType(self.textType.split("#"),childText)
			childText.SetText(str(text))
			childText.Show()
			self.children.append(childText)
			yPosition+=self.rangeText

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
			text = "%s\nx%d %s\n%s"%(localeInfo.WHEEL_REWARD_TEXT, itemCount,item.GetItemName(), localeInfo.WHEEL_REWARD_TEXT_1)
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

class WheelofFortune(ui.BoardWithTitleBar):
	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)
	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.Destroy()
		self.CreateGame()
	def Destroy(self):
		self.children = {}
		
		# Simple animation variables - much cleaner!
		self.isSpinning = False
		self.currentRotation = 0.0
		self.targetRotation = 0.0
		self.spinSpeed = 0.0
		self.animationStartTime = 0.0
		self.animationDuration = 4.0  # Total spin time in seconds
		
		self.giftIndex = 0
		self.showAnimation = True
		self.rewardData = []
		self.wheelMode = 0  # 0 = item mode, 1 = gold mode
		
	def Open(self):
		self.Show()
		self.SetTop()
		self.SetCenterPosition()
		
	def Close(self):
		self.CloseMessageWindow()
		
		# Stop any running animation
		self.isSpinning = False
		
		self.Hide()
		
	def OnPressEscapeKey(self):
		self.Close()
		return True

	def GetCurrentImageDir(self):
		"""Get the current image directory based on wheel mode"""
		if self.wheelMode == 0:  # Item mode
			return IMG_DIR_ITEM
		else:  # Gold mode
			return IMG_DIR_GOLD

	def LoadImageSafely(self, imageBox, imagePath, fallbackPath=None):
		"""Load image with fallback to original path if mode-specific path fails"""
		try:
			imageBox.LoadImage(imagePath)
			return True
		except:
			if fallbackPath:
				try:
					imageBox.LoadImage(fallbackPath)
					return True
				except:
					pass
			return False

	def LoadButtonSafely(self, button, upPath, overPath, downPath, fallbackDir=None):
		"""Load button visuals with fallback"""
		try:
			button.SetUpVisual(upPath)
			button.SetOverVisual(overPath)
			button.SetDownVisual(downPath)
			return True
		except:
			if fallbackDir:
				try:
					button.SetUpVisual(fallbackDir + "wheel_0.tga")
					button.SetOverVisual(fallbackDir + "wheel_1.tga")
					button.SetDownVisual(fallbackDir + "wheel_2.tga")
					return True
				except:
					pass
			return False

	def UpdateWallpaper(self):
		"""Update the wallpaper based on current mode"""
		if "board" in self.children:
			current_dir = self.GetCurrentImageDir()
			wallpaper_path = current_dir + "wallpaper.tga"
			fallback_path = IMG_DIR + "wallpaper.tga"
			
			if not self.LoadImageSafely(self.children["board"], wallpaper_path, fallback_path):
				chat.AppendChat(chat.CHAT_TYPE_INFO, "Failed to load wallpaper from: " + wallpaper_path)
		
	def CreateGame(self):
		self.SetSize(449, 700)
		self.SetCenterPosition()
		self.SetTitleName(localeInfo.WHEEL_GAME_TITLE)
		self.SetCloseEvent(self.Close)
		self.AddFlag("movable")
		self.AddFlag("attach")
		self.AddFlag("float")

		board = ui.ImageBox()
		board.SetParent(self)
		# Start with original wallpaper path as fallback
		board.LoadImage(IMG_DIR + "wallpaper.tga")
		board.SetPosition(5,28)
		board.Show()
		self.children["board"] = board

		# Mode switch buttons - use fallback to original button images
		itemModeBtn = ui.Button()
		itemModeBtn.SetParent(board)
		# Try to load item mode button images, fallback to default
		if not self.LoadImageSafely(itemModeBtn, IMG_DIR_ITEM + "button-tab-0.png", "button-tab-0.png"):
			itemModeBtn.SetUpVisual(IMG_DIR_ITEM + "button-tab-0.png")
			itemModeBtn.SetOverVisual(IMG_DIR_ITEM + "button-tab-1.png")
			itemModeBtn.SetDownVisual(IMG_DIR_ITEM + "button-tab-2.png")
		itemModeBtn.SetText(localeInfo.WHEEL_GAME_ITEM_MODE)
		itemModeBtn.SetEvent(ui.__mem_func__(self.SetItemMode))
		itemModeBtn.SetPosition(346, 2)
		itemModeBtn.Show()
		self.children["itemModeBtn"] = itemModeBtn

		goldModeBtn = ui.Button()
		goldModeBtn.SetParent(board)
		# Try to load gold mode button images, fallback to default
		if not self.LoadImageSafely(goldModeBtn, IMG_DIR_GOLD + "button-tab-0.png", "button-tab-0.png"):
			goldModeBtn.SetUpVisual(IMG_DIR_GOLD + "button-tab-0.png")
			goldModeBtn.SetOverVisual(IMG_DIR_GOLD + "button-tab-1.png")
			goldModeBtn.SetDownVisual(IMG_DIR_GOLD + "button-tab-2.png")
		goldModeBtn.SetText(localeInfo.WHEEL_GAME_CHEQUE_MODE)
		goldModeBtn.SetEvent(ui.__mem_func__(self.SetGoldMode))
		goldModeBtn.SetPosition(346, 36)
		goldModeBtn.Show()
		self.children["goldModeBtn"] = goldModeBtn

		# Use original directory for images initially, will be updated when mode changes
		fortuneImg = ui.ImageBox()
		fortuneImg.SetParent(board)
		fortuneImg.AddFlag("not_pick")
		fortuneImg.LoadImage(IMG_DIR + "fortune.tga")
		fortuneImg.SetPosition(28,23)
		fortuneImg.Show()
		self.children["fortuneImg"] = fortuneImg

		fortuneSingle = ui.ExpandedImageBox()
		fortuneSingle.SetParent(board)
		fortuneSingle.AddFlag("not_pick")
		fortuneSingle.LoadImage(IMG_DIR + "single.tga")
		fortuneSingle.SetPosition(164,38)
		fortuneSingle.Show()
		self.children["fortuneSingle"] = fortuneSingle

		wheelCircle = ui.ExpandedImageBox()
		wheelCircle.SetParent(board)
		wheelCircle.LoadImage(IMG_DIR + "circle.tga")
		wheelCircle.AddFlag("not_pick")
		wheelCircle.SetPosition(155,136)
		wheelCircle.Show()
		self.children["wheelCircle"] = wheelCircle

		wheelBtn = ui.Button()
		wheelBtn.SetParent(board)
		wheelBtn.SetUpVisual(IMG_DIR + "wheel_0.tga")
		wheelBtn.SetOverVisual(IMG_DIR + "wheel_1.tga")
		wheelBtn.SetDownVisual(IMG_DIR + "wheel_2.tga")
		wheelBtn.SetPosition(160,155)
		wheelBtn.SetEvent(ui.__mem_func__(self.StartWheel))
		wheelBtn.Show()
		self.children["wheelBtn"] = wheelBtn

		# Cost item icon (will change based on mode)
		wheelBtnItem = ui.ImageBox()
		wheelBtnItem.SetParent(wheelBtn)
		wheelBtnItem.LoadImage("icon/item/summer/220570.png")  # Default to item token
		wheelBtnItem.SetPosition(wheelBtn.GetWidth() / 2 - 16, wheelBtn.GetHeight() / 2 - 40)
		wheelBtnItem.AddFlag("not_pick")
		wheelBtnItem.Show()
		self.children["wheelBtnItem"] = wheelBtnItem

		# Cost display text
		costText = ui.TextLine()
		costText.SetParent(wheelBtn)
		costText.SetFontName("Tahoma:14b")
		costText.SetText(localeInfo.WHEEL_GAME_ITEM_WHEEL_COST)
		costText.SetPosition(wheelBtn.GetWidth() / 2, wheelBtn.GetHeight() / 2 - 10)
		costText.SetHorizontalAlignCenter()
		costText.SetOutline()
		costText.Show()
		self.children["costText"] = costText

		# Gold cost text (hidden by default)
		goldCostText = ui.TextLine()
		goldCostText.SetParent(wheelBtn)
		goldCostText.SetFontName("Tahoma:14b")
		goldCostText.SetText(localeInfo.WHEEL_GAME_ITEM_GOLD_COST)
		goldCostText.SetPosition(wheelBtn.GetWidth() / 2, wheelBtn.GetHeight() / 2 - 10)
		goldCostText.SetHorizontalAlignCenter()
		goldCostText.SetOutline()
		goldCostText.Hide()
		self.children["goldCostText"] = goldCostText

		if INFO_MODE:
			debugText = ui.MultiTextLine()
			debugText.SetParent(board)
			debugText.SetPosition(180,140+wheelCircle.GetHeight())
			debugText.Show()
			self.children["debugText"] = debugText

		event = lambda index = 0 : ui.__mem_func__(self.SetAnimationStatus)(index)
		self.children["animationCheckBox"] = ui.CheckBoxNew(board, 10, 10, event)

		animationText = ui.TextLine()
		animationText.SetParent(board)
		animationText.SetText(localeInfo.WHEEL_GAME_SKIP_ANIMATION)
		animationText.SetOutline()
		animationText.SetPosition(10+self.children["animationCheckBox"].GetWidth()+10,10)
		animationText.Show()
		self.children["animationText"] = animationText

		# Add title background and text
		titleBg = ui.ImageBox()
		titleBg.SetParent(self)
		titleBg.LoadImage(IMG_DIR + "title_bg.tga")
		titleBg.SetPosition((self.GetWidth() / 2) - 197, 28 + board.GetHeight() + 10)
		titleBg.AddFlag("not_pick")
		titleBg.Show()
		self.children["titleBg"] = titleBg

		titleText = ui.TextLine()
		titleText.SetParent(titleBg)
		titleText.SetText(localeInfo.WHEEL_OF_FORTUNE_REWARDS)
		titleText.SetWindowHorizontalAlignCenter()
		titleText.SetWindowVerticalAlignCenter()
		titleText.SetVerticalAlignCenter()
		titleText.SetHorizontalAlignCenter()
		titleText.SetPosition(0, 0)
		titleText.Show()
		self.children["titleText"] = titleText

		# Add item slot window for rewards display
		wndItem = ui.GridSlotWindow()
		wndItem.SetParent(self)
		wndItem.SetPosition(32, 28 + board.GetHeight() + 10 + titleBg.GetHeight() + 15)
		wndItem.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
		wndItem.SetOverInItemEvent(ui.__mem_func__(self.__OverIn))
		wndItem.SetOverOutItemEvent(ui.__mem_func__(self.__OverOut))
		wndItem.ArrangeSlot(0, 12, 6, 32, 32, 0, 0)
		wndItem.RefreshSlot()
		wndItem.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)
		wndItem.Show()
		wndItem.itemDataDict = {}
		self.children["wndItem"] = wndItem

		# Wheel item positions
		itemPosition = [[205,65], [275,90], [320,160], [320,235], [275,290], [205,320], [130,295], [90,235], [85,160], [130,90]]
		for j in xrange(len(itemPosition)):
			wheelItem = ImageBoxSpecial()
			wheelItem.SetParent(board)
			wheelItem.SetPosition(itemPosition[j][0],itemPosition[j][1])
			wheelItem.SetSize(32,32)
			wheelItem.Show()
			self.children["wheelItem%d"%j] = wheelItem
		
		self.SetTrailerMode()
		self.UpdateModeDisplay()

	def SetItemMode(self):
		self.wheelMode = 0
		self.UpdateModeDisplay()
		self.UpdateWallpaper()  # Update wallpaper when mode changes
		self.UpdateAllModeImages()  # Update all mode-specific images
		self.ResetSingleFortune()  # Reset pointer position
		self.SetTrailerMode()
	
	def SetGoldMode(self):
		self.wheelMode = 1
		self.UpdateModeDisplay()
		self.UpdateWallpaper()  # Update wallpaper when mode changes
		self.UpdateAllModeImages()  # Update all mode-specific images
		self.ResetSingleFortune()  # Reset pointer position
		self.SetTrailerMode()
		
	def ResetSingleFortune(self):
		"""Reset the single fortune pointer to its default position and rotation"""
		if "fortuneSingle" in self.children:
			self.children["fortuneSingle"].SetPosition(164, 38)  # Default position
			self.children["fortuneSingle"].SetRotation(0)  # Default rotation
			
		# Reset current rotation state
		self.currentRotation = 0.0
		self.targetRotation = 0.0
		self.isSpinning = False

	def UpdateAllModeImages(self):
		"""Update all images that should change based on mode"""
		current_dir = self.GetCurrentImageDir()
		
		# Update fortune image
		if "fortuneImg" in self.children:
			fortune_path = current_dir + "fortune.tga"
			fallback_path = IMG_DIR + "fortune.tga"
			self.LoadImageSafely(self.children["fortuneImg"], fortune_path, fallback_path)
		
		# Update single fortune image
		if "fortuneSingle" in self.children:
			single_path = current_dir + "single.tga"
			fallback_path = IMG_DIR + "single.tga"
			self.LoadImageSafely(self.children["fortuneSingle"], single_path, fallback_path)
		
		# Update wheel circle
		if "wheelCircle" in self.children:
			circle_path = current_dir + "circle.tga"
			fallback_path = IMG_DIR + "circle.tga"
			self.LoadImageSafely(self.children["wheelCircle"], circle_path, fallback_path)
		
		# Update wheel button images
		if "wheelBtn" in self.children:
			wheelBtn = self.children["wheelBtn"]
			wheel0_path = current_dir + "wheel_0.tga"
			wheel1_path = current_dir + "wheel_1.tga"
			wheel2_path = current_dir + "wheel_2.tga"
			
			if not self.LoadButtonSafely(wheelBtn, wheel0_path, wheel1_path, wheel2_path, IMG_DIR):
				# Final fallback to original images
				wheelBtn.SetUpVisual(IMG_DIR + "wheel_0.tga")
				wheelBtn.SetOverVisual(IMG_DIR + "wheel_1.tga")
				wheelBtn.SetDownVisual(IMG_DIR + "wheel_2.tga")

	def UpdateModeDisplay(self):
		# Update mode text and cost display
		if self.wheelMode == 0:
			self.children["costText"].SetText(localeInfo.WHEEL_GAME_ITEM_WHEEL_COST)
			self.children["wheelBtnItem"].LoadImage("icon/item/summer/220570.png")
			self.children["wheelBtnItem"].Show()
			self.children["goldCostText"].Hide()
		else:
			self.children["costText"].SetText(localeInfo.WHEEL_GAME_ITEM_GOLD_COST)
			self.children["wheelBtnItem"].Hide()
			self.children["goldCostText"].Hide()

	def __OverIn(self, index = -1):
		interface = constInfo.GetInterfaceInstance()
		if not interface:
			return
		if index >= 0:
			interface.tooltipItem.SetItemToolTip(self.children["wndItem"].itemDataDict[index])

	def __OverOut(self, index = -1):
		interface = constInfo.GetInterfaceInstance()
		if not interface:
			return
		interface.tooltipItem.HideToolTip()

	def SetLoadWhellData(self, itemData):
		"""Load reward data from server"""
		self.rewardData = []
		splitData = itemData.split("#")
		for splitText in splitData:
			splitItem = splitText.split("|")
			if len(splitItem) == 2:
				self.rewardData.append([int(splitItem[0]), int(splitItem[1])])
		self.RefreshRewardSlots()

	def RefreshRewardSlots(self):
		"""Refresh the reward display slots"""
		itemDataDict = {}
		wndItem = self.children["wndItem"]
		
		# Clear all slots
		for j in xrange(12 * 6):
			wndItem.ClearSlot(j)

		# Fill slots with reward data
		grid = Grid(12, 6)
		for itemData in self.rewardData:
			item.SelectItem(itemData[0])
			itemSize = item.GetItemSize()
			pos = grid.find_blank(itemSize[0], itemSize[1])
			if pos == -1:
				continue
			grid.put(pos, itemSize[0], itemSize[1])
			itemDataDict[pos] = itemData[0]
			wndItem.SetItemSlot(pos, itemData[0], 0 if itemData[1] == 1 else itemData[1])
		
		wndItem.itemDataDict = itemDataDict
		wndItem.RefreshSlot()

	def SetAnimationStatus(self, key):
		value = 0
		if not self.children["animationCheckBox"].GetCheck():
			value = 1
		self.children["animationCheckBox"].SetCheck(value)
		if value == 1:
			self.showAnimation = False
		else:
			self.showAnimation = True

	def GetGiftRotation(self, giftIndex):
		"""Get target rotation for specific gift index"""
		# Each slot is 36 degrees apart (360/10 = 36)
		base_rotation = giftIndex * 36
		# Add some random variation for natural feel
		variation = app.GetRandom(-5, 5)
		return base_rotation + variation

	def CloseMessageWindow(self):
		if self.children.has_key("message_window"):
			msgWindow = self.children["message_window"]
			if msgWindow != None:
				msgWindow.Hide()
				msgWindow = None

	def GetGiftData(self, itemVnum, itemCount):
		self.CloseMessageWindow()
		self.children["message_window"] = MessageWindow()
		self.children["message_window"].LoadMessage(itemVnum, itemCount)

	def GetRotationToIndex(self, rotation):
		"""Convert rotation to slot index"""
		# Normalize rotation
		rotation = rotation % 360
		# Each slot is 36 degrees
		return int((rotation + 18) / 36) % 10

	def SetTrailerMode(self):
		# Load items based on current wheel mode
		current_item_list = item_list if self.wheelMode == 0 else gold_item_list
		self.rewardData = current_item_list[:]
		self.RefreshRewardSlots()
		
		# Set up wheel items for animation
		for j in xrange(10):
			self.children["wheelItem%d"%j].Clear()
			for x in xrange(10):
				itemData = current_item_list[app.GetRandom(0,len(current_item_list)-1)]
				item.SelectItem(itemData[0])
				self.children["wheelItem%d"%j].LoadImage(item.GetIconImageFileName(), itemData[0], itemData[1])

	def SendServerDone(self):
		if self.wheelMode == 0:
			net.SendChatPacket("/wheel_of_fortune done")
		else:
			net.SendChatPacket("/wheel_of_fortune_gold done")

	def Simulation(self):
		current_item_list = item_list if self.wheelMode == 0 else gold_item_list
		itemList = []
		while True:
			itemData = current_item_list[app.GetRandom(0,len(current_item_list)-1)]
			if not itemData in itemList:
				itemList.append(itemData)
			if len(itemList) == 10:
				break
		text = ""
		for j in xrange(len(itemList)):
			text+=str(itemList[j][0])
			text+= "|"
			text+=str(itemList[j][1])
			text+="#"
		self.SetItemData(str(text))
		self.OnSetWhell(app.GetRandom(0,9))

	def StartWheel(self):
		# Don't start if already spinning
		if self.isSpinning:
			return
			
		if self.wheelMode == 0:
			net.SendChatPacket("/wheel_of_fortune start")
		else:
			net.SendChatPacket("/wheel_of_fortune_gold start")
		#self.Simulation()

	def SetItemData(self, data):
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

	def UpdateSingleFortune(self):
		"""Update pointer position based on current rotation"""
		single_pos = [[164,38,0], [230,63,35], [270,120,70], [266,188,110], [226,241,142], [160,262,180], [98,243,215], [56,189,250], [53,119,285], [96,61,324]]
		index = self.GetRotationToIndex(self.currentRotation)
		if index < len(single_pos):
			self.children["fortuneSingle"].SetRotation(single_pos[index][2])
			self.children["fortuneSingle"].SetPosition(single_pos[index][0], single_pos[index][1])

	def OnSetWhell(self, giftIndex):
		"""Start wheel spinning animation"""
		# Don't start new animation if already spinning
		if self.isSpinning:
			return
			
		self.giftIndex = giftIndex
		self.targetRotation = self.GetGiftRotation(giftIndex)
		
		if self.showAnimation:
			# Start smooth animation
			self.isSpinning = True
			self.animationStartTime = app.GetTime()
			
			# Add multiple rotations for dramatic effect (3-5 full spins)
			extra_spins = app.GetRandom(3, 5) * 360
			self.targetRotation += extra_spins
			
		else:
			# Skip animation
			self.currentRotation = self.targetRotation % 360
			self.children["wheelCircle"].SetRotation(self.currentRotation)
			self.UpdateSingleFortune()
			self.SendServerDone()

		if INFO_MODE and "debugText" in self.children:
			self.children["debugText"].SetText("Target Index: %d\nTarget Rotation: %d\nSpinning: %s" % (
				self.giftIndex, self.targetRotation, str(self.isSpinning)))

	def OnUpdate(self):
		"""Simple, smooth animation update"""
		if not self.isSpinning:
			return
		
		current_time = app.GetTime()
		elapsed_time = current_time - self.animationStartTime
		
		# Animation progress (0.0 to 1.0)
		progress = elapsed_time / self.animationDuration
		
		if progress >= 1.0:
			# Animation finished
			self.currentRotation = self.targetRotation
			self.isSpinning = False
			progress = 1.0
		else:
			# Smooth easing: starts fast, ends slow (ease-out cubic)
			eased_progress = 1.0 - pow(1.0 - progress, 3)
			self.currentRotation = eased_progress * self.targetRotation
		
		# Apply rotation
		display_rotation = self.currentRotation % 360
		self.children["wheelCircle"].SetRotation(display_rotation)
		self.UpdateSingleFortune()
		
		# Debug info
		if INFO_MODE and "debugText" in self.children:
			self.children["debugText"].SetText(
				"Progress: %.2f\nCurrent: %.1f\nTarget: %.1f\nSpinning: %s" % (
					progress, display_rotation, self.targetRotation % 360, str(self.isSpinning)))
		
		# Send completion when animation finishes
		if not self.isSpinning and progress >= 1.0:
			self.SendServerDone()

	def OnPressEscapeKey(self):
		self.Close()
		return True