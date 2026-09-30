import wndMgr
import uiToolTip
import uiCommon
from _weakref import proxy

import localeInfo
import app
import net
import ui

import battlepass

_PATH = "elvion/game/battlepass/"

# Reward slots shown per page
_SLOTS_PER_PAGE = 6
# Points required per level
_PTS_PER_LEVEL = 10000
# Maximum reward pages (both passes share pagination)
_MAX_PAGES = 4

# Track indices — mirror server BPSlot.track / MISSION_TIME_* values
TRACK_DAILY = 0
TRACK_WEEKLY = 1
TRACK_EVENT = 2

# Pass types — mirror server BATTLE_PASS_NORMAL / BATTLE_PASS_EVENT
PASS_NORMAL = 0
PASS_EVENT = 1

# Tab pages
_PAGE_MAIN = 1
_PAGE_MISSIONS = 2
_PAGE_SHOP = 3
_PAGE_EVENT = 4

# Season end timestamp for the normal pass.
_SEASON_END_TS = 1775516643

_TRIGGER_ICON = {
	0:  _PATH + "icons/icon_kill.png",
	1:  _PATH + "icons/icon_sell.png",
	2:  _PATH + "icons/icon_destroy.png",
	3:  _PATH + "icons/icon_drop.png",
	4:  _PATH + "icons/icon_craft.png",
	5:  _PATH + "icons/icon_chest.png",
	6:  _PATH + "icons/icon_sash.png",
	7:  _PATH + "icons/icon_default.png",
	8:  _PATH + "icons/icon_dungeon.png",
	9:  _PATH + "icons/icon_default.png",
	10: _PATH + "icons/icon_message.png",
	11: _PATH + "icons/icon_time.png",
	12: _PATH + "icons/icon_refine.png",
	13: _PATH + "icons/icon_switch.png",
	14: _PATH + "icons/icon_default.png",
	15: _PATH + "icons/icon_stone.png",
	16: _PATH + "icons/icon_boss.png",
	17: _PATH + "icons/icon_dungeon.png",
	18: _PATH + "icons/icon_dungeon.png",
}


def _fmt(n):
	# Number formatter: 1500 -> '1.5k', 2000000 -> '2.0kk'.
	for exp, suffix in ((12, "kkkk"), (9, "kkk"), (6, "kk"), (3, "k")):
		if n >= 10 ** exp:
			return "{:.1f}{}".format(float(n) / 10 ** exp, suffix)
	return str(n)


def _time_left(end_ts):
	secs = max(0, end_ts - app.GetGlobalTimeStamp())
	d, r = divmod(secs, 86400)
	h, r = divmod(r, 3600)
	m    = r // 60
	parts = []
	if d: parts.append("{}d".format(d))
	if h: parts.append("{}h".format(h))
	if m: parts.append("{}min".format(m))
	return " ".join(parts) if parts else "0min"

class MissionRow(ui.Window):
	def __init__(self, mission_id, bg_image):
		ui.Window.__init__(self)
		self.SetWindowName("MissionRow_{}".format(mission_id))
		self._id      = mission_id
		self._base_y  = 0
		self._par_ref = None

		prog, _, _ = battlepass.GetMissionProgress(mission_id)
		goal, pts,   name  = battlepass.GetMissionInfo(mission_id)

		self._bg = ui.MakeExpandedImageBox(self, bg_image, 0, 0, "not_pick")

		self._icon = ui.ImageBox()
		self._icon.SetParent(self._bg)
		self._icon.SetPosition(11, 16)
		self._icon.LoadImage(
			_TRIGGER_ICON.get(battlepass.GetMissionType(mission_id),
							_PATH + "icons/icon_default.png"))
		self._icon.Show()

		self._done = ui.ImageBox()
		self._done.SetParent(self._bg)
		self._done.SetPosition(27, 33)
		self._done.LoadImage(_PATH + "finished.png")
		self._done.Hide()

		self._bar = ui.MakeExpandedImageBox(
			self._bg, _PATH + "quest_gauge.png", 49, 35, "not_pick")
		self._bar.SetPercentage(prog, goal)

		self._title = ui.TextLine()
		self._title.SetParent(self._bg)
		self._title.SetPosition(49, 4)
		self._title.SetFontName("Verdana:13b")
		self._title.SetPackedFontColor(0xFF987d66)
		self._title.SetText(
			name if goal else localeInfo.BATTLEPASS_MISSION_MISSING.format(mission_id))
		self._title.SetHorizontalAlignLeft()
		self._title.SetOutline()
		self._title.Show()

		self._count = ui.TextLine()
		self._count.SetParent(self._bg)
		self._count.SetPosition(93, 31)
		self._count.SetFontName("Verdana:12b")
		self._count.SetText("{} / {}".format(_fmt(prog), _fmt(goal)))
		self._count.SetHorizontalAlignLeft()
		self._count.SetOutline()
		self._count.Show()

		self._pts_lbl = ui.TextLine()
		self._pts_lbl.SetParent(self._bg)
		self._pts_lbl.SetPosition(246, 29)
		self._pts_lbl.SetFontName("Verdana:12b")
		self._pts_lbl.SetText(str(pts))
		self._pts_lbl.SetHorizontalAlignLeft()
		self._pts_lbl.SetOutline()
		self._pts_lbl.Show()

		self._clip_targets = [self._bg, self._icon, self._done, self._bar]

		self._sync_done(prog, goal)

	# ── helpers ──────────────────────────────────────────────────────────────

	def _sync_done(self, prog, goal):
		if goal > 0 and prog >= goal:
			self._done.Show()
		else:
			self._done.Hide()

	# ── ui.Window overrides ──────────────────────────────────────────────────

	def SetParent(self, parent):
		ui.Window.SetParent(self, parent)
		self._par_ref = proxy(parent)

	def SetClippingMaskWindow(self, win):
		ui.Window.SetClippingMaskWindow(self, win)
		for c in self._clip_targets:
			c.SetClippingMaskWindow(win)

	def OnRender(self):
		if not self._par_ref:
			return
		px, py = self._par_ref.GetGlobalPosition()
		pw, ph = self._par_ref.GetWidth(), self._par_ref.GetHeight()
		for c in self._clip_targets:
			if hasattr(c, "SetClipRect"):
				c.SetClipRect(px, py, px + pw, py + ph)

	def OnUpdate(self):
		prog, _, _ = battlepass.GetMissionProgress(self._id)
		goal, _, _ = battlepass.GetMissionInfo(self._id)
		self._count.SetText("{} / {}".format(_fmt(prog), _fmt(goal)))
		self._bar.SetPercentage(prog, goal)
		self._sync_done(prog, goal)

	def __del__(self):
		ui.Window.__del__(self)
		self._par_ref = None

	# ── scroll helpers ───────────────────────────────────────────────────────

	def SetBaseY(self, y):  self._base_y = y
	def GetBaseY(self):     return self._base_y

class MissionScroll(ui.Window):
	_BG_MAP = {
		TRACK_DAILY:  _PATH + "quest_view_daily.png",
		TRACK_WEEKLY: _PATH + "quest_view_weekly.png",
		TRACK_EVENT:  _PATH + "quest_event.png",
	}

	def __init__(self, track):
		ui.Window.__init__(self)
		self.SetWindowName("MissionScroll_{}".format(track))
		self._bg_img = self._BG_MAP.get(track, _PATH + "quest_view_daily.png")
		self._rows   = []

	def __del__(self):
		ui.Window.__del__(self)
		self._rows = []

	def Populate(self, mission_ids, row_h=50):
		self.Clear()
		y = 0
		for mid in mission_ids:
			row = MissionRow(mid, self._bg_img)
			row.SetParent(self)
			row.SetSize(self.GetWidth(), row_h)
			row.SetPosition(0, y)
			row.SetBaseY(y)
			row.SetClippingMaskWindow(self)
			row.Show()
			self._rows.append(row)
			y += row_h

	def Clear(self):
		for row in self._rows:
			row.Hide()
			row.Destroy()
		self._rows = []

	def Count(self):
		return len(self._rows)

	def OnScroll(self, pos):
		scroll_range = sum(r.GetHeight() for r in self._rows) - self.GetHeight()
		if scroll_range <= 0:
			return
		offset = int(pos * scroll_range)
		for row in self._rows:
			row.SetPosition(0, row.GetBaseY() - offset)

class StageBar(object):
	_CLR_PAST    = 0xFFFFD700
	_CLR_CURRENT = 0xFFFFFFFF
	_CLR_FUTURE  = 0xFF808080

	def __init__(self, parent, icon_img, count, bar_px=382):
		self._icons  = []
		self._labels = []
		spacing = bar_px / max(count - 1, 1) - 11
		for i in xrange(count):
			ico = ui.ImageBox()
			ico.SetParent(parent)
			ico.LoadImage(icon_img)
			ico.SetPosition(int(i * spacing) + 14, 0)
			ico.Show()

			lbl = ui.TextLine()
			lbl.SetParent(ico)
			lbl.SetPosition(12, 4)
			lbl.SetFontName("Verdana:12b")
			lbl.SetPackedFontColor(self._CLR_CURRENT)
			lbl.SetText(str(i + 1))
			lbl.SetOutline()
			lbl.SetHorizontalAlignCenter()
			lbl.Show()

			self._icons.append(ico)
			self._labels.append(lbl)

	def Refresh(self, cur_lvl, page_start, visible):
		for i, (ico, lbl) in enumerate(zip(self._icons, self._labels)):
			abs_lvl = page_start + i
			lbl.SetText(str(abs_lvl + 1))
			if not visible:
				ico.Hide()
				lbl.Hide()
				continue
			ico.Show()
			lbl.Show()
			if abs_lvl < cur_lvl:
				lbl.SetPackedFontColor(self._CLR_PAST)
			elif abs_lvl == cur_lvl:
				lbl.SetPackedFontColor(self._CLR_CURRENT)
			else:
				lbl.SetPackedFontColor(self._CLR_FUTURE)

class _NullStageBar(object):
	def Refresh(self, *args, **kwargs): pass


class _NullMissionScroll(object):
	def Populate(self, *args, **kwargs): pass
	def Clear(self):                     pass
	def Count(self):                     return 0
	def OnScroll(self, *args):           pass


class _NullScrollBar(object):
	def SetPos(self, *args):           pass
	def GetPos(self):                  return 0.0
	def SetContentHeight(self, *args): pass
	def SetScrollEvent(self, *args):   pass

class BattlePassWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self._pass = {
			PASS_NORMAL: [0, 0, 0],
			PASS_EVENT:  [0, 0, 0],
		}
		self._event_active = False
		self._opened       = False

		self._scroll = {}
		self._sbar   = {}
		self._stgbar = {}

		self._slots = {"free": [], "premium": [], "event": []}
		self._tabs  = {"free": [], "premium": [], "event": []}
		self._icons = {"free": [], "premium": [], "event": []}

		self._tip = uiToolTip.ItemToolTip()
		self._tip.Hide()
		self._dlg = None

		self._build_ui()

	def _build_ui(self):
		try:
			ldr = ui.PythonScriptLoader()
			ldr.LoadScriptFile(self, "uiscript/battlepasswindow.py")
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
		except:
			import exception
			exception.Abort("BattlePassWindow._build_ui")

		self._page_wnd = {
			_PAGE_MAIN:     self.GetChild("board_01"),
			_PAGE_MISSIONS: self.GetChild("board_02"),
		}
		self._page_btn = {
			_PAGE_MAIN:     self.GetChild("Tab_Button_01"),
			_PAGE_MISSIONS: self.GetChild("Tab_Button_02"),
		}
		for pid, btn in self._page_btn.items():
			btn.SetEvent(ui.__mem_func__(self.SetState), pid)

		self._stgbar[PASS_NORMAL] = StageBar(
			self.GetChild("stagesWindow"), _PATH + "stage.png", _SLOTS_PER_PAGE)
		self._stgbar[PASS_EVENT] = _NullStageBar()

		# ── mission scroll areas ─────────────────────────────────────────────
		def _mk_scroll(track, win_name, sb_name, w, h):
			host = self.GetChild(win_name)
			sb   = self.GetChild(sb_name)
			ms   = MissionScroll(track)
			ms.SetParent(host)
			ms.SetPosition(0, 0)
			ms.SetSize(w, h)
			ms.Show()
			self._scroll[track] = ms
			self._sbar[track]   = sb

			def _on_wheel(nLen, _sb=sb, _ms=ms):
				step = 0.1
				p    = max(0.0, min(1.0, _sb.GetPos() + (-step if nLen > 0 else step)))
				_sb.SetPos(p)
				_ms.OnScroll(p)
				return True
			host.OnMouseWheelScroll = _on_wheel

			sb.SetScrollEvent(lambda _sb2=sb, _ms2=ms: _ms2.OnScroll(_sb2.GetPos()))

		_mk_scroll(TRACK_DAILY,  "window_daily",  "ItemScrollbardaily",  276, 285)
		_mk_scroll(TRACK_WEEKLY, "window_weekly", "ItemScrollbarweekly", 276, 285)

		self._scroll[TRACK_EVENT] = _NullMissionScroll()
		self._sbar[TRACK_EVENT]   = _NullScrollBar()

		for x in xrange(_SLOTS_PER_PAGE):
			idx = x + 1
			for tier in ("free", "premium"):
				self._tabs[tier].append(
					self.GetChild("bg_{}_0{}".format(tier, idx)))
				self._slots[tier].append(
					self.GetChild("slot_{}_0{}".format(tier, idx)))
				self._icons[tier].append(
					self.GetChild("level_icon_{}_0{}".format(tier, idx)))

			for tier in ("free", "premium"):
				sl = self._slots[tier]
				sl[x].SetOverInItemEvent(
					lambda _, _sl=sl, _x=x: self._tip_in(_sl, _x))
				sl[x].SetOverOutItemEvent(ui.__mem_func__(self._tip_out))

		self._wipe_slots()

		self.GetChild("previouspage").SetEvent(
			ui.__mem_func__(self._turn_page), -1)
		self.GetChild("nextpage").SetEvent(
			ui.__mem_func__(self._turn_page), +1)
		self.GetChild("buttonBuyPremium").SetEvent(
			ui.__mem_func__(self._ask_premium))

		wndMgr.SetOutlineFlag(False)

		dlg = uiCommon.QuestionDialog()
		dlg.SetText(localeInfo.BATTLEPASS_PREMIUM_QUESTION_1)
		dlg.SetAcceptEvent(lambda: self._confirm_premium(True))
		dlg.SetCancelEvent(lambda: self._confirm_premium(False))
		dlg.Hide()
		self._dlg = dlg

		self.SetState(_PAGE_MAIN)

	def Open(self):
		self.Show()
		if not self._opened:
			net.SendChatPacket("/battle_pass open 1")
			self._opened = True

	def Close(self):
		self.Hide()
		if self._dlg:
			self._dlg.Close()

	def SetState(self, page_id):
		for pid, btn in self._page_btn.items():
			if pid != page_id:
				btn.SetUp()
		for pw in self._page_wnd.itervalues():
			pw.Hide()
		if page_id in self._page_wnd:
			self._page_wnd[page_id].Show()
		self.RefreshGlobal()

	def RefreshGlobal(self):
		self._pull_state()
		for sb in self._sbar.itervalues():
			sb.SetPos(0)
		self._wipe_slots()

		buckets = {TRACK_DAILY: [], TRACK_WEEKLY: [], TRACK_EVENT: []}
		for m in battlepass.GetMissions():
			t = m.get("type", TRACK_DAILY)
			if t in buckets:
				buckets[t].append(m["id"])

		for track, ids in buckets.items():
			ms = self._scroll[track]
			ms.Populate(ids)
			self._sbar[track].SetContentHeight(500 + ms.Count() * 9.4)

		self._draw_pass(PASS_NORMAL)

	def RefreshLocal(self):
		self._pull_state()
		self._draw_pass(PASS_NORMAL)

	def _pull_state(self):
		pts,  prem  = battlepass.GetPlayerInfo(PASS_NORMAL)
		epts, eprem = battlepass.GetPlayerInfo(PASS_EVENT)
		self._pass[PASS_NORMAL] = [pts,  prem,  self._pass[PASS_NORMAL][2]]
		self._pass[PASS_EVENT]  = [epts, eprem, self._pass[PASS_EVENT][2]]
		self._event_active = bool(battlepass.IsEventActive())

	def _draw_pass(self, pass_type):
		if pass_type == PASS_EVENT:
			return

		pts, is_prem, page = self._pass[pass_type]
		cur_lvl  = pts // _PTS_PER_LEVEL
		pg_start = page * _SLOTS_PER_PAGE

		self._stgbar[pass_type].Refresh(cur_lvl, pg_start, True)

		self._fill_slots("free",    battlepass.GetRewardsFree(),
						page, cur_lvl, unlocked=True)
		self._fill_slots("premium", battlepass.GetRewardsPremium(),
						page, cur_lvl, unlocked=bool(is_prem))
		self._set_bar("mainProgressBar", pts, pg_start)

	def _fill_slots(self, tier, rewards, page, cur_lvl, unlocked):
		slots = self._slots[tier]
		icons = self._icons[tier]
		if not slots:
			return
		for col in xrange(_SLOTS_PER_PAGE):
			lvl_idx = page * _SLOTS_PER_PAGE + col
			slot    = slots[col]
			icon    = icons[col]
			if not rewards or lvl_idx >= len(rewards):
				slot.ClearSlot(0)
				icon.LoadImage(_PATH + "buttons/locked.png")
				icon.SetPosition(0, 0)
				continue
			rwd   = rewards[lvl_idx]
			vnum  = rwd["vnum"]
			count = rwd["count"] if rwd["count"] >= 2 else 0
			slot.SetItemSlot(0, vnum, count)
			slot.itemIndex = vnum
			wndMgr.SetSlotItemOffset(slot.hWnd, 0, 4, 4)
			if unlocked and cur_lvl > lvl_idx:
				icon.LoadImage(_PATH + "buttons/claimed.png")
				if tier == "free":
					icon.SetPosition(0, 16)
				else:
					icon.SetPosition(0, 23)
			else:
				icon.LoadImage(_PATH + "buttons/locked.png")
				icon.SetPosition(0, 0)

	def _set_bar(self, child_name, pts, pg_start):
		exp_lo = pg_start * _PTS_PER_LEVEL
		exp_hi = exp_lo + _SLOTS_PER_PAGE * _PTS_PER_LEVEL
		if pts <= exp_lo:
			pct = 0
		elif pts >= exp_hi:
			pct = 100
		else:
			pct = int(float(pts - exp_lo) / float(exp_hi - exp_lo) * 100)
		try:
			self.GetChild(child_name).SetPercentage(pct, 100)
		except:
			pass

	def _wipe_slots(self):
		for s in (self._slots["free"] + self._slots["premium"] + self._slots["event"]):
			s.ClearSlot(0)
		for bg in self._tabs["free"]:    bg.LoadImage(_PATH + "reward_free_locked.png")
		for bg in self._tabs["premium"]: bg.LoadImage(_PATH + "reward_premium_locked.png")
		# Reset all icon positions back to default
		for ic in (self._icons["free"] + self._icons["premium"]):
			ic.SetPosition(0, 0)
		try:
			self.GetChild("ExpGauge").SetPercentage(0, 1)
		except:
			pass

	def _turn_page(self, delta):
		for pt in (PASS_NORMAL, PASS_EVENT):
			pts, prem, page = self._pass[pt]
			self._pass[pt] = [pts, prem, max(0, min(_MAX_PAGES, page + delta))]
		self._wipe_slots()
		self._draw_pass(PASS_NORMAL)

	def _tip_in(self, slot_list, idx):
		if self._tip:
			self._tip.ClearToolTip()
			vnum = getattr(slot_list[idx], "itemIndex", 0)
			if vnum:
				self._tip.AddItemData(vnum, [0, 0, 0, 0, 0, 0])
				self._tip.ShowToolTip()

	def _tip_out(self):
		if self._tip:
			self._tip.HideToolTip()

	# ── premium upgrade ───────────────────────────────────────────────────────

	def _ask_premium(self):
		if self._dlg:
			self._dlg.Open()

	def _confirm_premium(self, accepted):
		if accepted:
			net.SendChatPacket("/battle_pass premium 1")
		if self._dlg:
			self._dlg.Hide()

	# ── shop ─────────────────────────────────────────────────────────────────

	def _cmd_shop(self):
		net.SendChatPacket("/open_shop 84")

	# ── frame update ─────────────────────────────────────────────────────────

	def OnUpdate(self):
		now = app.GetGlobalTimeStamp()

		# Normal pass timer
		if now >= _SEASON_END_TS:
			self._child_set("expPoints",     "SetText", localeInfo.BATTLEPASS_EXP_ENDED)
			self._child_set("battlePassEnd", "SetText", localeInfo.BATTLEPASS_EXPIRED)
			self._child_set("premiumPoints", "SetText", "Current Stage: 0")
		else:
			pts = self._pass[PASS_NORMAL][0]
			lvl = pts // _PTS_PER_LEVEL
			self._child_set("battlePassEnd",
							"SetText", _time_left(_SEASON_END_TS))
			self._child_set("premiumPoints",
							"SetText", "{}".format(lvl))
			self._child_set("expPoints",
							"SetText", "{} / {}".format(pts, (lvl + 1) * _PTS_PER_LEVEL))

	def _child_vis(self, name, show):
		try:
			if show:
				self.GetChild(name).Show()
			else:
				self.GetChild(name).Hide()
		except:
			pass

	def _child_set(self, name, method, *args):
		try:
			getattr(self.GetChild(name), method)(*args)
		except:
			pass

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def Destroy(self):
		self.ClearDictionary()
		if self._dlg:
			self._dlg.Close()
		for ms in self._scroll.itervalues():
			ms.Clear()
		self._slots  = {"free": [], "premium": [], "event": []}
		self._tabs   = {"free": [], "premium": [], "event": []}
		self._icons  = {"free": [], "premium": [], "event": []}
		self._stgbar = {}
		self._tip    = None
		self._dlg    = None

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		self._tip = None
		self._dlg = None
