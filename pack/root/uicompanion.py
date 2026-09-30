import app
import dbg
import ui
import chrmgr
import constInfo
import uiToolTip 
import wndMgr 
import net 
import item
import player
import localeInfo 
import chat
import uiScriptLocale
import skill
import mouseModule

# Constants
AFFECT_DICT = uiToolTip.ItemToolTip.AFFECT_DICT
PET_MAX_SKILL_LV_BY_POINT = 10
PET_SKILL_START = 301
PET_SKILL_END = 312
PET_SKILL_COUNT = PET_SKILL_END - PET_SKILL_START
COLOR_GREEN = 0xff89b88d

# Mount bonus information
MOUNT_BONUS_INFO = (
	#(localeInfo.MOUNT_TOOLTIP_MAX_HP, (0, 250, 500, 750, 1000, 1000, 1250, 1500, 1750, 2000, 2000, 2250, 2500, 2750, 3000, 3000, 3250, 3500, 3750, 4000, 4250, 4500, 4750, 5000, 5000, 5250, 5500, 5750, 6000, 6000, 6250, 6500, 6750, 7000, 7000, 7250, 7500, 7750, 8000, 8000, 8250, 8500, 8750, 9000, 9000, 9250, 9500, 9750, 10000, 10000, 10250, 10500, 10750, 11000, 11000, 11250, 11500, 11750, 12000, 12000, 12250, 12500, 12750, 13000, 13000, 13250, 13500, 13750, 14000, 14000, 14250, 14500, 14750, 15000, 15000, 15250, 15500, 15750, 16000, 16000, 16250, 16500, 16750, 17000, 17000, 17250, 17500, 17750, 18000, 18000, 18250, 18500, 18750, 19000, 19000, 19250, 19500, 19750, 20000, 20000, 20000, 20250, 20500, 20750, 21000, 21000, 21250, 21500, 21750, 22000, 22000, 22250, 22500, 22750, 23000, 23000, 23250, 23500, 23750, 24000, 24000, 24250, 24500, 24750, 25000, 25000, 25250, 25500, 25750, 26000, 26000, 26250, 26500, 26750, 27000, 27000, 27250, 27500, 27750, 28000, 28000, 28250, 28500, 28750, 29000, 29000, 29250, 29500, 29750, 30000, 30000, 30000)),
	(localeInfo.MOUNT_TOOLTIP_APPLY_ATTBONUS_MONSTER, (0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 30, 30)),
	(localeInfo.MOUNT_TOOLTIP_APPLY_ATTBONUS_STONE, (0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10)),
	(localeInfo.MOUNT_TOOLTIP_APPLY_ATTBONUS_BOSS, (0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10)),
	(localeInfo.MOUNT_TOOLTIP_MOUNT_SPEED, (2, 2, 4, 4, 6, 6, 8, 8, 10, 10, 12, 12, 14, 14, 16, 16, 18, 18, 20, 20, 22, 22, 24, 24, 26, 26, 28, 28, 30, 30, 32, 32, 34, 34, 36, 36, 38, 38, 40, 40, 42, 42, 44, 44, 46, 46, 48, 48, 50, 50, 52, 52, 54, 54, 56, 56, 58, 58, 60, 60, 62, 62, 64, 64, 66, 66, 68, 68, 70, 70, 72, 72, 74, 74, 76, 76, 78, 78, 80, 80, 82, 82, 84, 84, 86, 86, 88, 88, 90, 90, 92, 92, 94, 94, 96, 96, 98, 98, 100, 100, 102, 102, 104, 104, 106, 106, 108, 108, 110, 110, 112, 112, 114, 114, 116, 116, 118, 118, 120, 120, 122, 122, 124, 124, 126, 126, 128, 128, 130, 130, 132, 132, 134, 134, 136, 136, 138, 138, 140, 140, 142, 142, 144, 144, 146, 146, 148, 148, 150, 150, 150)),
)


def GetPetSkillName(skill_index):
	skill_id = skill_index + PET_SKILL_START
	try:
		name = skill.GetSkillName(skill_id)
		if name and name.strip() != "":
			return name
	except:
		pass
	return "noname"


class TextToolTip(ui.Window):
	def __init__(self, y_offset=0):
		super(TextToolTip, self).__init__("TOP_MOST")
		self.y_offset = y_offset
		self.text_line = ui.TextLine()
		self.text_line.SetParent(self)
		self.text_line.SetHorizontalAlignLeft()
		self.text_line.SetOutline()
		self.text_line.Hide()

	def __del__(self):
		super(TextToolTip, self).__del__()

	def SetText(self, text):
		self.text_line.SetText(text)

	def OnRender(self):
		mouse_x, mouse_y = wndMgr.GetMousePosition()
		self.text_line.SetPosition(mouse_x - 30, mouse_y - 30 + self.y_offset)
		self.text_line.Show()


class CompanionWindow(ui.ScriptWindow):
	MODE_PET = 0
	MODE_MOUNT = 1
	
	def __init__(self, wnd_inventory=None):
		super(CompanionWindow, self).__init__()
		self.wnd_inventory = wnd_inventory
		self.current_mode = self.MODE_PET
		self.active_slot = -1
		self.tooltipItem = None
		self.tool_tip = uiToolTip.ToolTip()
		self.tool_tip.Hide()
		self.tool_tip_exp = None
		self.tool_tip_mount_exp = None
		self.levelToolTip = uiToolTip.ToolTip(230)
		self.tooltip_manager = {}
		self.mountLevel = 0
		
		self.title_bar = None
		self.pet_button = None
		self.mount_button = None
		self.board_01 = None
		self.board_02 = None
		
		self.pet_components = {}
		self.mount_components = {}
		
		self._initialize_pet_data()
		self._load_ui()

	def __del__(self):
		super(CompanionWindow, self).__del__()

	def _initialize_pet_data(self):
		try:
			if not hasattr(constInfo, 'gui_pets') or not constInfo.gui_pets:
				constInfo.gui_pets = {
					"NAME": "", "RACE": 0, "LEVEL": 1, "EXP": 0, "EXP_NEED": 1,
					"POINTS": 0, "SKILLS": []
				}
			if len(constInfo.gui_pets.get("SKILLS", [])) < PET_SKILL_COUNT:
				constInfo.gui_pets["SKILLS"] = [{"lv": 0, "affect": []} for _ in xrange(PET_SKILL_COUNT)]
		except Exception as e:
			dbg.TraceError("[Companion] Error initializing: {}".format(str(e)))

	def _is_pet_data_valid(self):
		try:
			if not hasattr(constInfo, 'gui_pets') or not constInfo.gui_pets:
				return False
			for key in ["SKILLS", "POINTS", "LEVEL", "EXP", "EXP_NEED"]:
				if key not in constInfo.gui_pets:
					return False
			skills = constInfo.gui_pets.get("SKILLS", [])
			return isinstance(skills, list) and len(skills) >= PET_SKILL_COUNT
		except:
			return False

	def Destroy(self):
		self.ClearDictionary()

	def _load_ui(self):
		try:
			ui.PythonScriptLoader().LoadScriptFile(self, "uiscript/companionwindow.py")
		except KeyError:
			dbg.TraceError("[Companion] Error loading UI")
			return
		self.SetCenterPosition()
		self._bind_ui_objects()

	def _bind_ui_objects(self):
		try:
			self.title_bar = self.GetChild("TitleBar")
			self.title_bar.SetCloseEvent(ui.__mem_func__(self.Close))
			
			self.pet_button = self.GetChild("petButton")
			self.mount_button = self.GetChild("mountButton")
			self.pet_button.SetEvent(ui.__mem_func__(self.SwitchToPetMode))
			self.mount_button.SetEvent(ui.__mem_func__(self.SwitchToMountMode))
			
			self.board_01 = self.GetChild("board_01")
			self.board_02 = self.GetChild("board_02")
			
			self._bind_pet_components()
			self._bind_mount_components()
			self.SwitchToPetMode()
		except KeyError as e:
			dbg.TraceError("[Companion] Bind error: {}".format(str(e)))

	def _bind_pet_components(self):
		try:
			self.pet_components['extra_bonus'] = self.GetChild("ExtraBonus")
			self._setup_bonus_tooltip()
			
			self.pet_components['skill_points'] = self.GetChild("skill_points")
			self.pet_components['pet_level'] = self.GetChild("pet_level")
			self.pet_components['pet_name'] = self.GetChild("pet_name")
			self.pet_components['petFeedItem'] = self.GetChild("pet_feed_item")
			self.pet_components['exp_hover_info'] = self.GetChild("exp_hover_info")
			
			gauge_names = ["exp_gauge_01", "exp_gauge_02", "exp_gauge_03", "exp_gauge_04"]
			self.pet_components['exp_gauges'] = [self.GetChild(n) for n in gauge_names]
			for g in self.pet_components['exp_gauges']:
				g.SetSize(0, 0)
				g.Hide()
			
			self.pet_components['islot'] = self.GetChild("islot")
			slot = self.pet_components['islot']
			slot.SetOverInItemEvent(ui.__mem_func__(self.OnPetSlotOverIn))
			slot.SetOverOutItemEvent(ui.__mem_func__(self.wnd_inventory.OverOutItem))
			slot.SetUnselectItemSlotEvent(ui.__mem_func__(self.OnPetSlotClick))
			slot.SetUseSlotEvent(ui.__mem_func__(self.OnPetSlotClick))
			slot.SetSelectEmptySlotEvent(ui.__mem_func__(self.OnPetSlotClick))
			slot.SetSelectItemSlotEvent(ui.__mem_func__(self.OnPetSlotClick))
			
			self.pet_components['skills'] = self.GetChild("skills")
			skills = self.pet_components['skills']
			skills.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			skills.SetOverInItemEvent(ui.__mem_func__(self.OnSkillOver))
			skills.SetOverOutItemEvent(ui.__mem_func__(self.OnSkillOut))
			skills.SetPressedSlotButtonEvent(ui.__mem_func__(self.OnSkillUpgrade))
			skills.SetSelectItemSlotEvent(ui.__mem_func__(self.OnSkillClick))
			skills.SetUnselectItemSlotEvent(ui.__mem_func__(self.OnSkillClick))
			skills.AppendSlotButton(
				"d:/ymir work/ui/game/windows/btn_plus_up.sub",
				"d:/ymir work/ui/game/windows/btn_plus_over.sub",
				"d:/ymir work/ui/game/windows/btn_plus_down.sub"
			)
			
			self.pet_components['eqs'] = self.GetChild("eqs")
			eqs = self.pet_components['eqs']
			eqs.SetOverInItemEvent(ui.__mem_func__(self.OnPetEquipOverIn))
			eqs.SetOverOutItemEvent(ui.__mem_func__(self.wnd_inventory.OverOutItem))
			eqs.SetUnselectItemSlotEvent(ui.__mem_func__(self.OnPetEquipClick))
			eqs.SetUseSlotEvent(ui.__mem_func__(self.OnPetEquipClick))
			eqs.SetSelectEmptySlotEvent(ui.__mem_func__(self.OnPetEquipClick))
			eqs.SetSelectItemSlotEvent(ui.__mem_func__(self.OnPetEquipClick))
			
			self.PetReload()
		except KeyError as e:
			dbg.TraceError("[Companion] Pet bind error: {}".format(str(e)))

	def _bind_mount_components(self):
		try:
			self.mount_components['equipSlots'] = self.GetChild("equip_slot")
			self.mount_components['costumeSlot'] = self.GetChild("costume_slot")
			self.mount_components['value_name'] = self.GetChild("value_name")
			self.mount_components['valueLevel'] = self.GetChild("value_level")
			self.mount_components['mountFeedItem'] = self.GetChild("mount_feed_item")
			#self.mount_components['mountExpText'] = self.GetChild("mount_exp_text")
			self.mount_components['mount_exp_hover_info'] = self.GetChild("mount_exp_hover_info")
			mount_gauge_names = ["mount_exp_gauge_01", "mount_exp_gauge_02", "mount_exp_gauge_03", "mount_exp_gauge_04"]
			self.mount_components['exp_gauges'] = [self.GetChild(n) for n in mount_gauge_names]
			for g in self.mount_components['exp_gauges']:
				g.SetSize(0, 0)
				g.Hide()
			self.mount_components['mountButton'] = self.GetChild("mount_button")
			self.mount_components['mountButtonUnsummon'] = self.GetChild("mount_button_unsummon")
			
			self.mount_components['bonusLabels'] = {
				'att_bonus_monster': self.GetChild("label_att_bonus_monster"),
				'att_bonus_metine': self.GetChild("label_att_bonus_metine"),
				'att_bonus_sefi': self.GetChild("label_att_bonus_sefi"),
				'mount_movement_speed': self.GetChild("label_mount_movement_speed")
			}
			
			for slot_comp in [self.mount_components['equipSlots'], self.mount_components['costumeSlot']]:
				slot_comp.SetOverInItemEvent(ui.__mem_func__(self.wnd_inventory.OverInItem))
				slot_comp.SetOverOutItemEvent(ui.__mem_func__(self.wnd_inventory.OverOutItem))
				slot_comp.SetUnselectItemSlotEvent(ui.__mem_func__(self.wnd_inventory.UseItemSlot))
				slot_comp.SetUseSlotEvent(ui.__mem_func__(self.wnd_inventory.UseItemSlot))
				slot_comp.SetSelectEmptySlotEvent(ui.__mem_func__(self.wnd_inventory.SelectEmptySlot))
				slot_comp.SetSelectItemSlotEvent(ui.__mem_func__(self.wnd_inventory.SelectItemSlot))
			
			self.mount_components['mountButtonUnsummon'].SetEvent(ui.__mem_func__(self.__UnsummonMount))
			self.mount_components['mountButton'].SetEvent(ui.__mem_func__(self.__SummonMount))
		except KeyError as e:
			dbg.TraceError("[Companion] Mount bind error: {}".format(str(e)))

	def _setup_bonus_tooltip(self):
		tooltip = uiToolTip.ToolTipNew(330)
		tooltip.SetDefaultFontName("Tahoma:14")
		tooltip.AppendSpace(3)
		tooltip.AppendTextLine(uiScriptLocale.PET_INFO_1)
		tooltip.AppendTextLine(uiScriptLocale.PET_INFO_2)
		self.tooltip_manager[0] = tooltip

	def SwitchToPetMode(self):
		self.current_mode = self.MODE_PET
		self.board_01.Show()
		self.board_02.Hide()
		self.pet_button.Down()
		self.mount_button.SetUp()
		self.PetReload()

	def SwitchToMountMode(self):
		self.current_mode = self.MODE_MOUNT
		self.board_01.Hide()
		self.board_02.Show()
		self.mount_button.Down()
		self.pet_button.SetUp()
		self.RefreshMountEquipmentSlots()

	def SetItemToolTip(self, itemTooltip):
		self.tooltipItem = itemTooltip

	def Open(self):
		if self.IsShow():
			self.Close()
		else:
			self.SetCenterPosition()
			self.Show()

	def Close(self):
		self._deactivate_all_skills()
		if self.tooltip_manager.get(0):
			self.tooltip_manager[0].Hide()
		if self.tool_tip_exp:
			self.tool_tip_exp.Hide()
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()
		self.Hide()
		self.OnSkillOut()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnPetSlotOverIn(self, slotIndex):
		if self.wnd_inventory:
			self.wnd_inventory.OverInItem(slotIndex)

	def OnPetSlotClick(self, slotIndex):
		if self.wnd_inventory:
			self.wnd_inventory.UseItemSlot(slotIndex)

	def OnPetEquipOverIn(self, slotIndex):
		if self.wnd_inventory:
			self.wnd_inventory.OverInItem(slotIndex)

	def OnPetEquipClick(self, slotIndex):
		if self.wnd_inventory:
			self.wnd_inventory.UseItemSlot(slotIndex)

	# Pet Methods
	def OnSkillClick(self, slot_number):
		try:
			if not self._is_pet_data_valid() or slot_number >= PET_SKILL_COUNT:
				return
			skill_level = constInfo.gui_pets["SKILLS"][slot_number].get("lv", 0)
			if not (19 < skill_level < 30):
				return
			skills = self.pet_components['skills']
			if skills.IsActivatedSlot(slot_number):
				skills.DeactivateSlot(slot_number)
				net.SendPetAction(0)
			else:
				self._deactivate_all_skills()
				skills.ActivateSlot(slot_number)
				net.SendPetAction(slot_number + PET_SKILL_START)
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_INFO_1.format(GetPetSkillName(slot_number)))
		except Exception as e:
			dbg.TraceError("[Companion] Skill click error: {}".format(str(e)))

	def _deactivate_all_skills(self):
		skills = self.pet_components.get('skills')
		if skills:
			for i in xrange(PET_SKILL_COUNT):
				skills.DeactivateSlot(i)
			net.SendPetAction(0)

	def OnSkillUpgrade(self, slot_number):
		net.SendChatPacket("/pet_skillup {}".format(slot_number + PET_SKILL_START))

	def PetReload(self):
		try:
			if not self._is_pet_data_valid():
				self._initialize_pet_data()
				return
			points = int(constInfo.gui_pets.get("POINTS", 0))
			skills = self.pet_components.get('skills')
			if not skills:
				return
			skills.HideAllSlotButton()
			for i in xrange(PET_SKILL_COUNT):
				self._update_skill_slot(i, points)
		except Exception as e:
			dbg.TraceError("[Companion] PetReload error: {}".format(str(e)))

	def _update_skill_slot(self, idx, pts):
		try:
			if not self._is_pet_data_valid():
				return
			lv = constInfo.gui_pets["SKILLS"][idx].get("lv", 0)
			sid = idx + PET_SKILL_START
			dlv, grd = self._calc_skill_display(lv)
			skills = self.pet_components['skills']
			if skills:
				skills.SetSkillSlotNew(idx, sid, grd, dlv)
				skills.SetSlotCountNew(idx, grd, dlv)
				if lv < PET_MAX_SKILL_LV_BY_POINT and pts > 0:
					skills.ShowSlotButton(idx)
		except Exception as e:
			dbg.TraceError("[Companion] Slot update error: {}".format(str(e)))

	def _calc_skill_display(self, lv):
		if 10 <= lv <= 19:
			return lv - 9, 1
		elif 20 <= lv <= 29:
			return lv - 19, 2
		elif lv > 29:
			return lv - 29, 3
		return lv, 0

	#if app.ENABLE_EXTRABONUS_SYSTEM:
	#def _are_all_skills_maxed(self):
	#	for i in xrange(PET_SKILL_COUNT):
	#		if constInfo.gui_pets["SKILLS"][i]["lv"] != 30:
	#			return False
	#	return True
			
	def OnUpdate(self):
		if self.current_mode == self.MODE_PET:
			self._update_tooltips()
			self._update_pet_info()
			self._update_item_slots()
			self._auto_deactivate_maxed_skills()
		if self.current_mode == self.MODE_MOUNT:
			self.RefreshMountEquipmentSlots()
			if self.mount_components.get('mount_exp_hover_info') and self.mount_components['mount_exp_hover_info'].IsIn():
				if self.tool_tip_mount_exp:
					self.tool_tip_mount_exp.Show()
			else:
				if self.tool_tip_mount_exp:
					self.tool_tip_mount_exp.Hide()
			
	def _update_tooltips(self):
		if self.pet_components.get('extra_bonus') and self.pet_components['extra_bonus'].IsIn():
			if self.tooltip_manager.get(0):
				self.tooltip_manager[0].Show()
		else:
			if self.tooltip_manager.get(0):
				self.tooltip_manager[0].Hide()
	
		if self.tool_tip.IsShow():
			skills = self.pet_components.get('skills')
			if skills:
				wndMgr.RefreshSlot(skills.GetWindowHandle())
	
		if self.pet_components.get('exp_hover_info') and self.pet_components['exp_hover_info'].IsIn():
			if self.tool_tip_exp:
				self.tool_tip_exp.Show()
		else:
			if self.tool_tip_exp:
				self.tool_tip_exp.Hide()
	
	def _update_pet_info(self):
		try:
			if not self._is_pet_data_valid():
				return
			
			pet_data = constInfo.gui_pets
			
			points = int(pet_data.get("POINTS", 0))
			if self.pet_components.get('skill_points'):
				self.pet_components['skill_points'].SetText(str(points))
				if points > 0:
					self.pet_components['skill_points'].Show()
	
			level = int(pet_data.get("LEVEL", 1))
			if self.pet_components.get('pet_level'):
				self.pet_components['pet_level'].SetText("Lv. " + str(level))
			
			if level < 40:
				self.pet_components['petFeedItem'].SetText(localeInfo.COMPANION_PET_EXP_WITH_CHAR)
			elif level < 70:
				self.pet_components['petFeedItem'].SetText("|Eemoji/feed_pet_1|e" + localeInfo.COMPANION_PET_FEED_ITEM_1)
			elif level <= 110:
				self.pet_components['petFeedItem'].SetText("|Eemoji/feed_pet_2|e" + localeInfo.COMPANION_PET_FEED_ITEM_2)
			else:
				self.pet_components['petFeedItem'].SetText("|Eemoji/feed_pet_3|e" + localeInfo.COMPANION_PET_FEED_ITEM_3)

			current_exp = int(pet_data.get("EXP", 0))
			max_exp = int(pet_data.get("EXP_NEED", 1))
			self.SetExp(current_exp, max_exp)
			
		except (KeyError, TypeError, ValueError) as e:
			dbg.TraceError("[Companion] Error updating pet info: {}".format(str(e)))
	
	def _update_item_slots(self):
		try:
			pet_slot = item.SLOT_ITEM_NEW_PET
			pet_item_id = player.GetItemIndex(pet_slot)
			if self.pet_components.get('islot'):
				self.pet_components['islot'].SetItemSlot(pet_slot, pet_item_id, 0)
			
			if self.pet_components.get('eqs'):
				equipment_slots = [
					item.EQUIPMENT_NEW_PET_EQ0,
					item.EQUIPMENT_NEW_PET_EQ1,
					item.EQUIPMENT_NEW_PET_EQ2,
					item.EQUIPMENT_NEW_PET_EQ3,
					item.EQUIPMENT_NEW_PET_EQ4
				]
				
				for slot_number in equipment_slots:
					item_index = player.GetItemIndex(slot_number)
					self.pet_components['eqs'].SetItemSlot(slot_number, item_index, 0)
				
				self.pet_components['eqs'].RefreshSlot()
			
		except Exception as e:
			dbg.TraceError("[Companion] Error updating item slots: {}".format(str(e)))
	
	def _auto_deactivate_maxed_skills(self):
		if not self._is_pet_data_valid():
			return
			
		skills = self.pet_components.get('skills')
		if not skills:
			return
			
		for skill_index in xrange(PET_SKILL_COUNT):
			skill_level = constInfo.gui_pets["SKILLS"][skill_index]["lv"]
			if skill_level > 29 and skills.IsActivatedSlot(skill_index):
				skills.DeactivateSlot(skill_index)
				net.SendPetAction(0)

	def OnSkillOver(self, slot_number):
		self.active_slot = slot_number
		self.tool_tip.ClearToolTip()
		self.tool_tip.SetTitle(GetPetSkillName(slot_number))
		try:
			for aff in constInfo.gui_pets["SKILLS"][slot_number]["affect"]:
				if not aff:
					continue
				parts = aff.split(",")
				if len(parts) >= 2:
					self.tool_tip.AppendSpace(2)
					aid, aval = int(parts[0]), int(parts[1])
					self.tool_tip.AppendTextLine(str(AFFECT_DICT[aid](aval)), COLOR_GREEN)
		except:
			pass
		self.tool_tip.Show()

	def OnSkillOut(self):
		if self.tool_tip:
			self.tool_tip.Hide()

	def SetExp(self, cur, mx):
		cur, mx = max(int(cur), 0), max(int(mx), 0)
		pct = float(cur) / mx * 100 if mx > 0 else 0.0
		
		for g in self.pet_components['exp_gauges']:
			g.Hide()
		if mx > 0:
			qp = mx / 4
			if qp > 0:
				fc = min(4, cur / qp)
				for i in xrange(fc):
					self.pet_components['exp_gauges'][i].SetRenderingRect(0.0, 0.0, 0.0, 0.0)
					self.pet_components['exp_gauges'][i].Show()
				if fc < 4:
					self.pet_components['exp_gauges'][fc].SetRenderingRect(0.0, float(cur % qp) / qp - 1.0, 0.0, 0.0)
					self.pet_components['exp_gauges'][fc].Show()
		
		txt = uiScriptLocale.HOVER_EXP_INFO % (
			localeInfo.MoneyFormat(str(cur)),
			localeInfo.MoneyFormat(str(mx)),
			pct
		)
		if not self.tool_tip_exp:
			self.tool_tip_exp = TextToolTip(15)
		self.tool_tip_exp.SetText(txt)

	# Mount Methods
	def SetBasicInfo(self, vnum, name, level, exp, req_exp, summoned):
		self.mount_components['value_name'].SetOutline()
		self.mount_components['value_name'].SetText(name)
		
		self.mount_components['valueLevel'].SetOutline()
		self.mount_components['valueLevel'].SetText(str(level))
		self.mountLevel = level
		chrmgr.SetMountSpeedBonus(MOUNT_BONUS_INFO[3][1][max(0, level - 1)])

		self.UpdateBonusLabels(level)
		if level <= 24:
			self.mount_components['mountFeedItem'].SetText("|Eemoji/feed_1|e" + localeInfo.MOUNT_WINDOW_FEED_ITEM_1)
		elif level >= 25 and level <= 49:
			self.mount_components['mountFeedItem'].SetText("|Eemoji/feed_2|e" + localeInfo.MOUNT_WINDOW_FEED_ITEM_2)
		elif level >= 50 and level <= 74:
			self.mount_components['mountFeedItem'].SetText("|Eemoji/feed_3|e" + localeInfo.MOUNT_WINDOW_FEED_ITEM_3)
		else:
			self.mount_components['mountFeedItem'].SetText("|Eemoji/feed_4|e" + localeInfo.MOUNT_WINDOW_FEED_ITEM_4)
			
		self.SetMountExp(exp, req_exp)

		if summoned == True:
			self.mount_components['mountButton'].Hide()
			self.mount_components['mountButtonUnsummon'].Show()
		else:
			self.mount_components['mountButton'].Show()
			self.mount_components['mountButtonUnsummon'].Hide()
			
	def SetMountExp(self, cur, mx):
		cur, mx = max(int(cur), 0), max(int(mx), 0)
		pct = float(cur) / mx * 100 if mx > 0 else 0.0

		for g in self.mount_components['exp_gauges']:
			g.Hide()
		if mx > 0:
			qp = mx / 4
			if qp > 0:
				fc = min(4, cur / qp)
				for i in xrange(fc):
					self.mount_components['exp_gauges'][i].SetRenderingRect(0.0, 0.0, 0.0, 0.0)
					self.mount_components['exp_gauges'][i].Show()
				if fc < 4:
					self.mount_components['exp_gauges'][fc].SetRenderingRect(0.0, float(cur % qp) / qp - 1.0, 0.0, 0.0)
					self.mount_components['exp_gauges'][fc].Show()

		#self.mount_components['mountExpText'].SetText("{} / {}".format(localeInfo.MoneyFormat(str(cur)), localeInfo.MoneyFormat(str(mx))))

		txt = uiScriptLocale.HOVER_EXP_INFO % (
			localeInfo.MoneyFormat(str(cur)),
			localeInfo.MoneyFormat(str(mx)),
			pct
		)
		self.tool_tip_mount_exp = TextToolTip(15)
		self.tool_tip_mount_exp.SetText(txt)
		self.tool_tip_mount_exp.Hide()

	def UpdateBonusLabels(self, level):
		att_bonus_monster = MOUNT_BONUS_INFO[0][1][level-1]
		att_bonus_metine = MOUNT_BONUS_INFO[1][1][level-1]
		att_bonus_sefi = MOUNT_BONUS_INFO[2][1][level-1]
		mount_movement_speed = MOUNT_BONUS_INFO[3][1][level-1]
		
		self.mount_components['bonusLabels']['att_bonus_monster'].SetText("|cff95876e|H|h{}|H|h: |cffc2bba5|H|h+{}%|H|h".format(localeInfo.MOUNT_TOOLTIP_APPLY_ATTBONUS_MONSTER, att_bonus_monster))
		self.mount_components['bonusLabels']['att_bonus_metine'].SetText("|cff95876e|H|h{}|H|h: |cffc2bba5|H|h+{}%|H|h".format(localeInfo.MOUNT_TOOLTIP_APPLY_ATTBONUS_STONE, att_bonus_metine))
		self.mount_components['bonusLabels']['att_bonus_sefi'].SetText("|cff95876e|H|h{}|H|h: |cffc2bba5|H|h+{}%|H|h".format(localeInfo.MOUNT_TOOLTIP_APPLY_ATTBONUS_BOSS, att_bonus_sefi))
		self.mount_components['bonusLabels']['mount_movement_speed'].SetText("|cff95876e|H|h{}|H|h: |cffc2bba5|H|h+{}%|H|h".format(localeInfo.MOUNT_TOOLTIP_MOUNT_SPEED, mount_movement_speed))
			
	def __UnsummonMount(self):
		net.SendChatPacket("/horse_unsummon")
		
	def __SummonMount(self):
		net.SendChatPacket("/horse_summon")
	
	def RefreshMountEquipmentSlots(self):
		getItemVNum=player.GetItemIndex
		for i in xrange(item.MOUNT_SLOT_COUNT):
			self.mount_components['equipSlots'].SetItemSlot(item.MOUNT_SLOT_START + i, getItemVNum(item.MOUNT_SLOT_START + i), 0)
		self.mount_components['equipSlots'].RefreshSlot()
		
		self.mount_components['costumeSlot'].SetItemSlot(item.COSTUME_SLOT_MOUNT, getItemVNum(item.COSTUME_SLOT_MOUNT), 0)
		self.mount_components['costumeSlot'].RefreshSlot()
				
	def __OnUnselectItemSlot(self, slotIndex):
		net.SendItemUsePacket(slotIndex)
		
	def __OnSelectEmptySlot(self, slotIndex):
		pass
			
	def __OnSelectItemSlot(self, slotIndex):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return
	
		if mouseModule.mouseController.isAttached():
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedItemCount = mouseModule.mouseController.GetAttachedItemCount()
	
			if attachedSlotPos != slotIndex and player.SLOT_TYPE_INVENTORY == attachedSlotType:
				net.SendItemMovePacket(attachedSlotPos, slotIndex, attachedItemCount)
	
			mouseModule.mouseController.DeattachObject()
		else:
			selectedItemVNum = player.GetItemIndex(player.INVENTORY, slotIndex)
			itemCount = player.GetItemCount(player.INVENTORY, slotIndex)
			mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, slotIndex, selectedItemVNum, itemCount)
	
	def __OnOverInItem(self, slotIndex):
		if self.tooltipItem:
			self.tooltipItem.SetInventoryItem(slotIndex)
	
	def __OnOverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OnPressEscapeKey(self):
		self.Close()
		return True
	
	def Show(self):
		super(CompanionWindow, self).Show()
	
	def Close(self):
		self._deactivate_all_skills()
	
		if self.tooltip_manager.get(0):
			self.tooltip_manager[0].Hide()
	
		if self.tool_tip_exp:
			self.tool_tip_exp.Hide()

		if self.tool_tip_mount_exp:
			self.tool_tip_mount_exp.Hide()

		self.Hide()
		self.OnSkillOut()