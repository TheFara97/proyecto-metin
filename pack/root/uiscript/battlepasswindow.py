import uiScriptLocale

SIZE_X = 490+107
SIZE_Y = 438
STATUS_Y = 200
WIDTH = (SCREEN_WIDTH / 2) - (SIZE_X / 2)
HEIGHT = (SCREEN_HEIGHT / 2) - (SIZE_Y / 2)-20
PATH = "elvion/game/battlepass/"
MOVING_POS = 2
MOVING_POS_DOWN = 50
MOVING_POS_UP = 65
FREE_COLOR = 0xFFFFFFFF
PREMIUM_COLOR = 0xFFFFFFFF

window = {
	"name" : "BattlePassDialog",
	"style" : ("movable", "float",),
	"x" : WIDTH, 
	"y" : HEIGHT,
	"width" : SIZE_X+39, 
	"height" : SIZE_Y,
	"children":
	(
		{
			"name" : "Tab_Button_01",
			"type" : "button",
			"x" : 6, "y" : 38,
			"default_image" : PATH + "buttons/rewards0.png",
			"over_image" : PATH + "buttons/rewards1.png",
			"down_image" : PATH + "buttons/rewards2.png",
			"tooltip_text" : uiScriptLocale.BATTLEPASS_REWARDS,
			"tooltip_x" : -45,
			"tooltip_y" : 12,
		},
		{
			"name" : "Tab_Button_02",
			"type" : "button",
			"x" : 6, "y" : 38+44,
			"default_image" : PATH + "buttons/quest0.png",
			"over_image" : PATH + "buttons/quest1.png",
			"down_image" : PATH + "buttons/quest2.png",
			"tooltip_text" : uiScriptLocale.BATTLEPASS_MISSIONS,
			"tooltip_x" : -45,
			"tooltip_y" : 12,
		},
		#{
		#	"name" : "Tab_Button_03",
		#	"type" : "button",
		#	"x" : 6, "y" : 28+44+44+44,
		#	"default_image" : PATH + "buttons/event0.png",
		#	"over_image" : PATH + "buttons/event1.png",
		#	"down_image" : PATH + "buttons/event2.png",
		#	"tooltip_text" : uiScriptLocale.BATTLEPASS_SHOP,
		#	"tooltip_x" : -45,
		#	"tooltip_y" : 12,
		#},
		#{
		#	"name" : "Tab_Button_04",
		#	"type" : "button",
		#	"x" : 6, "y" : 28+44+44,
		#	"default_image" : PATH + "buttons/quest_event0.png",
		#	"over_image" : PATH + "buttons/quest_event1.png",
		#	"down_image" : PATH + "buttons/quest_event2.png",
		#	"tooltip_text" : uiScriptLocale.BATTLEPASS_EVENT_MISSIONS,
		#	"tooltip_x" : -45,
		#	"tooltip_y" : 12,
		#},
		{
			"name" : "Board",
			"type" : "board",
			"style" : ("attach",),
	
			"x" : 39,
			"y" : 0,
	
			"width" : SIZE_X,
			"height" : SIZE_Y,
			"children" :
			(
				{
					"name" : "board_01",
					"type" : "window",
					"style" : ("attach",),
					"x" : 0, "y" : 0,
					"width" : SIZE_X, "height" : SIZE_Y,
					"children":
					(
						{
							"name" : "backgroundImage",
							"type" : "image",
							"x" : 9, "y" : 140,
							"image" : PATH + "background_reward_down.png",
						},
						{
							"name" : "backgroundImage",
							"type" : "image",
							"x" : 9, "y" : 104,
							"image" : PATH + "header_information.png",
						},
						{
							"name" : "completeTasks",
							"type" : "text",
							"x" : 40, "y" : 114,
							"text": uiScriptLocale.BATTLEPASS_COMPLETE_TASK_TEXT,
							"outline" : 1,
							"horizontal_align" : "left",
						},
						{
							"name" : "mainProgressBackground",
							"type" : "image",
							"x" : 5, "y" : 62,
							"image" : PATH + "main_progress_background.png",
							"children": 
							(
								{
									"name" : "mainProgressEmpty",
									"type" : "image",
									"x" : 78, "y" : 6,
									"image" : PATH + "main_progress_empty.png",
									"children":
									(
										{
											"name" : "mainProgressBar",
											"type" : "expanded_image",
											"x" : 0, "y" : -1,
											"image" : PATH + "main_progress_full.png",
										},
									),
								},
							),
						},
						#{
						#	"name" : "stagesWindow",
						#	"type" : "window",
						#	"x" : 83, "y" : 0,
						#	"width" : 382, "height" : 17,
						#},
				
						#{
						#	"name" : "eventProgressBackground",
						#	"type" : "image",
						#	"x" : 5, "y" : 320,
						#	"image" : PATH + "event_progress_background.png",
						#	"children": 
						#	(
						#		{
						#			"name" : "eventProgressEmpty",
						#			"type" : "image",
						#			"x" : 78, "y" : 6,
						#			"image" : PATH + "event_progress_empty.png",
						#			"children":
						#			(
						#				{
						#					"name" : "eventProgressBar",
						#					"type" : "expanded_image",
						#					"x" : 0, "y" : -1,
						#					"image" : PATH + "event_progress_full.png",
						#				},
						#			),
						#		},
						#	),
						#},
						#{
						#	"name" : "eventStagesWindow",
						#	"type" : "window",
						#	"x" : 83, "y" : 322,
						#	"width" : 382, "height" : 17,
						#},
						#
						#{
						#	"name" : "eventTimeTitle",
						#	"type" : "text",
						#	"x" : 10, "y" : 348,
						#	"text": "Event Time:",
						#	"outline" : 1,
						#	"fontname":"Verdana:10b",
						#},
						#{
						#	"name" : "eventBattlePassEnd",
						#	"type" : "text",
						#	"x" : 75, "y" : 348,
						#	"text": "Not Active",
						#	"outline" : 1,
						#	"fontname":"Verdana:10b",
						#},
						#{
						#	"name" : "eventPremiumPoints",
						#	"type" : "text",
						#	"x" : 200, "y" : 348,
						#	"text": "Event Stage: 0",
						#	"outline" : 1,
						#	"fontname":"Verdana:10b",
						#},
						#{
						#	"name" : "eventExpPoints",
						#	"type" : "text",
						#	"x" : 320, "y" : 348,
						#	"text": "0 / 10000",
						#	"outline" : 1,
						#	"fontname":"Verdana:10b",
						#},
						
						{
							"name" : "background",
							"type" : "window",
							"x" : 70, "y" : 158,
							"width" : 420, "height" : 261,
							"horizontal_align" : "center",
							"children": 
							(
								{"name":"bg_free_01","type":"image","x":13 + MOVING_POS,"y":4,"image":PATH+"reward_free_locked.png","children":[{"name":"slot_free_01","type":"slot","x":13,"y":10+40,"width":40,"height":40,"image": PATH+"slot.png","slot":[{"index":0,"x":0,"y":0,"width":40,"height":40}]}, {"name": "level_slot_free_01","type": "image","x": 0,"horizontal_align":"center","y": 24, "image" : PATH + "level_tier_free.png", "children":({"name":"level_icon_free_01","type":"image","x":0,"y":0,"horizontal_align":"center","vertical_align":"center","image":PATH+"buttons/locked.png"},),},]},
								{"name":"bg_free_02","type":"image","x":13 + 65 + MOVING_POS,"y":4,"image":PATH+"reward_free_locked.png","children":[{"name":"slot_free_02","type":"slot","x":13,"y":10+40,"width":40,"height":40,"image": PATH+"slot.png","slot":[{"index":0,"x":0,"y":0,"width":40,"height":40}]}, {"name": "level_slot_free_02","type": "image","x": 0,"horizontal_align":"center","y": 24, "image" : PATH + "level_tier_free.png", "children":({"name":"level_icon_free_02","type":"image","x":0,"y":0,"horizontal_align":"center","vertical_align":"center","image":PATH+"buttons/locked.png"},),},]},
								{"name":"bg_free_03","type":"image","x":13 + 65 * 2 + MOVING_POS,"y":4,"image":PATH+"reward_free_locked.png","children":[{"name":"slot_free_03","type":"slot","x":13,"y":10+40,"width":40,"height":40,"image": PATH+"slot.png","slot":[{"index":0,"x":0,"y":0,"width":40,"height":40}]}, {"name": "level_slot_free_03","type": "image","x": 0,"horizontal_align":"center","y": 24, "image" : PATH + "level_tier_free.png", "children":({"name":"level_icon_free_03","type":"image","x":0,"y":0,"horizontal_align":"center","vertical_align":"center","image":PATH+"buttons/locked.png"},),},]},
								{"name":"bg_free_04","type":"image","x":13 + 65 * 3 + MOVING_POS,"y":4,"image":PATH+"reward_free_locked.png","children":[{"name":"slot_free_04","type":"slot","x":13,"y":10+40,"width":40,"height":40,"image": PATH+"slot.png","slot":[{"index":0,"x":0,"y":0,"width":40,"height":40}]}, {"name": "level_slot_free_04","type": "image","x": 0,"horizontal_align":"center","y": 24, "image" : PATH + "level_tier_free.png", "children":({"name":"level_icon_free_04","type":"image","x":0,"y":0,"horizontal_align":"center","vertical_align":"center","image":PATH+"buttons/locked.png"},),},]},
								{"name":"bg_free_05","type":"image","x":13 + 65 * 4 + MOVING_POS,"y":4,"image":PATH+"reward_free_locked.png","children":[{"name":"slot_free_05","type":"slot","x":13,"y":10+40,"width":40,"height":40,"image": PATH+"slot.png","slot":[{"index":0,"x":0,"y":0,"width":40,"height":40}]}, {"name": "level_slot_free_05","type": "image","x": 0,"horizontal_align":"center","y": 24, "image" : PATH + "level_tier_free.png", "children":({"name":"level_icon_free_05","type":"image","x":0,"y":0,"horizontal_align":"center","vertical_align":"center","image":PATH+"buttons/locked.png"},),},]},
								{"name":"bg_free_06","type":"image","x":13 + 65 * 5 + MOVING_POS,"y":4,"image":PATH+"reward_free_locked.png","children":[{"name":"slot_free_06","type":"slot","x":13,"y":10+40,"width":40,"height":40,"image": PATH+"slot.png","slot":[{"index":0,"x":0,"y":0,"width":40,"height":40}]}, {"name": "level_slot_free_06","type": "image","x": 0,"horizontal_align":"center","y": 24, "image" : PATH + "level_tier_free.png", "children":({"name":"level_icon_free_06","type":"image","x":0,"y":0,"horizontal_align":"center","vertical_align":"center","image":PATH+"buttons/locked.png"},),},]},
								
								{"name":"bg_premium_01","type":"image","x":13 + MOVING_POS,"y":138,"image":PATH+"reward_premium_locked.png","children":[{"name":"slot_premium_01","type":"slot","x":13,"y":10+40,"width":40,"height":40,"image": PATH+"slot.png","slot":[{"index":0,"x":0,"y":0,"width":40,"height":40}]}, {"name": "level_slot_premium_01","type": "image","x": 0,"horizontal_align":"center","y": 24, "image" : PATH + "level_tier_premium.png", "children":({"name":"level_icon_premium_01","type":"image","x":0,"y":0,"horizontal_align":"center","vertical_align":"center","image":PATH+"buttons/locked.png"},),},]},
								{"name":"bg_premium_02","type":"image","x":13 + 65 + MOVING_POS,"y":138,"image":PATH+"reward_premium_locked.png","children":[{"name":"slot_premium_02","type":"slot","x":13,"y":10+40,"width":40,"height":40,"image": PATH+"slot.png","slot":[{"index":0,"x":0,"y":0,"width":40,"height":40}]}, {"name": "level_slot_premium_02","type": "image","x": 0,"horizontal_align":"center","y": 24, "image" : PATH + "level_tier_premium.png", "children":({"name":"level_icon_premium_02","type":"image","x":0,"y":0,"horizontal_align":"center","vertical_align":"center","image":PATH+"buttons/locked.png"},),},]},
								{"name":"bg_premium_03","type":"image","x":13 + 65 * 2 + MOVING_POS,"y":138,"image":PATH+"reward_premium_locked.png","children":[{"name":"slot_premium_03","type":"slot","x":13,"y":10+40,"width":40,"height":40,"image": PATH+"slot.png","slot":[{"index":0,"x":0,"y":0,"width":40,"height":40}]}, {"name": "level_slot_premium_03","type": "image","x": 0,"horizontal_align":"center","y": 24, "image" : PATH + "level_tier_premium.png", "children":({"name":"level_icon_premium_03","type":"image","x":0,"y":0,"horizontal_align":"center","vertical_align":"center","image":PATH+"buttons/locked.png"},),},]},
								{"name":"bg_premium_04","type":"image","x":13 + 65 * 3 + MOVING_POS,"y":138,"image":PATH+"reward_premium_locked.png","children":[{"name":"slot_premium_04","type":"slot","x":13,"y":10+40,"width":40,"height":40,"image": PATH+"slot.png","slot":[{"index":0,"x":0,"y":0,"width":40,"height":40}]}, {"name": "level_slot_premium_04","type": "image","x": 0,"horizontal_align":"center","y": 24, "image" : PATH + "level_tier_premium.png", "children":({"name":"level_icon_premium_04","type":"image","x":0,"y":0,"horizontal_align":"center","vertical_align":"center","image":PATH+"buttons/locked.png"},),},]},
								{"name":"bg_premium_05","type":"image","x":13 + 65 * 4 + MOVING_POS,"y":138,"image":PATH+"reward_premium_locked.png","children":[{"name":"slot_premium_05","type":"slot","x":13,"y":10+40,"width":40,"height":40,"image": PATH+"slot.png","slot":[{"index":0,"x":0,"y":0,"width":40,"height":40}]}, {"name": "level_slot_premium_05","type": "image","x": 0,"horizontal_align":"center","y": 24, "image" : PATH + "level_tier_premium.png", "children":({"name":"level_icon_premium_05","type":"image","x":0,"y":0,"horizontal_align":"center","vertical_align":"center","image":PATH+"buttons/locked.png"},),},]},
								{"name":"bg_premium_06","type":"image","x":13 + 65 * 5 + MOVING_POS,"y":138,"image":PATH+"reward_premium_locked.png","children":[{"name":"slot_premium_06","type":"slot","x":13,"y":10+40,"width":40,"height":40,"image": PATH+"slot.png","slot":[{"index":0,"x":0,"y":0,"width":40,"height":40}]}, {"name": "level_slot_premium_06","type": "image","x": 0,"horizontal_align":"center","y": 24, "image" : PATH + "level_tier_premium.png", "children":({"name":"level_icon_premium_06","type":"image","x":0,"y":0,"horizontal_align":"center","vertical_align":"center","image":PATH+"buttons/locked.png"},),},]},
								
								#{"name":"bg_event_01","type":"image","x":69 + MOVING_POS,"y":188,"image":PATH+"reward_event_locked.png","children":[{"name":"slot_event_01","type":"slot","x":13,"y":10,"width":40,"height":40,"image": PATH+"slot.png","slot":[{"index":0,"x":0,"y":0,"width":40,"height":40}]}, {"name": "level_slot_event_01","type": "image","x": 0,"horizontal_align":"center","y": 63, "image" : PATH + "level_tier_event.png", "children":({"name":"level_icon_event_01","type":"image","x":0,"y":0,"horizontal_align":"center","vertical_align":"center","image":PATH+"buttons/locked.png"},),},]},
								#{"name":"bg_event_02","type":"image","x":69 + 65 + MOVING_POS,"y":188,"image":PATH+"reward_event_locked.png","children":[{"name":"slot_event_02","type":"slot","x":13,"y":10,"width":40,"height":40,"image": PATH+"slot.png","slot":[{"index":0,"x":0,"y":0,"width":40,"height":40}]}, {"name": "level_slot_event_02","type": "image","x": 0,"horizontal_align":"center","y": 63, "image" : PATH + "level_tier_event.png", "children":({"name":"level_icon_event_02","type":"image","x":0,"y":0,"horizontal_align":"center","vertical_align":"center","image":PATH+"buttons/locked.png"},),},]},
								#{"name":"bg_event_03","type":"image","x":69 + 65 * 2 + MOVING_POS,"y":188,"image":PATH+"reward_event_locked.png","children":[{"name":"slot_event_03","type":"slot","x":13,"y":10,"width":40,"height":40,"image": PATH+"slot.png","slot":[{"index":0,"x":0,"y":0,"width":40,"height":40}]}, {"name": "level_slot_event_03","type": "image","x": 0,"horizontal_align":"center","y": 63, "image" : PATH + "level_tier_event.png", "children":({"name":"level_icon_event_03","type":"image","x":0,"y":0,"horizontal_align":"center","vertical_align":"center","image":PATH+"buttons/locked.png"},),},]},
								#{"name":"bg_event_04","type":"image","x":69 + 65 * 3 + MOVING_POS,"y":188,"image":PATH+"reward_event_locked.png","children":[{"name":"slot_event_04","type":"slot","x":13,"y":10,"width":40,"height":40,"image": PATH+"slot.png","slot":[{"index":0,"x":0,"y":0,"width":40,"height":40}]}, {"name": "level_slot_event_04","type": "image","x": 0,"horizontal_align":"center","y": 63, "image" : PATH + "level_tier_event.png", "children":({"name":"level_icon_event_04","type":"image","x":0,"y":0,"horizontal_align":"center","vertical_align":"center","image":PATH+"buttons/locked.png"},),},]},
								#{"name":"bg_event_05","type":"image","x":69 + 65 * 4 + MOVING_POS,"y":188,"image":PATH+"reward_event_locked.png","children":[{"name":"slot_event_05","type":"slot","x":13,"y":10,"width":40,"height":40,"image": PATH+"slot.png","slot":[{"index":0,"x":0,"y":0,"width":40,"height":40}]}, {"name": "level_slot_event_05","type": "image","x": 0,"horizontal_align":"center","y": 63, "image" : PATH + "level_tier_event.png", "children":({"name":"level_icon_event_05","type":"image","x":0,"y":0,"horizontal_align":"center","vertical_align":"center","image":PATH+"buttons/locked.png"},),},]},
								#{"name":"bg_event_06","type":"image","x":69 + 65 * 5 + MOVING_POS,"y":188,"image":PATH+"reward_event_locked.png","children":[{"name":"slot_event_06","type":"slot","x":13,"y":10,"width":40,"height":40,"image": PATH+"slot.png","slot":[{"index":0,"x":0,"y":0,"width":40,"height":40}]}, {"name": "level_slot_event_06","type": "image","x": 0,"horizontal_align":"center","y": 63, "image" : PATH + "level_tier_event.png", "children":({"name":"level_icon_event_06","type":"image","x":0,"y":0,"horizontal_align":"center","vertical_align":"center","image":PATH+"buttons/locked.png"},),},]},
							),
						},
						{
							"name" : "previouspage",
							"type" : "button",
							"x" : 164, "y" : 204,
							"default_image" : PATH + "buttons/previous0.png",
							"over_image" : PATH + "buttons/previous1.png",
							"down_image" : PATH + "buttons/previous2.png",
						},
				
						{
							"name" : "nextpage",
							"type" : "button",
							"x" : 559, "y" : 204,
							"default_image" : PATH + "buttons/next0.png",
							"over_image" : PATH + "buttons/next1.png",
							"down_image" : PATH + "buttons/next2.png",
						},
						{
							"name" : "FreeRewards",
							"type" : "image",
							"x" : 12, "y" : 144,
							"image" : PATH + "free.png",
						},
						{
							"name" : "PremiumRewards",
							"type" : "image",
							"x" : 12, "y" : 285,
							"image" : PATH + "premium.png",
						},
						#{
						#	"name" : "EventRewards",
						#	"type" : "image",
						#	"x" : 3, "y" : 276,
						#	"image" : PATH + "event.png",
						#},
						#{
						#	"name" : "EventRewardsTBA",
						#	"type" : "image",
						#	"x" : 5, "y" : 278,
						#	"image" : PATH + "event_tba.png",
						#},
						#{
						#	"name" : "EventTBAText",
						#	"type" : "text",
						#	"x" : -115, "y" : 312,
						#	"text": uiScriptLocale.BATTLEPASS_NO_ACTIVE_EVENTS,
						#	"outline" : 1,
						#	"horizontal_align" : "center",
						#	"fontname":"Verdana:18b",
						#},
					),
				},
				{
					"name" : "stagesWindow",
					"type" : "window",
					"x" : 155+26, "y" : 108+79+82,
					"width" : 427, "height" : 30,
				},
				{
					"name" : "board_02",
					"type" : "window",
					"style" : ("attach",),
					"x" : 0, "y" : 0,
					"width" : SIZE_X, "height" : SIZE_Y,
					"children" :
					(
						{
							"name" : "background",
							"type" : "image",
							"style": ("attach",),
							"x" : 9, "y" : 104,
							"image" : PATH + "background_quest.png",
							"children":
							(
								{
									"name" : "background",
									"type" : "image",
									"style": ("attach",),
									"x" : 0, "y" : 0,
									"image" : PATH + "background_quest.png",
									"children" :
									(
										{
											"name" : "titleBarDaily",
											"type" : "image",
											"style": ("attach",),
											"x" : 4, "y" : 5,
											"image" : PATH + "titlebar_mission.png",
										},
										{
											"name" : "titleBarWeekly",
											"type" : "image",
											"style": ("attach",),
											"x" : 289, "y" : 5,
											"image" : PATH + "titlebar_premium.png",
										},
										{
											"name" : "window_daily",
											"type" : "window",
											"width": 282,
											"height": 286,
											"horizontal_align" : "left",
											"x" : 5,
											"y" : 33,
											"children":
											(
												{
													"name" : "daily",
													"type" : "text",
													"x" : 0,
													"y" : -159,
													"text": uiScriptLocale.BATTLE_PASS_2,
													"outline" : 1,
													"all_align" : "center",
													"fontname" : "Verdana:12b",
												}, 
											),
										},
										{
											"name" : "window_weekly",
											"type" : "window",
											"width": 282,
											"height": 286,
											"horizontal_align" : "left",
											"x" : 290,
											"y" : 33,
											"children":
											(
												{
													"name" : "weekly",
													"type" : "text",
													"x" : 0,
													"y" : -159,
													"text": uiScriptLocale.BATTLE_PASS_3,
													"outline" : 1,
													"all_align" : "center",
													"fontname" : "Verdana:12b",
												}, 
											),
										},
										#{
										#	"name" : "shadowDaily",
										#	"type" : "image",
										#	"style": ("attach",),
										#	"x" : 2, "y" : 293,
										#	"image" : PATH + "shadow.png",
										#},
										#{
										#	"name" : "shadowWeekly",
										#	"type" : "image",
										#	"style": ("attach",),
										#	"x" : 229, "y" : 293,
										#	"image" : PATH + "shadow.png",
										#},
										{
											"name": "ItemScrollbardaily",
											"type": "modern_scrollbar",
											"x": 282,
											"y": 34,
											"width": 6,
											"size": 283,
											"content_height": 500,
											"smooth": 1,
											"scroll_step": 0.30,
										},	
										{
											"name": "ItemScrollbarweekly",
											"type": "modern_scrollbar",
											"x": 567,
											"y": 34,
											"width": 6,
											"size": 283,
											"content_height": 500,
											"smooth": 1,
											"scroll_step": 0.30,
										},
									
									),
								},
							),
						},
					),
				},
				#{
				#	"name" : "board_03",
				#	"type" : "window",
				#	"style" : ("attach",),
				#	"x" : 0, "y" : 0,
				#	"width" : SIZE_X, "height" : SIZE_Y,
				#	"children" :
				#	(
				#		{
				#			"name" : "background",
				#			"type" : "image",
				#			"style": ("attach",),
				#			"x" : 10, "y" : 34,
				#			"image" : PATH + "background_reward.png",
				#			"children":
				#			(
				#				{
				#					"name" : "background",
				#					"type" : "image",
				#					"style": ("attach",),
				#					"x" : 5, "y" : 62,
				#					"image" : PATH + "background_quest.png",
				#					"children" :
				#					(
				#						{
				#							"name" : "titleBarEvent",
				#							"type" : "image",
				#							"style": ("attach",),
				#							"x" : 5, "y" : 5,
				#							"image" : PATH + "event_missions_title.png",
				#						},
				#						{
				#							"name" : "window_event",
				#							"type" : "window",
				#							"width": 438,
				#							"height": 288,
				#							"horizontal_align" : "left",
				#							"x" : 5,
				#							"y" : 30,
				#							"children":
				#							(
				#								{
				#									"name" : "event",
				#									"type" : "text",
				#									"x" : 0,
				#									"y" : -158,
				#									"text": "Event missions",
				#									"outline" : 1,
				#									"all_align" : "center",
				#									"fontname" : "Verdana:14b",
				#								}, 
				#							),
				#						},
				#						{
				#							"name" : "shadowEvent",
				#							"type" : "image",
				#							"style": ("attach",),
				#							"x" : 7, "y" : 293,
				#							"image" : PATH + "shadow_event.png",
				#						},
				#						{
				#							"name": "ItemScrollbarevent",
				#							"type": "modern_scrollbar",
				#							"x": 450,
				#							"y": 5,
				#							"width": 6,
				#							"size": 315,
				#							"content_height": 500,
				#							"smooth": 1,
				#							"scroll_step": 0.30,
				#						},	
				#					),
				#				},
				#			),
				#		},
				#	),
				#},
				
				{
					"name" : "topNavi",
					"type" : "image",
					"x" : 9, "y" : 35,
					"image" : PATH + "top_navi.png",
				},
				{
					"name" : "buttonBuyPremium",
					"type" : "button",
					"x" : 414,
					"y" : 48,
					"default_image" : PATH + "buttons/buy_premium0.png",
					"over_image" : PATH + "buttons/buy_premium1.png",
					"down_image" : PATH + "buttons/buy_premium2.png",
				},
				
				{
					"name" : "timeTitle",
					"type" : "text",
					"x" : -20, "y" : 44,
					"text": uiScriptLocale.BATTLEPASS_REMAINING_TIME,
					"outline" : 1,
					"horizontal_align" : "center",
					"fontname":"Verdana:12b",
					"color":0xFFbfa185,
				},
				{
					"name" : "time_window",
					"type" : "window",
					"x" : 302, "y" : 43,
					"width" : 126, "height" : 15,
					"horizontal_align" : "right",
					"children":
					(
						{
							"name" : "battlePassEnd",
							"type" : "text",
							"x" : 0, "y" : 0,
							"text": "0d 0h 0m 0s.",
							"outline" : 1,
							"all_align" : "center",
							"fontname":"Verdana:12b",
							"color":0xFFbfa185,
						},
					),
				},
				{
					"name" : "backgroundLevel_r",
					"type" : "image",
					"x" : 91, "y" : 80,
					"image" : PATH + "main_progress_empty.png",
					"children":
					(
						{
							"name" : "ExpGauge",
							"type" : "expanded_image",
							"x" : 0, "y" : 0,
							"horizontal_align" : "left",
							"image" : PATH + "main_progress_full.png",
						},
					),
				},
				{
					"name" : "expPoints",
					"type" : "text",
					"x" : 260, "y" : 76,
					"text": "500 / 500",
					"outline" : 1, 
					"text_horizontal_align" : "left",
					"fontname":"Tahoma:12",
					"color":0xFFbfa185,
				}, 
				{
					"name" : "battlepassExperienceText",
					"type" : "text",
					"x" : 93, "y" : 43,
					"text": uiScriptLocale.BATTLEPASS_EXPERIENCE_TEXT,
					"outline" : 1, 
					"horizontal_align" : "left",
					"fontname":"Tahoma:14",
					"color":0xFFbfa185,
				}, 
				
				{
					"name" : "points_window",
					"type" : "window",
					"x" : 46, "y" : 53,
					"width" : 32, "height" : 32,
					"children":
					(
						{
							"name" : "premiumPoints",
							"type" : "text",
							"x" : 0, "y" : 0,
							"text_horizontal_align" : "center",
							"text": "30",
							"outline" : 1, 
							"fontname":"Tahoma:16b",
							"color":0xFFffffff,
						}, 
					),
				},
				{
					"name" : "battlepassLevelText",
					"type" : "text",
					"x" : 46, "y" : 69,
					"text": "Level",
					"outline" : 1, 
					"text_horizontal_align" : "center",
					"fontname":"Tahoma:12",
					"color":0xFFbfa185,
				}, 
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),
	
					"x" : 21,
					"y" : 11,
	
					"width" : 449+106,
					"color" : "red",
	
					"children" :
					(
						{
							"name" : "TitleName",
							"type" : "text",
							"text" : uiScriptLocale.BATTLEPASS_WINDOW,
							"horizontal_align" : "center",
							"text_horizontal_align" : "center",
							"x" : 0,
							"y" : 2,
						},
					),
				},
			),
		},
	),
}