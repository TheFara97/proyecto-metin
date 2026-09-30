import uiScriptLocale

ROOT = "d:/ymir work/ui/minimap/"

window = {
	"name" : "AtlasWindow",
	"style" : ("movable", "float",),

	"x" : 40,
	"y" : 130,

	"width" : 256 + 15,
	"height" : 256 + 38,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",

			"x" : 0,
			"y" : 0,

			"width" : 256 + 15,
			"height" : 256 + 38,

			"title" : uiScriptLocale.ZONE_MAP,
		},
		{
			"name" : "info",
			"type" : "thinboard",
		
			"x" : 0,
			"y" : 0,
		
			"width" : 10,
			"height" : 10,
		
			"children" : 
			(
				{
					"name" : "info_text1",
					"type" : "text",
		
					"x" : 15,
					"y" : 17,
					
					"outline": 1,
					"text" : "Name:",
				},
				{
					"name" : "info_text2",
					"type" : "text",
		
					"x" : 15,
					"y" : 32,
					
					"outline": 1,
					"text" : "Coordinates:",
				},
				{
					"name" : "info_text3",
					"type" : "text",
		
					"x" : 15,
					"y" : 47,
					
					"outline": 1,
					"text" : "Respawn:",
				},
				{
					"name" : "info_text4",
					"type" : "text",
		
					"x" : 15,
					"y" : 62,
					
					"outline": 1,
					"text" : "Cooldown:",
				},
				{
					"name" : "info_text5",
					"type" : "text",
		
					"x" : 15,
					"y" : 80,
					
					"outline": 1,
					"text" : "Open wiki",
				}
			),
		},
	),
}
