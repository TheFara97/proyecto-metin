#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Modern Mount Window System
Rewritten for Python 2.7.18 with modern practices and leak-free design
"""

import ui
import uiToolTip
import net
import player
import item
import exception
import constInfo
import mouseModule
import snd
import localeInfo
import wndMgr
import emoji
import uiScriptLocale
import app
from uiToolTip import ItemToolTip

AFFECT_DICT = ItemToolTip.AFFECT_DICT
SKILL_NAMES = ("1", "2", "3", "4", "5")
PODKOWA_VNUM = 80069
PODKOWA_PRICE = [5000, 30000]

REQUIRED_ITEMS = {
    0: 71129,
    1: 71122
}

REQUIRED_ITEMS_AMOUNT = {
    0: 1,
    1: 1
}


class MouseTooltip(ui.Window):
    """Tooltip that follows mouse cursor"""
    
    def __init__(self, y_offset=0):
        super(MouseTooltip, self).__init__("TOP_MOST")
        
        self._y_offset = y_offset
        self._text_line = None
        self._setup_ui()
    
    def __del__(self):
        """Cleanup resources"""
        self._cleanup()
        super(MouseTooltip, self).__del__()
    
    def _cleanup(self):
        """Clean up text line"""
        if self._text_line:
            self._text_line.Hide()
            self._text_line = None
    
    def _setup_ui(self):
        """Setup UI components"""
        self._text_line = ui.TextLine()
        self._text_line.SetParent(self)
        self._text_line.SetHorizontalAlignLeft()
        self._text_line.SetOutline()
        self._text_line.Show()
    
    def SetText(self, text):
        """Set tooltip text"""
        if self._text_line:
            self._text_line.SetText(text)
    
    def OnRender(self):
        """Render tooltip at mouse position"""
        if self._text_line:
            mouse_x, mouse_y = wndMgr.GetMousePosition()
            self._text_line.SetPosition(mouse_x - 30, mouse_y - 30 + self._y_offset)


class MountSlotManager(object):
    """Manager for mount slot operations"""
    
    def __init__(self, parent_window):
        self._parent = parent_window
        self._mount_slot = None
        self._tooltip = None
    
    def _cleanup(self):
        """Cleanup resources"""
        if self._tooltip:
            self._tooltip.HideToolTip()
            self._tooltip = None
        self._mount_slot = None
        self._parent = None
    
    def setup_slot(self, mount_slot):
        """Setup mount slot with events"""
        self._mount_slot = mount_slot
        
        if self._mount_slot:
            self._mount_slot.SetOverInItemEvent(ui.__mem_func__(self._on_over_in_item))
            self._mount_slot.SetOverOutItemEvent(ui.__mem_func__(self._on_over_out_item))
            self._mount_slot.SetUseSlotEvent(ui.__mem_func__(self._on_use_item))
            self._mount_slot.SetUnselectItemSlotEvent(ui.__mem_func__(self._on_use_item))
            self._mount_slot.SetSelectEmptySlotEvent(ui.__mem_func__(self._on_select_empty_slot))
            self._mount_slot.SetSelectItemSlotEvent(ui.__mem_func__(self._on_select_empty_slot))
    
    def set_tooltip(self, tooltip):
        """Set tooltip reference"""
        self._tooltip = tooltip
    
    def refresh_slot(self):
        """Refresh mount slot display"""
        if not self._mount_slot:
            return
        
        slot_number = item.COSTUME_SLOT_MOUNT
        mount_item_index = player.GetItemIndex(slot_number)
        
        self._mount_slot.SetItemSlot(slot_number, mount_item_index, 0)
        self._mount_slot.RefreshSlot()
    
    def _on_over_in_item(self, slot):
        """Handle mouse over item"""
        if self._tooltip:
            self._tooltip.SetInventoryItem(slot)
            self._tooltip.Show()
    
    def _on_over_out_item(self):
        """Handle mouse out of item"""
        if self._tooltip:
            self._tooltip.HideToolTip()
    
    def _on_select_empty_slot(self, slot):
        """Handle empty slot selection"""
        mouse_controller = mouseModule.mouseController
        
        if mouse_controller.isAttached():
            self._handle_attached_item(slot, mouse_controller)
        else:
            self._attach_slot_item(slot, mouse_controller)
    
    def _handle_attached_item(self, slot, mouse_controller):
        """Handle item attached to mouse"""
        attached_slot_type = mouse_controller.GetAttachedType()
        attached_slot_number = mouse_controller.GetAttachedSlotNumber()
        
        if attached_slot_type != player.SLOT_TYPE_INVENTORY:
            mouse_controller.DeattachObject()
            return
        
        vnum = player.GetItemIndex(attached_slot_number)
        item.SelectItem(vnum)
        
        if item.GetItemType() != item.COSTUME_SLOT_MOUNT:
            mouse_controller.DeattachObject()
            return
        
        # Use the item
        net.SendItemUsePacket(attached_slot_number)
        self._on_over_out_item()
        
        if self._parent and hasattr(self._parent, 'refresh_display'):
            self._parent.refresh_display()
        
        mouse_controller.DeattachObject()
    
    def _attach_slot_item(self, slot, mouse_controller):
        """Attach slot item to mouse"""
        item_index = player.GetItemIndex(slot)
        item_count = player.GetItemCount(slot)
        
        if item_index > 0:
            mouse_controller.AttachObject(
                self._parent, player.SLOT_TYPE_INVENTORY, 
                slot, item_index, item_count
            )
            snd.PlaySound("sound/ui/pick.wav")
    
    def _on_use_item(self, slot):
        """Handle item use"""
        if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS():
            return
        
        net.SendItemUsePacket(slot)
        mouseModule.mouseController.DeattachObject()
        self._on_over_out_item()
        
        if self._parent and hasattr(self._parent, 'refresh_display'):
            self._parent.refresh_display()


class MountInfoManager(object):
    """Manager for mount information display"""
    
    def __init__(self, parent_window):
        self._parent = parent_window
        self._level = 0
        self._exp_info = [0, 0]
        self._skill_points = 0
        self._ui_elements = {}
    
    def _cleanup(self):
        """Cleanup resources"""
        self._ui_elements.clear()
        self._parent = None
    
    def setup_ui_elements(self, mount_level_element, skill_points_element=None):
        """Setup UI element references"""
        self._ui_elements = {
            "mount_level": mount_level_element,
            "skill_points": skill_points_element
        }
    
    def set_level(self, level):
        """Set mount level"""
        self._level = level
        self._update_level_display()
    
    def set_exp_info(self, current_exp, max_exp):
        """Set experience information"""
        self._exp_info = [current_exp, max_exp]
    
    def set_skill_points(self, points):
        """Set skill points"""
        self._skill_points = points
        self._update_skill_points_display()
    
    def clear_display(self):
        """Clear all display information"""
        mount_level = self._ui_elements.get("mount_level")
        skill_points = self._ui_elements.get("skill_points")
        
        if mount_level:
            mount_level.SetText("")
        
        if skill_points:
            skill_points.SetText("")
    
    def _update_level_display(self):
        """Update level display"""
        mount_level = self._ui_elements.get("mount_level")
        if mount_level:
            mount_level.SetText(str(self._level))
    
    def _update_skill_points_display(self):
        """Update skill points display"""
        skill_points = self._ui_elements.get("skill_points")
        if skill_points:
            skill_points.SetText(str(self._skill_points))
    
    def get_level(self):
        """Get current mount level"""
        return self._level
    
    def get_exp_info(self):
        """Get experience information"""
        return tuple(self._exp_info)
    
    def get_skill_points(self):
        """Get skill points"""
        return self._skill_points


if app.ENABLE_MOUNT_COSTUME_SYSTEM:
    class MountWindow(ui.ScriptWindow):
        """Modern mount window with improved architecture"""
        
        def __init__(self):
            super(MountWindow, self).__init__()
            
            self._is_loaded = False
            self._ui_elements = {}
            
            self._slot_manager = MountSlotManager(self)
            self._info_manager = MountInfoManager(self)
            
            self._item_tooltip = None
            self._exp_tooltip = None
            
            self._initialize_tooltips()
            self._load_window()
        
        def __del__(self):
            """Cleanup resources"""
            self._cleanup()
            super(MountWindow, self).__del__()
        
        def _cleanup(self):
            """Clean up all resources"""
            if self._slot_manager:
                self._slot_manager._cleanup()
                self._slot_manager = None
            
            if self._info_manager:
                self._info_manager._cleanup()
                self._info_manager = None
            
            if self._item_tooltip:
                self._item_tooltip.Hide()
                self._item_tooltip = None
            
            if self._exp_tooltip:
                self._exp_tooltip = None
            
            self._ui_elements.clear()
        
        def _initialize_tooltips(self):
            """Initialize tooltip objects"""
            self._item_tooltip = uiToolTip.ItemToolTip()
            self._item_tooltip.Hide()
            
            self._exp_tooltip = None  # Will be created if needed
        
        def _load_window(self):
            """Load window UI"""
            if self._is_loaded:
                return
            
            try:
                py_loader = ui.PythonScriptLoader()
                py_loader.LoadScriptFile(self, "uiscript/mount_window.py")
                
                self._bind_ui_elements()
                self._setup_window()
                
                self._is_loaded = True
                
            except Exception as e:
                import exception
                exception.Abort("MountWindow._load_window: {0}".format(str(e)))
        
        def _bind_ui_elements(self):
            """Bind UI element references"""
            try:
                # Main elements
                self._ui_elements["title_bar"] = self.GetChild("TitleBar")
                self._ui_elements["mount_level"] = self.GetChild("mount_level")
                self._ui_elements["mount_slot"] = self.GetChild("MountSlot")
                
                # Optional elements
                try:
                    self._ui_elements["skill_points"] = self.GetChild("skill_points")
                except:
                    self._ui_elements["skill_points"] = None
                
            except Exception as e:
                import exception
                exception.Abort("MountWindow._bind_ui_elements: {0}".format(str(e)))
        
        def _setup_window(self):
            """Setup window components"""
            title_bar = self._ui_elements.get("title_bar")
            if title_bar:
                title_bar.SetCloseEvent(ui.__mem_func__(self.Close))
            
            mount_slot = self._ui_elements.get("mount_slot")
            if mount_slot:
                self._slot_manager.setup_slot(mount_slot)
                self._slot_manager.set_tooltip(self._item_tooltip)
            
            mount_level = self._ui_elements.get("mount_level")
            skill_points = self._ui_elements.get("skill_points")
            self._info_manager.setup_ui_elements(mount_level, skill_points)
            
            self.SetCenterPosition()
            self.SetTop()
        
        def Show(self):
            """Show mount window"""
            self._load_window()
            
            if self._is_loaded:
                self.refresh_display()
                super(MountWindow, self).Show()
        
        def Open(self):
            """Open mount window (alias for Show)"""
            self.Show()
        
        def Close(self):
            """Close mount window"""
            self.Hide()
        
        def Destroy(self):
            """Destroy mount window"""
            self.Hide()
            self.ClearDictionary()
            self._cleanup()
        
        def OnPressEscapeKey(self):
            """Handle escape key press"""
            self.Close()
            return True
        
        def refresh_display(self):
            """Refresh all display elements"""
            if not self._is_loaded:
                return
            
            try:
                if self._slot_manager:
                    self._slot_manager.refresh_slot()
                
                self._refresh_mount_info()
                
            except Exception as e:
                import exception
                exception.Abort("MountWindow.refresh_display: {0}".format(str(e)))
        
        def _refresh_mount_info(self):
            """Refresh mount information display"""
            # This would typically get mount data from the game
            # For now, we'll use placeholder logic
            
            # Get mount level (this would come from game data)
            mount_level = self._get_mount_level()
            self._info_manager.set_level(mount_level)
            
            # Get skill points (this would come from game data)
            skill_points = self._get_mount_skill_points()
            self._info_manager.set_skill_points(skill_points)
        
        def _get_mount_level(self):
            """Get current mount level"""
            slot_number = item.COSTUME_SLOT_MOUNT
            mount_item_index = player.GetItemIndex(slot_number)
            
            if mount_item_index > 0:
                return 1
            
            return 0
        
        def _get_mount_skill_points(self):
            """Get mount skill points"""
            # This would typically query the game for skill points
            # Placeholder implementation
            return 0  # Placeholder
        
        def set_clear(self):
            """Clear mount information display"""
            if self._info_manager:
                self._info_manager.clear_display()
        
        def set_level(self, level):
            """Set mount level externally"""
            if self._info_manager:
                self._info_manager.set_level(level)
        
        def set_exp_info(self, current_exp, max_exp):
            """Set experience information externally"""
            if self._info_manager:
                self._info_manager.set_exp_info(current_exp, max_exp)
        
        def set_skill_points(self, points):
            """Set skill points externally"""
            if self._info_manager:
                self._info_manager.set_skill_points(points)
        
        def get_mount_info(self):
            """Get current mount information"""
            if self._info_manager:
                return {
                    "level": self._info_manager.get_level(),
                    "exp_info": self._info_manager.get_exp_info(),
                    "skill_points": self._info_manager.get_skill_points()
                }
            return {"level": 0, "exp_info": (0, 0), "skill_points": 0}
        
        def SelectEmptySlot(self, slot):
            """Forward to slot manager"""
            if self._slot_manager:
                self._slot_manager._on_select_empty_slot(slot)
        
        def UseItem(self, slot):
            """Forward to slot manager"""
            if self._slot_manager:
                self._slot_manager._on_use_item(slot)
        
        def OverInItem(self, slot):
            """Forward to slot manager"""
            if self._slot_manager:
                self._slot_manager._on_over_in_item(slot)
        
        def OverOutItem(self):
            """Forward to slot manager"""
            if self._slot_manager:
                self._slot_manager._on_over_out_item()
        
        def RefreshSlots(self):
            """Refresh slots (backward compatibility)"""
            self.refresh_display()
        
        def SetClear(self):
            """Clear display (backward compatibility)"""
            self.set_clear()


def CreateMountWindow():
    """Factory function to create mount window"""
    if app.ENABLE_MOUNT_COSTUME_SYSTEM:
        return MountWindow()
    return None


# Utility functions for mount system
class MountUtils(object):
    """Utility functions for mount system"""
    
    @staticmethod
    def is_mount_item(item_vnum):
        """Check if item is a mount costume"""
        item.SelectItem(item_vnum)
        return item.GetItemType() == item.COSTUME_SLOT_MOUNT
    
    @staticmethod
    def get_current_mount():
        """Get currently equipped mount"""
        slot_number = item.COSTUME_SLOT_MOUNT
        return player.GetItemIndex(slot_number)
    
    @staticmethod
    def has_mount_equipped():
        """Check if player has mount equipped"""
        return MountUtils.get_current_mount() > 0
    
    @staticmethod
    def get_required_items_for_upgrade(level):
        """Get required items for mount upgrade"""
        if level in REQUIRED_ITEMS:
            return {
                "item_vnum": REQUIRED_ITEMS[level],
                "item_amount": REQUIRED_ITEMS_AMOUNT.get(level, 1)
            }
        return None
    
    @staticmethod
    def play_mount_sound(sound_type="pick"):
        """Play mount-related sound"""
        sound_map = {
            "pick": "sound/ui/pick.wav",
            "drop": "sound/ui/drop.wav",
            "click": "sound/ui/click.wav"
        }
        
        sound_file = sound_map.get(sound_type, "sound/ui/pick.wav")
        snd.PlaySound(sound_file)


# Example usage and integration
class MountSystemManager(object):
    """Manager for the entire mount system"""
    
    def __init__(self):
        self._mount_window = None
        self._is_initialized = False
    
    def initialize(self):
        """Initialize mount system"""
        if not app.ENABLE_MOUNT_COSTUME_SYSTEM:
            return False
        
        try:
            self._mount_window = CreateMountWindow()
            self._is_initialized = True
            return True
        except Exception as e:
            import exception
            exception.Abort("MountSystemManager.initialize: {0}".format(str(e)))
            return False
    
    def open_mount_window(self):
        """Open mount window"""
        if self._mount_window:
            self._mount_window.Open()
    
    def close_mount_window(self):
        """Close mount window"""
        if self._mount_window:
            self._mount_window.Close()
    
    def is_window_open(self):
        """Check if mount window is open"""
        return self._mount_window and self._mount_window.IsShow()
    
    def update_mount_info(self, level, current_exp, max_exp, skill_points):
        """Update mount information"""
        if self._mount_window:
            self._mount_window.set_level(level)
            self._mount_window.set_exp_info(current_exp, max_exp)
            self._mount_window.set_skill_points(skill_points)
    
    def refresh_mount_display(self):
        """Refresh mount display"""
        if self._mount_window:
            self._mount_window.refresh_display()
    
    def cleanup(self):
        """Cleanup mount system"""
        if self._mount_window:
            self._mount_window.Destroy()
            self._mount_window = None
        self._is_initialized = False
		