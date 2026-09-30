import uiScriptLocale
GUILD_PATH = "elvion/game/guild{}"
LOCALE_PATH = uiScriptLocale.GUILD_PATH
window = {
	"name" : "GuildWindow",
	"style" : ("movable", "float",),
	"x" : SCREEN_WIDTH / 2 - 350,
	"y" : SCREEN_HEIGHT / 2 - 200,
	"width" : 656,
	"height" : 323,
	"children" :
	(
		# Add dummy tab windows that Python code expects
		{"name" : "Tab_01", "type" : "window", "x" : 0, "y" : 0, "width" : 1, "height" : 1},
		{"name" : "Tab_02", "type" : "window", "x" : 0, "y" : 0, "width" : 1, "height" : 1},
		{"name" : "Tab_03", "type" : "window", "x" : 0, "y" : 0, "width" : 1, "height" : 1},
		{"name" : "Tab_04", "type" : "window", "x" : 0, "y" : 0, "width" : 1, "height" : 1},
		{"name" : "Tab_05", "type" : "window", "x" : 0, "y" : 0, "width" : 1, "height" : 1},
		{"name" : "Tab_06", "type" : "window", "x" : 0, "y" : 0, "width" : 1, "height" : 1},
		
		# Your tab buttons
		{
			"name" : "Tab_Button_01",
			"type" : "radio_button",
			"x" : 5,
			"y" : 30,
			"default_image" : GUILD_PATH.format("/tab_home_norm.png"),
			"over_image" : GUILD_PATH.format("/tab_home_hover.png"),
			"down_image" : GUILD_PATH.format("/tab_home_down.png"),
		},
		{
			"name" : "Tab_Button_02",
			"type" : "radio_button",
			"x" : 5,
			"y" : 75,
			"default_image" : GUILD_PATH.format("/tab_armor_norm.png"),
			"over_image" : GUILD_PATH.format("/tab_armor_hover.png"),
			"down_image" : GUILD_PATH.format("/tab_armor_down.png"),
		},
		{
			"name" : "Tab_Button_03",
			"type" : "radio_button",
			"x" : 5,
			"y" : 120,
			"default_image" : GUILD_PATH.format("/tab_members_norm.png"),
			"over_image" : GUILD_PATH.format("/tab_members_hover.png"),
			"down_image" : GUILD_PATH.format("/tab_members_down.png"),
		},
		{
			"name" : "Tab_Button_04",
			"type" : "radio_button",
			"x" : 5,
			"y" : 165,
			"default_image" : GUILD_PATH.format("/tab_raid_norm.png"),
			"over_image" : GUILD_PATH.format("/tab_raid_hover.png"),
			"down_image" : GUILD_PATH.format("/tab_raid_down.png"),
		},
		{
			"name" : "Tab_Button_05",
			"type" : "radio_button",
			"x" : 5,
			"y" : 255,
			#"default_image" : GUILD_PATH.format("/tab_members_admin_norm.png"),
			#"over_image" : GUILD_PATH.format("/tab_members_admin_hover.png"),
			#"down_image" : GUILD_PATH.format("/tab_members_admin_down.png"),
			"default_image" : GUILD_PATH.format("/tab_logs_norm.png"),
			"over_image" : GUILD_PATH.format("/tab_logs_hover.png"),
			"down_image" : GUILD_PATH.format("/tab_logs_down.png"),
		},
		{
			"name" : "Tab_Button_06",
			"type" : "radio_button",
			"x" : 5,
			"y" : 210,
			"default_image" : GUILD_PATH.format("/tab_members_admin_norm.png"),
			"over_image" : GUILD_PATH.format("/tab_members_admin_hover.png"),
			"down_image" : GUILD_PATH.format("/tab_members_admin_down.png"),
		},
		#{
		#	"name" : "Board",
		#	"type" : "window",
		#	"x" : 39,
		#	"y" : 0,
		#	"width" : 656,
		#	"height" : 323,
		#},
	),
}