import uiScriptLocale
import item

BOARD_WIDTH = 512
BOARD_HEIGHT = 475

window = {
	"name" : "ArtefaktSystemWindow",

	"x" : 0,
	"y" : 0,

	"style" : ("movable", "float",),

	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,

	"children" :
	(
		{
			"name" : "Board",
			"type" : "board",
			"style" : ("movable","float",),

			"x" : 0,
			"y" : 0,

			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGHT,
			
			"children" :
			(
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 19,
					"y" : 7,

					"width" : BOARD_WIDTH-35,
					"color" : "yellow",

					"children" :
					(
						{ "name":"TitleName", "x": 0, "y": -3, "type":"text", "text": uiScriptLocale.ARTEFAKT_WINDOW_TITLE, "horizontal_align" : "center", "outline":"1","all_align" : "center", },
					),
				},
				
				{
					"name" : "Artefakt_Base",
					"type" : "image",

					"x" : 0,
					"y" : 29,
					
					"image" : "arezzo/artefakty.png",
					"horizontal_align":"center",		

					"children" :
					(

						{
							"name" : "ArtefaktSlots",
							"type" : "slot",

							"x" : 3,
							"y" : 3,

							"width" : 462,
							"height" : 432,

							"slot" : (
										{"index":item.EQUIPMENT_ARTEFAKT6, "x":60, "y":102, "width":32, "height":32},
										{"index":item.EQUIPMENT_ARTEFAKT7, "x":46, "y":155, "width":32, "height":32},
										{"index":item.EQUIPMENT_ARTEFAKT8, "x":50, "y":207, "width":32, "height":32},
										{"index":item.EQUIPMENT_ARTEFAKT9, "x":397, "y":102, "width":32, "height":32},
										{"index":item.EQUIPMENT_ARTEFAKT10, "x":411, "y":155, "width":32, "height":32},
										{"index":item.EQUIPMENT_ARTEFAKT11, "x":407, "y":207, "width":32, "height":32},

										{"index":item.EQUIPMENT_ARTEFAKT1, "x":149, "y":107, "width":32, "height":32},
										{"index":item.EQUIPMENT_ARTEFAKT2, "x":137, "y":185, "width":32, "height":32},
										{"index":item.EQUIPMENT_ARTEFAKT3, "x":311, "y":107, "width":32, "height":32},
										{"index":item.EQUIPMENT_ARTEFAKT4, "x":320, "y":185, "width":32, "height":32},
										{"index":item.EQUIPMENT_ARTEFAKT5, "x":230, "y":66, "width":32, "height":32},
										{"index":item.EQUIPMENT_ARTEFAKT12, "x":230, "y":251, "width":32, "height":32},
										{"index":item.EQUIPMENT_ARTEFAKT13, "x":65, "y":255, "width":32, "height":32},
										{"index":item.EQUIPMENT_ARTEFAKT14, "x":393, "y":259, "width":32, "height":32},
									),
						},
					),
				},
			),
		},
	),
}