#-*- coding: iso-8859-1 -*-

window = {
	"name" : "SystemsWindow",
	"x" : SCREEN_WIDTH - 190 - 148,
	"y" : SCREEN_HEIGHT - 55 - 565 + 209 + 32 - 37,
	"width" : 140,
	"height" : 500,
	"type" : "window",
	"children" :
	(
		{
			"name" : "ExpandBtn",
			"type" : "button",
			"x" : -6,
			"y" : 69,
			"default_image" : "locale/pl/ui/config/btn_expand_normal.tga",
			"over_image" : "locale/pl/ui/config/btn_expand_over.tga",
			"down_image" : "locale/pl/ui/config/btn_expand_down.tga",
			"disable_image" : "locale/pl/ui/config/btn_expand_disabled.tga",
		},
		{
			"name" : "SystemsWindowLayer",
			"x" : 5,
			"y" : 0,
			"width" : 140,
			"height" : 500,
			"children" :
			(
				#{
				# 	"name" : "BiologButton",
				# 	"type" : "button",
				#
				# 	"x" : 50,
				# 	"y" : 360,
				#	
				# 	"default_image" : "icon/button/biolog_button0.png",
				# 	"over_image" : "icon/button/biolog_button1.png",
				# 	"down_image" : "icon/button/biolog_button2.png",
				#
				#},
				#{
				# 	"name" : "ZlomiarzButton",
				# 	"type" : "button",
				#
				# 	"x" : 50,
				# 	"y" : 360+60,
				#	
				# 	"default_image" : "icon/button/zlomiarz_button0.png",
				# 	"over_image" : "icon/button/zlomiarz_button1.png",
				# 	"down_image" : "icon/button/zlomiarz_button2.png",
				#	
				#},
				{
					"name" : "MinimizeBtn",
					"type" : "button",
					"x" : 2,
					"y" : 69,
					"width" : 10,
					"default_image" : "locale/pl/ui/config/btn_minimize_normal.tga",
					"over_image" : "locale/pl/ui/config/btn_minimize_over.tga",
					"down_image" : "locale/pl/ui/config/btn_minimize_down.tga",
					"disable_image" : "locale/pl/ui/config/btn_minimize_disabled.tga",
				},
				{
					"name" : "SystemsWindowBoard",
					"type" : "board",
					"style" : ("attach", "float"),
					"x" : 10,
					"y" : 0,
					"width" : 140,
					"height" : 346,
					"children" :
					(
						{
                            "name" : "Mapa_Button",
							"type" : "button",
							"x" : 8,
							"y" : 8,
							"default_image" : "icon/button/tp1.png",
							"over_image" : "icon/button/tp2.png",
							"down_image" : "icon/button/tp3.png",

						},
						{
							"name" : "Localizacja_Button",
							"type" : "button",
							"x" : 9+32,
							"y" : 8,
							"default_image" : "icon/button/zapis1.png",
							"over_image" : "icon/button/zapis2.png",
							"down_image" : "icon/button/zapis3.png",

						},
						{
							"name" : "OfflineShopButton",
							"type" : "button",
							"x" : 8,
							"y" : 8+33,
							"default_image" : "icon/button/offline1.png",
							"over_image" : "icon/button/offline2.png",
							"down_image" : "icon/button/offline3.png",
							
						},
						{
							"name" : "aExp_Button",
							"type" : "button",
							"x" : 9+32,
							"y" : 8+33,
							"default_image" : "icon/button/exp1.png",
							"over_image" : "icon/button/exp2.png",
							"down_image" : "icon/button/exp3.png",

						},
						{
							"name" : "Dodatkowe_Button",
							"type" : "button",
							"x" : 8,
							"y" : 8+33+33,
							"default_image" : "icon/button/podreczny1.png",
							"over_image" : "icon/button/podreczny2.png",
							"down_image" : "icon/button/podreczny3.png",

						},
						{
							"name" : "Pet_Button",
							"type" : "button",
							"x" : 9+32,
							"y" : 8+33+33,
							"default_image" : "icon/button/pet1.png",
							"over_image" : "icon/button/pet2.png",
							"down_image" : "icon/button/pet3.png",

						},
						{
							"name" : "Ranking_Button",
							"type" : "button",
							"x" : 8,
							"y" : 8+33+33+33,
							"default_image" : "icon/button/title1.png",
							"over_image" : "icon/button/title2.png",
							"down_image" : "icon/button/title3.png",

						},
						{
							"name" : "Dopek_Button",
							"type" : "button",
							"x" : 9+32,
							"y" : 8+33+33+33,
							"default_image" : "icon/button/dopek1.png",
							"over_image" : "icon/button/dopek2.png",
							"down_image" : "icon/button/dopek3.png",

						},
						{
							"name" : "TabelaBon_Button",
							"type" : "button",
							"x" : 8,
							"y" : 8+33+33+33+33,
							"default_image" : "icon/button/tabelabon1.png",
							"over_image" : "icon/button/tabelabon2.png",
							"down_image" : "icon/button/tabelabon3.png",

						},
						{
							"name" : "Switcher_Button",
							"type" : "button",
							"x" : 9+32,
							"y" : 8+33+33+33+33,
							"default_image" : "icon/button/tabela1.png",
							"over_image" : "icon/button/tabela2.png",
							"down_image" : "icon/button/tabela3.png",

						},
						{
							"name" : "Shaman_Button",
						 	"type" : "button",
						 	"x" : 8,
						 	"y" : 8+33+33+33+33+33,
						 	"default_image" : "icon/button/shaman1.png",
						 	"over_image" : "icon/button/shaman2.png",
						 	"down_image" : "icon/button/shaman3.png",

						},
						{
						 	"name" : "ShopSearch_Button",
						 	"type" : "button",
						 	"x" : 9+32,
						 	"y" : 8+33+33+33+33+33,
						 	"default_image" : "icon/button/shopsearch1.png",
						 	"over_image" : "icon/button/shopsearch2.png",
						 	"down_image" : "icon/button/shopsearch3.png",

						},
						{
							"name" : "BossTp_Button",
							"type" : "button",
							"x" : 8,
                            "y" : 8+33+33+33+33+33+33,
							"default_image" : "icon/button/tpboss1.png",
							"over_image" : "icon/button/tpboss2.png",
							"down_image" : "icon/button/tpboss3.png",

						},	
						{
							"name" : "Artefakt_Button",
							"type" : "button",
							"x" : 9+32,
							"y" : 8+33+33+33+33+33+33,
							"default_image" : "icon/button/rune1.png",
							"over_image" : "icon/button/rune2.png",
							"down_image" : "icon/button/rune3.png",

						},				
                        {
							"name" : "Kosz_Button",
							"type" : "button",
							"x" : 9,
							"y" : 8+33+33+33+33+33+33+33,
							"default_image" : "icon/button/kosz1.png",
							"over_image" : "icon/button/kosz2.png",
							"down_image" : "icon/button/kosz3.png",

						},
						{
							"name" : "Rune_Button",
							"type" : "button",
							"x" : 9+32,
							"y" : 8+33+33+33+33+33+33+33,
							"default_image" : "icon/button/artefakt1.png",
							"over_image" : "icon/button/artefakt2.png",
							"down_image" : "icon/button/artefakt3.png",

						},		
                        {
							"name" : "Mount_Button",
							"type" : "button",
							"x" : 9,
							"y" : 8+33+33+33+33+33+33+33+33,
							"default_image" : "icon/button/mount1.png",
							"over_image" : "icon/button/mount2.png",
							"down_image" : "icon/button/mount3.png",

						},
						{
							"name" : "Dung_Button",
							"type" : "button",
							"x" : 9+32,
							"y" : 8+33+33+33+33+33+33+33+33,
							"default_image" : "icon/button/dung1.png",
							"over_image" : "icon/button/dung2.png",
							"down_image" : "icon/button/dung3.png",

						},	

						{
							"name" : "ChangeEq_Button",
							"type" : "button",
							"x" : 9,
							"y" : 8+33+33+33+33+33+33+33+33+33,
							"default_image" : "icon/button/zmianaeq1.png",
							"over_image" : "icon/button/zmianaeq2.png",
							"down_image" : "icon/button/zmianaeq3.png",
						},
						{
							"name" : "DepoIs_Button",
							"type" : "button",
							"x" : 9+32,
							"y" : 8+33+33+33+33+33+33+33+33+33,
							"default_image" : "icon/button/depo1.png",
							"over_image" : "icon/button/depo2.png",
							"down_image" : "icon/button/depo3.png",
						},	
					),
				},
			)
		},
	),
}
