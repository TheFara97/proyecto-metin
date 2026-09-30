import uiScriptLocale
GUILD_PATH = "elvion/game/guild{}"

window = {
	"name" : "GuildWindow_BoardPage",

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
		
			"image" : GUILD_PATH.format("/member_auth/bg.png"),
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
			"name" : "GradeNumber", "type" : "image", "x" : 71, "y" : 40, "image" : "elvion/game/guild/member_auth/number_icon.png",
			"tooltip_text_new" : uiScriptLocale.GUILD_GRADE_NUM,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,
		},
		{
			"name" : "GradeName", "type" : "image", "x" : 77+145+22, "y" : 40, "image" : "elvion/game/guild/member/grade_icon.png",
			"tooltip_text_new" : uiScriptLocale.GUILD_GRADE_RANK,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,
		},
		{
			"name" : "InviteAuthority", "type" : "image", "x" : 71+145+143+3, "y" : 40, "image" : "elvion/game/guild/member_auth/accept_member_icon.png",
			"tooltip_text_new" : uiScriptLocale.GUILD_GRADE_PERMISSION_JOIN,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,
		},
		{
			"name" : "DriveOutAuthority", "type" : "image", "x" : 71+145+143+95-42, "y" : 40, "image" : "elvion/game/guild/member_auth/detele_member_icon.png",
			"tooltip_text_new" : uiScriptLocale.GUILD_GRADE_PERMISSION_DELETE,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,
		},
		{
			"name" : "NoticeAuthority", "type" : "image", "x" : 71+145+143+95+49-44, "y" : 40, "image" : "elvion/game/guild/member_auth/write_message_icon.png",
			"tooltip_text_new" : uiScriptLocale.GUILD_GRADE_PERMISSION_NOTICE,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,
		},
		{
			"name" : "SkillAuthority", "type" : "image", "x" : 71+145+143+95+49+49-45, "y" : 40, "image" : "elvion/game/guild/member_auth/skill_icon.png",
			"tooltip_text_new" : uiScriptLocale.GUILD_GRADE_PERMISSION_SKILL,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,
		},
		{
			"name" : "GuardManageAuthority", "type" : "image", "x" : 304+251, "y" : 40, "image" : "elvion/game/guild/member_auth/guard_manage_icon.png",
			"tooltip_text_new" : uiScriptLocale.GUILD_GRADE_PERMISSION_GUARD,
			"tooltip_text_color" : 0xfff1e6c0,
			"tooltip_x" : 0,
			"tooltip_y" : -35,
		},

	),
}
