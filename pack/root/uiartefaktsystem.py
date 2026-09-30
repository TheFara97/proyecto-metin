import ui
import item
import uiToolTip
import player
import constInfo
import net
import mouseModule
import snd


class ArtefaktWindow(ui.ScriptWindow):
    """Window for managing artifact equipment slots"""
    
    # Class constants
    UI_SCRIPT_PATH = "UIScript/ArtefaktSystemWindow.py"
    PICK_SOUND_PATH = "sound/ui/pick.wav"
    
    def __init__(self):
        super(ArtefaktWindow, self).__init__()
        
        # Initialize state
        self._is_loaded = False
        self._tooltip_item = None
        self._artefakt_slots = None
        self._board = None
        
        # Setup components
        self._initialize_tooltip()
        self._attempt_load_window()
    
    def __del__(self):
        """Cleanup resources"""
        self._cleanup_tooltip()
        super(ArtefaktWindow, self).__del__()
    
    @property
    def is_loaded(self):
        """Check if window is loaded"""
        return self._is_loaded
    
    def _initialize_tooltip(self):
        """Initialize tooltip component"""
        try:
            self._tooltip_item = uiToolTip.ItemToolTip()
            self._tooltip_item.Hide()
        except Exception as e:
            self._log_error("Failed to initialize tooltip", e)
    
    def _cleanup_tooltip(self):
        """Clean up tooltip resources"""
        if self._tooltip_item:
            self._tooltip_item.Hide()
            self._tooltip_item = None
    
    def _attempt_load_window(self):
        """Attempt to load window on initialization"""
        try:
            self._load_window()
        except Exception as e:
            self._log_error("Failed to load window during initialization", e)
    
    def Open(self):
        """Open the artefakt window"""
        if self._is_loaded:
            self._refresh_slot()
            self.Show()
        else:
            self._load_window()
            if self._is_loaded:
                self.Show()
    
    def Close(self):
        """Close the artefakt window"""
        self.Hide()
    
    def OnPressEscapeKey(self):
        """Handle escape key press"""
        self.Close()
        return True
    
    def _load_window(self):
        """Load window UI from script file"""
        if self._is_loaded:
            return
        
        try:
            self._load_ui_script()
            self._setup_ui_components()
            self._finalize_window_setup()
            self._is_loaded = True
            
        except Exception as e:
            self._log_error("Failed to load window", e)
    
    def _load_ui_script(self):
        """Load UI script file"""
        try:
            py_scr_loader = ui.PythonScriptLoader()
            py_scr_loader.LoadScriptFile(self, self.UI_SCRIPT_PATH)
        except Exception as e:
            import exception
            exception.Abort("ArtefaktSystemWindow.LoadWindow.LoadObject")
            raise e
    
    def _setup_ui_components(self):
        """Setup UI components and event handlers"""
        try:
            self._board = self.GetChild("Board")
            
            title_bar = self.GetChild("TitleBar")
            title_bar.SetCloseEvent(ui.__mem_func__(self.Close))
            
            self._setup_artefakt_slots()
            
        except Exception as e:
            self._log_error("Failed to setup UI components", e)
            raise e
    
    def _setup_artefakt_slots(self):
        """Setup artefakt slots with event handlers"""
        artefakt_slots = self.GetChild("ArtefaktSlots")
        
        # Set up event handlers
        event_mappings = [
            ("SetOverInItemEvent", self._on_over_in_item),
            ("SetOverOutItemEvent", self._on_over_out_item),
            ("SetUseSlotEvent", self._on_use_item),
            ("SetUnselectItemSlotEvent", self._on_use_item),
            ("SetSelectEmptySlotEvent", self._on_select_empty_slot),
            ("SetSelectItemSlotEvent", self._on_select_empty_slot),
        ]
        
        for method_name, handler in event_mappings:
            try:
                getattr(artefakt_slots, method_name)(ui.__mem_func__(handler))
            except AttributeError as e:
                self._log_error("Failed to set event handler: {}".format(method_name), e)
        
        self._artefakt_slots = artefakt_slots
    
    def _finalize_window_setup(self):
        """Finalize window setup"""
        self._refresh_slot()
        self.SetCenterPosition()
    
    def _on_over_in_item(self, slot):
        """Handle mouse over item event"""
        if self._tooltip_item:
            try:
                self._tooltip_item.SetInventoryItem(slot)
                self._tooltip_item.Show()
            except Exception as e:
                self._log_error("Failed to show tooltip for slot {}".format(slot), e)
    
    def _on_over_out_item(self):
        """Handle mouse out item event"""
        if self._tooltip_item:
            try:
                self._tooltip_item.HideToolTip()
            except Exception as e:
                self._log_error("Failed to hide tooltip", e)
    
    def _on_select_empty_slot(self, slot):
        """Handle selection of empty slot"""
        try:
            if self._is_mouse_attached():
                self._handle_attached_item_placement(slot)
            else:
                self._handle_item_pickup(slot)
        except Exception as e:
            self._log_error("Failed to handle empty slot selection for slot {}".format(slot), e)
    
    def _is_mouse_attached(self):
        """Check if mouse has attached object"""
        return mouseModule.mouseController.isAttached()
    
    def _handle_attached_item_placement(self, slot):
        """Handle placement of attached item"""
        attached_slot_type = mouseModule.mouseController.GetAttachedType()
        attached_slot_number = mouseModule.mouseController.GetAttachedSlotNumber()
        
        if not self._is_valid_artefakt_item(attached_slot_number):
            return
        
        if attached_slot_type == player.SLOT_TYPE_INVENTORY:
            self._place_item_in_slot(attached_slot_number)
    
    def _is_valid_artefakt_item(self, slot_number):
        """Check if item is a valid artefakt"""
        try:
            vnum = player.GetItemIndex(slot_number)
            item.SelectItem(vnum)
            return item.GetItemType() == item.ITEM_TYPE_ARTEFAKT
        except Exception as e:
            self._log_error("Failed to validate artefakt item for slot {}".format(slot_number), e)
            return False
    
    def _place_item_in_slot(self, slot_number):
        """Place item in artefakt slot"""
        try:
            net.SendItemUsePacket(slot_number)
            self._on_over_out_item()
            self._refresh_slot()
            mouseModule.mouseController.DeattachObject()
        except Exception as e:
            self._log_error("Failed to place item from slot {}".format(slot_number), e)
    
    def _handle_item_pickup(self, slot):
        """Handle picking up item from slot"""
        try:
            item_index = player.GetItemIndex(slot)
            item_count = player.GetItemCount(slot)
            
            mouseModule.mouseController.AttachObject(
                self, player.SLOT_TYPE_INVENTORY, slot, item_index, item_count
            )
            self._play_pick_sound()
        except Exception as e:
            self._log_error("Failed to pickup item from slot {}".format(slot), e)
    
    def _play_pick_sound(self):
        """Play item pickup sound"""
        try:
            snd.PlaySound(self.PICK_SOUND_PATH)
        except Exception as e:
            self._log_error("Failed to play pickup sound", e)
    
    def _on_use_item(self, slot):
        """Handle item use event"""
        if self._is_dialog_blocking():
            return
        
        try:
            self._use_item_in_slot(slot)
        except Exception as e:
            self._log_error("Failed to use item in slot {}".format(slot), e)
    
    def _is_dialog_blocking(self):
        """Check if dialog is blocking actions"""
        try:
            return constInfo.GET_ITEM_QUESTION_DIALOG_STATUS()
        except Exception:
            return False
    
    def _use_item_in_slot(self, slot):
        """Use item in specified slot"""
        net.SendItemUsePacket(slot)
        mouseModule.mouseController.DeattachObject()
        self._on_over_out_item()
        self._refresh_slot()
    
    def _refresh_slot(self):
        """Refresh all artefakt slots"""
        if not self._artefakt_slots:
            return
        
        try:
            for i in xrange(item.ARTEFAKT_SLOT_MAX):
                slot_number = item.EQUIPMENT_ARTEFAKT1 + i
                item_index = player.GetItemIndex(slot_number)
                
                self._artefakt_slots.SetItemSlot(slot_number, item_index, 0)

            slot_number1 = item.EQUIPMENT_ARTEFAKT13
            slot_number2 = item.EQUIPMENT_ARTEFAKT14
            item_index1 = player.GetItemIndex(slot_number1)
            item_index2 = player.GetItemIndex(slot_number2)
                
            self._artefakt_slots.SetItemSlot(slot_number1, item_index1, 0)
            self._artefakt_slots.SetItemSlot(slot_number2, item_index2, 0)
            
            self._artefakt_slots.RefreshSlot()
            
        except Exception as e:
            self._log_error("Failed to refresh slots", e)
    
    def _log_error(self, message, error=None):
        """Log error messages"""
        full_message = "ArtefaktWindow: {}".format(message)
        if error:
            full_message += " - Error: {}".format(str(error))
        
        try:
            import dbg
            dbg.TraceError(full_message)
        except ImportError:
            print full_message
    
    def OverInItem(self, slot):
        """Legacy compatibility method"""
        return self._on_over_in_item(slot)
    
    def OverOutItem(self):
        """Legacy compatibility method"""
        return self._on_over_out_item()
    
    def SelectEmptySlot(self, slot):
        """Legacy compatibility method"""
        return self._on_select_empty_slot(slot)
    
    def UseItem(self, slot):
        """Legacy compatibility method"""
        return self._on_use_item(slot)
    
    def RefreshSlot(self):
        """Legacy compatibility method"""
        return self._refresh_slot()


# Factory function for creating ArtefaktWindow instances
def CreateArtefaktWindow():
    """Factory function to create ArtefaktWindow instance"""
    try:
        return ArtefaktWindow()
    except Exception as e:
        import dbg
        dbg.TraceError("Failed to create ArtefaktWindow: {}".format(str(e)))
        return None
		