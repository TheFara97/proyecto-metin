import uiScriptLocale

SMALL_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_01.sub"
LARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_03.sub"
XLARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_04.sub"
GUILD_PATH = "elvion/game/guild{}"

window = {
	"name" : "GuildWindow_GuildInfoPage",

	"x" : 39,
	"y" : 0,

	"width" : 656-39,
	"height" : 323,

	"children" :
	(
		{
			"name" : "Board",
			"type" : "image",
		
			"x" : 0,
			"y" : 0,
		
			"image" : GUILD_PATH.format("/guardian_items/bg.png"),
		},

		{
			"name" : "guild_information_header",
			"type" : "window",
			"x" : 8, "y" : 35,
			"width" : 317, "height" : 26,
			"children":
			(
				{ 
					"name":"Guild_Info_Point_Value",
					"type":"text",
					"x":0,
					"y":0,
					"text":uiScriptLocale.GUILD_GUARDIAN_ITEMS_INFO,
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFdcc2a9,
					"all_align" : "center",
				},
			),
		},

		{
			"name" : "guild_skill_header",
			"type" : "window",
			"x" : 329, "y" : 35,
			"width" : 271, "height" : 26,
			"children":
			(
				{ 
					"name":"Guild_Info_Point_Value",
					"type":"text",
					"x":0,
					"y":0,
					"text":uiScriptLocale.GUILD_GUARDIAN_BONUS_OVERVIEW,
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFdcc2a9,
					"all_align" : "center",
				},
			),
		},
		{
			"name" : "GuardianEq",
			"type" : "image",
			"x" : 104,
			"y" : 72,
			"image" : GUILD_PATH.format("/guardian_items/guardian_eq.png"),
			"children":
			(
				{
					"name" : "GuardEq_Slots",
					"type" : "slot",
		
					"x" : 0,
					"y" : 0,
		
					"width" : 144,
					"height" : 175,
		
					"slot" : (
								{"index":0, "x":0, "y":5, "width":32, "height":96},
								{"index":1, "x":36, "y":37, "width":32, "height":64},
								{"index":2, "x":36, "y":0, "width":32, "height":32},
								{"index":3, "x":73, "y":37, "width":32, "height":32},
								{"index":4, "x":112, "y":37, "width":32, "height":32},
								{"index":5, "x":73, "y":69, "width":32, "height":32},
								{"index":6, "x":112, "y":69, "width":32, "height":32},
								{"index":7, "x":36, "y":106, "width":32, "height":32},
								{"index":8, "x":36, "y":145, "width":32, "height":32},
							),
				},
			),
		},
		{
			"name" : "GuardEq_Activate_Button",
			"type" : "button",
			"x" : 103,
			"y" : 253,
			"text" : "Activate Items",
			"default_image" : GUILD_PATH.format("/guardian_items/active_btn_norm.png"),
			"over_image" : GUILD_PATH.format("/guardian_items/active_btn_hover.png"),
			"down_image" : GUILD_PATH.format("/guardian_items/active_btn_down.png"),
		},
		{
			"name" : "GuardEq_Deactivate_Button",
			"type" : "button",
			"x" : 103,
			"y" : 253,
			"text" : "Deactivate Items",
			"default_image" : GUILD_PATH.format("/guardian_items/active_btn_norm.png"),
			"over_image" : GUILD_PATH.format("/guardian_items/active_btn_hover.png"),
			"down_image" : GUILD_PATH.format("/guardian_items/active_btn_down.png"),
		},
		{
			"name":"GuardEq_Activate_LimitTxt",
			"type":"text",
			"x":-152,
			"y":133,
			"text":"Possible again in 1s.",
			"outline" : 1,
			"fontname":"Tahoma:12",
			"color":0xFF616060,
			"all_align": "center",
		},
		{
			"name":"GuardEq_Deactivate_LimitTxt",
			"type":"text",
			"x":-152,
			"y":133,
			"text":"Possible again in 1s.",
			"outline" : 1,
			"fontname":"Tahoma:12",
			"color":0xFF616060,
			"all_align": "center",
		},
		{
			"name":"GuardEq_IsActivate",
			"type":"text",
			"x":-22,
			"y":-85,
			"text":"Not active",
			"outline" : 1,
			"fontname":"Verdana:12b",
			"color":0xFF616060,
			"all_align": "center",
		},
			
			
		# Guild passive skills bonus
		{
			"name" : "window_event",
			"type" : "window",
			"width": 260,
			"height": 240,
			"x" :334,
			"y" : 65,
			"children":
			(
				{
					"name":"guardian_bonus_1",
					"type":"image",
					"x" : 0,
					"y" : 0,
					"image": GUILD_PATH.format("/guardian_items/guardian_bonus_slot.png"),
					"children" :
					(
						{
							"name" : "maximum_member_count_bonus",
							"type":"text",
							"text":"Maximum member count",
							"x":25,
							"y":7,
							"outline" : 1,
							"fontname":"Tahoma:12",
							"color":0xFFa3bf85,
						},
						{
							"name" : "maximum_member_count_value_slot",
							"type" : "window",
							"width": 85,
							"height": 22,
							"x" :170,
							"y" : 2,
							"children":
							(
								{
									"name" : "maximum_member_count_value",
									"type":"text",
									"text":"+0",
									"x":2,
									"y":0,
									"all_align":"center",
									"outline" : 1,
									"fontname":"Tahoma:12",
									"color":0xFFa3bf85,
								},
							),
						},
					),
				},
				{
					"name":"guardian_bonus_2",
					"type":"image",
					"x" : 0,
					"y" : 30,
					"image": GUILD_PATH.format("/guardian_items/guardian_bonus_slot.png"),
					"children" :
					(
						{
							"name" : "chance_bonus_raid_bonus",
							"type":"text",
							"text":"Chance for bonus raid",
							"x":25,
							"y":7,
							"outline" : 1,
							"fontname":"Tahoma:12",
							"color":0xFFa3bf85,
						},
						{
							"name" : "chance_bonus_raid_value_slot",
							"type" : "window",
							"width": 85,
							"height": 22,
							"x" :170,
							"y" : 2,
							"children":
							(
								{
									"name" : "chance_bonus_raid_value",
									"type":"text",
									"text":"+0%",
									"x":2,
									"y":0,
									"all_align":"center",
									"outline" : 1,
									"fontname":"Tahoma:12",
									"color":0xFFa3bf85,
								},
							),
						},
					),
				},
				{
					"name":"guardian_bonus_3",
					"type":"image",
					"x" : 0,
					"y" : 60,
					"image": GUILD_PATH.format("/guardian_items/guardian_bonus_slot.png"),
					"children" :
					(
						{
							"name" : "strong_against_monster_bonus",
							"type":"text",
							"text":"Strong against monsters",
							"x":25,
							"y":7,
							"outline" : 1,
							"fontname":"Tahoma:12",
							"color":0xFFa3bf85,
						},
						{
							"name" : "strong_against_monster_value_slot",
							"type" : "window",
							"width": 85,
							"height": 22,
							"x" :170,
							"y" : 2,
							"children":
							(
								{
									"name" : "strong_against_monster_value",
									"type":"text",
									"text":"+25%",
									"x":2,
									"y":0,
									"all_align":"center",
									"outline" : 1,
									"fontname":"Tahoma:12",
									"color":0xFFa3bf85,
								},
							),
						},
					),
				},
				{
					"name":"guardian_bonus_4",
					"type":"image",
					"x" : 0,
					"y" : 90,
					"image": GUILD_PATH.format("/guardian_items/guardian_bonus_slot.png"),
					"children" :
					(
						{
							"name" : "strong_against_elements_bonus",
							"type":"text",
							"text":"Strong against all elements",
							"x":25,
							"y":7,
							"outline" : 1,
							"fontname":"Tahoma:12",
							"color":0xFFa3bf85,
						},
						{
							"name" : "strong_against_elements_value_slot",
							"type" : "window",
							"width": 85,
							"height": 22,
							"x" :170,
							"y" : 2,
							"children":
							(
								{
									"name" : "strong_against_elements_value",
									"type":"text",
									"text":"+50%",
									"x":2,
									"y":0,
									"all_align":"center",
									"outline" : 1,
									"fontname":"Tahoma:12",
									"color":0xFFa3bf85,
								},
							),
						},
					),
				},
				{
					"name":"guardian_bonus_5",
					"type":"image",
					"x" : 0,
					"y" : 120,
					"image": GUILD_PATH.format("/guardian_items/guardian_bonus_slot.png"),
					"children" :
					(
						{
							"name" : "strong_against_stones_bonus",
							"type":"text",
							"text":"Strong against stones",
							"x":25,
							"y":7,
							"outline" : 1,
							"fontname":"Tahoma:12",
							"color":0xFFa3bf85,
						},
						{
							"name" : "strong_against_stones_value_slot",
							"type" : "window",
							"width": 85,
							"height": 22,
							"x" :170,
							"y" : 2,
							"children":
							(
								{
									"name" : "strong_against_stones_value",
									"type":"text",
									"text":"+10%",
									"x":2,
									"y":0,
									"all_align":"center",
									"outline" : 1,
									"fontname":"Tahoma:12",
									"color":0xFFa3bf85,
								},
							),
						},
					),
				},
				{
					"name":"guardian_bonus_6",
					"type":"image",
					"x" : 0,
					"y" : 150,
					"image": GUILD_PATH.format("/guardian_items/guardian_bonus_slot.png"),
					"children" :
					(
						{
							"name" : "strong_against_bosses_bonus",
							"type":"text",
							"text":"Strong against bosses",
							"x":25,
							"y":7,
							"outline" : 1,
							"fontname":"Tahoma:12",
							"color":0xFFa3bf85,
						},
						{
							"name" : "strong_against_bosses_value_slot",
							"type" : "window",
							"width": 85,
							"height": 22,
							"x" :170,
							"y" : 2,
							"children":
							(
								{
									"name" : "strong_against_bosses_value",
									"type":"text",
									"text":"+10%",
									"x":2,
									"y":0,
									"all_align":"center",
									"outline" : 1,
									"fontname":"Tahoma:12",
									"color":0xFFa3bf85,
								},
							),
						},
					),
				},
				{
					"name":"guardian_bonus_7",
					"type":"image",
					"x" : 0,
					"y" : 180,
					"image": GUILD_PATH.format("/guardian_items/guardian_bonus_slot.png"),
					"children" :
					(
						{
							"name" : "average_damage_bonus",
							"type":"text",
							"text":"Average Damage",
							"x":25,
							"y":7,
							"outline" : 1,
							"fontname":"Tahoma:12",
							"color":0xFFa3bf85,
						},
						{
							"name" : "average_damage_value_slot",
							"type" : "window",
							"width": 85,
							"height": 22,
							"x" :170,
							"y" : 2,
							"children":
							(
								{
									"name" : "average_damage_value",
									"type":"text",
									"text":"+5%",
									"x":2,
									"y":0,
									"all_align":"center",
									"outline" : 1,
									"fontname":"Tahoma:12",
									"color":0xFFa3bf85,
								},
							),
						},
					),
				},
				{
					"name":"guardian_bonus_8",
					"type":"image",
					"x" : 0,
					"y" : 210,
					"image": GUILD_PATH.format("/guardian_items/guardian_bonus_slot.png"),
					"children" :
					(
						{
							"name" : "skill_damage_bonus",
							"type":"text",
							"text":"Skill Damage",
							"x":25,
							"y":7,
							"outline" : 1,
							"fontname":"Tahoma:12",
							"color":0xFFa3bf85,
						},
						{
							"name" : "skill_damage_value_slot",
							"type" : "window",
							"width": 85,
							"height": 22,
							"x" :170,
							"y" : 2,
							"children":
							(
								{
									"name" : "skill_damage_value",
									"type":"text",
									"text":"+5%",
									"x":2,
									"y":0,
									"all_align":"center",
									"outline" : 1,
									"fontname":"Tahoma:12",
									"color":0xFFa3bf85,
								},
							),
						},
					),
				},
			),
		},
		{
			"name" : "TitleBar",
			"type" : "titlebar",
			"style" : ("attach",),
	
			"x" : 21,
			"y" : 12,
	
			"width" : 571,
			"color" : "red",
	
			"children" :
			(
				{
					"name" : "TitleName",
					"type" : "text",
					"text" : "Guild",
					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"outline": 1,
					"x" : 0,
					"y" : 2,
				},
			),
		},
	),
}
