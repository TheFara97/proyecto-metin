import uiScriptLocale

SMALL_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_01.sub"
LARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_03.sub"
XLARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_04.sub"
GUILD_PATH = "elvion/game/guild{}"

window = {
	"name" : "GuildWindow_MemberPage",

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
		
			"image" : GUILD_PATH.format("/member/bg.png"),
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
			"name" : "name_icon",
			"type" : "image",
			"x" : 71,
			"y" : 40,
			"image" : GUILD_PATH.format("/member/name_icon.png"),
			"tooltip_text_new" : uiScriptLocale.GUILD_MEMBER_NAME,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,
		},
		{
			"name" : "grade_icon",
			"type" : "image",
			"x" : 71+145,
			"y" : 40,
			"image" : GUILD_PATH.format("/member/grade_icon.png"),
			"tooltip_text_new" : uiScriptLocale.GUILD_MEMBER_RANK,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,
		},
		{
			"name" : "class_icon",
			"type" : "image",
			"x" : 71+145+143,
			"y" : 40,
			"image" : GUILD_PATH.format("/member/class_icon.png"),
			"tooltip_text_new" : uiScriptLocale.GUILD_MEMBER_JOB,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,
		},
		{
			"name" : "level_icon",
			"type" : "image",
			"x" : 71+145+143+95,
			"y" : 40,
			"image" : GUILD_PATH.format("/member/level_icon.png"),
			"tooltip_text_new" : uiScriptLocale.GUILD_MEMBER_LEVEL,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,
		},
		{
			"name" : "donated_icon",
			"type" : "image",
			"x" : 71+145+143+95+49,
			"y" : 40,
			"image" : GUILD_PATH.format("/member/donate_icon.png"),
			"tooltip_text_new" : uiScriptLocale.GUILD_MEMBER_SPECIFIC_GRAVITY,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,
		},
		{
			"name" : "co_leader_icon",
			"type" : "image",
			"x" : 71+145+143+95+49+49,
			"y" : 40,
			"image" : GUILD_PATH.format("/member/combatant_icon.png"),
			"tooltip_text_new" : uiScriptLocale.GUILD_MEMBER_KNIGHT,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,
		},
			
		# Guild passive skills bonus
		{
			"name" : "window_members",
			"type" : "window",
			"width": 585,
			"height": 238,
			"x" :4,
			"y" : 67,
			"children":
			(
				#{
				#	"name" : "IndexName", "type" : "text", "x" : 43, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_NAME,
				#},
				#{
				#	"name" : "IndexGrade", "type" : "text", "x" : 119, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_RANK,
				#},
				#{
				#	"name" : "IndexJob", "type" : "text", "x" : 177, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_JOB,
				#},
				#{
				#	"name" : "IndexLevel", "type" : "text", "x" : 217, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_LEVEL,
				#},
				#{
				#	"name" : "IndexOffer", "type" : "text", "x" : 251, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_SPECIFIC_GRAVITY,
				#},
				#{
				#	"name" : "IndexGeneral", "type" : "text", "x" : 304, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_KNIGHT,
				#},
				
			),
		},
		{
			"name" : "ScrollBar",
			"type" : "modern_scrollbar",
			"x" : 596,
			"y" : 67,
			"width" : 6,
			"size" : 238,
			"content_height" : 238,
			"smooth" : 1,
			"scroll_step" : 0.15,
		},
	),
}
