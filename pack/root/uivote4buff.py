import ui
import app
import net
import chat
import item
import m2web
import wndMgr
import uiCommon
import constInfo
import uiToolTip
import localeInfo
from _weakref import proxy

## HIDE_BIG_BUTTON_ON_SUCCESSFUL_VOTE = True : Hides the v4b screen button once the player votes and selects a bonus (you must set a another option for the player to be able to open the UI again to change the bonus)
## HIDE_BIG_BUTTON_ON_SUCCESSFUL_VOTE = False : The v4b screen button is always displayed
HIDE_BIG_BUTTON_ON_SUCCESSFUL_VOTE = True

## This timeout is the time to wait before closing the connection from the api to check the vote status if it doesn't respond (10s by default, should be more than enough)
API_REQUEST_TIMEOUT = 10000 # milliseconds

## DO NOT TOUCH THIS AT ALL
VOTE_DURATION = 86400

class Vote4BuffButton(ui.Window):
	def __init__(self, interface):
		super(Vote4BuffButton, self).__init__()

		self.AddFlag("float")
		self.AddFlag("movable")

		self.interface = proxy(interface)

		self.button = ui.Button()
		self.button.SetParent(self)
		self.button.AddFlag("attach")
		self.button.SetUpVisual("elvion/game/vote4buff/vote_icon_m2.tga")
		self.button.SetOverVisual("elvion/game/vote4buff/vote_icon_m2.tga")
		self.button.SetDownVisual("elvion/game/vote4buff/vote_icon_m2.tga")
		self.button.Show()

		self.SetSize(self.button.GetWidth(), self.button.GetHeight())

		# if you have your own logic to save the button's position add it here to set the saved position
		self.SetPosition(150, wndMgr.GetScreenHeight() - self.GetHeight() - 90)
		self.Show()

	def OnMouseLeftButtonDown(self):
		if self.interface:
			self.interface.ToggleVote4Buff()

	def OnMoveWindow(self, x, y):
		# if you have your own logic to save the button's position, add it here to update the saved position
		pass

class Vote4BuffWindow(ui.ScriptWindow):
	def __init__(self):
		super(Vote4BuffWindow, self).__init__()

		self.Initialize()
		self.LoadWindow()

	def Initialize(self):
		self.board = None
		self.voteButton = None
		self.bonusButtons = []
		self.infoLabel = None
		self.bottomInfoWrapper = None
		self.bottomInfo = None
		self.questionDlg = None
		self.popupDlg = None
		self.nextUpdateTime = 0
		self.interface = None
		self.tooltip = None
		self.voteApiRequest = None
		#self.infoButton = None

	def BindInterface(self, interface):
		self.interface = proxy(interface)

	def LoadWindow(self):
		try:
			pyScriptLoader = ui.PythonScriptLoader()
			pyScriptLoader.LoadScriptFile(self, "UIScript/vote4buffwindow.py")

		except:
			import exception
			exception.Abort("Vote4BuffWindow.LoadWindow - File {} not found.".format("UIScript/vote4buffwindow.py"))

		self.board = self.GetChild("board")
		self.board.SetCloseEvent(ui.__mem_func__(self.Close))

		#self.infoButton = self.GetChild("info_button")
		#self.infoButton.ShowToolTip = ui.__mem_func__(self.OnOverInInfo)
		#self.infoButton.HideToolTip = ui.__mem_func__(self.OnOverOutToolTip)

		self.voteButton = self.GetChild("vote_button")
		self.voteButton.SetEvent(ui.__mem_func__(self.OnClickVoteButton))
		self.voteButton.ShowToolTip = ui.__mem_func__(self.OnOverInVote)
		self.voteButton.HideToolTip = ui.__mem_func__(self.OnOverOutToolTip)
		
		self.shopButton = self.GetChild("shop_button")
		self.shopButton.SetEvent(ui.__mem_func__(self.__ShopBtn))

		self.infoLabel = self.GetChild("info_label")
		
		self.bottomInfoWrapper = self.GetChild("bottom_info_wrapper")
		self.bottomInfo = self.GetChild("bottom_info")

		for i in range(3):
			self.bonusButtons.append(self.GetChild("bonus_button{}".format(i)))
			self.bonusButtons[i].SetEvent(ui.__mem_func__(self.OnClickBonusButton), i + 1)
			self.bonusButtons[i].HideToolTip = ui.__mem_func__(self.OnOverOutToolTip)
		
		self.bonusButtons[0].ShowToolTip = ui.__mem_func__(self.OnOverInBonus0)
		self.bonusButtons[1].ShowToolTip = ui.__mem_func__(self.OnOverInBonus1)
		self.bonusButtons[2].ShowToolTip = ui.__mem_func__(self.OnOverInBonus2)
		
		self.tooltip = uiToolTip.ToolTip()
		self.tooltip.Hide()

	def Open(self):
		self.SetTop()
		self.Show()
		
		self.RefreshInfo()

	def Close(self):
		self.OnCancelBonusSelection()
		self.ClosePopupDlg()
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True
	
	def Destroy(self):
		if self.voteApiRequest:
			m2web.Destroy(self.voteApiRequest)

		self.Close()
		self.ClearDictionary()
		self.Initialize()

	def __Popup(self, text):
		if not self.popupDlg:
			self.popupDlg = uiCommon.PopupDialog()
		self.popupDlg.SetText(text)
		self.popupDlg.Open()
		
	def __ShopBtn(self):
		net.SendChatPacket("/open_shop 16")

	def OnOverInInfo(self):
		self.tooltip.ClearToolTip()

		self.tooltip.AppendTextLine("Info ToolTip")

		self.tooltip.AlignHorizonalCenter()
		self.tooltip.Show()

	def OnOverInVote(self):
		item_rewards = constInfo.VOTE4BUFF_DATA["ITEM_REWARDS"]
		if item_rewards:
			self.tooltip.ClearToolTip()
			self.tooltip.SetTitle(localeInfo.VOTE4BUFF_VOTE_REWARDS)

			# Automatically gather vote rewards from server and display them with text (one reward per line)
			for (vnum, count) in item_rewards:
				# You can display it in a fancier way with item icon and so (up to the server owner)
				item.SelectItem(vnum)
				self.tooltip.AppendTextLine("{}x {}".format(count, item.GetItemName()))

			self.tooltip.AlignHorizonalCenter()
			self.tooltip.Show()

	def OnOverInBonus0(self):
		self.OnOverInBonus(0)
	
	def OnOverInBonus1(self):
		self.OnOverInBonus(1)
	
	def OnOverInBonus2(self):
		self.OnOverInBonus(2)

	def OnOverInBonus(self, index):
		self.tooltip.ClearToolTip()
		self.tooltip.SetTitle(localeInfo.VOTE4BUFF_BONUS_TITLE)

		if not constInfo.VOTE4BUFF_DATA or "AVAILABLE_BONUSES" not in constInfo.VOTE4BUFF_DATA:
			return
		if index < 0 or index >= len(constInfo.VOTE4BUFF_DATA["AVAILABLE_BONUSES"]):
			return

		# Automatically gather bonuses from server and display them in green/red color depending on the value
		bonuses = constInfo.VOTE4BUFF_DATA["AVAILABLE_BONUSES"][index]
		for (apply, value) in bonuses:
			self.tooltip.AppendTextLine(GetAffectString(apply, value), self.tooltip.POSITIVE_COLOR if value >= 0 else self.tooltip.NEGATIVE_COLOR)

		self.tooltip.AlignHorizonalCenter()
		self.tooltip.Show()

	def OnOverOutToolTip(self):
		self.tooltip.Hide()

	def RefreshInfo(self):
		if not IsVoteValid():
			self.voteButton.Enable()
			self.bottomInfoWrapper.LoadImage("elvion/game/vote4buff/vote_available.tga")
		else:
			self.voteButton.Disable()
			self.bottomInfoWrapper.LoadImage("elvion/game/vote4buff/vote_unavailable.tga")

		if not constInfo.VOTE4BUFF_DATA["SELECTED_BONUS"] and IsVoteValid():
			# Refreshing right after votting triggers a popup so the user realizes he has to take a bonus
			self.__Popup(localeInfo.VOTE4BUFF_BONUS_SELECTION)
		else:
			# Simple refresh of the bonus buttons, the selected one will look brighter than the others
			for i in range(len(self.bonusButtons)):
				if i == constInfo.VOTE4BUFF_DATA["SELECTED_BONUS"] - 1:
					self.bonusButtons[i].Disable()
				else:
					self.bonusButtons[i].Enable()

	def OnClickVoteButton(self):
		if not constInfo.VOTE4BUFF_DATA["API_URL"]:
			return
		
		if self.voteApiRequest:
			return
		
		self.voteButton.Disable()
		
		# Create the api vote request (to check the vote status of the player)
		self.voteApiRequest = m2web.Request(constInfo.VOTE4BUFF_DATA["API_URL"], API_REQUEST_TIMEOUT)

	def OnClickBonusButton(self, index):
		if not IsVoteValid():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.VOTE4BUFF_WARNING_MSG1)
			return

		price = constInfo.VOTE4BUFF_DATA["CHANGE_BONUS_PRICE"]

		if not constInfo.VOTE4BUFF_DATA["SELECTED_BONUS"] or not price: # select initial bonus, normal question
			question = localeInfo.VOTE4BUFF_QUESTION1
		elif price: # change the bonus
			question = localeInfo.VOTE4BUFF_QUESTION2 % localeInfo.NumberToMoneyString(price)

		if not self.questionDlg:
			self.questionDlg = uiCommon.QuestionDialog()
		self.questionDlg.index = index
		self.questionDlg.SetText(question)
		self.questionDlg.SetAcceptEvent(ui.__mem_func__(self.OnAcceptBonusSelection))
		self.questionDlg.SetCancelEvent(ui.__mem_func__(self.OnCancelBonusSelection))
		self.questionDlg.Open()

	def OnAcceptBonusSelection(self):
		if self.questionDlg:
			index = getattr(self.questionDlg, "index", 0)
			net.SendChatPacket("/vote4buff bonus {}".format(index))

		self.OnCancelBonusSelection()
	
	def OnCancelBonusSelection(self):
		if self.questionDlg:
			self.questionDlg.Close()
			self.questionDlg = None

	def ClosePopupDlg(self):
		if self.popupDlg:
			self.popupDlg.Close()
			self.popupDlg = None

	def Update(self):
		if not constInfo.VOTE4BUFF_DATA["LOADED"]:
			return
		
		timeLeft = -1

		if IsVoteValid():
			timeLeft = max(-1, (constInfo.VOTE4BUFF_DATA["LAST_VOTE_TIME"] + VOTE_DURATION) - app.GetGlobalTimeStamp())
			if self.IsShow():
				self.bottomInfo.SetText(localeInfo.VOTE4BUFF_TIME_LEFT % SecondToHMS(timeLeft))

		if timeLeft == -1 and self.bottomInfo.GetText() != localeInfo.VOTE4BUFF_AVAILABLE:
			if constInfo.VOTE4BUFF_DATA["SELECTED_BONUS"] != 0:
				self.bonusButtons[constInfo.VOTE4BUFF_DATA["SELECTED_BONUS"] - 1].Enable()
				constInfo.VOTE4BUFF_DATA["SELECTED_BONUS"] = 0
			self.bottomInfo.SetText(localeInfo.VOTE4BUFF_AVAILABLE)
			self.RefreshInfo()
			self.interface.RefreshVote4BuffButton()

		if self.voteApiRequest and m2web.IsCompleted(self.voteApiRequest):
			# Api request is completed, let's check the result of it
			response = m2web.GetResponse(self.voteApiRequest)
			m2web.Destroy(self.voteApiRequest)
			self.voteApiRequest = None

			# Parse status from metin2pserver.info JSON response
			# Actual format: {"result":{"status":"1","time":"..."}}
			# status "1" = voted, "2" = already rewarded/duplicate
			status = 0
			idx = response.find('"status"')
			if idx != -1:
				colon = response.find(':', idx)
				if colon != -1:
					try:
						raw = response[colon + 1:colon + 6].strip().split(',')[0].split('}')[0].strip().strip('"')
						status = int(raw)
					except:
						pass

			if status == 1: # Vote found - send confirmation to server
				net.SendChatPacket("/vote4buff vote {}".format(VOTE_DURATION))
			elif status == 2: # Already rewarded today
				self.__Popup(localeInfo.VOTE4BUFF_WARNING_MSG2)
				self.voteButton.Enable()
			else: # No vote found - open vote page in browser
				m2web.OpenExternalBrowser(constInfo.VOTE4BUFF_DATA["VOTE_URL"])
				self.__Popup(localeInfo.VOTE4BUFF_WARNING_MSG2)
				self.voteButton.Enable()

def IsVoteValid():
	if not constInfo.VOTE4BUFF_DATA["LOADED"]:
		return False
	
	return constInfo.VOTE4BUFF_DATA["LAST_VOTE_TIME"] + VOTE_DURATION > app.GetGlobalTimeStamp()

def IsBonusSelected():
	if not IsVoteValid():
		return False
	
	return constInfo.VOTE4BUFF_DATA["SELECTED_BONUS"] > 0

def SecondToHMS(time):
	time = max(0, time)
	hours = int(time / (60 * 60))
	time -= 60 * 60 * hours
	mins = int(time / 60)
	time -= 60 * mins
	secs = int(time)
	return "%02d:%02d:%02d" % (hours, mins, secs)

def GetAffectString(affectType, affectValue):
	if not affectType:
		return ""

	try:
		return uiToolTip.ItemToolTip.AFFECT_DICT[affectType](affectValue)
	except TypeError:
		return "UNKNOWN_VALUE[%s] %s" % (affectType, affectValue)
	except KeyError:
		return "UNKNOWN_TYPE[%s] %s" % (affectType, affectValue)
