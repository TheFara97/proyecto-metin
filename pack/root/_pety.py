# -*- coding: utf-8 -*-

import app
import dbg
import ui
import constInfo
import event
import uiToolTip 
import exception 
import wndMgr 
import net 
import item 
import player 
import localeInfo 
import chat
import uiScriptLocale
import pack
import skill
from uiToolTip import ItemToolTip

# Constants
AFFECT_DICT = ItemToolTip.AFFECT_DICT
PET_MAX_SKILL_LV_BY_POINT = 10
PET_SKILL_START = 301
PET_SKILL_END = 312
PET_SKILL_COUNT = PET_SKILL_END - PET_SKILL_START

# Color constants
COLOR_GREEN = 0xff89b88d


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
    """Custom tooltip for text display"""
    
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
        """Set tooltip text"""
        self.text_line.SetText(text)

    def OnRender(self):
        """Render tooltip at mouse position"""
        mouse_x, mouse_y = wndMgr.GetMousePosition()
        self.text_line.SetPosition(mouse_x - 30, mouse_y - 30 + self.y_offset)
        self.text_line.Show()


class PetWindow(ui.ScriptWindow):
    """Pet management window"""
    
    def __init__(self, wnd_inventory=None):
        super(PetWindow, self).__init__()
        self.wnd_inventory = wnd_inventory
        self.active_slot = -1
        
        # Initialize components
        self._initialize_components()
        self._load_ui()

    def __del__(self):
        super(PetWindow, self).__del__()

    def _initialize_components(self):
        """Initialize all window components"""
        self.tool_tip = uiToolTip.ToolTip()
        self.tool_tip.Hide()
        self.tool_tip_exp = None
        
        # GUI component references
        self.gui_components = {
            'points': None,
            'name': None,
            'petname_t': None,
            'lv': None,
            'exp': None,
            'exp_need': None,
            'skills': None,
            'islot': None,
            'eqs': None,
        }
        
        # UI elements
        self.title_bar = None
        self.extra_bonus = None
        self.exp_hover_info = None
        self.exp_gauges = []
        self.tooltip_manager = {}
        
        # Initialize pet data if not exists
        self._initialize_pet_data()

    def _initialize_pet_data(self):
        """Initialize pet data structure if not exists"""
        try:
            if not hasattr(constInfo, 'gui_pets') or not constInfo.gui_pets:
                constInfo.gui_pets = {
                    "NAME": "",
                    "RACE": 0,
                    "LEVEL": 1,
                    "EXP": 0,
                    "EXP_NEED": 1,
                    "POINTS": 0,
                    "SKILLS": []
                }
            
            # Ensure SKILLS list has enough entries
            if len(constInfo.gui_pets.get("SKILLS", [])) < PET_SKILL_COUNT:
                constInfo.gui_pets["SKILLS"] = []
                for i in xrange(PET_SKILL_COUNT):
                    constInfo.gui_pets["SKILLS"].append({
                        "lv": 0,
                        "affect": []
                    })
                    
        except Exception as e:
            dbg.TraceError("[Pet] Error initializing pet data: {}".format(str(e)))

    def _is_pet_data_valid(self):
        """Check if pet data is properly initialized"""
        try:
            if not hasattr(constInfo, 'gui_pets') or not constInfo.gui_pets:
                return False
                
            required_keys = ["SKILLS", "POINTS", "LEVEL", "EXP", "EXP_NEED"]
            for key in required_keys:
                if key not in constInfo.gui_pets:
                    return False
                    
            skills = constInfo.gui_pets.get("SKILLS", [])
            if not isinstance(skills, list) or len(skills) < PET_SKILL_COUNT:
                return False
                
            return True
            
        except Exception:
            return False

    def Destroy(self):
        """Clean up resources"""
        self.ClearDictionary()
        self._initialize_components()

    def _load_ui(self):
        """Load UI script and bind objects"""
        try:
            ui.PythonScriptLoader().LoadScriptFile(self, "uiscript/_pety_script.py")
        except KeyError:
            dbg.TraceError("[Pet] Error loading _pety_script.py - file missing or corrupted")
            return
            
        self.SetCenterPosition()
        self._bind_ui_objects()

    def _bind_ui_objects(self):
        """Bind UI objects and set up event handlers"""
        try:
            # Title bar
            self.title_bar = self.GetChild("TitleBar")
            self.title_bar.SetCloseEvent(ui.__mem_func__(self.Close))

            # Extra bonus tooltip
            self.extra_bonus = self.GetChild("ExtraBonus")
            self._setup_bonus_tooltip()

            # Item slots
            self._setup_item_slots()
            
            # Equipment slots
            self._setup_equipment_slots()

            # Skill points and level
            self.gui_components['points'] = self.GetChild("skill_points")
            self.gui_components['lv'] = self.GetChild("pet_level")
            
            # Experience info
            self.exp_hover_info = self.GetChild("exp_hover_info")
            self._setup_exp_gauges()

            # Skills
            self._setup_skills()
            
            self.PetReload()

        except KeyError as e:
            dbg.TraceError("[Pet] Failed to bind UI objects: {}".format(str(e)))

    def _setup_bonus_tooltip(self):
        """Setup extra bonus tooltip"""
        tooltip = uiToolTip.ToolTipNew(330)
        tooltip.SetDefaultFontName("Tahoma:14")
        tooltip.AppendSpace(3)
        tooltip.AppendTextLine(uiScriptLocale.PET_INFO_1)
        tooltip.AppendTextLine(uiScriptLocale.PET_INFO_2)
        self.tooltip_manager[0] = tooltip

    def _setup_item_slots(self):
        """Setup main item slot"""
        self.gui_components['islot'] = self.GetChild("islot")
        slot = self.gui_components['islot']
        
        # Bind inventory events
        slot.SetOverInItemEvent(ui.__mem_func__(self.wnd_inventory.OverInItem))
        slot.SetOverOutItemEvent(ui.__mem_func__(self.wnd_inventory.OverOutItem))
        slot.SetUnselectItemSlotEvent(ui.__mem_func__(self.wnd_inventory.UseItemSlot))
        slot.SetUseSlotEvent(ui.__mem_func__(self.wnd_inventory.UseItemSlot))
        slot.SetSelectEmptySlotEvent(ui.__mem_func__(self.wnd_inventory.SelectEmptySlot))
        slot.SetSelectItemSlotEvent(ui.__mem_func__(self.wnd_inventory.SelectItemSlot))

    def _setup_equipment_slots(self):
        """Setup equipment slots"""
        self.gui_components['eqs'] = self.GetChild("eqs")
        slot = self.gui_components['eqs']
        
        # Bind inventory events
        slot.SetOverInItemEvent(ui.__mem_func__(self.wnd_inventory.OverInItem))
        slot.SetOverOutItemEvent(ui.__mem_func__(self.wnd_inventory.OverOutItem))
        slot.SetUnselectItemSlotEvent(ui.__mem_func__(self.wnd_inventory.UseItemSlot))
        slot.SetUseSlotEvent(ui.__mem_func__(self.wnd_inventory.UseItemSlot))
        slot.SetSelectEmptySlotEvent(ui.__mem_func__(self.wnd_inventory.SelectEmptySlot))
        slot.SetSelectItemSlotEvent(ui.__mem_func__(self.wnd_inventory.SelectItemSlot))

    def _setup_exp_gauges(self):
        """Setup experience gauge elements"""
        gauge_names = ["exp_gauge_01", "exp_gauge_02", "exp_gauge_03", "exp_gauge_04"]
        self.exp_gauges = [self.GetChild(name) for name in gauge_names]
        
        for gauge in self.exp_gauges:
            gauge.SetSize(0, 0)
            gauge.Hide()

    def _setup_skills(self):
        """Setup skill slots and events"""
        self.gui_components['skills'] = self.GetChild("skills")
        skills = self.gui_components['skills']
        
        skills.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
        skills.SetOverInItemEvent(ui.__mem_func__(self.OnSkillOver))
        skills.SetOverOutItemEvent(ui.__mem_func__(self.OnSkillOut))
        skills.SetPressedSlotButtonEvent(ui.__mem_func__(self.OnSkillUpgrade))
        skills.SetSelectItemSlotEvent(ui.__mem_func__(self.OnSkillClick))
        skills.SetUnselectItemSlotEvent(ui.__mem_func__(self.OnSkillClick))
        
        # Add plus buttons
        skills.AppendSlotButton(
            "d:/ymir work/ui/game/windows/btn_plus_up.sub",
            "d:/ymir work/ui/game/windows/btn_plus_over.sub",
            "d:/ymir work/ui/game/windows/btn_plus_down.sub"
        )

    def OnSkillClick(self, slot_number):
        """Handle skill slot click for activation/deactivation"""
        try:
            if not self._is_pet_data_valid() or slot_number >= PET_SKILL_COUNT:
                return
                
            skill_data = constInfo.gui_pets["SKILLS"][slot_number]
            skill_level = skill_data.get("lv", 0)
            
            if not self._can_activate_skill(skill_level):
                return
                
            skills = self.gui_components.get('skills')
            if not skills:
                return
            
            if skills.IsActivatedSlot(slot_number):
                # Deactivate current skill
                skills.DeactivateSlot(slot_number)
                net.SendPetAction(0)
            else:
                # Deactivate all skills first
                self._deactivate_all_skills()
                
                # Activate selected skill
                skills.ActivateSlot(slot_number)
                net.SendPetAction(slot_number + PET_SKILL_START)
                
                # Użyj dynamicznej nazwy umiejętności
                skill_name = GetPetSkillName(slot_number)
                chat.AppendChat(chat.CHAT_TYPE_INFO, 
                              localeInfo.PET_INFO_1.format(skill_name))
                                  
        except (IndexError, KeyError, TypeError) as e:
            dbg.TraceError("[Pet] Error in skill click: {}".format(str(e)))

    def _can_activate_skill(self, skill_level):
        """Check if skill can be activated"""
        return 19 < skill_level < 30

    def _deactivate_all_skills(self):
        """Deactivate all active skills"""
        skills = self.gui_components['skills']
        for i in xrange(PET_SKILL_COUNT):
            skills.DeactivateSlot(i)
        net.SendPetAction(0)

    def OnSkillUpgrade(self, slot_number):
        """Handle skill upgrade button press"""
        skill_id = slot_number + PET_SKILL_START
        net.SendChatPacket("/pet_skillup {}".format(skill_id))

    def PetReload(self):
        """Reload pet skill information"""
        try:
            # Check if pet data is properly initialized
            if not self._is_pet_data_valid():
                self._initialize_pet_data()
                return
                
            points = int(constInfo.gui_pets.get("POINTS", 0))
            skills = self.gui_components.get('skills')
            
            if not skills:
                dbg.TraceError("[Pet] Skills component not initialized")
                return
                
            skills.HideAllSlotButton()

            for skill_index in xrange(PET_SKILL_COUNT):
                self._update_skill_slot(skill_index, points)
                
        except (KeyError, TypeError, ValueError) as e:
            dbg.TraceError("[Pet] Error in PetReload: {}".format(str(e)))
            self._initialize_pet_data()

    def _update_skill_slot(self, skill_index, available_points):
        """Update individual skill slot"""
        try:
            # Check if pet data exists and is properly initialized
            if not self._is_pet_data_valid():
                return
                
            skill_data = constInfo.gui_pets["SKILLS"][skill_index]
            skill_level = skill_data.get("lv", 0)
            skill_id = skill_index + PET_SKILL_START
            
            # Calculate display values
            display_level, grade = self._calculate_skill_display(skill_level)
            
            # Update slot appearance
            skills = self.gui_components['skills']
            if skills:
                skills.SetSkillSlotNew(skill_index, skill_id, grade, display_level)
                skills.SetSlotCountNew(skill_index, grade, display_level)

                # Show upgrade button if possible
                if self._can_upgrade_skill(skill_level, available_points):
                    skills.ShowSlotButton(skill_index)
                    
        except (IndexError, KeyError, TypeError) as e:
            dbg.TraceError("[Pet] Error updating skill slot {}: {}".format(skill_index, str(e)))

    def _calculate_skill_display(self, skill_level):
        """Calculate display level and grade for skill"""
        if 10 <= skill_level <= 19:
            return skill_level - 9, 1
        elif 20 <= skill_level <= 29:
            return skill_level - 19, 2
        elif skill_level > 29:
            return skill_level - 29, 3
        else:
            return skill_level, 0

    def _can_upgrade_skill(self, skill_level, available_points):
        """Check if skill can be upgraded"""
        return skill_level < PET_MAX_SKILL_LV_BY_POINT and available_points > 0

    if app.ENABLE_EXTRABONUS_SYSTEM:
        def _are_all_skills_maxed(self):
            """Check if all skills are at maximum level"""
            for skill_index in xrange(PET_SKILL_COUNT):
                skill_level = constInfo.gui_pets["SKILLS"][skill_index]["lv"]
                if skill_level != 30:
                    return False
            return True

    def OnSkillOver(self, slot_number):
        """Handle mouse over skill slot"""
        self.active_slot = slot_number
        self._show_skill_tooltip(slot_number)

    def _show_skill_tooltip(self, slot_number):
        """Display skill tooltip z dynamiczną nazwą"""
        self.tool_tip.ClearToolTip()
        
        # Użyj dynamicznej nazwy umiejętności
        skill_name = GetPetSkillName(slot_number)
        self.tool_tip.SetTitle(skill_name)
        
        try:
            skill_data = constInfo.gui_pets["SKILLS"][slot_number]
            affect_list = skill_data["affect"]
            
            for affect_string in affect_list:
                if not affect_string:
                    continue
                    
                self._add_affect_to_tooltip(affect_string)
                
        except (KeyError, IndexError):
            pass
            
        self.tool_tip.Show()

    def _add_affect_to_tooltip(self, affect_string):
        """Add affect information to tooltip"""
        affect_data = affect_string.split(",")
        if len(affect_data) < 2:
            return
            
        self.tool_tip.AppendSpace(2)
        affect_id = int(affect_data[0])
        affect_value = int(affect_data[1])
        
        # Get affect description
        affect_text = str(AFFECT_DICT[affect_id](affect_value))
        self.tool_tip.AppendTextLine(affect_text, COLOR_GREEN)
        
        # Add extra bonus info if applicable
        if app.ENABLE_EXTRABONUS_SYSTEM and self._are_all_skills_maxed():
            self._add_extra_bonus_info(affect_value)

    def _add_extra_bonus_info(self, affect_value):
        """Add extra bonus information to tooltip"""
        self.tool_tip.AppendSpace(5)
        bonus_percent = (affect_value * 30) / 100
        bonus_text = localeInfo.PET_INFO_4.format(bonus_percent)
        self.tool_tip.AppendTextLine(bonus_text)
        self.tool_tip.AppendSpace(2)

    def SetExp(self, current_exp, max_exp):
        """Update experience display"""
        current_exp = int(max(current_exp, 0))
        max_exp = int(max(max_exp, 0))
        
        if max_exp > 0:
            exp_percentage = float(current_exp) / max_exp * 100
        else:
            exp_percentage = 0.0

        self._update_exp_gauges(current_exp, max_exp)
        self._create_exp_tooltip(current_exp, max_exp, exp_percentage)

    def _update_exp_gauges(self, current_exp, max_exp):
        """Update experience gauge visual"""
        # Hide all gauges first
        for gauge in self.exp_gauges:
            gauge.Hide()

        if max_exp <= 0:
            return

        quarter_point = max_exp / 4
        if quarter_point == 0:
            return

        full_count = min(4, current_exp / quarter_point)

        # Show full gauges
        for i in xrange(full_count):
            self.exp_gauges[i].SetRenderingRect(0.0, 0.0, 0.0, 0.0)
            self.exp_gauges[i].Show()

        # Show partial gauge
        if full_count < 4:
            remaining_exp = current_exp % quarter_point
            percentage = float(remaining_exp) / quarter_point - 1.0
            self.exp_gauges[full_count].SetRenderingRect(0.0, percentage, 0.0, 0.0)
            self.exp_gauges[full_count].Show()

    def _create_exp_tooltip(self, current_exp, max_exp, percentage):
        """Create experience tooltip"""
        tooltip_text = uiScriptLocale.ASLAN_BUFF_HOVER_EXP_INFO % (
            localeInfo.MoneyFormat(str(current_exp)),
            localeInfo.MoneyFormat(str(max_exp)),
            percentage
        )
        self.tool_tip_exp = TextToolTip(15)
        self.tool_tip_exp.SetText(tooltip_text)
        self.tool_tip_exp.Hide()

    def OnUpdate(self):
        """Update window state"""
        self._update_tooltips()
        self._update_pet_info()
        self._update_item_slots()
        self._auto_deactivate_maxed_skills()

    def _update_tooltips(self):
        """Update tooltip visibility"""
        # Extra bonus tooltip
        if self.extra_bonus.IsIn():
            self.tooltip_manager[0].Show()
        else:
            self.tooltip_manager[0].Hide()

        # Skill tooltip
        if self.tool_tip.IsShow():
            wndMgr.RefreshSlot(self.gui_components['skills'].GetWindowHandle())

        # Experience tooltip
        if self.exp_hover_info.IsIn():
            if self.tool_tip_exp:
                self.tool_tip_exp.Show()
        else:
            if self.tool_tip_exp:
                self.tool_tip_exp.Hide()

    def _update_pet_info(self):
        """Update pet information display"""
        try:
            if not self._is_pet_data_valid():
                return
                
            pet_data = constInfo.gui_pets
            
            # Update skill points
            points = int(pet_data.get("POINTS", 0))
            points_display = self.gui_components.get('points')
            if points_display:
                points_display.SetText(str(points))
                
                if points > 0:
                    points_display.Show()

            # Update level
            level = str(pet_data.get("LEVEL", 1))
            level_display = self.gui_components.get('lv')
            if level_display:
                level_display.SetText("Lv. " + level)

            # Update experience
            current_exp = int(pet_data.get("EXP", 0))
            max_exp = int(pet_data.get("EXP_NEED", 1))
            self.SetExp(current_exp, max_exp)
            
        except (KeyError, TypeError, ValueError) as e:
            dbg.TraceError("[Pet] Error updating pet info: {}".format(str(e)))

    def _update_item_slots(self):
        """Update item slot displays"""
        try:
            # Main pet slot
            pet_slot = item.SLOT_ITEM_NEW_PET
            pet_item_id = player.GetItemIndex(pet_slot)
            islot = self.gui_components.get('islot')
            if islot:
                islot.SetItemSlot(pet_slot, pet_item_id, 0)
            
            # Equipment slots - use loop like artefakt system
            eqs_slot = self.gui_components.get('eqs')
            if eqs_slot:
                equipment_slots = [
                    item.EQUIPMENT_NEW_PET_EQ0,
                    item.EQUIPMENT_NEW_PET_EQ1,
                    item.EQUIPMENT_NEW_PET_EQ2,
                    item.EQUIPMENT_NEW_PET_EQ3,
                    item.EQUIPMENT_NEW_PET_EQ4
                ]
                
                for slot_number in equipment_slots:
                    item_index = player.GetItemIndex(slot_number)
                    eqs_slot.SetItemSlot(slot_number, item_index, 0)
                
                eqs_slot.RefreshSlot()
            
        except Exception as e:
            dbg.TraceError("[Pet] Error updating item slots: {}".format(str(e)))

    def _auto_deactivate_maxed_skills(self):
        """Auto-deactivate skills that reached max level"""
        skills = self.gui_components['skills']
        
        for skill_index in xrange(PET_SKILL_COUNT):
            skill_level = constInfo.gui_pets["SKILLS"][skill_index]["lv"]
            if skill_level > 29 and skills.IsActivatedSlot(skill_index):
                skills.DeactivateSlot(skill_index)
                net.SendPetAction(0)

    def OnSkillOut(self):
        """Handle mouse out from skill slot"""
        if self.tool_tip:
            self.tool_tip.Hide()

    def OnPressEscapeKey(self):
        """Handle escape key press"""
        self.Close()
        return True

    def Show(self):
        """Show the window"""
        super(PetWindow, self).Show()

    def Close(self):
        """Close the window and clean up"""
        # Deactivate all skills
        self._deactivate_all_skills()

        # Hide tooltips
        if self.tooltip_manager.get(0):
            self.tooltip_manager[0].Hide()

        if self.tool_tip_exp:
            self.tool_tip_exp.Hide()

        self.Hide()
        self.OnSkillOut()


# Legacy alias for backward compatibility
Pety = PetWindow
