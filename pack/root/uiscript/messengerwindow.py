import uiScriptLocale
import localeInfo

FRIENDLIST_PATH = "elvion/game/friendlist{}"

window = {
	"name" : "MessengerWindow",

	"x" : SCREEN_WIDTH - 210,
	"y" : SCREEN_HEIGHT - 400 - 50,

	"style" : ("movable", "float", "animate",),

	"width" : 237,
	"height" : 360,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",

			"x" : 0,
			"y" : 0,

			"width" : 237,
			"height" : 360,
			"title" : localeInfo.MESSENGER_TITLE,
		},

		{
			"name" : "SearchBG",
			"type" : "image",
			"x" : 11,
			"y" : 77,
			"image" : FRIENDLIST_PATH.format("/search_bg.png"),
			"children" : (
				{
					"name" : "editline",
					"type" : "editline",
					"x" : 3,
					"y" : 4,
					"width" : 110,
					"height" : 15,
					"input_limit" : 18,
				},

				{
					"name" : "clear_btn",
					"type" : "button",
					"x" : 164,
					"y" : 4,
					"tooltip_text" : "Clear",
					"tooltip_x" : 0,
					"tooltip_y" : 35,
					"default_image" : FRIENDLIST_PATH.format("/buttons/clear_btn_0.png"),
					"over_image" : FRIENDLIST_PATH.format("/buttons/clear_btn_1.png"),
					"down_image" : FRIENDLIST_PATH.format("/buttons/clear_btn_2.png"),
				},
				{
					"name" : "search_btn",
					"type" : "button",
					"x" : 183,
					"y" : 1,
					"tooltip_x" : 0,
					"tooltip_y" : 35,
					"default_image" : FRIENDLIST_PATH.format("/buttons/search_btn_0.png"),
					"over_image" : FRIENDLIST_PATH.format("/buttons/search_btn_1.png"),
					"down_image" : FRIENDLIST_PATH.format("/buttons/search_btn_2.png"),
				},
			),
		},

		{
			"name" : "content_bg",
			"type" : "image",
			"x" : 9,
			"y" : 106,
			"image" : FRIENDLIST_PATH.format("/content_bg.png"),
		},

		{
			"name" : "character_bg",
			"type" : "image",
			"x" : 11,
			"y" : 33,
			"image" : FRIENDLIST_PATH.format("/character_bg.png"),
		},

		{
			"name" : "AddFriendButton",
			"type" : "button",

			"x" : 8 + (36 * 0),
			"y" : 36 + 28 + 214 + 45,

			"tooltip_text" : localeInfo.MESSENGER_ADD_FRIEND,
			"tooltip_x" : 0,
			"tooltip_y" : 35,

			"default_image" : FRIENDLIST_PATH.format("/buttons/add_friend_norm.png"),
			"over_image" : FRIENDLIST_PATH.format("/buttons/add_friend_hover.png"),
			"down_image" : FRIENDLIST_PATH.format("/buttons/add_friend_down.png"),
			"disable_image" : FRIENDLIST_PATH.format("/buttons/add_friend_disabled.png"),
		},
		{
			"name" : "WhisperButton",
			"type" : "button",

			"x" : 8 + (36 * 1),
			"y" : 36 + 28 + 214 + 45,

			"tooltip_text" : localeInfo.MESSENGER_WHISPER,
			"tooltip_x" : 0,
			"tooltip_y" : 35,

			"default_image" : FRIENDLIST_PATH.format("/buttons/messenger_btn_norm.png"),
			"over_image" : FRIENDLIST_PATH.format("/buttons/messenger_btn_hover.png"),
			"down_image" : FRIENDLIST_PATH.format("/buttons/messenger_btn_down.png"),
			"disable_image" : FRIENDLIST_PATH.format("/buttons/messenger_btn_disabled.png"),
		},
		{
			"name" : "MobileButton",
			"type" : "button",

			"x" : 8 + (36 * 5),
			"y" : 36 + 28 + 214 + 45,

			"tooltip_text" : localeInfo.MESSENGER_MOBILE,
			"tooltip_x" : 0,
			"tooltip_y" : 35,

			"default_image" : "d:/ymir work/ui/game/windows/messenger_mobile_01.sub",
			"over_image" : "d:/ymir work/ui/game/windows/messenger_mobile_02.sub",
			"down_image" : "d:/ymir work/ui/game/windows/messenger_mobile_03.sub",
			"disable_image" : "d:/ymir work/ui/game/windows/messenger_mobile_04.sub",
		},

		{
			"name" : "RemoveButton",
			"type" : "button",

			"x" : 8 + (36 * 2),
			"y" : 36 + 28 + 214 + 45,
			
			"tooltip_text" : localeInfo.MESSENGER_DELETE_FRIEND,
			"tooltip_x" : 0,
			"tooltip_y" : 35,
			
			"default_image" : FRIENDLIST_PATH.format("/buttons/delete_norm.png"),
			"over_image" : FRIENDLIST_PATH.format("/buttons/delete_hover.png"),
			"down_image" : FRIENDLIST_PATH.format("/buttons/delete_down.png"),
			"disable_image" : FRIENDLIST_PATH.format("/buttons/delete_disabled.png"),
		},

		{
			"name" : "BlockButton",
			"type" : "button",

			"x" : 8 + (36 * 3),
			"y" : 36 + 28 + 214 + 45,

			"tooltip_text" : "Block",
			"tooltip_x" : 0,
			"tooltip_y" : 35,

			"default_image" : FRIENDLIST_PATH.format("/buttons/mute_norm.png"),
			"over_image" : FRIENDLIST_PATH.format("/buttons/mute_hover.png"),
			"down_image" : FRIENDLIST_PATH.format("/buttons/mute_down.png"),
			"disable_image" : FRIENDLIST_PATH.format("/buttons/mute_disabled.png"),
		},

		{
			"name" : "GuildButton",
			"type" : "button",

			"x" : 8 + (36 * 4),
			"y" : 36 + 28 + 214 + 45,

			"tooltip_text" : localeInfo.MESSENGER_OPEN_GUILD,
			"tooltip_x" : 0,
			"tooltip_y" : 35,

			"default_image" : FRIENDLIST_PATH.format("/buttons/guild_norm.png"),
			"over_image" : FRIENDLIST_PATH.format("/buttons/guild_hover.png"),
			"down_image" : FRIENDLIST_PATH.format("/buttons/guild_down.png"),
			"disable_image" : FRIENDLIST_PATH.format("/buttons/guild_disabled.png"),
		},

		{
			"name" : "SupportButton",
			"type" : "button",

			"x" : 8 + (36 * 5),
			"y" : 36 + 28 + 214 + 45,

			"tooltip_text" : localeInfo.MESSENGER_SUPPORT,
			"tooltip_x" : 0,
			"tooltip_y" : 35,

			"default_image" : FRIENDLIST_PATH.format("/buttons/gm_btn_norm.png"),
			"over_image" : FRIENDLIST_PATH.format("/buttons/gm_btn_hover.png"),
			"down_image" : FRIENDLIST_PATH.format("/buttons/gm_btn_down.png"),
			"disable_image" : FRIENDLIST_PATH.format("/buttons/gm_btn_down.png"),
		},

	),
}