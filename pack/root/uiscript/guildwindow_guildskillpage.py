import uiScriptLocale

ADD_HEIGHT = 17
LOCALE_PATH = uiScriptLocale.WINDOWS_PATH
GUILD_PATH = "elvion/game/guild{}"

window = {
	"name" : "GuildWindow_GuildSkillPage",

	"x" : 39,
	"y" : 0,

	"width" : 656-39,
	"height" : 323,

	"children" :
	(
		{
			"name" : "bg",
			"type" : "image",
		
			"x" : 0,
			"y" : 0,
		
			"image" : GUILD_PATH.format("/guild_logs/bg.png"),
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
		{
			"name" : "LogsScrollBar",
			"type" : "modern_scrollbar",
			"x" : 589,
			"y" : 68,
			"width": 6,
			"size": 234,
			"content_height": 234,  # Will be updated dynamically based on log count
			"smooth": 1,
			"scroll_step": 0.30,
		},

		{
			"name" : "guild_selected_raid_header",
			"type" : "window",
			"x" : 11, "y" : 36,
			"width" : 591, "height" : 26,
			"children":
			(
				{ 
					"name":"Guild_Info_Point_Value",
					"type":"text",
					"x":0,
					"y":0,
					"text":uiScriptLocale.GUILD_LOGS_HEADER,
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFdcc2a9,
					"all_align" : "center",
				},
			),
		},

		{
			"name" : "RefreshLogsButton",
			"type" : "button",
			"x" : 559,
			"y" : 40,
			"text" : "",
			"tooltip_text" : uiScriptLocale.GUILD_LOGS_REFRESH,
			"default_image" : GUILD_PATH.format("/guild_logs/refresh_btn_norm.png"),
			"over_image" :  GUILD_PATH.format("/guild_logs/refresh_btn_hover.png"),
			"down_image" :  GUILD_PATH.format("/guild_logs/refresh_btn_down.png"),
		},
		{
			"name" : "SaveLogsButton",
			"type" : "button",
			"x" : 580,
			"y" : 40,
			"text" : "",
			"tooltip_text" : uiScriptLocale.GUILD_LOGS_SAVE,
			"default_image" : GUILD_PATH.format("/guild_logs/save_btn_norm.png"),
			"over_image" :  GUILD_PATH.format("/guild_logs/save_btn_hover.png"),
			"down_image" :  GUILD_PATH.format("/guild_logs/save_btn_down.png"),
		},

	),
}
