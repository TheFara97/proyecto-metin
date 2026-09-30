import uiScriptLocale

window = {
	"name" : "SelectSkillDialog",
	"style" : ("movable", "float",),

	"x" : 0,
	"y" : 0,
						
	"width" : 300,
	"height" : 235,

	"children" :
	(
		{
			"name" : "Board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : 300,
			"height" : 205,

			"children" :
			(		
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 6,
					"y" : 7,

					"width" : 288,
					"color" : "red",
					
					"children":
					(
						{ "name" : "TitleName", "type":"text", "x":0, "y":-1, "text":uiScriptLocale.SKILL_SELECT_TITLE, "all_align":"center" },
					),
				},	
						
				{
					"name" : "FirstSkillSlotBack",
					"type" : "grid_table",
					
					"x" : 15,
					"y" : 35,
					
					"start_index" : 0,
					
					"x_count" : 6,
					"y_count" : 1,
					
					"x_step" : 40,
					"y_step" : 40,
					
					"x_blank" : 6,
					"y_blank" : 1,
					
					
				},
				{
					"name" : "FirstSkillSlot",
					"type" : "grid_table",
					
					"x" : 15 + 4,
					"y" : 35 + 4,
					
					"start_index" : 0,
					
					"x_count" : 6,
					"y_count" : 1,
					
					"x_step" : 40,
					"y_step" : 40,
					
					"x_blank" : 6,
					"y_blank" : 1,
					
					"image" : "d:/ymir work/ui/public/Slot_Base.sub"
				},
				
				{
					"name" : "SecondSkillSlotBack",
					"type" : "grid_table",
					
					"x" : 15,
					"y" : 120,
					
					"start_index" : 0,
					
					"x_count" : 6,
					"y_count" : 1,
					
					"x_step" : 40,
					"y_step" : 40,
					
					"x_blank" : 6,
					"y_blank" : 1,
					
				},
				{
					"name" : "SecondSkillSlot",
					"type" : "grid_table",
					
					"x" : 15 + 4,
					"y" : 120 + 4,
					
					"start_index" : 0,
					
					"x_count" : 6,
					"y_count" : 1,
					
					"x_step" : 40,
					"y_step" : 40,
					
					"x_blank" : 6,
					"y_blank" : 1,
					
					"image" : "d:/ymir work/ui/public/Slot_Base.sub"
				},
				
				{
					"name" : "SelectButtonFirst",
					"type" : "button",
					
					"x" : 0,
					"y" : 80,
					
					"horizontal_align" : "center",
					
					"default_image" : "d:/ymir work/ui/public/XLarge_Button_01.sub",
					"over_image" : "d:/ymir work/ui/public/XLarge_Button_02.sub",
					"down_image" : "d:/ymir work/ui/public/XLarge_Button_03.sub",
				},
				
				{
					"name" : "SelectButtonSecond",
					"type" : "button",
					
					"x" : 0,
					"y" : 167,
					
					"horizontal_align" : "center",
					
					"default_image" : "d:/ymir work/ui/public/XLarge_Button_01.sub",
					"over_image" : "d:/ymir work/ui/public/XLarge_Button_02.sub",
					"down_image" : "d:/ymir work/ui/public/XLarge_Button_03.sub",
				},				
			),
		},
	),
}