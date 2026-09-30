import uiScriptLocale
import localeInfo

ROOT = "d:/ymir work/ui/game/"

Y_ADD_POSITION = 0
window = {
	"name" : "ExpandTaskBar",

	"x" : SCREEN_WIDTH/2 - 142-135-19,
	"y" : SCREEN_HEIGHT - 94,

	"width" : 40 * 16,
	"height" : 37,

	"children" :
	(
		{
			"name" : "ExpanedTaskBar_Board",
			"type" : "window",

			"x" : 0,
			"y" : 0,

			"width" : 40 * 14,
			"height" : 37,

			"children" :
			(

				{
					"name" : "PotionsButton",
					"type" : "button",
					"style" : ("ltr", ),
					"x" : 0,
					"y" : 0,
					"width" : 37,
					"height" : 37,
					"tooltip_text" : localeInfo.EXPANDEDTASKBAR_POTIONS,
					"default_image" : "elvion/game/taskbar/expanded_buttons/potions_norm.png",
					"over_image" : "elvion/game/taskbar/expanded_buttons/potions_hover.png",
					"down_image" : "elvion/game/taskbar/expanded_buttons/potions_down.png",
				},

				{
					"name" : "OfflineShopButton",
					"type" : "button",
					"style" : ("ltr", ),
					"x" : 40 * 1,
					"y" : 0,
					"width" : 37,
					"height" : 37,
					"tooltip_text" : localeInfo.EXPANDEDTASKBAR_OFFLINESHOP,
					"default_image" : "elvion/game/taskbar/expanded_buttons/offlineshop_norm.png",
					"over_image" : "elvion/game/taskbar/expanded_buttons/offlineshop_hover.png",
					"down_image" : "elvion/game/taskbar/expanded_buttons/offlineshop_down.png",
				},

				{
					"name" : "CompanionButton",
					"type" : "button",
					"style" : ("ltr", ),
					"x" : 40 * 2,
					"y" : 0,
					"width" : 37,
					"height" : 37,
					"tooltip_text" : localeInfo.EXPANDEDTASKBAR_COMPANION,
					"default_image" : "elvion/game/taskbar/expanded_buttons/companion_norm.png",
					"over_image" : "elvion/game/taskbar/expanded_buttons/companion_hover.png",
					"down_image" : "elvion/game/taskbar/expanded_buttons/companion_down.png",
					"children" :
					(
						{
						"name" : "CompanionSkillDot",
						"type" : "image",
						"x" : 22,
						"y" : 0,
						"image" : "elvion/game/companion/skillpoint_notify.png",
						},
					),
				},

				{
					"name" : "ShopSearchButton",
					"type" : "button",
					"style" : ("ltr", ),
					"x" : 40 * 3,
					"y" : 0,
					"width" : 37,
					"height" : 37,
					"tooltip_text" : localeInfo.EXPANDEDTASKBAR_SHOPSEARCH,
					"default_image" : "elvion/game/taskbar/expanded_buttons/searchshop_norm.png",
					"over_image" : "elvion/game/taskbar/expanded_buttons/searchshop_hover.png",
					"down_image" : "elvion/game/taskbar/expanded_buttons/searchshop_down.png",
				},

				{
					"name" : "BonusesButton",
					"type" : "button",
					"style" : ("ltr", ),
					"x" : 40 * 4,
					"y" : 0,
					"width" : 37,
					"height" : 37,
					"tooltip_text" : localeInfo.EXPANDEDTASKBAR_BONUSES,
					"default_image" : "elvion/game/taskbar/expanded_buttons/bonuses_norm.png",
					"over_image" : "elvion/game/taskbar/expanded_buttons/bonuses_hover.png",
					"down_image" : "elvion/game/taskbar/expanded_buttons/bonuses_down.png",
				},

				{
					"name" : "SwitchbotButton",
					"type" : "button",
					"style" : ("ltr", ),
					"x" : 40 * 5,
					"y" : 0,
					"width" : 37,
					"height" : 37,
					"tooltip_text" : localeInfo.EXPANDEDTASKBAR_SWITCHBOT,
					"default_image" : "elvion/game/taskbar/expanded_buttons/switchbot_norm.png",
					"over_image" : "elvion/game/taskbar/expanded_buttons/switchbot_hover.png",
					"down_image" : "elvion/game/taskbar/expanded_buttons/switchbot_down.png",
				},

				{
					"name" : "AutoBuffButton",
					"type" : "button",
					"style" : ("ltr", ),
					"x" : 40 * 6,
					"y" : 0,
					"width" : 37,
					"height" : 37,
					"tooltip_text" : localeInfo.EXPANDEDTASKBAR_AUTOBUFF,
					"default_image" : "elvion/game/taskbar/expanded_buttons/autobuff_norm.png",
					"over_image" : "elvion/game/taskbar/expanded_buttons/autobuff_hover.png",
					"down_image" : "elvion/game/taskbar/expanded_buttons/autobuff_down.png",
				},

				{
					"name" : "RankingButton",
					"type" : "button",
					"style" : ("ltr", ),
					"x" : 40 * 7,
					"y" : 0,
					"width" : 37,
					"height" : 37,
					"tooltip_text" : localeInfo.EXPANDEDTASKBAR_RANKING,
					"default_image" : "elvion/game/taskbar/expanded_buttons/ranking_norm.png",
					"over_image" : "elvion/game/taskbar/expanded_buttons/ranking_hover.png",
					"down_image" : "elvion/game/taskbar/expanded_buttons/ranking_down.png",
				},

				{
					"name" : "BinButton",
					"type" : "button",
					"style" : ("ltr", ),
					"x" : 40 * 8,
					"y" : 0,
					"width" : 37,
					"height" : 37,
					"tooltip_text" : localeInfo.EXPANDEDTASKBAR_BIN,
					"default_image" : "elvion/game/taskbar/expanded_buttons/delete_norm.png",
					"over_image" : "elvion/game/taskbar/expanded_buttons/delete_hover.png",
					"down_image" : "elvion/game/taskbar/expanded_buttons/delete_down.png",
				},

				{
					"name" : "RuneButton",
					"type" : "button",
					"style" : ("ltr", ),
					"x" : 40 * 9,
					"y" : 0,
					"width" : 37,
					"height" : 37,
					"tooltip_text" : localeInfo.EXPANDEDTASKBAR_RUNE,
					"default_image" : "elvion/game/taskbar/expanded_buttons/runes_norm.png",
					"over_image" : "elvion/game/taskbar/expanded_buttons/runes_hover.png",
					"down_image" : "elvion/game/taskbar/expanded_buttons/runes_down.png",
				},

				{
					"name" : "ArtefaktButton",
					"type" : "button",
					"style" : ("ltr", ),
					"x" : 40 * 10,
					"y" : 0,
					"width" : 37,
					"height" : 37,
					"tooltip_text" : localeInfo.EXPANDEDTASKBAR_ARTEFAKT,
					"default_image" : "elvion/game/taskbar/expanded_buttons/artifacts_norm.png",
					"over_image" : "elvion/game/taskbar/expanded_buttons/artifacts_hover.png",
					"down_image" : "elvion/game/taskbar/expanded_buttons/artifacts_down.png",
				},

				{
					"name" : "ChangeEquipmentButton",
					"type" : "button",
					"style" : ("ltr", ),
					"x" : 40 * 11,
					"y" : 0,
					"width" : 37,
					"height" : 37,
					"tooltip_text" : localeInfo.EXPANDEDTASKBAR_CHANGEEQUIPMENT,
					"default_image" : "elvion/game/taskbar/expanded_buttons/changeequipment_norm.png",
					"over_image" : "elvion/game/taskbar/expanded_buttons/changeequipment_hover.png",
					"down_image" : "elvion/game/taskbar/expanded_buttons/changeequipment_down.png",
				},

				{
					"name" : "StorageButton",
					"type" : "button",
					"style" : ("ltr", ),
					"x" : 40 * 12,
					"y" : 0,
					"width" : 37,
					"height" : 37,
					"tooltip_text" : localeInfo.EXPANDEDTASKBAR_STORAGE,
					"default_image" : "elvion/game/taskbar/expanded_buttons/storage_norm.png",
					"over_image" : "elvion/game/taskbar/expanded_buttons/storage_hover.png",
					"down_image" : "elvion/game/taskbar/expanded_buttons/storage_down.png",
				},

				{
					"name" : "SaveLocationButton",
					"type" : "button",
					"style" : ("ltr", ),
					"x" : 40 * 13,
					"y" : 0,
					"width" : 37,
					"height" : 37,
					"tooltip_text" : localeInfo.EXPANDEDTASKBAR_SAVELOCATION,
					"default_image" : "elvion/game/taskbar/expanded_buttons/save_location_norm.png",
					"over_image" : "elvion/game/taskbar/expanded_buttons/save_location_hover.png",
					"down_image" : "elvion/game/taskbar/expanded_buttons/save_location_down.png",
				},

				{
					"name" : "WikiButton",
					"type" : "button",
					"style" : ("ltr", ),
					"x" : 40 * 14,
					"y" : 0,
					"width" : 37,
					"height" : 37,
					"tooltip_text" : localeInfo.EXPANDEDTASKBAR_WIKI,
					"default_image" : "elvion/game/taskbar/expanded_buttons/01_normal.png",
					"over_image" : "elvion/game/taskbar/expanded_buttons/01_hover.png",
					"down_image" : "elvion/game/taskbar/expanded_buttons/01_down.png",
				},
			),
		},
	),
}
