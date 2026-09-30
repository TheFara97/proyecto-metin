import localeInfo
import uiScriptLocale

TITLE_PATH = "elvion/game/title_achievement{}"

window = {
	"name": "TitleAchievementWindow",
	"style" : ("movable", "float",),

	"x" : 39,
	"y" : 0,

	"width" : 497,
	"height" : 368,
	"children": 
	(
		{
			"name" : "bg",
			"type" : "image",
		
			"x" : 0,
			"y" : 0,
		
			"image" : TITLE_PATH.format("/bg.png"),
		},
		{
			"name" : "TitleBar",
			"type" : "titlebar",
			"style" : ("attach",),
	
			"x" : 21,
			"y" : 12,
	
			"width" : 456,
			"color" : "red",
	
			"children" :
			(
				{
					"name" : "TitleName",
					"type" : "text",
					"text": localeInfo.TITLE_ACHIEVEMENT_WINDOW_TITLEBAR,
					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"outline": 1,
					"x" : 0,
					"y" : 2,
				},
			),
		},
		{
			"name": "allTitlesBtn", 
			"type": "button",
			"x": 15+121, 
			"y": 36,
			"default_image": TITLE_PATH.format("/btn_norm.png"),
			"over_image": TITLE_PATH.format("/btn_hover.png"),
			"down_image": TITLE_PATH.format("/btn_down.png"),
			"children":
			(
				{ 
					"name":"allTitlesBtnText",
					"type":"text",
					"x":0,
					"y":-1,
					"text": localeInfo.TITLE_ACHIEVEMENT_WINDOW_ALL_TITLES,
					"outline" : 1,
					"fontname":"Verdana:12b",
					"color":0xFFf5e2d0,
					"all_align" : "center",
				},
			),
		},
		{
			"name": "ownedTitlesButton", 
			"type": "button",
			"x": 15+117+121, 
			"y": 36,
			"default_image": TITLE_PATH.format("/btn_norm.png"),
			"over_image": TITLE_PATH.format("/btn_hover.png"),
			"down_image": TITLE_PATH.format("/btn_down.png"),
			"children":
			(
				{ 
					"name":"ownedTitlesBtnText",
					"type":"text",
					"x":0,
					"y":-1,
					"text": localeInfo.TITLE_ACHIEVEMENT_WINDOW_UNLOCKED_TITLES,
					"outline" : 1,
					"fontname":"Verdana:12b",
					"color":0xFFf5e2d0,
					"all_align" : "center",
				},
			),
		},
		{
			"name": "favouriteButton", 
			"type": "button",
			"x": 15+117+117+121, 
			"y": 36,
			"default_image": TITLE_PATH.format("/btn_norm.png"),
			"over_image": TITLE_PATH.format("/btn_hover.png"),
			"down_image": TITLE_PATH.format("/btn_down.png"),
			"children":
			(
				{ 
					"name":"ownedTitlesBtnText",
					"type":"text",
					"x":0,
					"y":-1,
					"text": localeInfo.TITLE_ACHIEVEMENT_WINDOW_FAVOURITE_TITLES,
					"outline" : 1,
					"fontname":"Verdana:12b",
					"color":0xFFf5e2d0,
					"all_align" : "center",
				},
			),
		},
		{
			"name" : "titlesHeader",
			"type" : "window",
			
			"x" : 17,
			"y" : 67,
			
			"width" : 464,
			"height" : 27,
			
			"children" :
			(
				{ 
					"name":"titlesList",
					"type":"text",
					"x":0,
					"y":-1,
					"text": localeInfo.TITLE_ACHIEVEMENT_WINDOW_TITLES,
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFf5e2d0,
					"all_align" : "center",
				},
			),
		},
		## Search Field Container
		{
			"name" : "SearchFieldContainer",
			"type" : "window",
			
			"x" : 16,
			"y" : 37,
			
			"width" : 144,
			"height" : 33,
			
			#"children" :
			#(
			#	{
			#		"name" : "SearchFieldImage",
			#		"type" : "image",
			#		
			#		"x" : 0,
			#		"y" : 0,
			#		
			#		"image" : TITLE_PATH.format("/title_search.png"),
			#	},
			#),
		},
		{
			"name" : "titlesScrollbar",
			"type" : "modern_scrollbar",
			"x" : 474,
			"y" : 96,
			"width": 6,
			"size": 254,
			"content_height": 256,
			"smooth": 1,
			"scroll_step": 0.30,
		},
	),
}