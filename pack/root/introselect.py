# -*- coding: iso-8859-1 -*-

import grp
import app
import wndMgr
import snd
import net
import systemSetting
import localeInfo
import chr
import ui
import musicInfo
import playerSettingModule
import uiCommon
import chat
import player

LEAVE_BUTTON_FOR_POTAL = False

class CharacterRenderer(ui.Window):
	"""Character rendering component for the selection window."""
	
	def __init__(self):
		ui.Window.__init__(self)
		self._setup_renderer()
	
	def _setup_renderer(self):
		"""Initialize renderer settings."""
		pass
	
	def __del__(self):
		"""Clean up renderer resources."""
		#self.ClearDictionary()
		ui.Window.__del__(self)
	
	def OnRender(self):
		"""Render the character in 3D space."""
		grp.ClearDepthBuffer()
		grp.SetGameRenderState()
		grp.PushState()
		grp.SetOmniLight()

		screen_width = wndMgr.GetScreenWidth()
		screen_height = wndMgr.GetScreenHeight()
		new_screen_width = float(screen_width + 30)
		new_screen_height = float(screen_height + 30)
		
		grp.SetViewport(0.0, 0.0, 1.0, 1.0)

		app.SetCenterPosition(0, -40.0, 30.0)
		app.SetCamera(2000.0, 15.0, 180.0, 90.0)
		grp.SetPerspective(12.0, new_screen_width / new_screen_height, 1000.0, 3000.0)

		x, y = app.GetCursorPosition()
		grp.SetCursorPosition(x, y)

		chr.Deform()
		chr.Render()

		grp.RestoreViewport()
		grp.PopState()
		grp.SetInterfaceRenderState()


class SelectCharacterWindow(ui.Window):
	"""Main character selection window class."""
	
	SLOT_COUNT = 4
	
	RACE_FACE_IMAGES = {
		0: "warrior_m",  # RACE_WARRIOR_M
		1: "assassin_w",    # RACE_ASSASSIN_W
		2: "sura_m",     # RACE_SURA_M
		3: "shaman_w",   # RACE_SHAMAN_W
		4: "warrior_w",  # RACE_WARRIOR_W
		5: "assassin_m",    # RACE_ASSASSIN_M
		6: "sura_w",     # RACE_SURA_W
		7: "shaman_m",   # RACE_SHAMAN_M
	}
	
	def __init__(self, stream):
		ui.Window.__init__(self)
		self._setup_network(stream)
		self._initialize_attributes()
		
	def _setup_network(self, stream):
		"""Setup network phase and handlers."""
		net.SetPhaseWindow(net.PHASE_WINDOW_SELECT, self)
		self.stream = stream
		
	def _initialize_attributes(self):
		"""Initialize all class attributes."""
		self.slot = self.stream.GetCharacterSlot()
		self.open_loading_flag = False
		self.start_index = -1
		self.start_reserving_time = 0
		self.dlg_board = 0
		self.change_name_flag = False
		self.name_input_board = None
		self.sended_change_name_packet = False
		self.is_load = 0
		self.selected = 0
		
		self.cur_gauge = [0.0, 0.0, 0.0, 0.0]
		self.dest_gauge = [0.0, 0.0, 0.0, 0.0]
		
		self._init_ui_components()
		
	def _init_ui_components(self):
		"""Initialize UI component references."""
		self.empire_name = None
		self.flag_dict = {}
		self.btn_start = None
		self.btn_delete = None
		self.btn_exit = None
		self.back_ground = None
		self.dlg_question = None
		self.dlg_question_text = None
		self.dlg_question_accept_button = None
		self.dlg_question_cancel_button = None
		self.private_input_board = None
		self.chr_renderer = None
		
		# Stat display elements
		self.stat_hth = None
		self.stat_int = None
		self.stat_str = None
		self.stat_dex = None
		self.gauge_hth = None
		self.gauge_int = None
		self.gauge_str = None
		self.gauge_dex = None
		
		# Slot-related UI elements
		self.slot_buttons = []
		self.slot_name_texts = []
		self.slot_level_texts = []
		self.slot_guild_texts = []
		self.slot_create_buttons = []
		self.slot_tooltip = None

	def __del__(self):
		#self.ClearDictionary()
		ui.Window.__del__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_SELECT, 0)

	def Open(self):
		"""Open and initialize the character selection window."""
		self._load_dialogs()
		self._setup_game_data()
		self._initialize_character_board()
		self._setup_ui_state()
		self._setup_camera_and_music()
		self._handle_auto_select()
		
	def _load_dialogs(self):
		"""Load required dialog windows."""
		self._load_board_dialog("elvion/game/select_create/selectcharacterwindow.py")
		self._load_question_dialog("uiscript/questiondialog.py")
		
	def _setup_game_data(self):
		"""Load game data if needed."""
		if not app.ENABLE_PERFORMANCE_IMPROVEMENTS_NEW:
			playerSettingModule.LoadGameData("INIT")
			
	def _initialize_character_board(self):
		"""Initialize the character board UI."""
		self.InitCharacterBoard()
		
	def _setup_ui_state(self):
		"""Setup initial UI button states."""
		self.btn_start.Enable()
		# Remove this line
		# self.btn_create.Enable()
		self.btn_delete.Enable()
		self.btn_exit.Enable()
		self.dlg_board.Show()
		
		self.SetWindowName("SelectCharacterWindow")
		self.SetTop()
		self.Show()
		self.SetFocus()
		
		if self.slot >= 0:
			self.SelectSlot(self.slot)
			
	def _setup_camera_and_music(self):
		"""Setup camera position and background music."""
		if musicInfo.selectMusic != "":
			snd.SetMusicVolume(systemSetting.GetMusicVolume())
			snd.FadeInMusic("content/sound/BGM/" + musicInfo.selectMusic)

		app.SetCenterPosition(0.0, 0.0, 0.0)
		app.SetCamera(1550.0, 15.0, 180.0, 95.0)
		
	def _handle_auto_select(self):
		"""Handle automatic character selection."""
		self.is_load = 1
		self.Refresh()
	
		# Make sure only selected character is visible
		for i in range(self.SLOT_COUNT):
			if chr.HasInstance(i):
				chr.SelectInstance(i)
				if i == self.slot:
					chr.Show()
				else:
					chr.Hide()
	
		if self.stream.isAutoSelect:
			self.SelectSlot(self.stream.GetCharacterSlot())
			self.StartGame()
	
		self.selected = 0
		app.ShowCursor()

	def Close(self):
		"""Close the character selection window and cleanup."""
		self._cleanup_music()
		self._cleanup_ui()
		self._cleanup_characters()
		self._hide_and_cleanup()
		
	def _cleanup_music(self):
		"""Stop background music."""
		if musicInfo.selectMusic != "":
			snd.FadeOutMusic("content/sound/BGM/" + musicInfo.selectMusic)
			
	def _cleanup_ui(self):
		"""Cleanup UI components."""
		self.stream.popupWindow.Close()
	
		if self.dlg_board:
			self.dlg_board.ClearDictionary()
	
		# Reset UI component references
		self.empire_name = None
		self.flag_dict = {}
		self.dlg_board = None
		self.btn_start = None
		# Remove this line
		# self.btn_create = None
		self.btn_delete = None
		self.btn_exit = None
		self.back_ground = None
	
		if hasattr(self, 'dlg_question') and self.dlg_question:
			self.dlg_question.ClearDictionary()
			
		self.dlg_question = None
		self.dlg_question_text = None
		self.dlg_question_accept_button = None
		self.dlg_question_cancel_button = None
		self.private_input_board = None
		self.name_input_board = None
		
		# Cleanup slot elements
		self.slot_buttons = None
		self.slot_name_texts = None
		self.slot_level_texts = None
		self.slot_guild_texts = None
		self.slot_create_buttons = None
		self.slot_tooltip = None
		
	def _cleanup_characters(self):
		"""Delete all character instances."""
		for i in range(self.SLOT_COUNT):
			chr.DeleteInstance(i)
			
	def _hide_and_cleanup(self):
		"""Hide window and cleanup cursor."""
		self.Hide()
		self.KillFocus()
		app.HideCursor()

	def _update_slot_display(self, slot_index, character_data):
		"""Update the display for a character slot."""
		if not character_data or character_data['id'] == 0:
			# Empty slot
			self.slot_buttons[slot_index].SetUpVisual("elvion/game/select_create/character_face/no_face.png")
			self.slot_buttons[slot_index].SetOverVisual("elvion/game/select_create/character_face/no_face.png")
			self.slot_buttons[slot_index].SetDownVisual("elvion/game/select_create/character_face/no_face.png")
			self.slot_name_texts[slot_index].SetText("")
			self.slot_level_texts[slot_index].SetText("")
			self.slot_guild_texts[slot_index].SetText("")
			self.slot_create_buttons[slot_index].Show()
		else:
			# Occupied slot
			race = character_data['race']
			face_name = self.RACE_FACE_IMAGES.get(race, "no_face")
			face_path = "elvion/game/select_create/character_face/{}.png".format(face_name)
			
			self.slot_buttons[slot_index].SetUpVisual(face_path)
			self.slot_buttons[slot_index].SetOverVisual(face_path)
			self.slot_buttons[slot_index].SetDownVisual(face_path)
			
			self.slot_name_texts[slot_index].SetText(character_data['name'])
			self.slot_level_texts[slot_index].SetText(str(character_data['level']))
			
			guild_name = character_data['guild_name'] if character_data['guild_name'] else localeInfo.SELECT_NOT_JOIN_GUILD
			self.slot_guild_texts[slot_index].SetText(guild_name)
			
			self.slot_create_buttons[slot_index].Hide()
	
	def Refresh(self):
		"""Refresh character data display."""
		if not self.is_load:
			return
	
		# Process slots in normal order (0, 1, 2, 3)
		for index in range(self.SLOT_COUNT):
			character_data = self._get_character_data(index)
			
			if character_data['id']:
				self._make_character_from_data(index, character_data)
				# Get full character data for display
				full_data = self._get_detailed_character_data(index)
				self._update_slot_display(index, full_data)
			else:
				self._update_slot_display(index, None)
	
		# Select the initial slot and hide other characters
		if self.slot >= 0:
			self.SelectSlot(self.slot)
		
		# Make sure only the selected character is visible
		for i in range(self.SLOT_COUNT):
			if chr.HasInstance(i):
				chr.SelectInstance(i)
				if i == self.slot:
					chr.Show()
				else:
					chr.Hide()
	
	def _get_detailed_character_data(self, index):
		"""Get detailed character data including level and guild."""
		char_id = net.GetAccountCharacterSlotDataInteger(index, net.ACCOUNT_CHARACTER_SLOT_ID)
		if char_id == 0:
			return None
			
		return {
			'id': char_id,
			'name': net.GetAccountCharacterSlotDataString(index, net.ACCOUNT_CHARACTER_SLOT_NAME),
			'level': net.GetAccountCharacterSlotDataInteger(index, net.ACCOUNT_CHARACTER_SLOT_LEVEL),
			'race': net.GetAccountCharacterSlotDataInteger(index, net.ACCOUNT_CHARACTER_SLOT_RACE),
			'form': net.GetAccountCharacterSlotDataInteger(index, net.ACCOUNT_CHARACTER_SLOT_FORM),
			'hair': net.GetAccountCharacterSlotDataInteger(index, net.ACCOUNT_CHARACTER_SLOT_HAIR),
			'acce': net.GetAccountCharacterSlotDataInteger(index, net.ACCOUNT_CHARACTER_SLOT_ACCE),
			'guild_name': net.GetAccountCharacterSlotDataString(index, net.ACCOUNT_CHARACTER_SLOT_GUILD_NAME),
			'playtime': net.GetAccountCharacterSlotDataInteger(index, net.ACCOUNT_CHARACTER_SLOT_PLAYTIME)
		}
		
	def _get_character_data(self, index):
		"""Get character data for a specific slot."""
		return {
			'id': net.GetAccountCharacterSlotDataInteger(index, net.ACCOUNT_CHARACTER_SLOT_ID),
			'race': net.GetAccountCharacterSlotDataInteger(index, net.ACCOUNT_CHARACTER_SLOT_RACE),
			'form': net.GetAccountCharacterSlotDataInteger(index, net.ACCOUNT_CHARACTER_SLOT_FORM),
			'name': net.GetAccountCharacterSlotDataString(index, net.ACCOUNT_CHARACTER_SLOT_NAME),
			'hair': net.GetAccountCharacterSlotDataInteger(index, net.ACCOUNT_CHARACTER_SLOT_HAIR),
			'acce': net.GetAccountCharacterSlotDataInteger(index, net.ACCOUNT_CHARACTER_SLOT_ACCE)
		}
		
	def _make_character_from_data(self, index, data):
		"""Create character from data dictionary."""
		self.MakeCharacter(index, data['id'], data['name'], data['race'], 
						data['form'], data['hair'], data['acce'])

	def GetCharacterSlotID(self, slot_index):
		"""Get character ID for a specific slot."""
		return net.GetAccountCharacterSlotDataInteger(slot_index, net.ACCOUNT_CHARACTER_SLOT_ID)

	def _load_question_dialog(self, file_name):
		"""Load the confirmation question dialog."""
		self.dlg_question = ui.ScriptWindow()

		try:
			py_scr_loader = ui.PythonScriptLoader()
			py_scr_loader.LoadScriptFile(self.dlg_question, file_name)
		except:
			import exception
			exception.Abort("SelectCharacterWindow.LoadQuestionDialog.LoadScript")

		try:
			get_object = self.dlg_question.GetChild
			self.dlg_question_text = get_object("message")
			self.dlg_question_accept_button = get_object("accept")
			self.dlg_question_cancel_button = get_object("cancel")
		except:
			import exception
			exception.Abort("SelectCharacterWindow.LoadQuestionDialog.BindObject")

		self._setup_question_dialog_events()
		return True
		
	def _setup_question_dialog_events(self):
		"""Setup question dialog button events."""
		self.dlg_question_text.SetText(localeInfo.SELECT_DO_YOU_DELETE_REALLY)
		self.dlg_question_accept_button.SetEvent(ui.__mem_func__(self.RequestDeleteCharacter))
		self.dlg_question_cancel_button.SetEvent(ui.__mem_func__(self.dlg_question.Hide))

	def _load_board_dialog(self, file_name):
		"""Load the main character selection board."""
		self.dlg_board = ui.ScriptWindow()

		try:
			py_scr_loader = ui.PythonScriptLoader()
			py_scr_loader.LoadScriptFile(self.dlg_board, file_name)
		except:
			import exception
			exception.Abort("SelectCharacterWindow.LoadBoardDialog.LoadScript")

		try:
			self._bind_board_objects()
		except:
			import exception
			exception.Abort("SelectCharacterWindow.LoadBoardDialog.BindObject")

		self._setup_board_events()
		self._setup_character_renderer()
		return True
		
	def _bind_board_objects(self):
		"""Bind UI objects from the board dialog."""
		get_object = self.dlg_board.GetChild
		
		self.btn_start = get_object("select_button")
		self.btn_delete = get_object("delete_button")
		self.btn_exit = get_object("exit_button")
		self.back_ground = get_object("BackGround")
		
		# Get stat value text elements DIRECTLY from main dialog
		# The children are accessible at the top level
		self.stat_hth = get_object("con_value")
		self.stat_int = get_object("int_value")
		self.stat_str = get_object("str_value")
		self.stat_dex = get_object("dex_value")
		
		# Get stat gauge elements
		self.gauge_hth = get_object("con_gauge")
		self.gauge_int = get_object("int_gauge")
		self.gauge_str = get_object("str_gauge")
		self.gauge_dex = get_object("dex_gauge")
		
		# Store slot UI elements
		self.slot_buttons = []
		self.slot_name_texts = []
		self.slot_level_texts = []
		self.slot_guild_texts = []
		self.slot_create_buttons = []
		
		# For each character slot
		for i in range(self.SLOT_COUNT):
			# Get the radio button
			char_slot = get_object("char{}".format(i))
			self.slot_buttons.append(char_slot)
			
			# Get child text elements DIRECTLY from the main dialog
			try:
				self.slot_name_texts.append(get_object("char{}_name_value".format(i)))
				self.slot_level_texts.append(get_object("char{}_level_value".format(i)))
				self.slot_guild_texts.append(get_object("char{}_guild_value".format(i)))
			except:
				import dbg
				dbg.TraceError("Failed to get text elements for slot {}".format(i))
				
			self.slot_create_buttons.append(get_object("char{}_create_button".format(i)))
			
			# Set up events
			char_slot.SetEvent(ui.__mem_func__(self.SelectSlot), i)
		
	def _setup_board_events(self):
		"""Setup board button events."""
		self.btn_start.SetEvent(ui.__mem_func__(self.StartGame))
		# Remove this line - we'll set create events per slot
		# self.btn_create.SetEvent(ui.__mem_func__(self.CreateCharacter))
		self.btn_exit.SetEvent(ui.__mem_func__(self.ExitSelect))
		self.btn_delete.SetEvent(ui.__mem_func__(self.InputPrivateCode))
		
		# Set create button events for each slot
		for i in range(self.SLOT_COUNT):
			self.slot_create_buttons[i].SetEvent(ui.__mem_func__(self.CreateCharacterForSlot), i)
		
		# Set tooltip events for each slot
		for i in range(self.SLOT_COUNT):
			self.slot_buttons[i].SetOverEvent(lambda index=i: self._show_slot_tooltip(index))
			self.slot_buttons[i].SetOverOutEvent(self._hide_slot_tooltip)
			
	def CreateCharacterForSlot(self, slot_index):
		"""Create a character in the specified slot."""
		character_id = self.GetCharacterSlotID(slot_index)
		
		if character_id != 0:
			# Slot is occupied
			return
		
		self.stream.SetCharacterSlot(slot_index)
		
		if self._should_show_empire_selection():
			self.stream.SetReselectEmpirePhase()
		else:
			self.stream.SetCreateCharacterPhase()

	def _show_slot_tooltip(self, slot_index):
		"""Show tooltip with playtime for character slot."""
		character_data = self._get_detailed_character_data(slot_index)
		
		if not character_data or character_data['id'] == 0:
			return
		
		playtime = character_data['playtime']
		
		# Calculate playtime display
		days = playtime / (60 * 24)
		hours = (playtime - (days * 60 * 24)) / 60
		minutes = playtime - (hours * 60) - (days * 60 * 24)
		
		playtime_text = localeInfo.SELECT_PLAYTIME + " : "
		if days:
			playtime_text += "{} {} ".format(days, localeInfo.DAY)
		if hours:
			playtime_text += "{} {} ".format(hours, localeInfo.HOUR)
		playtime_text += "{} {}".format(minutes, localeInfo.MINUTE)
		
		# Show tooltip (you'll need to create a tooltip instance)
		if not hasattr(self, 'slot_tooltip'):
			import uiToolTip
			self.slot_tooltip = uiToolTip.ToolTip()
		
		self.slot_tooltip.ClearToolTip()
		self.slot_tooltip.SetThinBoardSize(len(playtime_text) * 6 + 20)
		self.slot_tooltip.AppendTextLine(playtime_text, 0xffffff00)
		
		x, y = wndMgr.GetMousePosition()
		self.slot_tooltip.SetToolTipPosition(x + 50, y + 50)
		self.slot_tooltip.Show()
	
	def _hide_slot_tooltip(self):
		"""Hide the slot tooltip."""
		if hasattr(self, 'slot_tooltip'):
			self.slot_tooltip.Hide()
		
	def _setup_character_renderer(self):
		"""Setup the character renderer component."""
		self.chr_renderer = CharacterRenderer()
		self.chr_renderer.SetParent(self.back_ground)
		self.chr_renderer.Show()

	def SameLoginDisconnect(self):
		"""Handle same login disconnect event."""
		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(localeInfo.LOGIN_FAILURE_SAMELOGIN, 
								self.ExitSelect, localeInfo.UI_OK)

	def MakeCharacter(self, index, character_id, name, race, form, hair, acce):
		"""Create a character instance with given parameters."""
		if character_id == 0:
			return
	
		chr.CreateInstance(index)
		chr.SelectInstance(index)
		chr.SetVirtualID(index)
		chr.SetNameString(name)
		chr.SetRace(race)
		chr.SetArmor(form)
		chr.SetHair(hair)
		chr.SetAcce(acce)
		chr.Refresh()
		chr.SetMotionMode(chr.MOTION_MODE_GENERAL)
		chr.SetLoopMotion(chr.MOTION_INTRO_WAIT)
		chr.SetRotation(0.0)
		
		# Hide by default - will be shown when selected
		if index != self.slot:
			chr.Hide()

	def StartGame(self):
		"""Start the game with selected character."""
		if self._should_prevent_start():
			return
			
		if self.change_name_flag:
			self.OpenChangeNameDialog()
			return

		if self.start_index != -1:
			return

		self._prepare_game_start()
		self._set_character_animations()
		
	def _should_prevent_start(self):
		"""Check if game start should be prevented."""
		return self.sended_change_name_packet
		
	def _prepare_game_start(self):
		"""Prepare UI and game state for starting."""
		if musicInfo.selectMusic != "":
			snd.FadeLimitOutMusic("content/sound/BGM/" + musicInfo.selectMusic, 
								systemSetting.GetMusicVolume() * 0.05)

		self._disable_all_buttons()
		self.dlg_question.Hide()
		self.stream.SetCharacterSlot(self.slot)
		self.selected = 1
		self.start_index = self.slot
		self.start_reserving_time = app.GetTime()
		
	def _disable_all_buttons(self):
		"""Disable all UI buttons."""
		buttons = [self.btn_start, self.btn_delete, self.btn_exit]
		for button in buttons:
			if button:
				button.SetUp()
				button.Disable()
			
	def _set_character_animations(self):
		"""Set character selection animations."""
		for i in range(self.SLOT_COUNT):
			if not chr.HasInstance(i):
				continue

			chr.SelectInstance(i)
			if i == self.slot:
				chr.PushOnceMotion(chr.MOTION_INTRO_SELECTED, 0.1)
			else:
				chr.PushOnceMotion(chr.MOTION_INTRO_NOT_SELECTED, 0.1)

	def OpenChangeNameDialog(self):
		"""Open the change name dialog."""
		name_input_board = uiCommon.InputDialogWithDescription()
		name_input_board.SetTitle(localeInfo.SELECT_CHANGE_NAME_TITLE)
		name_input_board.SetAcceptEvent(ui.__mem_func__(self.AcceptInputName))
		name_input_board.SetCancelEvent(ui.__mem_func__(self.CancelInputName))
		name_input_board.SetMaxLength(chr.PLAYER_NAME_MAX_LEN)
		name_input_board.SetBoardWidth(200)
		name_input_board.SetDescription(localeInfo.SELECT_INPUT_CHANGING_NAME)
		name_input_board.Open()
		name_input_board.slot = self.slot
		self.name_input_board = name_input_board

	def OnChangeName(self, character_id, name):
		"""Handle name change confirmation."""
		self.SelectSlot(character_id)
		self.sended_change_name_packet = False
		self.PopupMessage(localeInfo.SELECT_CHANGED_NAME)

	def AcceptInputName(self):
		"""Accept the new character name."""
		change_name = self.name_input_board.GetText()
		if not change_name:
			return

		self.sended_change_name_packet = True
		net.SendChangeNamePacket(self.name_input_board.slot, change_name)
		return self.CancelInputName()

	def CancelInputName(self):
		"""Cancel name input dialog."""
		if self.name_input_board:
			self.name_input_board.Close()
			self.name_input_board = None
		return True

	def OnCreateFailure(self, failure_type):
		"""Handle character creation failure."""
		self.sended_change_name_packet = False
		
		failure_messages = {
			0: localeInfo.SELECT_CHANGE_FAILURE_STRANGE_NAME,
			1: localeInfo.SELECT_CHANGE_FAILURE_ALREADY_EXIST_NAME,
			100: localeInfo.SELECT_CHANGE_FAILURE_STRANGE_INDEX
		}
		
		message = failure_messages.get(failure_type, "Unknown error")
		self.PopupMessage(message)

	def CreateCharacter(self):
		"""Create a new character in the currently selected slot."""
		if self.slot >= 0 and self.slot < self.SLOT_COUNT:
			self.CreateCharacterForSlot(self.slot)
				
	def _should_show_empire_selection(self):
		"""Check if empire selection should be shown."""
		EMPIRE_MODE = 1
		return EMPIRE_MODE and self._are_all_slots_empty()

	def _are_all_slots_empty(self):
		"""Check if all character slots are empty."""
		for slot in range(self.SLOT_COUNT):
			if net.GetAccountCharacterSlotDataInteger(slot, net.ACCOUNT_CHARACTER_SLOT_ID) != 0:
				return False
		return True

	def PopupDeleteQuestion(self):
		"""Show character deletion confirmation dialog."""
		character_id = self.GetCharacterSlotID(self.slot)
		if character_id == 0:
			return

		self.dlg_question.Show()
		self.dlg_question.SetTop()

	def RequestDeleteCharacter(self):
		"""Request character deletion."""
		self.dlg_question.Hide()

		character_id = self.GetCharacterSlotID(self.slot)
		if character_id == 0:
			self.PopupMessage(localeInfo.SELECT_EMPTY_SLOT)
			return

		net.SendDestroyCharacterPacket(self.slot, "1234567")
		self.PopupMessage(localeInfo.SELECT_DELEING)

	def InputPrivateCode(self):
		"""Show private code input dialog for character deletion."""
		private_input_board = uiCommon.InputDialogWithDescription()
		private_input_board.SetTitle(localeInfo.INPUT_PRIVATE_CODE_DIALOG_TITLE)
		private_input_board.SetAcceptEvent(ui.__mem_func__(self.AcceptInputPrivateCode))
		private_input_board.SetCancelEvent(ui.__mem_func__(self.CancelInputPrivateCode))
		private_input_board.SetNumberMode()
		private_input_board.SetSecretMode()
		private_input_board.SetMaxLength(7)
		private_input_board.SetBoardWidth(250)
		private_input_board.SetDescription(localeInfo.INPUT_PRIVATE_CODE_DIALOG_DESCRIPTION)
		private_input_board.Open()
		self.private_input_board = private_input_board

	def AcceptInputPrivateCode(self):
		"""Accept private code and delete character."""
		private_code = self.private_input_board.GetText()
		if not private_code:
			return

		character_id = self.GetCharacterSlotID(self.slot)
		if character_id == 0:
			self.PopupMessage(localeInfo.SELECT_EMPTY_SLOT)
			return

		net.SendDestroyCharacterPacket(self.slot, private_code)
		self.PopupMessage(localeInfo.SELECT_DELEING)
		self.CancelInputPrivateCode()
		return True

	def CancelInputPrivateCode(self):
		"""Cancel private code input."""
		self.private_input_board = None
		return True

	def OnDeleteSuccess(self, slot):
		"""Handle successful character deletion."""
		self.PopupMessage(localeInfo.SELECT_DELETED)
		self.DeleteCharacter(slot)
	
	def DeleteCharacter(self, index):
		"""Delete character from slot."""
		chr.DeleteInstance(index)
		
		# Update the slot display to show it's empty
		self._update_slot_display(index, None)
		
		# If we deleted the currently selected slot, hide the action buttons
		if index == self.slot:
			if hasattr(self, 'btn_start') and self.btn_start:
				self.btn_start.Hide()
			if hasattr(self, 'btn_delete') and self.btn_delete:
				self.btn_delete.Hide()
		
		# Re-select the current slot to update the display
		self.SelectSlot(self.slot)

	def OnDeleteFailure(self):
		"""Handle character deletion failure."""
		self.PopupMessage(localeInfo.SELECT_CAN_NOT_DELETE)

	def ExitSelect(self):
		"""Exit character selection."""
		self.dlg_question.Hide()
		self.Hide()
		self.stream.SetLoginPhase()

	def GetSlotIndex(self):
		"""Get currently selected slot index."""
		return self.slot

	def SelectSlot(self, index):
		"""Select a character slot."""
		if not self._is_valid_slot_index(index):
			return
	
		self.slot = index
		
		# Update radio button states - set all to Up, then set selected to Down
		for i in range(self.SLOT_COUNT):
			if self.slot_buttons[i]:
				self.slot_buttons[i].SetUp()
		
		# Set the selected button to Down
		if self.slot_buttons[index]:
			self.slot_buttons[index].Down()
		
		chr.SelectInstance(self.slot)
		
		character_id = net.GetAccountCharacterSlotDataInteger(self.slot, net.ACCOUNT_CHARACTER_SLOT_ID)
		
		if character_id != 0:
			# Show action buttons
			if hasattr(self, 'btn_start') and self.btn_start:
				self.btn_start.Show()
			if hasattr(self, 'btn_delete') and self.btn_delete:
				self.btn_delete.Show()
			
			# Get stat values
			value_hth = net.GetAccountCharacterSlotDataInteger(self.slot, net.ACCOUNT_CHARACTER_SLOT_HTH)
			value_int = net.GetAccountCharacterSlotDataInteger(self.slot, net.ACCOUNT_CHARACTER_SLOT_INT)
			value_str = net.GetAccountCharacterSlotDataInteger(self.slot, net.ACCOUNT_CHARACTER_SLOT_STR)
			value_dex = net.GetAccountCharacterSlotDataInteger(self.slot, net.ACCOUNT_CHARACTER_SLOT_DEX)
			
			# Update stat text displays
			self.stat_hth.SetText(str(value_hth))
			self.stat_int.SetText(str(value_int))
			self.stat_str.SetText(str(value_str))
			self.stat_dex.SetText(str(value_dex))
			
			# Calculate and set gauge rendering rectangles
			# SetRenderingRect parameters: (left, top, right, bottom)
			# We use -1.0 + percentage for the right value to show percentage filled
			stats_summary = float(value_hth + value_int + value_str + value_dex)
			if stats_summary > 0.0:
				hth_percent = float(value_hth) / 90.0
				int_percent = float(value_int) / 90.0
				str_percent = float(value_str) / 90.0
				dex_percent = float(value_dex) / 90.0
				
				self.gauge_hth.SetRenderingRect(0.0, 0.0, -1.0 + hth_percent, 0.0)
				self.gauge_int.SetRenderingRect(0.0, 0.0, -1.0 + int_percent, 0.0)
				self.gauge_str.SetRenderingRect(0.0, 0.0, -1.0 + str_percent, 0.0)
				self.gauge_dex.SetRenderingRect(0.0, 0.0, -1.0 + dex_percent, 0.0)
			else:
				self.gauge_hth.SetRenderingRect(0.0, 0.0, 0.0, 0.0)
				self.gauge_int.SetRenderingRect(0.0, 0.0, 0.0, 0.0)
				self.gauge_str.SetRenderingRect(0.0, 0.0, 0.0, 0.0)
				self.gauge_dex.SetRenderingRect(0.0, 0.0, 0.0, 0.0)
			
			# Make sure the right character is visible
			for i in range(self.SLOT_COUNT):
				if chr.HasInstance(i):
					chr.SelectInstance(i)
					if i == self.slot:
						chr.Show()
					else:
						chr.Hide()
			
			# Create character instance if it doesn't exist
			if not chr.HasInstance(self.slot):
				character_data = self._get_detailed_character_data(index)
				if character_data:
					self._create_character_instance(index, character_data)
		else:
			# Empty slot - hide buttons and clear stats
			if hasattr(self, 'btn_start') and self.btn_start:
				self.btn_start.Hide()
			if hasattr(self, 'btn_delete') and self.btn_delete:
				self.btn_delete.Hide()
			
			# Clear stat displays
			self.stat_hth.SetText("0")
			self.stat_int.SetText("0")
			self.stat_str.SetText("0")
			self.stat_dex.SetText("0")
			
			# Reset gauges
			self.gauge_hth.SetRenderingRect(0.0, 0.0, 0.0, 0.0)
			self.gauge_int.SetRenderingRect(0.0, 0.0, 0.0, 0.0)
			self.gauge_str.SetRenderingRect(0.0, 0.0, 0.0, 0.0)
			self.gauge_dex.SetRenderingRect(0.0, 0.0, 0.0, 0.0)
			
	def _is_valid_slot_index(self, index):
		"""Check if slot index is valid."""
		return 0 <= index < self.SLOT_COUNT
		
	def _clear_all_character_instances(self):
		"""Clear all character instances."""
		for i in range(self.SLOT_COUNT):
			chr.DeleteInstance(i)
			
	def _display_character_info(self, index):
		"""Display character information for selected slot."""
		# Show main action buttons
		if hasattr(self, 'btn_start') and self.btn_start:
			self.btn_start.Show()
		if hasattr(self, 'btn_delete') and self.btn_delete:
			self.btn_delete.Show()
	
		# Get character data and create instance
		character_data = self._get_detailed_character_data(index)
		if character_data:
			self._create_character_instance(character_data)
		
	def _create_character_instance(self, index, data):
		"""Create character instance from data."""
		self.change_name_flag = net.GetAccountCharacterSlotDataInteger(
			index, net.ACCOUNT_CHARACTER_SLOT_CHANGE_NAME_FLAG)
		self.MakeCharacter(index, data['id'], data['name'], data['race'], 
						data['form'], data['hair'], data['acce'])
						
	def _update_character_info_display(self, data):
		pass

	def InitCharacterBoard(self):
		pass

	def OnKeyDown(self, key):
		"""Handle keyboard input."""
		# Use app constants for better compatibility
		if key == app.DIK_ESCAPE:
			return self.OnPressEscapeKey()
		elif key == app.DIK_RETURN or key == app.DIK_NUMPADENTER:
			self._handle_enter_key()
			return True
		elif key == app.DIK_LEFT or key == app.DIK_A:
			self._decrease_slot_index()
			return True
		elif key == app.DIK_RIGHT or key == app.DIK_D:
			self._increase_slot_index()
			return True
		elif key == app.DIK_1:
			self.SelectSlot(0)
			return True
		elif key == app.DIK_2:
			self.SelectSlot(1)
			return True
		elif key == app.DIK_3:
			self.SelectSlot(2)
			return True
		elif key == app.DIK_4:
			self.SelectSlot(3)
			return True
			
		# Legacy key mapping for backward compatibility
		key_handlers = {
			1: self.ExitSelect,
			2: lambda: self.SelectSlot(0),
			3: lambda: self.SelectSlot(1),
			4: lambda: self.SelectSlot(2),
			5: lambda: self.SelectSlot(3),
			28: self._handle_enter_key,
			203: self._decrease_slot_index,
			205: self._increase_slot_index
		}
		
		handler = key_handlers.get(key)
		if handler:
			handler()
			
		return True
		
	def _handle_enter_key(self):
		"""Handle enter key press."""
		character_id = net.GetAccountCharacterSlotDataInteger(self.slot, net.ACCOUNT_CHARACTER_SLOT_ID)
		if character_id == 0:
			self.CreateCharacter()
		else:
			self.StartGame()

	def _decrease_slot_index(self):
		"""Move to previous slot."""
		slot_index = (self.GetSlotIndex() - 1 + self.SLOT_COUNT) % self.SLOT_COUNT
		self.SelectSlot(slot_index)

	def _increase_slot_index(self):
		"""Move to next slot."""
		slot_index = (self.GetSlotIndex() + 1) % self.SLOT_COUNT
		self.SelectSlot(slot_index)

	def OnUpdate(self):
		"""Update loop for character animation and game loading."""
		chr.Update()

		for i in range(self.SLOT_COUNT):
			if not chr.HasInstance(i):
				continue
			chr.SelectInstance(i)

		if self.start_index != -1:
			if not self.open_loading_flag:
				chr_slot = self.stream.GetCharacterSlot()
				net.DirectEnter(chr_slot)
				self.open_loading_flag = True
				player.SetPlayTime(net.GetAccountCharacterSlotDataInteger(
					self.slot, net.ACCOUNT_CHARACTER_SLOT_PLAYTIME))
				chat.Clear()

	def EmptyFunc(self):
		"""Empty function for default callbacks."""
		pass

	def PopupMessage(self, msg, func=None):
		"""Display popup message."""
		if func is None:
			func = self.EmptyFunc

		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(msg, func, localeInfo.UI_OK)

	def OnPressExitKey(self):
		"""Handle exit key press."""
		self.ExitSelect()
		return True
	
	def OnPressEscapeKey(self):
		"""Handle escape key press."""
		if self.dlg_question and self.dlg_question.IsShow():
			self.dlg_question.Hide()
			return True
		if self.name_input_board:
			self.CancelInputName()
			return True
		if self.private_input_board:
			self.CancelInputPrivateCode()
			return True
		return self.OnPressExitKey()
	
	def OnIMEReturn(self):
		"""Handle IME Return key press."""
		self._handle_enter_key()
		return True