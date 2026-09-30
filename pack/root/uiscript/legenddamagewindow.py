import localeInfo
GUILD_PATH = "elvion/game/guild{}"

window = {
	"name" : "LegendDamageWindow",
	"style" : ("movable", "float",),
	"x" : SCREEN_WIDTH / 2 - 166,
	"y" : SCREEN_HEIGHT / 2 - 234,
	"width" : 332,
	"height" : 468,

	"children" :
	(
		{
			"name" : "Board",
			"type" : "image",
			"x" : 0,
			"y" : 0,

			"image" : ("elvion/game/ranking_boss/bg.png"),

			"children" :
			(
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),
					"x" : 21,
					"y" : 11,
					"width" : 290,
					"color" : "red",

					"children" :
					(
						{
							"name" : "TitleName",
							"type" : "text",
							"x" : 0,
							"y" : 3,
							"text" : localeInfo.RANKING_DAMAGE_BOSS,
							"horizontal_align" : "center",
							"text_horizontal_align" : "center",
						},
					),
				},
				{
					"name" : "rankPos",
					"type" : "image",
					"x" : 17+14,
					"y" : 39,
					"image" : GUILD_PATH.format("/member/combatant_icon.png"),
					"tooltip_text_new" : localeInfo.RANKING_DAMAGE_BOSS_POSITION,
					"tooltip_text_color" : 0xfff1e6c0,
					"tooltip_x" : 0,
					"tooltip_y" : -35,
				},
				{
					"name" : "playerName",
					"type" : "image",
					"x" : 17+95+6,
					"y" : 39,
					"image" : GUILD_PATH.format("/member/name_icon.png"),
					"tooltip_text_new" : localeInfo.RANKING_DAMAGE_PLAYER_NAME,
					"tooltip_text_color" : 0xfff1e6c0,
					"tooltip_x" : 0,
					"tooltip_y" : -35,
				},
				{
					"name" : "playerLevel",
					"type" : "image",
					"x" : 17+95+82-6,
					"y" : 39,
					"image" : GUILD_PATH.format("/member/level_icon.png"),
					"tooltip_text_new" : localeInfo.RANKING_DAMAGE_PLAYER_LEVEL,
					"tooltip_text_color" : 0xfff1e6c0,
					"tooltip_x" : 0,
					"tooltip_y" : -35,
				},
				{
					"name" : "damageDone",
					"type" : "image",
					"x" : 17+95+82+76-14,
					"y" : 39,
					"image" : GUILD_PATH.format("/member/class_icon.png"),
					"tooltip_text_new" : localeInfo.RANKING_DAMAGE_PLAYER_DAMAGE_DONE,
					"tooltip_text_color" : 0xfff1e6c0,
					"tooltip_x" : 0,
					"tooltip_y" : -35,
				},
			),
		},
	),
}