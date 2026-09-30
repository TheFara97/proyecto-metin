#-*- coding: iso-8859-1 -*-
import uiScriptLocale

SIZE_X = 769
SIZE_Y = 540

WIDTH = (SCREEN_WIDTH / 2) - (SIZE_X / 2)
HEIGHT = (SCREEN_HEIGHT / 2) - (SIZE_Y / 2)-40

PATH = "d:/ymir work/ui/battle_pass/"

window = {
	"name" : "BattlePassDialog",
	"style" : ("movable", "float",),
	"x" : WIDTH,
	"y" : HEIGHT,	
	"width" : SIZE_X,
	"height" : SIZE_Y,
	"children" :
	(
		{
			"name" : "mainWindow",
			"type" : "window",
			"style" : ("attach",),
			"x" : 0,"y" : 0,
			"width" : SIZE_X,
			"height" : SIZE_Y,
			"children" :
			(
				{
					"name" : "backgroundReward",
					"type" : "image",
					"style": ("attach",),
					"x" : 5,
					"y" : 30,
					"image" : PATH + "background_reward.png",
					"children" :
					(
						{
							"name" : "Free",
							"type" : "image",
							"x" : 80, "y" : 180,
							"image" : PATH + "free.png",
							"tooltip_text_new" : " Nagrody za darmow¹ wersjê ",
							"tooltip_text_color" : 0xfff1e6c0,
							"tooltip_x" : 0,
							"tooltip_y" : 0,
						},
						{
							"name" : "Premium",
							"type" : "image",
							"x" : 80, "y" : 310,
							"image" : PATH + "premium.png",
							"tooltip_text_new" : " Nagrody za p³atn¹ wersjê ",
							"tooltip_text_color" : 0xfff1e6c0,
							"tooltip_x" : 0,
							"tooltip_y" : 0,
						},
						{
							"name" : "backgroundLevel_r",
							"type" : "image",

							"x" : 405,
							"y" : 90,

							"image" : PATH + "background_level.png",
						},

						{
							"name" : "buttonBuyPremium",
							"type" : "button",
							"x" : 80,
							"y" : 460,
							"default_image" : PATH + "buttons/buy_premium0.png",
							"over_image" : PATH + "buttons/buy_premium1.png",
							"down_image" : PATH + "buttons/buy_premium2.png",
							"tooltip_text_new" : " Cena zakupu: 500 SM ",
							"tooltip_text_color" : 0xfff1e6c0,
							"tooltip_x" : 0,
							"tooltip_y" : 45,
						},

						{
							"name" : "buttonMoveQuest",
							"type" : "button",

							"x" : 220,
							"y" : 460,

							"default_image" : PATH + "buttons/quest0.png",
							"over_image" : PATH + "buttons/quest1.png",
							"down_image" : PATH + "buttons/quest2.png",
						},

						{
							"name" : "buttonPreviousPage_r",
							"type" : "button",

							"x" : 535,
							"y" : 460,

							"default_image" : PATH + "buttons/previous0.png",
							"over_image" : PATH + "buttons/previous1.png",
							"down_image" : PATH + "buttons/previous2.png",
						},

						{
							"name" : "buttonNextPage_r",
							"type" : "button",

							"x" : 613,
							"y" : 460,

							"default_image" : PATH + "buttons/next0.png",
							"over_image" : PATH + "buttons/next1.png",
							"down_image" : PATH + "buttons/next2.png",
						},
					),
				},

				{
					"name" : "backgroundQuest",
					"type" : "image",
					"style": ("attach",),
					"x" : 5,
					"y" : 30,
					"image" : PATH + "background_quest.png",
					"children" :
					(
						{
							"name" : "backgroundLevel_q",
							"type" : "image",

							"x" : 405,
							"y" : 90,

							"image" : PATH + "background_level.png",
						},

						{
							"name" : "buttonMoveReward",
							"type" : "button",

							"x" : 88,
							"y" : 460,

							"default_image" : PATH + "buttons/back0.png",
							"over_image" : PATH + "buttons/back1.png",
							"down_image" : PATH + "buttons/back2.png",
						},

						{
							"name" : "buttonPreviousPage_q",
							"type" : "button",

							"x" : 535,
							"y" : 460,

							"default_image" : PATH + "buttons/previous0.png",
							"over_image" : PATH + "buttons/previous1.png",
							"down_image" : PATH + "buttons/previous2.png",
						},

						{
							"name" : "buttonNextPage_q",
							"type" : "button",

							"x" : 613,
							"y" : 460,

							"default_image" : PATH + "buttons/next0.png",
							"over_image" : PATH + "buttons/next1.png",
							"down_image" : PATH + "buttons/next2.png",
						},
					),
				},
				{
					"name" : "ExitButton",
					"type" : "button",
					"x" : 693, "y" : 96,
					"default_image" : PATH+"exit0.png",
					"over_image" : PATH+"exit1.png",
					"down_image" : PATH+"exit2.png",
				},
			),
		},
	),
}