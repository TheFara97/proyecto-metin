import ui
import net
import chat
import player
import app
import localeInfo
import ime
import messenger
import chr
import uiScriptLocale
import time
import datetime
import uiCommon
import constInfo

class WhisperButton(ui.Button):
	def __init__(self):
		ui.Button.__init__(self, "TOP_MOST")

	def __del__(self):
		ui.Button.__del__(self)

	def SetToolTipText(self, text, x=0, y = 32):
		ui.Button.SetToolTipText(self, text, x, y)
		self.ToolTipText.Show()

	def SetToolTipTextWithColor(self, text, color, x=0, y = 32):
		ui.Button.SetToolTipText(self, text, x, y)
		self.ToolTipText.SetPackedFontColor(color)
		self.ToolTipText.Show()

	def ShowToolTip(self):
		if 0 != self.ToolTipText:
			self.ToolTipText.Show()

	def HideToolTip(self):
		if 0 != self.ToolTipText:
			self.ToolTipText.Show()

class WhisperDialog(ui.ScriptWindow):

	class TextRenderer(ui.Window):
		def SetTargetName(self, targetName):
			self.targetName = targetName

		def OnRender(self):
			(x, y) = self.GetGlobalPosition()
			chat.RenderWhisper(self.targetName, x, y)

	class ResizeButton(ui.DragButton):

		def __init__(self):
			ui.DragButton.__init__(self)

		def __del__(self):
			ui.DragButton.__del__(self)

		def OnMouseOverIn(self):
			app.SetCursor(app.HVSIZE)

		def OnMouseOverOut(self):
			app.SetCursor(app.NORMAL)

	def __init__(self, eventMinimize, eventClose):
		print "NEW WHISPER DIALOG  ----------------------------------------------------------------------------"
		ui.ScriptWindow.__init__(self)
		self.targetName = ""
		self.eventMinimize = eventMinimize
		self.eventClose = eventClose
		self.eventAcceptTarget = None
		self.questionDialog = None
	def __del__(self):
		print "---------------------------------------------------------------------------- DELETE WHISPER DIALOG"
		ui.ScriptWindow.__del__(self)

	def LoadDialog(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/WhisperDialog.py")
		except:
			import exception
			exception.Abort("WhisperDialog.LoadDialog.LoadScript")

		try:
			GetObject=self.GetChild
			self.titleName = GetObject("titlename")
			self.titleNameEdit = GetObject("titlename_edit")
			self.closeButton = GetObject("closebutton")
			self.scrollBar = GetObject("scrollbar")
			self.messenger_scrollbar = GetObject("messenger_scrollbar")
			self.chatLine = GetObject("chatline")
			self.minimizeButton = GetObject("minimizebutton")
			self.historyButton = GetObject("historybutton")
			self.historyDeleteButton = GetObject("historyDeleteButton")
			self.friendbutton = GetObject("friendbutton")
			self.ignoreButton = GetObject("ignorebutton")
			self.acceptButton = GetObject("acceptbutton")
			self.sendButton = GetObject("sendbutton")
			self.messenger_list = GetObject("messenger_list")
			self.messenger_players_window = GetObject("messenger_players_window")
			self.new_whisper = GetObject("new_whisper")
			self.board = GetObject("board")
			self.editBar = GetObject("editbar")
			self.gamemasterMark = GetObject("gamemastermark")
			self.searchPlayerShopButton = GetObject("searchplayershopbutton")
		except:
			import exception
			exception.Abort("DialogWindow.LoadDialog.BindObject")

		self.gamemasterMark.Hide()
		self.titleName.SetText("")
		self.titleNameEdit.SetText("")
		self.minimizeButton.SetEvent(ui.__mem_func__(self.Minimize))
		self.historyButton.SetEvent(ui.__mem_func__(self.ShowHistory))
		self.historyDeleteButton.SetEvent(ui.__mem_func__(self.DeleteHistoryQuestion))
		self.closeButton.SetEvent(ui.__mem_func__(self.Close))
		self.scrollBar.SetPos(1.0)
		self.scrollBar.SetScrollEvent(ui.__mem_func__(self.OnScroll))
		self.messenger_scrollbar.SetScrollEvent(ui.__mem_func__(self.ScrollMessengerList))
		self.chatLine.SetReturnEvent(ui.__mem_func__(self.SendWhisper))
		self.chatLine.SetEscapeEvent(ui.__mem_func__(self.Minimize))
		self.chatLine.SetMultiLine()
		self.sendButton.SetEvent(ui.__mem_func__(self.SendWhisper))
		self.titleNameEdit.SetReturnEvent(ui.__mem_func__(self.AcceptTarget))
		self.titleNameEdit.SetEscapeEvent(ui.__mem_func__(self.Close))
		self.ignoreButton.SetEvent(ui.__mem_func__(self.IgnoreTarget))
		self.acceptButton.SetEvent(ui.__mem_func__(self.AcceptTarget))
		self.friendbutton.SetEvent(ui.__mem_func__(self.AddToFriend))
		self.searchPlayerShopButton.SetEvent(ui.__mem_func__(self.searchPlayerShop))

		self.textRenderer = self.TextRenderer()
		self.textRenderer.SetParent(self.board)
		self.textRenderer.SetPosition(20, 28)
		self.textRenderer.SetTargetName("")
		self.textRenderer.Show()

		self.resizeButton = self.ResizeButton()
		self.resizeButton.SetParent(self.board)
		self.resizeButton.SetSize(20, 20)
		self.resizeButton.SetPosition(370, 270)
		self.resizeButton.SetMoveEvent(ui.__mem_func__(self.ResizeWhisperDialog))
		self.resizeButton.Show()

		self.messenger_list.titleBar.btnClose.Hide()
		self.messenger_list.SetTitleColor(0xffbfa185)
		self.messenger_players = {}
		self.messenger_players_time = {}
		self.messenger_players_news = {}
		
		self.new_whisper.SetEvent(ui.__mem_func__(self.interface.OpenWhisperDialogWithoutTarget))

		self.ResizeWhisperDialog()

	def Destroy(self):
		self.ClearDictionary()
		self.targetName = None
		self.eventMinimize = None
		self.eventClose = None
		self.eventAcceptTarget = None
		self.questionDialog = None
		self.titleName = None
		self.titleNameEdit = None
		self.closeButton = None
		self.scrollBar = None
		self.messenger_scrollbar = None
		self.chatLine = None
		self.sendButton = None
		self.messenger_list = None
		self.messenger_players_window = None
		self.historyButton = None
		self.historyDeleteButton = None
		self.new_whisper = None
		self.ignoreButton = None
		self.acceptButton = None
		self.friendbutton = None
		self.minimizeButton = None
		self.textRenderer = None
		self.board = None
		self.editBar = None
		self.resizeButton = None
		self.messenger_players = None
		self.messenger_players_time = None
		self.messenger_players_news = None
		self.whisper_status = None
		self.interface = None
		self.searchPlayerShopButton = None

	def ResizeWhisperDialog(self):
		(xPos, yPos) = self.resizeButton.GetLocalPosition()
		if xPos < 370:
			self.resizeButton.SetPosition(370, yPos)
			return
		if yPos < 270:
			self.resizeButton.SetPosition(xPos, 270)
			return
		self.SetWhisperDialogSize(xPos + 20, yPos + 20)
		
		self.UpdateScrollBar()
		self.ScrollMessengerList()

	def SetWhisperDialogSize(self, width, height):
		try:
			max = min(300, int((width-90)/6) * 3 - 6)

			self.messenger_list.SetSize(self.messenger_list.GetWidth(), height)
			self.messenger_players_window.SetSize(self.messenger_list.GetWidth(), height-52)
			self.board.SetSize(width, height)
			self.scrollBar.SetPosition(width-25, 35)
			self.scrollBar.SetScrollBarSize(height-100)
			self.scrollBar.SetPos(1.0)

			self.messenger_scrollbar.SetPosition(190-25, 47)
			self.messenger_scrollbar.SetScrollBarSize(height-40)

			self.editBar.SetSize(width-18, 50)
			self.chatLine.SetSize(width-90, 40)
			self.chatLine.SetLimitWidth(width-90)
			self.SetSize(width + 190, height)#width +190 because of messenger list

			if 0 != self.targetName:
				chat.SetWhisperBoxSize(self.targetName, width - 50, height - 90)

			self.textRenderer.SetPosition(20, 28)
			# self.scrollBar.SetPosition(width-25, 35)
			self.editBar.SetPosition(10, height-60)
			self.sendButton.SetPosition(width-80, 10)
			self.minimizeButton.SetPosition(width-80, 12)
#ifdef WHISPER_HISTORY
			self.historyButton.SetPosition(width-47, 9)
			if self.historyButton.IsShow():
				self.historyDeleteButton.SetPosition(width-71, 9)
			else:
				self.historyDeleteButton.SetPosition(width-71, 9)
			
#endif
			self.closeButton.SetPosition(width-24, 9)

			self.SetChatLineMax(max)

		except:
			import exception
			exception.Abort("WhisperDialog.SetWhisperDialogSize.BindObject")

	def SetChatLineMax(self, max):
		self.chatLine.SetMax(max)

		from grpText import GetSplitingTextLine

		text = self.chatLine.GetText()
		if text:
			self.chatLine.SetText(GetSplitingTextLine(text, max, 0))

	def AddFriend(self):
		net.SendMessengerAddByNamePacket(self.targetName)
		self.AddFriendButton.Hide()


	def searchPlayerShop(self):
		try:
			import uiShopSearch
			
			# Create or get the shop search window
			if not hasattr(self.interface, 'wndShopSearch') or not self.interface.wndShopSearch:
				self.interface.wndShopSearch = uiShopSearch.ShopSearch()
				self.interface.wndShopSearch.BindInterface(self.interface)
				if hasattr(self.interface, 'tooltipItem'):
					self.interface.wndShopSearch.SetItemToolTip(self.interface.tooltipItem)
			
			shopSearch = self.interface.wndShopSearch
			
			# Use the alternative show method that doesn't auto-search
			if hasattr(shopSearch, 'ShowWithoutSearch'):
				shopSearch.ShowWithoutSearch()
			else:
				# Fallback: show normally but clear the window immediately
				shopSearch.Show(True)
			
			if self.targetName and len(self.targetName) > 0:
				# Clear any existing search results to prevent confusion
				shopSearch.ItemList.Clear()
				
				# Set up for player name search
				shopSearch.sItemName.SetText(self.targetName)
				
				# Enable player search mode
				if not shopSearch.CheckBox.IsChecked:
					shopSearch.OnCheckBox()  # This toggles the checkbox properly
				
				# Reset to search all categories
				shopSearch.SetCategory(0, 255, False)
				
				# Manually trigger search after a short delay to avoid cooldown
				import app
				shopSearch.searchTime = app.GetTime() + 0.1  # Very short delay
				shopSearch.OnSearch()
			
		except Exception as e:
			import chat
			chat.AppendChat(chat.CHAT_TYPE_INFO, "Failed to open shop search: " + str(e))
		

	def OpenWithTarget(self, targetName):	
		#net.SendChatPacket("/whisper_status %s" % str(targetName))	TODO
		if app.ENABLE_BADGE_NOTIFICATION_MANAGER:
			chat.CreateWhisper(targetName, self.chatLine.hWnd)
		else:
			chat.CreateWhisper(targetName)
		chat.SetWhisperBoxSize(targetName, self.GetWidth() - 60, self.GetHeight() - 90)
		self.chatLine.SetFocus()
		self.titleName.SetText(targetName)
		self.targetName = targetName
		self.textRenderer.SetTargetName(targetName)
		self.titleNameEdit.Hide()

		if messenger.IsFriendByName(targetName):
			self.friendbutton.Hide()
			self.ignoreButton.SetPosition(184, 9)
			self.searchPlayerShopButton.SetPosition(116+2*22, 9)
		else:
			self.friendbutton.Show()
			self.ignoreButton.SetPosition(184, 9)
			self.searchPlayerShopButton.SetPosition(116+2*22, 9)
				
		self.ignoreButton.Show()
		self.searchPlayerShopButton.Show()

		#if messenger.IsBlockByName(targetName):
		#	self.ignoreButton.SetToolTipText(uiScriptLocale.WHISPER_UNBAN)
		#	self.ignoreButton.SetUpVisual("zaris/whisper/block.png")
		#else:
		#	self.ignoreButton.SetToolTipText(uiScriptLocale.WHISPER_BAN)
		#	self.ignoreButton.SetUpVisual("zaris/whisper/block_not.png")

		self.acceptButton.Hide()
		self.minimizeButton.Hide()
		
		self.closeButton.Show()
		
		if uiScriptLocale.WhisperExists(player.GetName(), self.targetName):
			whisper_key = "%s_%s" % (player.GetName(), str(self.targetName))

			if whisper_key in constInfo.whisper_history_state and constInfo.whisper_history_state[whisper_key] == 2:
				self.ShowHistory()

			self.historyButton.Show()
			self.historyDeleteButton.Show()

		self.whisper_status = ui.Button()
		self.whisper_status.SetParent(self.board)
		self.whisper_status.SetPosition(9, 11)
		self.whisper_status.SetUpVisual("zaris/whisper/whisper_offline.png")
		self.whisper_status.SetOverVisual("zaris/whisper/whisper_offline.png")
		self.whisper_status.SetDownVisual("zaris/whisper/whisper_offline.png")
		self.whisper_status.SetToolTipText(uiScriptLocale.WHISPER_OFFLINE)
		self.whisper_status.SetToolTipColor(0xffFF0000)
		self.whisper_status.Hide()

		if targetName in constInfo.MESSENGER_NEWS_LIST:
			constInfo.MESSENGER_NEWS_LIST.remove(targetName)
		
		if not constInfo.MESSENGER_NEWS_LIST:
			self.interface.SetMessengerReaded()
			
		self.ResizeWhisperDialog()
		self.LoadMessengerList()
		self.CheckGameMaster()

	def CheckGameMaster(self):
		if len(self.targetName) > 0 and (self.interface.IsGameMasterName(self.targetName) or self.targetName[0] == "["):
			self.gamemasterMark.Show()
		else:
			self.gamemasterMark.Hide()
			
	def OnUpdate(self):
		status_dict = {
			0 : ["zaris/whisper/whisper_offline.png",uiScriptLocale.WHISPER_OFFLINE,0xffFF0000],#off
			1 : ["zaris/whisper/whisper_online.png",uiScriptLocale.WHISPER_ONLINE,0xff22BB22],#on
			2 : ["zaris/whisper/whisper_afk.png",uiScriptLocale.WHISPER_AFK, 0xffFF8C00],#afk
		}
		STATUS_OFFLINE = 0
		STATUS_ONLINE = 1
		STATUS_AFK = 2
		name = str(self.targetName)

		if name in constInfo.MESSENGER_NEWS_LIST:
			constInfo.MESSENGER_NEWS_LIST.remove(name)

			if not constInfo.MESSENGER_NEWS_LIST:
				self.interface.SetMessengerReaded()

			self.LoadMessengerList()

		if name in constInfo.players_online.keys():
			status = constInfo.players_online[name][0]
			lastDisconnect = constInfo.players_online[name][1]
			offlineTime = app.GetGlobalTimeStamp() - lastDisconnect

			if lastDisconnect > 0 and offlineTime >= 5:
				status = STATUS_OFFLINE

			self.whisper_status.SetUpVisual(status_dict[status][0])
			self.whisper_status.SetOverVisual(status_dict[status][0])
			self.whisper_status.SetDownVisual(status_dict[status][0])
			status_tooltip = status_dict[status][1]

			if status == STATUS_OFFLINE:
				strTime = ""

				if offlineTime >= 86400:
					strTime = str(offlineTime/86400) + " " + localeInfo.DAY
				elif offlineTime >= 3600:
					strTime = str(offlineTime/3600) + " " + localeInfo.HOUR
				elif offlineTime >= 60:
					strTime = str(offlineTime/60) + " " + localeInfo.MINUTE
				else:
					strTime = str(offlineTime) + " " + localeInfo.SECOND

				status_tooltip = status_dict[status][1] % strTime

			self.whisper_status.SetToolTipText(status_tooltip)
			self.whisper_status.SetToolTipColor(status_dict[status][2])

	def OpenWithoutTarget(self, event):
		self.eventAcceptTarget = event
		self.titleName.SetText("")
		self.titleNameEdit.SetText("")
		self.titleNameEdit.SetFocus()
		self.targetName = ""
		self.titleNameEdit.Show()
		self.ignoreButton.Hide()
		self.friendbutton.Hide()
		self.acceptButton.Show()
		self.searchPlayerShopButton.Hide()
		self.minimizeButton.Hide()
		self.historyButton.Hide()
		self.closeButton.Show()
		self.historyDeleteButton.Hide()
		self.gamemasterMark.Hide()
		self.LoadMessengerList()
		if not constInfo.MESSENGER_NEWS_LIST:
			self.interface.SetMessengerReaded()

	def SetGameMasterLook(self):
		self.gamemasterMark.Show()

	def Minimize(self):
		self.titleNameEdit.KillFocus()
		self.chatLine.KillFocus()
		self.Hide()

		if None != self.eventMinimize:
			self.eventMinimize(self.targetName)

	def ShowHistory(self):
		constInfo.whisper_history_state["%s_%s" % (player.GetName(), str(self.targetName))] = 1
		chat.ClearWhisper(self.targetName)
		uiScriptLocale.LoadWhisper(player.GetName(), str(self.targetName))
		self.ResizeWhisperDialog()

	def DeleteHistoryQuestion(self):
		self.questionDialog = uiCommon.QuestionDialog()
		self.questionDialog.SetText(uiScriptLocale.WHISPER_DELETE_CONFIRM % self.targetName)
		self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.DeleteHistory))
		self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		self.questionDialog.Open()
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def DeleteHistory(self):
		self.OnCloseQuestionDialog()
		chat.ClearWhisper(self.targetName)
		uiScriptLocale.DeleteWhisper(player.GetName(), str(self.targetName))
		self.LoadMessengerList()
		self.interface.OpenWhisperDialogWithoutTarget()

	def OnCloseQuestionDialog(self):
		if not self.questionDialog:
			return

		self.questionDialog.Close()
		self.questionDialog = None
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	def Close(self):
		self.titleNameEdit.KillFocus()
		self.chatLine.KillFocus()
		self.Hide()

		if None != self.eventClose:
			self.eventClose(self.targetName)

	def AddToFriend(self):
		net.SendMessengerAddByNamePacket(self.targetName)
		self.friendButton.Hide()
		self.ignoreButton.SetPosition(184,10)
		self.searchPlayerShopButton.SetPosition(116+2*22,10)

	def IgnoreTarget(self):
		pass
		#if messenger.IsBlockByName(self.targetName):
		#	messenger.RemoveBlock(self.targetName)
		#	net.SendMessengerRemoveBlockPacket(self.targetName, self.targetName)
		#	self.ignoreButton.SetToolTipText(uiScriptLocale.WHISPER_BAN)
		#	self.ignoreButton.SetUpVisual("d:/ymir work/ui/block_not.png")
		#else:
		#	net.SendMessengerAddBlockByNamePacket(self.targetName)
		#	self.ignoreButton.SetToolTipText(uiScriptLocale.WHISPER_UNBAN)
		#	self.ignoreButton.SetUpVisual("d:/ymir work/ui/block.png")

	def AcceptTarget(self):
		name = self.titleNameEdit.GetText()
		if len(name) <= 0:
			self.Close()
			return

		if None != self.eventAcceptTarget:
			self.titleNameEdit.KillFocus()
			self.eventAcceptTarget(name)

	def OnScroll(self):
		chat.SetWhisperPosition(self.targetName, self.scrollBar.GetPos())

	def LoadMessengerList(self):
		player_name = player.GetName()
		uiScriptLocale.LoadMessengerList(player_name)

		for i in xrange(len(self.messenger_players)):
			self.messenger_players[i].Hide()
			self.messenger_players_time[i].Hide()
			if self.messenger_players_news.has_key(i):
				self.messenger_players_news[i].Hide()

		self.messenger_players = {}
		self.messenger_players_time = {}
		self.messenger_players_news = {}
		i = 0

		if player_name in constInfo.MESSENGER_LIST:
			for key, value in sorted(constInfo.MESSENGER_LIST[player_name].items(), key=lambda kv: kv[1], reverse=True):
				row = ui.Button()
				row.SetParent(self.messenger_players_window)
				row.SetPosition(0,25)
				row.SetUpVisual("zaris/whisper/messenger_user_button_norm.png")
				row.SetOverVisual("zaris/whisper/messenger_user_button_hover.png")
				row.SetDownVisual("zaris/whisper/messenger_user_button_down.png")
				row.SetText(key)

				if key.startswith("[INFO]"):
					row.SetTextColor(0xff00ffff)
				elif key.startswith("["):#GM
					row.SetTextColor(0xffffff00)
				elif key in ("Tržiště", "Marketplace", "Hero Market", "Tržiště postav"):
					row.SetTextColor(0xff00ff00)
				elif key == "Switchbot":
					row.SetTextColor(0xffb266ff)
				else:
					row.SetTextColor(0xffbfa185)  # White for normal players
				
				row.SetTextOutline(True)

				row.SetEvent(ui.__mem_func__(self.OpenWithTarget), key)
				self.messenger_players[i] = row
				
				time_info = ui.TextLine()
				time_info.SetParent(self.messenger_players[i])
				nice_time = datetime.datetime.fromtimestamp(value).strftime('[%d.%m %H:%M]')
				time_info.SetText(str(nice_time))
				time_info.SetPackedFontColor(0xffbfa185)
				time_info.SetPosition(92,2)
				self.messenger_players_time[i] = time_info

				if key in constInfo.MESSENGER_NEWS_LIST:
					new = ui.ImageBox()
					new.SetParent(self.messenger_players[i])
					new.LoadImage("zaris/whisper/new_message_icon.png")
					new.SetPosition(0,0)
					self.messenger_players_news[i] = new
				
				i = i + 1
		
		self.UpdateScrollBar()
		self.ScrollMessengerList()
	
	def UpdateScrollBar(self):
		chatScrollHeight = self.board.GetHeight() - 100
		messengerListScrollHeigh = self.messenger_players_window.GetHeight()

		if len(self.targetName) > 0:
			totalHeight = chat.GetWhisperTotalHeight(self.targetName)

			if totalHeight <= chatScrollHeight:
				totalHeight = chatScrollHeight + 1

			scrollStep = float(15 * 5) / float(totalHeight - chatScrollHeight)
			if scrollStep < 0.01:
				scrollStep = 0.01

			self.scrollBar.SetMiddleBarSize(float(chatScrollHeight) / float(totalHeight))
			self.scrollBar.SetScrollStep(scrollStep)
			self.scrollBar.SetPos(1.0)


		totalHeight = max(1, len(self.messenger_players)) * 25

		if totalHeight < messengerListScrollHeigh:
			totalHeight = messengerListScrollHeigh + 1
		
		scrollStep = float(25*5) / float(totalHeight - messengerListScrollHeigh)

		if scrollStep < 0.01:
			scrollStep = 0.01

		self.messenger_scrollbar.SetMiddleBarSize(float(messengerListScrollHeigh) / float(totalHeight))
		self.messenger_scrollbar.SetScrollStep(scrollStep)

	def ScrollMessengerList(self):
		LINE_Y_STEP = 25
		totalLines = len(self.messenger_players)
		view_results = int(self.messenger_players_window.GetHeight() / LINE_Y_STEP)
		pos = int(self.messenger_scrollbar.GetPos() * max(0, totalLines - view_results))
		default_y = -(pos * LINE_Y_STEP)

		for i in xrange(totalLines):
			pos_y = default_y + (i * LINE_Y_STEP)
			self.messenger_players[i].SetPosition(0, pos_y)

			if pos_y < 0 or pos_y >= (view_results * LINE_Y_STEP):
				self.messenger_players[i].Hide()
				self.messenger_players_time[i].Hide()
				if self.messenger_players_news.has_key(i):
					self.messenger_players_news[i].Hide()
			else:
				self.messenger_players[i].Show()
				self.messenger_players_time[i].Show()
				if self.messenger_players_news.has_key(i):
					self.messenger_players_news[i].Show()

	def SendWhisper(self):
		text = self.chatLine.GetText()
		textLength = len(text)

		if len(str(self.targetName)) == 0:
			return

		#if app.OFFLINE_MESSAGE:
		#	name = str(self.targetName)
		#
		#	if name in constInfo.players_online.keys():
		#		lastDisconnect = constInfo.players_online[name][1]
		#
		#		if lastDisconnect > 0:
		#			text = "#" + text

		if textLength > 0:
			if net.IsInsultIn(text):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHAT_INSULT_STRING)
				return

			if app.ENABLE_LINK_IN_CHAT:
				link = self.GetLink(text)
				if link != "":
					if not chr.IsGameMaster():
						text = text.replace(link, "|cFF00C0FC|h|Hweb:" + link.replace("://", "XxX") + "|h" + link + "|h|r")
					else:
						text = text.replace(link, "|cFF00C0FC|h|Hsysweb:" + link.replace("://", "XxX") + "|h" + link + "|h|r")

#			if app.OFFLINE_MESSAGE:
#				if text[0]=="#" and len(text)>1:
#					if self.targetName == player.GetName():
#						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OFFLINE_WHISPER_CANNOT_SEND_MYSELF)
#						return
#            
#					text = text.replace("#", "", 1) #remove offline message arg("#")
#					net.SendOfflineMessagePacket(self.targetName, text)
#					append_text = "|cff13C9FF|H|h" + str(time.strftime("[%d.%m %H:%M]")) + player.GetName() + ": " + text + "|r"
#					chat.AppendWhisper(chat.WHISPER_TYPE_CHAT, self.targetName, append_text)
#ifdef WHISPER_HISTORY
#					uiScriptLocale.SaveWhisper(player.GetName(), self.targetName, append_text)
#endif
#					self.chatLine.SetText("")
#					self.LoadMessengerList()
#					return

			net.SendWhisperPacket(self.targetName, text)
			self.chatLine.SetText("")

			append_text = "|cff13C9FF|H|h" + str(time.strftime("[%d.%m %H:%M]")) + player.GetName() + ": " + text + "|r"
			chat.AppendWhisper(chat.WHISPER_TYPE_CHAT, self.targetName, append_text)
#ifdef WHISPER_HISTORY
			uiScriptLocale.SaveWhisper(player.GetName(), self.targetName, append_text)
#endif
			if self.targetName in constInfo.MESSENGER_NEWS_LIST:
				constInfo.MESSENGER_NEWS_LIST.remove(self.targetName)
			if not constInfo.MESSENGER_NEWS_LIST:
				self.interface.SetMessengerReaded()
			self.LoadMessengerList()

	def OnTop(self):
		self.chatLine.SetFocus()
		if app.ENABLE_BADGE_NOTIFICATION_MANAGER:
			if self.targetName:
				chat.SeenWhisper(self.targetName)

	def BindInterface(self, interface):
		self.interface = interface

	def OnMouseLeftButtonDown(self):
		hyperlink = ui.GetHyperlink()
		if hyperlink:
			if app.IsPressed(app.DIK_LALT):
				link = chat.GetLinkFromHyperlink(hyperlink)
				ime.PasteString(link)
			else:
				self.interface.MakeHyperlinkTooltip(hyperlink)

	if app.ENABLE_LINK_IN_CHAT:
		def GetLink(self, text):
			start = text.find("http://")
			if start == -1:
				start = text.find("https://")
			if start == -1:
				return ""

			return text[start:len(text)].split(" ")[0]

if "__main__" == __name__:
	import uiTest

	class TestApp(uiTest.App):
		def OnInit(self):
			wnd = WhisperDialog(self.OnMax, self.OnMin)
			wnd.LoadDialog()
			wnd.OpenWithoutTarget(self.OnNew)
			wnd.SetPosition(0, 0)
			wnd.Show()

			self.wnd = wnd

		def OnMax(self):
			pass

		def OnMin(self):
			pass

		def OnNew(self):
			pass

	TestApp().MainLoop()
