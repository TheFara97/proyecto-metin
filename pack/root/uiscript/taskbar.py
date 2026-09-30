import uiScriptLocale
import localeInfo

ROOT = "d:/ymir work/ui/game/"
ELVION = "elvion/game/taskbar/"

Y_ADD_POSITION = 0

window = {
	"name" : "TaskBar",

	"x" : 0,
	"y" : SCREEN_HEIGHT - 50,

	"width" : SCREEN_WIDTH,
	"height" : 50,

	"children" :
	(
		{
			"name" : "Base_Board_01",
			"type" : "expanded_image",

			"x" : 0,
			"y" : 0,

			"rect" : (0.0, 0.0, float(SCREEN_WIDTH) / 256.0, 0.0),

			"image" : ELVION + "base.png"
		},

		{
			"name" : "Gauge_Board",
			"type" : "image",

			"x" : 0,
			"y" : -30,

			"image" : ELVION + "left_bg.png",

			"children" :
			(
				{
					"name" : "RampageGauge",
					"type" : "ani_image",

					"x" : 12,
					"y" : 13,
					"width"  : 40,
					"height" : 40,

					"delay" : 6,

					"images" :
					(
						ELVION + "rampage/1.png",
						ELVION + "rampage/2.png",
						ELVION + "rampage/3.png",
						ELVION + "rampage/4.png",
						ELVION + "rampage/5.png",
						ELVION + "rampage/6.png",
						ELVION + "rampage/7.png",
						ELVION + "rampage/8.png",
						ELVION + "rampage/9.png",
						ELVION + "rampage/10.png",
						ELVION + "rampage/11.png",
						ELVION + "rampage/12.png",
						ELVION + "rampage/13.png",
						ELVION + "rampage/14.png",
						ELVION + "rampage/15.png",
						ELVION + "rampage/16.png",
						ELVION + "rampage/17.png",
					)
				},
				{
					"name" : "RampageGauge2",
					"type" : "ani_image",

					"x" : 8,
					"y" : 4,
					"width"  : 40,
					"height" : 40,

					"delay" : 6,

					"images" :
					(
						"locale/ymir_ui/mall/00.sub",
						"locale/ymir_ui/mall/01.sub",
						"locale/ymir_ui/mall/02.sub",
						"locale/ymir_ui/mall/03.sub",
						"locale/ymir_ui/mall/04.sub",
						"locale/ymir_ui/mall/05.sub",
						"locale/ymir_ui/mall/06.sub",
						"locale/ymir_ui/mall/07.sub",
						"locale/ymir_ui/mall/08.sub",
						"locale/ymir_ui/mall/09.sub",
						"locale/ymir_ui/mall/11.sub",
						"locale/ymir_ui/mall/12.sub",
						"locale/ymir_ui/mall/13.sub",
						"locale/ymir_ui/mall/14.sub",
						"locale/ymir_ui/mall/15.sub",
						"locale/ymir_ui/mall/16.sub",
					)
				},
				{
					"name" : "HPGauge_Board",
					"type" : "window",

					"x" : 86,
					"y" : 39,

					"width" : 95,
					"height" : 11,

					"children" :
					(
						{
							"name" : "HPRecoveryGaugeBar",
							"type" : "bar",

							"x" : 0,
							"y" : 0,
							"width" : 95,
							"height" : 13,
							"color" : 0x55ff0000,
						},
						{
							"name" : "HPGauge",
							"type" : "ani_image",

							"x" : 0,
							"y" : 0,

							"delay" : 6,

							"images" :
							(
								ELVION + "hp/1.png",
								ELVION + "hp/2.png",
								ELVION + "hp/3.png",
								ELVION + "hp/4.png",
								ELVION + "hp/5.png",
								ELVION + "hp/6.png",
								ELVION + "hp/7.png",
							),
						},
					),
				},
				{
					"name" : "SPGauge_Board",
					"type" : "window",

					"x" : 86,
					"y" : 55,

					"width" : 95,
					"height" : 11,

					"children" :
					(
						{
							"name" : "SPRecoveryGaugeBar",
							"type" : "bar",

							"x" : 0,
							"y" : 0,
							"width" : 95,
							"height" : 13,
							"color" : 0x550000ff,
						},
						{
							"name" : "SPGauge",
							"type" : "ani_image",

							"x" : 0,
							"y" : 0,

							"delay" : 6,

							"images" :
							(
								ELVION + "sp/1.png",
								ELVION + "sp/2.png",
								ELVION + "sp/3.png",
								ELVION + "sp/4.png",
								ELVION + "sp/5.png",
								ELVION + "sp/6.png",
								ELVION + "sp/7.png",
							),
						},
					),
				},
				{
					"name" : "STGauge_Board",
					"type" : "window",

					"x" : 86,
					"y" : 70,

					"width" : 95,
					"height" : 6,

					"children" :
					(
						{
							"name" : "STGauge",
							"type" : "ani_image",

							"x" : 0,
							"y" : 0,

							"delay" : 6,

							"images" :
							(
								ELVION + "st/1.png",
								ELVION + "st/2.png",
								ELVION + "st/3.png",
								ELVION + "st/4.png",
								ELVION + "st/5.png",
								ELVION + "st/6.png",
								ELVION + "st/7.png",
							),
						},
					),
				},

			),
		},
		{
			"name" : "EXP_Gauge_Board",
			"type" : "image",

			"x" : 236,
			"y" : 9 + Y_ADD_POSITION,

			"image" : ELVION + "left_exp_bar.png",

			"children" :
			(
				{
					"name" : "EXPGauge_01",
					"type" : "expanded_image",
				
					"x" : 9,
					"y" : 3,
				
					"image" : ELVION + "exp_full.png",
				},
				{
					"name" : "EXPGauge_02",
					"type" : "expanded_image",
				
					"x" : 46,
					"y" : 3,
				
					"image" : ELVION + "exp_full.png",
				},
				{
					"name" : "EXPGauge_03",
					"type" : "expanded_image",
				
					"x" : 82,
					"y" : 3,
				
					"image" : ELVION + "exp_full.png",
				},
				{
					"name" : "EXPGauge_04",
					"type" : "expanded_image",
				
					"x" : 119,
					"y" : 3,
				
					"image" : ELVION + "exp_full.png",
				},
								{ 	
					"name": "antyexp_window",
						"type":"window",
					"x":10,"y":1,
					"width" : 100, 
					"height": 100,
					"children":
					(
						{
							"name" : "antyexp_img0",
							"type" : "expanded_image",
				
							"x" : 5,
							"y" : 9,
							"x_scale":0.55,
							"y_scale":0.55,
				
							"image" : "d:/ymir work/ui/slot_off.png",
						},
						{
							"name" : "antyexp_img1",
							"type" : "expanded_image",
				
							"x" : 5+37,
							"y" : 9,
							"x_scale":0.55,
							"y_scale":0.55,
				
							"image" : "d:/ymir work/ui/slot_off.png",
						},
						{
							"name" : "antyexp_img2",
							"type" : "expanded_image",
				
							"x" : 5+37+37,
							"y" : 9,
							"x_scale":0.55,
							"y_scale":0.55,
				
							"image" : "d:/ymir work/ui/slot_off.png",
						},
						{
							"name" : "antyexp_img3",
							"type" : "expanded_image",
				
							"x" : 5+37+37+37,
							"y" : 9,
							"x_scale":0.55,
							"y_scale":0.55,
				
							"image" : "d:/ymir work/ui/slot_off.png",
						},
					),
				},
			),
		},

		{
			"name" : "LeftMouseButton",
			"type" : "button",

			"x" : SCREEN_WIDTH/2 - 128 - 72,
			"y" : 3 + Y_ADD_POSITION + 13,

			"tooltip_text_new" : localeInfo.TASKBAR_ATTACK,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,

			"default_image" : ELVION + "attack_norm.png",
			"over_image" : ELVION + "attack_hover.png",
			"down_image" : ELVION + "attack_down.png",
		},
		{
			"name" : "RightMouseButton",
			"type" : "button",

			"x" : SCREEN_WIDTH/2 + 128 + 66 + 11 - 19,
			"y" : 3 + Y_ADD_POSITION + 13,

			"tooltip_text_new" : localeInfo.TASKBAR_CAMERA,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,

			"default_image" : ELVION + "camera_norm.png",
			"over_image" : ELVION + "camera_hover.png",
			"down_image" : ELVION + "camera_down.png",
		},
		
		{
			"name" : "SeparatorRight",
			"type" : "image",

			"x" : SCREEN_WIDTH - 144 - 103 - 19,
			"y" : 9 + Y_ADD_POSITION - 6,

			"image" : ELVION + "separator.png",
		},
		
		{
			"name" : "SeparatorLeft",
			"type" : "image",

			"x" : SCREEN_WIDTH / 2 - 567,
			"y" : 9 + Y_ADD_POSITION - 6,

			"image" : ELVION + "separator.png",
		},

		{
			"name" : "CharacterButton",
			"type" : "button",

			"x" : SCREEN_WIDTH - 144 - 103,
			"y" : 3 + Y_ADD_POSITION + 7,

			"tooltip_text_new" : uiScriptLocale.TASKBAR_CHARACTER,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,

			"default_image" : ELVION + "character_norm.png",
			"over_image" : ELVION + "character_hover.png",
			"down_image" : ELVION + "character_down.png",
		},
		{
			"name" : "InventoryButton",
			"type" : "button",

			"x" : SCREEN_WIDTH - 144 - 103 + 37,
			"y" : 3 + Y_ADD_POSITION + 7,

			"tooltip_text_new" : uiScriptLocale.TASKBAR_INVENTORY,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,

			"default_image" : ELVION + "inventory_norm.png",
			"over_image" : ELVION + "inventory_hover.png",
			"down_image" : ELVION + "inventory_down.png",
		},
		{
			"name" : "MessengerButton",
			"type" : "button",

			"x" : SCREEN_WIDTH - 144 - 103 + 37 + 37,
			"y" : 3 + Y_ADD_POSITION + 7,

			"tooltip_text_new" : uiScriptLocale.TASKBAR_MESSENGER,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,

			"default_image" : ELVION + "friend_norm.png",
			"over_image" : ELVION + "friend_hover.png",
			"down_image" : ELVION + "friend_down.png",
		},
		{
			"name" : "SystemButton",
			"type" : "button",

			"x" : SCREEN_WIDTH - 144 - 103 + 37 + 37 + 37,
			"y" : 3 + Y_ADD_POSITION + 7,

			"tooltip_text_new" : uiScriptLocale.TASKBAR_SYSTEM,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,

			"default_image" : ELVION + "settings_norm.png",
			"over_image" : ELVION + "settings_hover.png",
			"down_image" : ELVION + "settings_down.png",
		},
		{
			"name" : "RemoteButton",
			"type" : "button",

			"x" : SCREEN_WIDTH - 144 - 103 + 37 + 37 + 37 + 50,
			"y" : 3 + Y_ADD_POSITION + 7,

			"tooltip_text_new" : uiScriptLocale.TASKBAR_ITEMSHOP,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,

			"default_image" : ELVION + "remote_norm.png",
			"over_image" : ELVION + "remote_hover.png",
			"down_image" : ELVION + "remote_down.png",
		},

		{
			"name" : "quickslot_board",
			"type" : "window",

			"x" : SCREEN_WIDTH/2 - 128 + 32 + 10 - 74,
			"y" : 7 + Y_ADD_POSITION,

			"width" : 256 + 14 + 2 + 11 + 60,
			"height" : 37,

			"children" :
			(
				{
					"name" : "ExpandButton",
					"type" : "button",

					"x" : 148,
					"y" : 8,
					"tooltip_text_new" : uiScriptLocale.TASKBAR_EXPAND,
					"tooltip_text_color" : 0xfff1e6c0,
					"tooltip_x" : 0,
					"tooltip_y" : -35,

					"default_image" : ELVION + "chat_norm.png",
					"over_image" : ELVION + "chat_hover.png",
					"down_image" : ELVION + "chat_down.png",
				},
				{
					"name" : "quick_slot_1",
					"type" : "grid_table",

					"start_index" : 0,

					"x" : 0,
					"y" : 3,

					"x_count" : 4,
					"y_count" : 1,
					"x_step" : 37,
					"y_step" : 32,

					"image" : ELVION + "slot.png",
					"image_r" : 1.0,
					"image_g" : 1.0,
					"image_b" : 1.0,
					"image_a" : 1.0,

					"children" :
					(
						{ "name" : "slot_1", "type" : "image", "x" : 5, "y" : 19, "image" : ELVION + "key/1.png", },
						{ "name" : "slot_2", "type" : "image", "x" : 41, "y" : 19, "image" : ELVION + "key/2.png", },
						{ "name" : "slot_3", "type" : "image", "x" : 78, "y" : 19, "image" : ELVION + "key/3.png", },
						{ "name" : "slot_4", "type" : "image", "x" : 115, "y" : 19, "image" : ELVION + "key/4.png", },
					),
				},
				{
					"name" : "quick_slot_2",
					"type" : "grid_table",

					"start_index" : 4,

					"x" : 128 + 14 + 33,
					"y" : 3,

					"x_count" : 4,
					"y_count" : 1,
					"x_step" : 37,
					"y_step" : 32,

					"image" : ELVION + "slot.png",
					"image_r" : 1.0,
					"image_g" : 1.0,
					"image_b" : 1.0,
					"image_a" : 1.0,

					"children" :
					(
						{ "name" : "slot_5", "type" : "image", "x" : 5, "y" : 19, "image" : ELVION + "key/f1.png", },
						{ "name" : "slot_6", "type" : "image", "x" : 42, "y" : 19, "image" : ELVION + "key/f2.png", },
						{ "name" : "slot_7", "type" : "image", "x" : 79, "y" : 19, "image" : ELVION + "key/f3.png", },
						{ "name" : "slot_8", "type" : "image", "x" : 116, "y" : 19, "image" : ELVION + "key/f4.png", },
					),
				},
				{
					"name" : "QuickSlotBoard",
					"type" : "window",

					"x" : 128+14+128+2+48,
					"y" : 0,
					"width" : 11,
					"height" : 37,
					"children" :
					(
						{
							"name" : "QuickSlotNumberBox",
							"type" : "image",
							"x" : 1,
							"y" : 13,
							"image" : ELVION + "quickslot_center.png",
						},
						{
							"name" : "QuickPageUpButton",
							"type" : "button",
							"tooltip_text_new" : uiScriptLocale.TASKBAR_PREV_QUICKSLOT,
							"tooltip_text_color" : 0xfff1e6c0,
							"tooltip_x" : 0,
							"tooltip_y" : -35,
							"x" : 1,
							"y" : 7,
							"default_image" : ELVION + "quickslot_up_norm.png",
							"over_image" : ELVION + "quickslot_up_hover.png",
							"down_image" : ELVION + "quickslot_up_down.png",
						},

						{
							"name" : "QuickPageNumber",
							"type" : "image",
							"x" : 4, "y" : 16, "image" : "d:/ymir work/ui/game/taskbar/1.sub",
						},
						{
							"name" : "QuickPageDownButton",
							"type" : "button",
							"tooltip_text_new" : uiScriptLocale.TASKBAR_NEXT_QUICKSLOT,
							"tooltip_text_color" : 0xfff1e6c0,
							"tooltip_x" : 0,
							"tooltip_y" : -35,
							"x" : 1,
							"y" : 26,

							"default_image" : ELVION + "quickslot_down_norm.png",
							"over_image" : ELVION + "quickslot_down_hover.png",
							"down_image" : ELVION + "quickslot_down_down.png",
						},

					),
				},
			),
		},

		{
			"name" : "AntiexpButton",
			"type" : "button",

			"x" : 406,
			"y" : 3 + Y_ADD_POSITION + 7,

			"tooltip_text_new" : uiScriptLocale.TASKBAR_ANTIEXP,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,

			"default_image" : ELVION + "exp_norm.png",
			"over_image" : ELVION + "exp_hover.png",
			"down_image" : ELVION + "exp_down.png",
		},

	),
}
