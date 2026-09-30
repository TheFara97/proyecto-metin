import uiScriptLocale

ROOT = "d:/ymir work/ui/game/taskbar/"
ELVION = "elvion/game/taskbar/"

window = {
	"name" : "ButtonWindow",

	"x" : 0,
	"y" : 0,

	"width" : 32,
	"height" : 32 * 4,

	"children" :
	[
		{
			"name" : "button_move_and_attack",
			"type" : "button",

			"x" : 0,
			"y" : 0,

			"tooltip_text_new" : uiScriptLocale.MOUSEBUTTON_ATTACK,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,

			"default_image" : ELVION + "attack_norm.png",
			"over_image" : ELVION + "attack_hover.png",
			"down_image" : ELVION + "attack_down.png",
		},
		{
			"name" : "button_auto_attack",
			"type" : "button",

			"x" : 0,
			"y" : 32,

			"tooltip_text_new" : uiScriptLocale.MOUSEBUTTON_AUTO_ATTACK,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,

			"default_image" : ELVION + "auto_attack_norm.png",
			"over_image" : ELVION + "auto_attack_hover.png",
			"down_image" : ELVION + "auto_attack_down.png",
		},
		{
			"name" : "button_camera",
			"type" : "button",

			"x" : 0,
			"y" : 64,

			"tooltip_text_new" : uiScriptLocale.MOUSEBUTTON_CAMERA,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,

			"default_image" : ELVION + "camera_norm.png",
			"over_image" : ELVION + "camera_hover.png",
			"down_image" : ELVION + "camera_down.png",
		},
		{
			"name" : "button_queue_on",
			"type" : "button",
			"x" : 0,
			"y" : 64 + 32,
			"tooltip_text_new" : uiScriptLocale.MOUSEBUTTON_QUEUE_ON,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,
			"default_image" : ELVION + "queue_01.png",
			"over_image" : ELVION + "queue_02.png",
			"down_image" : ELVION + "queue_03.png",
		},
	],

}