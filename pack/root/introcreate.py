#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Modern Character Creation Window
Rewritten for better maintainability and cleaner code structure
"""

import chr
import grp
import app
import net
import snd
import wndMgr
import systemSetting
import localeInfo
import ui
import musicInfo
import playerSettingModule
import uiScriptLocale


class CharacterConstants(object):
	"""Constants for character creation"""
	LOCALE_PATH = "uiscript/" + uiScriptLocale.CODEPAGE + "_"
	
	# Gender constants
	MALE = 0
	FEMALE = 1
	
	# Shape constants
	SHAPE_0 = 0
	SHAPE_1 = 1
	
	# Slot configuration
	PAGE_COUNT = 2
	SLOT_COUNT = 4
	BASE_CHR_ID = 3
	
	# Animation timing
	CREATION_DELAY = 1.5


class CharacterData(object):
	"""Manages character race data"""
	
	def __init__(self):
		self.race_data = {
			CharacterConstants.MALE: [
				playerSettingModule.RACE_WARRIOR_M,
				playerSettingModule.RACE_ASSASSIN_M,
				playerSettingModule.RACE_SURA_M,
				playerSettingModule.RACE_SHAMAN_M
			],
			CharacterConstants.FEMALE: [
				playerSettingModule.RACE_WARRIOR_W,
				playerSettingModule.RACE_ASSASSIN_W,
				playerSettingModule.RACE_SURA_W,
				playerSettingModule.RACE_SHAMAN_W
			]
		}
	
	def get_race(self, gender, slot):
		"""Get race ID for given gender and slot"""
		return self.race_data.get(gender, [])[slot] if slot < len(self.race_data.get(gender, [])) else None


class CharacterRenderer(ui.Window):
	"""Handles 3D character rendering"""
	
	def __init__(self):
		super(CharacterRenderer, self).__init__()
	
	def OnRender(self):
		"""Render the 3D character model"""
		grp.ClearDepthBuffer()
		grp.SetGameRenderState()
		grp.PushState()
		grp.SetOmniLight()

		# Calculate screen dimensions with padding
		screen_width = wndMgr.GetScreenWidth()
		screen_height = wndMgr.GetScreenHeight()
		new_screen_width = float(screen_width + 30)
		new_screen_height = float(screen_height + 30)
		
		# Set up viewport and camera
		grp.SetViewport(0.0, 0.0, 1.0, 1.0)
		app.SetCenterPosition(0, -40.0, 30.0)
		app.SetCamera(2000.0, 15.0, 180.0, 90.0)
		grp.SetPerspective(12.0, new_screen_width / new_screen_height, 1000.0, 3000.0)

		# Set cursor position
		cursor_x, cursor_y = app.GetCursorPosition()
		grp.SetCursorPosition(cursor_x, cursor_y)

		# Render character
		chr.Deform()
		chr.Render()

		# Restore state
		grp.RestoreViewport()
		grp.PopState()
		grp.SetInterfaceRenderState()


class UIManager(object):
	"""Manages UI elements and their events"""
	
	def __init__(self, character_window):
		self.character_window = character_window
		self.ui_elements = {}
		self.button_groups = {}
	
	def load_ui(self):
		"""Load and setup UI elements"""
		try:
			dlg_board = ui.ScriptWindow()
			script_loader = ui.PythonScriptLoader()
			script_loader.LoadScriptFile(dlg_board, "elvion/game/select_create/createcharacterwindow.py")
			
			# Store main dialog
			self.ui_elements['dialog'] = dlg_board
			
			# Get UI elements
			get_child = dlg_board.GetChild
			self._setup_buttons(get_child)
			self._setup_input(get_child)
			self._setup_character_slots(get_child)
			self._setup_background(get_child)
			
			return dlg_board
			
		except Exception:
			import exception
			exception.Abort("UIManager.load_ui.LoadObject")
	
	def _setup_buttons(self, get_child):
		"""Setup button elements"""
		self.ui_elements['btn_create'] = get_child("create_button")
		self.ui_elements['btn_exit'] = get_child("exit_button")
	
	def _setup_input(self, get_child):
		"""Setup input elements"""
		self.ui_elements['edit_name'] = get_child("name_value")
	
	def _setup_character_slots(self, get_child):
		"""Setup character slot buttons"""
		self.button_groups['slots'] = [
			get_child("char0"),
			get_child("char1"),
			get_child("char2"),
			get_child("char3")
		]
		
		# Store gender and shape buttons for each slot with unique names
		self.button_groups['gender_buttons'] = {}
		self.button_groups['shape_buttons'] = {}
		
		for i in range(4):
			self.button_groups['gender_buttons'][i] = {
				'man': get_child("char%d_man" % i),
				'woman': get_child("char%d_woman" % i)
			}
			self.button_groups['shape_buttons'][i] = {
				'shape1': get_child("char%d_shape1" % i),
				'shape2': get_child("char%d_shape2" % i)
			}
	
	def _setup_background(self, get_child):
		"""Setup background element"""
		self.ui_elements['background'] = get_child("BackGround")
	
	def bind_events(self):
		"""Bind events to UI elements"""
		self.ui_elements['btn_create'].SetEvent(
			ui.__mem_func__(self.character_window.create_character)
		)
		self.ui_elements['btn_exit'].SetEvent(
			ui.__mem_func__(self.character_window.cancel_create)
		)
		
		# Bind events for each slot and its children
		for i, slot_button in enumerate(self.button_groups['slots']):
			slot_button.SetEvent(
				ui.__mem_func__(self.character_window.select_slot), i
			)
			
			# Bind gender buttons for this slot
			gender_btns = self.button_groups['gender_buttons'][i]
			gender_btns['man'].SetEvent(ui.__mem_func__(self.character_window.select_gender), CharacterConstants.MALE)
			gender_btns['woman'].SetEvent(ui.__mem_func__(self.character_window.select_gender), CharacterConstants.FEMALE)
			
			# Bind shape buttons for this slot
			shape_btns = self.button_groups['shape_buttons'][i]
			shape_btns['shape1'].SetEvent(ui.__mem_func__(self.character_window.select_shape), CharacterConstants.SHAPE_0)
			shape_btns['shape2'].SetEvent(ui.__mem_func__(self.character_window.select_shape), CharacterConstants.SHAPE_1)
		
		name_input = self.ui_elements['edit_name']
		name_input.SetReturnEvent(
			ui.__mem_func__(self.character_window.create_character)
		)
		name_input.SetEscapeEvent(
			ui.__mem_func__(self.character_window.cancel_create)
		)
	
	def get_element(self, key):
		"""Get UI element by key"""
		return self.ui_elements.get(key)
	
	def get_button_group(self, group_name):
		"""Get button group by name"""
		return self.button_groups.get(group_name, [])


class CharacterState(object):
	"""Manages character creation state"""
	
	def __init__(self):
		self.reset()
	
	def reset(self):
		"""Reset state to defaults"""
		self.gender = CharacterConstants.MALE
		self.slot = -1
		self.shape = CharacterConstants.SHAPE_0
		self.reserving_race_index = -1
		self.reserving_shape_index = -1
		self.reserving_start_time = 0


class CreateCharacterWindow(ui.Window):
	"""Modern character creation window with clean architecture"""
	
	def __init__(self, stream):
		super(CreateCharacterWindow, self).__init__()
		
		# Core components
		self.stream = stream
		self.state = CharacterState()
		self.character_data = CharacterData()
		self.ui_manager = UIManager(self)
		self.renderer = None
		
		# Set network phase
		net.SetPhaseWindow(net.PHASE_WINDOW_CREATE, self)
	
	def __del__(self):
		print "DELETE CREATE WINDOW"
		net.SetPhaseWindow(net.PHASE_WINDOW_CREATE, 0)
		super(CreateCharacterWindow, self).__del__()
	
	def Open(self):
		"""Open and initialize the character creation window"""
		print "OPEN CREATE WINDOW"
		
		if not app.ENABLE_PERFORMANCE_IMPROVEMENTS_NEW:
			playerSettingModule.LoadGameData("INIT")
		
		self.state.reset()
		
		self._setup_ui()
		self._setup_renderer()
		self._initialize_characters()
		
		self._show_window()
		
		self._setup_audio()
		
		app.ShowCursor()
	
	def Close(self):
		"""Close the character creation window"""
		# Stop music
		if musicInfo.createMusic:
			snd.FadeOutMusic("content/sound/BGM/" + musicInfo.createMusic)
		
		# Clean up character instances
		self._cleanup_characters()
		
		# Hide UI
		self.ui_manager.get_element('dialog').Hide()
		self.Hide()
		
		# Hide cursor
		app.HideCursor()
	
	def _setup_ui(self):
		"""Setup UI components"""
		dialog = self.ui_manager.load_ui()
		self.ui_manager.bind_events()
		
		name_input = self.ui_manager.get_element('edit_name')
		name_input.SetText("")
		
		# Hide all slot children initially
		self._hide_all_slot_children()
	
	def _hide_all_slot_children(self):
		"""Hide all gender and shape buttons initially"""
		for i in range(4):
			gender_btns = self.ui_manager.button_groups['gender_buttons'][i]
			shape_btns = self.ui_manager.button_groups['shape_buttons'][i]
			
			gender_btns['man'].Hide()
			gender_btns['woman'].Hide()
			shape_btns['shape1'].Hide()
			shape_btns['shape2'].Hide()
	
	def _setup_renderer(self):
		"""Setup 3D character renderer"""
		self.renderer = CharacterRenderer()
		background = self.ui_manager.get_element('background')
		self.renderer.SetParent(background)
		self.renderer.Show()
	
	def _initialize_characters(self):
		"""Initialize character instances"""
		self.enable_window()
		random_slot = app.GetRandom(0, CharacterConstants.SLOT_COUNT)
		self.select_slot(random_slot)
		app.SetCamera(500.0, 10.0, 180.0, 95.0)
	
	def _show_window(self):
		"""Show the window and dialog"""
		self.Show()
		self.ui_manager.get_element('dialog').Show()
	
	def _setup_audio(self):
		"""Setup background music"""
		if musicInfo.createMusic:
			snd.SetMusicVolume(systemSetting.GetMusicVolume())
			snd.FadeInMusic("content/sound/BGM/" + musicInfo.createMusic)
	
	def _cleanup_characters(self):
		"""Clean up character instances"""
		total_instances = CharacterConstants.BASE_CHR_ID + (
			CharacterConstants.SLOT_COUNT * CharacterConstants.PAGE_COUNT
		)
		for chr_id in xrange(total_instances):
			chr.DeleteInstance(chr_id)
	
	def enable_window(self):
		"""Enable all window controls"""
		self.state.reserving_race_index = -1
		self.state.reserving_shape_index = -1
		
		btn_create = self.ui_manager.get_element('btn_create')
		btn_exit = self.ui_manager.get_element('btn_exit')
		name_input = self.ui_manager.get_element('edit_name')
		
		if btn_create:
			btn_create.Enable()
		if btn_exit:
			btn_exit.Enable()
		
		# Enable all slot buttons only (NOT their children yet)
		for button in self.ui_manager.get_button_group('slots'):
			if button:
				button.Enable()
		
		if name_input:
			name_input.SetFocus()
			name_input.Enable()
		
		self._setup_character_animations()
	
	def disable_window(self):
		"""Disable all window controls"""
		btn_create = self.ui_manager.get_element('btn_create')
		btn_exit = self.ui_manager.get_element('btn_exit')
		name_input = self.ui_manager.get_element('edit_name')
		
		if btn_create:
			btn_create.Disable()
		if btn_exit:
			btn_exit.Disable()
		if name_input:
			name_input.Disable()
		
		# Disable all slot buttons
		for button in self.ui_manager.get_button_group('slots'):
			if button:
				button.Disable()
	
	def _toggle_slot_children_visibility(self, selected_slot):
		"""Show children only for selected slot, hide others"""
		for i in range(4):
			gender_btns = self.ui_manager.button_groups['gender_buttons'][i]
			shape_btns = self.ui_manager.button_groups['shape_buttons'][i]
			
			if i == selected_slot:
				# Show and enable children for selected slot
				gender_btns['man'].Show()
				gender_btns['woman'].Show()
				shape_btns['shape1'].Show()
				shape_btns['shape2'].Show()
				
				gender_btns['man'].Enable()
				gender_btns['woman'].Enable()
				shape_btns['shape1'].Enable()
				shape_btns['shape2'].Enable()
				
				# Set default states
				gender_btns['man'].SetUp()
				gender_btns['woman'].SetUp()
				shape_btns['shape1'].SetUp()
				shape_btns['shape2'].SetUp()
				
				# Set current gender state
				if self.state.gender == CharacterConstants.MALE:
					gender_btns['man'].Down()
				else:
					gender_btns['woman'].Down()
				
				# Set current shape state
				if self.state.shape == CharacterConstants.SHAPE_0:
					shape_btns['shape1'].Down()
				else:
					shape_btns['shape2'].Down()
			else:
				# Hide and disable children for non-selected slots
				gender_btns['man'].Hide()
				gender_btns['woman'].Hide()
				shape_btns['shape1'].Hide()
				shape_btns['shape2'].Hide()
				
				gender_btns['man'].Disable()
				gender_btns['woman'].Disable()
				shape_btns['shape1'].Disable()
				shape_btns['shape2'].Disable()
	
	def _setup_character_animations(self):
		"""Setup idle animations for all characters"""
		for page in xrange(CharacterConstants.PAGE_COUNT):
			for slot in xrange(CharacterConstants.SLOT_COUNT):
				chr_id = self._get_slot_chr_id(page, slot)
				chr.SelectInstance(chr_id)
				chr.BlendLoopMotion(chr.MOTION_INTRO_WAIT, 0.1)
	
	def _get_slot_chr_id(self, page, slot):
		"""Get character ID for given page and slot"""
		return CharacterConstants.BASE_CHR_ID + page * CharacterConstants.SLOT_COUNT + slot
	
	def _make_character(self, chr_id, race):
		"""Create and setup a character instance"""
		chr.CreateInstance(chr_id)
		chr.SelectInstance(chr_id)
		chr.SetVirtualID(chr_id)
		chr.SetRace(race)
		chr.SetArmor(0)
		chr.SetHair(0)
		chr.Refresh()
		chr.SetMotionMode(chr.MOTION_MODE_GENERAL)
		chr.SetLoopMotion(chr.MOTION_INTRO_WAIT)
		chr.SetRotation(0.0)
		chr.Hide()
	
	def select_gender(self, gender):
		"""Select character gender"""
		if self.state.slot < 0:
			return
		
		# Update button states for current slot only
		gender_btns = self.ui_manager.button_groups['gender_buttons'][self.state.slot]
		
		gender_btns['man'].SetUp()
		gender_btns['woman'].SetUp()
		
		if gender == CharacterConstants.MALE:
			gender_btns['man'].Down()
		else:
			gender_btns['woman'].Down()
		
		self.state.gender = gender
		
		# Show/hide appropriate character models
		self._toggle_gender_visibility(gender)
		
		# Recreate current character
		self._recreate_current_character()
	
	def select_shape(self, shape):
		"""Select character shape"""
		if self.state.slot < 0:
			return
		
		self.state.shape = shape
		
		# Update button states for current slot only
		shape_btns = self.ui_manager.button_groups['shape_buttons'][self.state.slot]
		
		shape_btns['shape1'].SetUp()
		shape_btns['shape2'].SetUp()
		
		if shape == CharacterConstants.SHAPE_0:
			shape_btns['shape1'].Down()
		else:
			shape_btns['shape2'].Down()
		
		if self.state.slot >= 0:
			chr_id = self._get_slot_chr_id(self.state.gender, self.state.slot)
			chr.SelectInstance(chr_id)
			chr.ChangeShape(shape)
			chr.SetMotionMode(chr.MOTION_MODE_GENERAL)
			chr.SetLoopMotion(chr.MOTION_INTRO_WAIT)
	
	def _apply_gender_visibility(self):
		"""Apply gender visibility without recreation"""
		self._toggle_gender_visibility(self.state.gender)
	
	def _toggle_gender_visibility(self, gender):
		"""Toggle character visibility based on gender"""
		for i in xrange(CharacterConstants.SLOT_COUNT):
			male_chr_id = self._get_slot_chr_id(CharacterConstants.MALE, i)
			female_chr_id = self._get_slot_chr_id(CharacterConstants.FEMALE, i)
			
			chr.SelectInstance(male_chr_id)
			chr.Show() if gender == CharacterConstants.MALE else chr.Hide()
			
			chr.SelectInstance(female_chr_id)
			chr.Show() if gender == CharacterConstants.FEMALE else chr.Hide()
	
	def get_slot_index(self):
		"""Get current slot index"""
		return self.state.slot
	
	def select_slot(self, slot):
		"""Select character slot"""
		if not self._is_valid_slot(slot):
			return
		
		prev_slot = self.state.slot
		self.state.slot = slot
		
		if self.IsShow():
			snd.PlaySound("sound/ui/click.wav")
		
		self._update_slot_selection(slot)
		
		# Always show children for the selected slot
		# But only recreate character if actually changing slots
		self._toggle_slot_children_visibility(slot)
		
		if prev_slot != slot:
			self._recreate_current_character()
	
	def _is_valid_slot(self, slot):
		"""Check if slot index is valid"""
		return slot is not None and 0 <= slot < CharacterConstants.SLOT_COUNT
	
	def _update_slot_selection(self, slot):
		"""Update slot button states"""
		slot_buttons = self.ui_manager.get_button_group('slots')
		if slot_buttons and slot < len(slot_buttons):
			for button in slot_buttons:
				button.SetUp()
			slot_buttons[slot].Down()
	
	def _recreate_current_character(self):
		"""Recreate the currently selected character"""
		if self.state.slot < 0:
			return
		
		chr_id = self._get_slot_chr_id(self.state.gender, self.state.slot)
		race = self.character_data.get_race(self.state.gender, self.state.slot)
		
		if race is not None:
			# Clean up existing instances
			self._cleanup_characters()
			
			# Create new character
			self._make_character(chr_id, race)
			self.select_shape(self.state.shape)
			self._apply_gender_visibility()
	
	def create_character(self):
		"""Initiate character creation process"""
		if self.state.reserving_race_index != -1:
			return
		
		character_name = self.ui_manager.get_element('edit_name').GetText()
		if not self._validate_character_name(character_name):
			return
		
		if musicInfo.selectMusic:
			snd.FadeLimitOutMusic(
				"content/sound/BGM/" + musicInfo.selectMusic,
				systemSetting.GetMusicVolume() * 0.05
			)
		
		self.disable_window()
		self._start_character_creation()
	
	def _validate_character_name(self, name):
		"""Validate character name"""
		if not name:
			self._popup_message(localeInfo.CREATE_INPUT_NAME, self.enable_window)
			return False
		
		if localeInfo.CREATE_GM_NAME in name:
			self._popup_message(localeInfo.CREATE_ERROR_GM_NAME, self.enable_window)
			return False
		
		if net.IsInsultIn(name):
			self._popup_message(localeInfo.CREATE_ERROR_INSULT_NAME, self.enable_window)
			return False
		
		return True
	
	def _start_character_creation(self):
		"""Start the character creation animation sequence"""
		chr.SelectInstance(self._get_slot_chr_id(self.state.gender, self.state.slot))
		
		self.state.reserving_race_index = chr.GetRace()
		self.state.reserving_shape_index = self.state.shape
		self.state.reserving_start_time = app.GetTime()
		
		for slot in xrange(CharacterConstants.SLOT_COUNT):
			chr.SelectInstance(self._get_slot_chr_id(self.state.gender, slot))
			chr.PushOnceMotion(chr.MOTION_INTRO_SELECTED)
	
	def cancel_create(self):
		"""Cancel character creation and return to selection"""
		self.stream.SetSelectCharacterPhase()
	
	def OnCreateSuccess(self):
		"""Handle successful character creation"""
		self.stream.SetSelectCharacterPhase()
	
	def OnCreateFailure(self, error_type):
		"""Handle character creation failure"""
		if error_type == 1:
			self._popup_message(localeInfo.CREATE_EXIST_SAME_NAME, self.enable_window)
		else:
			self._popup_message(localeInfo.CREATE_FAILURE, self.enable_window)
	
	def OnUpdate(self):
		"""Update character rendering and handle creation timing"""
		chr.Update()
		
		for page in xrange(CharacterConstants.PAGE_COUNT):
			for slot in xrange(CharacterConstants.SLOT_COUNT):
				chr.SelectInstance(self._get_slot_chr_id(page, slot))
				chr.Show()
		
		self._handle_creation_timing()
	
	def _handle_creation_timing(self):
		"""Handle character creation timing and network communication"""
		if self.state.reserving_race_index == -1:
			return
		
		elapsed_time = app.GetTime() - self.state.reserving_start_time
		if elapsed_time >= CharacterConstants.CREATION_DELAY:
			self._send_character_creation_packet()
	
	def _send_character_creation_packet(self):
		"""Send character creation packet to server"""
		character_slot = self.stream.GetCharacterSlot()
		character_name = self.ui_manager.get_element('edit_name').GetText()
		race_index = self.state.reserving_race_index
		shape_index = self.state.reserving_shape_index
		
		net.SendCreateCharacterPacket(
			character_slot, character_name, race_index, shape_index, 0, 0, 0, 0
		)
		
		self.state.reserving_race_index = -1
	
	def _popup_message(self, message, callback=None):
		"""Show popup message"""
		if callback is None:
			callback = self._empty_func
		
		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(message, callback, localeInfo.UI_OK)
	
	def _empty_func(self):
		"""Empty function for default callbacks"""
		pass
	
	def OnPressExitKey(self):
		"""Handle exit key press"""
		self.cancel_create()
		return True