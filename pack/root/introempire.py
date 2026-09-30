#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Modern Empire Selection Window
Rewritten for better maintainability and cleaner code structure
"""

import ui
import net
import wndMgr
import app
import _weakref


class EmpireConstants(object):
    """Constants for empire selection"""
    DEFAULT_EMPIRE_ID = 3
    ALPHA_TRANSITION_SPEED = 7.0
    ALPHA_THRESHOLD = 0.0001
    
    # Empire mappings
    EMPIRE_A = net.EMPIRE_A
    EMPIRE_B = net.EMPIRE_B
    EMPIRE_C = net.EMPIRE_C
    
    ALL_EMPIRES = [EMPIRE_A, EMPIRE_B, EMPIRE_C]


class AlphaManager(object):
    """Manages alpha transitions for empire areas"""
    
    def __init__(self, empires):
        self.current_alpha = {empire: 0.0 for empire in empires}
        self.target_alpha = {empire: 0.0 for empire in empires}
    
    def set_target_alpha(self, empire, alpha):
        """Set target alpha for an empire"""
        if empire in self.target_alpha:
            self.target_alpha[empire] = alpha
    
    def set_all_targets(self, alpha_dict):
        """Set target alpha for all empires"""
        self.target_alpha.update(alpha_dict)
    
    def reset_all_except(self, empire):
        """Reset all empire alphas to 0 except the specified one"""
        for emp in self.target_alpha.keys():
            self.target_alpha[emp] = 1.0 if emp == empire else 0.0
    
    def update_alpha_transitions(self, empire_areas):
        """Update alpha transitions for all empire areas"""
        for empire, image in empire_areas.items():
            if empire not in self.current_alpha:
                continue
                
            current = self.current_alpha[empire]
            target = self.target_alpha[empire]
            
            alpha_diff = abs(target - current)
            if alpha_diff / 10 > EmpireConstants.ALPHA_THRESHOLD:
                current += (target - current) / EmpireConstants.ALPHA_TRANSITION_SPEED
            else:
                current = target
            
            self.current_alpha[empire] = current
            image.SetAlpha(current)


class EmpireButton(ui.Window):
    """Interactive button for empire selection"""
    
    def __init__(self, owner, empire_id):
        super(EmpireButton, self).__init__()
        self.owner = _weakref.proxy(owner)
        self.empire_id = empire_id
    
    def OnMouseOverIn(self):
        """Handle mouse over event"""
        self.owner.on_empire_hover_in(self.empire_id)
    
    def OnMouseOverOut(self):
        """Handle mouse out event"""
        self.owner.on_empire_hover_out(self.empire_id)
    
    def OnMouseLeftButtonDown(self):
        """Handle mouse click event"""
        if self.owner.selected_empire_id != self.empire_id:
            self.owner.on_empire_select(self.empire_id)


class UIManager(object):
    """Manages UI elements and script loading"""
    
    def __init__(self, window):
        self.window = window
        self.script_loader = ui.PythonScriptLoader()
        self.empire_areas = {}
        self.empire_buttons = {}
        self.select_button = None
        self.exit_button = None
    
    def load_script(self, filename):
        """Load UI script file"""
        try:
            self.script_loader.LoadScriptFile(self.window, filename)
        except Exception:
            import exception
            exception.Abort("UIManager.load_script.LoadObject")
        
        self._bind_ui_elements()
        self._setup_event_handlers()
    
    def _bind_ui_elements(self):
        """Bind UI elements from script"""
        try:
            get_child = self.window.GetChild
            
            # Bind buttons
            self.select_button = get_child("select_button")
            self.exit_button = get_child("exit_button")
            
            # Bind empire areas
            self.empire_areas[EmpireConstants.EMPIRE_A] = get_child("area_a")
            self.empire_areas[EmpireConstants.EMPIRE_C] = get_child("area_c")
            
            # Initialize alpha values
            for image in self.empire_areas.values():
                image.SetAlpha(0.0)
                
        except Exception:
            import exception
            exception.Abort("UIManager._bind_ui_elements.BindObject")
    
    def _setup_event_handlers(self):
        """Setup event handlers for UI elements"""
        if self.select_button:
            self.select_button.SetEvent(ui.__mem_func__(self.window.on_select_click))
        if self.exit_button:
            self.exit_button.SetEvent(ui.__mem_func__(self.window.on_exit_click))
    
    def create_empire_buttons(self):
        """Create interactive buttons for empire areas"""
        for empire_id, image in self.empire_areas.items():
            x, y = image.GetGlobalPosition()
            
            button = EmpireButton(self.window, empire_id)
            button.SetParent(self.window)
            button.SetPosition(x, y)
            button.SetSize(image.GetWidth(), image.GetHeight())
            button.Show()
            
            self.empire_buttons[empire_id] = button
    
    def cleanup(self):
        """Clean up UI resources"""
        self.empire_areas.clear()
        self.empire_buttons.clear()
        self.select_button = None
        self.exit_button = None


class BaseEmpireWindow(ui.ScriptWindow):
    """Base class for empire selection windows"""
    
    def __init__(self, stream):
        super(BaseEmpireWindow, self).__init__()
        
        # Core components
        self.stream = stream
        self.ui_manager = UIManager(self)
        self.alpha_manager = AlphaManager(EmpireConstants.ALL_EMPIRES)
        
        # State
        self.selected_empire_id = EmpireConstants.DEFAULT_EMPIRE_ID
        
        # Set network phase
        net.SetPhaseWindow(net.PHASE_WINDOW_EMPIRE, self)
    
    def __del__(self):
        """Cleanup on destruction"""
        super(BaseEmpireWindow, self).__del__()
        net.SetPhaseWindow(net.PHASE_WINDOW_EMPIRE, 0)
    
    def Open(self):
        """Open the empire selection window"""
        self._setup_window()
        self._load_ui()
        self._initialize_selection()
        self._show_window()
    
    def Close(self):
        """Close the empire selection window"""
        self._cleanup_resources()
        self.KillFocus()
        self.Hide()
        app.HideCursor()
    
    def _setup_window(self):
        """Setup window properties"""
        screen_width = wndMgr.GetScreenWidth()
        screen_height = wndMgr.GetScreenHeight()
        self.SetSize(screen_width, screen_height)
        self.SetWindowName("SelectEmpireWindow")
    
    def _load_ui(self):
        """Load UI script and create interactive elements"""
        self.ui_manager.load_script("elvion/game/empire/selectempirewindow.py")
        self.ui_manager.create_empire_buttons()
    
    def _initialize_selection(self):
        """Initialize empire selection"""
        self.on_empire_select(self.selected_empire_id)
    
    def _show_window(self):
        """Show window and cursor"""
        self.Show()
        app.ShowCursor()
    
    def _cleanup_resources(self):
        """Clean up window resources"""
        self.ClearDictionary()
        self.ui_manager.cleanup()
    
    def on_empire_hover_in(self, empire_id):
        """Handle empire hover in event"""
        self.alpha_manager.set_target_alpha(empire_id, 1.0)
    
    def on_empire_hover_out(self, empire_id):
        """Handle empire hover out event"""
        if empire_id != self.selected_empire_id:
            self.alpha_manager.set_target_alpha(empire_id, 0.0)
    
    def on_empire_select(self, empire_id):
        """Handle empire selection"""
        self.selected_empire_id = empire_id
        self.alpha_manager.reset_all_except(empire_id)
    
    def on_select_click(self):
        """Handle select button click - to be overridden"""
        raise NotImplementedError("Subclasses must implement on_select_click")
    
    def on_exit_click(self):
        """Handle exit button click - to be overridden"""
        raise NotImplementedError("Subclasses must implement on_exit_click")
    
    def OnUpdate(self):
        """Update alpha transitions"""
        self.alpha_manager.update_alpha_transitions(self.ui_manager.empire_areas)
    
    def OnPressEscapeKey(self):
        """Handle escape key press"""
        self.on_exit_click()
        return True
    
    def OnPressExitKey(self):
        """Handle exit key press"""
        self.on_exit_click()
        return True


class SelectEmpireWindow(BaseEmpireWindow):
    """Main empire selection window for new users"""
    
    def on_select_click(self):
        """Handle select button click"""
        net.SendSelectEmpirePacket(self.selected_empire_id)
        self.stream.SetSelectCharacterPhase()
    
    def on_exit_click(self):
        """Handle exit button click"""
        self.stream.SetLoginPhase()


class ReselectEmpireWindow(BaseEmpireWindow):
    """Empire reselection window for existing users"""
    
    def on_select_click(self):
        """Handle select button click"""
        net.SendSelectEmpirePacket(self.selected_empire_id)
        self.stream.SetCreateCharacterPhase()
    
    def on_exit_click(self):
        """Handle exit button click"""
        self.stream.SetSelectCharacterPhase()


# Backwards compatibility aliases
class EmpireButton_Legacy(EmpireButton):
    """Legacy alias for backwards compatibility"""
    pass
	