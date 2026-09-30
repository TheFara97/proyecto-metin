import ui, app, net, constInfo, player
import datetime
import chat
import dbg
import wndMgr
import localeInfo
import exception
import emoji
from uiToolTip import ItemToolTip

personal_activated_events = {}

EVENT_PERSONAL_DURATION = 1800

_tracker_saved_pos = None

IMG_DIR = "d:/ymir work/ui/game/event_calendar/"
IMG_ICON_DIR = "d:/ymir work/ui/game/event_calendar/icons/"
MINI_IMG_ICON_DIR = "d:/ymir work/ui/game/event_calendar/icons/mini_gui/"

EVENT_DAY_INDEX = 0
EVENT_ID = 1
EVENT_INDEX = 2
EVENT_START_TEXT = 3
EVENT_END_TEXT = 4
EVENT_EMPIRE_FLAG = 5
EVENT_CHANNEL_FLAG = 6
EVENT_VALUE0 = 7
EVENT_VALUE1 = 8
EVENT_VALUE2 = 9
EVENT_VALUE3 = 10
EVENT_START_TIME = 11
EVENT_END_TIME = 12
EVENT_IS_ACTIVE = 13

events_default_data = {
    player.BONUS_EVENT: ["bonus_event", localeInfo.BONUS_EVENT],
    player.DOUBLE_BOSS_LOOT_EVENT: ["icon_event_boss", localeInfo.DOUBLE_BOSS_LOOT_EVENT],
    player.DOUBLE_METIN_LOOT_EVENT: ["icon_event_stone", localeInfo.DOUBLE_METIN_LOOT_EVENT],
    player.DOUBLE_MISSION_BOOK_EVENT: ["double_mission_book_event", localeInfo.DOUBLE_MISSION_BOOK_EVENT],
    player.DUNGEON_COOLDOWN_EVENT: ["dungeon_cooldown_event", localeInfo.DUNGEON_COOLDOWN_EVENT],
    player.DUNGEON_CHEST_LOOT_EVENT: ["dungeon_ticket_loot_event", localeInfo.DUNGEON_CHEST_LOOT_EVENT],
    player.EMPIRE_WAR_EVENT: ["empire_war_event", ""],
    player.MOONLIGHT_EVENT: ["icon_event_moonlight", localeInfo.MOONLIGHT_EVENT],
    player.VALENTINE_EVENT: ["valentine_event", localeInfo.VALENTINE_EVENT],
    player.TOURNAMENT_EVENT: ["tournament_event", ""],
    player.WHELL_OF_FORTUNE_EVENT: ["whell_of_fortune_event", localeInfo.WHELL_OF_FORTUNE_EVENT],
    player.HALLOWEEN_EVENT: ["halloween_event", localeInfo.HALLOWEEN_EVENT],
    player.NPC_SEARCH_EVENT: ["npc_search", ""],
    player.ITEM_DROP_EVENT: ["item_drop_event", localeInfo.ITEM_DROP_EVENT],
    player.YANG_DROP_EVENT: ["money_drop_event", localeInfo.YANG_DROP_EVENT],
    player.EXP_EVENT: ["icon_event_exp", localeInfo.EXP_EVENT],
    player.SWITCH_RARE_EVENT: ["icon_event_rare", localeInfo.SWITCH_RARE_EVENT],
    player.SWITCH_AVERAGE_EVENT: ["icon_event_average_damage", localeInfo.SWITCH_AVERAGE_EVENT],
    player.SWITCH_RUNE_EVENT: ["icon_event_relics", localeInfo.SWITCH_RUNE_EVENT],
    player.DOUBLE_ACHIEVEMENT_POINTS: ["icon_event_double_ap", localeInfo.DOUBLE_ACHIEVEMENT_POINTS],
    player.MINING_CHEST_EVENT: ["icon_event_mining_chest", localeInfo.MINING_CHEST_EVENT],
    player.DOUBLE_ORE_EVENT: ["icon_event_double_ore", localeInfo.DOUBLE_ORE_EVENT],
    player.BIGGER_STAR_CHANCE: ["icon_event_star", localeInfo.BIGGER_STAR_CHANCE],
}

server_event_data = {}


class EventDataManager(object):
    
    @staticmethod
    def calculate_day_count(month, year):
        if month == 2:
            if ((year % 400 == 0) or (year % 4 == 0 and year % 100 != 0)):
                return 29
            else:
                return 28
        elif month in (1, 3, 5, 7, 8, 10, 12):
            return 31
        else:
            return 30
    
    @staticmethod
    def set_event_status(event_id, event_status, end_time, end_time_text):
        for day_index, event_list in server_event_data.iteritems():
            if event_id in event_list:
                event_list[event_id][EVENT_IS_ACTIVE] = event_status
                event_list[event_id][EVENT_END_TIME] = end_time
                event_list[event_id][EVENT_END_TEXT] = end_time_text
    
    @staticmethod
    def set_server_data(day_index, event_id, event_index, start_time, end_time, 
                       empire_flag, channel_flag, value0, value1, value2, value3, 
                       start_real_time, end_real_time, is_already_start):
        event_data = [
            day_index, event_id, event_index, start_time, end_time, empire_flag,
            channel_flag, value0, value1, value2, value3, start_real_time,
            end_real_time, is_already_start
        ]
        
        if day_index in server_event_data:
            server_event_data[day_index][event_id] = event_data
        else:
            server_event_data[day_index] = {event_id: event_data}
    
    @staticmethod
    def is_event_id_active(event_id):
        for day_index, event_list in server_event_data.iteritems():
            if event_id in event_list:
                return event_list[event_id][EVENT_IS_ACTIVE]
        return False
    
    @staticmethod
    def is_event_index_active(event_index):
        for day_index, event_list in server_event_data.iteritems():
            for event_id, event_data in event_list.iteritems():
                if event_data[EVENT_IS_ACTIVE] and event_data[EVENT_INDEX] == event_index:
                    return True
        return False
    
    @staticmethod
    def get_event_index_data(event_index):
        for day_index, event_list in server_event_data.iteritems():
            for event_id, event_data in event_list.iteritems():
                if event_data[EVENT_INDEX] == event_index:
                    return event_data
        return None
    
    @staticmethod
    def get_event_id_data(event_id):
        """Get event data by event ID"""
        for day_index, event_list in server_event_data.iteritems():
            if event_id in event_list:
                return event_list[event_id]
        return None


class ImageBoxSpecial(ui.ImageBox):
    
    def __init__(self, is_mini_icon=False):
        super(ImageBoxSpecial, self).__init__()
        self._initialize_attributes(is_mini_icon)
        self._setup_mini_icon_behavior()
    
    def __del__(self):
        super(ImageBoxSpecial, self).__del__()
    
    def _initialize_attributes(self, is_mini_icon):
        self.mini_icon = None
        self.event_list = []
        self.image_index = 0
        self.is_mini_icon = is_mini_icon
        self.day_index = 0
        
        self.waiting_time = 2.0
        self.sleep_time = 0.0
        self.alpha_value = 0.3
        self.increase_value = 0.05
        self.min_alpha = 0.3
        self.max_alpha = 1.0
        self.alpha_status = False
    
    def _setup_mini_icon_behavior(self):
        if self.is_mini_icon:
            self.AddFlag("attach")
            self.AddFlag("movable")
            x, y = wndMgr.GetScreenWidth() - 245, 130
            self.SetPosition(x, y)
            self.SetEvent(ui.__mem_func__(self._on_click_event_icon), "mouse_click")
            self.SetMouseLeftButtonDoubleClickEvent(ui.__mem_func__(self._on_click_double))
            self.SetMouseRightButtonDownEvent(ui.__mem_func__(self._next_event_with_key))
    
    def Destroy(self):
        self.mini_icon = None
        self.event_list = []
        self.image_index = 0
        self.is_mini_icon = False
        self.waiting_time = 0.0
        self.sleep_time = 0.0
        self.alpha_value = 0.0
        self.increase_value = 0.0
        self.min_alpha = 0.0
        self.max_alpha = 0.0
        self.alpha_status = False
    
    def OnMoveWindow(self, x, y):
        screen_width, screen_height = wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight()
        
        if x < 0:
            x = 0
        elif x + self.GetWidth() >= screen_width - 70:
            x = screen_width - 80 - self.GetWidth()
        
        if y < 0:
            y = 0
        elif y + self.GetHeight() >= screen_height - 100:
            y = screen_height - 100 - self.GetHeight()
        
        self.SetPosition(x, y)
    
    def _next_event_with_key(self):
        if len(self.event_list) > 1:
            self.sleep_time = 0
            self.alpha_value = self.max_alpha
            self.alpha_status = True
    
    def SetBackgroundImage(self, image):
        self.LoadImage(image)
        self.SAFE_SetStringEvent("MOUSE_OVER_IN", self.OverInItem)
        self.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.OverOutItem)
        self.SetEvent(ui.__mem_func__(self._on_click_day_cell), "mouse_click")

    def _on_click_day_cell(self):
        interface = constInfo.GetInterfaceInstance()
        if interface and interface.wndEventManager:
            interface.wndEventManager.ShowDayPanel(self.day_index)

    def _on_click_event_icon(self):
        index, event_id = self._get_next_image(self.image_index)
        event_data = EventDataManager.get_event_id_data(event_id)

        if event_data and event_data[EVENT_IS_ACTIVE]:
            interface = constInfo.GetInterfaceInstance()
            if interface and interface.wndEventManager:
                interface.wndEventManager.OnClick(event_data[EVENT_INDEX])
    
    def _on_click_double(self):
        interface = constInfo.GetInterfaceInstance()
        if interface:
            interface.OpenEventCalendar()
    
    def SetImage(self, folder):
        if self.mini_icon is None:
            self.mini_icon = ui.ImageBox()
            self.mini_icon.SetParent(self)
            self.mini_icon.AddFlag("not_pick")
        
        image_path = (MINI_IMG_ICON_DIR if self.is_mini_icon else IMG_ICON_DIR) + folder + ".tga"
        self.mini_icon.LoadImage(image_path)
        self.mini_icon.SetPosition(0, 6)
        self.mini_icon.Show()
        
        if self.is_mini_icon:
            self.SetSize(96, 96)
        
        self.alpha_value = self.min_alpha
        self.alpha_status = False
    
    def OverOutItem(self):
        interface = constInfo.GetInterfaceInstance()
        if interface and interface.tooltipItem:
            interface.tooltipItem.HideToolTip()
    
    def OverInItem(self):
        interface = constInfo.GetInterfaceInstance()
        if interface and interface.wndEventManager:
            interface.wndEventManager.OverInItem(self.day_index)
    
    def Clear(self):
        self.mini_icon = None
        self.event_list = []
        if self.is_mini_icon:
            self.SetSize(0, 0)
    
    def DeleteImage(self, event_index):
        if 0 <= event_index < len(self.event_list):
            del self.event_list[event_index]
            if self.is_mini_icon and len(self.event_list) == 0:
                self.SetSize(0, 0)
    
    def AppendImage(self, event_id):
        if event_id in self.event_list:
            return
        
        self.event_list.append(event_id)
        
        if len(self.event_list) == 1:
            self.image_index = 0
            event_data = EventDataManager.get_event_id_data(event_id)
            if event_data:
                event_index = event_data[EVENT_INDEX]
                if event_index in events_default_data:
                    self.SetImage(events_default_data[event_index][0])
        
        if self.is_mini_icon:
            self.SetSize(96, 96)
    
    def _get_next_image(self, list_index):
        if list_index >= len(self.event_list):
            if len(self.event_list) > 0:
                return 0, self.event_list[0]
            return 0, 0
        return list_index, self.event_list[list_index]
    
    def OnUpdate(self):
        if len(self.event_list) <= 1:
            self.image_index = 0
            return
        
        if self.sleep_time > app.GetTime():
            return
        
        if self.alpha_status:
            self.alpha_value -= self.increase_value
            if self.alpha_value < self.min_alpha:
                self.alpha_value = self.min_alpha
                self.alpha_status = False
                
                image_index, event_id = self._get_next_image(self.image_index + 1)
                event_data = EventDataManager.get_event_id_data(event_id)
                if event_data:
                    event_index = event_data[EVENT_INDEX]
                    if event_index in events_default_data:
                        self.SetImage(events_default_data[event_index][0])
                self.image_index = image_index
        else:
            self.alpha_value += self.increase_value
            if self.alpha_value > self.max_alpha:
                self.alpha_status = True
                self.sleep_time = app.GetTime() + self.waiting_time
        
        if self.mini_icon:
            self.mini_icon.SetAlpha(self.alpha_value)


class EventCalendarWindow(ui.ScriptWindow):
    
    def __init__(self):
        super(EventCalendarWindow, self).__init__()
        self._initialize_attributes()
        self._load_window()
    
    def __del__(self):
        super(EventCalendarWindow, self).__del__()
    
    def _initialize_attributes(self):
        self.children = {}
        self.current_month = 0
        self.current_year = 0
        self.board = None
        self.exit_btn = None
        self.final_date_text = None
        self.day_panel = None
    
    def Destroy(self):
        self.children = {}
        self.current_month = 0
        self.current_year = 0
        if self.day_panel:
            self.day_panel.Destroy()
            self.day_panel = None
    
    def _load_window(self):
        try:
            py_scr_loader = ui.PythonScriptLoader()
            py_scr_loader.LoadScriptFile(self, "uiscript/eventcalendar.py")
        except:
            exception.Abort("EventCalendar.LoadDialog.LoadScript")
        
        try:
            self.board = self.GetChild("board")
            self.exit_btn = self.GetChild("ExitButton")
            self.final_date_text = self.GetChild("final_date_text")
            
            self.exit_btn.SetEvent(ui.__mem_func__(self.Close))
        except:
            exception.Abort("EventCalendar.BindObjects")
        
        self._initialize_calendar()
    
    def _initialize_calendar(self):
        dt = datetime.datetime.today()
        self.current_month, self.current_year = dt.month, dt.year
        day_count = EventDataManager.calculate_day_count(self.current_month, self.current_year)
        
        for day in xrange(day_count):
            self._create_day_element(day, dt)
        
        self.Refresh()
    
    def _create_day_element(self, day, current_date):
        y_calculate = day // 8
        x_calculate = day - (y_calculate * 8)
        
        day_images = ImageBoxSpecial(False)
        day_images.SetParent(self.board)
        
        if current_date.day == day + 1:
            day_images.SetBackgroundImage(IMG_DIR + "today_bg.png")
        else:
            day_images.SetBackgroundImage(IMG_DIR + "black_bg.png")
        
        day_images.SetPosition(8 + (x_calculate * 66), 8 + (y_calculate * 62))
        day_images.day_index = day + 1
        day_images.Show()
        self.children["dayImages%d" % day] = day_images
        
        day_index = ui.NumberLine()
        day_index.SetParent(day_images)
        day_index.SetNumber(str(day + 1))
        day_index.SetPosition(8, 8)
        day_index.Show()
        self.children["dayIndex%d" % day] = day_index
    
    def Open(self):
        self.Show()
        self.Refresh()
        self.SetTop()
    
    def Close(self):
        self.Hide()
    
    def OnPressEscapeKey(self):
        self.Close()
        return True
    
    def OnUpdate(self):
        for day_index in xrange(31):
            day_key = "dayImages%d" % day_index
            if day_key in self.children:
                self.children[day_key].OnUpdate()
    
    def Refresh(self):
        dt = datetime.datetime.today()
        day_count = EventDataManager.calculate_day_count(self.current_month, self.current_year)
        
        self.final_date_text.SetText(
            "|cFF9cca67%02d-%02d  |r-  |cFF9cca67%02d-%02d  |r(%04d)" % 
            (self.current_month, 1, self.current_month, day_count, self.current_year)
        )
        
        for day_index in xrange(31):
            self._refresh_day(day_index, dt)
    
    def _refresh_day(self, day_index, current_date):
        day_key = "dayImages%d" % day_index
        if day_key not in self.children:
            return
        
        day_event_image = self.children[day_key]
        day_event_image.Clear()
        
        if (day_index + 1) in server_event_data:
            event_dict = server_event_data[day_index + 1]
            
            if current_date.day == day_index + 1:
                day_event_image.SetBackgroundImage(IMG_DIR + "today_bg.png")
            else:
                day_event_image.SetBackgroundImage(IMG_DIR + "blue_bg.tga")
            
            for event_id in event_dict:
                day_event_image.AppendImage(event_id)
        else:
            if current_date.day == day_index + 1:
                day_event_image.SetBackgroundImage(IMG_DIR + "today_bg.png")
            else:
                day_event_image.SetBackgroundImage(IMG_DIR + "black_bg.png")
        
        day_event_image.Show()
    
    def ShowDayPanel(self, day_index):
        if self.day_panel is None:
            self.day_panel = EventDayPanel(self)
        self.day_panel.Load(day_index, self.current_month, self.current_year)
        self.day_panel.Show()

    def RefreshDayPanel(self):
        if self.day_panel and self.day_panel.IsShow():
            self.day_panel.RefreshButtons()

    def OnEventActivateResult(self, event_id, result):
        if self.day_panel:
            self.day_panel.OnActivateResult(event_id, result)

    def OnClick(self, event_index):
        pass
    
    def _get_bonus_name(self, affect, value):
        return ItemToolTip.AFFECT_DICT[affect](value)
    
    def _calculate_time(self, event_index, start_time_text, end_time_text, end_time):
        empty_times = ("1970-01-01 03:00:00", "1970-01-01 02:00:00", 
                      "1970-01-01 01:00:00", "1970-01-01 00:00:00", "")
        
        if end_time_text in empty_times:
            start_time_second = start_time_text.split(" ")[1]
            return localeInfo.PVP_EVENT_TIME % start_time_second
        
        start_parts = start_time_text.split(" ")
        end_parts = end_time_text.split(" ")
        start_date, start_time = start_parts[0], start_parts[1]
        end_date, end_time = end_parts[0], end_parts[1]
        
        begin_text = ""
        end_text = ""
        
        if start_date != end_date:
            start_date_parts = start_date.split("-")
            end_date_parts = end_date.split("-")
            begin_text += start_date_parts[2] + "/" + start_date_parts[1] + " "
            end_text += end_date_parts[2] + "/" + end_date_parts[1] + " "
        
        begin_text += start_time
        end_text += end_time
        
        return localeInfo.NORMAL_EVENT_TIME % (begin_text, end_text)
    
    def _get_map_name(self, map_index):
        map_names = {
            61: localeInfo.MOUNT_SOHAN_MAP_NAME,
            62: localeInfo.MOUNT_DOYUMHWAN_MAP_NAME,
            63: localeInfo.MOUNT_YONGBI_MAP_NAME,
        }
        return map_names.get(map_index, "Unknown Map Name")
    
    def OverInItem(self, day_index):
        interface = constInfo.GetInterfaceInstance()
        if not interface or not interface.tooltipItem:
            return
        
        tooltip_item = interface.tooltipItem
        tooltip_item.ClearToolTip()
        tooltip_item.ShowToolTip()
        tooltip_item.SetTitle(
            localeInfo.EVENT_TOOLTIP_TITLE % 
            "%04d-%02d-%02d" % (self.current_year, self.current_month, day_index)
        )
        tooltip_item.AppendSpace(5)
        
        if day_index in server_event_data:
            self._add_event_tooltips(tooltip_item, server_event_data[day_index])
        else:
            tooltip_item.AppendTextLine(localeInfo.EVENT_TOOLTIP_DOESNT_HAVE_EVENT)
    
    def _add_event_tooltips(self, tooltip_item, event_list):
        events = event_list.values()
        events.sort(key=lambda x: x[EVENT_START_TIME])
        
        for event_data in events:
            self._add_single_event_tooltip(tooltip_item, event_data)
    
    def _add_single_event_tooltip(self, tooltip_item, event_data):
        event_name = ""
        top_info = ""
        
        empire_texts = [
            localeInfo.ALL_KINGDOMS, localeInfo.RED_KINGDOM,
            localeInfo.YELLOW_KINGDOM, localeInfo.BLUE_KINGDOM
        ]
        
        empire_flag = event_data[EVENT_EMPIRE_FLAG]
        if empire_flag >= len(empire_texts):
            empire_flag = 0
        top_info += empire_texts[empire_flag] + ", "
        
        if event_data[EVENT_CHANNEL_FLAG] == 0:
            top_info += "|cFFcabdab" + localeInfo.ALL_CHANNEL + "|r "
        else:
            top_info += ("|cFFcabdab" + localeInfo.ONLY_CHANNEL + "|r") % event_data[EVENT_CHANNEL_FLAG]
        
        if self._handle_special_event_tooltip(tooltip_item, event_data):
            return
        
        event_index = event_data[EVENT_INDEX]
        if event_index == player.BONUS_EVENT:
            if event_data[EVENT_VALUE0] > 0 and event_data[EVENT_VALUE1] > 0:
                event_name += (events_default_data[event_index][1] + " " + 
                             self._get_bonus_name(event_data[EVENT_VALUE0], event_data[EVENT_VALUE1]))
            else:
                event_name += events_default_data[event_index][1] + " " + localeInfo.NONE_AFFECT
        elif event_index in (player.ITEM_DROP_EVENT, player.YANG_DROP_EVENT, player.EXP_EVENT):
            event_name += events_default_data[event_index][1] % event_data[EVENT_VALUE0]
        else:
            event_name += events_default_data[event_index][1]
        
        tooltip_item.AppendTextLine(event_name)
        tooltip_item.AppendSpace(5)
        tooltip_item.AppendTextLine(top_info)
        tooltip_item.AppendSpace(5)
        tooltip_item.AppendTextLine(
            self._calculate_time(event_index, event_data[EVENT_START_TEXT], 
                               event_data[EVENT_END_TEXT], event_data[EVENT_END_TIME])
        )
        tooltip_item.AppendSpace(5)
        tooltip_item.AppendTextLine("-" * 41)
        tooltip_item.ResizeToolTipWidth(272)
        tooltip_item.AlignTextLineHorizonalCenter()
    
    def _handle_special_event_tooltip(self, tooltip_item, event_data):
        event_index = event_data[EVENT_INDEX]
        
        if event_index == player.NPC_SEARCH_EVENT:
            tooltip_item.AppendTextLine(localeInfo.NPC_SEARCH)
            tooltip_item.AppendTextLine(localeInfo.NPC_SEARCH_TEXT)
            
            for j in xrange(4):
                if event_data[EVENT_VALUE0 + j] > 0:
                    tooltip_item.AppendTextLine(self._get_map_name(event_data[EVENT_VALUE0 + j]))
            
            tooltip_item.AppendSpace(5)
            tooltip_item.AppendTextLine(
                self._calculate_time(event_index, event_data[EVENT_START_TEXT], 
                                   event_data[EVENT_END_TEXT], event_data[EVENT_END_TIME])
            )
            tooltip_item.AppendSpace(5)
            tooltip_item.AppendTextLine("-" * 41)
            tooltip_item.AppendTextLine("")
            return True
        
        elif event_index == player.SWITCH_AVERAGE_EVENT:
            tooltip_item.AppendTextLine(localeInfo.SWITCH_AVERAGE_EVENT)
            tooltip_item.AppendTextLine(localeInfo.SWITCH_AVERAGE_EVENT_TEXT)
            tooltip_item.AppendSpace(5)
            tooltip_item.AppendTextLine(
                self._calculate_time(event_index, event_data[EVENT_START_TEXT], 
                                   event_data[EVENT_END_TEXT], event_data[EVENT_END_TIME])
            )
            tooltip_item.AppendSpace(5)
            tooltip_item.AppendTextLine("-" * 41)
            return True
        
        elif event_index == player.SWITCH_RUNE_EVENT:
            tooltip_item.AppendTextLine(localeInfo.SWITCH_RUNE_EVENT)
            tooltip_item.AppendTextLine(localeInfo.SWITCH_RUNE_EVENT_TEXT)
            tooltip_item.AppendSpace(5)
            tooltip_item.AppendTextLine(
                self._calculate_time(event_index, event_data[EVENT_START_TEXT], 
                                   event_data[EVENT_END_TEXT], event_data[EVENT_END_TIME])
            )
            tooltip_item.AppendSpace(5)
            tooltip_item.AppendTextLine("-" * 41)
            return True
        
        return False


class EventDayPanel(ui.Window):

    PANEL_WIDTH  = 220
    PANEL_HEIGHT = 373
    ROW_HEIGHT   = 50
    BTN_WIDTH    = 47
    BTN_HEIGHT   = 20

    def __init__(self, parent):
        super(EventDayPanel, self).__init__()
        self.SetParent(parent)
        self.SetSize(self.PANEL_WIDTH, self.PANEL_HEIGHT)
        self.SetPosition(591, 54)
        self.AddFlag("float")

        self._create_background()
        self._title_text = None
        self._rows = []
        self._day_index = 0

    def __del__(self):
        super(EventDayPanel, self).__del__()

    def _create_background(self):
        bg = ui.Bar()
        bg.SetParent(self)
        bg.AddFlag("not_pick")
        bg.SetPosition(0, 0)
        bg.SetSize(self.PANEL_WIDTH, self.PANEL_HEIGHT)
        bg.SetColor(0x8833210d)
        bg.Show()
        self._bg = bg

        border = ui.Bar()
        border.SetParent(self)
        border.AddFlag("not_pick")
        border.SetPosition(0, 0)
        border.SetSize(self.PANEL_WIDTH, 1)
        border.SetColor(0x886e502e)
        border.Show()
        self._border = border

    def Destroy(self):
        self._clear_rows()
        self._title_text = None
        self._bg = None
        self._border = None

    def _clear_rows(self):
        self._rows = []

    def Load(self, day_index, month, year):
        self._day_index = day_index
        self._clear_rows()

        if self._title_text is None:
            title = ui.TextLine()
            title.SetParent(self)
            title.AddFlag("not_pick")
            title.SetHorizontalAlignCenter()
            title.SetPosition(self.PANEL_WIDTH / 2, 8)
            title.SetFontName("Tahoma:14b")
            title.SetOutline()
            title.Show()
            self._title_text = title

        self._title_text.SetText(
            "|cFFc79e65%04d-%02d-%02d|r" % (year, month, day_index)
        )

        if day_index not in server_event_data:
            self._add_no_events_label()
            return

        event_dict = server_event_data[day_index]
        events = sorted(event_dict.values(), key=lambda x: x[EVENT_START_TIME])

        y_offset = 34
        for event_data in events:
            row = self._create_event_row(event_data, y_offset)
            if row:
                self._rows.append(row)
                y_offset += self.ROW_HEIGHT

    def _add_no_events_label(self):
        label = ui.TextLine()
        label.SetParent(self)
        label.AddFlag("not_pick")
        label.SetHorizontalAlignCenter()
        label.SetPosition(self.PANEL_WIDTH / 2, 60)
        label.SetText("|cFF888888No events|r")
        label.SetOutline()
        label.Show()
        self._rows.append({"label": label})

    def _create_event_row(self, event_data, y):
        event_index = event_data[EVENT_INDEX]
        event_id = event_data[EVENT_ID]

        if event_index not in events_default_data:
            return None

        row = {}

        # Icon
        icon = ui.ImageBox()
        icon.SetParent(self)
        icon.AddFlag("not_pick")
        icon_path = IMG_ICON_DIR + events_default_data[event_index][0] + ".tga"
        icon.LoadImage(icon_path)
        icon.SetPosition(6, y + 4)
        icon.Show()
        row["icon"] = icon

        # Name
        name_text = ui.TextLine()
        name_text.SetParent(self)
        name_text.AddFlag("not_pick")
        name_text.SetPosition(70, y + 21)
        event_name = events_default_data[event_index][1]
        if len(event_name) > 18:
            event_name = event_name[:17] + ".."
        name_text.SetText(event_name)
        name_text.Show()
        row["name"] = name_text

        # Start button
        btn = ui.Button()
        btn.SetParent(self)
        btn.SetUpVisual("elvion/game/eventcalendar/btn_norm.png")
        btn.SetOverVisual("elvion/game/eventcalendar/btn_hover.png")
        btn.SetDownVisual("elvion/game/eventcalendar/btn_down.png")
        btn.SetPosition(self.PANEL_WIDTH - self.BTN_WIDTH - 6, y + (self.ROW_HEIGHT - self.BTN_HEIGHT) / 2)
        btn.SetSize(self.BTN_WIDTH, self.BTN_HEIGHT)
        self._apply_button_state(btn, event_id)
        btn.Show()
        row["btn"] = btn
        row["event_id"] = event_id
        return row

    def _get_event_duration(self, event_id):
        for day_events in server_event_data.values():
            if event_id in day_events:
                val0 = day_events[event_id][EVENT_VALUE0]
                if val0 > 0:
                    return val0
                break
        return EVENT_PERSONAL_DURATION

    def _apply_button_state(self, btn, event_id):
        import time
        activation_time = personal_activated_events.get(event_id)
        if activation_time is None:
            btn.Enable()
            btn.SetText("|cFF7b6959Start|r")
            btn.SetEvent(ui.__mem_func__(self._on_start_click), event_id)
        elif (time.time() - activation_time) < self._get_event_duration(event_id):
            btn.SetText("|cFF7b8d51Active|r")
            btn.Disable()
        else:
            btn.SetText("|cFF3e3a34Expired|r")
            btn.Disable()

    def _on_start_click(self, event_id):
        net.SendEventActivatePacket(event_id)

    def RefreshButtons(self):
        for row in self._rows:
            btn = row.get("btn")
            event_id = row.get("event_id")
            if btn is None or event_id is None:
                continue
            self._apply_button_state(btn, event_id)

    def OnActivateResult(self, event_id, result):
        import time
        EVENT_ACTIVATE_SUCCESS = 0
        for row in self._rows:
            if row.get("event_id") != event_id:
                continue
            btn = row.get("btn")
            if not btn:
                continue
            if result == EVENT_ACTIVATE_SUCCESS:
                self._apply_button_state(btn, event_id)
            else:
                btn.SetText("|cFF888888Expired|r")
                btn.Disable()
            break


class PersonalEventTracker(ui.Window):
    PANEL_WIDTH  = 255
    ROW_HEIGHT   = 26
    ICON_SIZE    = 20
    PADDING      = 5
    TITLE_HEIGHT = 20

    def __init__(self):
        super(PersonalEventTracker, self).__init__()
        self.AddFlag("float")
        self._rows = []
        self._last_tick = 0.0
        self._bg = None
        self._title = None
        self._dragging = False
        self._drag_ox = 0
        self._drag_oy = 0
        self._build_chrome()

    def __del__(self):
        self._rows = []
        super(PersonalEventTracker, self).__del__()

    def OnMouseLeftButtonDown(self):
        mx, my = wndMgr.GetMousePosition()
        l, t, r, b = self.GetRect()
        if my - t <= self.TITLE_HEIGHT:
            self._dragging = True
            self._drag_ox = mx - l
            self._drag_oy = my - t
        return True

    def OnMouseLeftButtonUp(self):
        if self._dragging:
            global _tracker_saved_pos
            l, t, r, b = self.GetRect()
            _tracker_saved_pos = (l, t)
        self._dragging = False
        return True

    def _build_chrome(self):
        bg = ui.Bar()
        bg.SetParent(self)
        bg.SetPosition(0, 0)
        bg.SetColor(0x8833210d)
        bg.AddFlag("not_pick")
        bg.Show()
        self._bg = bg

        title = ui.TextLine()
        title.SetParent(self)
        title.AddFlag("not_pick")
        title.SetPosition(self.PADDING, 3)
        title.SetFontName("Verdana:12b")
        title.SetOutline()
        title.SetText(localeInfo.EVENTCALENDAR_PERSONAL_EVENTS)
        title.SetPackedFontColor(0xFFcabdab)
        title.Show()
        self._title = title

    def _clear_rows(self):
        self._rows = []

    def _get_event_duration(self, event_id):
        for day_events in server_event_data.values():
            if event_id in day_events:
                val0 = day_events[event_id][EVENT_VALUE0]
                if val0 > 0:
                    return val0
                break
        return EVENT_PERSONAL_DURATION

    def _get_event_index(self, event_id):
        for day_events in server_event_data.values():
            if event_id in day_events:
                return day_events[event_id][EVENT_INDEX]
        return None

    def Refresh(self):
        import time
        self._clear_rows()
        now = time.time()

        active = []
        for event_id, activation_time in personal_activated_events.items():
            duration = self._get_event_duration(event_id)
            if (now - activation_time) < duration:
                active.append((event_id, activation_time, duration))

        if not active:
            self.Hide()
            return

        total_height = self.TITLE_HEIGHT + self.PADDING + len(active) * self.ROW_HEIGHT + self.PADDING
        self.SetSize(self.PANEL_WIDTH, total_height)
        self._bg.SetSize(self.PANEL_WIDTH, total_height)

        for i, (event_id, activation_time, duration) in enumerate(active):
            y = self.TITLE_HEIGHT + self.PADDING + i * self.ROW_HEIGHT
            row = self._build_row(event_id, activation_time, duration, y)
            self._rows.append(row)

        self.Show()

    def _build_row(self, event_id, activation_time, duration, y):
        row = {"event_id": event_id, "activation_time": activation_time, "duration": duration}

        # Icon
        event_index = self._get_event_index(event_id)
        icon = ui.ImageBox()
        icon.SetParent(self)
        icon.AddFlag("not_pick")
        if event_index is not None and event_index in events_default_data:
            try:
                icon.LoadImage("%s%s.tga" % (IMG_DIR, events_default_data[event_index][0]))
            except:
                pass
        icon.SetPosition(self.PADDING, y + (self.ROW_HEIGHT - self.ICON_SIZE) // 2)
        icon.Show()
        row["icon"] = icon

        # Name
        name_text = ui.TextLine()
        name_text.SetParent(self)
        name_text.AddFlag("not_pick")
        name_text.SetPosition(self.PADDING + self.ICON_SIZE + 4 - 24, y + 4)
        event_name = ""
        if event_index is not None and event_index in events_default_data:
            event_name = events_default_data[event_index][1]
        name_text.SetText(event_name)
        name_text.SetPackedFontColor(0xFFFFFFFF)
        name_text.Show()
        row["name"] = name_text

        timer = ui.TextLine()
        timer.SetParent(self)
        timer.AddFlag("not_pick")
        timer.SetPosition(self.PANEL_WIDTH - 52, y + 4)
        timer.Show()
        row["timer"] = timer

        self._update_row_timer(row)
        return row

    def _update_row_timer(self, row):
        import time
        remaining = int(row["activation_time"] + row["duration"] - time.time())
        if remaining > 0:
            row["timer"].SetText("%02d:%02d |Eemoji/time|e" % (remaining // 60, remaining % 60))
            row["timer"].SetPackedFontColor(0xFFc79e65)
        else:
            row["timer"].SetText("Expired")
            row["timer"].SetPackedFontColor(0xFF888888)

    def OnUpdate(self):
        if self._dragging:
            mx, my = wndMgr.GetMousePosition()
            self.SetPosition(mx - self._drag_ox, my - self._drag_oy)

        import time
        now = time.time()
        if now - self._last_tick < 1.0:
            return
        self._last_tick = now

        any_active = False
        for row in self._rows:
            remaining = int(row["activation_time"] + row["duration"] - now)
            if remaining > 0:
                any_active = True
                row["timer"].SetText("%02d:%02d |Eemoji/time|e" % (remaining // 60, remaining % 60))
                row["timer"].SetPackedFontColor(0xFFc79e65)
            else:
                row["timer"].SetText("Expired")
                row["timer"].SetPackedFontColor(0xFF888888)

        if not any_active:
            self.Refresh()


class MovableImage(ImageBoxSpecial):
    
    def __init__(self):
        super(MovableImage, self).__init__(is_mini_icon=True)
        self._initialize_movable_attributes()
        self._create_ui_elements()
    
    def __del__(self):
        super(MovableImage, self).__del__()
    
    def _initialize_movable_attributes(self):
        self.window = None
        self.event_cache = []
        self.time_list = []
        self.time_text = None
        self.time_text_ex = None
    
    def Destroy(self):
        self.window = None
        self.event_cache = []
        self.time_list = []
        self.time_text = None
        super(MovableImage, self).Destroy()
    
    def _create_ui_elements(self):
        # Create window container
        window = ui.Window()
        window.SetParent(self)
        window.AddFlag("not_pick")
        window.OnUpdate = ui.__mem_func__(self.OnUpdate)
        window.Show()
        self.window = window
        
        # Create time text
        time_text = ui.TextLine()
        time_text.SetParent(self)
        time_text.AddFlag("not_pick")
        time_text.SetHorizontalAlignCenter()
        time_text.SetPosition(45, 55+31)
        time_text.SetOutline()
        time_text.Show()
        self.time_text = time_text
        
        # Create additional time text
        time_text_ex = ui.TextLine()
        time_text_ex.SetParent(self)
        time_text_ex.AddFlag("not_pick")
        time_text_ex.SetHorizontalAlignCenter()
        time_text_ex.SetPosition(45, 73+31)
        time_text_ex.SetText(localeInfo.BONUS_NEXT_EVENT)
        time_text_ex.SetOutline()
        time_text_ex.Show()
        self.time_text_ex = time_text_ex
    
    def Clear(self):
        self.event_cache = []
        self.time_list = []
        self.time_text.SetText("")
        self.time_text_ex.Hide()
        super(MovableImage, self).Clear()
        self.Hide()
    
    def Refresh(self):
        self.Clear()
        for day_index, event_list in server_event_data.iteritems():
            for event_id, event_data in event_list.iteritems():
                self.AppendEvent(
                    event_data[EVENT_ID], event_data[EVENT_START_TIME],
                    event_data[EVENT_END_TIME], event_data[EVENT_IS_ACTIVE]
                )
    
    def LoadTime(self, event_id, start_time, end_time, is_already_start):
        self.AppendImage(event_id)
        self.time_list.append([start_time, end_time, is_already_start])
        
        if len(self.time_list) > 1:
            self.time_text_ex.Show()
        else:
            self.time_text_ex.Hide()
        
        self.Show()
    
    def CheckCacheEvent(self):
        if len(self.event_cache) == 0:
            return
        
        client_global_time = app.GetGlobalTimeStamp()
        for j in xrange(len(self.event_cache)):
            start_time = self.event_cache[j][1] - client_global_time
            if 0 <= start_time <= (60 * 30):  # 30 minutes
                self.Refresh()
                return
    
    def AppendEvent(self, event_id, start_time, end_time, is_already_start):
        client_global_time = app.GetGlobalTimeStamp()
        
        new_start_time = start_time - client_global_time if start_time != 0 else 0
        new_end_time = end_time - client_global_time if end_time != 0 else 0
        
        if self._is_permanent_event(start_time, end_time, new_start_time, is_already_start):
            self.LoadTime(event_id, start_time, end_time, 2)
        elif new_start_time <= 0 and new_end_time <= 0:
            return
        elif 0 < new_start_time <= (60 * 30):
            self.LoadTime(event_id, start_time, end_time, 0)
        elif new_end_time > 0 and is_already_start:
            self.LoadTime(event_id, start_time, end_time, 1)
        else:
            self.event_cache.append([event_id, start_time, end_time, is_already_start])
        
        if len(self.time_list) > 1:
            self.time_text_ex.Show()
        else:
            self.time_text_ex.Hide()
    
    def _is_permanent_event(self, start_time, end_time, new_start_time, is_already_start):
        return ((start_time != 0 and new_start_time <= 0 and end_time == start_time and is_already_start) or
                (start_time != 0 and new_start_time <= 0 and end_time == 0 and is_already_start))
    
    def DeleteEvent(self, index):
        self.DeleteImage(index)
        if 0 <= index < len(self.time_list):
            del self.time_list[index]
        
        if len(self.time_list) <= 1:
            self.time_text_ex.Hide()
    
    def FormatTime(self, seconds):
        if seconds == 0:
            return ""
        
        m, s = divmod(seconds, 60)
        h, m = divmod(m, 60)
        return "%02dh %02dm %02ds" % (h, m, s)
    
    def OnUpdate(self):
        super(MovableImage, self).OnUpdate()
        self.CheckCacheEvent()
        
        if self.image_index < len(self.time_list):
            time_data = self.time_list[self.image_index]
            self._update_time_display(time_data)
    
    def _update_time_display(self, time_data):
        if time_data[2] == 0:  # Event starting soon
            left_time = time_data[0] - app.GetGlobalTimeStamp()
            if left_time > 0:
                self.time_text.SetText("Začne za: " + localeInfo.BONUS_START_IN % self.FormatTime(left_time))
                if not self.time_text.IsShow():
                    self.time_text.Show()
        
        elif time_data[2] == 1:  # Event ending soon
            left_time = time_data[1] - app.GetGlobalTimeStamp()
            if left_time > 0:
                self.time_text.SetText(
                    "{} ".format(emoji.AppendEmoji("icon/emoji/image-example-007.png")) +
                    localeInfo.BONUS_END_IN % self.FormatTime(left_time)
                )
                self.time_text.SetFontName("Tahoma:14")
                if not self.time_text.IsShow():
                    self.time_text.Show()
        
        elif time_data[2] == 2:  # Permanent event
            if self.time_text.IsShow():
                self.time_text.Hide()


def CalculateDayCount(month, year):
    return EventDataManager.calculate_day_count(month, year)

def SetEventStatus(eventID, eventStatus, endTime, endTimeText):
    EventDataManager.set_event_status(eventID, eventStatus, endTime, endTimeText)

def SetServerData(dayIndex, eventID, eventIndex, startTime, endTime, empireFlag, 
                 channelFlag, value0, value1, value2, value3, startRealTime, 
                 endRealTime, isAlreadyStart):
    EventDataManager.set_server_data(dayIndex, eventID, eventIndex, startTime, endTime, 
                                   empireFlag, channelFlag, value0, value1, value2, value3, 
                                   startRealTime, endRealTime, isAlreadyStart)

def IsEventIDActive(eventID):
    return EventDataManager.is_event_id_active(eventID)

def IsEventIndexActive(eventIndex):
    return EventDataManager.is_event_index_active(eventIndex)

def GetEventIndexData(eventIndex):
    return EventDataManager.get_event_index_data(eventIndex)

def GetEventIDData(eventID):
    return EventDataManager.get_event_id_data(eventID)
	