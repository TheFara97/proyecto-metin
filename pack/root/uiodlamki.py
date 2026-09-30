#-*- coding: iso-8859-1 -*-
import ui
import exception
import player
import mouseModule
import chat
import emoji
import uiInventory
import item
import net
import uiToolTip
import constInfo
import localeInfo
from _weakref import proxy

class OdlamkiWindow(ui.ScriptWindow):
    def __init__(self):
        self.tooltipItem = None
        self.isLoaded = 0
        self.__itemPos = [[player.INVENTORY, -1] for _ in xrange(18)]
        self.__grid = None
        self.wndInventory = uiInventory.InventoryWindow()
        ui.ScriptWindow.__init__(self)
        self.__LoadWindow()

    def __del__(self):
        ui.ScriptWindow.__del__(self)

    def Show(self):
        if self.isLoaded:
            ui.ScriptWindow.Show(self)
        else:
            ui.ScriptWindow.Show(self)

    def Open(self):
        self.Show()
        self.RefreshSlot()

    def BindInterfaceClass(self, interface):
        self.interface = interface

    def SetItemToolTip(self, itemToolTip):
        self.tooltipItem = proxy(itemToolTip)

    def __LoadWindow(self):
        if self.isLoaded == 1:
            return
            
        self.isLoaded = 1
        try:
            ui.PythonScriptLoader().LoadScriptFile(self, "uiscript/odlamkiwindow.py")
        except:
            exception.Abort("OdlamkiWindow.LoadDialog.LoadScript")

        try:
            self.titleBar = self.GetChild("TitleBar")
            self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
            self.text = self.GetChild("fast_add")
            self.text.SetText(emoji.AppendEmoji("icon/emoji/key_shift.png")+" + "+emoji.AppendEmoji("icon/emoji/key_rclick.png")+"  Rychlé pøidání")

            self.text.SetText(localeInfo.STONE_CRAFTING_QUICK_ADD.format(emoji.AppendEmoji("icon/emoji/key_shift.png"),
                                                               emoji.AppendEmoji("icon/emoji/key_rclick.png")))           
            self.wndItem = self.GetChild("ItemSlot")

            self.wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
            self.wndItem.SetUseSlotEvent(ui.__mem_func__(self.SelectItemSlot))
            self.wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
            self.wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
            self.wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

            t = uiToolTip.ToolTipNew
            self.tt = {
				0:t(190),
			}
            self.tt[0].AppendSpace(10)
            self.tt[0].AppendTextLine(emoji.AppendEmoji("icon/emoji/28961.png"))
            self.tt[0].AppendSpace(10)
            self.tt[0].AppendTextLine(localeInfo.STONE_CRAFTING_DESC.format(5,2))
            self.tt[0].AppendSpace(3)
            self.tt[0].AppendTextLine(localeInfo.STONE_CRAFTING_DESC.format(6,3))
            self.tt[0].AppendSpace(3)
            self.tt[0].AppendTextLine(localeInfo.STONE_CRAFTING_DESC.format(7,4))
            self.tt[0].AppendSpace(3)
            self.tt[0].AppendTextLine(localeInfo.STONE_CRAFTING_DESC.format(8,5))
            self.tt[0].AppendSpace(3)
            self.tt[0].AppendTextLine(localeInfo.STONE_CRAFTING_DESC.format(9,6))
            self.tt[0].AppendSpace(6)

            self.info = self.GetChild("info")

            self.AcceptButton = self.GetChild("AcceptButton")
            self.AcceptButton.SetEvent(ui.__mem_func__(self.Send))

            self.__grid = Grid(6,3)
        except:
            exception.Abort("OdlamkiWindow.LoadDialog.BindObject")
        
        self.SetCenterPosition()
        self.SetTop()

    def Destroy(self):
        self.Hide()
        self.ClearDictionary()

    def Close(self):
        if self.tt[0]:
            self.tt[0].Hide()
        self.Hide()

    def OnPressEscapeKey(self):
        self.Close()
        return TRUE

    def Hide(self):
        ui.ScriptWindow.Hide(self)

        if self.tooltipItem:
            if self.tooltipItem.IsShow():
                self.tooltipItem.HideToolTip()

    def OnUpdate(self):
        if self.info.IsIn():
            self.tt[0].Show()
        else:
            self.tt[0].Hide()

    def OverInItem(self, slotIndex):
        if self.tooltipItem:
            self.tooltipItem.SetInventoryItem(self.__itemPos[slotIndex][1], self.__itemPos[slotIndex][0])

    def OverOutItem(self):
        if self.tooltipItem:
            self.tooltipItem.HideToolTip()

    def SelectEmptySlot(self, slotIndex):
        isAttached = mouseModule.mouseController.isAttached()
        if isAttached:
            attachedSlotType = mouseModule.mouseController.GetAttachedType()
            attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
            attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
            mouseModule.mouseController.DeattachObject()

            if player.SLOT_TYPE_INVENTORY != attachedSlotType and player.SLOT_TYPE_STONE_INVENTORY != attachedSlotType:
                return

            item.SelectItem(player.GetItemIndex(attachedInvenType, attachedSlotPos))

            itemType = item.GetItemType()
            if not item.ITEM_TYPE_METIN == itemType:
                chat.AppendChat(chat.CHAT_TYPE_INFO, "Mo¿esz w³o¿yæ tylko kamienie dusz!")
                return

            (width, height) = item.GetItemSize()

            if not self.__grid.is_empty(slotIndex, width, height):
                return

            itemPos = [attachedInvenType, attachedSlotPos]
            if itemPos in self.__itemPos:
                return

            self.__itemPos[slotIndex] = itemPos

            self.__grid.put(slotIndex, width, height)

            self.RefreshSlot()

            constInfo.itemOdlamkiGrid.append([slotIndex, attachedSlotPos])
            constInfo.listOdlamkiItem.append(attachedSlotPos)
            self.wndInventory.RefreshBagSlotWindow()

    def SelectItemSlot(self, slotIndex):
        isAttached = mouseModule.mouseController.isAttached()
        if isAttached:
            mouseModule.mouseController.DeattachObject()
            return

        item.SelectItem(mouseModule.mouseController.GetAttachedItemIndex())
        (width, height) = item.GetItemSize()

        self.__grid.clear(slotIndex, width, height)
        
        self.__itemPos[slotIndex][0] = player.INVENTORY
        self.__itemPos[slotIndex][1] = -1

        for removeGrid in constInfo.itemOdlamkiGrid:
            if slotIndex == removeGrid[0]:
                constInfo.itemOdlamkiGrid.remove(removeGrid)
                constInfo.listOdlamkiItem.remove(removeGrid[1])
                self.wndInventory.RefreshBagSlotWindow()

        self.__grid.reset()
        self.RefreshSlot()

    def RefreshSlot(self):
        if not self.isLoaded:
            return

        for i in xrange(self.wndItem.GetSlotCount()):
            itemVnum = player.GetItemIndex(self.__itemPos[i][0], self.__itemPos[i][1])
            itemCount = player.GetItemCount(self.__itemPos[i][0], self.__itemPos[i][1])

            if not itemCount:
                self.wndItem.ClearSlot(i)
                continue

            self.wndItem.SetItemSlot(i, itemVnum, 0 if itemCount == 1 else itemCount)

        self.wndItem.RefreshSlot()

    def AppendSlot(self, wnd, cell):
        itemVnum = player.GetItemIndex(wnd, cell)
        if not itemVnum:
            return

        itemPos = [wnd, cell]
        if itemPos in self.__itemPos:
            return

        item.SelectItem(itemVnum)
        (width, height) = item.GetItemSize()

        pos = self.__grid.find_blank(width, height)

        if pos != -1:
            self.__itemPos[pos] = itemPos

            self.__grid.put(pos, width, height)

            self.RefreshSlot()

            constInfo.itemOdlamkiGrid.append([pos, cell])
            constInfo.listOdlamkiItem.append(cell)
            self.wndInventory.RefreshBagSlotWindow()

    def Send(self):
        out = []

        for slot in self.__itemPos:
            if slot[0] == player.INVENTORY and slot[1] == -1:
                continue

            out.append(slot)

        net.SendOdlamki(out)
        constInfo.listOdlamkiItem = []
        self.wndInventory.RefreshBagSlotWindow()

    def ClearWindow(self):
        self.__itemPos = [[player.INVENTORY, -1] for _ in xrange(18)]
        self.__grid.reset()
        self.RefreshSlot()
        constInfo.itemOdlamkiGrid = []
        constInfo.listOdlamkiItem = []
        if self.wndInventory:
            self.wndInventory.RefreshBagSlotWindow()

class Grid:
    def __init__(self, width, height):
        self.grid = [False] * (width * height)
        self.width = width
        self.height = height

    def __str__(self):
        output = "Grid {}x{} Information\n".format(self.width, self.height)
        for row in range(self.height):
            for col in range(self.width):
                output += "Status of %d: " % (row * self.width + col)
                output += "NotEmpty, " if self.grid[row *
                                                   self.width + col] else "Empty, "
            output += "\n"

        return output

    def find_blank(self, width, height):
        if width > self.width or height > self.height:
            return -1

        for row in range(self.height):
            for col in range(self.width):
                index = row * self.width + col
                if self.is_empty(index, width, height):
                    return index

        return -1

    def put(self, pos, width, height):
        if not self.is_empty(pos, width, height):
            return False

        for row in range(height):
            start = pos + (row * self.width)
            self.grid[start] = True
            col = 1
            while col < width:
                self.grid[start + col] = True
                col += 1

        return True

    def clear(self, pos, width, height):
        if pos < 0 or pos >= (self.width * self.height):
            return

        for row in range(height):
            start = pos + (row * self.width)
            self.grid[start] = False
            col = 1
            while col < width:
                self.grid[start + col] = False
                col += 1

    def is_empty(self, pos, width, height):
        if pos < 0:
            return False

        row = pos // self.width
        if (row + height) > self.height:
            return False

        if (pos + width) > ((row * self.width) + self.width):
            return False

        for row in range(height):
            start = pos + (row * self.width)
            if self.grid[start]:
                return False

            col = 1
            while col < width:
                if self.grid[start + col]:
                    return False
                col += 1

        return True

    def get_size(self):
        return self.width * self.height

    def reset(self):
        self.grid = [False] * (self.width * self.height)
