import ui
import app
import player
import eventManager
import time
import localeInfo
import updateable
import renderTarget

from ui_event import Event

class WorldBossData:
	def __init__(self, vnum):
		self.vnum = vnum
		self.alive = False
		self.respawn_time = 0
		self.map_index = 0
		self.x = 0
		self.y = 0
		self.ranking = []
		self.personal = None

	def update_day_info(self, alive, respawn_time, map_index=0, x=0, y=0):
		self.alive = alive
		self.respawn_time = respawn_time
		self.map_index = map_index
		self.x = x
		self.y = y

	def add_ranking_entry(self, rank, name, damage, timestamp):
		self.ranking.append({
			"rank": rank,
			"name": name,
			"damage": damage,
			"timestamp": timestamp
		})

	def set_personal_entry(self, name, damage, timestamp):
		self.personal = {
			"name": name,
			"damage": damage,
			"timestamp": timestamp
		}

	def clear_ranking(self):
		del self.ranking[:]

		self.personal = None

class WorldBossController(ui.SimplyWindow):
	def __init__(self, *args, **kwargs):
		super(WorldBossController, self).__init__(
			"UI", ("movable", "float"),
			645, 375,
			self.__Initialize, self.__Destroy
		)

	def __Initialize(self):
		import dbg
		#dbg.TraceError("WorldBossController.__Initialize() called")
		self.windowConfig["user-defines"] = {
			"data" : {}
		}
		self.SetSize(*self.windowConfig["size"].values())
		
		self.respawn_seconds_remaining = 0
		self.current_boss = None

		board = self.__CreateBoard()
		self.tab = self.__CreateTabs(board)
		self.renderTarget = self.__CreateRenderTarget(board)
		
		self.titleBar = ui.TitleBar()
		self.titleBar.SetParent(self)
		self.titleBar.MakeTitleBar(0, "red")
		self.titleBar.SetPosition(22, 9)
		self.titleBar.SetWidth(598)
		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
		self.titleBar.Show()

		self.titleName = ui.TextLine()
		self.titleName.SetParent(self.titleBar)
		self.titleName.SetPosition(0, 3)
		self.titleName.SetText("Worldboss")
		self.titleName.SetWindowHorizontalAlignCenter()
		self.titleName.SetHorizontalAlignCenter()
		self.titleName.SetOutline()
		self.titleName.Show()

		right_panel_x = self.renderTarget.GetLocalPosition()[0] + self.renderTarget.GetWidth() - 1
		self.headers, self.dungeonImage = self.__CreateHeaders(right_panel_x, board)
		self.__CreateInfoBoxes(right_panel_x, board, self.headers[2], self.dungeonImage)

		self.AppendObject("data", (self.renderTarget, self.headers, self.dungeonImage))

		#dbg.TraceError("Registering event observers")
		eventManager.EventManager().add_observer("WORLD_BOSS_REGISTER_DAY", self.__RegisterDay)
		eventManager.EventManager().add_observer("WORLD_BOSS_REGISTER_RANKING", self.__RegisterRanking)
		eventManager.EventManager().add_observer("WORLD_BOSS_REGISTER_PERSONAL", self.__RegisterPersonal)
		#dbg.TraceError("Event observers registered successfully")

	def __Destroy(self):
		eventManager.EventManager().remove_observer("WORLD_BOSS_REGISTER_DAY", self.__RegisterDay)
		eventManager.EventManager().remove_observer("WORLD_BOSS_REGISTER_RANKING", self.__RegisterRanking)
		eventManager.EventManager().remove_observer("WORLD_BOSS_REGISTER_PERSONAL", self.__RegisterPersonal)

	def Open(self):
		import dbg
		#dbg.TraceError("WorldBossController.Open() called")

		super(WorldBossController, self).Open()

		# Clear stale data so we don't display outdated boss status
		self.windowConfig["user-defines"]["data"] = {}
		self.current_boss = None
		self.respawn_seconds_remaining = 0

		# Request fresh data from server
		import net
		#dbg.TraceError("Requesting world boss data from server")
		net.SendChatPacket("/world_boss_request")

		self.__Refresh()

	def Close(self):
		super(WorldBossController, self).Close()

	def OnKeyDown(self, key):
		if key == app.VK_ESCAPE:
			self.Close()
			return True
		return False

	def __CreateBoard(self):
		board = ui.ImageBox()
		board.SetParent(self)
		board.SetPosition(0, 0)
		#board.SetSize(*self.GetSize())
		board.LoadImage("elvion/game/world-boss/background.png")
		#board.SetTitleName("World Boss")
		#board.SetCloseEvent(Event(self.Close))
		board.Show()
		self.AppendObject("board", board)
		return board

	def __CreateTabs(self, parent):
		tab = ui.TabBoard()
		tab.SetParent(parent)
		tab.SetPosition(10, 30)
		tab.SetSize(parent.GetWidth(), 33)
		tab.SetTabWidth(89)
		tab.SetVisual(
			"elvion/game/world-boss/button-tab-0.png",
			"elvion/game/world-boss/button-tab-1.png",
			"elvion/game/world-boss/button-tab-1.png"
		)
		tab.Show()

		self.day_names = ["Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"]
		for i, day in enumerate(self.day_names):
			tab.AppendTab(day)
			tab.SetTabEvent(day, Event(self.__RequestData, i))

		self.AppendObject("tab", tab)
		return tab

	def __CreateRenderTarget(self, parent):
		tab = self.GetObject("tab")
		renderTarget = ui.RenderTarget()
		renderTarget.SetParent(parent)
		renderTarget.SetPosition(tab.GetLocalPosition()[0], tab.GetLocalPosition()[1] + tab.GetHeight() + 2)
		renderTarget.SetSize(298, 299)
		renderTarget.SetRenderTarget(5)
		renderTarget.Show()
		return renderTarget

	def __CreateHeaders(self, x, parent):
		headers = [ui.TransparentButton() for _ in range(3)]

		headers[0].SetParent(parent)
		headers[0].SetPosition(x, self.tab.GetLocalPosition()[1] + self.tab.GetHeight())
		headers[0].SetUpVisual("elvion/game/world-boss/header-up.png")
		headers[0].Show()
		headers[0].SetText(localeInfo.WORLDBOSS_HEADER_BOSS_INFO)

		image = ui.ImageBox()
		image.SetParent(parent)
		image.SetPosition(x, headers[0].GetLocalPosition()[1] + headers[0].GetHeight() + 3)
		image.LoadImage("elvion/game/world-boss/dungeon-image-1.png")
		image.Show()

		headers[1].SetParent(parent)
		headers[1].SetPosition(x, image.GetLocalPosition()[1] + image.GetHeight() + 3)
		headers[1].SetUpVisual("elvion/game/world-boss/header-status.png")
		headers[1].Show()
		headers[1].SetText(localeInfo.WORLDBOSS_HEADER_TIME_STATUS)

		headers[2].SetParent(parent)
		headers[2].SetPosition(x + headers[1].GetWidth(), headers[1].GetLocalPosition()[1])
		headers[2].SetUpVisual("elvion/game/world-boss/header-ranking.png")
		headers[2].Show()
		headers[2].SetText(localeInfo.WORLDBOSS_HEADER_RANKING)

		return headers, image

	def __CreateInfoBoxes(self, x, parent, header_ranking, image):
		y_pos = header_ranking.GetLocalPosition()[1] + header_ranking.GetHeight()
	
		box_status = ui.MakeImageBox(parent, "elvion/game/world-boss/background-status.png", x, y_pos)
		self.AppendObject("boxes", box_status, True)
	
		self.status_images = []
		self.status_labels = []  # Labels (Status:, Time:)
		self.status_values = []  # Values (Alive/Dead, time)
	
		for i in range(2):
			img = ui.ImageBox()
			img.SetParent(box_status)
			img.LoadImage("elvion/game/world-boss/input-status.png")
			img.SetPosition(0, 10 + i * 35)
			img.SetWindowHorizontalAlignCenter()
			img.Show()
			self.status_images.append(img)
	
		# Create labels (left side)
		label_texts = ["Status:", "Respawn:"]
		for i in range(2):
			label = ui.TextLine()
			label.SetParent(self.status_images[i])
			label.SetPosition(8, 0)
			label.SetText(label_texts[i])
			label.SetWindowVerticalAlignCenter()
			label.SetVerticalAlignCenter()
			label.SetPackedFontColor(0xFFbfa185)  # Label color
			label.SetOutline()
			label.Show()
			self.status_labels.append(label)
	
		# Create values (right side)
		for i in range(2):
			value = ui.TextLine()
			value.SetParent(self.status_images[i])
			value.SetPosition(9, 0)  # Right side
			value.SetText("")
			value.SetWindowHorizontalAlignRight()
			value.SetHorizontalAlignRight()
			value.SetWindowVerticalAlignCenter()
			value.SetVerticalAlignCenter()
			value.SetOutline()
			value.SetPackedFontColor(0xFF7ef472)  # Default white
			value.Show()
			self.status_values.append(value)
	
		self.AppendObject("status-data", (self.status_images, self.status_labels, self.status_values))
	
		teleport_btn = ui.Button()
		teleport_btn.SetParent(box_status)
		teleport_btn.SetPosition(0, 38)
		teleport_btn.SetUpVisual("elvion/game/world-boss/button-teleport-0.png")
		teleport_btn.SetOverVisual("elvion/game/world-boss/button-teleport-0.png")
		teleport_btn.SetDownVisual("elvion/game/world-boss/button-teleport-0.png")
		teleport_btn.SetText("Teleport")
		teleport_btn.SetEvent(ui.__mem_func__(self.__OnClickTeleport))
		teleport_btn.SetWindowHorizontalAlignCenter()
		teleport_btn.SetWindowVerticalAlignBottom()
		teleport_btn.Show()
		self.AppendObject("teleport-button", teleport_btn)
	
		x_pos_next = x + box_status.GetWidth()
		box_ranking = ui.MakeImageBox(parent, "elvion/game/world-boss/background-ranking.png", x_pos_next, y_pos)
		self.AppendObject("boxes", box_ranking)
	
		rank_img = ui.ImageBox()
		rank_img.SetParent(box_ranking)
		rank_img.SetPosition(0, 10)
		rank_img.LoadImage("elvion/game/world-boss/ranking-rows.png")
		rank_img.SetWindowHorizontalAlignCenter()
		rank_img.Show()
		self.AppendObject("ranking-image", rank_img)
	
		self.ranking_lines = []
	
		for i in range(5):
			line = ui.TextLine()
			line.SetParent(rank_img)
			line.SetPosition(45, 2 + i * 20)
			line.SetText("")
			line.Show()
			self.ranking_lines.append(line)
	
		self.AppendObject("ranking-lines", self.ranking_lines)

	def __RequestData(self, day_index):
		self.__UpdateDungeonImage(day_index)
		if day_index not in self.windowConfig["user-defines"]["data"]:
			return

		entries = list(self.windowConfig["user-defines"]["data"][day_index].values())
		if not entries:
			return

		self.__DisplayEntry(entries[0])

	def __Refresh(self):
		renderTarget.SetBackground(5, "elvion/game/world-boss/background-render.png")
		renderTarget.SetVisibility(5, True)
		self.__RequestData(0)

	def __UpdateStatusInfo(self, status_text, time_text, is_alive):
		# Update status value with color
		self.status_values[0].SetText(status_text)
		if is_alive:
			self.status_values[0].SetPackedFontColor(0xFF00FF00)  # Green for alive
		else:
			self.status_values[0].SetPackedFontColor(0xFFFF0000)  # Red for dead
		
		# Update time value with different color
		self.status_values[1].SetText(time_text)
		if is_alive:
			self.status_values[1].SetPackedFontColor(0xFFFFD700)  # Gold when alive
		else:
			self.status_values[1].SetPackedFontColor(0xFFCCCCCC)  # Gray when dead

	def __UpdateDungeonImage(self, index):
		dungeon_images = [
			"elvion/game/world-boss/dungeon-image-1.png",
			"elvion/game/world-boss/dungeon-image-2.png",
			"elvion/game/world-boss/dungeon-image-3.png",
			"elvion/game/world-boss/dungeon-image-4.png",
			"elvion/game/world-boss/dungeon-image-5.png",
			"elvion/game/world-boss/dungeon-image-6.png",
			"elvion/game/world-boss/dungeon-image-7.png",
		]
		self.dungeonImage.LoadImage(dungeon_images[index % len(dungeon_images)])

	def __ConvertDayIndexFromServer(self, server_day_index):
		return (server_day_index - 1) % 7

	def __RegisterDay(self, server_day_index, vnum, is_alive, next_spawn_time, map_index=0, x=0, y=0):
		import dbg
		#dbg.TraceError("[WorldBoss] __RegisterDay: day=%d, vnum=%d, alive=%d, time=%d, map=%d, coords=(%d,%d)" % 
		#	(server_day_index, vnum, is_alive, next_spawn_time, map_index, x, y))
		
		day_index = self.__ConvertDayIndexFromServer(server_day_index)
		
		if day_index not in self.windowConfig["user-defines"]["data"]:
			self.windowConfig["user-defines"]["data"][day_index] = {}
	
		if vnum not in self.windowConfig["user-defines"]["data"][day_index]:
			self.windowConfig["user-defines"]["data"][day_index][vnum] = WorldBossData(vnum)
	
		boss = self.windowConfig["user-defines"]["data"][day_index][vnum]
		boss.update_day_info(is_alive, next_spawn_time, map_index, x, y)
		boss.clear_ranking()
	
		if self.IsShow() and hasattr(self, 'tab') and self.tab.GetSelectedTabIndex() == day_index:
			self.__RequestData(day_index)

	def __RegisterRanking(self, server_day_index, vnum, rank, name, damage, timestamp):
		day_index = self.__ConvertDayIndexFromServer(server_day_index)
		if day_index in self.windowConfig["user-defines"]["data"] and vnum in self.windowConfig["user-defines"]["data"][day_index]:
			self.windowConfig["user-defines"]["data"][day_index][vnum].add_ranking_entry(rank, name, damage, timestamp)

	def __RegisterPersonal(self, server_day_index, vnum, name, damage, timestamp):
		day_index = self.__ConvertDayIndexFromServer(server_day_index)
		if day_index in self.windowConfig["user-defines"]["data"] and vnum in self.windowConfig["user-defines"]["data"][day_index]:
			self.windowConfig["user-defines"]["data"][day_index][vnum].set_personal_entry(name, damage, timestamp)

	def __OnClickTeleport(self):
		if not hasattr(self, 'current_boss') or not self.current_boss:
			return
		
		import chat
		chat.AppendChat(chat.CHAT_TYPE_INFO, "Attempting to teleport to boss vnum: %d" % self.current_boss.vnum)
		
		# Send teleport command
		import net
		net.SendChatPacket("/world_boss_teleport %d" % self.current_boss.vnum)

	def __DisplayEntry(self, boss):
		self.current_boss = boss
	
		if boss.alive:
			alive_text = "Alive"
			time_text = "Now"
			self.respawn_seconds_remaining = 0
			is_alive = True
		else:
			alive_text = "Dead"
			is_alive = False
			
			# Calculate seconds remaining until spawn
			import time
			current_time = int(time.time())
			seconds_until_spawn = boss.respawn_time - current_time
			
			# If spawn time is in the past, show "Soon"
			if seconds_until_spawn <= 0:
				time_text = "Soon"
				self.respawn_seconds_remaining = 0
			else:
				self.respawn_seconds_remaining = app.GetTime() + seconds_until_spawn
				time_text = localeInfo.sec2time(seconds_until_spawn, "DHMS")
	
		self.__UpdateStatusInfo(alive_text, time_text, is_alive)
		
		# Select the model
		renderTarget.SelectModel(5, boss.vnum)
		
		# Set zoom based on boss vnum
		zoom_settings = {
			201100: 8000.0,  # Another boss
			201101: 7000.0,     # Your world boss
			201102: 7000.0,     # Your world boss
			201103: 9000.0,  # Another boss
			201104: 7000.0,  # Another boss
			201105: 12500.0,  # Another boss
			201106: 3000.0,  # Another boss
		}
		
		zoom = zoom_settings.get(boss.vnum, 2500.0)
		renderTarget.SetCustomZoom(5, zoom)
		
		self.__UpdateRanking(boss)

	def OnUpdate(self):
		if not hasattr(self, 'current_boss') or not self.current_boss:
			return
			
		if not hasattr(self, 'respawn_seconds_remaining') or self.respawn_seconds_remaining == 0:
			return
		
		# Check if countdown is still active
		if self.respawn_seconds_remaining - app.GetTime() > 0:
			remaining_seconds = self.respawn_seconds_remaining - app.GetTime()
			self.status_values[1].SetText(localeInfo.sec2time(int(remaining_seconds), "DHMS"))
		else:
			# Countdown finished - boss is now alive
			self.status_values[0].SetText("Alive")
			self.status_values[0].SetPackedFontColor(0xFF00FF00)  # Green
			self.status_values[1].SetText("Now")
			self.status_values[1].SetPackedFontColor(0xFFFFD700)  # Gold
			self.respawn_seconds_remaining = 0

	def __UpdateRanking(self, boss):
		print("--- Ranking ---")
		sys_err(boss.ranking)
		ranking_lines = self.GetObject("ranking-lines")
		for line, entry in zip(ranking_lines, boss.ranking):
			line.SetText("{} - {}".format(entry['name'], entry['damage']))

		# Clear remaining lines if ranking has fewer than 5 entries
		for line in ranking_lines[len(boss.ranking):]:
			line.SetText("")

	def __FormatRespawnTime(self, timestamp):
		"""
		Zwraca sformatowany czas z timestampu.
		"""
		return time.strftime("%H:%M:%S", time.localtime(timestamp))