import uiScriptLocale
import item
import app

import player
EQUIPMENT_START_INDEX = player.EQUIPMENT_SLOT_START

if app.ENABLE_EXTEND_INVEN_SYSTEM and app.ENABLE_CHEQUE_SYSTEM:
	window = {
		"name" : "InventoryWindow",

		"x" : SCREEN_WIDTH - 198 - 30,
		"y" : SCREEN_HEIGHT - 37 - 618,

		"style" : ("movable", "float",),

		"width" : 195+30,
		"height" : 618,

		"children" :
		(
			{
				"name" : "board",
				"type" : "board",
				"style" : ("attach",),

				"x" : 30,
				"y" : 0,

				"width" : 195,
				"height" : 618,

				"children" :
				(

					{
						"name" : "inventoryBackground",
						"type" : "image",
						"style" : ("attach",),
						
						"x" : 9,
						"y" : 35,
						
						"image" : "elvion/game/inventory/inventory_background.png",
					},
					{
						"name" : "TitleBar",
						"type" : "titlebar",
						"style" : ("attach",),

						"x" : 19,
						"y" : 11,

						"width" : 155,
						"color" : "yellow",

						"children" :
						(
							{ "name":"TitleName", "type":"text", "x":81, "y":3, "text":uiScriptLocale.INVENTORY_TITLE, "text_horizontal_align":"center","outline":"1" },
						),
					},

					#{
					#	"name" : "RefreshButton",
					#	"type" : "button",
					#
					#	"x" : 11,
					#	"y" : 8,
					#
					#	"tooltip_text_new" : uiScriptLocale.INVENTORY_EX_STACK_ITEMS,
					#	"tooltip_text_color" : 0xfff1e6c0,
					#	"tooltip_x" : 0,
					#	"tooltip_y" : -35,
					#
					#	"default_image" : "d:/ymir work/ui/item_stack_btn0.png",
					#	"over_image" : "d:/ymir work/ui/item_stack_btn1.png",
					#	"down_image" : "d:/ymir work/ui/item_stack_btn2.png",
					#},

					{
						"name" : "Equipment_Base_Tab_0",
						"type" : "image",

						"x" : 10,
						"y" : 40,

						"image" : "elvion/game/inventory/equipment.png",

						"children" :
						(
							{
								"name" : "EquipmentSlot_0",
								"type" : "slot",

								"x" : 3,
								"y" : 3,

								"width" : 150,
								"height" : 182,

								"slot" : (
											{"index":item.EQUIPMENT_BODY, "x":49, "y":44, "width":32, "height":64},
											{"index":item.EQUIPMENT_HEAD, "x":49, "y":7, "width":32, "height":32},
											{"index":item.EQUIPMENT_SHOES, "x":49, "y":153, "width":32, "height":32},
											{"index":item.EQUIPMENT_WRIST, "x":85, "y":76, "width":32, "height":32},
											{"index":item.EQUIPMENT_WEAPON, "x":11, "y":13, "width":32, "height":96},
											{"index":item.EQUIPMENT_NECK, "x":125, "y":76, "width":32, "height":32},
											{"index":item.EQUIPMENT_EAR, "x":125, "y":44, "width":32, "height":32},
											{"index":item.EQUIPMENT_UNIQUE1, "x":12, "y":153, "width":32, "height":32},
											{"index":item.EQUIPMENT_UNIQUE2, "x":85, "y":153, "width":32, "height":32},
											{"index":item.EQUIPMENT_ARROW, "x":124, "y":9, "width":32, "height":32},
											{"index":item.EQUIPMENT_SHIELD, "x":85, "y":44, "width":32, "height":32},
											{"index":item.EQUIPMENT_RING1, "x":12, "y":114, "width":32, "height":32},
											{"index":item.EQUIPMENT_RING2, "x":85, "y":114, "width":32, "height":32},
											{"index":item.EQUIPMENT_BELT, "x":49, "y":114, "width":32, "height":32},
											{"index":item.EQUIPMENT_GLOVE, "x":125, "y":153, "width":32, "height":32}
										),
							},
							{
								"name" : "DSSButton",
								"type" : "button",

								"x" : 124,
								"y" : 116,

								"tooltip_text_new" : uiScriptLocale.INVENTORY_EX_ALCHEMY,
								"tooltip_text_color" : 0xfff1e6c0,
								"tooltip_x" : 0,
								"tooltip_y" : -35,

								"default_image" : "d:/ymir work/ui/dragonsoul/dss_inventory_button_01.tga",
								"over_image" : "d:/ymir work/ui/dragonsoul/dss_inventory_button_02.tga",
								"down_image" : "d:/ymir work/ui/dragonsoul/dss_inventory_button_03.tga",
							},

							{
								"name" : "CostumeButton",
								"type" : "button",

								"x" : 88,
								"y" : 11,

								#"tooltip_text_new" : uiScriptLocale.COSTUME_TITLE,
								#"tooltip_text_color" : 0xfff1e6c0,
								#"tooltip_x" : 0,
								#"tooltip_y" : -35,

								"default_image" : "d:/ymir work/ui/game/taskbar/costume_Button_01.tga",
								"over_image" : "d:/ymir work/ui/game/taskbar/costume_Button_02.tga",
								"down_image" : "d:/ymir work/ui/game/taskbar/costume_Button_03.tga",
							},
							#{
							#	"name" : "Equipment_Tab_01",
							#	"type" : "radio_button",
							#
							#	"x" : 10,
							#	"y" : 30,
							#
							#	#"tooltip_text_new" : uiScriptLocale.INVENTORY_ARMOR_BUTTON,
							#	#"tooltip_text_color" : 0xfff1e6c0,
							#	#"tooltip_x" : 0,
							#	#"tooltip_y" : -35,
							#
							#	"default_image" : "zaris/inventory/armor_button_norm.png",
							#	"over_image" : "zaris/inventory/armor_button_hover.png",
							#	"down_image" : "zaris/inventory/armor_button_down.png",
							#},
							#{
							#	"name" : "Equipment_Tab_02",
							#	"type" : "radio_button",
							#
							#	"x" : 87,
							#	"y" : 30,
							#
							#	#"tooltip_text_new" : uiScriptLocale.INVENTORY_TALISMAN_BUTTON,
							#	#"tooltip_text_color" : 0xfff1e6c0,
							#	#"tooltip_x" : 0,
							#	#"tooltip_y" : -35,
							#
							#	"default_image" : "zaris/inventory/talisman_button_norm.png",
							#	"over_image" : "zaris/inventory/talisman_button_hover.png",
							#	"down_image" : "zaris/inventory/talisman_button_down.png",
							#},
						),
					},
					
					# Added Pendant Equipment Tab
					{
						"name" : "Equipment_Base_Tab_1",
						"type" : "image",
						"style" : ("attach",),
						
						"x" : 10+9,
						"y" : 44,
						
						"image" : "elvion/game/inventory/talisman_bg.png",
						
						"children" :
						(
							{
								"name" : "EquipmentSlot_1",
								"type" : "slot",
								
								"x" : 3,
								"y" : 3,
								
								"width" : 150,
								"height" : 182,
								
								"slot" : (),
							},
							{
								"name" : "pendant_page_0",
								"type" : "radio_button",
								
								"x" : 9,
								"y" : 147,
								
								"default_image" : "d:/ymir work/ui/game/inventory/talisman1-icon-1.png",
								"over_image" : "d:/ymir work/ui/game/inventory/talisman1-icon-2.png",
								"down_image" : "d:/ymir work/ui/game/inventory/talisman1-icon-3.png",
							},
							{
								"name" : "pendant_page_1",
								"type" : "radio_button",
								
								"x" : 110,
								"y" : 147,
								
								"default_image" : "d:/ymir work/ui/game/inventory/talisman2-icon-1.png",
								"over_image" : "d:/ymir work/ui/game/inventory/talisman2-icon-2.png",
								"down_image" : "d:/ymir work/ui/game/inventory/talisman2-icon-3.png",
							},
						),
					},

					{
						"name" : "Inventory_Tab_01",
						"type" : "radio_button",

						"x" : 9,
						"y" : 50 + 191,

						"default_image" : "elvion/game/inventory/inventory_btn_norm.png",
						"over_image" : "elvion/game/inventory/inventory_btn_hover.png",
						"down_image" : "elvion/game/inventory/inventory_btn_down.png",

						"children" :
						(
							{
								"name" : "Inventory_Tab_01_Print",
								"type" : "text",

								"x" : 0,
								"y" : 0,

								"all_align" : "center",

								#"text" : "I",
								"outline":"1",
							},
						),
					},
					{
						"name" : "Inventory_Tab_02",
						"type" : "radio_button",

						"x" : 9 + 44,
						"y" : 50 + 191,

						"default_image" : "elvion/game/inventory/inventory_btn_2_norm.png",
						"over_image" : "elvion/game/inventory/inventory_btn_2_hover.png",
						"down_image" : "elvion/game/inventory/inventory_btn_2_down.png",

						"children" :
						(
							{
								"name" : "Inventory_Tab_02_Print",
								"type" : "text",

								"x" : 0,
								"y" : 0,

								"all_align" : "center",

								#"text" : "II",
								"outline":"1",
							},
						),
					},

					{
						"name" : "Inventory_Tab_03",
						"type" : "radio_button",

						"x" : 9 + 44 + 44,
						"y" : 50 + 191,

						"default_image" : "elvion/game/inventory/inventory_btn_3_norm.png",
						"over_image" : "elvion/game/inventory/inventory_btn_3_hover.png",
						"down_image" : "elvion/game/inventory/inventory_btn_3_down.png",

						"children" :
						(
							{
								"name" : "Inventory_Tab_03_Print",
								"type" : "text",

								"x" : 0,
								"y" : 0,

								"all_align" : "center",

								#"text" : "III",
								"outline":"1",
							},
						),
					},

					{
						"name" : "Inventory_Tab_04",
						"type" : "radio_button",

						"x" : 9 + 44 + 44 + 44,
						"y" : 50 + 191,

						"default_image" : "elvion/game/inventory/inventory_btn_4_norm.png",
						"over_image" : "elvion/game/inventory/inventory_btn_4_hover.png",
						"down_image" : "elvion/game/inventory/inventory_btn_4_down.png",

						"children" :
						(
							{
								"name" : "Inventory_Tab_04_Print",
								"type" : "text",

								"x" : 0,
								"y" : 0,

								"all_align" : "center",

								#"text" : "IV",
								"outline":"1",
							},
						),
					},

					{
						"name" : "ItemSlot",
						"type" : "grid_table",

						"x" : 17,
						"y" : 273,

						"start_index" : 0,
						"x_count" : 5,
						"y_count" : 9,
						"x_step" : 32,
						"y_step" : 32,

						"image" : "elvion/game/inventory/slot.png"
					},

					{
						"name":"Money_Icon",
						"type":"image",
						"vertical_align":"bottom",

						"x":12,
						"y":66,

						"image":"d:/ymir work/ui/game/windows/money_icon.sub",
					},
					{
						"name":"Cheque_Icon",
						"type":"image",

						"x":12,
						"y":46,
						"vertical_align":"bottom",
						"image":"d:/ymir work/ui/game/windows/cheque_icon.sub",
					},
					{
						"name":"Pkt_Osiag_Icon",
						"type":"image",

						"x":12,
						"y":26,
						"vertical_align":"bottom",
						"image":"icon/emoji/achievement_point.tga",
					},
					{
						"name":"Money_Slot",
						"type":"button",

						"x":32,
						"y":68,

						"vertical_align":"bottom",

						"default_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",
						"over_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",
						"down_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",

						"children" :
						(
							{
								"name" : "Money",
								"type" : "text",

								"x" : 3,
								"y" : 3,

								"horizontal_align" : "right",
								"text_horizontal_align" : "right",

								"text" : "123456789",
							},
						),
					},
					{
						"name":"Cheque_Slot",
						"type":"button",

						"x":32,
						"y":48,

						"default_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",
						"over_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",
						"down_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",
						"vertical_align":"bottom",
						"children" :
						(
							{
								"name" : "Cheque",
								"type" : "text",

								"x" : 3,
								"y" : 3,

								"horizontal_align" : "right",
								"text_horizontal_align" : "right",

								"text" : "99",
							},
						),
					},
					{
						"name":"Pkt_Osiag_Slot",
						"type":"button",

						"x":32,
						"y":28,

						"default_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",
						"over_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",
						"down_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",
						"vertical_align":"bottom",
						"children" :
						(
							{
								"name" : "Pkt_Osiag",
								"type" : "text",

								"x" : 3,
								"y" : 3,

								"horizontal_align" : "right",
								"text_horizontal_align" : "right",

								"text" : "99",
							},
						),
					},
				),
			},
			{
				"name" : "Equipment_Tab_0",
				"type" : "radio_button",

				"x" : 6,
				"y" : 39,

				"tooltip_text_new" : uiScriptLocale.INVENTORY_ARMOR_BUTTON,
				"tooltip_text_color" : 0xfff1e6c0,
				"tooltip_x" : 0,
				"tooltip_y" : -35,

				"default_image" : "elvion/game/inventory/inventory_page_1_norm.png",
				"over_image" : "elvion/game/inventory/inventory_page_1_hover.png",
				"down_image" : "elvion/game/inventory/inventory_page_1_down.png",
			},
			{
				"name" : "Equipment_Tab_1",
				"type" : "radio_button",

				"x" : 6,
				"y" : 39+28,

				"tooltip_text_new" : uiScriptLocale.INVENTORY_TALISMAN_BUTTON,
				"tooltip_text_color" : 0xfff1e6c0,
				"tooltip_x" : 0,
				"tooltip_y" : -35,

				"default_image" : "elvion/game/inventory/inventory_page_2_norm.png",
				"over_image" : "elvion/game/inventory/inventory_page_2_hover.png",
				"down_image" : "elvion/game/inventory/inventory_page_2_down.png",
			},
		),
	}
elif app.ENABLE_EXTEND_INVEN_SYSTEM and not app.ENABLE_CHEQUE_SYSTEM:
	window = {
		"name" : "InventoryWindow",

		"x" : SCREEN_WIDTH - 176,
		"y" : SCREEN_HEIGHT - 37 - 565,

		"style" : ("movable", "float",),

		"width" : 176,
		"height" : 565,

		"children" :
		(
			{
				"name" : "board",
				"type" : "board",
				"style" : ("attach",),

				"x" : 0,
				"y" : 0,

				"width" : 176,
				"height" : 565,

				"children" :
				(
					{
						"name" : "TitleBar",
						"type" : "titlebar",
						"style" : ("attach",),

						"x" : 8,
						"y" : 7,

						"width" : 161,
						"color" : "yellow",

						"children" :
						(
							{ "name":"TitleName", "type":"text", "x":81, "y":0, "text":uiScriptLocale.INVENTORY_TITLE, "text_horizontal_align":"center","outline":"1" },
						),
					},

					{
						"name" : "RefreshButton",
						"type" : "button",

						"x" : 8,
						"y" : 7,

						"tooltip_text_new" : uiScriptLocale.INVENTORY_EX_STACK_ITEMS,
						"tooltip_text_color" : 0xfff1e6c0,
						"tooltip_x" : 0,
						"tooltip_y" : -35,

						"default_image" : "d:/ymir work/ui/pattern/titlebar_inv_refresh_baseframe.tga",
						"over_image" : "d:/ymir work/ui/pattern/titlebar_inv_refresh_baseframe1.tga",
						"down_image" : "d:/ymir work/ui/pattern/titlebar_inv_refresh_baseframe2.tga",
					},

					{
						"name" : "Equipment_Base",
						"type" : "image",

						"x" : 10,
						"y" : 33,

						"image" : "d:/ymir work/ui/equipment_bg_without_ring.tga",

						"children" :
						(

							{
								"name" : "EquipmentSlot",
								"type" : "slot",

								"x" : 3,
								"y" : 3,

								"width" : 150,
								"height" : 182,

								"slot" : (
											{"index":EQUIPMENT_START_INDEX+0, "x":39, "y":37, "width":32, "height":64},
											{"index":EQUIPMENT_START_INDEX+1, "x":39, "y":2, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+2, "x":39, "y":145, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+3, "x":75, "y":67, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+4, "x":3, "y":3, "width":32, "height":96},
											{"index":EQUIPMENT_START_INDEX+5, "x":114, "y":67, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+6, "x":114, "y":35, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+7, "x":2, "y":145, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+8, "x":75, "y":145, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+9, "x":114, "y":2, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+10, "x":75, "y":35, "width":32, "height":32},
											{"index":item.EQUIPMENT_BELT, "x":39, "y":106, "width":32, "height":32},
										),
							},
							{
								"name" : "DSSButton",
								"type" : "button",

								"x" : 114,
								"y" : 107,

								"tooltip_text_new" : uiScriptLocale.INVENTORY_EX_ALCHEMY,
								"tooltip_text_color" : 0xfff1e6c0,
								"tooltip_x" : 0,
								"tooltip_y" : -35,

								"default_image" : "d:/ymir work/ui/dragonsoul/dss_inventory_button_01.tga",
								"over_image" : "d:/ymir work/ui/dragonsoul/dss_inventory_button_02.tga",
								"down_image" : "d:/ymir work/ui/dragonsoul/dss_inventory_button_03.tga",
							},
							{
								"name" : "MallButton",
								"type" : "button",

								"x" : 118,
								"y" : 148,

								"tooltip_text" : uiScriptLocale.MALL_TITLE,

								"default_image" : "d:/ymir work/ui/game/TaskBar/Mall_Button_01.tga",
								"over_image" : "d:/ymir work/ui/game/TaskBar/Mall_Button_02.tga",
								"down_image" : "d:/ymir work/ui/game/TaskBar/Mall_Button_03.tga",
							},
							{
								"name" : "CostumeButton",
								"type" : "button",

								"x" : 78,
								"y" : 5,

								"tooltip_text_new" : "test\ntest",
								"tooltip_text_color" : 0xfff1e6c0,
								"tooltip_x" : 0,
								"tooltip_y" : -35,

								"default_image" : "d:/ymir work/ui/game/taskbar/costume_Button_01.tga",
								"over_image" : "d:/ymir work/ui/game/taskbar/costume_Button_02.tga",
								"down_image" : "d:/ymir work/ui/game/taskbar/costume_Button_03.tga",
							},
							{
								"name" : "Equipment_Tab_01",
								"type" : "radio_button",

								"x" : 86,
								"y" : 161,

								"default_image" : "d:/ymir work/ui/game/windows/tab_button_small_01.sub",
								"over_image" : "d:/ymir work/ui/game/windows/tab_button_small_02.sub",
								"down_image" : "d:/ymir work/ui/game/windows/tab_button_small_03.sub",

								"children" :
								(
									{
										"name" : "Equipment_Tab_01_Print",
										"type" : "text",

										"x" : 0,
										"y" : 0,

										"all_align" : "center",

										"text" : "I",
									},
								),
							},
							{
								"name" : "Equipment_Tab_02",
								"type" : "radio_button",

								"x" : 86 + 32,
								"y" : 161,

								"default_image" : "d:/ymir work/ui/game/windows/tab_button_small_01.sub",
								"over_image" : "d:/ymir work/ui/game/windows/tab_button_small_02.sub",
								"down_image" : "d:/ymir work/ui/game/windows/tab_button_small_03.sub",

								"children" :
								(
									{
										"name" : "Equipment_Tab_02_Print",
										"type" : "text",

										"x" : 0,
										"y" : 0,

										"all_align" : "center",

										"text" : "II",
									},
								),
							},

						),
					},

					{
						"name" : "Inventory_Tab_01",
						"type" : "radio_button",

						"x" : 10,
						"y" : 33 + 191,

						"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_01.sub",
						"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_02.sub",
						"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_03.sub",

						"children" :
						(
							{
								"name" : "Inventory_Tab_01_Print",
								"type" : "text",

								"x" : 0,
								"y" : 0,

								"all_align" : "center",

								"text" : "I",
								"outline":"1",
							},
						),
					},
					{
						"name" : "Inventory_Tab_02",
						"type" : "radio_button",

						"x" : 10 + 39,
						"y" : 33 + 191,

						"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_01.sub",
						"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_02.sub",
						"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_03.sub",

						"children" :
						(
							{
								"name" : "Inventory_Tab_02_Print",
								"type" : "text",

								"x" : 0,
								"y" : 0,

								"all_align" : "center",

								"text" : "II",
								"outline":"1",
							},
						),
					},

					{
						"name" : "Inventory_Tab_03",
						"type" : "radio_button",

						"x" : 10 + 39 + 39,
						"y" : 33 + 191,

						"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_01.sub",
						"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_02.sub",
						"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_03.sub",

						"children" :
						(
							{
								"name" : "Inventory_Tab_03_Print",
								"type" : "text",

								"x" : 0,
								"y" : 0,

								"all_align" : "center",

								"text" : "III",
								"outline":"1",
							},
						),
					},

					{
						"name" : "Inventory_Tab_04",
						"type" : "radio_button",

						"x" : 10 + 39 + 39 + 39,
						"y" : 33 + 191,

						"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_01.sub",
						"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_02.sub",
						"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_03.sub",

						"children" :
						(
							{
								"name" : "Inventory_Tab_04_Print",
								"type" : "text",

								"x" : 0,
								"y" : 0,

								"all_align" : "center",

								"text" : "IV",
								"outline":"1",
							},
						),
					},

					{
						"name" : "ItemSlot",
						"type" : "grid_table",

						"x" : 8,
						"y" : 246,

						"start_index" : 0,
						"x_count" : 5,
						"y_count" : 9,
						"x_step" : 32,
						"y_step" : 32,

						"image" : "d:/ymir work/ui/public/Slot_Base.sub"
					},

					{
						"name":"Money_Icon",
						"type":"image",

						"x":10,
						"y":26,

						"image":"d:/ymir work/ui/game/windows/money_icon.sub",
					},
					{
						"name":"Cheque_Icon",
						"type":"image",
						"vertical_align":"bottom",
						
						"x":10,
						"y":46,

						"image":"d:/ymir work/ui/game/windows/cheque_icon.sub",
					},

					{
						"name":"Money_Slot",
						"type":"button",

						"x":28,
						"y":28,

						"horizontal_align":"center",
						"vertical_align":"bottom",

						"default_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",
						"over_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",
						"down_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",

						"children" :
						(
							{
								"name" : "Money",
								"type" : "text",

								"x" : 3,
								"y" : 3,

								"horizontal_align" : "right",
								"text_horizontal_align" : "right",

								"text" : "123456789",
							},
						),
					},
					{
						"name":"Cheque_Slot",
						"type":"button",

						"x":28,
						"y":48,

						"vertical_align":"bottom",

						"default_image" : "d:/ymir work/ui/public/cheque_slot.sub",
						"over_image" : "d:/ymir work/ui/public/cheque_slot.sub",
						"down_image" : "d:/ymir work/ui/public/cheque_slot.sub",

						"children" :
						(
							{
								"name" : "Cheque",
								"type" : "text",

								"x" : 3,
								"y" : 3,

								"horizontal_align" : "right",
								"text_horizontal_align" : "right",

								"text" : "99",
							},
						),
					},
				),
			},
		),
	}
elif not app.ENABLE_EXTEND_INVEN_SYSTEM and app.ENABLE_CHEQUE_SYSTEM:
	window = {
		"name" : "InventoryWindow",

		"x" : SCREEN_WIDTH - 176,
		"y" : SCREEN_HEIGHT - 37 - 565,

		"style" : ("movable", "float",),

		"width" : 176,
		"height" : 565,

		"children" :
		(
			{
				"name" : "board",
				"type" : "board",
				"style" : ("attach",),

				"x" : 0,
				"y" : 0,

				"width" : 176,
				"height" : 565,

				"children" :
				(
					{
						"name" : "TitleBar",
						"type" : "titlebar",
						"style" : ("attach",),

						"x" : 8,
						"y" : 7,

						"width" : 161,
						"color" : "yellow",

						"children" :
						(
							{ "name":"TitleName", "type":"text", "x":81, "y":0, "text":uiScriptLocale.INVENTORY_TITLE, "text_horizontal_align":"center","outline":"1" },
						),
					},

					{
						"name" : "RefreshButton",
						"type" : "button",

						"x" : 8,
						"y" : 7,

						"tooltip_text_new" : uiScriptLocale.INVENTORY_EX_STACK_ITEMS,
						"tooltip_text_color" : 0xfff1e6c0,
						"tooltip_x" : 0,
						"tooltip_y" : -35,

						"default_image" : "d:/ymir work/ui/pattern/titlebar_inv_refresh_baseframe.tga",
						"over_image" : "d:/ymir work/ui/pattern/titlebar_inv_refresh_baseframe1.tga",
						"down_image" : "d:/ymir work/ui/pattern/titlebar_inv_refresh_baseframe2.tga",
					},

					{
						"name" : "Equipment_Base",
						"type" : "image",

						"x" : 10,
						"y" : 33,

						"image" : "d:/ymir work/ui/equipment_bg_without_ring.tga",

						"children" :
						(

							{
								"name" : "EquipmentSlot",
								"type" : "slot",

								"x" : 3,
								"y" : 3,

								"width" : 150,
								"height" : 182,

								"slot" : (
											{"index":EQUIPMENT_START_INDEX+0, "x":39, "y":37, "width":32, "height":64},
											{"index":EQUIPMENT_START_INDEX+1, "x":39, "y":2, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+2, "x":39, "y":145, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+3, "x":75, "y":67, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+4, "x":3, "y":3, "width":32, "height":96},
											{"index":EQUIPMENT_START_INDEX+5, "x":114, "y":67, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+6, "x":114, "y":35, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+7, "x":2, "y":145, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+8, "x":75, "y":145, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+9, "x":114, "y":2, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+10, "x":75, "y":35, "width":32, "height":32},
											{"index":item.EQUIPMENT_BELT, "x":39, "y":106, "width":32, "height":32},
										),
							},
							{
								"name" : "DSSButton",
								"type" : "button",

								"x" : 114,
								"y" : 107,

								"tooltip_text_new" : uiScriptLocale.INVENTORY_EX_ALCHEMY,
								"tooltip_text_color" : 0xfff1e6c0,
								"tooltip_x" : 0,
								"tooltip_y" : -35,

								"default_image" : "d:/ymir work/ui/dragonsoul/dss_inventory_button_01.tga",
								"over_image" : "d:/ymir work/ui/dragonsoul/dss_inventory_button_02.tga",
								"down_image" : "d:/ymir work/ui/dragonsoul/dss_inventory_button_03.tga",
							},
							{
								"name" : "MallButton",
								"type" : "button",

								"x" : 118,
								"y" : 148,

								"tooltip_text" : uiScriptLocale.MALL_TITLE,

								"default_image" : "d:/ymir work/ui/game/TaskBar/Mall_Button_01.tga",
								"over_image" : "d:/ymir work/ui/game/TaskBar/Mall_Button_02.tga",
								"down_image" : "d:/ymir work/ui/game/TaskBar/Mall_Button_03.tga",
							},
							{
								"name" : "CostumeButton",
								"type" : "button",

								"x" : 78,
								"y" : 5,

								"tooltip_text_new" : uiScriptLocale.COSTUME_TITLE,
								"tooltip_text_color" : 0xfff1e6c0,
								"tooltip_x" : 0,
								"tooltip_y" : -35,
								
								"default_image" : "d:/ymir work/ui/game/taskbar/costume_Button_01.tga",
								"over_image" : "d:/ymir work/ui/game/taskbar/costume_Button_02.tga",
								"down_image" : "d:/ymir work/ui/game/taskbar/costume_Button_03.tga",
							},
							{
								"name" : "Equipment_Tab_01",
								"type" : "radio_button",

								"x" : 86,
								"y" : 161,

								"default_image" : "d:/ymir work/ui/game/windows/tab_button_small_01.sub",
								"over_image" : "d:/ymir work/ui/game/windows/tab_button_small_02.sub",
								"down_image" : "d:/ymir work/ui/game/windows/tab_button_small_03.sub",

								"children" :
								(
									{
										"name" : "Equipment_Tab_01_Print",
										"type" : "text",

										"x" : 0,
										"y" : 0,

										"all_align" : "center",

										"text" : "I",
									},
								),
							},
							{
								"name" : "Equipment_Tab_02",
								"type" : "radio_button",

								"x" : 86 + 32,
								"y" : 161,

								"default_image" : "d:/ymir work/ui/game/windows/tab_button_small_01.sub",
								"over_image" : "d:/ymir work/ui/game/windows/tab_button_small_02.sub",
								"down_image" : "d:/ymir work/ui/game/windows/tab_button_small_03.sub",

								"children" :
								(
									{
										"name" : "Equipment_Tab_02_Print",
										"type" : "text",

										"x" : 0,
										"y" : 0,

										"all_align" : "center",

										"text" : "II",
									},
								),
							},

						),
					},

					{
						"name" : "Inventory_Tab_01",
						"type" : "radio_button",

						"x" : 10,
						"y" : 33 + 191,

						"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_01.sub",
						"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_02.sub",
						"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_03.sub",

						"children" :
						(
							{
								"name" : "Inventory_Tab_01_Print",
								"type" : "text",

								"x" : 0,
								"y" : 0,

								"all_align" : "center",

								"text" : "I",
							},
						),
					},
					{
						"name" : "Inventory_Tab_02",
						"type" : "radio_button",

						"x" : 10 + 78,
						"y" : 33 + 191,

						"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_01.sub",
						"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_02.sub",
						"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_03.sub",

						"children" :
						(
							{
								"name" : "Inventory_Tab_02_Print",
								"type" : "text",

								"x" : 0,
								"y" : 0,

								"all_align" : "center",

								"text" : "II",
							},
						),
					},

					{
						"name" : "ItemSlot",
						"type" : "grid_table",

						"x" : 8,
						"y" : 246,

						"start_index" : 0,
						"x_count" : 5,
						"y_count" : 9,
						"x_step" : 32,
						"y_step" : 32,

						"image" : "d:/ymir work/ui/public/Slot_Base.sub"
					},

					{
						"name":"Money_Icon",
						"type":"image",
						"vertical_align":"bottom",

						"x":57,
						"y":26,

						"image":"d:/ymir work/ui/game/windows/money_icon.sub",
					},
					{
						"name":"Money_Slot",
						"type":"button",

						"x":75,
						"y":28,

						"vertical_align":"bottom",

						"default_image" : "d:/ymir work/ui/public/gold_slot.sub",
						"over_image" : "d:/ymir work/ui/public/gold_slot.sub",
						"down_image" : "d:/ymir work/ui/public/gold_slot.sub",

						"children" :
						(
							{
								"name" : "Money",
								"type" : "text",

								"x" : 3,
								"y" : 3,

								"horizontal_align" : "right",
								"text_horizontal_align" : "right",

								"text" : "123456789",
							},
						),
					},
					{
						"name":"Cheque_Icon",
						"type":"image",
						"vertical_align":"bottom",

						"x":10,
						"y":26,

						"image":"d:/ymir work/ui/game/windows/cheque_icon.sub",
					},
					{
						"name":"Cheque_Slot",
						"type":"button",

						"x":28,
						"y":28,

						"vertical_align":"bottom",

						"default_image" : "d:/ymir work/ui/public/cheque_slot.sub",
						"over_image" : "d:/ymir work/ui/public/cheque_slot.sub",
						"down_image" : "d:/ymir work/ui/public/cheque_slot.sub",

						"children" :
						(
							{
								"name" : "Cheque",
								"type" : "text",

								"x" : 3,
								"y" : 3,

								"horizontal_align" : "right",
								"text_horizontal_align" : "right",

								"text" : "99",
							},
						),
					},
				),
			},
		),
	}
else:
	window = {
		"name" : "InventoryWindow",

		"x" : SCREEN_WIDTH - 176,
		"y" : SCREEN_HEIGHT - 37 - 565,

		"style" : ("movable", "float",),

		"width" : 176,
		"height" : 565,

		"children" :
		(
			{
				"name" : "board",
				"type" : "board",
				"style" : ("attach",),

				"x" : 0,
				"y" : 0,

				"width" : 176,
				"height" : 565,

				"children" :
				(
					{
						"name" : "TitleBar",
						"type" : "titlebar",
						"style" : ("attach",),

						"x" : 8,
						"y" : 7,

						"width" : 161,
						"color" : "yellow",

						"children" :
						(
							{ "name":"TitleName", "type":"text", "x":81, "y":0, "text":uiScriptLocale.INVENTORY_TITLE, "text_horizontal_align":"center","outline":"1" },
						),
					},

					{
						"name" : "RefreshButton",
						"type" : "button",

						"x" : 8,
						"y" : 7,

						"tooltip_text_new" : uiScriptLocale.INVENTORY_EX_STACK_ITEMS,
						"tooltip_text_color" : 0xfff1e6c0,
						"tooltip_x" : 0,
						"tooltip_y" : -35,

						"default_image" : "d:/ymir work/ui/pattern/titlebar_inv_refresh_baseframe.tga",
						"over_image" : "d:/ymir work/ui/pattern/titlebar_inv_refresh_baseframe1.tga",
						"down_image" : "d:/ymir work/ui/pattern/titlebar_inv_refresh_baseframe2.tga",
					},

					{
						"name" : "Equipment_Base",
						"type" : "image",

						"x" : 10,
						"y" : 33,

						"image" : "d:/ymir work/ui/equipment_bg_without_ring.tga",

						"children" :
						(

							{
								"name" : "EquipmentSlot",
								"type" : "slot",

								"x" : 3,
								"y" : 3,

								"width" : 150,
								"height" : 182,

								"slot" : (
											{"index":EQUIPMENT_START_INDEX+0, "x":39, "y":37, "width":32, "height":64},
											{"index":EQUIPMENT_START_INDEX+1, "x":39, "y":2, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+2, "x":39, "y":145, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+3, "x":75, "y":67, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+4, "x":3, "y":3, "width":32, "height":96},
											{"index":EQUIPMENT_START_INDEX+5, "x":114, "y":67, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+6, "x":114, "y":35, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+7, "x":2, "y":145, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+8, "x":75, "y":145, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+9, "x":114, "y":2, "width":32, "height":32},
											{"index":EQUIPMENT_START_INDEX+10, "x":75, "y":35, "width":32, "height":32},
											{"index":item.EQUIPMENT_BELT, "x":39, "y":106, "width":32, "height":32},
										),
							},
							{
								"name" : "DSSButton",
								"type" : "button",

								"x" : 114,
								"y" : 107,

								"tooltip_text_new" : uiScriptLocale.INVENTORY_EX_ALCHEMY,
								"tooltip_text_color" : 0xfff1e6c0,
								"tooltip_x" : 0,
								"tooltip_y" : -35,
								
								"default_image" : "d:/ymir work/ui/dragonsoul/dss_inventory_button_01.tga",
								"over_image" : "d:/ymir work/ui/dragonsoul/dss_inventory_button_02.tga",
								"down_image" : "d:/ymir work/ui/dragonsoul/dss_inventory_button_03.tga",
							},
							{
								"name" : "MallButton",
								"type" : "button",

								"x" : 118,
								"y" : 148,

								"tooltip_text" : uiScriptLocale.MALL_TITLE,

								"default_image" : "d:/ymir work/ui/game/TaskBar/Mall_Button_01.tga",
								"over_image" : "d:/ymir work/ui/game/TaskBar/Mall_Button_02.tga",
								"down_image" : "d:/ymir work/ui/game/TaskBar/Mall_Button_03.tga",
							},
							{
								"name" : "RuneButton",
								"type" : "button",

								"x" : 76,
								"y" : 108,

								"tooltip_text" : "Rune System",

								"default_image" : "arezzo/runes_01.tga",
								"over_image" : "arezzo/runes_01.tga",
								"down_image" : "arezzo/runes_01.tga",
							},
							{
								"name" : "CostumeButton",
								"type" : "button",

								"x" : 78,
								"y" : 5,

								"tooltip_text_new" : uiScriptLocale.COSTUME_TITLE,
								"tooltip_text_color" : 0xfff1e6c0,
								"tooltip_x" : 0,
								"tooltip_y" : -35,

								"default_image" : "d:/ymir work/ui/game/taskbar/costume_Button_01.tga",
								"over_image" : "d:/ymir work/ui/game/taskbar/costume_Button_02.tga",
								"down_image" : "d:/ymir work/ui/game/taskbar/costume_Button_03.tga",
							},
							{
								"name" : "Equipment_Tab_01",
								"type" : "radio_button",

								"x" : 86,
								"y" : 161,

								"default_image" : "d:/ymir work/ui/game/windows/tab_button_small_01.sub",
								"over_image" : "d:/ymir work/ui/game/windows/tab_button_small_02.sub",
								"down_image" : "d:/ymir work/ui/game/windows/tab_button_small_03.sub",

								"children" :
								(
									{
										"name" : "Equipment_Tab_01_Print",
										"type" : "text",

										"x" : 0,
										"y" : 0,

										"all_align" : "center",

										"text" : "I",
									},
								),
							},
							{
								"name" : "Equipment_Tab_02",
								"type" : "radio_button",

								"x" : 86 + 32,
								"y" : 161,

								"default_image" : "d:/ymir work/ui/game/windows/tab_button_small_01.sub",
								"over_image" : "d:/ymir work/ui/game/windows/tab_button_small_02.sub",
								"down_image" : "d:/ymir work/ui/game/windows/tab_button_small_03.sub",

								"children" :
								(
									{
										"name" : "Equipment_Tab_02_Print",
										"type" : "text",

										"x" : 0,
										"y" : 0,

										"all_align" : "center",

										"text" : "II",
									},
								),
							},

						),
					},

					{
						"name" : "Inventory_Tab_01",
						"type" : "radio_button",

						"x" : 10,
						"y" : 33 + 191,

						"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_01.sub",
						"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_02.sub",
						"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_03.sub",

						"children" :
						(
							{
								"name" : "Inventory_Tab_01_Print",
								"type" : "text",

								"x" : 0,
								"y" : 0,

								"all_align" : "center",

								"text" : "I",
							},
						),
					},
					{
						"name" : "Inventory_Tab_02",
						"type" : "radio_button",

						"x" : 10 + 78,
						"y" : 33 + 191,

						"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_01.sub",
						"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_02.sub",
						"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_03.sub",

						"children" :
						(
							{
								"name" : "Inventory_Tab_02_Print",
								"type" : "text",

								"x" : 0,
								"y" : 0,

								"all_align" : "center",

								"text" : "II",
							},
						),
					},

					{
						"name" : "ItemSlot",
						"type" : "grid_table",

						"x" : 8,
						"y" : 246,

						"start_index" : 0,
						"x_count" : 5,
						"y_count" : 9,
						"x_step" : 32,
						"y_step" : 32,

						"image" : "d:/ymir work/ui/public/Slot_Base.sub"
					},

					{
						"name":"Money_Slot",
						"type":"button",

						"x":8,
						"y":28,

						"horizontal_align":"center",
						"vertical_align":"bottom",

						"default_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",
						"over_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",
						"down_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",

						"children" :
						(
							{
								"name":"Money_Icon",
								"type":"image",

								"x":-18,
								"y":2,

								"image":"d:/ymir work/ui/game/windows/money_icon.sub",
							},

							{
								"name" : "Money",
								"type" : "text",

								"x" : 3,
								"y" : 3,

								"horizontal_align" : "right",
								"text_horizontal_align" : "right",

								"text" : "123456789",
							},
						),
					},
					{
						"name":"Cheque_Icon",
						"type":"image",
						"vertical_align":"bottom",
						
						"x":10,
						"y":26,

						"image":"d:/ymir work/ui/game/windows/cheque_icon.sub",
					},
					{
						"name":"Cheque_Slot",
						"type":"button",

						"x":28,
						"y":28,

						"vertical_align":"bottom",

						"default_image" : "d:/ymir work/ui/public/cheque_slot.sub",
						"over_image" : "d:/ymir work/ui/public/cheque_slot.sub",
						"down_image" : "d:/ymir work/ui/public/cheque_slot.sub",

						"children" :
						(
							{
								"name" : "Cheque",
								"type" : "text",

								"x" : 3,
								"y" : 3,

								"horizontal_align" : "right",
								"text_horizontal_align" : "right",

								"text" : "99",
							},
						),
					},
				),
			},
		),
	}
