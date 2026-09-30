import ui
import wndMgr
import app
import player
import localeInfo
import item

GOLD_COLOR = 0xFFFEE3AE

WIDTH = 150
HEIGHT = 80

BG_PATH = "elvion/game/notifications/popup_bg.png"


def SplitDescription(desc, limit):
    total_tokens = desc.split()
    line_tokens = []
    line_len = 0
    lines = []

    for token in total_tokens:
        if "|" in token:
            sep_pos = token.find("|")
            line_tokens.append(token[:sep_pos])
            lines.append(" ".join(line_tokens))
            line_len = len(token) - (sep_pos + 1)
            line_tokens = [token[sep_pos + 1:]]
        else:
            line_len += len(token)
            if len(line_tokens) + line_len > limit:
                lines.append(" ".join(line_tokens))
                line_len = len(token)
                line_tokens = [token]
            else:
                line_tokens.append(token)

    if line_tokens:
        lines.append(" ".join(line_tokens))

    return lines


class NotificationWindow(ui.ScriptWindow):
    def __init__(self):
        ui.ScriptWindow.__init__(self)
        self.SetSize(WIDTH, HEIGHT)
        self.UI = {}
        self.isActiveSlide = False
        self.isActiveSlideOut = False
        self.endTime = 0
        self.wndWidth = 0
        self.queue = []
        self.__LoadWindow()

    def __LoadWindow(self):
        screenHeight = wndMgr.GetScreenHeight()

        self.SetPosition(-WIDTH, screenHeight / 2 - 200)
        self.UI["bg"] = ui.MakeImageBox(self, BG_PATH, 0, 0)
        self.UI["bg"].SetTop()
        self.UI["title"] = ui.MakeText(self.UI["bg"], "", 0, 57, None)
        self.UI["title"].SetWindowHorizontalAlignCenter()
        self.UI["title"].SetHorizontalAlignCenter()
        self.UI["title"].SetPackedFontColor(GOLD_COLOR)

    def AddToQueue(self, category, value, additionalValue):
        notification = Notification(category, value, additionalValue)
        self.queue.append(notification)
        self.SlideIn()

    def LoadInfo(self, notification):
        self.UI["title"].SetText(notification.GetTitle())
        self.UI["icon"] = ui.MakeImageBox(self.UI["bg"], notification.GetIcon(), 0, 18)
        self.UI["icon"].SetWindowHorizontalAlignCenter()

        lines = SplitDescription(notification.GetDescription(), 35)
        stepY = 74 if len(lines) > 1 else 82
        self.UI["description"] = []

        for count, line in enumerate(lines):
            description = ui.MakeText(self.UI["bg"], line, 0, stepY, None)
            description.SetWindowHorizontalAlignCenter()
            description.SetHorizontalAlignCenter()
            self.UI["description"].append(description)
            stepY += 15

    def SlideIn(self):
        self.LoadInfo(self.queue[0])
        self.SetTop()
        self.Show()
        self.isActiveSlide = True
        self.endTime = app.GetGlobalTimeStamp() + 5

    def Close(self):
        self.Hide()

    def OnUpdate(self):
        if self.isActiveSlide:
            x, y = self.GetLocalPosition()
            if x < 0:
                self.SetPosition(x + 10, y)

        if self.endTime - app.GetGlobalTimeStamp() <= 0 and not self.isActiveSlideOut and self.isActiveSlide:
            self.isActiveSlide = False
            self.isActiveSlideOut = True

        if self.isActiveSlideOut:
            x, y = self.GetLocalPosition()
            if x > -WIDTH:
                self.SetPosition(x - 10, y)

            if x <= -WIDTH:
                self.isActiveSlideOut = False
                del self.queue[0]
                self.Close()

                if len(self.queue) > 0:
                    self.SlideIn()


class Notification:
    NOTIFICATION_MAP = {
        player.NOTIFICATION_CATEGORY_SPECIAL_ITEM_EXPIRED: {
            "description": localeInfo.NOTIFICATION_CATEGORY_SPECIAL_ITEM_EXPIRED_DESCRIPTION,
        },
        player.NOTIFICATION_CATEGORY_BIOLOG_MISSION_READY: {
            "description": localeInfo.NOTIFICATION_CATEGORY_BIOLOG_MISSION_READY_DESCRIPTION,
        },
        player.NOTIFICATION_CATEGORY_ALCHEMY_EXPIRED: {
            "description": localeInfo.NOTIFICATION_CATEGORY_ALCHEMY_EXPIRED_DESCRIPTION,
        },
        player.NOTIFICATION_CATEGORY_MELT_EXPIRED: {
            "description": localeInfo.NOTIFICATION_CATEGORY_MELT_EXPIRED_DESCRIPTION,
        },
    }

    def __init__(self, category, value, additionalValue):
        self.category = category
        self.value = value
        self.additionalValue = additionalValue

    def GetDescription(self):
        return self.NOTIFICATION_MAP.get(self.category, {}).get("description", "")

    def GetTitle(self):
        if player.NOTIFICATION_CATEGORY_SPECIAL_ITEM_EXPIRED == self.category:
            item.SelectItem(self.value)
            return item.GetItemName()
        elif player.NOTIFICATION_CATEGORY_BIOLOG_MISSION_READY == self.category:
            item.SelectItem(self.value)
            return item.GetItemName()
        elif player.NOTIFICATION_CATEGORY_ALCHEMY_EXPIRED == self.category:
            item.SelectItem(self.value)
            return item.GetItemName()
        elif player.NOTIFICATION_CATEGORY_MELT_EXPIRED == self.category:
            item.SelectItem(self.value)
            return item.GetItemName()

        return self.NOTIFICATION_MAP.get(self.category, {}).get("title", "")

    def GetIcon(self):
        if player.NOTIFICATION_CATEGORY_SPECIAL_ITEM_EXPIRED == self.category:
            item.SelectItem(self.value)
            return item.GetIconImageFileName()
        elif player.NOTIFICATION_CATEGORY_BIOLOG_MISSION_READY == self.category:
            item.SelectItem(self.value)
            return item.GetIconImageFileName()
        elif player.NOTIFICATION_CATEGORY_ALCHEMY_EXPIRED == self.category:
            item.SelectItem(self.value)
            return item.GetIconImageFileName()
        elif player.NOTIFICATION_CATEGORY_MELT_EXPIRED == self.category:
            item.SelectItem(self.value)
            return item.GetIconImageFileName()

        return self.NOTIFICATION_MAP.get(self.category, {}).get("icon", "")
