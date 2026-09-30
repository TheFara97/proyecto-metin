#-*- coding: iso-8859-1 -*-
import dbg
import player
import item
import grp
import wndMgr
import skill
import shop
import exchange
import grpText
import safebox
import localeInfo
import app
import background
import switchbot
import nonplayer
import chr
import guild
import ui
import mouseModule
import chat
import constInfo
import uiScriptLocale
if app.ENABLE_AURA_SYSTEM:
	import aura
import renderTarget
import new_uiWeeklyRank
if app.ENABLE_ACCE_COSTUME_SYSTEM:
	import acce
import emoji
import net

if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
	import privateShop

item_ids = [50187, 50188, 90051, 90041, 90071]

POLY_DMG_VAL = {
	0 : [ 1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10 ],
	1 : [ 11,11,11,12,12,13,13,14,14,15,15 ],
	2 : [ 16,16,16,17,18,19,20,21,22,23,24 ],
	3 : [ 25,25 ]
}

pasywkiValues = {
	180 : 9,
	181 : 9,
	182 : 4.5,
	183 : 4.5,
	184 : 4.5,
	185 : 6,
	186 : 3,
	187 : 6,
	188 : 4,
	189 : 900,
	190 : 3,
}

if app.ENABLE_EXTRABONUS_SYSTEM:
	GLOVE_BONUS_TABLE = {
		118 : 20,
		72 : 30,
		113 : 30,
		17 : 50,
		71 : 25,
		23 : 15,
		1 : 2000,
		15 : 20,
		7 : 8,
		8 : 8,
		14 : 8,
		12 : 5,
		105 : 20,
	}


zestaw_data = {
	50183: {
		"male": {
			"hairVnum": 45979, 
			"bodyVnum": 41998, 
			"acceVnum": 85538,
			"weapons": {
				"sword": 40577,
				"twohand": 40580,
				"dagger": 40578,
				"bow": 40579,
				"sura_sword": 40583,
				"bell": 40581,
				"fan": 40582
			}
		}, 
		"female": {
			"hairVnum": 45979, 
			"bodyVnum": 41999, 
			"acceVnum": 85538,
			"weapons": {
				"sword": 40577,
				"twohand": 40580,
				"dagger": 40578,
				"bow": 40579,
				"sura_sword": 40583,
				"bell": 40581,
				"fan": 40582
			}
		}
	},
	50184: {
		"male": {
			"hairVnum": 45980, 
			"bodyVnum": 41700, 
			"acceVnum": 85539,
			"weapons": {
				"sword": 40584,
				"twohand": 40587,
				"dagger": 40585,
				"bow": 40586,
				"sura_sword": 40590,
				"bell": 40588,
				"fan": 40589
			}
		}, 
		"female": {
			"hairVnum": 45980, 
			"bodyVnum": 41701, 
			"acceVnum": 85539,
			"weapons": {
				"sword": 40584,
				"twohand": 40587,
				"dagger": 40585,
				"bow": 40586,
				"sura_sword": 40590, 
				"bell": 40588,
				"fan": 40589
			}
		}
	},
	50185: {
		"male": {
			"hairVnum": 45964, 
			"bodyVnum": 41964, 
			"acceVnum": 85521,
			"weapons": {
				"sword": 40717,
				"twohand": 40720,
				"dagger": 40718,
				"bow": 40719,
				"sura_sword": 40723,
				"bell": 40721,
				"fan": 40722
			}
		}, 
		"female": {
			"hairVnum": 45964, 
			"bodyVnum": 41965, 
			"acceVnum": 85521,
			"weapons": {
				"sword": 40717,
				"twohand": 40720,
				"dagger": 40718,
				"bow": 40719,
				"sura_sword": 40723,
				"bell": 40721,
				"fan": 40722
			}
		}
	},
	50186: {
		"male": {
			"hairVnum": 61, 
			"bodyVnum": 41966, 
			"acceVnum": 85522,
			"weapons": {
				"sword": 40724,
				"twohand": 40727,
				"dagger": 40725,
				"bow": 40726,
				"sura_sword": 40730,
				"bell": 40728,
				"fan": 40729
			}
		}, 
		"female": {
			"hairVnum": 61, 
			"bodyVnum": 41967, 
			"acceVnum": 85522,
			"weapons": {
				"sword": 40724,
				"twohand": 40727,
				"dagger": 40725,
				"bow": 40726,
				"sura_sword": 40730,
				"bell": 40728,
				"fan": 40729
			}
		}
	},
	50187: {
		"male": {
			"hairVnum": 45926, 
			"bodyVnum": 41926, 
			"acceVnum": 85503,
			"weapons": {
				"sword": 40940,
				"twohand": 40943,
				"dagger": 40941,
				"bow": 40942,
				"sura_sword": 40940,
				"bell": 40944,
				"fan": 40945
			}
		}, 
		"female": {
			"hairVnum": 45926, 
			"bodyVnum": 41926, 
			"acceVnum": 85503,
			"weapons": {
				"sword": 40940,
				"twohand": 40943,
				"dagger": 40941,
				"bow": 40942,
				"sura_sword": 40940,
				"bell": 40944,
				"fan": 40945
			}
		}
	},
	50188: {
		"male": {
			"hairVnum": 45932, 
			"bodyVnum": 41932, 
			"acceVnum": 85506,
			"weapons": {
				"sword": 40970,
				"twohand": 40973,
				"dagger": 40971,
				"bow": 40972,
				"sura_sword": 40970,
				"bell": 40974,
				"fan": 40975
			}
		}, 
		"female": {
			"hairVnum": 45932, 
			"bodyVnum": 41933, 
			"acceVnum": 85506,
			"weapons": {
				"sword": 40970,
				"twohand": 40973,
				"dagger": 40971,
				"bow": 40972,
				"sura_sword": 40970,
				"bell": 40974,
				"fan": 40975
			}
		}
	},
	50189: {
		"male": {
			"hairVnum": 45934, 
			"bodyVnum": 41934, 
			"acceVnum": 85507,
			"weapons": {
				"sword": 40980,
				"twohand": 40983,
				"dagger": 40981,
				"bow": 40982,
				"sura_sword": 40986,
				"bell": 40984,
				"fan": 40985
			}
		}, 
		"female": {
			"hairVnum": 45934, 
			"bodyVnum": 41935, 
			"acceVnum": 85507,
			"weapons": {
				"sword": 40980,
				"twohand": 40983,
				"dagger": 40981,
				"bow": 40982,
				"sura_sword": 40986,
				"bell": 40984,
				"fan": 40985
			}
		}
	},
	50190: {
		"male": {
			"hairVnum": 45936, 
			"bodyVnum": 41936, 
			"acceVnum": 85508,
			"weapons": {
				"sword": 40990,
				"twohand": 40993,
				"dagger": 40991,
				"bow": 40992,
				"sura_sword": 40990,
				"bell": 40994,
				"fan": 40995
			}
		}, 
		"female": {
			"hairVnum": 45937, 
			"bodyVnum": 41937, 
			"acceVnum": 85508,
			"weapons": {
				"sword": 40990,
				"twohand": 40993,
				"dagger": 40991,
				"bow": 40992,
				"sura_sword": 40990,
				"bell": 40994,
				"fan": 40995
			}
		}
	},
	50191: {
		"male": {
			"hairVnum": 45938, 
			"bodyVnum": 41938, 
			"acceVnum": 85509,
			"weapons": {
				"sword": 40800,
				"twohand": 40803,
				"dagger": 40801,
				"bow": 40802,
				"sura_sword": 40800,
				"bell": 40804,
				"fan": 40805
			}
		}, 
		"female": {
			"hairVnum": 45939, 
			"bodyVnum": 41937, 
			"acceVnum": 85508,
			"weapons": {
				"sword": 40800,
				"twohand": 40803,
				"dagger": 40801,
				"bow": 40802,
				"sura_sword": 40800,
				"bell": 40804,
				"fan": 40805
			}
		}
	},
	50192: {
		"male": {
			"hairVnum": 45940, 
			"bodyVnum": 41940, 
			"acceVnum": 85510,
			"weapons": {
				"sword": 40810,
				"twohand": 40813,
				"dagger": 40811,
				"bow": 40812,
				"sura_sword": 40810,
				"bell": 40814,
				"fan": 40815
			}
		}, 
		"female": {
			"hairVnum": 45941, 
			"bodyVnum": 41941, 
			"acceVnum": 85510,
			"weapons": {
				"sword": 40810,
				"twohand": 40813,
				"dagger": 40811,
				"bow": 40812,
				"sura_sword": 40810,
				"bell": 40814,
				"fan": 40815
			}
		}
	},
	50193: {
		"male": {
			"hairVnum": 45942, 
			"bodyVnum": 41942, 
			"acceVnum": 85511,
			"weapons": {
				"sword": 40820,
				"twohand": 40823,
				"dagger": 40821,
				"bow": 40822,
				"sura_sword": 40820,
				"bell": 40824,
				"fan": 40825
			}
		}, 
		"female": {
			"hairVnum": 45943, 
			"bodyVnum": 41943, 
			"acceVnum": 85511,
			"weapons": {
				"sword": 40820,
				"twohand": 40823,
				"dagger": 40821,
				"bow": 40822,
				"sura_sword": 40820,
				"bell": 40824,
				"fan": 40825
			}
		}
	},
	50194: {
		"male": {
			"hairVnum": 45944, 
			"bodyVnum": 41944, 
			"acceVnum": 85512,
			"weapons": {
				"sword": 40830,
				"twohand": 40833,
				"dagger": 40831,
				"bow": 40832,
				"sura_sword": 40830,
				"bell": 40834,
				"fan": 40835
			}
		}, 
		"female": {
			"hairVnum": 45945, 
			"bodyVnum": 41945, 
			"acceVnum": 85512,
			"weapons": {
				"sword": 40830,
				"twohand": 40833,
				"dagger": 40831,
				"bow": 40832,
				"sura_sword": 40830,
				"bell": 40834,
				"fan": 40835
			}
		}
	},
	50195: {
		"male": {
			"hairVnum": 45946, 
			"bodyVnum": 41946, 
			"acceVnum": 85513,
			"weapons": {
				"sword": 40840,
				"twohand": 40843,
				"dagger": 40841,
				"bow": 40842,
				"sura_sword": 40840,
				"bell": 40844,
				"fan": 40845
			}
		}, 
		"female": {
			"hairVnum": 45947, 
			"bodyVnum": 41947, 
			"acceVnum": 85513,
			"weapons": {
				"sword": 40840,
				"twohand": 40843,
				"dagger": 40841,
				"bow": 40842,
				"sura_sword": 40840,
				"bell": 40844,
				"fan": 40845
			}
		}
	},
	50196: {
		"male": {
			"hairVnum": 45948, 
			"bodyVnum": 41948, 
			"acceVnum": 85514,
			"weapons": {
				"sword": 40850,
				"twohand": 40853,
				"dagger": 40851,
				"bow": 40852,
				"sura_sword": 40850,
				"bell": 40854,
				"fan": 40855
			}
		}, 
		"female": {
			"hairVnum": 45949, 
			"bodyVnum": 41949, 
			"acceVnum": 85514,
			"weapons": {
				"sword": 40850,
				"twohand": 40853,
				"dagger": 40851,
				"bow": 40852,
				"sura_sword": 40850,
				"bell": 40854,
				"fan": 40855
			}
		}
	},
	50197: {
		"male": {
			"hairVnum": 45950, 
			"bodyVnum": 41950, 
			"acceVnum": 85515,
			"weapons": {
				"sword": 40860,
				"twohand": 40863,
				"dagger": 40861,
				"bow": 40862,
				"sura_sword": 40860,
				"bell": 40864,
				"fan": 40865
			}
		}, 
		"female": {
			"hairVnum": 45951, 
			"bodyVnum": 41951, 
			"acceVnum": 85515,
			"weapons": {
				"sword": 40860,
				"twohand": 40863,
				"dagger": 40861,
				"bow": 40862,
				"sura_sword": 40860,
				"bell": 40864,
				"fan": 40865
			}
		}
	},
	50198: {
		"male": {
			"hairVnum": 45952, 
			"bodyVnum": 41952, 
			"acceVnum": 85516,
			"weapons": {
				"sword": 40870,
				"twohand": 40873,
				"dagger": 40871,
				"bow": 40872,
				"sura_sword": 40870,
				"bell": 40874,
				"fan": 40875
			}
		}, 
		"female": {
			"hairVnum": 45953, 
			"bodyVnum": 41953, 
			"acceVnum": 85516,
			"weapons": {
				"sword": 40870,
				"twohand": 40873,
				"dagger": 40871,
				"bow": 40872,
				"sura_sword": 40870,
				"bell": 40874,
				"fan": 40875
			}
		}
	},
	50199: {
		"male": {
			"hairVnum": 45900, 
			"bodyVnum": 41900, 
			"acceVnum": 85500,
			"weapons": {
				"sword": 40900,
				"twohand": 40903,
				"dagger": 40901,
				"bow": 40902,
				"sura_sword": 40900,
				"bell": 40904,
				"fan": 40905
			}
		}, 
		"female": {
			"hairVnum": 45900, 
			"bodyVnum": 41901, 
			"acceVnum": 85500,
			"weapons": {
				"sword": 40900,
				"twohand": 40903,
				"dagger": 40901,
				"bow": 40902,
				"sura_sword": 40900,
				"bell": 40904,
				"fan": 40905
			}
		}
	},
	90042: {
		"male": {
			"hairVnum": 45973, 
			"bodyVnum": 41976, 
			"acceVnum": 85527,
			"weapons": {
				"sword": 40500,
				"twohand": 40503,
				"dagger": 40501,
				"bow": 40502,
				"sura_sword": 40506,
				"bell": 40504,
				"fan": 40505
			}
		}, 
		"female": {
			"hairVnum": 45973, 
			"bodyVnum": 41977, 
			"acceVnum": 85527,
			"weapons": {
				"sword": 40500,
				"twohand": 40503,
				"dagger": 40501,
				"bow": 40502,
				"sura_sword": 40506,
				"bell": 40504,
				"fan": 40505
			}
		}
	},
	90043: {
		"male": {
			"hairVnum": 45969, 
			"bodyVnum": 41980, 
			"acceVnum": 85529,
			"weapons": {
				"sword": 40514,
				"twohand": 40517,
				"dagger": 40515,
				"bow": 40516,
				"sura_sword": 40520,
				"bell": 40518,
				"fan": 40519
			}
		}, 
		"female": {
			"hairVnum": 45969, 
			"bodyVnum": 41981, 
			"acceVnum": 85529,
			"weapons": {
				"sword": 40514,
				"twohand": 40517,
				"dagger": 40515,
				"bow": 40516,
				"sura_sword": 40520,
				"bell": 40518,
				"fan": 40519
			}
		}
	},
	90044: {
		"male": {
			"hairVnum": 45970, 
			"bodyVnum": 41982, 
			"acceVnum": 85530,
			"weapons": {
				"sword": 40521,
				"twohand": 40524,
				"dagger": 40522,
				"bow": 40523,
				"sura_sword": 40527,
				"bell": 40525,
				"fan": 40526
			}
		}, 
		"female": {
			"hairVnum": 45970, 
			"bodyVnum": 41983, 
			"acceVnum": 85530,
			"weapons": {
				"sword": 40521,
				"twohand": 40524,
				"dagger": 40522,
				"bow": 40523,
				"sura_sword": 40527,
				"bell": 40525,
				"fan": 40526
			}
		}
	},
	90045: {
		"male": {
			"hairVnum": 45980, 
			"bodyVnum": 41984, 
			"acceVnum": 85531,
			"weapons": {
				"sword": 40528,
				"twohand": 40531,
				"dagger": 40529,
				"bow": 40530,
				"sura_sword": 40534,
				"bell": 40532,
				"fan": 40533
			}
		}, 
		"female": {
			"hairVnum": 45980, 
			"bodyVnum": 41985, 
			"acceVnum": 85531,
			"weapons": {
				"sword": 40528,
				"twohand": 40531,
				"dagger": 40529,
				"bow": 40530,
				"sura_sword": 40534,
				"bell": 40532,
				"fan": 40533
			}
		}
	},
	90046: {
		"male": {
			"hairVnum": 45972, 
			"bodyVnum": 41986, 
			"acceVnum": 85532,
			"weapons": {
				"sword": 40535,
				"twohand": 40538,
				"dagger": 40536,
				"bow": 40537,
				"sura_sword": 40541,
				"bell": 40539,
				"fan": 40540
			}
		}, 
		"female": {
			"hairVnum": 45972, 
			"bodyVnum": 41987, 
			"acceVnum": 85532,
			"weapons": {
				"sword": 40535,
				"twohand": 40538,
				"dagger": 40536,
				"bow": 40537,
				"sura_sword": 40541,
				"bell": 40539,
				"fan": 40540
			}
		}
	},
	90052: {
		"male": {
			"hairVnum": 45928, 
			"bodyVnum": 41928, 
			"acceVnum": 85504,
			"weapons": {
				"sword": 40950,
				"twohand": 40953,
				"dagger": 40951,
				"bow": 40952,
				"sura_sword": 40950,
				"bell": 40954,
				"fan": 40955
			}
		}, 
		"female": {
			"hairVnum": 45929, 
			"bodyVnum": 41929, 
			"acceVnum": 85504,
			"weapons": {
				"sword": 40950,
				"twohand": 40953,
				"dagger": 40951,
				"bow": 40952,
				"sura_sword": 40950,
				"bell": 40954,
				"fan": 40955
			}
		}
	},
	90053: {
		"male": {
			"hairVnum": 45930, 
			"bodyVnum": 41930, 
			"acceVnum": 85505,
			"weapons": {
				"sword": 40960,
				"twohand": 40963,
				"dagger": 40961,
				"bow": 40962,
				"sura_sword": 40960,
				"bell": 40964,
				"fan": 40965
			}
		}, 
		"female": {
			"hairVnum": 45931, 
			"bodyVnum": 41931, 
			"acceVnum": 85505,
			"weapons": {
				"sword": 40960,
				"twohand": 40963,
				"dagger": 40961,
				"bow": 40962,
				"sura_sword": 40960,
				"bell": 40964,
				"fan": 40965
			}
		}
	},
	90054: {
		"male": {
			"hairVnum": 45968, 
			"bodyVnum": 41978, 
			"acceVnum": 85528,
			"weapons": {
				"sword": 40507,
				"twohand": 40510,
				"dagger": 40508,
				"bow": 40509,
				"sura_sword": 40513,
				"bell": 40511,
				"fan": 40512
			}
		}, 
		"female": {
			"hairVnum": 45968, 
			"bodyVnum": 41979, 
			"acceVnum": 85528,
			"weapons": {
				"sword": 40507,
				"twohand": 40510,
				"dagger": 40508,
				"bow": 40509,
				"sura_sword": 40513,
				"bell": 40511,
				"fan": 40512
			}
		}
	},
	90055: {
		"male": {
			"hairVnum": 45958, 
			"bodyVnum": 41958, 
			"acceVnum": 85519,
			"weapons": {
				"sword": 40700,
				"twohand": 40703,
				"dagger": 40701,
				"bow": 40702,
				"sura_sword": 40700,
				"bell": 40704,
				"fan": 40705
			}
		}, 
		"female": {
			"hairVnum": 45958, 
			"bodyVnum": 41959, 
			"acceVnum": 85519,
			"weapons": {
				"sword": 40700,
				"twohand": 40703,
				"dagger": 40701,
				"bow": 40702,
				"sura_sword": 40700,
				"bell": 40704,
				"fan": 40705
			}
		}
	},
	90056: {
		"male": {
			"hairVnum": 45960, 
			"bodyVnum": 41960, 
			"acceVnum": 85520,
			"weapons": {
				"sword": 40710,
				"twohand": 40713,
				"dagger": 40711,
				"bow": 40712,
				"sura_sword": 40710,
				"bell": 40714,
				"fan": 40715
			}
		}, 
		"female": {
			"hairVnum": 45960, 
			"bodyVnum": 41961, 
			"acceVnum": 85520,
			"weapons": {
				"sword": 40710,
				"twohand": 40713,
				"dagger": 40711,
				"bow": 40712,
				"sura_sword": 40710,
				"bell": 40714,
				"fan": 40715
			}
		}
	},
	90070: {
		"male": {
			"hairVnum": 45974, 
			"bodyVnum": 41988, 
			"acceVnum": 85533,
			"weapons": {
				"sword": 40542,
				"twohand": 40545,
				"dagger": 40543,
				"bow": 40544,
				"sura_sword": 40548,
				"bell": 40546,
				"fan": 40547
			}
		}, 
		"female": {
			"hairVnum": 45974, 
			"bodyVnum": 41989, 
			"acceVnum": 85533,
			"weapons": {
				"sword": 40542,
				"twohand": 40545,
				"dagger": 40543,
				"bow": 40544,
				"sura_sword": 40548,
				"bell": 40546,
				"fan": 40547
			}
		}
	},
	90072: {
		"male": {
			"hairVnum": 45977, 
			"bodyVnum": 41994, 
			"acceVnum": 85536,
			"weapons": {
				"sword": 40563,
				"twohand": 40566,
				"dagger": 40564,
				"bow": 40565,
				"sura_sword": 40569,
				"bell": 40567,
				"fan": 40568
			}
		}, 
		"female": {
			"hairVnum": 45977, 
			"bodyVnum": 41995, 
			"acceVnum": 85536,
			"weapons": {
				"sword": 40563,
				"twohand": 40566,
				"dagger": 40564,
				"bow": 40565,
				"sura_sword": 40569,
				"bell": 40567,
				"fan": 40568
			}
		}
	},
	50210: {
		"male": {
			"hairVnum": 45981, 
			"bodyVnum": 41702, 
			"acceVnum": 85540,
			"weapons": {
				"sword": 40591,
				"twohand": 40594,
				"dagger": 40592,
				"bow": 40593,
				"sura_sword": 40597,
				"bell": 40595,
				"fan": 40596
			}
		}, 
		"female": {
			"hairVnum": 45981, 
			"bodyVnum": 41703, 
			"acceVnum": 85540,
			"weapons": {
				"sword": 40591,
				"twohand": 40594,
				"dagger": 40592,
				"bow": 40593,
				"sura_sword": 40597,
				"bell": 40595,
				"fan": 40596
			}
		}
	},
	50211: {
		"male": {
			"hairVnum": 45982, 
			"bodyVnum": 41704, 
			"acceVnum": 85541,
			"weapons": {
				"sword": 40598,
				"twohand": 40601,
				"dagger": 40599,
				"bow": 40600,
				"sura_sword": 40604,
				"bell": 40602,
				"fan": 40603
			}
		}, 
		"female": {
			"hairVnum": 45982, 
			"bodyVnum": 41705, 
			"acceVnum": 85541,
			"weapons": {
				"sword": 40598,
				"twohand": 40601,
				"dagger": 40599,
				"bow": 40600,
				"sura_sword": 40604,
				"bell": 40602,
				"fan": 40603
			}
		}
	},
	50182: {
		"male": {
			"hairVnum": 45978, 
			"bodyVnum": 41996, 
			"acceVnum": 85537,
			"weapons": {
				"sword": 40570,
				"twohand": 40573,
				"dagger": 40571,
				"bow": 40572,
				"sura_sword": 40576,
				"bell": 40574,
				"fan": 40575
			}
		}, 
		"female": {
			"hairVnum": 45978, 
			"bodyVnum": 41997, 
			"acceVnum": 85537,
			"weapons": {
				"sword": 40570,
				"twohand": 40573,
				"dagger": 40571,
				"bow": 40572,
				"sura_sword": 40576,
				"bell": 40574,
				"fan": 40575
			}
		}
	},
	50160: {
		"male": {
			"hairVnum": 45962, 
			"bodyVnum": 41706, 
			"acceVnum": 85542,
			"weapons": {
				"sword": 40612,
				"twohand": 40615,
				"dagger": 40613,
				"bow": 40614,
				"sura_sword": 40618,
				"bell": 40616,
				"fan": 40617
			}
		}, 
		"female": {
			"hairVnum": 45962, 
			"bodyVnum": 41707, 
			"acceVnum": 85542,
			"weapons": {
				"sword": 40612,
				"twohand": 40615,
				"dagger": 40613,
				"bow": 40614,
				"sura_sword": 40618,
				"bell": 40616,
				"fan": 40617
			}
		}
	},
	50161: {
		"male": {
			"hairVnum": 45983, 
			"bodyVnum": 41710, 
			"acceVnum": 85544,
			"weapons": {
				"sword": 40626,
				"twohand": 40629,
				"dagger": 40627,
				"bow": 40628,
				"sura_sword": 40626,
				"bell": 40630,
				"fan": 40631
			}
		}, 
		"female": {
			"hairVnum": 45983, 
			"bodyVnum": 41711, 
			"acceVnum": 85544,
			"weapons": {
				"sword": 40626,
				"twohand": 40629,
				"dagger": 40627,
				"bow": 40628,
				"sura_sword": 40626,
				"bell": 40630,
				"fan": 40631
			}
		}
	},
	50162: {
		"male": {
			"hairVnum": 45984, 
			"bodyVnum": 41714, 
			"acceVnum": 85546,
			"weapons": {
				"sword": 40640,
				"twohand": 40643,
				"dagger": 40641,
				"bow": 40642,
				"sura_sword": 40646,
				"bell": 40644,
				"fan": 40645
			}
		}, 
		"female": {
			"hairVnum": 45984, 
			"bodyVnum": 41715, 
			"acceVnum": 85546,
			"weapons": {
				"sword": 40640,
				"twohand": 40643,
				"dagger": 40641,
				"bow": 40642,
				"sura_sword": 40646,
				"bell": 40644,
				"fan": 40645
			}
		}
	},
	50163: {
		"male": {
			"hairVnum": 45985, 
			"bodyVnum": 41718, 
			"acceVnum": 85548,
			"weapons": {
				"sword": 40654,
				"twohand": 40657,
				"dagger": 40655,
				"bow": 40656,
				"sura_sword": 40660,
				"bell": 40658,
				"fan": 40659
			}
		}, 
		"female": {
			"hairVnum": 45985, 
			"bodyVnum": 41718, 
			"acceVnum": 85548,
			"weapons": {
				"sword": 40654,
				"twohand": 40657,
				"dagger": 40655,
				"bow": 40656,
				"sura_sword": 40660,
				"bell": 40658,
				"fan": 40659
			}
		}
	},
	50164: {
		"male": {
			"hairVnum": 45986, 
			"bodyVnum": 41722, 
			"acceVnum": 85550,
			"weapons": {
				"sword": 40668,
				"twohand": 40671,
				"dagger": 40669,
				"bow":  40670,
				"sura_sword": 40674,
				"bell": 40672,
				"fan": 40673
			}
		}, 
		"female": {
			"hairVnum": 45986, 
			"bodyVnum": 41723, 
			"acceVnum": 85550,
			"weapons": {
				"sword": 40668,
				"twohand": 40671,
				"dagger": 40669,
				"bow":  40670,
				"sura_sword": 40674,
				"bell": 40672,
				"fan": 40673
			}
		}
	},
	50168: {
		"male": {
			"hairVnum": 46052, 
			"bodyVnum": 41756, 
			"acceVnum": 85568,
			"weapons": {
				"sword": 40398,
				"twohand": 40401,
				"dagger": 40399,
				"bow":  40400,
				"sura_sword": 40404,
				"bell": 40402,
				"fan": 40403
			}
		}, 
		"female": {
			"hairVnum": 46053, 
			"bodyVnum": 41757, 
			"acceVnum": 85568,
			"weapons": {
				"sword": 40398,
				"twohand": 40401,
				"dagger": 40399,
				"bow":  40400,
				"sura_sword": 40404,
				"bell": 40402,
				"fan": 40403
			}
		}
	},
	50181: {
		"male": {
			"hairVnum": 45987, 
			"bodyVnum": 41726, 
			"acceVnum": 85553,
			"weapons": {
				"sword": 40682,
				"twohand": 40685,
				"dagger": 40683,
				"bow": 40684,
				"sura_sword": 40688,
				"bell": 40686,
				"fan": 40687
			}
		}, 
		"female": {
			"hairVnum": 45987, 
			"bodyVnum": 41727, 
			"acceVnum": 85553,
			"weapons": {
				"sword": 40682,
				"twohand": 40685,
				"dagger": 40683,
				"bow": 40684,
				"sura_sword": 40688,
				"bell": 40686,
				"fan": 40687
			}
		}
	},
	50165: {
		"male": {
			"hairVnum": 45993, 
			"bodyVnum": 41748, 
			"acceVnum": 85564,
			"weapons": {
				"sword": 40370,
				"twohand": 40373,
				"dagger": 40371,
				"bow": 40372,
				"sura_sword": 40376,
				"bell": 40374,
				"fan": 40375
			}
		}, 
		"female": {
			"hairVnum": 45993, 
			"bodyVnum": 41749, 
			"acceVnum": 85564,
			"weapons": {
				"sword": 40370,
				"twohand": 40373,
				"dagger": 40371,
				"bow": 40372,
				"sura_sword": 40376,
				"bell": 40374,
				"fan": 40375
			}
		}
	},
	50166: {
		"male": {
			"hairVnum": 45993, 
			"bodyVnum": 41732, 
			"acceVnum": 85556,
			"weapons": {
				"sword": 40314,
				"twohand": 40317,
				"dagger": 40315,
				"bow": 40316,
				"sura_sword": 40320,
				"bell": 40318,
				"fan": 40319
			}
		}, 
		"female": {
			"hairVnum": 45993, 
			"bodyVnum": 41732, 
			"acceVnum": 85556,
			"weapons": {
				"sword": 40314,
				"twohand": 40317,
				"dagger": 40315,
				"bow": 40316,
				"sura_sword": 40320,
				"bell": 40318,
				"fan": 40319
			}
		}
	},
	91206: {
		"male": {
			"hairVnum": 45994, 
			"bodyVnum": 41752, 
			"acceVnum": 85566,
			"weapons": {
				"sword": 40384,
				"twohand": 40387,
				"dagger": 40385,
				"bow": 40386,
				"sura_sword": 40390,
				"bell": 40388,
				"fan": 40389
			}
		}, 
		"female": {
			"hairVnum": 45994, 
			"bodyVnum": 41753, 
			"acceVnum": 85566,
			"weapons": {
				"sword": 40384,
				"twohand": 40387,
				"dagger": 40385,
				"bow": 40386,
				"sura_sword": 40390,
				"bell": 40388,
				"fan": 40389
			}
		}
	},
	91225: {
		"male": {
			"hairVnum": 45997, 
			"bodyVnum": 41766, 
			"acceVnum": 85573,
			"weapons": {
				"sword": 40433,
				"twohand": 40436,
				"dagger": 40434,
				"bow": 40435,
				"sura_sword": 40439,
				"bell": 40437,
				"fan": 40438
			}
		}, 
		"female": {
			"hairVnum": 45997, 
			"bodyVnum": 41767, 
			"acceVnum": 85573,
			"weapons": {
				"sword": 40433,
				"twohand": 40436,
				"dagger": 40434,
				"bow": 40435,
				"sura_sword": 40439,
				"bell": 40437,
				"fan": 40438
			}
		}
	},
	91226: {
		"male": {
			"hairVnum": 45998, 
			"bodyVnum": 41768, 
			"acceVnum": 85574,
			"weapons": {
				"sword": 40440,
				"twohand": 40443,
				"dagger": 40441,
				"bow": 40442,
				"sura_sword": 40446,
				"bell": 40444,
				"fan": 40445
			}
		}, 
		"female": {
			"hairVnum": 45998, 
			"bodyVnum": 41769, 
			"acceVnum": 85574,
			"weapons": {
				"sword": 40440,
				"twohand": 40443,
				"dagger": 40441,
				"bow": 40442,
				"sura_sword": 40446,
				"bell": 40444,
				"fan": 40445
			}
		}
	},
	91227: {
		"male": {
			"hairVnum": 45998, 
			"bodyVnum": 41770, 
			"acceVnum": 85575,
			"weapons": {
				"sword": 40447,
				"twohand": 40450,
				"dagger": 40448,
				"bow": 40449,
				"sura_sword": 40453,
				"bell": 40451,
				"fan": 40452
			}
		}, 
		"female": {
			"hairVnum": 45998, 
			"bodyVnum": 41771, 
			"acceVnum": 85575,
			"weapons": {
				"sword": 40447,
				"twohand": 40450,
				"dagger": 40448,
				"bow": 40449,
				"sura_sword": 40453,
				"bell": 40451,
				"fan": 40452
			}
		}
	},
	91230: {
		"male": {
			"hairVnum": 45999, 
			"bodyVnum": 41772, 
			"acceVnum": 85576,
			"weapons": {
				"sword": 40454,
				"twohand": 40457,
				"dagger": 40455,
				"bow": 40456,
				"sura_sword": 40460,
				"bell": 40458,
				"fan": 40459
			}
		}, 
		"female": {
			"hairVnum": 45999, 
			"bodyVnum": 41773, 
			"acceVnum": 85576,
			"weapons": {
				"sword": 40454,
				"twohand": 40457,
				"dagger": 40455,
				"bow": 40456,
				"sura_sword": 40460,
				"bell": 40458,
				"fan": 40459
			}
		}
	},
	91244: {
		"male": {
			"hairVnum": 46000,
			"bodyVnum": 41774,
			"acceVnum": 85577,
			"weapons": {
				"sword": 40461,
				"twohand": 40464,
				"dagger": 40462,
				"bow": 40463,
				"sura_sword": 40467,
				"bell": 40465,
				"fan": 40466
			}
		}, 
		"female": {
			"hairVnum": 46000, 
			"bodyVnum": 41775, 
			"acceVnum": 85577,
			"weapons": {
				"sword": 40461,
				"twohand": 40464,
				"dagger": 40462,
				"bow": 40463,
				"sura_sword": 40467,
				"bell": 40465,
				"fan": 40466
			}
		}
	},
}

WARP_SCROLLS = [22011, 22000, 22010]

DESC_DEFAULT_MAX_COLS = 26
DESC_WESTERN_MAX_COLS = 35
DESC_WESTERN_MAX_WIDTH = 220

def chop(n):
	return round(n - 0.5, 1)

def SplitDescription(desc, limit):
	total_tokens = desc.split()
	line_tokens = []
	line_len = 0
	lines = []

	for token in total_tokens:
		if "|" in token:
			sep_pos = token.find("|")
			line_tokens.append(token[:sep_pos])
			lines.append(" ".join(line_tokens))
			line_len = len(token) - (sep_pos + 1)
			line_tokens = [token[sep_pos+1:]]
		elif app.WJ_MULTI_TEXTLINE and "\\n" in token:
			sep_pos = token.find("\\n")
			line_tokens.append(token[:sep_pos])

			lines.append(" ".join(line_tokens))
			line_len = len(token) - (sep_pos + 2)
			line_tokens = [token[sep_pos+2:]]
		else:
			line_len += len(token)
			if len(line_tokens) + line_len > limit:
				lines.append(" ".join(line_tokens))
				line_len = len(token)
				line_tokens = [token]
			else:
				line_tokens.append(token)

	if line_tokens:
		lines.append(" ".join(line_tokens))

	return lines

class ToolTip(ui.ThinBoard):

	swordEmoji = "  |Eemoji/sword|e  "
	klepsydraEmoji = "  |Eemoji/klepsydra|e  "

	TOOL_TIP_WIDTH = 220
	
	TOOL_TIP_HEIGHT = 10

	TEXT_LINE_HEIGHT = 17

	TITLE_COLOR = grp.GenerateColor(0.9490, 0.9058, 0.7568, 1.0)
	SPECIAL_TITLE_COLOR = grp.GenerateColor(1.0, 0.7843, 0.0, 1.0)
	SPECIAL_TITLE_COLOR2 = grp.GenerateColor(1.0, 0.8745, 0.4196, 1.0)

	NORMAL_COLOR = grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0)
	FONT_COLOR = grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0)
	PRICE_COLOR = 0xffFFB96D

	HIGH_PRICE_COLOR = SPECIAL_TITLE_COLOR
	MIDDLE_PRICE_COLOR = SPECIAL_TITLE_COLOR
	LOW_PRICE_COLOR = SPECIAL_TITLE_COLOR

	ENABLE_COLOR = grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0)
	DISABLE_COLOR = grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0)

	NEGATIVE_COLOR = grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0)
	POSITIVE_COLOR = grp.GenerateColor(0.5411, 0.7254, 0.5568, 1.0)
	SPECIAL_POSITIVE_COLOR2 = grp.GenerateColor((50.00 / 255), (250.00 / 255), (207.00 / 255), 1.0)
	SPECIAL_POSITIVE_COLOR = grp.GenerateColor(0.6911, 0.8754, 0.7068, 1.0)
	#SPECIAL_POSITIVE_COLOR2 = grp.GenerateColor(0.8824, 0.9804, 0.8824, 1.0)
	#SPECIAL_POSITIVE_COLOR2 = grp.GenerateColor(1.0, 0.8, 0.8, 1.0)
	SPECIAL_POSITIVE_COLOR5 = grp.GenerateColor(0.7824, 0.9004, 0.8024, 1.0)

	if app.ENABLE_BUY_WITH_ITEM:
		SHOP_ITEM_COLOR = 0xffc9bea7
		
	GLOVE_COLOR = grp.GenerateColor((122.00 / 255), (246.00 / 255), (212.00 / 255), 1.0)
	GLOVE_COLOR1 = grp.GenerateColor((137.00 / 255), (184.00 / 255), (141.00 / 255), 1.0)

	CONDITION_COLOR = 0xffBEB47D
	CAN_LEVEL_UP_COLOR = 0xff8EC292
	CANNOT_LEVEL_UP_COLOR = DISABLE_COLOR
	TEXTLINE_2ND_COLOR_DEFAULT = grp.GenerateColor(1.0, 1.0, 0.6078, 1.0)
	ITEMSHOP_UNIQUE_ITEM_COLOR = 0xFF008784
	NEED_SKILL_POINT_COLOR = 0xff9A9CDB
	renderOnKey = False

	def __init__(self, width = TOOL_TIP_WIDTH, isPickable=False):
		ui.ThinBoard.__init__(self, "TOP_MOST")

		if isPickable:
			pass
		else:
			self.AddFlag("not_pick")

		self.AddFlag("float")

		self.showFunctions = False
		self.followFlag = True
		self.toolTipWidth = width

		self.xPos = -1
		self.yPos = -1

		self.tooltipParent = None
		
		self.defFontName = localeInfo.UI_DEF_FONT
		self.ClearToolTip()

	def __del__(self):
		ui.ThinBoard.__del__(self)
	
	def AppendTextLineMultiBuy(self, text, color = FONT_COLOR, centerAlign = True, height = 0):
		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetFontName(self.defFontName)
		textLine.SetPackedFontColor(color)
		textLine.SetText(text)
		textLine.SetOutline()
		textLine.SetFeather(False)
		textLine.Show()

		if centerAlign:
			if height != 0:
				textLine.SetPosition(self.toolTipWidth/2, self.toolTipHeight + (height / 2))
			else:
				textLine.SetPosition(self.toolTipWidth/2, self.toolTipHeight)
			textLine.SetHorizontalAlignCenter()
		else:
			textLine.SetPosition(10, self.toolTipHeight)

		self.childrenList.append(textLine)
		
		if height == 0:
			height = self.TEXT_LINE_HEIGHT

		self.toolTipHeight += height
		self.ResizeToolTip()
		return textLine

	def ClearToolTip(self):
		self.toolTipHeight = 12
		self.childrenList = []

	def SetFollow(self, flag):
		self.followFlag = flag

	def SetDefaultFontName(self, fontName):
		self.defFontName = fontName

	def AppendSpace(self, size):
		self.toolTipHeight += size
		self.ResizeToolTip()

	def AppendHorizontalLine(self):

		for i in xrange(2):
			horizontalLine = ui.Line()
			horizontalLine.SetParent(self)
			horizontalLine.SetPosition(0, self.toolTipHeight + 3 + i)
			horizontalLine.SetWindowHorizontalAlignCenter()
			horizontalLine.SetSize(150, 0)
			horizontalLine.Show()

			if 0 == i:
				horizontalLine.SetColor(0xff555555)
			else:
				horizontalLine.SetColor(0xff000000)

			self.childrenList.append(horizontalLine)

		self.toolTipHeight += 11
		self.ResizeToolTip()

	def AlignHorizonalCenter(self):
		for child in self.childrenList:
			(x, y)=child.GetLocalPosition()
			child.SetPosition(self.toolTipWidth/2, y)

		self.ResizeToolTip()

	def SetThinBoardSize(self, width, height = 12):
		self.toolTipWidth = width
		self.toolTipHeight = height

	if app.ENABLE_DUNGEON_INFO_SYSTEM:
		def TextAlignHorizonalCenter(self):
			for child in self.childrenList:
				(x, y) = child.GetLocalPosition()
				try:
					if child.GetText() != "":
						child.SetPosition(self.toolTipWidth / 2, y)
				except:
					pass

			self.ResizeToolTip()

	def AutoAppendTextLine(self, text, color = FONT_COLOR, centerAlign = True, rebuild = False):
		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetFontName(self.defFontName)
		textLine.SetPackedFontColor(color)
		textLine.SetText(text)
		textLine.SetOutline()
		textLine.SetFeather(False)
		textLine.Show()

		if centerAlign:
			textLine.SetPosition(self.toolTipWidth/2, self.toolTipHeight)
			textLine.SetHorizontalAlignCenter()

		else:
			textLine.SetPosition(10, self.toolTipHeight)
		
		textLine.toolTipHeight = self.toolTipHeight
		textLine.width = 0

		self.childrenList.append(textLine)

		(textWidth, textHeight)=textLine.GetTextSize()

		textWidth += 40
		textHeight += 5

		if self.toolTipWidth < textWidth:
			self.toolTipWidth = textWidth
			if rebuild:
				for items in self.childrenList:
					if items.width > 0:
						items.SetPosition(self.toolTipWidth/2 - (items.width / 2), items.toolTipHeight)
					else:
						items.SetPosition(self.toolTipWidth/2, items.toolTipHeight)
						items.SetHorizontalAlignCenter()
		
		self.toolTipHeight += textHeight

		return textLine

	def ResizeToolTipText(self, x, y):
		self.SetSize(x, y)
		
	def AppendTwoColorTextLine(self, text, color, text2, color2 = TEXTLINE_2ND_COLOR_DEFAULT, centerAlign = True):
		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetFontName(self.defFontName)
		textLine.SetPackedFontColor(color)
		textLine.SetText(text)
		textLine.SetOutline()
		textLine.SetFeather(False)
		w, h = textLine.GetTextSize()
		
		textLine2 = ui.TextLine()
		textLine2.SetParent(textLine)
		textLine2.SetFontName(self.defFontName)
		textLine2.SetPackedFontColor(color2)
		textLine2.SetText(text2)
		textLine2.SetOutline()
		textLine2.SetFeather(False)
		textLine2.Show()
		
		w2, h2 = textLine2.GetTextSize()
		
		textLine.Show()
		if centerAlign:
			textLine.SetPosition(self.toolTipWidth/2-w2/2, self.toolTipHeight)
			textLine.SetHorizontalAlignCenter()
			textLine2.SetPosition(w/2, 0)
		else:
			textLine.SetPosition(10, self.toolTipHeight)
		
		self.childrenList.append(textLine)
		self.childrenList.append(textLine2)
		
		self.toolTipHeight += self.TEXT_LINE_HEIGHT
		
		self.ResizeToolTip()
		return textLine
		
	def AutoAppendNewTextLine(self, text, color = FONT_COLOR, centerAlign = True):
		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetFontName(self.defFontName)
		textLine.SetPackedFontColor(color)
		textLine.SetText(text)
		textLine.SetOutline()
		textLine.SetFeather(FALSE)
		textLine.Show()
		textLine.SetPosition(15, self.toolTipHeight)
		
		self.childrenList.append(textLine)
		(textWidth, textHeight) = textLine.GetTextSize()
		textWidth += 30
		textHeight += 10
		if self.toolTipWidth < textWidth:
			self.toolTipWidth = textWidth
		
		self.toolTipHeight += textHeight
		self.ResizeToolTipText(textWidth, self.toolTipHeight)
		return textLine

	def AppendTextLine(self, text, color = FONT_COLOR, centerAlign = True, rebuild = True, renew_timebar = 1):
		#if renew_timebar and text and text.find(localeInfo.LEFT_TIME+" : ") != -1:
		#	mainString = text.split(localeInfo.LEFT_TIME+" : ")[0].replace("(", "")
		#	timeString = text.split(localeInfo.LEFT_TIME+" : ")[1].replace(")", "")
		#	
		#	if len(mainString) > 0:
		#		self.AppendTextLine(mainString, color, centerAlign, rebuild)
		#	
		#	self.AppendSpace(5)
		#	self.AppendTextLine("["+localeInfo.DESC_COOLDOWN+"]", self.SPECIAL_TITLE_COLOR, True, True, 0)
		#	self.AppendTextLine(localeInfo.LEFT_TIME + " : "+timeString, color, True, True, 0)
		#	return
			
			
		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetFontName(self.defFontName)
		textLine.SetPackedFontColor(color)
		textLine.SetText(text)
		textLine.SetOutline()
		textLine.SetFeather(False)
		if app.WJ_MULTI_TEXTLINE:
			textLine.SetLineHeight(self.TEXT_LINE_HEIGHT)
		textLine.Show()
		
		if centerAlign:
			textLine.SetPosition(self.toolTipWidth/2, self.toolTipHeight)
			textLine.SetHorizontalAlignCenter()

		else:
			textLine.SetPosition(10, self.toolTipHeight)
		textLine.toolTipHeight = self.toolTipHeight
		textLine.width = 0

		self.childrenList.append(textLine)

		if app.WJ_MULTI_TEXTLINE:
			lineCount = textLine.GetTextLineCount()
			self.toolTipHeight += self.TEXT_LINE_HEIGHT * lineCount
		else:
			self.toolTipHeight += self.TEXT_LINE_HEIGHT
		
		(textWidth, textHeight)=textLine.GetTextSize()

		textWidth += 40
		textHeight += 5

		if self.toolTipWidth < textWidth:
			oldSize = self.toolTipWidth
			self.toolTipWidth = textWidth
			if rebuild:
				self.Rebuild(textWidth, oldSize)

		
		self.ResizeToolTip()

		return textLine

	def AppendTextLineInfo(self, text, text2, rebuild=True, countInfo=True, space=2, color=""):
		#if systemSetting.GetShortcutInfoSetting() == 1:
		#	return

		#if countInfo:
		#	if self.countInfo == 0:
		#		self.AppendSpace(10)
		#	# self.AppendTextLine("["+localeInfo.OPT_TITLE_04+"]", self.SPECIAL_TITLE_COLOR)
		#	self.countInfo += 1

		colhex = 0xAAcdac8a
		colorNames = {
			# "gold" : 0xffAD9F05,
		}
		if colorNames.has_key(color):
			colhex = colorNames[color]

		self.AppendSpace(space)
		mainWindow = ui.ScriptWindow()
		mainWindow.SetParent(self)
		mainWindow.Show()
		mainWindow.leftbar = ui.ImageBox()
		mainWindow.leftbar.SetParent(mainWindow)
		mainWindow.leftbar.SetPosition(0, 0)
		mainWindow.leftbar.LoadImage("elvion/game/tooltip/left" + color + ".png")
		# mainWindow.leftbar.SetDiffuseColor(0.5,0.5,1.0,1.0)
		mainWindow.leftbar.Show()
		mainWindow.rightbar = ui.ImageBox()
		mainWindow.rightbar.SetParent(mainWindow)
		mainWindow.rightbar.SetPosition(0, 0)
		mainWindow.rightbar.LoadImage("elvion/game/tooltip/right" + color + ".png")
		# mainWindow.rightbar.SetDiffuseColor(0.5,0.5,1.0,1.0)
		mainWindow.rightbar.Show()
		mainWindow.center = ui.ExpandedImageBox()
		mainWindow.center.SetParent(mainWindow)
		mainWindow.center.SetPosition(10, 0)
		mainWindow.center.LoadImage("elvion/game/tooltip/mid" + color + ".png")
		# mainWindow.center.SetDiffuseColor(0.5,0.5,1.0,1.0)
		mainWindow.center.Show()

		mainWindow.textLine = ui.TextLine()
		mainWindow.textLine.SetParent(mainWindow)
		mainWindow.textLine.SetFontName(self.defFontName)
		mainWindow.textLine.SetPackedFontColor(colhex)
		mainWindow.textLine.SetText(text2)
		mainWindow.textLine.SetOutline()
		mainWindow.textLine.SetFeather(False)
		mainWindow.textLine.Show()
		mainWindow.textLine.y = self.toolTipHeight
		mainWindow.textLine.SetPosition(10, 7)
		mainWindow.textLine2 = ui.TextLine()
		mainWindow.textLine2.SetParent(mainWindow)
		mainWindow.textLine2.SetFontName(self.defFontName)
		mainWindow.textLine2.SetPackedFontColor(colhex)
		mainWindow.textLine2.SetText(text)
		mainWindow.textLine2.SetOutline()
		mainWindow.textLine2.SetFeather(False)
		mainWindow.textLine2.SetHorizontalAlignRight()
		mainWindow.textLine2.Show()
		mainWindow.textLine2.y = self.toolTipHeight
		mainWindow.textLine2.SetPosition(self.toolTipWidth - 30, 6)
		mainWindow.SetPosition(10, self.toolTipHeight)

		mainWindow.toolTipHeight = self.toolTipHeight

		self.childrenList.append(mainWindow)

		self.toolTipHeight += 28

		(textWidth1, textHeight) = mainWindow.textLine.GetTextSize()
		(textWidth2, textHeight) = mainWindow.textLine2.GetTextSize()
		textWidth = textWidth1 + textWidth2
		textWidth += 40
		textHeight += 5

		if textWidth < 250:
			textWidth = 250

		mainWindow.width = self.toolTipWidth - 20
		mainWindow.rightbar.SetPosition(mainWindow.width - 10, 0)
		mainWindow.center.SetPercentage(mainWindow.width - 20, 10)

		if self.toolTipWidth < textWidth:
			oldSize = self.toolTipWidth
			mainWindow.width = textWidth - 20
			self.toolTipWidth = textWidth
			if rebuild:
				self.Rebuild(textWidth, oldSize)

		self.ResizeToolTip()

		return mainWindow
		
	def Rebuild(self, textWidth, oldSize):
		self.ResizeToolTip()
		for items in self.childrenList:
			if hasattr(items, 'rightbar'):
				items.width = self.toolTipWidth-20
				items.rightbar.SetPosition(items.width-10,0)
				items.center.SetPercentage(items.width-20, 10)
				items.SetPosition(self.toolTipWidth/2 - (items.width / 2), items.toolTipHeight)
				items.textLine2.SetPosition(self.toolTipWidth-30, 6)
				if hasattr(items, 'items'):
					for needitem in items.items:
						needitem[0].SetPosition(self.toolTipWidth-50,needitem[0].y)
						needitem[1].SetPosition(self.toolTipWidth-50-5,needitem[1].y)
			elif hasattr(items, 'rightbar2'):
				items.width = self.toolTipWidth-20
				items.rightbar2.SetPosition(items.width-10,0)
				items.center.SetPercentage(items.width-20, 10)
				items.SetPosition(self.toolTipWidth/2 - (items.width / 2), items.toolTipHeight)
				items.textLine.SetPosition(items.width/2, 7)
				items.textLine2.SetPosition(self.toolTipWidth-31, items.textLine2.y)	
				if hasattr(items, 'items'):
					for needitem in items.items:
						needitem[0].SetPosition(10,needitem[0].y)
						needitem[1].SetPosition(self.toolTipWidth-20-10,needitem[1].y)
			elif hasattr(items, 'rightbar3'):
				items.width = self.toolTipWidth-20
				items.rightbar3.SetPosition(items.width-10,0)
				items.center.SetPercentage(items.width-20, 10)
				items.SetPosition(self.toolTipWidth/2 - (items.width / 2), items.toolTipHeight)
				# items.textLine.SetPosition(40, items.textLine.y)
				# items.textLine2.SetPosition(self.toolTipWidth-31, items.textLine2.y)	
				# if hasattr(items, 'items'):
					# for needitem in items.items:
						# needitem[0].SetPosition(10,needitem[0].y)
						# needitem[1].SetPosition(self.toolTipWidth-20-10,needitem[1].y)
			elif hasattr(items, 'toolTipHeight'):
				if items.width > 0:
					items.SetPosition(self.toolTipWidth/2 - (items.width / 2), items.toolTipHeight)
				else:
					items.SetPosition(self.toolTipWidth/2, items.toolTipHeight)
					items.SetHorizontalAlignCenter()
			else:
				x,y = items.GetLocalPosition()
				items.SetPosition(x + (textWidth-oldSize)/2, y)

	def AppendTextLineInfoItem(self, vnum, textInfo, rebuild=True, itemInfo=True, space=2):
		if itemInfo:
			if self.itemInfo == 0:
				self.AppendSpace(15)
				self.AppendTextLine("[" + localeInfo.UISHAMAN27 + "]", self.SPECIAL_TITLE_COLOR)
			self.itemInfo += 1

		self.AppendSpace(space)
		mainWindow = ui.ScriptWindow()
		mainWindow.SetParent(self)
		mainWindow.Show()
		mainWindow.leftbar = ui.ExpandedImageBox()
		mainWindow.leftbar.SetParent(mainWindow)
		mainWindow.leftbar.SetPosition(0, 0)
		mainWindow.leftbar.LoadImage("elvion/game/tooltip/" + str(1) + "left.png")
		mainWindow.leftbar.SetDiffuseColor(0.5, 0.5, 0.5, 0.5)
		mainWindow.leftbar.Show()
		mainWindow.rightbar3 = ui.ImageBox()
		mainWindow.rightbar3.SetParent(mainWindow)
		mainWindow.rightbar3.SetPosition(0, 0)
		mainWindow.rightbar3.LoadImage("elvion/game/tooltip/" + str(1) + "right.png")
		mainWindow.rightbar3.SetDiffuseColor(0.5, 0.5, 0.5, 0.5)
		mainWindow.rightbar3.Show()
		mainWindow.center = ui.ExpandedImageBox()
		mainWindow.center.SetParent(mainWindow)
		mainWindow.center.SetPosition(10, 0)
		mainWindow.center.LoadImage("elvion/game/tooltip/" + str(1) + "mid.png")
		mainWindow.center.SetDiffuseColor(0.5, 0.5, 0.5, 0.5)
		mainWindow.center.Show()

		item.SelectItem(vnum)
		mainWindow.imageIcon = ui.ImageBox()
		mainWindow.imageIcon.SetParent(mainWindow)
		mainWindow.imageIcon.SetPosition(5, 10)
		itemName = item.GetIconImageFileName()
		if item.GetItemType() in [item.ITEM_TYPE_ARMOR, item.ITEM_TYPE_WEAPON]:
			if itemName.find("+") != -1:
				itemName = itemName.split("+")[0]
		mainWindow.imageIcon.LoadImage(itemName)
		mainWindow.imageIcon.Show()

		mainWindow.textLine = ui.TextLine()
		mainWindow.textLine.SetParent(mainWindow)
		mainWindow.textLine.SetFontName(self.defFontName)
		# mainWindow.textLine.SetPackedFontColor(0xAAcdac8a)
		mainWindow.textLine.SetText("|cffF1E6C0|H|h" + item.GetItemName())
		mainWindow.textLine.SetOutline()
		mainWindow.textLine.SetFeather(False)
		mainWindow.textLine.Show()
		mainWindow.textLine.y = 10
		if len(textInfo) <= 0:
			mainWindow.textLine.y = 19
		# mainWindow.textLine.SetHorizontalAlignCenter()
		# mainWindow.textLine.SetWindowHorizontalAlignCenter()
		mainWindow.textLine.SetPosition(45, mainWindow.textLine.y)
		mainWindow.textLine2 = ui.TextLine()
		mainWindow.textLine2.SetParent(mainWindow)
		mainWindow.textLine2.SetFontName(self.defFontName)
		mainWindow.textLine2.SetText(textInfo)
		# mainWindow.textLine2.SetPackedFontColor(0xAAcdac8a)
		mainWindow.textLine2.Show()

		mainWindow.textLine2.SetOutline()
		mainWindow.textLine2.SetFeather(False)
		# mainWindow.textLine2.SetHorizontalAlignRight()
		mainWindow.textLine2.Show()
		mainWindow.textLine2.y = 25
		mainWindow.textLine2.SetPosition(45, 25)
		mainWindow.SetPosition(10, self.toolTipHeight)

		mainWindow.toolTipHeight = self.toolTipHeight

		self.toolTipHeight += 54

		(textWidth1, textHeight) = mainWindow.textLine.GetTextSize()
		(textWidth2, textHeight) = mainWindow.textLine2.GetTextSize()
		textWidth = textWidth1
		if textWidth2 > textWidth:
			textWidth = textWidth2
		textWidth += 80
		# textWidth += 40
		textHeight += 5

		# if textWidth < 250:
		# textWidth = 250

		mainWindow.width = self.toolTipWidth - 20
		mainWindow.rightbar3.SetPosition(mainWindow.width - 10, 0)
		mainWindow.center.SetPercentage(mainWindow.width - 20, 10)

		self.childrenList.append(mainWindow)

		# mainWindow.textLine.SetPosition(40, mainWindow.textLine.y)

		if self.toolTipWidth < textWidth:
			oldSize = self.toolTipWidth
			mainWindow.width = textWidth - 20
			self.toolTipWidth = textWidth
			if True:
				self.Rebuild(textWidth, oldSize)

		self.ResizeToolTip()

		return mainWindow

	def AppendDescription(self, desc, limit, color = FONT_COLOR):
		if localeInfo.IsEUROPE():
			self.__AppendDescription_WesternLanguage(desc, color)
		else:
			self.__AppendDescription_EasternLanguage(desc, limit, color)

	def __AppendDescription_EasternLanguage(self, description, characterLimitation, color=FONT_COLOR):
		length = len(description)
		if 0 == length:
			return

		lineCount = grpText.GetSplitingTextLineCount(description, characterLimitation)
		for i in xrange(lineCount):
			if 0 == i:
				self.AppendSpace(5)
			self.AppendTextLine(grpText.GetSplitingTextLine(description, characterLimitation, i), color)

	def __AppendDescription_WesternLanguage(self, desc, color=FONT_COLOR):
		lines = SplitDescription(desc, DESC_WESTERN_MAX_COLS)
		if not lines:
			return

		self.AppendSpace(5)
		for line in lines:
			self.AppendTextLine(line, color)


	def ResizeToolTip(self):
		self.SetSize(self.toolTipWidth, self.TOOL_TIP_HEIGHT + self.toolTipHeight)

	def SetTitle(self, name):
		self.AppendTextLine(name, self.TITLE_COLOR)

	def GetLimitTextLineColor(self, curValue, limitValue):
		if curValue < limitValue:
			return self.DISABLE_COLOR

		return self.ENABLE_COLOR

	def GetChangeTextLineColor(self, value, isSpecial=False):
		if value > 0:
			if isSpecial:
				return self.SPECIAL_POSITIVE_COLOR
			else:
				return self.POSITIVE_COLOR

		if 0 == value:
			return self.NORMAL_COLOR

		return self.NEGATIVE_COLOR

	def SetToolTipPosition(self, x = -1, y = -1):
		self.xPos = x
		self.yPos = y

	def ShowToolTip(self):
		self.SetTop()
		self.Show()

		self.OnUpdate()

	def HideToolTip(self):
		self.Hide()

	def MakeToolTipNew(self):
		if not hasattr(self, "tooltipParent"):
			return
		if not hasattr(self.tooltipParent, "_tooltip_saved_args"):
			return
		if not self.tooltipParent.followFlag:
			return

		itemVnum = self.tooltipParent.itemVnum
		if not itemVnum:
			return
		
		showFunctions = self.tooltipParent.showFunctions
		isFromInventory = self.tooltipParent.isFromInventory
		guildSkin = self.tooltipParent.guildSkin
		isFromInventorySlot = self.tooltipParent.isFromInventorySlot
		isShopItem = self.tooltipParent.isShopItem
		isShopItemData = self.tooltipParent.isShopItemData
		(
			itemVnum, metinSlot, attrSlot, flags, unbindTime, window_type,
			slotIndex, isBorrow, showModelPreview, getInforced, getBlockTradeIS,
			showIcon, wikiItem, addwidth, window_open, tradeCount,
			guildEquipItem, count, getBlockTradeISSB
		) = self.tooltipParent._tooltip_saved_args

		self.tooltipParent.ClearToolTip()
		self.tooltipParent.HideToolTip()
		if isFromInventory:
			self.tooltipParent.SetFromInventory(isFromInventorySlot)
		self.tooltipParent.guildSkin = guildSkin
		self.tooltipParent.AddItemData(*self.tooltipParent._tooltip_saved_args)
		if showFunctions:
			self.tooltipParent.showFunctions = showFunctions
			self.tooltipParent.AppendFunctionLines(itemVnum, window_type, slotIndex)
		self.tooltipParent.isShopItem = isShopItem
		self.tooltipParent.isShopItemData = isShopItemData
		self.tooltipParent.CheckButtons()

		self.tooltipParent.ShowToolTip()

	def OnUpdate(self):

		self.MakeToolTipNew()
		if not self.followFlag:
			return

		x = 0
		y = 0
		width = self.GetWidth()
		height = self.toolTipHeight

		if -1 == self.xPos and -1 == self.yPos:

			(mouseX, mouseY) = wndMgr.GetMousePosition()

			if mouseY < wndMgr.GetScreenHeight() - 300:
				y = mouseY + 40
			else:
				y = mouseY - height - 30

			x = mouseX - width/2

		else:

			x = self.xPos - width/2
			y = self.yPos - height

		x = max(x, 0)
		y = max(y, 0)
		x = min(x + width/2, wndMgr.GetScreenWidth() - width/2) - width/2
		y = min(y + self.GetHeight(), wndMgr.GetScreenHeight()) - self.GetHeight()

		parentWindow = self.GetParentProxy()
		if parentWindow:
			(gx, gy) = parentWindow.GetGlobalPosition()
			x -= gx
			y -= gy

		self.SetPosition(x, y)

class ItemToolTip(ToolTip):
	CHARACTER_NAMES = (
		localeInfo.TOOLTIP_WARRIOR,
		localeInfo.TOOLTIP_ASSASSIN,
		localeInfo.TOOLTIP_SURA,
		localeInfo.TOOLTIP_SHAMAN
	)

	if app.ENABLE_WOLFMAN_CHARACTER:
		CHARACTER_NAMES += (
			localeInfo.TOOLTIP_WOLFMAN,
		)

	POSITIVE_COLOR = grp.GenerateColor(0.5411, 0.7254, 0.5568, 1.0)

	CHARACTER_COUNT = len(CHARACTER_NAMES)
	WEAR_NAMES = (
		localeInfo.TOOLTIP_ARMOR,
		localeInfo.TOOLTIP_HELMET,
		localeInfo.TOOLTIP_SHOES,
		localeInfo.TOOLTIP_WRISTLET,
		localeInfo.TOOLTIP_WEAPON,
		localeInfo.TOOLTIP_NECK,
		localeInfo.TOOLTIP_EAR,
		localeInfo.TOOLTIP_UNIQUE,
		localeInfo.TOOLTIP_SHIELD,
		localeInfo.TOOLTIP_ARROW,
	)
	WEAR_COUNT = len(WEAR_NAMES)

	AFFECT_DICT = {
		item.APPLY_MAX_HP : localeInfo.TOOLTIP_MAX_HP,
		item.APPLY_MAX_SP : localeInfo.TOOLTIP_MAX_SP,
		item.APPLY_CON : localeInfo.TOOLTIP_CON,
		item.APPLY_INT : localeInfo.TOOLTIP_INT,
		item.APPLY_STR : localeInfo.TOOLTIP_STR,
		item.APPLY_DEX : localeInfo.TOOLTIP_DEX,
		item.APPLY_ATT_SPEED : localeInfo.TOOLTIP_ATT_SPEED,
		item.APPLY_MOV_SPEED : localeInfo.TOOLTIP_MOV_SPEED,
		item.APPLY_CAST_SPEED : localeInfo.TOOLTIP_CAST_SPEED,
		item.APPLY_HP_REGEN : localeInfo.TOOLTIP_HP_REGEN,
		item.APPLY_SP_REGEN : localeInfo.TOOLTIP_SP_REGEN,
		item.APPLY_POISON_PCT : localeInfo.TOOLTIP_APPLY_POISON_PCT,
		item.APPLY_STUN_PCT : localeInfo.TOOLTIP_APPLY_STUN_PCT,
		item.APPLY_SLOW_PCT : localeInfo.TOOLTIP_APPLY_SLOW_PCT,
		item.APPLY_CRITICAL_PCT : localeInfo.TOOLTIP_APPLY_CRITICAL_PCT,
		item.APPLY_PENETRATE_PCT : localeInfo.TOOLTIP_APPLY_PENETRATE_PCT,
		item.APPLY_ATTBONUS_WARRIOR : localeInfo.TOOLTIP_APPLY_ATTBONUS_WARRIOR,
		item.APPLY_ATTBONUS_ASSASSIN : localeInfo.TOOLTIP_APPLY_ATTBONUS_ASSASSIN,
		item.APPLY_ATTBONUS_SURA : localeInfo.TOOLTIP_APPLY_ATTBONUS_SURA,
		item.APPLY_ATTBONUS_SHAMAN : localeInfo.TOOLTIP_APPLY_ATTBONUS_SHAMAN,
		item.APPLY_ATTBONUS_MONSTER : localeInfo.TOOLTIP_APPLY_ATTBONUS_MONSTER,
		item.APPLY_ATTBONUS_HUMAN : localeInfo.TOOLTIP_APPLY_ATTBONUS_HUMAN,
		item.APPLY_ATTBONUS_ANIMAL : localeInfo.TOOLTIP_APPLY_ATTBONUS_ANIMAL,
		item.APPLY_ATTBONUS_ORC : localeInfo.TOOLTIP_APPLY_ATTBONUS_ORC,
		item.APPLY_ATTBONUS_MILGYO : localeInfo.TOOLTIP_APPLY_ATTBONUS_MILGYO,
		item.APPLY_ATTBONUS_UNDEAD : localeInfo.TOOLTIP_APPLY_ATTBONUS_UNDEAD,
		item.APPLY_ATTBONUS_DEVIL : localeInfo.TOOLTIP_APPLY_ATTBONUS_DEVIL,
		item.APPLY_STEAL_HP : localeInfo.TOOLTIP_APPLY_STEAL_HP,
		item.APPLY_STEAL_SP : localeInfo.TOOLTIP_APPLY_STEAL_SP,
		item.APPLY_MANA_BURN_PCT : localeInfo.TOOLTIP_APPLY_MANA_BURN_PCT,
		item.APPLY_DAMAGE_SP_RECOVER : localeInfo.TOOLTIP_APPLY_DAMAGE_SP_RECOVER,
		item.APPLY_BLOCK : localeInfo.TOOLTIP_APPLY_BLOCK,
		item.APPLY_DODGE : localeInfo.TOOLTIP_APPLY_DODGE,
		item.APPLY_RESIST_SWORD : localeInfo.TOOLTIP_APPLY_RESIST_SWORD,
		item.APPLY_RESIST_TWOHAND : localeInfo.TOOLTIP_APPLY_RESIST_TWOHAND,
		item.APPLY_RESIST_DAGGER : localeInfo.TOOLTIP_APPLY_RESIST_DAGGER,
		item.APPLY_RESIST_BELL : localeInfo.TOOLTIP_APPLY_RESIST_BELL,
		item.APPLY_RESIST_FAN : localeInfo.TOOLTIP_APPLY_RESIST_FAN,
		item.APPLY_RESIST_BOW : localeInfo.TOOLTIP_RESIST_BOW,
		item.APPLY_RESIST_FIRE : localeInfo.TOOLTIP_RESIST_FIRE,
		item.APPLY_RESIST_ELEC : localeInfo.TOOLTIP_RESIST_ELEC,
		item.APPLY_RESIST_MAGIC : localeInfo.TOOLTIP_RESIST_MAGIC,
		item.APPLY_RESIST_WIND : localeInfo.TOOLTIP_APPLY_RESIST_WIND,
		item.APPLY_REFLECT_MELEE : localeInfo.TOOLTIP_APPLY_REFLECT_MELEE,
		item.APPLY_REFLECT_CURSE : localeInfo.TOOLTIP_APPLY_REFLECT_CURSE,
		item.APPLY_POISON_REDUCE : localeInfo.TOOLTIP_APPLY_POISON_REDUCE,
		item.APPLY_KILL_SP_RECOVER : localeInfo.TOOLTIP_APPLY_KILL_SP_RECOVER,
		item.APPLY_EXP_DOUBLE_BONUS : localeInfo.TOOLTIP_APPLY_EXP_DOUBLE_BONUS,
		item.APPLY_GOLD_DOUBLE_BONUS : localeInfo.TOOLTIP_APPLY_GOLD_DOUBLE_BONUS,
		item.APPLY_ITEM_DROP_BONUS : localeInfo.TOOLTIP_APPLY_ITEM_DROP_BONUS,
		item.APPLY_POTION_BONUS : localeInfo.TOOLTIP_APPLY_POTION_BONUS,
		item.APPLY_KILL_HP_RECOVER : localeInfo.TOOLTIP_APPLY_KILL_HP_RECOVER,
		item.APPLY_IMMUNE_STUN : localeInfo.TOOLTIP_APPLY_IMMUNE_STUN,
		item.APPLY_IMMUNE_SLOW : localeInfo.TOOLTIP_APPLY_IMMUNE_SLOW,
		item.APPLY_IMMUNE_FALL : localeInfo.TOOLTIP_APPLY_IMMUNE_FALL,
		item.APPLY_BOW_DISTANCE : localeInfo.TOOLTIP_BOW_DISTANCE,
		item.APPLY_DEF_GRADE_BONUS : localeInfo.TOOLTIP_DEF_GRADE,
		item.APPLY_ATT_GRADE_BONUS : localeInfo.TOOLTIP_ATT_GRADE,
		item.APPLY_MAGIC_ATT_GRADE : localeInfo.TOOLTIP_MAGIC_ATT_GRADE,
		item.APPLY_MAGIC_DEF_GRADE : localeInfo.TOOLTIP_MAGIC_DEF_GRADE,
		item.APPLY_MAX_STAMINA : localeInfo.TOOLTIP_MAX_STAMINA,
		item.APPLY_MALL_ATTBONUS : localeInfo.TOOLTIP_MALL_ATTBONUS,
		item.APPLY_MALL_DEFBONUS : localeInfo.TOOLTIP_MALL_DEFBONUS,
		item.APPLY_MALL_EXPBONUS : localeInfo.TOOLTIP_MALL_EXPBONUS,
		item.APPLY_MALL_ITEMBONUS : localeInfo.TOOLTIP_MALL_ITEMBONUS,
		item.APPLY_MALL_GOLDBONUS : localeInfo.TOOLTIP_MALL_GOLDBONUS,
		item.APPLY_SKILL_DAMAGE_BONUS : localeInfo.TOOLTIP_SKILL_DAMAGE_BONUS,
		item.APPLY_NORMAL_HIT_DAMAGE_BONUS : localeInfo.TOOLTIP_NORMAL_HIT_DAMAGE_BONUS,
		item.APPLY_SKILL_DEFEND_BONUS : localeInfo.TOOLTIP_SKILL_DEFEND_BONUS,
		item.APPLY_NORMAL_HIT_DEFEND_BONUS : localeInfo.TOOLTIP_NORMAL_HIT_DEFEND_BONUS,
		item.APPLY_PC_BANG_EXP_BONUS : localeInfo.TOOLTIP_MALL_EXPBONUS_P_STATIC,
		item.APPLY_PC_BANG_DROP_BONUS : localeInfo.TOOLTIP_MALL_ITEMBONUS_P_STATIC,
		item.APPLY_RESIST_WARRIOR : localeInfo.TOOLTIP_APPLY_RESIST_WARRIOR,
		item.APPLY_RESIST_ASSASSIN : localeInfo.TOOLTIP_APPLY_RESIST_ASSASSIN,
		item.APPLY_RESIST_SURA : localeInfo.TOOLTIP_APPLY_RESIST_SURA,
		item.APPLY_RESIST_SHAMAN : localeInfo.TOOLTIP_APPLY_RESIST_SHAMAN,
		item.APPLY_MAX_HP_PCT : localeInfo.TOOLTIP_APPLY_MAX_HP_PCT,
		item.APPLY_MAX_SP_PCT : localeInfo.TOOLTIP_APPLY_MAX_SP_PCT,
		item.APPLY_ENERGY : localeInfo.TOOLTIP_ENERGY,
		item.APPLY_COSTUME_ATTR_BONUS : localeInfo.TOOLTIP_COSTUME_ATTR_BONUS,
		item.APPLY_MAGIC_ATTBONUS_PER : localeInfo.TOOLTIP_MAGIC_ATTBONUS_PER,
		item.APPLY_MELEE_MAGIC_ATTBONUS_PER : localeInfo.TOOLTIP_MELEE_MAGIC_ATTBONUS_PER,
		item.APPLY_RESIST_ICE : localeInfo.TOOLTIP_RESIST_ICE,
		item.APPLY_RESIST_EARTH : localeInfo.TOOLTIP_RESIST_EARTH,
		item.APPLY_RESIST_DARK : localeInfo.TOOLTIP_RESIST_DARK,
		item.APPLY_ANTI_CRITICAL_PCT : localeInfo.TOOLTIP_ANTI_CRITICAL_PCT,
		item.APPLY_ANTI_PENETRATE_PCT : localeInfo.TOOLTIP_ANTI_PENETRATE_PCT,
		item.APPLY_RESIST_HUMAN : localeInfo.TOOLTIP_APPLY_RESIST_HUMAN,
		item.APPLY_ATTBONUS_BOSS : localeInfo.TOOLTIP_APPLY_ATTBONUS_BOSS,
		item.APPLY_ATTBONUS_WLADCA : localeInfo.TOOLTIP_APPLY_ATTBONUS_WLADCA,
		item.APPLY_ATTBONUS_STONE : localeInfo.TOOLTIP_APPLY_ATTBONUS_STONE,
		item.APPLY_RESIST_BOSS : localeInfo.TOOLTIP_RESIST_BOSS,
		item.APPLY_RESIST_WLADCA : localeInfo.TOOLTIP_RESIST_WLADCA,
		item.APPLY_RESIST_MONSTER : localeInfo.TOOLTIP_RESIST_MONSTER,
		item.APPLY_STAT_BONUS: localeInfo.TOOLTIP_STAT_BONUS,
		item.APPLY_ATTBONUS_DUNGEON : localeInfo.TOOLTIP_ATTBONUS_DUNGEON,
		item.APPLY_ATTBONUS_KLASY : localeInfo.TOOLTIP_APPLY_ATTBONUS_KLASY,
		item.APPLY_RESIST_KLASY : localeInfo.TOOLTIP_APPLY_RESIST_KLASY,
        item.APPLY_ATTBONUS_LEGENDA : localeInfo.TOOLTIP_APPLY_ATTBONUS_LEGENDA,
		item.APPLY_DMG_BONUS : localeInfo.TOOLTIP_APPLY_DMG_BONUS,
		item.APPLY_FINAL_DMG_BONUS : localeInfo.TOOLTIP_APPLY_FINAL_DMG_BONUS,
		item.APPLY_MINING_SUCCESS_CHANCE : localeInfo.TOOLTIP_MINING_SUCCESS_CHANCE,
		item.APPLY_MINING_MORE_ORES_CHANCE : localeInfo.TOOLTIP_MINING_MORE_ORES_CHANCE,
		item.APPLY_MINING_LESS_MINE_TIME : localeInfo.TOOLTIP_MINING_LESS_MINE_TIME,
		item.APPLY_MINING_MORE_POINTS : localeInfo.TOOLTIP_MINING_MORE_POINTS,

		item.APPLY_FISHING_INSTANT_CATCH_CHANCE : localeInfo.TOOLTIP_FISHING_INSTANT_CATCH_CHANCE,
		item.APPLY_FISHING_BIGGER_FISH_CHANCE : localeInfo.TOOLTIP_FISHING_BIGGER_FISH_CHANCE,
		item.APPLY_FISHING_NEXT_SECTION_CHANCE : localeInfo.TOOLTIP_FISHING_NEXT_SECTION_CHANCE,
		item.APPLY_FISHING_MORE_YANG_MULTIPLIER : localeInfo.TOOLTIP_FISHING_MORE_YANG_MULTIPLIER,
		item.APPLY_FISHING_MORE_POINTS_CHANCE : localeInfo.TOOLTIP_FISHING_MORE_POINTS_CHANCE,
		item.APPLY_FISHING_SUCCESS_CHANCE : localeInfo.TOOLTIP_FISHING_SUCCESS_CHANCE,
		item.APPLY_ATTBONUS_PVM_SKILL : localeInfo.TOOLTIP_ATTBONUS_PVM_SKILL,
		item.APPLY_ATTBONUS_ELEMENT : localeInfo.TOOLTIP_ATTBONUS_ELEMENT,
		item.APPLY_DEFBONUS_ELEMENT : localeInfo.TOOLTIP_DEFBONUS_ELEMENT,
		item.APPLY_ENCHANT_ELECT : localeInfo.TOOLTIP_ENCHANT_ELEC,
		item.APPLY_ENCHANT_FIRE : localeInfo.TOOLTIP_ENCHANT_FIRE,
		item.APPLY_ENCHANT_ICE : localeInfo.TOOLTIP_ENCHANT_ICE,
		item.APPLY_ENCHANT_WIND : localeInfo.TOOLTIP_ENCHANT_WIND,
		item.APPLY_ENCHANT_EARTH : localeInfo.TOOLTIP_ENCHANT_EARTH,
		item.APPLY_ENCHANT_DARK : localeInfo.TOOLTIP_ENCHANT_DARK,
		item.APPLY_DOUBLE_LOOT_SUMMER : localeInfo.TOOLTIP_DOUBLE_LOOT_SUMMER,
		item.APPLY_DOUBLE_LOOT_DROP : localeInfo.TOOLTIP_DOUBLE_LOOT_DROP,
		item.APPLY_ATTBONUS_PVM_STR : localeInfo.TOOLTIP_ATT_GRADE_PVM,
	}
	AFFECT_DICT_POLY = {
		item.APPLY_ATTBONUS_HUMAN : localeInfo.TOOLTIP_APPLY_ATTBONUS_HUMAN_POLY,
		item.APPLY_ATTBONUS_MONSTER : localeInfo.TOOLTIP_APPLY_ATTBONUS_MONSTER_POLY,
		item.APPLY_ATTBONUS_BOSS : localeInfo.TOOLTIP_APPLY_ATTBONUS_BOSS_POLY,
		item.APPLY_ATTBONUS_STONE : localeInfo.TOOLTIP_APPLY_ATTBONUS_STONE_POLY,
		item.APPLY_MAX_HP : localeInfo.TOOLTIP_APPLY_ATTBONUS_MAX_HP_POLY,
		item.APPLY_ENCHANT_DARK : localeInfo.TOOLTIP_APPLY_ATTBONUS_ENCHANT_DARK_POLY,
		item.APPLY_ATTBONUS_WLADCA : localeInfo.TOOLTIP_APPLY_ATTBONUS_WLADCA_POLY,
		item.APPLY_ATTBONUS_ELEMENT : localeInfo.TOOLTIP_ATTBONUS_ELEMENT_POLY,
	}

	MAX_AFFECT_VALUE = {
		0: -1,
		item.APPLY_MAX_HP: 2500,
		item.APPLY_MAX_SP: 200,
		item.APPLY_CON: 12,
		item.APPLY_INT: 12,
		item.APPLY_STR: 12,
		item.APPLY_DEX: 12,
		item.APPLY_ATT_SPEED: 8,
		item.APPLY_MOV_SPEED: 20,
		item.APPLY_CAST_SPEED: 20,
		item.APPLY_HP_REGEN: 30,
		item.APPLY_SP_REGEN: 20,
		item.APPLY_POISON_PCT: 8,
		item.APPLY_STUN_PCT: 8,
		item.APPLY_SLOW_PCT: 8,
		item.APPLY_CRITICAL_PCT: 10,
		item.APPLY_PENETRATE_PCT: 20,
		item.APPLY_ATTBONUS_WARRIOR: 10,
		item.APPLY_ATTBONUS_ASSASSIN: 10,
		item.APPLY_ATTBONUS_SURA: 10,
		item.APPLY_ATTBONUS_SHAMAN: 10,
		item.APPLY_ATTBONUS_MONSTER: 10,
		item.APPLY_ATTBONUS_HUMAN: 10,
		item.APPLY_ATTBONUS_ANIMAL: 20,
		item.APPLY_ATTBONUS_ORC: 20,
		item.APPLY_ATTBONUS_MILGYO: 20,
		item.APPLY_ATTBONUS_UNDEAD: 20,
		item.APPLY_ATTBONUS_DEVIL: 20,
		item.APPLY_STEAL_HP: 10,
		item.APPLY_STEAL_SP: 10,
		item.APPLY_MANA_BURN_PCT: 10,
		item.APPLY_DAMAGE_SP_RECOVER: 0,
		item.APPLY_BLOCK: 15,
		item.APPLY_DODGE: 15,
		item.APPLY_RESIST_SWORD: 15,
		item.APPLY_RESIST_TWOHAND: 15,
		item.APPLY_RESIST_DAGGER: 15,
		item.APPLY_RESIST_BELL: 15,
		item.APPLY_RESIST_FAN: 15,
		item.APPLY_RESIST_BOW: 15,
		item.APPLY_RESIST_FIRE: 15,
		item.APPLY_RESIST_ELEC: 15,
		item.APPLY_RESIST_MAGIC: 15,
		item.APPLY_RESIST_WIND: 15,
		item.APPLY_REFLECT_MELEE: 10,
		item.APPLY_REFLECT_CURSE: 0,
		item.APPLY_POISON_REDUCE: 8,
		item.APPLY_KILL_SP_RECOVER: 0,
		item.APPLY_EXP_DOUBLE_BONUS: 20,
		item.APPLY_GOLD_DOUBLE_BONUS: 20,
		item.APPLY_ITEM_DROP_BONUS: 20,
		item.APPLY_POTION_BONUS: 0,
		item.APPLY_KILL_HP_RECOVER: 0,
		item.APPLY_IMMUNE_STUN: 1,
		item.APPLY_IMMUNE_SLOW: 1,
		item.APPLY_IMMUNE_FALL: 0,
		item.APPLY_BOW_DISTANCE: 0,
		item.APPLY_DEF_GRADE_BONUS: 0,
		item.APPLY_ATT_GRADE_BONUS: 50,
		item.APPLY_MAGIC_ATT_GRADE: 50,
		item.APPLY_MAGIC_DEF_GRADE: 0,
		item.APPLY_MAX_STAMINA: 0,
		item.APPLY_MALL_ATTBONUS: 0,
		item.APPLY_MALL_DEFBONUS: 0,
		item.APPLY_MALL_EXPBONUS: 0,
		item.APPLY_MALL_ITEMBONUS: 0,
		item.APPLY_MALL_GOLDBONUS: 0,
		item.APPLY_SKILL_DAMAGE_BONUS: 0,
		item.APPLY_NORMAL_HIT_DAMAGE_BONUS: 0,
		item.APPLY_SKILL_DEFEND_BONUS: 0,
		item.APPLY_NORMAL_HIT_DEFEND_BONUS: 0,
		item.APPLY_PC_BANG_EXP_BONUS: 0,
		item.APPLY_PC_BANG_DROP_BONUS: 0,
		item.APPLY_RESIST_WARRIOR: 5,
		item.APPLY_RESIST_ASSASSIN: 5,
		item.APPLY_RESIST_SURA: 5,
		item.APPLY_RESIST_SHAMAN: 5,
		item.APPLY_MAX_HP_PCT: 0,
		item.APPLY_MAX_SP_PCT: 0,
		item.APPLY_ENERGY: 0,
		item.APPLY_COSTUME_ATTR_BONUS: 0,
		item.APPLY_MAGIC_ATTBONUS_PER: 0,
		item.APPLY_MELEE_MAGIC_ATTBONUS_PER: 0,
		item.APPLY_RESIST_ICE: 0,
		item.APPLY_RESIST_EARTH: 0,
		item.APPLY_RESIST_DARK: 0,
		item.APPLY_ANTI_CRITICAL_PCT: 0,
		item.APPLY_ANTI_PENETRATE_PCT: 0,
		item.APPLY_STAT_BONUS : 0,
		item.APPLY_RESIST_MONSTER : 0,
		item.APPLY_ATTBONUS_DUNGEON : 0,
		item.APPLY_ATTBONUS_LEGENDA : 0,
		item.APPLY_ATTBONUS_KLASY : 0,
		item.APPLY_DMG_BONUS : 0,
		item.APPLY_FINAL_DMG_BONUS : 0,
		item.APPLY_RESIST_MONSTER : 0,
		item.APPLY_RESIST_BOSS : 0,
		item.APPLY_RESIST_WLADCA : 0,
		item.APPLY_RESIST_HUMAN : 0,
		item.APPLY_ATTBONUS_BOSS : 8,
		item.APPLY_ATTBONUS_STONE : 8,
		item.APPLY_RESIST_KLASY : 0,
		item.APPLY_ATTBONUS_PVM_SKILL : 0,
		item.APPLY_ATTBONUS_ELEMENT : 0,
		item.APPLY_DEFBONUS_ELEMENT : 0,
		item.APPLY_DOUBLE_LOOT_SUMMER : 0,
		item.APPLY_DOUBLE_LOOT_DROP : 0,
	}

	MAX_AFFECT_VALUE_BELT = {
		0 : -1,
		item.APPLY_ATTBONUS_ANIMAL : 10,
		item.APPLY_ATTBONUS_MILGYO : 10,
		item.APPLY_RESIST_BELL : 10,
		item.APPLY_INT : 10,
		item.APPLY_STR : 10,
		item.APPLY_DEX : 10,
		item.APPLY_ATTBONUS_ORC : 10,
		item.APPLY_ATTBONUS_DEVIL : 10,
		item.APPLY_ATTBONUS_UNDEAD : 10,
		item.APPLY_RESIST_FAN : 10,
		item.APPLY_MAGIC_ATT_GRADE : 75,
		item.APPLY_ATT_GRADE_BONUS : 100,
		item.APPLY_RESIST_SWORD : 10,
		item.APPLY_RESIST_TWOHAND : 10,
		item.APPLY_RESIST_BOW : 10,
		item.APPLY_RESIST_DAGGER : 10,
	}
	MAX_AFFECT_VALUE_GLOVE = {
		0 : -1,
		item.APPLY_ATTBONUS_HUMAN : 7,
		item.APPLY_ATTBONUS_MILGYO : 7,
		item.APPLY_ATTBONUS_ORC : 7,
		item.APPLY_ATTBONUS_UNDEAD : 7,
		item.APPLY_MAX_SP : 1000,
		item.APPLY_ATTBONUS_STONE : 7,
		item.APPLY_ATTBONUS_DEVIL : 7,
		item.APPLY_STAT_BONUS : 7,
		item.APPLY_ATT_GRADE_BONUS : 75,
		item.APPLY_FINAL_DMG_BONUS : 0,
		item.APPLY_NORMAL_HIT_DAMAGE_BONUS : 0,
		item.APPLY_ATTBONUS_DUNGEON : 0,
		item.APPLY_SKILL_DAMAGE_BONUS : 0,
		item.APPLY_STEAL_HP : 0,
		item.APPLY_MAX_HP : 0,
		item.APPLY_CRITICAL_PCT : 0,
		item.APPLY_ATT_SPEED : 0,
		item.APPLY_MOV_SPEED : 0,
		item.APPLY_SLOW_PCT : 0,
		item.APPLY_POISON_PCT : 0,
		item.APPLY_RESIST_HUMAN : 0,
	}

	ATTRIBUTE_NEED_WIDTH = {
		23 : 230,
		24 : 230,
		25 : 230,
		26 : 220,
		27 : 210,

		35 : 210,
		36 : 210,
		37 : 210,
		38 : 210,
		39 : 210,
		40 : 210,
		41 : 210,

		42 : 220,
		43 : 230,
		45 : 230,
	}

	ANTI_FLAG_DICT = {
		0 : item.ITEM_ANTIFLAG_WARRIOR,
		1 : item.ITEM_ANTIFLAG_ASSASSIN,
		2 : item.ITEM_ANTIFLAG_SURA,
		3 : item.ITEM_ANTIFLAG_SHAMAN,
	}
	if app.ENABLE_WOLFMAN_CHARACTER:
		ANTI_FLAG_DICT.update({
			4 : item.ITEM_ANTIFLAG_WOLFMAN,
		})

	FONT_COLOR = grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0)

	def __init__(self, *args, **kwargs):
		ToolTip.__init__(self, *args, **kwargs)
		self.itemVnum = 0
		self.metinSlot = []
		self.isShopItem = False
		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			self.isOfflineShopItem = False

		self.bCannotUseItemForceSetDisableColor = True

		self.modelShow = True
		self.modelPreviewBoard = None
		self.modelPreviewRender = None
		self.modelPreviewBelka = None
		self.modelPreviewTextLine = None

		if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
			self.isPrivateSearchItem = False
			self.isPrivateShopSaleItem = False

	def __del__(self):
		ToolTip.__del__(self)
		self.metinSlot = None

	def SetCannotUseItemForceSetDisableColor(self, enable):
		self.bCannotUseItemForceSetDisableColor = enable

	def __AppendRealTimeToolTip(self, itemVnum, endTime):
		item.SelectItem(itemVnum)
		for i in xrange(item.LIMIT_MAX_NUM):
			(limitType, limitValue) = item.GetLimit(i)
			if item.LIMIT_REAL_TIME == limitType and limitValue > 0:
				self.AppendSpace(5)
				leftSec = max(0, endTime - app.GetGlobalTimeStamp())
				if leftSec > 0:
					self.AppendTextLine(localeInfo.LEFT_TIME + " : " + localeInfo.SecondToDHM(leftSec), self.NORMAL_COLOR)
				return
			else:
				continue

	def CanEquip(self):
		if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
			if self.isPrivateSearchItem or self.isPrivateShopSaleItem:
				return True

		if not item.IsEquipmentVID(self.itemVnum):
			return True

		race = player.GetOriginalRace()
		job = chr.RaceToJob(race)
		if not self.ANTI_FLAG_DICT.has_key(job):
			return False

		if item.IsAntiFlag(self.ANTI_FLAG_DICT[job]):
			return False

		sex = chr.RaceToSex(race)

		MALE = 1
		FEMALE = 0

		if item.IsAntiFlag(item.ITEM_ANTIFLAG_MALE) and sex == MALE:
			return False

		if item.IsAntiFlag(item.ITEM_ANTIFLAG_FEMALE) and sex == FEMALE:
			return False

		for i in xrange(item.LIMIT_MAX_NUM):
			(limitType, limitValue) = item.GetLimit(i)

			if item.LIMIT_LEVEL == limitType:
				if player.GetStatus(player.LEVEL) < limitValue:
					return False
			"""
			elif item.LIMIT_STR == limitType:
				if player.GetStatus(player.ST) < limitValue:
					return False
			elif item.LIMIT_DEX == limitType:
				if player.GetStatus(player.DX) < limitValue:
					return False
			elif item.LIMIT_INT == limitType:
				if player.GetStatus(player.IQ) < limitValue:
					return False
			elif item.LIMIT_CON == limitType:
				if player.GetStatus(player.HT) < limitValue:
					return False
			"""

		return True
		
	def AppendTextLine(self, text, color = FONT_COLOR, centerAlign = True, rebuild = True, renew_timebar = 1):
		if not self.CanEquip() and self.bCannotUseItemForceSetDisableColor:
			color = self.DISABLE_COLOR

		return ToolTip.AppendTextLine(self, text, color, centerAlign, rebuild, renew_timebar)

	def AppendTextLineBuyItem(self, text, color = FONT_COLOR, centerAlign = True, height = 0):
		if not self.CanEquip() and self.bCannotUseItemForceSetDisableColor:
			color = self.DISABLE_COLOR

		self.ResizeToolTip()
		return ToolTip.AppendTextLineMultiBuy(self, text, color, centerAlign, height)

	def GetAffectStrings(self, affectType, affectValue):
		if 0 == affectType:
			return None

		if 0 == affectValue:
			return None

		try:
			return self.AFFECT_DICT[affectType](affectValue)
		except TypeError:
			return "UNKNOWN_VALUE[%s] %s" % (affectType, affectValue)
		except KeyError:
			return "UNKNOWN_TYPE[%s] %s" % (affectType, affectValue)

	def ClearToolTip(self):
		self.isShopItem = False
		self.showFunctions = False
		self.toolTipWidth = self.TOOL_TIP_WIDTH
		ToolTip.ClearToolTip(self)

		if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
			self.isPrivateSearchItem = False
			self.isPrivateShopSaleItem = False

	if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
		def SetPrivateShopItem(self, slotIndex):
			itemVnum = privateShop.GetItemVnum(slotIndex)
			if 0 == itemVnum:
				return

			self.ClearToolTip()
			
			metinSlot = []
			for i in xrange(player.METIN_SOCKET_MAX_NUM):
				metinSlot.append(privateShop.GetItemMetinSocket(slotIndex, i))
			attrSlot = []
			for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				attrSlot.append(privateShop.GetItemAttribute(slotIndex, i))

			args = {}
			if app.ENABLE_PRIVATE_SHOP_REFINE_ELEMENT:
				args["refineElement"] = privateShop.GetItemRefineElement(slotIndex)

			apply_random_list = []
			if app.ENABLE_PRIVATE_SHOP_APPLY_RANDOM:
				for i in range(player.APPLY_RANDOM_SLOT_MAX_NUM):
					apply_random_list.append(privateShop.GetItemApplyRandom(slotIndex, i))
				args["apply_random_list"] = apply_random_list

			self.AddItemData(itemVnum, metinSlot, attrSlot, **args)

			if app.ENABLE_PRIVATE_SHOP_CHANGE_LOOK:
				changelookvnum = privateShop.GetItemChangeLookVnum(slotIndex)
				self.AppendChangeLookInfoItemVnum(changelookvnum)
			
			if app.ENABLE_CHEQUE_SYSTEM:
				goldPrice = privateShop.GetItemPrice(slotIndex)
				chequePrice = privateShop.GetChequeItemPrice(slotIndex)
				self.AppendSellingPrice(goldPrice, chequePrice, True)
			else:
				self.AppendSellingPrice(privateShop.GetItemPrice(slotIndex))
				
				
		def SetPrivateShopSearchItem(self, slotIndex):
			itemVnum = privateShop.GetSearchItemVnum(slotIndex)
			if 0 == itemVnum:
				return
				
			self.ClearToolTip()
			self.isPrivateSearchItem = True
			
			metinSlot = []
			for i in xrange(player.METIN_SOCKET_MAX_NUM):
				metinSlot.append(privateShop.GetSearchItemMetinSocket(slotIndex, i))
			attrSlot = []
			for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				attrSlot.append(privateShop.GetSearchItemAttribute(slotIndex, i))

			args = {}
			if app.ENABLE_PRIVATE_SHOP_REFINE_ELEMENT:
				args["refineElement"] = privateShop.GetSearchItemRefineElement(slotIndex)

			apply_random_list = []
			if app.ENABLE_PRIVATE_SHOP_APPLY_RANDOM:
				for i in range(player.APPLY_RANDOM_SLOT_MAX_NUM):
					apply_random_list.append(privateShop.GetSearchItemApplyRandom(slotIndex, i))
				args["apply_random_list"] = apply_random_list

			self.AddItemData(itemVnum, metinSlot, attrSlot, **args)

			if app.ENABLE_PRIVATE_SHOP_CHANGE_LOOK:
				changelookvnum = privateShop.GetSearchItemChangeLookVnum(slotIndex)
				self.AppendChangeLookInfoItemVnum(changelookvnum)
			
		def SetPrivateShopSaleItem(self, slotIndex):
			itemVnum = privateShop.GetSaleItemVnum(slotIndex)
			if 0 == itemVnum:
				return
			
			self.ClearToolTip()
			self.isPrivateShopSaleItem = True
			
			self.itemVnum = itemVnum
			
			metinSlot = []
			for i in xrange(player.METIN_SOCKET_MAX_NUM):
				metinSlot.append(privateShop.GetSaleItemMetinSocket(slotIndex, i))
			attrSlot = []
			for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				attrSlot.append(privateShop.GetSaleItemAttribute(slotIndex, i))

			args = {}
			if app.ENABLE_PRIVATE_SHOP_REFINE_ELEMENT:
				args["refineElement"] = privateShop.GetSaleItemRefineElement(slotIndex)

			apply_random_list = []
			if app.ENABLE_PRIVATE_SHOP_APPLY_RANDOM:
				for i in range(player.APPLY_RANDOM_SLOT_MAX_NUM):
					apply_random_list.append(privateShop.GetSaleItemApplyRandom(slotIndex, i))
				args["apply_random_list"] = apply_random_list

			self.AddItemData(itemVnum, metinSlot, attrSlot, **args)
			
			if app.ENABLE_PRIVATE_SHOP_CHANGE_LOOK:
				changelookvnum = privateShop.GetSaleItemChangeLookVnum(slotIndex)
				self.AppendChangeLookInfoItemVnum(changelookvnum)

			self.AppendHorizontalLine()

			self.AppendTextLine(localeInfo.PREMIUM_PRIVATE_SALE_CUSTOMER % privateShop.GetSaleCustomerName(slotIndex), self.TITLE_COLOR)
			
			timestamp = privateShop.GetSaleTime(slotIndex)

			self.AppendTextLine(localeInfo.PREMIUM_PRIVATE_SALE_TIME % localeInfo.GetFullDateFormat(timestamp), self.TITLE_COLOR)
			self.AppendSpace(2)
			
			if app.ENABLE_CHEQUE_SYSTEM:
				goldPrice = privateShop.GetSaleItemPrice(slotIndex)
				chequePrice = privateShop.GetSaleChequeItemPrice(slotIndex)
				self.AppendSellingPrice(goldPrice, chequePrice, True)
			else:
				self.AppendSellingPrice(privateShop.GetSaleItemPrice(slotIndex))

	if app.ENABLE_OFFLINE_SHOP_SYSTEM:
		def SetOfflineShopBuilderItem(self, invenType, invenPos, offlineShopIndex):
			itemVnum = player.GetItemIndex(invenType, invenPos)
			if (itemVnum == 0):
				return

			item.SelectItem(itemVnum)
			self.ClearToolTip()

			metinSlot = []
			for i in xrange(player.METIN_SOCKET_MAX_NUM):
				metinSlot.append(player.GetItemMetinSocket(invenType, invenPos, i))
			attrSlot = []
			for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				attrSlot.append(player.GetItemAttribute(invenType, invenPos, i))
			
			self.AddItemData(itemVnum, metinSlot, attrSlot)

			self.AppendTextLine(localeInfo.TOOLTIP_BUYPRICE_TEXT)
			self.AppendHorizontalLine()
			self.AppendOfflineShopSellingPrice(shop.GetOfflineShopItemPriceDisplayGold(invenType, invenPos), shop.GetOfflineShopItemPriceDisplayCheque(invenType, invenPos))

		def SetOfflineShopItem(self, slotIndex):
			itemVnum = shop.GetOfflineShopItemID(slotIndex)
			if (itemVnum == 0):
				return
				
			price_gold = shop.GetOfflineShopItemPriceGold(slotIndex)
			price_cheque = shop.GetOfflineShopItemPriceCheque(slotIndex)

			self.ClearToolTip()
			self.isOfflineShopItem = True
			
			metinSlot = []
			for i in xrange(player.METIN_SOCKET_MAX_NUM):
				metinSlot.append(shop.GetOfflineShopItemMetinSocket(slotIndex, i))
			attrSlot = []
			for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				attrSlot.append(shop.GetOfflineShopItemAttribute(slotIndex, i))
			
			self.AddItemData(itemVnum, metinSlot, attrSlot)

			self.AppendTextLine(localeInfo.SELL_PRICE)
			if price_gold > 0:
				self.AppendPrice(price_gold)
			if price_cheque > 0:
				self.AppendCheque(price_cheque)

			count = shop.GetOfflineShopItemCount(slotIndex)
			wyliczenie = (price_gold/count)
			wyliczenie2 = (price_cheque/count)

			if wyliczenie != 0 or wyliczenie2 != 0:
				if count > 1:
					self.AppendSpace(15)
					self.AppendTextLine(constInfo.price_color1+"P?edpokl?dan? cena za 1ks.")

				if not price_gold <= 0:
					if wyliczenie != 0:
						if count > 1:
							self.AppendSpace(5)
							self.AppendTextLine(localeInfo.NumberToGoldNotText(wyliczenie)+" {}".format(emoji.AppendEmoji("icon/emoji/money_icon.png")), self.GetPriceColor(wyliczenie))
							self.AppendSpace(2)

				if not price_cheque <= 0:
					if wyliczenie2 != 0:
						if count > 1:
							self.AppendSpace(5)
							self.AppendTextLine(localeInfo.NumberToGoldNotText(wyliczenie2)+" {}".format(emoji.AppendEmoji("icon/emoji/cheque_icon.png")), grp.GenerateColor(184./255, 184./255, 184./255, 1))
							self.AppendSpace(2)
			
	def SetInventoryItem(self, slotIndex, window_type = player.INVENTORY, showFunctions = False, bFromInventory = False):
		itemVnum = player.GetItemIndex(window_type, slotIndex)
		if 0 == itemVnum:
			return

		self.ClearToolTip()
		
		self.showFunctions = showFunctions
		if shop.IsOpen():
			if not shop.IsPrivateShop():
				item.SelectItem(itemVnum)
				self.AppendSellingPrice(player.GetISellItemPrice(window_type, slotIndex))

		metinSlot = [player.GetItemMetinSocket(window_type, slotIndex, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
		attrSlot = [player.GetItemAttribute(window_type, slotIndex, i) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]

		self.AddItemData(itemVnum, metinSlot, attrSlot, 0, 0, window_type, slotIndex)
		
		if showFunctions:
			self.AppendFunctionLines(itemVnum, window_type, slotIndex)
		
	def AppendFunctionLines(self, itemVnum, window_type, slotIndex):
		self.AppendTextLineInfo("|Eemoji/key_ctrl|e + |Eemoji/key_shift|e + |Eemoji/key_rclick|e",localeInfo.TOOLTIP_WIKI.replace("|Eemoji/key_ctrl|e + |Eemoji/key_shift|e + |Eemoji/key_rclick|e - ",""))
		self.AppendTextLineInfo("|Eemoji/key_ctrl|e + |Eemoji/key_shift|e + |Eemoji/key_lclick|e",localeInfo.TOOLTIP_SEARCH.replace("|Eemoji/key_ctrl|e + |Eemoji/key_shift|e + |Eemoji/key_lclick|e - ",""))
		

	def FormatMultiplePurchaseText(self):
		return localeInfo.MULTIPLE_PURCHASE.format(
			key_shift=emoji.AppendEmoji("icon/emoji/key_shift.png"),
			key_rclick=emoji.AppendEmoji("icon/emoji/key_rclick.png")
		)

	def SetShopItem(self, slotIndex):
		itemVnum = shop.GetItemID(slotIndex)
		if 0 == itemVnum:
			return

		price = shop.GetItemPrice(slotIndex)
		self.ClearToolTip()
		self.isShopItem = True

		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(shop.GetItemMetinSocket(slotIndex, i))
		attrSlot = []
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			attrSlot.append(shop.GetItemAttribute(slotIndex, i))

		if app.ENABLE_ITEMSHOP:
			item.SelectItem(itemVnum)
			for i in xrange(item.LIMIT_MAX_NUM):
				(limitType, limitValue) = item.GetLimit(i)
				if item.LIMIT_TIMER_BASED_ON_WEAR == limitType:
					metinSlot[2] = limitValue
				elif item.LIMIT_REAL_TIME == limitType or item.LIMIT_REAL_TIME_START_FIRST_USE == limitType:
					#metinSlot[2] = app.GetGlobalTimeStamp()+limitValue
					metinSlot[2] = limitValue

		self.AddItemData(itemVnum, metinSlot, attrSlot)
		if app.ENABLE_BUY_WITH_ITEM:
			self.AppendSpace(5)
			self.AppendTextLineBuyItem(localeInfo.ITEM_PRICE_TITLE, self.GetPriceColor(price))
			priced = False
			if price > 0:
				self.AppendPrice(price)
				priced = True

			for i in xrange(shop.MAX_SHOP_PRICES):
				(vnum, count) = shop.GetBuyWithItem(slotIndex, i)
				if vnum > 0 and count > 0:
					if not priced:
						priced = True
					
					self.AppendPrice_WithItem(vnum, count)
			
			if not priced:
				self.AppendTextLineBuyItem(localeInfo.ITEM_IS_FREE, self.GetPriceColor(price))
		else:
			self.AppendPrice(price)

		self.AppendSpace(5)
		self.AppendTextLine(self.FormatMultiplePurchaseText(), self.NORMAL_COLOR)

	def AppendGreenTextLine(self, text, color = POSITIVE_COLOR, centerAlign = True):
		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetFontName(self.defFontName)
		textLine.SetPackedFontColor(color)
		textLine.SetText(text)
		textLine.SetOutline()
		textLine.SetFeather(False)
		textLine.Show()

		if centerAlign:
			textLine.SetPosition(self.toolTipWidth/2, self.toolTipHeight)
			textLine.SetHorizontalAlignCenter()

		else:
			textLine.SetPosition(10, self.toolTipHeight)

		self.childrenList.append(textLine)

		self.toolTipHeight += self.TEXT_LINE_HEIGHT
		self.ResizeToolTip()

		return textLine

	def SetShopItemBySecondaryCoin(self, slotIndex):
		itemVnum = shop.GetItemID(slotIndex)
		if 0 == itemVnum:
			return

		price = shop.GetItemPrice(slotIndex)
		self.ClearToolTip()
		self.isShopItem = True

		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(shop.GetItemMetinSocket(slotIndex, i))
		attrSlot = []
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			attrSlot.append(shop.GetItemAttribute(slotIndex, i))

		self.AddItemData(itemVnum, metinSlot, attrSlot)
		self.AppendPriceBySecondaryCoin(price)

	if app.ENABLE_BUY_WITH_ITEM:
		def AppendTextLine_WithItem(self, text, img, color = FONT_COLOR, itemsize = 32):
			ayraC = text.split("|")
			if (len(ayraC) < 2):
				return

			priceText = ui.TextLine()
			priceText.SetParent(self)
			priceText.SetPackedFontColor(color)
			priceText.Show()
			priceText.SetText("%s" % ayraC[0])
			self.childrenList.append(priceText)
			
			tw, th = priceText.GetTextSize() 
			
			image = ui.ImageBox()
			image.SetParent(self)
			image.Show()
			image.LoadImage(img)
			self.childrenList.append(image)
			
			priceItemText = ui.TextLine()
			priceItemText.SetParent(self)
			priceItemText.SetPackedFontColor(color)
			priceItemText.Show()
			priceItemText.SetText(ayraC[1])
			self.childrenList.append(priceItemText)
			
			tiw, tih = priceItemText.GetTextSize() 
			
			topPos = self.toolTipHeight + 12
			leftPos = self.toolTipWidth/2 - (tw + tiw + 32) / 2
			priceText.SetPosition(leftPos, topPos)
			image.SetPosition(leftPos + tw, topPos - 12)
			priceItemText.SetPosition(leftPos + tw + 32, topPos)
			self.toolTipHeight += itemsize
			self.ResizeToolTip()

		def AppendPrice_WithItem(self, vnum, price):
			item.SelectItem(vnum)
			icon = item.GetIconImageFileName()
			(w, h) = item.GetItemSize()
			base_text = localeInfo.NumberToWithItemString(price, item.GetItemName())
			item_count = player.GetItemCountByVnum(vnum)
			base_color = grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0)  # Light gray color for base text

			self.AppendTextLine_WithItem(base_text + " (" + str(item_count) + ") ", icon, base_color, 32 * h)

	def SetExchangeOwnerItem(self, slotIndex):
		itemVnum = exchange.GetItemVnumFromSelf(slotIndex)
		if 0 == itemVnum:
			return

		self.ClearToolTip()

		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(exchange.GetItemMetinSocketFromSelf(slotIndex, i))
		attrSlot = []
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			attrSlot.append(exchange.GetItemAttributeFromSelf(slotIndex, i))
		self.AddItemData(itemVnum, metinSlot, attrSlot)

	def SetExchangeTargetItem(self, slotIndex):
		itemVnum = exchange.GetItemVnumFromTarget(slotIndex)
		if 0 == itemVnum:
			return

		self.ClearToolTip()

		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(exchange.GetItemMetinSocketFromTarget(slotIndex, i))
		attrSlot = []
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			attrSlot.append(exchange.GetItemAttributeFromTarget(slotIndex, i))
		self.AddItemData(itemVnum, metinSlot, attrSlot)

	def SetPrivateShopBuilderItem(self, invenType, invenPos, privateShopSlotIndex):
		itemVnum = player.GetItemIndex(invenType, invenPos)
		if 0 == itemVnum:
			return

		item.SelectItem(itemVnum)
		self.ClearToolTip()
		if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
			if app.ENABLE_CHEQUE_SYSTEM:
				goldPrice = privateShop.GetStockItemPrice(invenType, invenPos)
				chequePrice = privateShop.GetStockChequeItemPrice(invenType, invenPos)
				
				self.AppendSellingPrice(goldPrice, chequePrice, True)
			else:
				self.AppendSellingPrice(privateShop.GetStockItemPrice(invenType, invenPos))
		else:
			self.AppendSellingPrice(shop.GetPrivateShopItemPrice(invenType, invenPos))

		args = {}
		
		if app.ENABLE_PRIVATE_SHOP_REFINE_ELEMENT:
			args["refineElement"] = player.GetItemRefineElement(invenType, invenPos)

		apply_random_list = []
		if app.ENABLE_PRIVATE_SHOP_APPLY_RANDOM:
			for i in range(player.APPLY_RANDOM_SLOT_MAX_NUM):
				apply_random_list.append(player.GetItemApplyRandom(invenPos, i))
			args["apply_random_list"] = apply_random_list

		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(player.GetItemMetinSocket(invenPos, i))
		attrSlot = []
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			attrSlot.append(player.GetItemAttribute(invenPos, i))

		self.AddItemData(itemVnum, metinSlot, attrSlot, **args)

	def SetSafeBoxItem(self, slotIndex):
		itemVnum = safebox.GetItemID(slotIndex)
		if 0 == itemVnum:
			return

		self.ClearToolTip()
		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(safebox.GetItemMetinSocket(slotIndex, i))
		attrSlot = []
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			attrSlot.append(safebox.GetItemAttribute(slotIndex, i))

		self.AddItemData(itemVnum, metinSlot, attrSlot, safebox.GetItemFlags(slotIndex))

	def SetMallItem(self, slotIndex):
		itemVnum = safebox.GetMallItemID(slotIndex)
		if 0 == itemVnum:
			return

		self.ClearToolTip()
		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(safebox.GetMallItemMetinSocket(slotIndex, i))
		attrSlot = []
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			attrSlot.append(safebox.GetMallItemAttribute(slotIndex, i))
		
		if app.ENABLE_ITEMSHOP:
			item.SelectItem(itemVnum)
			for i in xrange(item.LIMIT_MAX_NUM):
				(limitType, limitValue) = item.GetLimit(i)
				if item.LIMIT_TIMER_BASED_ON_WEAR == limitType:
					metinSlot[0] = limitValue
				elif item.LIMIT_REAL_TIME == limitType or item.LIMIT_REAL_TIME_START_FIRST_USE == limitType:
					metinSlot[0] = app.GetGlobalTimeStamp()+limitValue

		self.AddItemData(itemVnum, metinSlot, attrSlot)

	if app.ENABLE_WIKI:
		def SetItemToolTipWiki(self, itemVnum):
			self.itemVnum = itemVnum
			item.SelectItem(itemVnum)
			metinSlot = [0 for i in xrange(player.METIN_SOCKET_MAX_NUM)]
			attrSlot = [(0,0) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]
			for i in xrange(item.LIMIT_MAX_NUM):
				(limitType, limitValue) = item.GetLimit(i)
				if item.LIMIT_TIMER_BASED_ON_WEAR == limitType:
					metinSlot[0] = limitValue
				elif item.LIMIT_REAL_TIME == limitType or item.LIMIT_REAL_TIME_START_FIRST_USE == limitType:
					metinSlot[0] = app.GetGlobalTimeStamp()+limitValue
			self.SetTitle(item.GetItemName())
			self.AppendDescription(item.GetItemDescription(), 26)
			self.AppendDescription(item.GetItemSummary(), 26, self.CONDITION_COLOR)
			self.__AppendAffectInformation()
			self.__AppendAttributeInformation(attrSlot)
			self.AppendWearableInformation()
			for i in xrange(item.LIMIT_MAX_NUM):
				(limitType, limitValue) = item.GetLimit(i)
				if item.LIMIT_REAL_TIME == limitType:
					if metinSlot[0] > 0:
						self.AppendMallItemLastTime(metinSlot[0])
					break
			self.ShowToolTip()

	def SetItemToolTip(self, itemVnum, showIcon=0):
		self.ClearToolTip()
		
		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(0)
		attrSlot = []
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			attrSlot.append((0, 0))

		if app.ENABLE_ITEMSHOP:
			item.SelectItem(itemVnum)
			for i in xrange(item.LIMIT_MAX_NUM):
				(limitType, limitValue) = item.GetLimit(i)
				if item.LIMIT_TIMER_BASED_ON_WEAR == limitType:
					metinSlot[0] = limitValue
				elif item.LIMIT_REAL_TIME_START_FIRST_USE == limitType:
					metinSlot[0] = limitValue
				elif item.LIMIT_REAL_TIME == limitType:
					metinSlot[0] = app.GetGlobalTimeStamp()+limitValue

		self.AddItemData(itemVnum, metinSlot, attrSlot, showIcon=showIcon)

	if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
		def GetBuffSkillLevelGrade(self, skillLevel):
			skillLevel = int(skillLevel)
			if skillLevel >= 0 and skillLevel < 20:
				return ("%d" % int(skillLevel))
			if skillLevel >= 20 and skillLevel < 30:
				return ("M%d" % int(skillLevel-19))
			if skillLevel >= 30 and skillLevel < 40: 
				return ("G%d" % int(skillLevel-29))
			if skillLevel == 40: 
				return "P"

		def AppendEXPGauge(self, exp_perc):
			IMG_PATH = "d:/ymir work/ui/aslan/buffnpc/"
			gauge_empty_list = []
			gauge_full_list = []
			x_pos = [35, 17, 1, 19]
			for i in xrange(4):
				gauge_empty = ui.ExpandedImageBox()
				gauge_empty.SetParent(self)
				gauge_empty.LoadImage(IMG_PATH + "exp_empty.sub")
				if i <= 1:
					gauge_empty.SetPosition(self.toolTipWidth/2 - x_pos[i], self.toolTipHeight)
				else:
					gauge_empty.SetPosition(self.toolTipWidth/2 + x_pos[i], self.toolTipHeight)
				gauge_empty.Show()

				gauge_full = ui.ExpandedImageBox()
				gauge_full.SetParent(self)
				gauge_full.LoadImage(IMG_PATH + "exp_full.sub")
				if i <= 1:
					gauge_full.SetPosition(self.toolTipWidth/2 - x_pos[i], self.toolTipHeight)
				else:
					gauge_full.SetPosition(self.toolTipWidth/2 + x_pos[i], self.toolTipHeight)
					
				gauge_empty_list.append(gauge_empty)
				gauge_full_list.append(gauge_full)
		
			exp_perc = float(exp_perc / 100.0)
			exp_bubble_perc = 25.0

			for i in xrange(4):
				if exp_perc > exp_bubble_perc:
					exp_bubble_perc += 25.0
					gauge_full_list[i].SetRenderingRect(0.0, 0.0, 0.0, 0.0)
					gauge_full_list[i].Show()
				else:
					exp_perc = float((exp_perc - exp_bubble_perc) * 4 / 100) 
					gauge_full_list[i].SetRenderingRect(0.0, exp_perc, 0.0, 0.0)
					gauge_full_list[i].Show()
					break

			self.childrenList.append(gauge_empty_list)
			self.childrenList.append(gauge_full_list)
			
			self.toolTipHeight += 18
			self.ResizeToolTip()

	def __AppendAttackSpeedInfo(self, item):
		atkSpd = item.GetValue(0)

		if atkSpd < 80:
			stSpd = localeInfo.TOOLTIP_ITEM_VERY_FAST
		elif atkSpd <= 95:
			stSpd = localeInfo.TOOLTIP_ITEM_FAST
		elif atkSpd <= 105:
			stSpd = localeInfo.TOOLTIP_ITEM_NORMAL
		elif atkSpd <= 120:
			stSpd = localeInfo.TOOLTIP_ITEM_SLOW
		else:
			stSpd = localeInfo.TOOLTIP_ITEM_VERY_SLOW

		self.AppendTextLine(localeInfo.TOOLTIP_ITEM_ATT_SPEED % stSpd, self.NORMAL_COLOR)

	def __AppendAttackGradeInfo(self):
		atkGrade = item.GetValue(1)
		self.AppendTextLine(localeInfo.TOOLTIP_ITEM_ATT_GRADE % atkGrade, self.GetChangeTextLineColor(atkGrade))

	if app.ENABLE_ACCE_COSTUME_SYSTEM:
		def CalcAcceValue(self, value, abs):
			if not value:
				return 0

			valueCalc 	= round(value * abs) / 100
			valueCalc 	-= 0.5
			valueCalc 	= int(valueCalc) +1 if valueCalc > 0 else int(valueCalc)
			value 		= 1 if (valueCalc <= 0 and value > 0) else valueCalc
			return value

	def __AppendAttackPowerInfo(self, itemAbsChance = 0):
		minPower = item.GetValue(3)
		maxPower = item.GetValue(4)
		addPower = item.GetValue(5)
		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			if itemAbsChance != 0:
				minPower = self.CalcAcceValue(minPower, itemAbsChance)
				maxPower = self.CalcAcceValue(maxPower, itemAbsChance)
				addPower = self.CalcAcceValue(addPower, itemAbsChance)

		if maxPower > minPower:
			self.AppendTextLine(localeInfo.TOOLTIP_ITEM_ATT_POWER % (minPower+addPower, maxPower+addPower), self.POSITIVE_COLOR)
		else:
			self.AppendTextLine(localeInfo.TOOLTIP_ITEM_ATT_POWER_ONE_ARG % (minPower+addPower), self.POSITIVE_COLOR)

	def __AppendMagicAttackInfo(self, itemAbsChance = 0):
		minMagicAttackPower = item.GetValue(1)
		maxMagicAttackPower = item.GetValue(2)
		addPower 			= item.GetValue(5)
		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			if itemAbsChance != 0:
				minMagicAttackPower = self.CalcAcceValue(minMagicAttackPower, itemAbsChance)
				maxMagicAttackPower = self.CalcAcceValue(maxMagicAttackPower, itemAbsChance)
				addPower 			= self.CalcAcceValue(addPower, itemAbsChance)

		if minMagicAttackPower > 0 or maxMagicAttackPower > 0:
			if maxMagicAttackPower > minMagicAttackPower:
				self.AppendTextLine(localeInfo.TOOLTIP_ITEM_MAGIC_ATT_POWER % (minMagicAttackPower+addPower, maxMagicAttackPower+addPower), self.POSITIVE_COLOR)
			else:
				self.AppendTextLine(localeInfo.TOOLTIP_ITEM_MAGIC_ATT_POWER_ONE_ARG % (minMagicAttackPower+addPower), self.POSITIVE_COLOR)

	def __AppendMagicDefenceInfo(self, itemAbsChance = 0):
		magicDefencePower = item.GetValue(0)
		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			if itemAbsChance != 0:
				magicDefencePower = self.CalcAcceValue(magicDefencePower, itemAbsChance)

		if magicDefencePower > 0:
			self.AppendTextLine(localeInfo.TOOLTIP_ITEM_MAGIC_DEF_POWER % magicDefencePower, self.GetChangeTextLineColor(magicDefencePower))

	def __AppendAttributeInformation(self, attrSlot, itemAbsChance = 0):
		if 0 != attrSlot:
			for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				type = attrSlot[i][0]
				value = attrSlot[i][1]

				if 0 == value:
					continue

				affectString = self.__GetAffectString(type, value)
				if app.ENABLE_ACCE_COSTUME_SYSTEM:
					if item.GetItemType() == item.ITEM_TYPE_COSTUME:
						if item.GetItemSubType() == item.COSTUME_TYPE_ACCE:
							if itemAbsChance != 0:
								value = self.CalcAcceValue(value, itemAbsChance)
								affectString = self.__GetAffectString(type, value)

				if app.ENABLE_AURA_SYSTEM:
					if item.GetItemType() == item.ITEM_TYPE_COSTUME and item.GetItemSubType() == item.COSTUME_TYPE_AURA and itemAbsChance:
						value = self.CalcSashValue(value, itemAbsChance)
						affectString = self.__GetAffectString(type, value)

				if affectString:
					affectColor = self.__GetAttributeColor(i, value, type)
					if item.GetItemType() == item.ITEM_TYPE_ARMOR and item.GetItemSubType() == item.ARMOR_GLOVE:
						if i > 2:
							self.AppendTextLine(affectString, affectColor)
						else:
							self.AppendTextLine(affectString, self.GetChangeTextLineColor(value))
					else:
						self.AppendTextLine(affectString, affectColor)

	def __GetAttributeColor(self, index, value, type=0):
		if item.GetItemType() == item.ITEM_TYPE_DS:
			return self.SPECIAL_POSITIVE_COLOR
		if value > 0:
			# Check for 6th and 7th bonus FIRST, before max value check
			if index >= player.ATTRIBUTE_SLOT_RARE_START and index <= player.ATTRIBUTE_SLOT_RARE_END:
				return self.SPECIAL_POSITIVE_COLOR2  # or whatever color you want
				
			if item.GetItemType() == item.ITEM_TYPE_ARMOR and item.GetItemSubType() == item.ARMOR_GLOVE:
				if index < 3:
					return self.POSITIVE_COLOR
				if value == self.MAX_AFFECT_VALUE_GLOVE[type]:
					return self.SPECIAL_TITLE_COLOR2
			elif item.GetItemType() == item.ITEM_TYPE_BELT:
				if value == self.MAX_AFFECT_VALUE_BELT[type]:
					return self.SPECIAL_TITLE_COLOR2
			else:
				if value == self.MAX_AFFECT_VALUE[type]:
					return self.SPECIAL_TITLE_COLOR2
	
			if index == player.ATTRIBUTE_SLOT_NORM_END:
				return self.SPECIAL_POSITIVE_COLOR5
			else:
				return self.SPECIAL_POSITIVE_COLOR
		elif value == 0:
			return self.NORMAL_COLOR
		else:
			return self.NEGATIVE_COLOR

	def __IsPolymorphItem(self, itemVnum):
		if itemVnum >= 70103 and itemVnum <= 70107:
			return 1
		return 0

	def __SetPolymorphItemTitle(self, monsterVnum):
		if localeInfo.IsVIETNAM():
			itemName =item.GetItemName()
			itemName+=" "
			itemName+=nonplayer.GetMonsterName(monsterVnum)
		else:
			itemName =nonplayer.GetMonsterName(monsterVnum)
			itemName+=" "
			itemName+=item.GetItemName()
		self.SetTitle(itemName)

	def __SetNormalItemTitle(self):
		self.SetTitle(item.GetItemName())

	def __SetSpecialItemTitle(self):
		self.AppendTextLine(item.GetItemName(), self.SPECIAL_TITLE_COLOR)

	def __SetItemTitle(self, itemVnum, metinSlot, attrSlot):
		if localeInfo.IsCANADA():
			if 72726 == itemVnum or 72730 == itemVnum:
				self.AppendTextLine(item.GetItemName(), grp.GenerateColor(1.0, 0.7843, 0.0, 1.0))
				return

		if self.__IsPolymorphItem(itemVnum):
			self.__SetPolymorphItemTitle(metinSlot[0])
		else:
			if self.__IsAttr(attrSlot):
				self.__SetSpecialItemTitle()
				return

			self.__SetNormalItemTitle()

	def __IsAttr(self, attrSlot):
		if not attrSlot:
			return False

		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			type = attrSlot[i][0]
			if 0 != type:
				return True

		return False

	def AddRefineItemData(self, itemVnum, metinSlot, attrSlot = 0):
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlotData=metinSlot[i]
			if self.GetMetinItemIndex(metinSlotData) == constInfo.ERROR_METIN_STONE:
				metinSlot[i]=player.METIN_SOCKET_TYPE_SILVER

		if app.ENABLE_GLOVE_SYSTEM:
			if itemVnum >= 23000 and itemVnum <= 23009:
				self.AddGloveItemData(itemVnum, metinSlot, attrSlot)
			else:
				self.AddItemData(itemVnum, metinSlot, attrSlot)
		else:
			self.AddItemData(itemVnum, metinSlot, attrSlot)


	if app.ENABLE_OFFLINE_SHOP:
		def AddRightClickForSale(self):
			self.AppendSpace(3)
			self.AppendTextLine(localeInfo.OFFLINESHOP_TOOLTIP_RIGHT_CLICK_FOR_SALE, centerAlign = False)
			self.AppendSpace(3)
			self.AppendTextLine(localeInfo.OFFLINESHOP_TOOLTIP_RIGHT_CLICK_FOR_SALE2, centerAlign = False)
			
	def AddItemData_Offline(self, itemVnum, itemDesc, itemSummary, metinSlot, attrSlot):
		self.__AdjustMaxWidth(attrSlot, itemDesc)
		self.__SetItemTitle(itemVnum, metinSlot, attrSlot)

		if self.__IsHair(itemVnum):
			self.__AppendHairIcon(itemVnum)

		if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
			if self.isPrivateSearchItem or self.isPrivateShopSaleItem:
				if not self.__IsHair(itemVnum):
					self.__AppendPrivateItemIcon(itemVnum)

		self.AppendDescription(itemDesc, 26)
		self.AppendDescription(itemSummary, 26, self.CONDITION_COLOR)

	def __SkillCostumeGetRace(self):
		if item.IsAntiFlag(item.ITEM_ANTIFLAG_ASSASSIN) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SURA) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SHAMAN):
			job = 0  # warrior
		elif item.IsAntiFlag(item.ITEM_ANTIFLAG_WARRIOR) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SURA) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SHAMAN):
			job = 1  # assassin
		elif item.IsAntiFlag(item.ITEM_ANTIFLAG_WARRIOR) and item.IsAntiFlag(item.ITEM_ANTIFLAG_ASSASSIN) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SHAMAN):
			job = 2  # sura
		elif item.IsAntiFlag(item.ITEM_ANTIFLAG_WARRIOR) and item.IsAntiFlag(item.ITEM_ANTIFLAG_ASSASSIN) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SURA):
			job = 3  # shaman
		else:
			job = chr.RaceToJob(player.GetOriginalRace())

		sex = chr.RaceToSex(player.GetOriginalRace())
		if sex == 0:  # female
			return job + 4
		return job

	def __ItemGetRace(self):
		race = 0

		if item.IsAntiFlag(item.ITEM_ANTIFLAG_ASSASSIN) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SURA) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SHAMAN):
			race = 9
		elif item.IsAntiFlag(item.ITEM_ANTIFLAG_WARRIOR) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SURA) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SHAMAN):
			race = 1
		elif item.IsAntiFlag(item.ITEM_ANTIFLAG_WARRIOR) and item.IsAntiFlag(item.ITEM_ANTIFLAG_ASSASSIN) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SHAMAN):
			race = 2
		elif item.IsAntiFlag(item.ITEM_ANTIFLAG_WARRIOR) and item.IsAntiFlag(item.ITEM_ANTIFLAG_ASSASSIN) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SURA):
			race = 3

		sex = chr.RaceToSex(player.GetOriginalRace())
		MALE = 1
		FEMALE = 0

		if item.IsAntiFlag(item.ITEM_ANTIFLAG_MALE) and sex == MALE:
			race = player.GetOriginalRace() + 4

		if item.IsAntiFlag(item.ITEM_ANTIFLAG_FEMALE) and sex == FEMALE:
			race = player.GetOriginalRace()

		if race == 0:
			race = player.GetOriginalRace()

		if race == 9:
			race = 0

		return race

	def __ItemGetRaceShaman(self):
		if item.IsAntiFlag(item.ITEM_ANTIFLAG_SHAMAN):
			return 3

		sex = chr.RaceToSex(player.GetOriginalRace())
		MALE = 1
		FEMALE = 0

		if item.IsAntiFlag(item.ITEM_ANTIFLAG_MALE) and sex == MALE:
			race = player.GetOriginalRace() + 4

		if item.IsAntiFlag(item.ITEM_ANTIFLAG_FEMALE) and sex == FEMALE:
			race = player.GetOriginalRace()
    
		return 3

	def AddItemData(self, itemVnum, metinSlot, attrSlot = 0, flags = 0, unbindTime = 0, window_type = player.INVENTORY, slotIndex = -1, showIcon = 0):
		self.itemVnum = itemVnum
		self.metinSlot = metinSlot

		item.SelectItem(itemVnum)
		itemType = item.GetItemType()
		itemSubType = item.GetItemSubType()
		self.__ModelPreviewClose()
		
		if app.ENABLE_SKILL_COSTUME_SYSTEM:
			if item.ITEM_TYPE_SKILL_COSTUME == itemType:
				self.__ModelPreviewOpen(itemVnum, self.__SkillCostumeGetRace(), "skill")

		if not item.GetItemDescription():
			self.__CalculateToolTipWidth()

		if 50026 == itemVnum:
			if 0 != metinSlot:
				name = item.GetItemName()
				if metinSlot[0] > 0:
					name += " "
					name += localeInfo.NumberToMoneyString(metinSlot[0])
			self.SetTitle(name)
			self.__AppendSealInformation(window_type, slotIndex)
			self.AdditionalTips(window_type, itemVnum, metinSlot, slotIndex)
			self.ShowToolTip()
			return

		elif 50300 == itemVnum:
			if 0 != metinSlot:
				self.__SetSkillBookToolTip(metinSlot[0], localeInfo.TOOLTIP_SKILLBOOK_NAME, 1)
			self.__AppendSealInformation(window_type, slotIndex)
			self.AdditionalTips(window_type, itemVnum, metinSlot, slotIndex)
			self.ShowToolTip()
			return
		elif 70037 == itemVnum:
			if 0 != metinSlot:
				self.__SetSkillBookToolTip(metinSlot[0], localeInfo.TOOLTIP_SKILL_FORGET_BOOK_NAME, 0)
			self.AppendDescription(item.GetItemDescription(), 26)
			self.AppendDescription(item.GetItemSummary(), 26, self.CONDITION_COLOR)
			self.__AppendSealInformation(window_type, slotIndex)
			self.AdditionalTips(window_type, itemVnum, metinSlot, slotIndex)
			self.ShowToolTip()
			return
		elif 70055 == itemVnum:
			if 0 != metinSlot:
				self.__SetSkillBookToolTip(metinSlot[0], localeInfo.TOOLTIP_SKILL_FORGET_BOOK_NAME, 0)
			self.AppendDescription(item.GetItemDescription(), 26)
			self.AppendDescription(item.GetItemSummary(), 26, self.CONDITION_COLOR)
			self.__AppendSealInformation(window_type, slotIndex)
			self.AdditionalTips(window_type, itemVnum, metinSlot, slotIndex)
			self.ShowToolTip()
			return




		itemDesc = item.GetItemDescription()
		itemSummary = item.GetItemSummary()

		isCostumeItem = 0
		isCostumeHair = 0
		isCostumeBody = 0
		isCostumeStole = 0
		isSkillCostumeItem = 0

				
					
					
			
		if app.ENABLE_MOUNT_COSTUME_SYSTEM:
			isCostumeMount = 0
		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			isCostumeAcce = 0
		if app.ENABLE_WEAPON_COSTUME_SYSTEM:
			isCostumeWeapon = 0
		if app.ENABLE_AURA_SYSTEM:
			isCostumeAura = 0

		if app.ENABLE_SKILL_COSTUME_SYSTEM:
			if item.ITEM_TYPE_SKILL_COSTUME == itemType:
				isSkillCostumeItem = 1

		if app.ENABLE_COSTUME_SYSTEM:
			if item.ITEM_TYPE_COSTUME == itemType:
				isCostumeItem = 1
				isCostumeHair = item.COSTUME_TYPE_HAIR == itemSubType
				isCostumeBody = item.COSTUME_TYPE_BODY == itemSubType
				isCostumeStole = item.COSTUME_TYPE_STOLE == itemSubType
				if app.ENABLE_MOUNT_COSTUME_SYSTEM:
					isCostumeMount = item.COSTUME_TYPE_MOUNT == itemSubType
				if app.ENABLE_ACCE_COSTUME_SYSTEM:
					isCostumeAcce = item.COSTUME_TYPE_ACCE == itemSubType
				if app.ENABLE_WEAPON_COSTUME_SYSTEM:
					isCostumeWeapon = item.COSTUME_TYPE_WEAPON == itemSubType
				if app.ENABLE_AURA_SYSTEM:
					isCostumeAura = itemSubType == item.COSTUME_TYPE_AURA

		if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
			isBuffCostume = (item.ITEM_TYPE_COSTUME == itemType and
				itemSubType in (item.COSTUME_TYPE_BUFF_BODY, item.COSTUME_TYPE_BUFF_HAIR, item.COSTUME_TYPE_BUFF_WEAPON))


		self.__AdjustMaxWidth(attrSlot, itemDesc)
		self.__SetItemTitle(itemVnum, metinSlot, attrSlot)
		
		

		if showIcon == 1:
			self.AppendSpace(3)
			self.AppendItemIcon(itemVnum)
			self.AppendSpace(3)

		self.AppendDescription(itemDesc, 26)
		self.AppendDescription(itemSummary, 26, self.CONDITION_COLOR)

		if item.ITEM_TYPE_WEAPON == itemType:

			self.__AppendLimitInformation()

			self.AppendSpace(5)

			if item.WEAPON_FAN == itemSubType:
				self.__AppendMagicAttackInfo()
				self.__AppendAttackPowerInfo()

			else:
				self.__AppendAttackPowerInfo()
				self.__AppendMagicAttackInfo()

			self.__AppendAffectInformation()
			self.__AppendAttributeInformation(attrSlot)

			self.AppendWearableInformation()

			if app.ENABLE_QUIVER_SYSTEM:
				if itemSubType != item.WEAPON_QUIVER:
					self.__AppendMetinSlotInfo(metinSlot)
				elif item.WEAPON_QUIVER == itemSubType:
					bHasRealtimeFlag = 0
					defaultValue = 0
					for i in xrange(item.LIMIT_MAX_NUM):
						(limitType, defaultValue) = item.GetLimit(i)
						if item.LIMIT_REAL_TIME == limitType:
							bHasRealtimeFlag = 1
							break

					if bHasRealtimeFlag == 1:
						self.AppendMallItemLastTime(defaultValue if self.isShopItem else metinSlot[0])
			else:
				self.__AppendMetinSlotInfo(metinSlot)
				
		elif itemVnum in (80005, 80006, 80007, 80008, 80009):
			self.AppendSpace(5)
			self.AppendTextLine("-" * 30, self.DISABLE_COLOR)
			self.AppendSpace(3)
		
			GOLD_BAR_VALUES = {
				80005: 500000,
				80006: 1000000,
				80007: 2000000,
				80008: 10000000,
				80009: 5000000,
			}
		
			bar_value = GOLD_BAR_VALUES.get(itemVnum, 0)
			count = player.GetItemCountByVnum(itemVnum)
			receive = bar_value * (count - 1) if count > 1 else 0
		
			self.AppendSpace(2)
			self.AppendTextLine(localeInfo.NumberToMoneyString(bar_value) + localeInfo.GOLDBAR_DESC_PER, self.NORMAL_COLOR)
		
			if count > 1:
				self.AppendTextLine(localeInfo.NumberToMoneyString(receive) + " (" + str(count - 1) + "x)", self.NORMAL_COLOR)
		
			self.AppendSpace(3)
			self.AppendTextLine("-" * 30, self.DISABLE_COLOR)
			self.AppendSpace(3)
			self.AppendTextLine(localeInfo.GOLDBAR_DESC_WARNING, self.DISABLE_COLOR)
			self.AppendSpace(3)
			
		elif app.ENABLE_VIP_SYSTEM and item.ITEM_TYPE_VIP == itemType:
			title = item.GetItemName() + " (" + localeInfo.SecondToDHM(metinSlot[0]) + ")"
			self.SetTitle(title)
			self.AppendSpace(5)
			self.AppendTextLine("-" * 30, self.DISABLE_COLOR)
			self.AppendSpace(3)
		
			# General
			self.AppendTextLine(localeInfo.VIP_SECTION_BENEFITS, self.POSITIVE_COLOR)
			self.AppendSpace(2)
			self.AppendTextLine(localeInfo.VIP_METIN_QUEUE, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.VIP_AUTO_PICKUP, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.VIP_EXP_BONUS, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.VIP_TITLE, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.VIP_SAVE_LOCATION, self.NORMAL_COLOR)
			self.AppendSpace(5)
			self.AppendTextLine("-" * 30, self.DISABLE_COLOR)
			self.AppendSpace(3)
		
			# Mining
			self.AutoAppendTextLine(emoji.AppendEmoji("icon/emoji/mining.tga") + " " + localeInfo.VIP_MINING_HEADER, self.POSITIVE_COLOR)
			self.AppendSpace(2)
			self.AppendTextLine(localeInfo.VIP_MINING_1, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.VIP_MINING_2, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.VIP_MINING_3, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.VIP_MINING_4, self.NORMAL_COLOR)
			self.AppendSpace(5)
			self.AppendTextLine("-" * 30, self.DISABLE_COLOR)
			self.AppendSpace(3)
		
			# Fishing
			self.AutoAppendTextLine(emoji.AppendEmoji("icon/emoji/fish.tga") + " " + localeInfo.VIP_FISHING_HEADER, self.POSITIVE_COLOR)
			self.AppendSpace(2)
			self.AppendTextLine(localeInfo.VIP_FISHING_1, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.VIP_FISHING_2, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.VIP_FISHING_3, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.VIP_FISHING_4, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.VIP_FISHING_5, self.NORMAL_COLOR)
			self.AppendSpace(5)
			self.AppendTextLine("-" * 30, self.DISABLE_COLOR)
			self.AppendSpace(3)
			
			# Warning
			self.AutoAppendTextLine(emoji.AppendEmoji("icon/emoji/info.tga") + " " + localeInfo.VIP_WARNING_HEADER, self.POSITIVE_COLOR)
			self.AppendSpace(2)
			self.AppendTextLine(localeInfo.VIP_TEXT_WARNING_1, self.POSITIVE_COLOR)
			self.AppendTextLine(localeInfo.VIP_TEXT_WARNING_2, self.POSITIVE_COLOR)
			self.AppendTextLine(localeInfo.VIP_TEXT_WARNING_3, self.POSITIVE_COLOR)

		elif item.ITEM_TYPE_ARMOR == itemType:
			self.__AppendLimitInformation()

			defGrade = item.GetValue(1)
			defBonus = item.GetValue(5)*2
			if defGrade > 0:
				self.AppendSpace(5)
				self.AppendTextLine(localeInfo.TOOLTIP_ITEM_DEF_GRADE % (defGrade+defBonus), self.GetChangeTextLineColor(defGrade))

			self.__AppendMagicDefenceInfo()
			self.__AppendAffectInformation()	
			self.__AppendAttributeInformation(attrSlot)

			if item.GetItemSubType() == item.ARMOR_GLOVE:
				reinforcement = int(metinSlot[0])
				if reinforcement > 0:
					self.AppendSpace(15)
					self.AutoAppendTextLine(emoji.AppendEmoji("icon/emoji/34000.png"))
					self.AppendSpace(4)
					self.AutoAppendTextLine(localeInfo.TOOLTIP_ENCHANTED)
					self.AppendSpace(3)

			self.AppendWearableInformation()

			if itemSubType in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
				self.__AppendAccessoryMetinSlotInfo(metinSlot, constInfo.GET_ACCESSORY_MATERIAL_VNUM(itemVnum, itemSubType))
			else:
				self.__AppendMetinSlotInfo(metinSlot)

		elif item.ITEM_TYPE_TITLE == itemType:
			titleid = item.GetValue(0)
			self.AppendSpace(5)
	
		elif app.ENABLE_NEW_PET_SYSTEM and itemType in (item.ITEM_NEW_PET, item.ITEM_NEW_PET_EQ):
			if item.GetValue(0) != 0:
				if itemVnum != 80109 and itemVnum != 80119 and itemVnum != 80129:
					self.__ModelPreviewOpen(itemVnum, item.GetValue(0), "monster")
			self.AppendSpace(5)
			self.__AppendLimitInformation()
			self.__AppendAttributeInformation(attrSlot)

			bonusLeftTime = metinSlot[1] - app.GetGlobalTimeStamp()
			if bonusLeftTime > 0:
				self.itemVnum = metinSlot[2]
				item.SelectItem(metinSlot[2])
				self.__AppendAffectInformation()
				self.itemVnum = itemVnum
				item.SelectItem(itemVnum)
				self.AppendCostumeBonuses(bonusLeftTime)

			if app.ENABLE_EXTRABONUS_SYSTEM:
				reinforcement = int(metinSlot[0])
				if reinforcement > 0:
					if itemVnum == 80109 or itemVnum == 80119 or itemVnum == 80129:
						self.__AppendExtraAffectInformation(1)
				else:
					if itemVnum != 80109 or itemVnum != 80119 or itemVnum != 80129:
						self.__AppendAffectInformation()
						
				if itemVnum == 80109 or itemVnum == 80119 or itemVnum == 80129:
					if reinforcement > 0:
						self.AppendSpace(15)
						self.AutoAppendTextLine(emoji.AppendEmoji("icon/emoji/34000.png"))
						self.AppendSpace(4)
						self.AutoAppendTextLine(localeInfo.TOOLTIP_ENCHANTED)
						self.AppendSpace(3)
			else:
				self.__AppendAffectInformation()

		elif item.ITEM_TYPE_MOUNT==itemType:
			self.AppendSpace(4)
			self.__AppendAffectInformation()
			self.AppendSpace(1)
			self.__AppendAttributeInformation(attrSlot)
			self.AppendSpace(2)
			self.__AppendLimitInformation()

		elif item.ITEM_TYPE_TOGGLE == itemType:
			time = metinSlot[0]
			self.__AppendLimitInformation()
			self.__AppendAffectInformation()
			self.__AppendAttributeInformation(attrSlot)
			self.AppendMallItemLastTime(time)

		elif item.ITEM_TYPE_RING == itemType:
			time = metinSlot[0]
			self.__AppendLimitInformation()
			self.__AppendAffectInformation()
			self.__AppendAttributeInformation(attrSlot)
			self.AppendMallItemLastTime(time)



		elif item.ITEM_TYPE_BELT == itemType:
			self.__AppendLimitInformation()
			self.__AppendAffectInformation()
			self.__AppendAttributeInformation(attrSlot)

			self.__AppendAccessoryMetinSlotInfo(metinSlot, constInfo.GET_BELT_MATERIAL_VNUM(itemVnum))
			
		elif item.ITEM_TYPE_ARTEFAKT == itemType:
			self.__AppendLimitInformation()
			self.__AppendAffectInformation()
			self.__AppendAttributeInformation(attrSlot)

			self.__AppendAccessoryMetinSlotInfo(metinSlot, constInfo.GET_BELT_MATERIAL_VNUM(itemVnum))
			

		elif item.ITEM_TYPE_RUNE == itemType or item.ITEM_TYPE_RUNE_RED == itemType or item.ITEM_TYPE_RUNE_BLUE == itemType or item.ITEM_TYPE_RUNE_BLACK == itemType or item.ITEM_TYPE_RUNE_YELLOW == itemType or item.ITEM_TYPE_RUNE_GREEN == itemType:
			self.__AppendLimitInformation()
			if app.ENABLE_EXTRABONUS_SYSTEM:
				reinforcement = int(metinSlot[0])
				if reinforcement > 0:
					self.__AppendExtraAffectInformation(1)
				else:
					self.__AppendAffectInformation()
			else:
				self.__AppendAffectInformation()
			self.__AppendAttributeInformation(attrSlot)
			if app.ENABLE_EXTRABONUS_SYSTEM:
				if reinforcement > 0:
					self.AppendSpace(15)
					self.AutoAppendTextLine(emoji.AppendEmoji("icon/emoji/34000.png"))
					self.AppendSpace(4)
					self.AutoAppendTextLine(localeInfo.TOOLTIP_ENCHANTED)
					self.AppendSpace(3)

		elif item.ITEM_TYPE_MOUNT_EQUIPMENT == itemType:
			self.__AppendLimitInformation()
			self.__AppendAffectInformation()
			self.__AppendAttributeInformation(attrSlot)

		elif app.ENABLE_PET_SYSTEM_EX and item.ITEM_TYPE_PET == itemType:
			self.__AppendLimitInformation()
			self.__AppendAffectInformation()
			self.__AppendAttributeInformation(attrSlot)
			self.AppendWearableInformation()
			self.AppendLastTimeInformation(metinSlot)

		elif item.ITEM_TYPE_COSTUME == itemType and item.GetItemSubType() == item.COSTUME_TYPE_MOUNT:
			self.__AppendLimitInformation()
			if item.GetValue(1) != 0:
				self.__ModelPreviewOpen(itemVnum, item.GetValue(1), "monster")

		if itemVnum in zestaw_data:
			race = player.GetOriginalRace()
			sex = chr.RaceToSex(race)
			if sex == 1:
				data = zestaw_data[itemVnum]["male"]
			else:
				data = zestaw_data[itemVnum]["female"]
			
			weaponVnum = self.__GetWeaponFromData(data)
			
			self.__IsBoxShowModel2(
				hairVnum=data["hairVnum"], 
				bodyVnum=data["bodyVnum"], 
				weaponVnum=weaponVnum,
				acceVnum=data["acceVnum"], 
				model=self.__ItemGetRace()
			)
			if 50166 != itemVnum:
				self.AutoAppendTextLine(localeInfo.COSTUME_HAIR)
				self.AutoAppendTextLine(localeInfo.COSTUME_BODY)
				self.AutoAppendTextLine(localeInfo.COSTUME_SASH)
				self.AutoAppendTextLine(localeInfo.COSTUME_WEAPON)
				self.AutoAppendTextLine(localeInfo.COSTUME_PICK_POSSIBILITY)

		elif 90051 == itemVnum:

			self.__IsBoxShowModel2(hairVnum=99903, bodyVnum=99902, weaponVnum=99901, acceVnum=99908, model=self.__ItemGetRaceShaman())
			self.AutoAppendTextLine(localeInfo.COSTUME_HAIR)
			self.AutoAppendTextLine(localeInfo.COSTUME_BODY)
			self.AutoAppendTextLine(localeInfo.COSTUME_WEAPON)

		elif 90041 == itemVnum:

			self.__IsBoxShowModel2(hairVnum=99906, bodyVnum=99905, weaponVnum=99904, acceVnum=99907, model=self.__ItemGetRaceShaman())
			self.AutoAppendTextLine(localeInfo.COSTUME_HAIR)
			self.AutoAppendTextLine(localeInfo.COSTUME_BODY)
			self.AutoAppendTextLine(localeInfo.COSTUME_WEAPON)

		elif 90071 == itemVnum:

			self.__IsBoxShowModel2(hairVnum=99909, bodyVnum=99908, weaponVnum=99907, acceVnum=99907, model=self.__ItemGetRaceShaman())
			self.AutoAppendTextLine(localeInfo.COSTUME_HAIR)
			self.AutoAppendTextLine(localeInfo.COSTUME_BODY)
			self.AutoAppendTextLine(localeInfo.COSTUME_WEAPON)

		elif 90073 == itemVnum:

			self.__IsBoxShowModel2(hairVnum=99912, bodyVnum=99911, weaponVnum=99910, acceVnum=99910, model=self.__ItemGetRaceShaman())
			self.AutoAppendTextLine(localeInfo.COSTUME_HAIR)
			self.AutoAppendTextLine(localeInfo.COSTUME_BODY)
			self.AutoAppendTextLine(localeInfo.COSTUME_WEAPON)

		elif 90074 == itemVnum:

			self.__IsBoxShowModel2(hairVnum=99915, bodyVnum=99914, weaponVnum=99913, acceVnum=99913, model=self.__ItemGetRaceShaman())
			self.AutoAppendTextLine(localeInfo.COSTUME_HAIR)
			self.AutoAppendTextLine(localeInfo.COSTUME_BODY)
			self.AutoAppendTextLine(localeInfo.COSTUME_WEAPON)

		elif 90075 == itemVnum:

			self.__IsBoxShowModel2(hairVnum=99924, bodyVnum=99923, weaponVnum=99922, acceVnum=99924, model=self.__ItemGetRaceShaman())
			self.AutoAppendTextLine(localeInfo.COSTUME_HAIR)
			self.AutoAppendTextLine(localeInfo.COSTUME_BODY)
			self.AutoAppendTextLine(localeInfo.COSTUME_WEAPON)

		elif 90076 == itemVnum:

			self.__IsBoxShowModel2(hairVnum=99927, bodyVnum=99926, weaponVnum=99925, acceVnum=99927, model=self.__ItemGetRaceShaman())
			self.AutoAppendTextLine(localeInfo.COSTUME_HAIR)
			self.AutoAppendTextLine(localeInfo.COSTUME_BODY)
			self.AutoAppendTextLine(localeInfo.COSTUME_WEAPON)

		elif 90077 == itemVnum:

			self.__IsBoxShowModel2(hairVnum=99930, bodyVnum=99929, weaponVnum=99928, acceVnum=99930, model=self.__ItemGetRaceShaman())
			self.AutoAppendTextLine(localeInfo.COSTUME_HAIR)
			self.AutoAppendTextLine(localeInfo.COSTUME_BODY)
			self.AutoAppendTextLine(localeInfo.COSTUME_WEAPON)

		elif 99901 == itemVnum:
			self.__IsBoxShowModel2(hairVnum=0, bodyVnum=0, weaponVnum=99901, acceVnum=0, model=self.__ItemGetRaceShaman())
			self.AutoAppendTextLine(localeInfo.COSTUME_WEAPON_FOR_BUFF)

		elif 99902 == itemVnum:
			self.__IsBoxShowModel2(hairVnum=0, bodyVnum=99902, weaponVnum=0, acceVnum=0, model=self.__ItemGetRaceShaman())
			self.AutoAppendTextLine(localeInfo.COSTUME_BODY_FOR_BUFF)
		elif 99903 == itemVnum:
			self.__IsBoxShowModel2(hairVnum=99903, bodyVnum=0, weaponVnum=0, acceVnum=0, model=self.__ItemGetRaceShaman())
			self.AutoAppendTextLine(localeInfo.COSTUME_HAIR_FOR_BUFF)
			
		elif 91207 == itemVnum:
			self.AppendSpace(5)
			self.AppendTextLine("-" * 30, self.DISABLE_COLOR)
			self.AppendSpace(3)
		
			# General
			self.AppendTextLine(localeInfo.STARTER_PACKAGE_CONTAINS, self.POSITIVE_COLOR)
			self.AppendSpace(2)
			self.AppendTextLine(localeInfo.STARTER_PACKAGE_SMALL_1, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.STARTER_PACKAGE_SMALL_2, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.STARTER_PACKAGE_SMALL_3, self.NORMAL_COLOR)
			self.AppendSpace(5)
			self.AppendTextLine("-" * 30, self.DISABLE_COLOR)
			self.AppendSpace(3)
			
		elif 91208 == itemVnum:
			self.AppendSpace(5)
			self.AppendTextLine("-" * 30, self.DISABLE_COLOR)
			self.AppendSpace(3)
		
			# General
			self.AppendTextLine(localeInfo.STARTER_PACKAGE_CONTAINS, self.POSITIVE_COLOR)
			self.AppendSpace(2)
			self.AppendTextLine(localeInfo.BATTLEPASS_PACKAGE_1, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.BATTLEPASS_PACKAGE_2, self.NORMAL_COLOR)
			self.AppendSpace(5)
			self.AppendTextLine("-" * 30, self.DISABLE_COLOR)
			self.AppendSpace(3)
			
		elif 91209 == itemVnum:
			self.AppendSpace(5)
			self.AppendTextLine("-" * 30, self.DISABLE_COLOR)
			self.AppendSpace(3)
		
			# General
			self.AppendTextLine(localeInfo.STARTER_PACKAGE_CONTAINS, self.POSITIVE_COLOR)
			self.AppendSpace(2)
			self.AppendTextLine(localeInfo.STARTER_PACKAGE_MEDIUM_1, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.STARTER_PACKAGE_MEDIUM_2, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.STARTER_PACKAGE_MEDIUM_3, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.STARTER_PACKAGE_MEDIUM_4, self.NORMAL_COLOR)
			self.AppendSpace(5)
			self.AppendTextLine("-" * 30, self.DISABLE_COLOR)
			self.AppendSpace(3)
			
		elif 91210 == itemVnum:
			self.AppendSpace(5)
			self.AppendTextLine("-" * 30, self.DISABLE_COLOR)
			self.AppendSpace(3)
		
			# General
			self.AppendTextLine(localeInfo.STARTER_PACKAGE_CONTAINS, self.POSITIVE_COLOR)
			self.AppendSpace(2)
			self.AppendTextLine(localeInfo.STARTER_PACKAGE_BIG_1, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.STARTER_PACKAGE_BIG_2, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.STARTER_PACKAGE_BIG_3, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.STARTER_PACKAGE_BIG_4, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.STARTER_PACKAGE_BIG_5, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.STARTER_PACKAGE_BIG_6, self.NORMAL_COLOR)
			self.AppendSpace(5)
			self.AppendTextLine("-" * 30, self.DISABLE_COLOR)
			self.AppendSpace(3)
			
		elif 91211 == itemVnum:
			self.AppendSpace(5)
			self.AppendTextLine("-" * 30, self.DISABLE_COLOR)
			self.AppendSpace(3)
		
			# General
			self.AppendTextLine(localeInfo.STARTER_PACKAGE_CONTAINS, self.POSITIVE_COLOR)
			self.AppendSpace(2)
			self.AppendTextLine(localeInfo.COSTUME_BOOST_PACKAGE_1, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.COSTUME_BOOST_PACKAGE_2, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.COSTUME_BOOST_PACKAGE_3, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.COSTUME_BOOST_PACKAGE_4, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.COSTUME_BOOST_PACKAGE_5, self.NORMAL_COLOR)
			self.AppendSpace(5)
			self.AppendTextLine("-" * 30, self.DISABLE_COLOR)
			self.AppendSpace(3)
			
		elif 34000 == itemVnum:
			self.AppendSpace(5)
			self.AppendTextLine(localeInfo.EXTRA_BONUS_ITEM_TITLE, self.TITLE_COLOR)
			self.AppendSpace(5)
			
			# Description
			self.AppendTextLine(localeInfo.EXTRA_BONUS_ITEM_DESC, self.NORMAL_COLOR)
			self.AppendSpace(5)
			
			# Separator
			self.AppendTextLine("-" * 30, self.DISABLE_COLOR)
			self.AppendSpace(3)
			
			# Compatible items header
			self.AppendTextLine(localeInfo.EXTRA_BONUS_COMPATIBLE_ITEMS, self.POSITIVE_COLOR)
			self.AppendSpace(3)
			
			# Sash (Socket bonus increase)
			self.AutoAppendTextLine(emoji.AppendEmoji("icon/emoji/sash.tga") + " " + localeInfo.EXTRA_BONUS_SASH_INFO, self.NORMAL_COLOR)
			self.AppendTextLine("  " + localeInfo.EXTRA_BONUS_SASH_EFFECT, self.POSITIVE_COLOR)
			self.AppendSpace(3)
			
			# Gloves (Attribute bonus increase)
			self.AutoAppendTextLine(emoji.AppendEmoji("icon/emoji/gloves.tga") + " " + localeInfo.EXTRA_BONUS_GLOVES_INFO, self.NORMAL_COLOR)
			self.AppendTextLine("  " + localeInfo.EXTRA_BONUS_GLOVES_EFFECT, self.POSITIVE_COLOR)
			self.AppendSpace(3)
			
			# Pet Equipment (Unlock bonus)
			self.AutoAppendTextLine(emoji.AppendEmoji("icon/emoji/pet.tga") + " " + localeInfo.EXTRA_BONUS_PET_INFO, self.NORMAL_COLOR)
			self.AppendTextLine("  " + localeInfo.EXTRA_BONUS_PET_EFFECT, self.POSITIVE_COLOR)
			self.AppendSpace(3)
			
			# Runes (Unlock bonus)
			self.AutoAppendTextLine(emoji.AppendEmoji("icon/emoji/rune.tga") + " " + localeInfo.EXTRA_BONUS_RUNE_INFO, self.NORMAL_COLOR)
			self.AppendTextLine("  " + localeInfo.EXTRA_BONUS_RUNE_EFFECT, self.POSITIVE_COLOR)
			self.AppendSpace(5)
			
			# Separator
			self.AppendTextLine("-" * 30, self.DISABLE_COLOR)
			self.AppendSpace(3)
			
			# Requirements header
			self.AppendTextLine(localeInfo.EXTRA_BONUS_REQUIREMENTS, self.NEGATIVE_COLOR)
			self.AppendSpace(2)
			
			# Requirements list
			self.AppendTextLine(localeInfo.EXTRA_BONUS_REQ_SASH, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.EXTRA_BONUS_REQ_GLOVES, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.EXTRA_BONUS_REQ_PET, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.EXTRA_BONUS_REQ_RUNE, self.NORMAL_COLOR)
			self.AppendSpace(5)
			
			# Warning
			self.AppendTextLine(localeInfo.EXTRA_BONUS_WARNING, self.NEGATIVE_COLOR)

		elif isSkillCostumeItem:
			self.__AppendLimitInformation()
			self.__AppendAttributeInformation(attrSlot)
			self.AppendWearableInformation()

		elif isCostumeItem:
			self.__AppendLimitInformation()
			self.__AppendAffectInformation()


			bonusLeftTime = metinSlot[1] - app.GetGlobalTimeStamp()
			if bonusLeftTime > 0:
				self.itemVnum = metinSlot[2]
				item.SelectItem(metinSlot[2])
				self.__AppendAffectInformation()
				self.itemVnum = itemVnum
				item.SelectItem(itemVnum)
				self.AppendCostumeBonuses(bonusLeftTime)

			self.__AppendAttributeInformation(attrSlot)

			if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM and isBuffCostume:
				buffInt = int(metinSlot[0]) if metinSlot[0] else 0
				if buffInt > 0:
					self.AppendSpace(5)
					self.AppendTextLine(localeInfo.BUFF_INTELLIGENCE.format(buffInt), self.CONDITION_COLOR)

			if isCostumeWeapon:
				self.__ModelPreviewOpen(itemVnum, self.__ItemGetRace(), "weapon")

			if isCostumeHair:
				self.__ModelPreviewOpen(item.GetValue(3), self.__ItemGetRace(), "hair")

			if isCostumeStole:
				self.__ModelPreviewOpen(itemVnum, self.__ItemGetRace(), "acce")

			if isCostumeBody:
				self.__ModelPreviewOpen(itemVnum, self.__ItemGetRace(), "body")

			if app.ENABLE_ACCE_COSTUME_SYSTEM and isCostumeAcce:
				absChance = int(metinSlot[acce.ABSORPTION_SOCKET])
				reinforcement = int(metinSlot[2])
				if reinforcement > 0:
						self.AppendSpace(15)
						self.AutoAppendTextLine(emoji.AppendEmoji("icon/emoji/34000.png"))
						self.AppendSpace(4)
						self.AutoAppendTextLine(localeInfo.TOOLTIP_ENCHANTED)
						self.AppendSpace(3)
				self.AppendSpace(5)
				self.AppendTextLine(localeInfo.ACCE_ABSORB_CHANCE % (absChance), self.CONDITION_COLOR)
				self.AppendSpace(5)

				itemAbsorbedVnum = int(metinSlot[acce.ABSORBED_SOCKET])

				self.__ModelPreviewOpen(itemVnum, self.__ItemGetRace(), "acce")

				if itemAbsorbedVnum:
					self.AppendHorizontalLine()
					item.SelectItem(itemAbsorbedVnum)
					if item.GetItemType() == item.ITEM_TYPE_WEAPON:
						if item.GetItemSubType() == item.WEAPON_FAN:
							self.__AppendMagicAttackInfo(metinSlot[acce.ABSORPTION_SOCKET])
							item.SelectItem(itemAbsorbedVnum)
							self.__AppendAttackPowerInfo(metinSlot[acce.ABSORPTION_SOCKET])
						else:
							self.__AppendAttackPowerInfo(metinSlot[acce.ABSORPTION_SOCKET])
							item.SelectItem(itemAbsorbedVnum)
							self.__AppendMagicAttackInfo(metinSlot[acce.ABSORPTION_SOCKET])
					elif item.GetItemType() == item.ITEM_TYPE_ARMOR:
						defGrade = item.GetValue(1)
						defBonus = item.GetValue(5) * 2
						defGrade = self.CalcAcceValue(defGrade, metinSlot[acce.ABSORPTION_SOCKET])
						defBonus = self.CalcAcceValue(defBonus, metinSlot[acce.ABSORPTION_SOCKET])

						if defGrade > 0:
							self.AppendSpace(5)
							self.AppendTextLine(localeInfo.TOOLTIP_ITEM_DEF_GRADE % (defGrade + defBonus), self.GetChangeTextLineColor(defGrade))
							
						item.SelectItem(itemAbsorbedVnum)
						self.__AppendMagicDefenceInfo(metinSlot[acce.ABSORPTION_SOCKET])

					item.SelectItem(itemAbsorbedVnum)
					for i in xrange(item.ITEM_APPLY_MAX_NUM):
						(affectType, affectValue) = item.GetAffect(i)
						affectValue = self.CalcAcceValue(affectValue, metinSlot[acce.ABSORPTION_SOCKET])
						affectString = self.__GetAffectString(affectType, affectValue)
						if (not app.USE_ACCE_ABSORB_WITH_NO_NEGATIVE_BONUS and affectString and affectValue != 0)\
								or (app.USE_ACCE_ABSORB_WITH_NO_NEGATIVE_BONUS and affectString and affectValue > 0):
							self.AppendTextLine(affectString, self.GetChangeTextLineColor(affectValue))

						item.SelectItem(itemAbsorbedVnum)

					item.SelectItem(itemVnum)

					self.__AppendAttributeInformation(attrSlot, metinSlot[acce.ABSORPTION_SOCKET])

					self.AppendSpace(5)
					self.AppendHorizontalLine()
					self.AppendSpace(5)
					self.AppendTextLine(localeInfo.SASH_ABSORBED_ITEM, self.CONDITION_COLOR)
					item.SelectItem(itemAbsorbedVnum)
					self.AppendTextLine(item.GetItemName(), self.CONDITION_COLOR)
				else:
					self.__AppendAttributeInformation(attrSlot)
			else:
				self.__AppendAttributeInformation(attrSlot)

			self.AppendWearableInformation()
			self.AppendLastTimeInformation(metinSlot)

		elif itemVnum >= 91201 and itemVnum <= 91210:
			if 0 != metinSlot:
				attrType = item.GetValue(0)
				attrValue = item.GetValue(1)
				self.__AppendLimitInformation()
				self.AppendHorizontalLine()

				affectString = self.__GetAffectString(attrType, attrValue)
				if affectString:
					affectColor = self.__GetAttributeColor(0, attrValue)
					self.AppendTextLine(affectString, affectColor)
					self.AppendTextLine(localeInfo.TOOLTIP_BONUS_CAN_BE_USE_X_TIMES % metinSlot[0], affectColor)
					
		elif item.ITEM_TYPE_ROD == itemType:
			if 0 != metinSlot:
				curLevel = item.GetValue(0) / 10
				curEXP = metinSlot[0]
				maxEXP = item.GetValue(2)
				self.__AppendLimitInformation()
				self.__AppendRodInformation(curLevel, curEXP, maxEXP)
				self.AppendHorizontalLine()
				self.__AppendAffectInformation()

				if 0 != attrSlot:
					for i in xrange(3):
						attrType = attrSlot[i][0]
						attrValue = attrSlot[i][1]

						if 0 == attrValue:
							continue

						affectString = self.__GetAffectString(attrType, attrValue)
						if affectString:
							affectColor = self.__GetAttributeColor(i, attrValue)
							self.AppendTextLine(affectString, affectColor)
							self.toolTipHeight -= 7
							self.AppendTextLine(localeInfo.TOOLTIP_FISHINGROD_CAN_USE % (attrSlot[3+i][1]), affectColor)

		elif item.ITEM_TYPE_PICK == itemType:

			if 0 != metinSlot:
				curLevel = item.GetValue(0) / 10
				curEXP = metinSlot[0]
				maxEXP = item.GetValue(2)
				self.__AppendLimitInformation()
				self.__AppendPickInformation(curLevel, curEXP, maxEXP)
				self.AppendHorizontalLine()
				self.__AppendAffectInformation()

		elif item.ITEM_TYPE_LOTTERY == itemType:
			if 0 != metinSlot:

				ticketNumber = int(metinSlot[0])
				stepNumber = int(metinSlot[1])

				self.AppendSpace(5)
				self.AppendTextLine(localeInfo.TOOLTIP_LOTTERY_STEP_NUMBER % (stepNumber), self.NORMAL_COLOR)
				self.AppendTextLine(localeInfo.TOOLTIP_LOTTO_NUMBER % (ticketNumber), self.NORMAL_COLOR);

		elif item.ITEM_TYPE_METIN == itemType:
			self.AppendMetinInformation()
			self.AppendMetinWearInformation()

		elif item.ITEM_TYPE_FISH == itemType:
			if 0 != metinSlot:
				self.__AppendFishInfo(metinSlot[0], metinSlot[1])


		elif itemVnum == 80050:
			self.AppendSpace(5)
			self.AutoAppendTextLine(localeInfo.FRUIT_OF_LIFE_DESC_1.format(200))
			self.AutoAppendTextLine(localeInfo.FRUIT_OF_LIFE_DESC_2.format(50000))
		elif itemVnum == 80051:
			self.AppendSpace(5)
			self.AutoAppendTextLine(localeInfo.FRUIT_OF_LIFE_DESC_1.format(200))
			self.AutoAppendTextLine(localeInfo.FRUIT_OF_LIFE_DESC_2_1.format(50000, 100000))
		elif itemVnum == 80052:
			self.AppendSpace(5)
			self.AutoAppendTextLine(localeInfo.FRUIT_OF_LIFE_DESC_1.format(200))
			self.AutoAppendTextLine(localeInfo.FRUIT_OF_LIFE_DESC_2_1.format(100000, 200000))
		elif itemVnum == 80053:
			self.AppendSpace(5)
			self.AutoAppendTextLine(localeInfo.FRUIT_OF_LIFE_DESC_1.format(200))
			self.AutoAppendTextLine(localeInfo.FRUIT_OF_LIFE_DESC_2_1.format(200000, 300000))
		elif itemVnum == 80054:
			self.AppendSpace(5)
			self.AutoAppendTextLine(localeInfo.FRUIT_OF_LIFE_DESC_1.format(200))
			self.AutoAppendTextLine(localeInfo.FRUIT_OF_LIFE_DESC_2_1.format(300000, 400000))
		elif itemVnum == 80055:
			self.AppendSpace(5)
			self.AutoAppendTextLine(localeInfo.FRUIT_OF_LIFE_DESC_1.format(200))
			self.AutoAppendTextLine(localeInfo.FRUIT_OF_LIFE_DESC_2_1.format(400000, 500000))
		elif itemVnum == 203028:
			self.AppendSpace(5)
			self.AutoAppendTextLine(localeInfo.FRUIT_OF_LIFE_DESC_1.format(200))
			self.AutoAppendTextLine(localeInfo.FRUIT_OF_LIFE_DESC_2_1.format(500000, 600000))
		elif itemVnum == 203027:
			self.AppendSpace(5)
			self.AutoAppendTextLine(localeInfo.FRUIT_OF_LIFE_DESC_1.format(200))
			self.AutoAppendTextLine(localeInfo.FRUIT_OF_LIFE_DESC_2_1.format(600000, 700000))

		elif app.ENABLE_EXTENDED_BLEND and item.ITEM_TYPE_BLEND == itemType:
			self.__AppendLimitInformation()
			self.AppendSpace(5)

			if metinSlot and metinSlot[0] != 0:
				affectType = item.GetValue(0)
				affectValue = metinSlot[0]
				time = metinSlot[2]

				affectText = self.__GetAffectString(affectType, affectValue)
				self.AutoAppendTextLine(affectText, self.POSITIVE_COLOR)
				self.AppendSpace(5)

				if time > 0:
					self.AutoAppendTextLine(localeInfo.DURATION, self.CONDITION_COLOR)
					self.AutoAppendTextLine(emoji.AppendEmoji("icon/emoji/image-example-007.png")+" "+localeInfo.SecondToDHM(time), self.NORMAL_COLOR)
				else:
					self.AppendSpace(5)

			else:
				affectType = item.GetValue(0)
				affectValue = item.GetValue(2)
				time = item.GetValue(1)

				affectText = self.__GetAffectString(affectType, affectValue)
				self.AutoAppendTextLine(affectText, self.POSITIVE_COLOR)
				self.AppendSpace(5)

				if time > 0:
					self.AutoAppendTextLine(localeInfo.DURATION, self.CONDITION_COLOR)
					self.AutoAppendTextLine(emoji.AppendEmoji("icon/emoji/image-example-007.png")+" "+localeInfo.SecondToDHM(time), self.NORMAL_COLOR)
				else:
					self.AppendSpace(5)

		elif item.ITEM_TYPE_UNIQUE == itemType:
			if 0 != metinSlot:
				time = metinSlot[0]
				self.__AppendLimitInformation()
				self.__AppendAffectInformation()
				self.__AppendAttributeInformation(attrSlot)
				self.AppendMallItemLastTime(time)

		elif item.ITEM_TYPE_USE == itemType:
			self.__AppendLimitInformation()

			if item.USE_POTION == itemSubType or item.USE_POTION_NODELAY == itemSubType:
				self.__AppendPotionInformation()

			elif item.USE_ADD_IS_BONUS == itemSubType:
				self.__AppendAffectInformation()

			elif item.USE_ABILITY_UP == itemSubType:
				self.__AppendAbilityPotionInformation()

			if 27989 == itemVnum or 76006 == itemVnum:
				if 0 != metinSlot:
					useCount = int(metinSlot[0])

					self.AppendSpace(5)
					self.AppendTextLine(localeInfo.TOOLTIP_REST_USABLE_COUNT % (6 - useCount), self.NORMAL_COLOR)

			elif 50004 == itemVnum:
				if 0 != metinSlot:
					useCount = int(metinSlot[0])

					self.AppendSpace(5)
					self.AppendTextLine(localeInfo.TOOLTIP_REST_USABLE_COUNT % (10 - useCount), self.NORMAL_COLOR)

			elif 50840 == itemVnum:
				self.AppendSpace(5)
				
				affect1 = self.GetAffectStrings(72, 5)

				self.AppendTextLine(affect1, self.POSITIVE_COLOR)
				self.AutoAppendTextLine(localeInfo.DURATION, self.CONDITION_COLOR)
				self.AutoAppendTextLine(emoji.AppendEmoji("icon/emoji/image-example-007.png")+" 1h", self.NORMAL_COLOR)

			elif constInfo.IS_AUTO_POTION(itemVnum):
				if 0 != metinSlot:
					isActivated = int(metinSlot[0])
					usedAmount = float(metinSlot[1])
					totalAmount = float(metinSlot[2])

					if 0 == totalAmount:
						totalAmount = 1

					self.AppendSpace(5)

					if 0 != isActivated:
						self.AppendTextLine("(%s)" % (localeInfo.TOOLTIP_AUTO_POTION_USING), self.SPECIAL_POSITIVE_COLOR)
						self.AppendSpace(5)

			elif itemVnum in WARP_SCROLLS:
				if 0 != metinSlot:
					xPos = int(metinSlot[0])
					yPos = int(metinSlot[1])

					if xPos != 0 and yPos != 0:
						(mapName, xBase, yBase) = background.GlobalPositionToMapInfo(xPos, yPos)

						localeMapName=localeInfo.MINIMAP_ZONE_NAME_DICT.get(mapName, "")

						self.AppendSpace(5)

						if localeMapName!="":
							self.AppendTextLine(localeInfo.TOOLTIP_MEMORIZED_POSITION % (localeMapName, int(xPos-xBase)/100, int(yPos-yBase)/100), self.NORMAL_COLOR)
						else:
							self.AppendTextLine(localeInfo.TOOLTIP_MEMORIZED_POSITION_ERROR % (int(xPos)/100, int(yPos)/100), self.NORMAL_COLOR)
							dbg.TraceError("NOT_EXIST_IN_MINIMAP_ZONE_NAME_DICT: %s" % mapName)

			if item.USE_SPECIAL == itemSubType:
				if not self.AppendLastTimeInformation(metinSlot):
					if 0 != metinSlot:
						time = metinSlot[player.METIN_SOCKET_MAX_NUM-1]

						if 1 == item.GetValue(2):
							self.AppendMallItemLastTime(time)

			elif item.USE_TIME_CHARGE_PER == itemSubType:
				if metinSlot[2]:
					self.AppendTextLine(localeInfo.TOOLTIP_TIME_CHARGER_PER(metinSlot[2]))
				else:
					self.AppendTextLine(localeInfo.TOOLTIP_TIME_CHARGER_PER(item.GetValue(0)))

				self.AppendLastTimeInformation(metinSlot)

			elif item.USE_TIME_CHARGE_FIX == itemSubType:
				if metinSlot[2]:
					self.AppendTextLine(localeInfo.TOOLTIP_TIME_CHARGER_FIX(metinSlot[2]))
				else:
					self.AppendTextLine(localeInfo.TOOLTIP_TIME_CHARGER_FIX(item.GetValue(0)))

				self.AppendLastTimeInformation(metinSlot)

		elif item.ITEM_TYPE_QUEST == itemType:
			for i in xrange(item.LIMIT_MAX_NUM):
				(limitType, limitValue) = item.GetLimit(i)

				if item.LIMIT_REAL_TIME == limitType:
					self.AppendMallItemLastTime(metinSlot[0])
		elif item.ITEM_TYPE_DS == itemType:
			self.AppendTextLine(self.__DragonSoulInfoString(itemVnum))
			self.__AppendAttributeInformation(attrSlot)
			
		elif item.ITEM_TYPE_GUILD_GUARD == itemType:
			(limitType, limitValue) = item.GetLimit(0)

			if limitValue > 0 and item.LIMIT_LEVEL == limitType:
				self.AppendSpace(5)
				self.AutoAppendTextLine("Guardian level requirement: %d"%(limitValue), self.ENABLE_COLOR)

			self.__AppendAffectInformation()
			#self.AppendHorizontalLine()
			#self.__AppendAttributeInformation(attrSlot)

			if constInfo.SHOW_GUARD_OTHER_GUILD_SOCKET_INFO:
				#self.AppendHorizontalLine()
				guildID = metinSlot[0]
				if guildID == 0:
					self.AutoAppendTextLine("|cFFFFD700Does not belong to any guild")
				else:
					if app.GetGlobalTimeStamp() <= metinSlot[1] + 60 * 60 * 24:
						self.AutoAppendTextLine("This item belong to guild: |cFFACE08A%s|r"%guild.GetGuildName(guildID))
						self.AutoAppendTextLine("Soulbinded to this guild for: |cFFACE08A%s|r"%localeInfo.SecondToDHM((metinSlot[1] + 60 * 60 * 24) - app.GetGlobalTimeStamp()))
					else:
						self.AutoAppendTextLine("Once belonged to guild |cFFACE08A%s|r"%guild.GetGuildName(guildID))
			self.ResizeToolTip()
			self.AlignHorizonalCenter()
			
		elif item.ITEM_TYPE_TOGGLE == itemType:
			self.__AppendLimitInformation()
			self.__AppendAffectInformation()

			isActivated = int(metinSlot[3])
			if 0 != isActivated:
				self.AppendTextLine("(%s)" % localeInfo.TOOLTIP_TOGGLE_ACTIVE, self.SPECIAL_POSITIVE_COLOR)

			if itemSubType in (item.TOGGLE_AUTO_RECOVERY_HP, item.TOGGLE_AUTO_RECOVERY_SP):
				if metinSlot != 0:
					usedAmount = float(metinSlot[1])
					totalAmount = float(metinSlot[2])
					if 0 == totalAmount:
						totalAmount = 1

					rest = 100.0 - ((usedAmount / totalAmount) * 100.0)
					self.AppendTextLine(localeInfo.TOOLTIP_REMAINING % (rest), self.POSITIVE_COLOR)

		for i in xrange(item.LIMIT_MAX_NUM):
			(limitType, limitValue) = item.GetLimit(i)

			if item.LIMIT_REAL_TIME_START_FIRST_USE == limitType:
				self.AppendRealTimeStartFirstUseLastTime(item, metinSlot, i)

			elif item.LIMIT_TIMER_BASED_ON_WEAR == limitType:
				self.AppendTimerBasedOnWearLastTime(metinSlot)

		self.__AppendSealInformation(window_type, slotIndex)

		self.AdditionalTips(window_type, itemVnum, metinSlot, slotIndex)
		self.AlignTextLineHorizonalCenter()
		self.ShowToolTip()

	def AlignTextLineHorizonalCenter(self):
		for child in self.childrenList:
			if type(child).__name__ == "TextLine":
				(x, y) = child.GetLocalPosition()
				child.SetPosition(self.toolTipWidth / 2, y)

		self.ResizeToolTip()

	def AdditionalTips(self, window_type, itemVnum, metinSlot, slotIndex):
		item.SelectItem(itemVnum)
		itemType = item.GetItemType()
		itemSubType = item.GetItemSubType()
		itemSize = item.GetItemSize()
		
		if constInfo.ZNAJDZ_SKRZYNIE_TOOLTIP == 1:
			if app.__BL_CHEST_DROP_INFO__:
				if slotIndex != -1:
					if window_type is not 5:
						self.AppendChestDropInfo(itemVnum)

		if slotIndex > 277 < 1087:
			if window_type is not 5:
				if (item.CanUseInSpecialInv() == 0):
					if constInfo.WYCIAGNIJ_TOOLTIP == 1:
						self.AppendSpace(8)
						self.AutoAppendTextLine(localeInfo.INVENTORY_PULL_OUT_ITEM.format(emoji.AppendEmoji("icon/emoji/key_rclick.png")))
						self.AppendSpace(8)
		else:
			if slotIndex > -1 < 180:
				if item.GetSpecialInvType() != 0:
					if constInfo.DODAJ_DODATKOWE_TOOLTIP == 1:
						self.AppendSpace(8)
						self.AutoAppendTextLine("{} + {}".format(emoji.AppendEmoji("icon/emoji/key_shift.png"), emoji.AppendEmoji("icon/emoji/key_rclick.png")))
						self.AutoAppendTextLine(localeInfo.INVENTORY_MOVE_TO_SPECIAL_INVENTORY)
						self.AppendSpace(8)
		if constInfo.ROZDZIEL_TOOLTIP == 1:
			if app.ENABLE_EMOJI_SYSTEM and item.IsFlag(item.ITEM_FLAG_STACKABLE) and window_type in (player.INVENTORY,):
				if player.GetItemCount(window_type, slotIndex) > 1:
					self.AutoAppendTextLine("{} + {} - {}".format(emoji.AppendEmoji("icon/emoji/key_shift.png"), emoji.AppendEmoji("icon/emoji/key_lclick.png"), localeInfo.INVENTORY_SEPARATE_ITEMS))

		if itemVnum == 80008 or itemVnum == 80009 or itemVnum == 80007 or itemVnum == 80006 or itemVnum == 80005:
			if player.GetItemCount(window_type, slotIndex) > 1:
				self.AutoAppendTextLine("{} + {} - {}".format(emoji.AppendEmoji("icon/emoji/key_alt.png"),  emoji.AppendEmoji("icon/emoji/key_rclick.png"), 
												  localeInfo.INVENTORY_USE_SOME_ITEMS.format(500)))

		if app.ENABLE_EMOJI_SYSTEM:
			if item.GetItemType() == item.ITEM_TYPE_GIFTBOX:
				if not itemVnum in item_ids:
					self.AutoAppendTextLine("{} + {} - {}".format(emoji.AppendEmoji("icon/emoji/key_ctrl.tga"),  emoji.AppendEmoji("icon/emoji/key_rclick.tga"), localeInfo.INVENTORY_SHOW_CHEST_DROP))
					if slotIndex != -1:
						self.AutoAppendTextLine("{} + {} - {}".format(emoji.AppendEmoji("icon/emoji/key_alt.png"),  emoji.AppendEmoji("icon/emoji/key_rclick.png"), 
													localeInfo.INVENTORY_USE_SOME_ITEMS.format(500)))

		if not itemVnum in item_ids:
			self.AutoAppendTextLine("{} + {} + {} - {}".format(emoji.AppendEmoji("icon/emoji/key_ctrl.tga"),  emoji.AppendEmoji("icon/emoji/key_alt.png"),  emoji.AppendEmoji("icon/emoji/key_rclick.png"), localeInfo.SEARCH_IN_SHOPS_SHORTCUT))
			self.AutoAppendTextLine("{} + {} + {} - {}".format(emoji.AppendEmoji("icon/emoji/key_ctrl.tga"),  emoji.AppendEmoji("icon/emoji/key_alt.png"),  emoji.AppendEmoji("icon/emoji/key_lclick.png"), localeInfo.TOOLTIP_WIKI))
		
		if itemVnum == 201234:
			self.AutoAppendTextLine("{} + {} + {} - {}".format(emoji.AppendEmoji("icon/emoji/key_ctrl.tga"),  emoji.AppendEmoji("icon/emoji/key_alt.png"), emoji.AppendEmoji("icon/emoji/key_rclick.png"), localeInfo.INVENTORY_OPEN_VOTESHOP))
												
		if itemVnum == 201242:
			self.AutoAppendTextLine("{} + {} + {} - {}".format(emoji.AppendEmoji("icon/emoji/key_ctrl.tga"),  emoji.AppendEmoji("icon/emoji/key_alt.png"), emoji.AppendEmoji("icon/emoji/key_rclick.png"), localeInfo.INVENTORY_OPEN_WORLDBOSS))

		if itemVnum == 70104:
			self.AppendSpace(5)
			self.AutoAppendTextLine(localeInfo.POLYMARBLE_STAGE.format(1))

		if itemVnum == 53916:
			self.AppendSpace(5)
			affect1 = self.GetAffectStrings(1, 2000)
			affect2 = self.GetAffectStrings(1, 15)
			affect3 = self.GetAffectStrings(53, 50)

			self.AutoAppendTextLine(affect1, self.POSITIVE_COLOR)
			self.AutoAppendTextLine(affect2, self.POSITIVE_COLOR)
			self.AutoAppendTextLine(affect3, self.POSITIVE_COLOR)

		if itemVnum == 70105:
			self.AppendSpace(5)
			self.AutoAppendTextLine(localeInfo.POLYMARBLE_STAGE.format(2))
			self.AppendSpace(2)
			affect1 = self.GetAffectStrings(63, 10)
			affect2 = self.GetAffectStrings(106, 10)
			self.AutoAppendTextLine(affect1, self.POSITIVE_COLOR)
			self.AutoAppendTextLine(affect2, self.POSITIVE_COLOR)
		
		if itemVnum == 70106:
			self.AppendSpace(5)
			self.AutoAppendTextLine(localeInfo.POLYMARBLE_STAGE.format(3))
			self.AppendSpace(2)
			affect1 = self.GetAffectStrings(63, 20)
			affect2 = self.GetAffectStrings(106, 15)
			self.AutoAppendTextLine(affect1, self.POSITIVE_COLOR)
			self.AutoAppendTextLine(affect2, self.POSITIVE_COLOR)

		if chr.IsGameMaster(0):
			self.AppendSpace(5)
			self.AutoAppendTextLine("VNUM: {}".format(itemVnum) + ", TYPE: {}".format(itemType) + ", SUBTYPE: {}".format(itemSubType))
			if metinSlot:
				self.AutoAppendTextLine("SOCKETS: [{}]".format(', '.join([str(i) for i in metinSlot])))
				self.AutoAppendTextLine("WINDOW_TYPE: {}".format(window_type) + ", SLOT_INDEX: {}".format(slotIndex))
				self.AutoAppendTextLine("SPECIAL_INV: TYPE: {}".format(item.GetSpecialInvType()) + ", CAN_USE_INSIDE: {}".format(item.CanUseInSpecialInv()))
			self.ResizeToolTip()

	def __DragonSoulInfoString (self, dwVnum):
		step = (dwVnum / 100) % 10
		refine = (dwVnum / 10) % 10
		if 0 == step:
			return localeInfo.DRAGON_SOUL_STEP_LEVEL1 + " " + localeInfo.DRAGON_SOUL_STRENGTH(refine)
		elif 1 == step:
			return localeInfo.DRAGON_SOUL_STEP_LEVEL2 + " " + localeInfo.DRAGON_SOUL_STRENGTH(refine)
		elif 2 == step:
			return localeInfo.DRAGON_SOUL_STEP_LEVEL3 + " " + localeInfo.DRAGON_SOUL_STRENGTH(refine)
		elif 3 == step:
			return localeInfo.DRAGON_SOUL_STEP_LEVEL4 + " " + localeInfo.DRAGON_SOUL_STRENGTH(refine)
		elif 4 == step:
			return localeInfo.DRAGON_SOUL_STEP_LEVEL5 + " " + localeInfo.DRAGON_SOUL_STRENGTH(refine)
		else:
			return ""

	def __AppendCostumeRealTimeItem(self, time):
			self.AppendSpace(5)
			self.AppendTextLine(localeInfo.AVAILABLE.format(localeInfo.TimeToDHMS(time)), self.NORMAL_COLOR)
			
	def __AppendCostumeRealTime(self, metinSlot):
		if metinSlot[3] > 0:
			self.AppendSpace(5)
			self.AppendTextLine(localeInfo.LEFT_BONUS_TIME + ": " + localeInfo.TimeToDHMS(metinSlot[3] - app.GetGlobalTimeStamp()), self.NORMAL_COLOR)
		
	def __IsHair(self, itemVnum):
		return (self.__IsOldHair(itemVnum) or
			self.__IsNewHair(itemVnum) or
			self.__IsNewHair2(itemVnum) or
			self.__IsNewHair3(itemVnum)
		)

	if app.__BL_CHEST_DROP_INFO__:
		def AppendChestDropInfo(self, itemVnum):
			hasinfo = item.HasDropInfo(itemVnum, False)
			if hasinfo:
				self.AppendSpace(5)
				self.AutoAppendTextLine(localeInfo.CHEST_DROP_INFO, self.NORMAL_COLOR)
				self.AutoAppendTextLine(localeInfo.CHEST_FIND_ITEMS_DESC_1)
				self.AutoAppendTextLine(localeInfo.CHEST_FIND_ITEMS_DESC_2)
				self.AppendSpace(5)

	def __IsOldHair(self, itemVnum):
		return itemVnum > 73000 and itemVnum < 74000

	def __IsNewHair(self, itemVnum):
		return itemVnum > 74000 and itemVnum < 75000

	def __IsNewHair2(self, itemVnum):
		return itemVnum > 75000 and itemVnum < 76000

	def __IsNewHair3(self, itemVnum):
		return ((74012 < itemVnum and itemVnum < 74022) or
			(74262 < itemVnum and itemVnum < 74272) or
			(74512 < itemVnum and itemVnum < 74599) or
			(74762 < itemVnum and itemVnum < 74799) or
			(99901 < itemVnum and itemVnum < 99903) or
			(45000 < itemVnum and itemVnum < 47000))

	if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
		def __AppendPrivateItemIcon(self, itemVnum):
			itemImage = ui.ImageBox()
			itemImage.SetParent(self)
			itemImage.Show()
			item.SelectItem(itemVnum)
			itemImage.LoadImage(item.GetIconImageFileName())
			itemImage.SetPosition((self.toolTipWidth/2)-16, self.toolTipHeight)
			self.toolTipHeight += itemImage.GetHeight()
			self.childrenList.append(itemImage)
			self.ResizeToolTip()

	if app.__BL_HYPERLINK_ITEM_ICON__:
		def AppendItemIcon(self, itemVnum):
			itemImage = ui.ImageBox()
			itemImage.SetParent(self)
			itemImage.Show()			
			item.SelectItem(itemVnum)
			try:
				itemImage.LoadImage(item.GetIconImageFileName())
			except:
				dbg.TraceError("ItemToolTip.AppendItemIcon() - Failed to find image file %d:%s" % 
					(itemVnum, item.GetIconImageFileName())
				)
			itemImage.SetPosition(self.toolTipWidth / 2 - itemImage.GetWidth() / 2, self.toolTipHeight)
			self.toolTipHeight += itemImage.GetHeight()
			self.childrenList.append(itemImage)
			self.ResizeToolTip()

	def __AppendHairIcon(self, itemVnum):
		itemImage = ui.ImageBox()
		itemImage.SetParent(self)
		itemImage.Show()

		if self.__IsOldHair(itemVnum):
			itemImage.LoadImage("d:/ymir work/item/quest/"+str(itemVnum)+".tga")
		elif self.__IsNewHair3(itemVnum):
			itemImage.LoadImage("icon/hair/%d.sub" % (itemVnum))
		elif self.__IsNewHair(itemVnum):
			itemImage.LoadImage("d:/ymir work/item/quest/"+str(itemVnum-1000)+".tga")
		elif self.__IsNewHair2(itemVnum):
			itemImage.LoadImage("icon/hair/%d.sub" % (itemVnum))

		if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
			if self.isPrivateSearchItem:
				itemImage.SetPosition((self.toolTipWidth/2)-48, self.toolTipHeight)
			else:
				itemImage.SetPosition((self.toolTipWidth/2)-48, self.toolTipHeight)
		else:
			itemImage.SetPosition(itemImage.GetWidth()/2, self.toolTipHeight)

		self.toolTipHeight += itemImage.GetHeight()
		self.childrenList.append(itemImage)
		self.ResizeToolTip()

	def __AdjustMaxWidth(self, attrSlot, desc):
		newToolTipWidth = self.toolTipWidth
		newToolTipWidth = max(self.__AdjustAttrMaxWidth(attrSlot), newToolTipWidth)
		newToolTipWidth = max(self.__AdjustDescMaxWidth(desc), newToolTipWidth)
		if newToolTipWidth > self.toolTipWidth:
			self.toolTipWidth = newToolTipWidth
			self.ResizeToolTip()

	def __AdjustAttrMaxWidth(self, attrSlot):
		if 0 == attrSlot:
			return self.toolTipWidth

		maxWidth = self.toolTipWidth
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			type = attrSlot[i][0]
			value = attrSlot[i][1]

			attrText = self.AppendTextLine(self.__GetAffectString(type, value))
			(tW, _) = attrText.GetTextSize()
			self.childrenList.remove(attrText)
			self.toolTipHeight -= self.TEXT_LINE_HEIGHT

			maxWidth = max(tW + 12, maxWidth)

		return maxWidth

	def __AdjustDescMaxWidth(self, desc):
		if len(desc) < DESC_DEFAULT_MAX_COLS:
			return self.toolTipWidth

		return DESC_WESTERN_MAX_WIDTH

	def ResizeToolTipWidth(self, width):
		self.toolTipWidth = width
		self.ResizeToolTip()

	def __CalculateToolTipWidth(self):
		affectTextLineLenList = []

		metinSocket = self.metinSlot
		if metinSocket:
			for socketIndex in metinSocket:
				if socketIndex:

					affectType, affectValue = item.GetAffect(0)
					affectString = self.__GetAffectString(affectType, affectValue)
					if affectString:
						affectTextLineLenList.append(len(affectString))

			if self.itemVnum:
				item.SelectItem(self.itemVnum)
			self.metinSlot = None

		if self.toolTipWidth == self.TOOL_TIP_WIDTH:
			if affectTextLineLenList:
				self.toolTipWidth += max(affectTextLineLenList) + 10

		self.AlignTextLineHorizonalCenter()

	def __SetSkillBookToolTip(self, skillIndex, bookName, skillGrade):
		skillName = skill.GetSkillName(skillIndex)

		if not skillName:
			return

		if localeInfo.IsVIETNAM():
			itemName = bookName + " " + skillName
		else:
			itemName = skillName + " " + bookName
		self.SetTitle(itemName)

	def __AppendPickInformation(self, curLevel, curEXP, maxEXP):
		self.AppendSpace(5)
		self.AppendTextLine(localeInfo.TOOLTIP_PICK_LEVEL % (curLevel), self.NORMAL_COLOR)
		self.AppendTextLine(localeInfo.TOOLTIP_PICK_EXP % (curEXP, maxEXP), self.NORMAL_COLOR)

		if curEXP == maxEXP:
			self.AppendSpace(5)
			self.AppendTextLine(localeInfo.TOOLTIP_PICK_UPGRADE1, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.TOOLTIP_PICK_UPGRADE2, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.TOOLTIP_PICK_UPGRADE3, self.NORMAL_COLOR)


	def __AppendRodInformation(self, curLevel, curEXP, maxEXP):
		self.AppendSpace(5)
		self.AppendTextLine(localeInfo.TOOLTIP_FISHINGROD_LEVEL % (curLevel), self.NORMAL_COLOR)
		self.AppendTextLine(localeInfo.TOOLTIP_FISHINGROD_EXP % (curEXP, maxEXP), self.NORMAL_COLOR)

		if curEXP == maxEXP:
			self.AppendSpace(5)
			self.AppendTextLine(localeInfo.TOOLTIP_FISHINGROD_UPGRADE1, self.NORMAL_COLOR)

	def __AppendLimitInformation(self):

		appendSpace = False

		for i in xrange(item.LIMIT_MAX_NUM):

			(limitType, limitValue) = item.GetLimit(i)

			if limitValue > 0:
				if False == appendSpace:
					self.AppendSpace(5)
					appendSpace = True

			else:
				continue

			if item.LIMIT_LEVEL == limitType:
				color = self.GetLimitTextLineColor(player.GetStatus(player.LEVEL), limitValue)
				self.AppendTextLine(self.swordEmoji + localeInfo.TOOLTIP_ITEM_LIMIT_LEVEL % (limitValue) + self.swordEmoji, color)
			elif item.LIMIT_MOUNT_LEVEL == limitType:
				color = self.GetLimitTextLineColor(constInfo.MOUNT_LEVEL, limitValue)
				self.AppendTextLine(localeInfo.TOOLTIP_ITEM_MOUNT_LIMIT_LEVEL % (limitValue), color)
			"""
			elif item.LIMIT_STR == limitType:
				color = self.GetLimitTextLineColor(player.GetStatus(player.ST), limitValue)
				self.AppendTextLine(localeInfo.TOOLTIP_ITEM_LIMIT_STR % (limitValue), color)
			elif item.LIMIT_DEX == limitType:
				color = self.GetLimitTextLineColor(player.GetStatus(player.DX), limitValue)
				self.AppendTextLine(localeInfo.TOOLTIP_ITEM_LIMIT_DEX % (limitValue), color)
			elif item.LIMIT_INT == limitType:
				color = self.GetLimitTextLineColor(player.GetStatus(player.IQ), limitValue)
				self.AppendTextLine(localeInfo.TOOLTIP_ITEM_LIMIT_INT % (limitValue), color)
			elif item.LIMIT_CON == limitType:
				color = self.GetLimitTextLineColor(player.GetStatus(player.HT), limitValue)
				self.AppendTextLine(localeInfo.TOOLTIP_ITEM_LIMIT_CON % (limitValue), color)
			"""

	def __AppendSealInformation(self, window_type, slotIndex):
		if not app.ENABLE_SOULBIND_SYSTEM:
			return

		itemSealDate = player.GetItemSealDate(window_type, slotIndex)
		if itemSealDate == item.GetDefaultSealDate():
			return

		if itemSealDate == item.GetUnlimitedSealDate():
			self.AppendSpace(5)
			self.AppendTextLine(localeInfo.TOOLTIP_SEALED, self.NEGATIVE_COLOR)

		elif itemSealDate > 0:
			self.AppendSpace(5)
			hours, minutes = player.GetItemUnSealLeftTime(window_type, slotIndex)
			self.AppendTextLine(localeInfo.TOOLTIP_UNSEAL_LEFT_TIME % (hours, minutes), self.NEGATIVE_COLOR)

	def __GetAffectString(self, affectType, affectValue):
		if 0 == affectType:
			return None

		if 0 == affectValue:
			return None

		try:
			return self.AFFECT_DICT[affectType](affectValue)
		except TypeError:
			return "UNKNOWN_VALUE[%s] %s" % (affectType, affectValue)
		except KeyError:
			return "UNKNOWN_TYPE[%s] %s" % (affectType, affectValue)

	def __AppendAffectInformation(self):
		for i in xrange(item.ITEM_APPLY_MAX_NUM):
			(affectType, affectValue) = item.GetAffect(i)
			if app.ENABLE_ACCE_COSTUME_SYSTEM and affectType==item.APPLY_ACCEDRAIN_RATE:
				continue
			affectString = self.__GetAffectString(affectType, affectValue)
			if affectString:
				self.AppendTextLine(affectString, self.GetChangeTextLineColor(affectValue))
	
	if app.ENABLE_EXTRABONUS_SYSTEM:
		def __AppendExtraAffectInformation(self, value):
			for i in xrange(item.ITEM_APPLY_MAX_NUM):
				(affectType, affectValue) = item.GetAffect(i)
				affectString = self.__GetAffectString(affectType, affectValue)
				if affectString:
					if value == 0:
						self.AppendTextLine("|cFF89b88d"+str(affectString)+" |cFFc8ff9c+"+str(affectValue*constInfo.RUNE_EXTRABONUS_VALUE/100)+"", self.GetChangeTextLineColor(affectValue))
					elif value == 1:
						self.AppendTextLine("|cFF89b88d"+str(affectString)+" |cFFc8ff9c+"+str(affectValue*constInfo.PET_EXTRABONUS_VALUE/100)+"", self.GetChangeTextLineColor(affectValue))

	if app.ENABLE_EXTRABONUS_SYSTEM:
		if app.ENABLE_GLOVE_SYSTEM:
			def __AppendGloveAttributeInformation(self, attrSlot, itemAbsChance = 0):
				if 0 != attrSlot:
					for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
						type = attrSlot[i][0]
						value = attrSlot[i][1]
						if 0 == value:
							continue
						affectString = self.__GetAffectString(type, value)
						if affectString:
							if i < 3:
								self.AppendTextLine("|cFF89b88d" + str(affectString) + " |cFFc8ff9c+" + str(GLOVE_BONUS_TABLE[type] / 9),self.GetChangeTextLineColor(value))
							else:
								self.AppendTextLine(affectString, self.SPECIAL_POSITIVE_COLOR)

			def AddGloveItemData(self, itemVnum, metinSlot, attrSlot = 0, flags = 0, unbindTime = 0, window_type = player.INVENTORY, slotIndex = -1):
				self.itemVnum = itemVnum
				item.SelectItem(itemVnum)
				itemType = item.GetItemType()
				itemSubType = item.GetItemSubType()
				self.__ModelPreviewClose()

				itemDesc = item.GetItemDescription()
				itemSummary = item.GetItemSummary()
				self.__AdjustMaxWidth(attrSlot, itemDesc)
				self.__SetItemTitle(itemVnum, metinSlot, attrSlot)
				self.AppendDescription(itemDesc, 26)
				self.AppendDescription(itemSummary, 26, self.CONDITION_COLOR)

				self.__AppendLimitInformation()
				self.__AppendMagicDefenceInfo()
				self.__AppendAffectInformation()	
				self.__AppendGloveAttributeInformation(attrSlot)
				self.AppendWearableInformation()
				self.__AppendMetinSlotInfo(metinSlot)

				self.__AppendSealInformation(window_type, slotIndex)
				self.AdditionalTips(window_type, itemVnum, metinSlot, slotIndex)
				self.AlignTextLineHorizonalCenter()
				self.ShowToolTip()

	def AppendLastTimeInformation(self, metinSlot):
		bHasRealtimeFlag = False
		for i in xrange(item.LIMIT_MAX_NUM):
			(limitType, limitValue) = item.GetLimit(i)
			if item.LIMIT_REAL_TIME == limitType:
				bHasRealtimeFlag = True

		if bHasRealtimeFlag:
			self.AppendMallItemLastTime(metinSlot[0])

		return bHasRealtimeFlag

	def AppendWearableInformation(self):

		self.AppendSpace(5)
		self.AppendTextLine(localeInfo.TOOLTIP_ITEM_WEARABLE_JOB, self.NORMAL_COLOR)

		flagList = (
			not item.IsAntiFlag(item.ITEM_ANTIFLAG_WARRIOR),
			not item.IsAntiFlag(item.ITEM_ANTIFLAG_ASSASSIN),
			not item.IsAntiFlag(item.ITEM_ANTIFLAG_SURA),
			not item.IsAntiFlag(item.ITEM_ANTIFLAG_SHAMAN))
		if app.ENABLE_WOLFMAN_CHARACTER:
			flagList += (not item.IsAntiFlag(item.ITEM_ANTIFLAG_WOLFMAN),)
		characterNames = ""
		for i in xrange(self.CHARACTER_COUNT):

			name = self.CHARACTER_NAMES[i]
			flag = flagList[i]

			if flag:
				characterNames += " "
				characterNames += name

		textLine = self.AppendTextLine(characterNames, self.NORMAL_COLOR, True)
		textLine.SetFeather()

		if item.IsAntiFlag(item.ITEM_ANTIFLAG_MALE):
			textLine = self.AppendTextLine(localeInfo.FOR_FEMALE, self.NORMAL_COLOR, True)
			textLine.SetFeather()

		if item.IsAntiFlag(item.ITEM_ANTIFLAG_FEMALE):
			textLine = self.AppendTextLine(localeInfo.FOR_MALE, self.NORMAL_COLOR, True)
			textLine.SetFeather()

	def __AppendPotionInformation(self):
		self.AppendSpace(5)

		healHP = item.GetValue(0)
		healSP = item.GetValue(1)
		healStatus = item.GetValue(2)
		healPercentageHP = item.GetValue(3)
		healPercentageSP = item.GetValue(4)

		if healHP > 0:
			self.AppendTextLine(localeInfo.TOOLTIP_POTION_PLUS_HP_POINT % healHP, self.GetChangeTextLineColor(healHP))
		if healSP > 0:
			self.AppendTextLine(localeInfo.TOOLTIP_POTION_PLUS_SP_POINT % healSP, self.GetChangeTextLineColor(healSP))
		if healStatus != 0:
			self.AppendTextLine(localeInfo.TOOLTIP_POTION_CURE)
		if healPercentageHP > 0:
			self.AppendTextLine(localeInfo.TOOLTIP_POTION_PLUS_HP_PERCENT % healPercentageHP, self.GetChangeTextLineColor(healPercentageHP))
		if healPercentageSP > 0:
			self.AppendTextLine(localeInfo.TOOLTIP_POTION_PLUS_SP_PERCENT % healPercentageSP, self.GetChangeTextLineColor(healPercentageSP))

	def __AppendAbilityPotionInformation(self):

		self.AppendSpace(5)

		abilityType = item.GetValue(0)
		time = item.GetValue(1)
		point = item.GetValue(2)


		affectText = self.__GetAffectString(abilityType, point)
		self.AutoAppendTextLine(affectText, self.POSITIVE_COLOR)
		self.AppendSpace(5)

		if time > 0:

			self.AutoAppendTextLine(localeInfo.DURATION, self.CONDITION_COLOR)
			self.AutoAppendTextLine(emoji.AppendEmoji("icon/emoji/image-example-007.png")+" "+localeInfo.SecondToDHM(time), self.NORMAL_COLOR)

	def GetPriceColor(self, price):
		if price>=constInfo.HIGH_PRICE:
			return self.HIGH_PRICE_COLOR
		if price>=constInfo.MIDDLE_PRICE:
			return self.MIDDLE_PRICE_COLOR
		else:
			return self.LOW_PRICE_COLOR

	def AppendPrice(self, price):
		if price <=0:
			return
		if not app.ENABLE_BUY_WITH_ITEM:
			self.AppendSpace(5)
		self.AppendTextLine(localeInfo.NumberToGoldNotText(price)+" {}".format(emoji.AppendEmoji("icon/emoji/money_icon.png")), self.GetPriceColor(price))

	def AppendPriceInfo(self, price, itemtable=[], count=1):
		if self.shopInfo == 0:
			self.AppendSpace(15)
			self.AppendTextLine("[" + localeInfo.OFFLINESHOP_NEW19 + "]", self.SPECIAL_TITLE_COLOR)
		self.AppendSpace(2)
		if self.shopInfo > 0:
			self.AppendSpace(5)
		self.shopInfo += 1
		mainWindow = ui.ScriptWindow()
		mainWindow.SetParent(self)
		mainWindow.Show()
		mainWindow.leftbar = ui.ExpandedImageBox()
		mainWindow.leftbar.SetParent(mainWindow)
		mainWindow.leftbar.SetPosition(0, 0)
		mainWindow.leftbar.LoadImage("elvion/game/tooltip/" + str(len(itemtable)) + "left.png")
		mainWindow.leftbar.SetDiffuseColor(1.0, 0.0, 0.0, 0.8)
		mainWindow.leftbar.Show()
		mainWindow.rightbar = ui.ImageBox()
		mainWindow.rightbar.SetParent(mainWindow)
		mainWindow.rightbar.SetPosition(0, 0)
		mainWindow.rightbar.LoadImage("elvion/game/tooltip/" + str(len(itemtable)) + "right.png")
		mainWindow.rightbar.SetDiffuseColor(1.0, 0.0, 0.0, 0.8)
		mainWindow.rightbar.Show()
		mainWindow.center = ui.ExpandedImageBox()
		mainWindow.center.SetParent(mainWindow)
		mainWindow.center.SetPosition(10, 0)
		mainWindow.center.LoadImage("elvion/game/tooltip/" + str(len(itemtable)) + "mid.png")
		mainWindow.center.SetDiffuseColor(1.0, 0.0, 0.0, 0.8)
		mainWindow.center.Show()

		mainWindow.textLine = ui.TextLine()
		mainWindow.textLine.SetParent(mainWindow)
		mainWindow.textLine.SetFontName(self.defFontName)
		mainWindow.textLine.SetPackedFontColor(0xBBFFac8a)
		mainWindow.textLine.SetText(localeInfo.OFFLINESHOP_NEW26 + ":")
		mainWindow.textLine.SetOutline()
		mainWindow.textLine.SetFeather(False)
		mainWindow.textLine.Show()
		mainWindow.textLine.y = self.toolTipHeight
		mainWindow.textLine.SetPosition(10, 7)
		mainWindow.textLine2 = ui.TextLine()
		mainWindow.textLine2.SetParent(mainWindow)
		mainWindow.textLine2.SetFontName(self.defFontName)
		# mainWindow.textLine2.SetPackedFontColor(0xBBFFac8a)
		text = localeInfo.NumberToMoneyString(price)
		# if count != 1:
		# text += " ("+localeInfo.NumberToMoneyString(price/count)+" / Item)"
		mainWindow.textLine2.SetText(text)

		allHave = True
		if player.GetElk() >= price:
			mainWindow.textLine2.SetPackedFontColor(0xBB8AFFAC)
		else:
			mainWindow.textLine2.SetPackedFontColor(0xBBFFac8a)
			allHave = False

		mainWindow.textLine2.SetOutline()
		mainWindow.textLine2.SetFeather(False)
		mainWindow.textLine2.SetHorizontalAlignRight()
		mainWindow.textLine2.Show()
		mainWindow.textLine2.y = 7
		mainWindow.textLine2.SetPosition(self.toolTipWidth - 31, 7)
		mainWindow.SetPosition(10, self.toolTipHeight)

		mainWindow.toolTipHeight = self.toolTipHeight

		self.toolTipHeight += 28

		(textWidth1, textHeight) = mainWindow.textLine.GetTextSize()
		(textWidth2, textHeight) = mainWindow.textLine2.GetTextSize()
		textWidth = textWidth1 + textWidth2
		textWidth += 40
		textHeight += 5

		if textWidth < 250:
			textWidth = 250

		mainWindow.items = []

		for needitem in itemtable:
			item.SelectItem(needitem[0])
			icon = ui.ExpandedImageBox()
			icon.SetParent(mainWindow)
			icon.y = 26 + len(mainWindow.items) * 24
			icon.SetPosition(self.toolTipWidth - 50, 26 + len(mainWindow.items) * 24)
			icon.LoadImage(item.GetIconImageFileName())
			icon.SetScale(0.6, 0.6)
			icon.SetSize(0, 0)
			icon.Show()
			textLine = ui.TextLine()
			textLine.SetParent(mainWindow)
			textLine.SetFontName(self.defFontName)
			# textLine.SetPackedFontColor(0xBBFFac8a)
			textLine.SetText(str(needitem[1]) + "x " + item.GetItemName())
			textLine.SetOutline()
			textLine.SetFeather(False)
			textLine.SetHorizontalAlignRight()
			textLine.Show()
			textLine.y = 8 + 21 + len(mainWindow.items) * 24
			textLine.SetPosition(self.toolTipWidth - 50 - 5, 8 + 21 + len(mainWindow.items) * 24)
			textLine.Show()

			if player.GetItemCountByVnum(needitem[0]) >= needitem[1]:
				textLine.SetPackedFontColor(0xBB8AFFAC)
			else:
				textLine.SetPackedFontColor(0xBBFFac8a)
				allHave = False

			row = []
			row.append(icon)
			row.append(textLine)
			mainWindow.items.append(row)
			self.toolTipHeight += 24

		if allHave:
			mainWindow.leftbar.SetDiffuseColor(0.0, 1.0, 0.0, 0.8)
			mainWindow.rightbar.SetDiffuseColor(0.0, 1.0, 0.0, 0.8)
			mainWindow.center.SetDiffuseColor(0.0, 1.0, 0.0, 0.8)
			mainWindow.textLine.SetPackedFontColor(0xBB8AFFAC)
		else:
			mainWindow.leftbar.SetDiffuseColor(1.0, 0.0, 0.0, 0.8)
			mainWindow.rightbar.SetDiffuseColor(1.0, 0.0, 0.0, 0.8)
			mainWindow.center.SetDiffuseColor(1.0, 0.0, 0.0, 0.8)
			mainWindow.textLine.SetPackedFontColor(0xBBFFac8a)

		mainWindow.width = self.toolTipWidth - 20
		mainWindow.rightbar.SetPosition(mainWindow.width - 10, 0)
		mainWindow.center.SetPercentage(mainWindow.width - 20, 10)

		self.childrenList.append(mainWindow)

		if self.toolTipWidth < textWidth:
			oldSize = self.toolTipWidth
			mainWindow.width = textWidth - 20
			self.toolTipWidth = textWidth
			if True:
				self.Rebuild(textWidth, oldSize)

		self.ResizeToolTip()

		return mainWindow

	def AppendItemInfo(self, title, itemtable=[], count=1, rgba=(1.0, 1.0, 1.0, 1.0), col1=0xAAcdac8a, col2=0xA0cdac8a):
		# if self.shopInfo == 0:
		# self.AppendSpace(15)
		# self.AppendTextLine("["+localeInfo.OFFLINESHOP_NEW19+"]", self.SPECIAL_TITLE_COLOR)
		# self.AppendSpace(2)
		# if self.shopInfo > 0:
		self.AppendSpace(5)
		# self.shopInfo+=1
		mainWindow = ui.ScriptWindow()
		mainWindow.SetParent(self)
		mainWindow.Show()
		mainWindow.leftbar = ui.ExpandedImageBox()
		mainWindow.leftbar.SetParent(mainWindow)
		mainWindow.leftbar.SetPosition(0, 0)
		mainWindow.leftbar.LoadImage("elvion/game/tooltip/" + str(len(itemtable)) + "left.png")
		mainWindow.leftbar.SetDiffuseColor(*rgba)
		mainWindow.leftbar.Show()
		mainWindow.rightbar2 = ui.ImageBox()
		mainWindow.rightbar2.SetParent(mainWindow)
		mainWindow.rightbar2.SetPosition(0, 0)
		mainWindow.rightbar2.LoadImage("elvion/game/tooltip/" + str(len(itemtable)) + "right.png")
		mainWindow.rightbar2.SetDiffuseColor(*rgba)
		mainWindow.rightbar2.Show()
		mainWindow.center = ui.ExpandedImageBox()
		mainWindow.center.SetParent(mainWindow)
		mainWindow.center.SetPosition(10, 0)
		mainWindow.center.LoadImage("elvion/game/tooltip/" + str(len(itemtable)) + "mid.png")
		mainWindow.center.SetDiffuseColor(*rgba)
		mainWindow.center.Show()

		mainWindow.textLine = ui.TextLine()
		mainWindow.textLine.SetParent(mainWindow)
		mainWindow.textLine.SetFontName(self.defFontName)
		mainWindow.textLine.SetPackedFontColor(col1)
		mainWindow.textLine.SetText(title)
		mainWindow.textLine.SetOutline()
		mainWindow.textLine.SetFeather(False)
		mainWindow.textLine.Show()
		mainWindow.textLine.y = self.toolTipHeight
		mainWindow.textLine.SetHorizontalAlignCenter()
		# mainWindow.textLine.SetWindowHorizontalAlignCenter()
		mainWindow.textLine.SetPosition(0, 7)
		mainWindow.textLine2 = ui.TextLine()
		mainWindow.textLine2.SetParent(mainWindow)
		mainWindow.textLine2.SetFontName(self.defFontName)
		mainWindow.textLine2.SetPackedFontColor(col1)
		mainWindow.textLine2.Hide()
		# text = localeInfo.NumberToMoneyString(price)
		# if count != 1:
		# text += " ("+localeInfo.NumberToMoneyString(price/count)+" / Item)"
		# mainWindow.textLine2.SetText(text)

		mainWindow.textLine2.SetOutline()
		mainWindow.textLine2.SetFeather(False)
		mainWindow.textLine2.SetHorizontalAlignRight()
		mainWindow.textLine2.Show()
		mainWindow.textLine2.y = 7
		mainWindow.textLine2.SetPosition(self.toolTipWidth - 31, 7)
		mainWindow.SetPosition(10, self.toolTipHeight)

		mainWindow.toolTipHeight = self.toolTipHeight

		self.toolTipHeight += 28

		(textWidth1, textHeight) = mainWindow.textLine.GetTextSize()
		(textWidth2, textHeight) = mainWindow.textLine2.GetTextSize()
		textWidth = textWidth1 + textWidth2
		textWidth += 40
		textHeight += 5

		if textWidth < 250:
			textWidth = 250

		mainWindow.items = []
		for needitem in itemtable:
			# item.SelectItem(needitem[0])
			# icon = ui.ExpandedImageBox()
			# icon.SetParent(mainWindow)
			# icon.y = 26 + len(mainWindow.items)*24
			# icon.SetPosition(self.toolTipWidth-50,26 + len(mainWindow.items)*24)
			# icon.LoadImage(item.GetIconImageFileName())
			# icon.SetScale(0.6,0.6)
			# icon.SetSize(0,0)
			# icon.Show()
			row = []
			textLine = ui.TextLine()
			textLine.SetParent(mainWindow)
			textLine.SetFontName(self.defFontName)
			textLine.SetPackedFontColor(col2)
			textLine.SetText(str(needitem[0]))
			textLine.SetOutline()
			textLine.SetFeather(False)
			# textLine.SetHorizontalAlignRight()
			textLine.Show()
			textLine.y = 8 + 21 + len(mainWindow.items) * 24
			textLine.SetPosition(10, 8 + 21 + len(mainWindow.items) * 24)
			textLine.Show()
			row.append(textLine)
			textLine = ui.TextLine()
			textLine.SetParent(mainWindow)
			textLine.SetFontName(self.defFontName)
			textLine.SetPackedFontColor(col2)
			textLine.SetText(str(needitem[1]))
			textLine.SetOutline()
			textLine.SetFeather(False)
			textLine.SetHorizontalAlignRight()
			textLine.Show()
			textLine.y = 8 + 21 + len(mainWindow.items) * 24
			textLine.SetPosition(self.toolTipWidth - 20 - 10, 8 + 21 + len(mainWindow.items) * 24)
			textLine.Show()
			row.append(textLine)
			# row.append(icon)
			# row.append(textLine2)
			mainWindow.items.append(row)
			self.toolTipHeight += 24

		mainWindow.width = self.toolTipWidth - 20
		mainWindow.rightbar2.SetPosition(mainWindow.width - 10, 0)
		mainWindow.center.SetPercentage(mainWindow.width - 20, 10)

		self.childrenList.append(mainWindow)

		mainWindow.textLine.SetPosition((self.toolTipWidth - 20) / 2, 7)

		if self.toolTipWidth < textWidth:
			oldSize = self.toolTipWidth
			mainWindow.width = textWidth - 20
			self.toolTipWidth = textWidth
			if True:
				self.Rebuild(textWidth, oldSize)

		self.ResizeToolTip()

		return mainWindow

	def AppendPriceBySecondaryCoin(self, price):
		self.AppendSpace(5)
		self.AppendTextLine("|cFFbfa66a"+localeInfo.TOOLTIP_BUYPRICE % (localeInfo.NumberToGoldNotText(price))+" {}".format(emoji.AppendEmoji("icon/emoji/achievement_point.tga")), self.GetPriceColor(price))

	if app.ENABLE_CHEQUE_SYSTEM:
		def AppendSellingPrice(self, price, cheque = 0, isPrivateShopBuilder = False):
			if item.IsAntiFlag(item.ITEM_ANTIFLAG_SELL) and \
				not isPrivateShopBuilder:		
				self.AppendTextLine(localeInfo.TOOLTIP_ANTI_SELL, self.DISABLE_COLOR)
				self.AppendSpace(5)
			else:
				
				if cheque > 0:
					self.AppendTextLine(localeInfo.CHEQUE_SYSTEM_WON % (str(cheque)), grp.GenerateColor(0.0, 0.8470, 1.0, 1.0))
				
				if price >= 0:
					self.AppendTextLine(localeInfo.CHEQUE_SYSTEM_YANG % (localeInfo.NumberToMoneyString(price)), self.GetPriceColor(price))
				
				self.AppendSpace(5)
	else:
		def AppendSellingPrice(self, price):
			if item.IsAntiFlag(item.ITEM_ANTIFLAG_SELL):			
				self.AppendTextLine(localeInfo.TOOLTIP_ANTI_SELL, self.DISABLE_COLOR)
				self.AppendSpace(5)
			else:
				self.AppendTextLine(localeInfo.TOOLTIP_SELLPRICE % (localeInfo.NumberToMoneyString(price)), self.GetPriceColor(price))
				self.AppendSpace(5)

	def AppendOfflineShopSellingPrice(self, price, price2):
		self.AppendTextLine(localeInfo.NumberToMoneyString(price), self.GetPriceColor(price))
		self.AppendTextLine(localeInfo.NumberToCheque(price2), grp.GenerateColor(184./255, 184./255, 184./255, 1))

	if app.ENABLE_CHEQUE_SYSTEM:
		def AppendCheque(self, price):
			if price <=0:
				return
			self.AppendSpace(5)
			self.AppendTextLine(localeInfo.NumberToGoldNotText(price)+" {}".format(emoji.AppendEmoji("icon/emoji/cheque_icon.png")), grp.GenerateColor(184./255, 184./255, 184./255, 1))

	def AppendMetinInformation(self):
		if constInfo.ENABLE_FULLSTONE_DETAILS:
			for i in xrange(item.ITEM_APPLY_MAX_NUM):
				(affectType, affectValue) = item.GetAffect(i)
				affectString = self.__GetAffectString(affectType, affectValue)
				if affectString:
					self.AppendSpace(5)
					self.AppendTextLine(affectString, self.GetChangeTextLineColor(affectValue))

	def AppendMetinWearInformation(self):

		self.AppendSpace(5)
		self.AppendTextLine(localeInfo.TOOLTIP_SOCKET_REFINABLE_ITEM, self.NORMAL_COLOR)

		flagList = (item.IsWearableFlag(item.WEARABLE_BODY),
					item.IsWearableFlag(item.WEARABLE_HEAD),
					item.IsWearableFlag(item.WEARABLE_FOOTS),
					item.IsWearableFlag(item.WEARABLE_WRIST),
					item.IsWearableFlag(item.WEARABLE_WEAPON),
					item.IsWearableFlag(item.WEARABLE_NECK),
					item.IsWearableFlag(item.WEARABLE_EAR),
					item.IsWearableFlag(item.WEARABLE_UNIQUE),
					item.IsWearableFlag(item.WEARABLE_SHIELD),
					item.IsWearableFlag(item.WEARABLE_ARROW))

		wearNames = ""
		for i in xrange(self.WEAR_COUNT):

			name = self.WEAR_NAMES[i]
			flag = flagList[i]

			if flag:
				wearNames += "  "
				wearNames += name

		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetFontName(self.defFontName)
		textLine.SetPosition(self.toolTipWidth/2, self.toolTipHeight)
		textLine.SetHorizontalAlignCenter()
		textLine.SetPackedFontColor(self.NORMAL_COLOR)
		textLine.SetText(wearNames)
		textLine.Show()
		self.childrenList.append(textLine)

		self.toolTipHeight += self.TEXT_LINE_HEIGHT
		self.ResizeToolTip()

	def GetMetinSocketType(self, number):
		if player.METIN_SOCKET_TYPE_NONE == number:
			return player.METIN_SOCKET_TYPE_NONE
		elif player.METIN_SOCKET_TYPE_SILVER == number:
			return player.METIN_SOCKET_TYPE_SILVER
		elif player.METIN_SOCKET_TYPE_GOLD == number:
			return player.METIN_SOCKET_TYPE_GOLD
		else:
			item.SelectItem(number)
			if item.METIN_NORMAL == item.GetItemSubType():
				return player.METIN_SOCKET_TYPE_SILVER
			elif item.METIN_GOLD == item.GetItemSubType():
				return player.METIN_SOCKET_TYPE_GOLD
			elif "USE_PUT_INTO_ACCESSORY_SOCKET" == item.GetUseType(number):
				return player.METIN_SOCKET_TYPE_SILVER
			elif "USE_PUT_INTO_RING_SOCKET" == item.GetUseType(number):
				return player.METIN_SOCKET_TYPE_SILVER
			elif "USE_PUT_INTO_BELT_SOCKET" == item.GetUseType(number):
				return player.METIN_SOCKET_TYPE_SILVER

		return player.METIN_SOCKET_TYPE_NONE

	def GetMetinItemIndex(self, number):
		if player.METIN_SOCKET_TYPE_SILVER == number:
			return 0
		if player.METIN_SOCKET_TYPE_GOLD == number:
			return 0

		return number

	def __AppendAccessoryMetinSlotInfo(self, metinSlot, mtrlVnum):
		ACCESSORY_SOCKET_MAX_SIZE = 3

		cur=min(metinSlot[0], ACCESSORY_SOCKET_MAX_SIZE)
		end=min(metinSlot[1], ACCESSORY_SOCKET_MAX_SIZE)

		affectType1, affectValue1 = item.GetAffect(0)
		affectList1=[0, max(1, affectValue1*10/100), max(2, affectValue1*20/100), max(3, affectValue1*40/100)]

		affectType2, affectValue2 = item.GetAffect(1)
		affectList2=[0, max(1, affectValue2*10/100), max(2, affectValue2*20/100), max(3, affectValue2*40/100)]

		affectType3, affectValue3 = item.GetAffect(2)
		affectList3=[0, max(1, affectValue3*10/100), max(2, affectValue3*20/100), max(3, affectValue3*40/100)]

		mtrlPos=0
		mtrlList=[mtrlVnum]*cur+[player.METIN_SOCKET_TYPE_SILVER]*(end-cur)
		for mtrl in mtrlList:
			affectString1 = self.__GetAffectString(affectType1, affectList1[mtrlPos+1]-affectList1[mtrlPos])
			affectString2 = self.__GetAffectString(affectType2, affectList2[mtrlPos+1]-affectList2[mtrlPos])
			affectString3 = self.__GetAffectString(affectType3, affectList3[mtrlPos+1]-affectList3[mtrlPos])

			leftTime = 0
			if cur == mtrlPos+1:
				leftTime=metinSlot[2]

			self.__AppendMetinSlotInfo_AppendMetinSocketData(mtrlPos, mtrl, affectString1, affectString2, affectString3, leftTime)
			mtrlPos+=1

	def __AppendMetinSlotInfo(self, metinSlot):
		if self.__AppendMetinSlotInfo_IsEmptySlotList(metinSlot):
			return
		
		if app.ENABLE_EXTENDED_SOCKETS:
			for i in xrange(player.ITEM_STONES_MAX_NUM):
				self.__AppendMetinSlotInfo_AppendMetinSocketData(i, metinSlot[i])
		else:
			for i in xrange(player.METIN_SOCKET_MAX_NUM):
				self.__AppendMetinSlotInfo_AppendMetinSocketData(i, metinSlot[i])			

	def __AppendMetinSlotInfo_IsEmptySlotList(self, metinSlot):
		if 0 == metinSlot:
			return 1
			
		if app.ENABLE_EXTENDED_SOCKETS:
			for i in xrange(player.ITEM_STONES_MAX_NUM):
				metinSlotData=metinSlot[i]
				if 0 != self.GetMetinSocketType(metinSlotData):
					if 0 != self.GetMetinItemIndex(metinSlotData):
						return 0
		else:
			for i in xrange(player.METIN_SOCKET_MAX_NUM):
				metinSlotData=metinSlot[i]
				if 0 != self.GetMetinSocketType(metinSlotData):
					if 0 != self.GetMetinItemIndex(metinSlotData):
						return 0

		return 1

	def __AppendMetinSlotInfo_AppendMetinSocketData(self, index, metinSlotData, custumAffectString="", custumAffectString2="", custumAffectString3="", leftTime=0):

		slotType = self.GetMetinSocketType(metinSlotData)
		itemIndex = self.GetMetinItemIndex(metinSlotData)

		if 0 == slotType:
			return

		self.AppendSpace(5)

		slotImage = ui.ImageBox()
		slotImage.SetParent(self)
		slotImage.Show()

		nameTextLine = ui.TextLine()
		nameTextLine.SetParent(self)
		nameTextLine.SetFontName(self.defFontName)
		nameTextLine.SetPackedFontColor(self.NORMAL_COLOR)
		nameTextLine.SetOutline()
		nameTextLine.SetFeather()
		nameTextLine.SetHorizontalAlignCenter()
		nameTextLine.Show()

		self.childrenList.append(nameTextLine)

		if player.METIN_SOCKET_TYPE_SILVER == slotType:
			slotImage.LoadImage("d:/ymir work/ui/game/windows/metin_slot_silver.sub")
		elif player.METIN_SOCKET_TYPE_GOLD == slotType:
			slotImage.LoadImage("d:/ymir work/ui/game/windows/metin_slot_gold.sub")

		self.childrenList.append(slotImage)

		slotImage.SetPosition(4, self.toolTipHeight-1)
		nameTextLine.SetPosition(0, self.toolTipHeight + 2)

		metinImage = ui.ImageBox()
		metinImage.SetParent(self)
		metinImage.Show()
		self.childrenList.append(metinImage)

		if itemIndex:

			item.SelectItem(itemIndex)

			try:
				metinImage.LoadImage(item.GetIconImageFileName())
			except:
				dbg.TraceError("ItemToolTip.__AppendMetinSocketData() - Failed to find image file %d:%s" %
					(itemIndex, item.GetIconImageFileName())
				)

			nameTextLine.SetText(item.GetItemName())

			affectTextLine = ui.TextLine()
			affectTextLine.SetParent(self)
			affectTextLine.SetFontName(self.defFontName)
			affectTextLine.SetPackedFontColor(self.POSITIVE_COLOR)
			affectTextLine.SetOutline()
			affectTextLine.SetFeather()
			affectTextLine.SetHorizontalAlignCenter()
			affectTextLine.Show()

			metinImage.SetPosition(6, self.toolTipHeight)
			affectTextLine.SetPosition(0, self.toolTipHeight + 16 + 2)

			if custumAffectString:
				affectTextLine.SetText(custumAffectString)
			elif itemIndex!=constInfo.ERROR_METIN_STONE:
				affectType, affectValue = item.GetAffect(0)
				affectString = self.__GetAffectString(affectType, affectValue)
				if affectString:
					affectTextLine.SetText(affectString)
			else:
				affectTextLine.SetText(localeInfo.TOOLTIP_APPLY_NOAFFECT)

			self.childrenList.append(affectTextLine)

			if constInfo.ENABLE_FULLSTONE_DETAILS and (not custumAffectString2) and (itemIndex!=constInfo.ERROR_METIN_STONE):
				custumAffectString2 = self.__GetAffectString(*item.GetAffect(1))

			if custumAffectString2:
				affectTextLine = ui.TextLine()
				affectTextLine.SetParent(self)
				affectTextLine.SetFontName(self.defFontName)
				affectTextLine.SetPackedFontColor(self.POSITIVE_COLOR)
				affectTextLine.SetPosition(0, self.toolTipHeight + 16 + 2 + 16 + 2)
				affectTextLine.SetHorizontalAlignCenter()
				affectTextLine.SetOutline()
				affectTextLine.SetFeather()
				affectTextLine.Show()
				affectTextLine.SetText(custumAffectString2)
				self.childrenList.append(affectTextLine)
				self.toolTipHeight += 16 + 2

			if constInfo.ENABLE_FULLSTONE_DETAILS and (not custumAffectString3) and (itemIndex!=constInfo.ERROR_METIN_STONE):
				custumAffectString3 = self.__GetAffectString(*item.GetAffect(2))

			if custumAffectString3:
				affectTextLine = ui.TextLine()
				affectTextLine.SetParent(self)
				affectTextLine.SetFontName(self.defFontName)
				affectTextLine.SetPackedFontColor(self.POSITIVE_COLOR)
				affectTextLine.SetPosition(0, self.toolTipHeight + 16 + 2 + 16 + 2)
				affectTextLine.SetHorizontalAlignCenter()
				affectTextLine.SetOutline()
				affectTextLine.SetFeather()
				affectTextLine.Show()
				affectTextLine.SetText(custumAffectString3)
				self.childrenList.append(affectTextLine)
				self.toolTipHeight += 16 + 2

			if 0 != leftTime:
				timeText = (emoji.AppendEmoji("icon/emoji/image-example-007.png") + " "+localeInfo.LEFT_TIME + ": " + localeInfo.SecondToDHM(leftTime))


				timeTextLine = ui.TextLine()
				timeTextLine.SetParent(self)
				timeTextLine.SetFontName(self.defFontName)
				timeTextLine.SetPosition(0, self.toolTipHeight + 16 + 2 + 16 + 2)
				timeTextLine.SetHorizontalAlignCenter()
				timeTextLine.SetOutline()
				timeTextLine.SetFeather()
				timeTextLine.Show()
				timeTextLine.SetText(timeText)
				self.childrenList.append(timeTextLine)
				self.toolTipHeight += 16 + 2

		else:
			nameTextLine.SetText(localeInfo.TOOLTIP_SOCKET_EMPTY)

		self.toolTipHeight += 35
		self.toolTipWidth += 25
		self.ResizeToolTip()

	def __AppendFishInfo(self, size, price):
		self.AppendSpace(5)
		if size > 0:
			self.AppendTextLine(localeInfo.TOOLTIP_FISH_LEN % (float(size) / 100.0), self.NORMAL_COLOR)
		if price > 0:
			self.AppendTextLine(localeInfo.TOOLTIP_FISH_PRICE % localeInfo.NumberToMoneyString(price), self.HIGH_PRICE_COLOR)

	def AppendUniqueItemLastTime(self, restMin):
		if restMin > 0:
			restSecond = restMin*60
			self.AppendSpace(5)
			self.AutoAppendTextLine(localeInfo.LEFTOVER_TIME, self.CONDITION_COLOR)
			self.AutoAppendTextLine(emoji.AppendEmoji("icon/emoji/image-example-007.png")+" "+localeInfo.SecondToDHM(restSecond), self.NORMAL_COLOR)

	def AppendMallItemLastTime(self, endTime):
		if endTime > 0:
			leftSec = max(0, endTime - app.GetGlobalTimeStamp())
			self.AppendSpace(5)
			self.AutoAppendTextLine(localeInfo.LEFTOVER_TIME, self.CONDITION_COLOR)
			self.AutoAppendTextLine(emoji.AppendEmoji("icon/emoji/image-example-007.png")+" "+localeInfo.SecondToDHM(leftSec), self.NORMAL_COLOR)

	if app.ENABLE_VS_COSTUME_BONUSES:
		def AppendCostumeBonuses(self, endTime):
			if endTime > 0:
				self.AppendSpace(5)
				self.AutoAppendTextLine(localeInfo.LEFTOVER_BONUS_TIME, self.CONDITION_COLOR)
				self.AutoAppendTextLine(emoji.AppendEmoji("icon/emoji/image-example-007.png")+" "+localeInfo.SecondToDHMS(endTime), self.NORMAL_COLOR)

	def AppendTimerBasedOnWearLastTime(self, metinSlot):
		if 0 == metinSlot[0]:
			self.AppendSpace(5)
			self.AppendTextLine(localeInfo.CANNOT_USE, self.DISABLE_COLOR)
		else:
			endTime = app.GetGlobalTimeStamp() + metinSlot[0]
			self.AppendMallItemLastTime(endTime)

	def AppendRealTimeStartFirstUseLastTime(self, item, metinSlot, limitIndex):
		useCount = metinSlot[1]
		endTime = metinSlot[0]

		if 0 == useCount:
			if 0 == endTime:
				(limitType, limitValue) = item.GetLimit(limitIndex)
				endTime = limitValue

			endTime += app.GetGlobalTimeStamp()

		self.AppendMallItemLastTime(endTime)

	if app.ENABLE_ACCE_COSTUME_SYSTEM:
		def SetAcceResultItem(self, slotIndex, window_type = player.INVENTORY):
			(itemVnum, MinAbs, MaxAbs) = acce.GetResultItem()
			if not itemVnum:
				return

			self.ClearToolTip()

			metinSlot 	= [player.GetItemMetinSocket(window_type, slotIndex, i) 	for i in xrange(player.METIN_SOCKET_MAX_NUM)	]
			attrSlot 	= [player.GetItemAttribute(window_type, slotIndex, i) 		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)	]

			item.SelectItem(itemVnum)
			itemType = item.GetItemType()
			itemSubType = item.GetItemSubType()
			if itemType != item.ITEM_TYPE_COSTUME and itemSubType != item.COSTUME_TYPE_ACCE:
				return

			absChance = MaxAbs
			itemDesc = item.GetItemDescription()
			self.__AdjustMaxWidth(attrSlot, itemDesc)
			self.__SetItemTitle(itemVnum, metinSlot, attrSlot)
			self.AppendDescription(itemDesc, 26)
			self.AppendDescription(item.GetItemSummary(), 26, self.CONDITION_COLOR)
			self.__AppendLimitInformation()

			if MinAbs == MaxAbs:
				self.AppendTextLine(localeInfo.ACCE_ABSORB_CHANCE % (MinAbs), self.CONDITION_COLOR)
			else:
				self.AppendTextLine(localeInfo.ACCE_ABSORB_CHANCE2 % (MinAbs, MaxAbs), self.CONDITION_COLOR)

			itemAbsorbedVnum = int(metinSlot[acce.ABSORBED_SOCKET])
			if itemAbsorbedVnum:
				item.SelectItem(itemAbsorbedVnum)
				if item.GetItemType() == item.ITEM_TYPE_WEAPON:
					if item.GetItemSubType() == item.WEAPON_FAN:
						self.__AppendMagicAttackInfo(absChance)
						item.SelectItem(itemAbsorbedVnum)
						self.__AppendAttackPowerInfo(absChance)
					else:
						self.__AppendAttackPowerInfo(absChance)
						item.SelectItem(itemAbsorbedVnum)
						self.__AppendMagicAttackInfo(absChance)
				elif item.GetItemType() == item.ITEM_TYPE_ARMOR:
					defGrade = item.GetValue(1)
					defBonus = item.GetValue(5) * 2
					defGrade = self.CalcAcceValue(defGrade, absChance)
					defBonus = self.CalcAcceValue(defBonus, absChance)

					if defGrade > 0:
						self.AppendSpace(5)
						self.AppendTextLine(localeInfo.TOOLTIP_ITEM_DEF_GRADE % (defGrade + defBonus), self.GetChangeTextLineColor(defGrade))

					item.SelectItem(itemAbsorbedVnum)
					self.__AppendMagicDefenceInfo(absChance)

				item.SelectItem(itemAbsorbedVnum)
				for i in xrange(item.ITEM_APPLY_MAX_NUM):
					(affectType, affectValue) = item.GetAffect(i)
					affectValue = self.CalcAcceValue(affectValue, absChance)
					affectString = self.__GetAffectString(affectType, affectValue)
					if (not app.USE_ACCE_ABSORB_WITH_NO_NEGATIVE_BONUS and affectString and affectValue != 0)\
							or (app.USE_ACCE_ABSORB_WITH_NO_NEGATIVE_BONUS and affectString and affectValue > 0):
						self.AppendTextLine(affectString, self.GetChangeTextLineColor(affectValue))

					item.SelectItem(itemAbsorbedVnum)

			item.SelectItem(itemVnum)
			self.__AppendAttributeInformation(attrSlot, MaxAbs)

			if itemAbsorbedVnum:
				self.AppendSpace(5)
				self.AppendHorizontalLine()
				self.AppendSpace(5)
				self.AppendTextLine(localeInfo.SASH_ABSORBED_ITEM, self.CONDITION_COLOR)
				item.SelectItem(itemAbsorbedVnum)
				self.AppendTextLine(item.GetItemName(), self.CONDITION_COLOR)

			self.AppendWearableInformation()
			self.ShowToolTip()

		def SetAcceResultAbsItem(self, slotIndex1, slotIndex2, window_type = player.INVENTORY):
			itemVnumAcce = player.GetItemIndex(window_type, slotIndex1)
			itemVnumTarget = player.GetItemIndex(window_type, slotIndex2)
			if not itemVnumAcce or not itemVnumTarget:
				return

			self.ClearToolTip()

			item.SelectItem(itemVnumAcce)
			itemType = item.GetItemType()
			itemSubType = item.GetItemSubType()
			if itemType != item.ITEM_TYPE_COSTUME and itemSubType != item.COSTUME_TYPE_ACCE:
				return

			metinSlot = [player.GetItemMetinSocket(window_type, slotIndex1, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
			attrSlot = [player.GetItemAttribute(window_type, slotIndex2, i) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]

			itemDesc = item.GetItemDescription()
			self.__AdjustMaxWidth(attrSlot, itemDesc)
			self.__SetItemTitle(itemVnumAcce, metinSlot, attrSlot)
			self.AppendDescription(itemDesc, 26)
			self.AppendDescription(item.GetItemSummary(), 26, self.CONDITION_COLOR)
			item.SelectItem(itemVnumAcce)
			self.__AppendLimitInformation()

			self.AppendTextLine(localeInfo.ACCE_ABSORB_CHANCE % (metinSlot[acce.ABSORPTION_SOCKET]), self.CONDITION_COLOR)

			itemAbsorbedVnum = itemVnumTarget
			item.SelectItem(itemAbsorbedVnum)
			if item.GetItemType() == item.ITEM_TYPE_WEAPON:
				if item.GetItemSubType() == item.WEAPON_FAN:
					self.__AppendMagicAttackInfo(metinSlot[acce.ABSORPTION_SOCKET])
					item.SelectItem(itemAbsorbedVnum)
					self.__AppendAttackPowerInfo(metinSlot[acce.ABSORPTION_SOCKET])
				else:
					self.__AppendAttackPowerInfo(metinSlot[acce.ABSORPTION_SOCKET])
					item.SelectItem(itemAbsorbedVnum)
					self.__AppendMagicAttackInfo(metinSlot[acce.ABSORPTION_SOCKET])
			elif item.GetItemType() == item.ITEM_TYPE_ARMOR:
				defGrade = item.GetValue(1)
				defBonus = item.GetValue(5) * 2
				defGrade = self.CalcAcceValue(defGrade, metinSlot[acce.ABSORPTION_SOCKET])
				defBonus = self.CalcAcceValue(defBonus, metinSlot[acce.ABSORPTION_SOCKET])

				if defGrade > 0:
					self.AppendSpace(5)
					self.AppendTextLine(localeInfo.TOOLTIP_ITEM_DEF_GRADE % (defGrade + defBonus), self.GetChangeTextLineColor(defGrade))

				item.SelectItem(itemAbsorbedVnum)
				self.__AppendMagicDefenceInfo(metinSlot[acce.ABSORPTION_SOCKET])

			item.SelectItem(itemAbsorbedVnum)
			for i in xrange(item.ITEM_APPLY_MAX_NUM):
				(affectType, affectValue) = item.GetAffect(i)
				affectValue = self.CalcAcceValue(affectValue, metinSlot[acce.ABSORPTION_SOCKET])
				affectString = self.__GetAffectString(affectType, affectValue)
				if (not app.USE_ACCE_ABSORB_WITH_NO_NEGATIVE_BONUS and affectString and affectValue != 0)\
						or (app.USE_ACCE_ABSORB_WITH_NO_NEGATIVE_BONUS and affectString and affectValue > 0):
					self.AppendTextLine(affectString, self.GetChangeTextLineColor(affectValue))

				item.SelectItem(itemAbsorbedVnum)

			item.SelectItem(itemAbsorbedVnum)
			for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				type = attrSlot[i][0]
				value = attrSlot[i][1]
				if not value:
					continue

				affectValue = self.CalcAcceValue(value, metinSlot[acce.ABSORPTION_SOCKET])
				affectString = self.__GetAffectString(type, affectValue)
				if (not app.USE_ACCE_ABSORB_WITH_NO_NEGATIVE_BONUS and affectString and affectValue != 0)\
						or (app.USE_ACCE_ABSORB_WITH_NO_NEGATIVE_BONUS and affectString and affectValue > 0):
					affectColor = self.__GetAttributeColor(i, affectValue)
					self.AppendTextLine(affectString, affectColor)

				item.SelectItem(itemAbsorbedVnum)

			item.SelectItem(itemVnumAcce)
			self.AppendSpace(5)
			self.AppendTextLine(localeInfo.TOOLTIP_ITEM_WEARABLE_JOB, self.NORMAL_COLOR)

			item.SelectItem(itemVnumAcce)
			flagList = (
						not item.IsAntiFlag(item.ITEM_ANTIFLAG_WARRIOR),
						not item.IsAntiFlag(item.ITEM_ANTIFLAG_ASSASSIN),
						not item.IsAntiFlag(item.ITEM_ANTIFLAG_SURA),
						not item.IsAntiFlag(item.ITEM_ANTIFLAG_SHAMAN)
			)

			if app.ENABLE_WOLFMAN_CHARACTER:
				flagList += (not item.IsAntiFlag(item.ITEM_ANTIFLAG_WOLFMAN),)

			characterNames = ""
			for i in xrange(self.CHARACTER_COUNT):
				name = self.CHARACTER_NAMES[i]
				flag = flagList[i]
				if flag:
					characterNames += " "
					characterNames += name

			textLine = self.AppendTextLine(characterNames, self.NORMAL_COLOR, True)
			textLine.SetFeather()

			item.SelectItem(itemVnumAcce)
			if item.IsAntiFlag(item.ITEM_ANTIFLAG_MALE):
				textLine = self.AppendTextLine(localeInfo.FOR_FEMALE, self.NORMAL_COLOR, True)
				textLine.SetFeather()

			if item.IsAntiFlag(item.ITEM_ANTIFLAG_FEMALE):
				textLine = self.AppendTextLine(localeInfo.FOR_MALE, self.NORMAL_COLOR, True)
				textLine.SetFeather()

			self.ShowToolTip()

	if app.ENABLE_AURA_SYSTEM:
		def SetAuraResultItem(self, slotIndex, window_type = player.INVENTORY):
			(itemVnum, MinAbs, MaxAbs) = aura.GetResultItem()
			if not itemVnum:
				return
			
			self.ClearToolTip()
			
			metinSlot = [player.GetItemMetinSocket(window_type, slotIndex, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
			attrSlot = [player.GetItemAttribute(window_type, slotIndex, i) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]
			
			item.SelectItem(itemVnum)
			itemType = item.GetItemType()
			itemSubType = item.GetItemSubType()
			if itemType != item.ITEM_TYPE_COSTUME and itemSubType != item.COSTUME_TYPE_AURA:
				return
			
			absChance = float(MinAbs) / 10.0
			itemDesc = item.GetItemDescription()
			self.__AdjustMaxWidth(attrSlot, itemDesc)
			self.__SetItemTitle(itemVnum, metinSlot, attrSlot)
			self.AppendDescription(itemDesc, 26)
			self.AppendDescription(item.GetItemSummary(), 26, self.CONDITION_COLOR)
			self.__AppendLimitInformation()
			
			self.AppendTextLine(localeInfo.AURA_LEVEL_STEP % (int(metinSlot[aura.LEVEL_SOCKET]), int(metinSlot[aura.LEVEL_SOCKET])), self.CONDITION_COLOR)
			self.AppendTextLine(localeInfo.AURA_DRAIN_PER % (absChance), self.CONDITION_COLOR)
			
			itemAbsorbedVnum = int(metinSlot[aura.ABSORBED_SOCKET])
			if itemAbsorbedVnum:
				item.SelectItem(itemAbsorbedVnum)
				if item.GetItemType() == item.ITEM_TYPE_WEAPON:
					if item.GetItemSubType() == item.WEAPON_FAN:
						self.__AppendMagicAttackInfo(absChance)
						item.SelectItem(itemAbsorbedVnum)
						self.__AppendAttackPowerInfo(absChance)
					else:
						self.__AppendAttackPowerInfo(absChance)
						item.SelectItem(itemAbsorbedVnum)
						self.__AppendMagicAttackInfo(absChance)
				elif item.GetItemType() == item.ITEM_TYPE_ARMOR:
					defGrade = item.GetValue(1)
					defBonus = item.GetValue(5) * 2
					defGrade = self.CalcSashValue(defGrade, absChance)
					defBonus = self.CalcSashValue(defBonus, absChance)
					
					if defGrade > 0:
						self.AppendSpace(5)
						self.AppendTextLine(localeInfo.TOOLTIP_ITEM_DEF_GRADE % (defGrade + defBonus), self.GetChangeTextLineColor(defGrade))
					
					item.SelectItem(itemAbsorbedVnum)
					self.__AppendMagicDefenceInfo(absChance)
				
				item.SelectItem(itemAbsorbedVnum)
				for i in xrange(item.ITEM_APPLY_MAX_NUM):
					(affectType, affectValue) = item.GetAffect(i)
					affectValue = self.CalcSashValue(affectValue, absChance)
					affectString = self.__GetAffectString(affectType, affectValue)
					if affectString and affectValue > 0:
						self.AppendTextLine(affectString, self.GetChangeTextLineColor(affectValue))
					
					item.SelectItem(itemAbsorbedVnum)
				
			item.SelectItem(itemVnum)
			self.__AppendAttributeInformation(attrSlot, MaxAbs)
			
			self.AppendWearableInformation()
			self.ShowToolTip()
			
		def SetAuraResultAbsItem(self, slotIndex1, slotIndex2, window_type = player.INVENTORY):
			itemVnumAura = player.GetItemIndex(window_type, slotIndex1)
			itemVnumTarget = player.GetItemIndex(window_type, slotIndex2)
			if not itemVnumAura or not itemVnumTarget:
				return
			
			self.ClearToolTip()
			
			item.SelectItem(itemVnumAura)
			itemType = item.GetItemType()
			itemSubType = item.GetItemSubType()
			if itemType != item.ITEM_TYPE_COSTUME and itemSubType != item.COSTUME_TYPE_AURA:
				return
			
			metinSlot = [player.GetItemMetinSocket(window_type, slotIndex1, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
			attrSlot = [player.GetItemAttribute(window_type, slotIndex2, i) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]
			
			itemDesc = item.GetItemDescription()
			self.__AdjustMaxWidth(attrSlot, itemDesc)
			self.__SetItemTitle(itemVnumAura, metinSlot, attrSlot)
			self.AppendDescription(itemDesc, 26)
			self.AppendDescription(item.GetItemSummary(), 26, self.CONDITION_COLOR)
			item.SelectItem(itemVnumAura)
			self.__AppendLimitInformation()
			
			absChance = float(metinSlot[aura.ABSORPTION_SOCKET]) / 10.0
			
			self.AppendTextLine(localeInfo.AURA_LEVEL_STEP % (int(metinSlot[aura.LEVEL_SOCKET]), int(metinSlot[aura.LEVEL_SOCKET])), self.CONDITION_COLOR)
			self.AppendTextLine(localeInfo.AURA_DRAIN_PER % (absChance), self.CONDITION_COLOR)
			
			itemAbsorbedVnum = itemVnumTarget
			item.SelectItem(itemAbsorbedVnum)
			if item.GetItemType() == item.ITEM_TYPE_WEAPON:
				if item.GetItemSubType() == item.WEAPON_FAN:
					self.__AppendMagicAttackInfo(absChance)
					item.SelectItem(itemAbsorbedVnum)
					self.__AppendAttackPowerInfo(absChance)
				else:
					self.__AppendAttackPowerInfo(absChance)
					item.SelectItem(itemAbsorbedVnum)
					self.__AppendMagicAttackInfo(absChance)
			elif item.GetItemType() == item.ITEM_TYPE_ARMOR:
				defGrade = item.GetValue(1)
				defBonus = item.GetValue(5) * 2
				defGrade = self.CalcSashValue(defGrade, absChance)
				defBonus = self.CalcSashValue(defBonus, absChance)
				
				if defGrade > 0:
					self.AppendSpace(5)
					self.AppendTextLine(localeInfo.TOOLTIP_ITEM_DEF_GRADE % (defGrade + defBonus), self.GetChangeTextLineColor(defGrade))
				
				item.SelectItem(itemAbsorbedVnum)
				self.__AppendMagicDefenceInfo(absChance)
			
			item.SelectItem(itemAbsorbedVnum)
			for i in xrange(item.ITEM_APPLY_MAX_NUM):
				(affectType, affectValue) = item.GetAffect(i)
				affectValue = self.CalcSashValue(affectValue, absChance)
				affectString = self.__GetAffectString(affectType, affectValue)
				if affectString and affectValue > 0:
					self.AppendTextLine(affectString, self.GetChangeTextLineColor(affectValue))
				
				item.SelectItem(itemAbsorbedVnum)
			
			item.SelectItem(itemAbsorbedVnum)
			for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				type = attrSlot[i][0]
				value = attrSlot[i][1]
				if not value:
					continue
				
				value = self.CalcSashValue(value, absChance)
				affectString = self.__GetAffectString(type, value)
				if affectString and value > 0:
					affectColor = self.__GetAttributeColor(i, value, type)
					self.AppendTextLine(affectString, affectColor)
				
				item.SelectItem(itemAbsorbedVnum)
			
			item.SelectItem(itemVnumAura)
			self.AppendSpace(5)
			self.AppendTextLine(localeInfo.TOOLTIP_ITEM_WEARABLE_JOB, self.NORMAL_COLOR)
			
			item.SelectItem(itemVnumAura)
			flagList = (
						not item.IsAntiFlag(item.ITEM_ANTIFLAG_WARRIOR),
						not item.IsAntiFlag(item.ITEM_ANTIFLAG_ASSASSIN),
						not item.IsAntiFlag(item.ITEM_ANTIFLAG_SURA),
						not item.IsAntiFlag(item.ITEM_ANTIFLAG_SHAMAN)
			)
			flagList += (not item.IsAntiFlag(item.ITEM_ANTIFLAG_WOLFMAN),)
			
			characterNames = ""
			for i in xrange(self.CHARACTER_COUNT):
				name = self.CHARACTER_NAMES[i]
				flag = flagList[i]
				if flag:
					characterNames += " "
					characterNames += name
			
			textLine = self.AppendTextLine(characterNames, self.NORMAL_COLOR, True)
			textLine.SetFeather()
			
			item.SelectItem(itemVnumAura)
			if item.IsAntiFlag(item.ITEM_ANTIFLAG_MALE):
				textLine = self.AppendTextLine(localeInfo.FOR_FEMALE, self.NORMAL_COLOR, True)
				textLine.SetFeather()
			
			if item.IsAntiFlag(item.ITEM_ANTIFLAG_FEMALE):
				textLine = self.AppendTextLine(localeInfo.FOR_MALE, self.NORMAL_COLOR, True)
				textLine.SetFeather()
			
			self.ShowToolTip()

	def __ModelPreviewOpen(self, Vnum, model, Type=""):
		for i in xrange(7):
			if switchbot.IsActive(i):
				return

		if not self.modelShow:
			return
			
		RENDER_TARGET_INDEX = 1

		if not self.modelPreviewBoard:
			self.modelPreviewBoard = ui.ThinBoard()
			self.modelPreviewBoard.SetParent(self)
			self.modelPreviewBoard.SetSize(190+10, 210+30)
			self.modelPreviewBoard.SetPosition(-202, 0)
			self.modelPreviewBoard.Show()

		if not self.modelPreviewRender:
			self.modelPreviewRender = ui.RenderTarget()
			self.modelPreviewRender.SetParent(self.modelPreviewBoard)
			self.modelPreviewRender.SetSize(194, 216)
			self.modelPreviewRender.SetPosition(3, 21)
			self.modelPreviewRender.SetRenderTarget(RENDER_TARGET_INDEX)
			self.modelPreviewRender.Show()
			renderTarget.SetBackground(RENDER_TARGET_INDEX, "d:/ymir work/ui/game/myshop_deco/model_view_bg.sub")

		if not self.modelPreviewBelka:
			self.modelPreviewBelka = ui.ImageBox()
			self.modelPreviewBelka.SetParent(self.modelPreviewBoard)
			self.modelPreviewBelka.SetPosition(0,0)
			self.modelPreviewBelka.LoadImage("d:/ymir work/ui/separator_big.tga")
			self.modelPreviewBelka.Show()

		if not self.modelPreviewTextLine:
			self.modelPreviewTextLine = ui.TextLine()
			self.modelPreviewTextLine.SetParent(self.modelPreviewBoard)
			self.modelPreviewTextLine.SetFontName(localeInfo.UI_DEF_FONT_LARGE + "b")
			self.modelPreviewTextLine.SetOutline(True)
			self.modelPreviewTextLine.SetPosition(0, 3)
			UI_DEF_FONT = "Tahoma:14b"
			self.modelPreviewTextLine.SetFontName(UI_DEF_FONT)
			self.modelPreviewTextLine.SetText(localeInfo.RENDER_TARGET_PREVIEW_ITEM)
			self.modelPreviewTextLine.SetOutline()
			self.modelPreviewTextLine.SetWindowHorizontalAlignCenter()
			self.modelPreviewTextLine.SetHorizontalAlignCenter()
			self.modelPreviewTextLine.Show()

		renderTarget.SetVisibility(RENDER_TARGET_INDEX, True)

		if Type == "monster":
			renderTarget.SelectModel(RENDER_TARGET_INDEX, model)
			return

		if Type == "skill":
			renderTarget.SelectModel(RENDER_TARGET_INDEX, model)
			renderTarget.SetSkillCostume(RENDER_TARGET_INDEX, Vnum)
			return

		if Type == "body" or Type == "hair" or Type == "acce" or Type == "weapon":
			renderTarget.SelectModel(RENDER_TARGET_INDEX, model)
			if Type == "body":
				renderTarget.SetArmor(RENDER_TARGET_INDEX, Vnum)
			elif Type == "hair":
				renderTarget.SetHair(RENDER_TARGET_INDEX, item.GetValue(3))
			elif Type == "acce":
				renderTarget.SetAcce(RENDER_TARGET_INDEX, Vnum)
			elif Type == "weapon":
				renderTarget.SetWeapon(RENDER_TARGET_INDEX, Vnum)
		else:
			renderTarget.SelectModel(RENDER_TARGET_INDEX, Vnum)
			
	def __GetWeaponFromData(self, data):
		"""Get random appropriate weapon from data based on character class"""
		chrModel = player.GetRace()
		chrJob = chr.RaceToJob(chrModel)
		
		weapons = data.get("weapons", {})
		
		# Handle weapon selection based on character class - ALWAYS RANDOM
		if chrJob == 0:  # Warrior
			# Always random selection between sword and two-handed
			warrRndTypes = ["sword", "twohand"]
			rnd = app.GetRandom(0, 1)
			return weapons.get(warrRndTypes[rnd], 0)
				
		elif chrJob == 1:  # Ninja
			# Always random selection between dagger and bow
			ninjaRndTypes = ["dagger", "bow"]
			rnd = app.GetRandom(0, 1)
			return weapons.get(ninjaRndTypes[rnd], 0)
				
		elif chrJob == 2:  # Sura
			# Always random selection between sword and sura_sword (scythe)
			suraRndTypes = ["sword", "sura_sword"]
			rnd = app.GetRandom(0, 1)
			return weapons.get(suraRndTypes[rnd], 0)
			
		elif chrJob == 3:  # Shaman
			# Always random selection between bell and fan
			shamRndTypes = ["bell", "fan"]
			rnd = app.GetRandom(0, 1)
			return weapons.get(shamRndTypes[rnd], 0)
		
		return 0  # No weapon for unknown class

	def __IsBoxShowModel(self, hairVnum, bodyVnum, acceVnum, model):
		for i in xrange(7):
			if switchbot.IsActive(i):
				return

		if not self.modelShow:
			return

		RENDER_TARGET_INDEX = 1

		if not self.modelPreviewBoard:
			self.modelPreviewBoard = ui.ThinBoard()
			self.modelPreviewBoard.SetParent(self)
			self.modelPreviewBoard.SetSize(190+10, 210+30)
			self.modelPreviewBoard.SetPosition(-202, 0)
			self.modelPreviewBoard.Show()

		if not self.modelPreviewRender:
			self.modelPreviewRender = ui.RenderTarget()
			self.modelPreviewRender.SetParent(self.modelPreviewBoard)
			self.modelPreviewRender.SetSize(194, 216)
			self.modelPreviewRender.SetPosition(3, 21)
			self.modelPreviewRender.SetRenderTarget(RENDER_TARGET_INDEX)
			self.modelPreviewRender.Show()
			renderTarget.SetBackground(RENDER_TARGET_INDEX, "d:/ymir work/ui/game/myshop_deco/model_view_bg.sub")

		if not self.modelPreviewBelka:
			self.modelPreviewBelka = ui.ImageBox()
			self.modelPreviewBelka.SetParent(self.modelPreviewBoard)
			self.modelPreviewBelka.SetPosition(0,0)
			self.modelPreviewBelka.LoadImage("d:/ymir work/ui/separator_big.tga")
			self.modelPreviewBelka.Show()

		if not self.modelPreviewTextLine:
			self.modelPreviewTextLine = ui.TextLine()
			self.modelPreviewTextLine.SetParent(self.modelPreviewBoard)
			self.modelPreviewTextLine.SetFontName(localeInfo.UI_DEF_FONT_LARGE + "b")
			self.modelPreviewTextLine.SetOutline(True)
			# self.modelPreviewTextLine.SetPackedFontColor(0xffffffff)
			self.modelPreviewTextLine.SetPosition(0, 3)
			UI_DEF_FONT = "Tahoma:14b"
			self.modelPreviewTextLine.SetFontName(UI_DEF_FONT)
			self.modelPreviewTextLine.SetText(localeInfo.RENDER_TARGET_PREVIEW_ITEM)
			self.modelPreviewTextLine.SetOutline()
			self.modelPreviewTextLine.SetWindowHorizontalAlignCenter()
			self.modelPreviewTextLine.SetHorizontalAlignCenter()
			self.modelPreviewTextLine.Show()

		renderTarget.SetVisibility(RENDER_TARGET_INDEX, True)

		renderTarget.SelectModel(RENDER_TARGET_INDEX, model)
		renderTarget.SetArmor(RENDER_TARGET_INDEX, bodyVnum)
		renderTarget.SetHair(RENDER_TARGET_INDEX, hairVnum)
		renderTarget.SetAcce(RENDER_TARGET_INDEX, acceVnum)
		self.__SetWeaponForPreview(RENDER_TARGET_INDEX, itemVnum)

	def __IsBoxShowModel2(self, hairVnum, bodyVnum, weaponVnum, acceVnum, model):
		for i in xrange(7):
			if switchbot.IsActive(i):
				return

		if not self.modelShow:
			return

		RENDER_TARGET_INDEX = 1

		if not self.modelPreviewBoard:
			self.modelPreviewBoard = ui.ThinBoard()
			self.modelPreviewBoard.SetParent(self)
			self.modelPreviewBoard.SetSize(190+10, 210+30)
			self.modelPreviewBoard.SetPosition(-202, 0)
			self.modelPreviewBoard.Show()

		if not self.modelPreviewRender:
			self.modelPreviewRender = ui.RenderTarget()
			self.modelPreviewRender.SetParent(self.modelPreviewBoard)
			self.modelPreviewRender.SetSize(194, 216)
			self.modelPreviewRender.SetPosition(3, 21)
			self.modelPreviewRender.SetRenderTarget(RENDER_TARGET_INDEX)
			self.modelPreviewRender.Show()
			renderTarget.SetBackground(RENDER_TARGET_INDEX, "d:/ymir work/ui/game/myshop_deco/model_view_bg.sub")

		if not self.modelPreviewBelka:
			self.modelPreviewBelka = ui.ImageBox()
			self.modelPreviewBelka.SetParent(self.modelPreviewBoard)
			self.modelPreviewBelka.SetPosition(0,0)
			self.modelPreviewBelka.LoadImage("d:/ymir work/ui/separator_big.tga")
			self.modelPreviewBelka.Show()

		if not self.modelPreviewTextLine:
			self.modelPreviewTextLine = ui.TextLine()
			self.modelPreviewTextLine.SetParent(self.modelPreviewBoard)
			self.modelPreviewTextLine.SetFontName(localeInfo.UI_DEF_FONT_LARGE + "b")
			self.modelPreviewTextLine.SetOutline(True)
			self.modelPreviewTextLine.SetPosition(0, 3)
			UI_DEF_FONT = "Tahoma:14b"
			self.modelPreviewTextLine.SetFontName(UI_DEF_FONT)
			self.modelPreviewTextLine.SetText(localeInfo.RENDER_TARGET_PREVIEW_ITEM)
			self.modelPreviewTextLine.SetOutline()
			self.modelPreviewTextLine.SetWindowHorizontalAlignCenter()
			self.modelPreviewTextLine.SetHorizontalAlignCenter()
			self.modelPreviewTextLine.Show()

		renderTarget.SetVisibility(RENDER_TARGET_INDEX, True)

		# Ustawienie modelu
		renderTarget.SelectModel(RENDER_TARGET_INDEX, model)
		# Ustawienie zbroi
		renderTarget.SetArmor(RENDER_TARGET_INDEX, bodyVnum)
		# Ustawienie fryzury
		renderTarget.SetHair(RENDER_TARGET_INDEX, hairVnum)
		# Ustawienie broni
		renderTarget.SetWeapon(RENDER_TARGET_INDEX, weaponVnum)
		# Ustawienie akcesori?w
		renderTarget.SetAcce(RENDER_TARGET_INDEX, acceVnum)

	def SetRenderOnKey(self, flag):
		self.renderOnKey = flag
		
	def __ModelPreviewClose(self):
		self.rtargetshow = 0
		RENDER_TARGET_INDEX = 1

		if self.modelPreviewBoard:
			self.modelPreviewBoard.Hide()
			self.modelPreviewRender.Hide()
			self.modelPreviewBelka.Hide()
			self.modelPreviewTextLine.Hide()

			self.modelPreviewBoard = None
			self.modelPreviewRender = None
			self.modelPreviewBelka = None
			self.modelPreviewTextLine = None

			renderTarget.SetVisibility(RENDER_TARGET_INDEX, False)
			# renderTarget.SelectModel(RENDER_TARGET_INDEX, -1)

	def SetModelShow(self, flag):
		self.modelShow = flag

	def HideToolTip(self):
		ToolTip.HideToolTip(self)
		
		self.__ModelPreviewClose()

##NEW TOOLTIP
class ToolTipNew(ui.ThinBoardNew):
	TOOL_TIP_WIDTH = 190
	TOOL_TIP_HEIGHT = 3
	TEXT_LINE_HEIGHT = 17
	TITLE_COLOR = grp.GenerateColor(0.9490, 0.9058, 0.7568, 1.0)
	SPECIAL_TITLE_COLOR = grp.GenerateColor(1.0, 0.7843, 0.0, 1.0)
	NORMAL_COLOR = grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0)
	FONT_COLOR = grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0)
	POSITIVE_COLOR = grp.GenerateColor(0.5411, 0.7254, 0.5568, 1.0)

	def __init__(self, width = TOOL_TIP_WIDTH, isPickable=False):
		ui.ThinBoardNew.__init__(self, "TOP_MOST")

		if isPickable:
			pass
		else:
			self.AddFlag("not_pick")

		self.AddFlag("float")

		self.followFlag = True
		self.toolTipWidth = width

		self.xPos = -1
		self.yPos = -1
		self.countInfo = 0

		self.defFontName = localeInfo.UI_DEF_FONT
		self.ClearToolTip()

	def __del__(self):
		ui.ThinBoardNew.__del__(self)

	def ClearToolTip(self):
		self.toolTipHeight = 3
		self.countInfo = 0
		self.childrenList = []

	def SetFollow(self, flag):
		self.followFlag = flag

	def SetDefaultFontName(self, fontName):
		self.defFontName = fontName

	def ResizeToolTip(self):
		self.SetSize(self.toolTipWidth, self.TOOL_TIP_HEIGHT + self.toolTipHeight)

	def AppendSpace(self, size):
		self.toolTipHeight += size
		self.ResizeToolTip()

	def AppendHorizontalLine(self):

		for i in xrange(2):
			horizontalLine = ui.Line()
			horizontalLine.SetParent(self)
			horizontalLine.SetPosition(0, self.toolTipHeight + 4 + i)
			horizontalLine.SetWindowHorizontalAlignCenter()
			horizontalLine.SetSize(100, 0)
			horizontalLine.Show()

			if 0 == i:
				horizontalLine.SetColor(0xff555555)
			else:
				horizontalLine.SetColor(0xff000000)

			self.childrenList.append(horizontalLine)

		self.toolTipHeight += 11
		self.ResizeToolTip()

	def AlignHorizonalCenter(self):
		for child in self.childrenList:
			(x, y)=child.GetLocalPosition()
			child.SetPosition(self.toolTipWidth/2, y)

		self.ResizeToolTip()

	def SetTitle(self, text, color = TITLE_COLOR, centerAlign = True):
		textLine = ui.TextLine()
		textLine.SetParent(self)
		TITLE_FONT = "Tahoma:14"
		textLine.SetFontName(TITLE_FONT)
		textLine.SetPackedFontColor(color)
		textLine.SetText(text)
		textLine.SetOutline()
		textLine.SetFeather(False)
		textLine.Show()

		if centerAlign:
			textLine.SetPosition(self.toolTipWidth/2, 5)
			textLine.SetHorizontalAlignCenter()

		else:
			textLine.SetPosition(10, self.toolTipHeight)

		self.childrenList.append(textLine)

		self.toolTipHeight += self.TEXT_LINE_HEIGHT
		self.ResizeToolTip()

		return textLine

	def AppendGreenTextLine(self, text, color = POSITIVE_COLOR, centerAlign = True):
		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetFontName("Tahoma:12")
		textLine.SetPackedFontColor(color)
		textLine.SetText(text)
		textLine.SetOutline()
		textLine.SetFeather(False)
		textLine.Show()

		if centerAlign:
			textLine.SetPosition(self.toolTipWidth/2, self.toolTipHeight)
			textLine.SetHorizontalAlignCenter()

		else:
			textLine.SetPosition(10, self.toolTipHeight)

		self.childrenList.append(textLine)

		self.toolTipHeight += self.TEXT_LINE_HEIGHT
		self.ResizeToolTip()

		return textLine

	def AppendTextLine(self, text, color = FONT_COLOR, centerAlign = True):
		textLine = ui.TextLine()
		textLine.SetParent(self)
		#textLine.SetFontName(self.defFontName)
		textLine.SetFontName("Tahoma:12")
		textLine.SetPackedFontColor(color)
		textLine.SetText(text)
		textLine.SetOutline()
		textLine.SetFeather(False)
		if app.WJ_MULTI_TEXTLINE:
			textLine.SetLineHeight(self.TEXT_LINE_HEIGHT)

		if centerAlign:
			textLine.SetPosition(self.toolTipWidth / 2, self.toolTipHeight)
			textLine.SetHorizontalAlignCenter()
		else:
			textLine.SetPosition(10, self.toolTipHeight)

		textLine.Show()

		self.childrenList.append(textLine)

		if app.WJ_MULTI_TEXTLINE:
			lineCount = textLine.GetTextLineCount()
			self.toolTipHeight += self.TEXT_LINE_HEIGHT * lineCount
		else:
			self.toolTipHeight += self.TEXT_LINE_HEIGHT

		self.ResizeToolTip()

		return textLine
	
	def SetToolTipPosition(self, x = -1, y = -1):
		self.xPos = x
		self.yPos = y

	def ShowToolTip(self):
		self.SetTop()
		self.Show()

		self.OnUpdate()

	def HideToolTip(self):
		self.Hide()

	def OnUpdate(self):

		if not self.followFlag:
			return

		x = 0
		y = 0
		width = self.GetWidth()
		height = self.toolTipHeight

		if -1 == self.xPos and -1 == self.yPos:

			(mouseX, mouseY) = wndMgr.GetMousePosition()

			if mouseY < wndMgr.GetScreenHeight() - 300:
				y = mouseY + 40
			else:
				y = mouseY - height - 30

			x = mouseX - width/2

		else:

			x = self.xPos - width/2
			y = self.yPos - height

		x = max(x, 0)
		y = max(y, 0)
		x = min(x + width/2, wndMgr.GetScreenWidth() - width/2) - width/2
		y = min(y + self.GetHeight(), wndMgr.GetScreenHeight()) - self.GetHeight()

		parentWindow = self.GetParentProxy()
		if parentWindow:
			(gx, gy) = parentWindow.GetGlobalPosition()
			x -= gx
			y -= gy

		self.SetPosition(x, y)
##end

class HyperlinkItemToolTip(ItemToolTip):
	def __init__(self):
		ItemToolTip.__init__(self, isPickable=True)

	def SetHyperlinkItem(self, tokens):
		minTokenCount = 3 + player.METIN_SOCKET_MAX_NUM
		maxTokenCount = minTokenCount + 2 * player.ATTRIBUTE_SLOT_MAX_NUM
		if tokens and len(tokens) >= minTokenCount and len(tokens) <= maxTokenCount:
			head, vnum, flag = tokens[:3]
			itemVnum = int(vnum, 16)
			
			import app
			if app.ENABLE_EXTENDED_SOCKETS:
				metinSlot = [int(metin, 16) for metin in tokens[3:9]]

				rests = tokens[9:]
			else:
				metinSlot = [int(metin, 16) for metin in tokens[3:6]]

				rests = tokens[6:]		
				
			if rests:
				attrSlot = []

				rests.reverse()
				while rests:
					key = int(rests.pop(), 16)
					if rests:
						val = int(rests.pop())
						attrSlot.append((key, val))

				attrSlot += [(0, 0)] * (player.ATTRIBUTE_SLOT_MAX_NUM - len(attrSlot))
			else:
				attrSlot = [(0, 0)] * player.ATTRIBUTE_SLOT_MAX_NUM

			self.ClearToolTip()
			self.AddItemData(itemVnum, metinSlot, attrSlot)

			ItemToolTip.OnUpdate(self)

	def OnUpdate(self):
		pass

	def OnMouseLeftButtonDown(self):
		self.Hide()

class SkillToolTip(ToolTip):

	POINT_NAME_DICT = {
		player.LEVEL : localeInfo.SKILL_TOOLTIP_LEVEL,
		player.IQ : localeInfo.SKILL_TOOLTIP_INT,
	}


	SKILL_TOOL_TIP_WIDTH = 200
	PARTY_SKILL_TOOL_TIP_WIDTH = 340

	PARTY_SKILL_EXPERIENCE_AFFECT_LIST = (	( 2, 2,  10,),
											( 8, 3,  20,),
											(14, 4,  30,),
											(22, 5,  45,),
											(28, 6,  60,),
											(34, 7,  80,),
											(38, 8, 100,), )

	PARTY_SKILL_PLUS_GRADE_AFFECT_LIST = (	( 4, 2, 1, 0,),
											(10, 3, 2, 0,),
											(16, 4, 2, 1,),
											(24, 5, 2, 2,), )

	PARTY_SKILL_ATTACKER_AFFECT_LIST = (	( 36, 3, ),
											( 26, 1, ),
											( 32, 2, ), )

	SKILL_GRADE_NAME = {	player.SKILL_GRADE_MASTER : localeInfo.SKILL_GRADE_NAME_MASTER,
							player.SKILL_GRADE_GRAND_MASTER : localeInfo.SKILL_GRADE_NAME_GRAND_MASTER,
							player.SKILL_GRADE_PERFECT_MASTER : localeInfo.SKILL_GRADE_NAME_PERFECT_MASTER, }

	AFFECT_NAME_DICT =	{
							"HP" : localeInfo.TOOLTIP_SKILL_AFFECT_ATT_POWER,
							"ATT_GRADE" : localeInfo.TOOLTIP_SKILL_AFFECT_ATT_GRADE,
							"DEF_GRADE" : localeInfo.TOOLTIP_SKILL_AFFECT_DEF_GRADE,
							"ATT_SPEED" : localeInfo.TOOLTIP_SKILL_AFFECT_ATT_SPEED,
							"MOV_SPEED" : localeInfo.TOOLTIP_SKILL_AFFECT_MOV_SPEED,
							"DODGE" : localeInfo.TOOLTIP_SKILL_AFFECT_DODGE,
							"RESIST_NORMAL" : localeInfo.TOOLTIP_SKILL_AFFECT_RESIST_NORMAL,
							"REFLECT_MELEE" : localeInfo.TOOLTIP_SKILL_AFFECT_REFLECT_MELEE,
						}
	AFFECT_APPEND_TEXT_DICT =	{
									"DODGE" : "%",
									"RESIST_NORMAL" : "%",
									"REFLECT_MELEE" : "%",
								}

	def __init__(self):
		ToolTip.__init__(self, self.SKILL_TOOL_TIP_WIDTH)
	def __del__(self):
		ToolTip.__del__(self)

	def SetSkill(self, skillIndex, skillLevel = -1):

		if 0 == skillIndex:
			return

		if skill.SKILL_TYPE_GUILD == skill.GetSkillType(skillIndex):

			if self.SKILL_TOOL_TIP_WIDTH != self.toolTipWidth:
				self.toolTipWidth = self.SKILL_TOOL_TIP_WIDTH
				self.ResizeToolTip()

			self.AppendDefaultData(skillIndex)
			self.AppendSkillConditionData(skillIndex)
			self.AppendGuildSkillData(skillIndex, skillLevel)

		else:

			if self.SKILL_TOOL_TIP_WIDTH != self.toolTipWidth:
				self.toolTipWidth = self.SKILL_TOOL_TIP_WIDTH
				self.ResizeToolTip()

			slotIndex = player.GetSkillSlotIndex(skillIndex)
			skillGrade = player.GetSkillGrade(slotIndex)
			skillLevel = player.GetSkillLevel(slotIndex)
			skillCurrentPercentage = player.GetSkillCurrentEfficientPercentage(slotIndex)
			skillNextPercentage = player.GetSkillNextEfficientPercentage(slotIndex)

			self.AppendDefaultData(skillIndex)
			self.AppendSkillConditionData(skillIndex)
			self.AppendSkillDataNew(slotIndex, skillIndex, skillGrade, skillLevel, skillCurrentPercentage, skillNextPercentage)
			self.AppendSkillRequirement(skillIndex, skillLevel)

		self.ShowToolTip()

	def SetSkillNew(self, slotIndex, skillIndex, skillGrade, skillLevel):

		if 0 == skillIndex:
			return

		if player.SKILL_INDEX_TONGSOL == skillIndex:

			slotIndex = player.GetSkillSlotIndex(skillIndex)
			skillLevel = player.GetSkillLevel(slotIndex)

			self.AppendDefaultData(skillIndex)
			self.AppendPartySkillData(skillGrade, skillLevel)

		elif player.SKILL_INDEX_FISHING == skillIndex:
			slotIndex = player.GetSkillSlotIndex(player.SKILL_INDEX_FISHING)
			skillValues = [0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20]
			skillValues2 = [0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20]
			skillValues3 = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30]
			skillValues4 = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 10, 11, 13, 15]
			skillValues5 = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20]
			skillValues6 = [0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7]
			skillLevel = player.GetSkillLevel(slotIndex) + [0, 19, 29, 39][player.GetSkillGrade(slotIndex)]
			self.AppendDefaultData(skillIndex)
			self.AppendSpace(2)
			self.AppendTextLine(localeInfo.TOOLTIP_FISHING_SKILL_BONUS_NOW, self.POSITIVE_COLOR)
			self.AppendTextLine(localeInfo.TOOLTIP_FISHING_SKILL_BONUS_BIGGER_FISH % skillValues[skillLevel], self.POSITIVE_COLOR)
			self.AppendTextLine(localeInfo.TOOLTIP_FISHING_SKILL_BONUS_RARE % skillValues2[skillLevel], self.POSITIVE_COLOR)
			self.AppendTextLine(localeInfo.TOOLTIP_FISHING_SKILL_BONUS_YANG % skillValues3[skillLevel], self.POSITIVE_COLOR)
			self.AppendTextLine(localeInfo.TOOLTIP_FISHING_SKILL_BONUS_INSTANT % skillValues4[skillLevel], self.POSITIVE_COLOR)
			self.AppendTextLine(localeInfo.TOOLTIP_FISHING_SKILL_BONUS_POINTS % skillValues5[skillLevel], self.POSITIVE_COLOR)
			self.AppendTextLine(localeInfo.TOOLTIP_FISHING_SKILL_BONUS_STARFISH % skillValues6[skillLevel], self.POSITIVE_COLOR)

			if skillLevel+1 <= 40:
				self.AppendSpace(5)
				self.AppendTextLine(localeInfo.TOOLTIP_FISHING_SKILL_BONUS_NEXT, self.NEGATIVE_COLOR)
				self.AppendTextLine(localeInfo.TOOLTIP_FISHING_SKILL_BONUS_BIGGER_FISH % skillValues[skillLevel+1], self.POSITIVE_COLOR)
				self.AppendTextLine(localeInfo.TOOLTIP_FISHING_SKILL_BONUS_RARE % skillValues2[skillLevel+1], self.POSITIVE_COLOR)
				self.AppendTextLine(localeInfo.TOOLTIP_FISHING_SKILL_BONUS_YANG % skillValues3[skillLevel+1], self.POSITIVE_COLOR)
				self.AppendTextLine(localeInfo.TOOLTIP_FISHING_SKILL_BONUS_INSTANT % skillValues4[skillLevel+1], self.POSITIVE_COLOR)
				self.AppendTextLine(localeInfo.TOOLTIP_FISHING_SKILL_BONUS_POINTS % skillValues5[skillLevel+1], self.POSITIVE_COLOR)
				self.AppendTextLine(localeInfo.TOOLTIP_FISHING_SKILL_BONUS_STARFISH % skillValues6[skillLevel+1], self.POSITIVE_COLOR)
			else:
				self.AppendSpace(2)
				self.AppendTextLine(localeInfo.TOOLTIP_FISHING_SKILL_MAX, self.SPECIAL_TITLE_COLOR)

			self.AppendSpace(4)
			myFishCountString = "0"
			count = int(constInfo.MY_FISH_COUNT)
			if count > 0:
				myFishCountString = "%s" % ('.'.join([ i-3<0 and str(count)[:i] or str(count)[i-3:i] for i in range(len(str(count))%3, len(str(count))+1, 3) if i ]))
			self.AppendTextLine(localeInfo.SKILL_FISHES_CAUGHT%myFishCountString, self.POSITIVE_COLOR)
				
		elif player.SKILL_INDEX_MINING == skillIndex:
			slotIndex = player.GetSkillSlotIndex(player.SKILL_INDEX_MINING)
			skillValues = [0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20]
			skillValues2 = [0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20]
			skillValues3 = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1]
			skillValues4 = [0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7]
			skillLevel = player.GetSkillLevel(slotIndex) + [0, 19, 29, 39][player.GetSkillGrade(slotIndex)]
			self.AppendDefaultData(skillIndex)
			self.AppendSpace(2)
			self.AppendTextLine(localeInfo.TOOLTIP_FISHING_SKILL_BONUS_NOW, self.POSITIVE_COLOR)
			self.AppendTextLine(localeInfo.SKILL_MINING_CHANCE%skillValues[skillLevel], self.POSITIVE_COLOR)
			self.AppendTextLine(localeInfo.SKILL_MINING_CHANCE_MORE_ORE%skillValues2[skillLevel], self.POSITIVE_COLOR)
			self.AppendTextLine(localeInfo.SKILL_MINING_LESS_TIME%skillValues3[skillLevel], self.POSITIVE_COLOR)
			self.AppendTextLine(localeInfo.SKILL_MINING_RELIC_CHANCE%skillValues4[skillLevel], self.POSITIVE_COLOR)

			if skillLevel+1 <= 40:
				self.AppendSpace(5)
				self.AppendTextLine(localeInfo.TOOLTIP_FISHING_SKILL_BONUS_NEXT, self.NEGATIVE_COLOR)
				self.AppendTextLine(localeInfo.SKILL_MINING_CHANCE%skillValues[skillLevel+1], self.POSITIVE_COLOR)
				self.AppendTextLine(localeInfo.SKILL_MINING_CHANCE_MORE_ORE%skillValues2[skillLevel+1], self.POSITIVE_COLOR)
				self.AppendTextLine(localeInfo.SKILL_MINING_LESS_TIME%skillValues3[skillLevel+1], self.POSITIVE_COLOR)
				self.AppendTextLine(localeInfo.SKILL_MINING_RELIC_CHANCE%skillValues4[skillLevel+1], self.POSITIVE_COLOR)
			else:
				self.AppendSpace(2)
				self.AppendTextLine(localeInfo.TOOLTIP_FISHING_SKILL_MAX, self.SPECIAL_TITLE_COLOR)

			self.AppendSpace(4)
			myOreCountString = "0"
			count = int(constInfo.MY_ORE_COUNT)
			if count > 0:
				myOreCountString = "%s" % ('.'.join([ i-3<0 and str(count)[:i] or str(count)[i-3:i] for i in range(len(str(count))%3, len(str(count))+1, 3) if i ]))
			self.AppendTextLine(localeInfo.SKILL_MINING_ORE_COUNT%myOreCountString, self.POSITIVE_COLOR)

		elif player.SKILL_INDEX_RIDING == skillIndex:

			slotIndex = player.GetSkillSlotIndex(skillIndex)
			self.AppendSupportSkillDefaultData(skillIndex, skillGrade, skillLevel, 30)

		elif player.SKILL_INDEX_SUMMON == skillIndex:

			maxLevel = 10

			self.ClearToolTip()
			self.__SetSkillTitle(skillIndex, skillGrade)

			## Description
			description = skill.GetSkillDescription(skillIndex)
			self.AppendDescription(description, 25)

			if skillLevel == 10:
				self.AppendSpace(5)
				self.AppendTextLine(localeInfo.TOOLTIP_SKILL_LEVEL_MASTER % (skillLevel), self.NORMAL_COLOR)
				self.AppendTextLine(localeInfo.SKILL_SUMMON_DESCRIPTION % (skillLevel*10), self.NORMAL_COLOR)

			else:
				self.AppendSpace(5)
				self.AppendTextLine(localeInfo.TOOLTIP_SKILL_LEVEL % (skillLevel), self.NORMAL_COLOR)
				self.__AppendSummonDescription(skillLevel, self.NORMAL_COLOR)

				self.AppendSpace(5)
				self.AppendTextLine(localeInfo.TOOLTIP_SKILL_LEVEL % (skillLevel+1), self.NEGATIVE_COLOR)
				self.__AppendSummonDescription(skillLevel+1, self.NEGATIVE_COLOR)

		elif 247 == skillIndex:  # PvM Balance

			if self.SKILL_TOOL_TIP_WIDTH != self.toolTipWidth:
				self.toolTipWidth = self.SKILL_TOOL_TIP_WIDTH
				self.ResizeToolTip()

			self.AppendDefaultData(skillIndex)

			job = chr.RaceToJob(player.GetRace())
			skillGroup = net.GetMainActorSkillGroup()

			BALANCE_MAP = {
				0: [(19, player.ST, "STR")],                            # Warrior: Mental (g2) only
				1: [(31, player.DX, "DEX"), (49, player.DX, "DEX")],   # Assassin: Dagger (g1) / Bow (g2)
				2: [(79, player.IQ, "INT")],                            # Sura: Black Magic (g1) only
				3: [(96, player.IQ, "INT"), (111, player.IQ, "INT")],   # Shaman: Dragon (g1) / Healer (g2)
			}

			self.AppendSpace(5)

			if job in BALANCE_MAP:
				candidates = BALANCE_MAP[job]

				if skillGroup == 0 or len(candidates) == 1:
					activeVnum, activeStatConst, activeStatName = candidates[0]
				else:
					groupIdx = max(0, min(skillGroup - 1, len(candidates) - 1))
					activeVnum, activeStatConst, activeStatName = candidates[groupIdx]

				pct = 0.0
				try:
					slotIdx = player.GetSkillSlotIndex(activeVnum)
					if slotIdx is not None and type(slotIdx) == int:
						pct = player.GetSkillCurrentEfficientPercentage(slotIdx)
				except:
					pct = 0.0

				statVal = player.GetStatus(activeStatConst)
				lv = player.GetStatus(player.LEVEL)
				
				if activeVnum in (96, 111):
					lvMultiplier = 2
				elif activeVnum in (31, 49):
					lvMultiplier = 6
				else:
					lvMultiplier = 3
				
				atkBonus = int((100 + statVal + lv * lvMultiplier) * pct)

				self.AppendTextLine(localeInfo.SKILL_PVM_LINKED_TO % skill.GetSkillName(activeVnum))
				self.AppendTextLine(localeInfo.SKILL_PVM_STRENGTHENED_BY % activeStatName)
				if atkBonus > 0:
					self.AppendTextLine(localeInfo.SKILL_PVM_ATK_VALUE % atkBonus)
				else:
					self.AppendTextLine(localeInfo.SKILL_PVM_ATK_VALUE_ZERO)
				self.AppendSpace(3)
				self.AppendTextLine(localeInfo.SKILL_PVM_LEVEL_FIXED, 0xFFcabdab)

			else:
				self.AppendTextLine(localeInfo.SKILL_PVM_NOT_AVAILABLE, self.NEGATIVE_COLOR)

		elif skill.SKILL_TYPE_GUILD == skill.GetSkillType(skillIndex):

			if self.SKILL_TOOL_TIP_WIDTH != self.toolTipWidth:
				self.toolTipWidth = self.SKILL_TOOL_TIP_WIDTH
				self.ResizeToolTip()

			self.AppendDefaultData(skillIndex)
			self.AppendSkillConditionData(skillIndex)
			self.AppendGuildSkillData(skillIndex, skillLevel)

		else:

			if self.SKILL_TOOL_TIP_WIDTH != self.toolTipWidth:
				self.toolTipWidth = self.SKILL_TOOL_TIP_WIDTH
				self.ResizeToolTip()

			slotIndex = player.GetSkillSlotIndex(skillIndex)

			skillCurrentPercentage = player.GetSkillCurrentEfficientPercentage(slotIndex)
			skillNextPercentage = player.GetSkillNextEfficientPercentage(slotIndex)

			self.AppendDefaultData(skillIndex, skillGrade)
			self.AppendSkillConditionData(skillIndex)
			self.AppendSkillDataNew(slotIndex, skillIndex, skillGrade, skillLevel, skillCurrentPercentage, skillNextPercentage)
			self.AppendSkillRequirement(skillIndex, skillLevel)

		self.ShowToolTip()

	def __SetSkillTitle(self, skillIndex, skillGrade):
		if chr.IsGameMaster(player.GetMainCharacterIndex()):
			self.AppendTextLine("[ID: %d" % skillIndex + "]")
			self.AppendSpace(5)
		self.SetTitle(skill.GetSkillName(skillIndex, skillGrade))
		self.__AppendSkillGradeName(skillIndex, skillGrade)

	def __AppendSkillGradeName(self, skillIndex, skillGrade):
		if self.SKILL_GRADE_NAME.has_key(skillGrade):
			self.AppendSpace(5)
			self.AppendTextLine(self.SKILL_GRADE_NAME[skillGrade] % (skill.GetSkillName(skillIndex, 0)), self.CAN_LEVEL_UP_COLOR)

	def SetSkillOnlyName(self, slotIndex, skillIndex, skillGrade):
		if 0 == skillIndex:
			return

		slotIndex = player.GetSkillSlotIndex(skillIndex)

		self.toolTipWidth = self.SKILL_TOOL_TIP_WIDTH
		self.ResizeToolTip()

		self.ClearToolTip()
		self.__SetSkillTitle(skillIndex, skillGrade)
		self.AppendDefaultData(skillIndex, skillGrade)
		self.AppendSkillConditionData(skillIndex)
		self.ShowToolTip()

	def AppendDefaultData(self, skillIndex, skillGrade = 0):
		self.ClearToolTip()
		self.__SetSkillTitle(skillIndex, skillGrade)

		## Level Limit
		levelLimit = skill.GetSkillLevelLimit(skillIndex)
		if levelLimit > 0:

			color = self.NORMAL_COLOR
			if player.GetStatus(player.LEVEL) < levelLimit:
				color = self.NEGATIVE_COLOR

			self.AppendSpace(5)
			self.AppendTextLine(localeInfo.TOOLTIP_ITEM_LIMIT_LEVEL % (levelLimit), color)

		## Description
		description = skill.GetSkillDescription(skillIndex)
		self.AppendDescription(description, 25)

	if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
		def AppendBuffDefaultData(self, skillIndex, skillGrade = 0):
			self.ClearToolTip()
			self.SetTitle(skill.GetSkillName(skillIndex, skillGrade))

			## Level Limit
			levelLimit = skill.GetSkillLevelLimit(skillIndex)
			if levelLimit > 0:

				color = self.NORMAL_COLOR
				if player.GetStatus(player.LEVEL) < levelLimit:
					color = self.NEGATIVE_COLOR

				self.AppendSpace(5)
				self.AppendTextLine(localeInfo.TOOLTIP_ITEM_LIMIT_LEVEL % (levelLimit), color)

			# ## Description
			# description = skill.GetSkillDescription(skillIndex)
			# self.AppendDescription(description, 25)

	def AppendSupportSkillDefaultData(self, skillIndex, skillGrade, skillLevel, maxLevel):
		self.ClearToolTip()
		self.__SetSkillTitle(skillIndex, skillGrade)

		## Description
		description = skill.GetSkillDescription(skillIndex)
		self.AppendDescription(description, 25)

		if 1 == skillGrade:
			skillLevel += 19
		elif 2 == skillGrade:
			skillLevel += 29
		elif 3 == skillGrade:
			skillLevel = 40

		self.AppendSpace(5)
		self.AppendTextLine(localeInfo.TOOLTIP_SKILL_LEVEL_WITH_MAX % (skillLevel, maxLevel), self.NORMAL_COLOR)

	def AppendSkillConditionData(self, skillIndex):
		conditionDataCount = skill.GetSkillConditionDescriptionCount(skillIndex)
		if conditionDataCount > 0:
			self.AppendSpace(5)
			for i in xrange(conditionDataCount):
				self.AppendTextLine(skill.GetSkillConditionDescription(skillIndex, i), self.CONDITION_COLOR)

	def AppendGuildSkillData(self, skillIndex, skillLevel):
		skillMaxLevel = 7
		skillCurrentPercentage = float(skillLevel) / float(skillMaxLevel)
		skillNextPercentage = float(skillLevel+1) / float(skillMaxLevel)
		## Current Level
		if skillLevel > 0:
			if self.HasSkillLevelDescription(skillIndex, skillLevel):
				self.AppendSpace(5)
				if skillLevel == skillMaxLevel:
					self.AppendTextLine(localeInfo.TOOLTIP_SKILL_LEVEL_MASTER % (skillLevel), self.NORMAL_COLOR)
				else:
					self.AppendTextLine(localeInfo.TOOLTIP_SKILL_LEVEL % (skillLevel), self.NORMAL_COLOR)

				#####

				for i in xrange(skill.GetSkillAffectDescriptionCount(skillIndex)):
					self.AppendTextLine(skill.GetSkillAffectDescription(skillIndex, i, skillCurrentPercentage), self.ENABLE_COLOR)

				## Cooltime
				coolTime = skill.GetSkillCoolTime(skillIndex, skillCurrentPercentage)
				if coolTime > 0:
					self.AppendTextLine(localeInfo.TOOLTIP_SKILL_COOL_TIME + str(coolTime), self.ENABLE_COLOR)

				## SP
				needGSP = skill.GetSkillNeedSP(skillIndex, skillCurrentPercentage)
				if needGSP > 0:
					self.AppendTextLine(localeInfo.TOOLTIP_NEED_GSP % (needGSP), self.ENABLE_COLOR)

		## Next Level
		if skillLevel < skillMaxLevel:
			if self.HasSkillLevelDescription(skillIndex, skillLevel+1):
				self.AppendSpace(5)
				self.AppendTextLine(localeInfo.TOOLTIP_NEXT_SKILL_LEVEL_1 % (skillLevel+1, skillMaxLevel), self.DISABLE_COLOR)

				#####

				for i in xrange(skill.GetSkillAffectDescriptionCount(skillIndex)):
					self.AppendTextLine(skill.GetSkillAffectDescription(skillIndex, i, skillNextPercentage), self.DISABLE_COLOR)

				## Cooltime
				coolTime = skill.GetSkillCoolTime(skillIndex, skillNextPercentage)
				if coolTime > 0:
					self.AppendTextLine(localeInfo.TOOLTIP_SKILL_COOL_TIME + str(coolTime), self.DISABLE_COLOR)

				## SP
				needGSP = skill.GetSkillNeedSP(skillIndex, skillNextPercentage)
				if needGSP > 0:
					self.AppendTextLine(localeInfo.TOOLTIP_NEED_GSP % (needGSP), self.DISABLE_COLOR)

	def AppendSkillDataNew(self, slotIndex, skillIndex, skillGrade, skillLevel, skillCurrentPercentage, skillNextPercentage):

		self.skillMaxLevelStartDict = { 0 : 17, 1 : 7, 2 : 10, }
		self.skillMaxLevelEndDict = { 0 : 20, 1 : 10, 2 : 10, }

		skillLevelUpPoint = 1
		realSkillGrade = player.GetSkillGrade(slotIndex)
		skillMaxLevelStart = self.skillMaxLevelStartDict.get(realSkillGrade, 15)
		skillMaxLevelEnd = self.skillMaxLevelEndDict.get(realSkillGrade, 20)

		## Current Level
		if skillLevel > 0:
			if self.HasSkillLevelDescription(skillIndex, skillLevel):
				self.AppendSpace(5)
				if skillGrade == skill.SKILL_GRADE_COUNT:
					pass
				elif skillLevel == skillMaxLevelEnd:
					self.AppendTextLine(localeInfo.TOOLTIP_SKILL_LEVEL_MASTER % (skillLevel), self.NORMAL_COLOR)
				else:
					self.AppendTextLine(localeInfo.TOOLTIP_SKILL_LEVEL % (skillLevel), self.NORMAL_COLOR)
				self.AppendSkillLevelDescriptionNew(skillIndex, skillCurrentPercentage, self.ENABLE_COLOR)

		## Next Level
		if skillGrade != skill.SKILL_GRADE_COUNT:
			if skillLevel < skillMaxLevelEnd:
				if self.HasSkillLevelDescription(skillIndex, skillLevel+skillLevelUpPoint):
					self.AppendSpace(5)
					if skillIndex == 141 or skillIndex == 142:
						self.AppendTextLine(localeInfo.TOOLTIP_NEXT_SKILL_LEVEL_3 % (skillLevel+1), self.DISABLE_COLOR)
					else:
						self.AppendTextLine(localeInfo.TOOLTIP_NEXT_SKILL_LEVEL_1 % (skillLevel+1, skillMaxLevelEnd), self.DISABLE_COLOR)
					self.AppendSkillLevelDescriptionNew(skillIndex, skillNextPercentage, self.DISABLE_COLOR)

		if skillIndex == 129:
			self.AppendSpace(5)
			if not skillGrade == 3:
				self.AppendTextLine(localeInfo.TOOLTIP_SKILL_LEVEL % (skillLevel), self.NORMAL_COLOR)

			self.AppendTextLine(localeInfo.SKILL_DATA_129_DESC_1.format(POLY_DMG_VAL[skillGrade][skillLevel]))

			if not skillGrade == 3:
				self.AppendSpace(5)
				self.AppendTextLine(localeInfo.TOOLTIP_NEXT_SKILL_LEVEL_1 % (skillLevel+1, 40), self.DISABLE_COLOR)
				if skillLevel == 19:					
					self.AppendTextLine(localeInfo.SKILL_DATA_129_DESC_1.format(POLY_DMG_VAL[1][0]))

				elif skillGrade == 1 and skillLevel == 10:
					self.AppendTextLine(localeInfo.SKILL_DATA_129_DESC_1.format(POLY_DMG_VAL[2][0]))
				elif skillGrade == 2 and skillLevel == 10:
					self.AppendTextLine(localeInfo.SKILL_DATA_129_DESC_1.format(POLY_DMG_VAL[3][0]))
				else:
					self.AppendTextLine(localeInfo.SKILL_DATA_129_DESC_1.format(POLY_DMG_VAL[skillGrade][skillLevel+1]))

			elif skillGrade == 3:
				self.AppendSpace(5)
				self.AppendTextLine(localeInfo.SKILLS_MAX_LEVEL, self.POSITIVE_COLOR)

		for i in xrange(180, 191):
			if skillIndex == i:
				self.AppendTextLine("|cFF999999_______________________")
				self.AppendSpace(5)
				self.AppendTextLine(localeInfo.PASSIVE_SKILLS_FULLED_DESC_1, self.NORMAL_COLOR)
				self.AppendTextLine(localeInfo.PASSIVE_SKILLS_FULLED_DESC_2, self.NORMAL_COLOR)
				self.AppendTextLine(localeInfo.PASSIVE_SKILLS_FULLED_DESC_3.format(pasywkiValues[skillIndex]))

	if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
		def SetSkillBuffNPC(self, slotIndex, skillIndex, skillGrade, skillLevel, curSkillPower, nextSkillPower, intPoints):

			if 0 == skillIndex:
				return

			self.AppendBuffDefaultData(skillIndex, skillGrade)
			# self.AppendSkillConditionData(skillIndex)
			self.AppendSkillDataBuffNPC(slotIndex, skillIndex, skillGrade, skillLevel, curSkillPower, nextSkillPower, intPoints)
			self.AppendSkillRequirement(skillIndex, skillLevel)

			self.ShowToolTip()
			
		def AppendSkillDataBuffNPC(self, slotIndex, skillIndex, skillGrade, skillLevel, skillCurrentPercentage, skillNextPercentage, intPoints):
			self.skillMaxLevelStartDict = { 0 : 17, 1 : 7, 2 : 10, }
			self.skillMaxLevelEndDict = { 0 : 20, 1 : 10, 2 : 10, }

			skillLevelUpPoint = 1
			realSkillGrade = player.GetSkillGrade(slotIndex)
			skillMaxLevelStart = self.skillMaxLevelStartDict.get(realSkillGrade, 15)
			skillMaxLevelEnd = self.skillMaxLevelEndDict.get(realSkillGrade, 20)

			if skillGrade == skill.SKILL_GRADE_COUNT:
				self.AppendTextLine(localeInfo.SKILLS_MAX_LEVEL, self.POSITIVE_COLOR)
			## Current Level
			if skillLevel > 0:
				self.AppendTextLine("|cFF999999_______________________")
				if self.HasSkillLevelDescription(skillIndex, skillLevel):
					self.AppendSpace(5)
					if skillGrade == skill.SKILL_GRADE_COUNT:
						pass
					elif skillLevel == skillMaxLevelEnd:
						self.AppendTextLine(localeInfo.TOOLTIP_SKILL_LEVEL_MASTER % (skillLevel), self.NORMAL_COLOR)
					else:
						self.AppendTextLine(localeInfo.TOOLTIP_SKILL_LEVEL % (skillLevel), self.NORMAL_COLOR)
					self.AppendSkillLevelDescriptionBuffNPC(skillIndex, skillCurrentPercentage, self.ENABLE_COLOR, intPoints)

			## Next Level
			if skillGrade != skill.SKILL_GRADE_COUNT:
				if skillLevel < skillMaxLevelEnd:
					self.AppendTextLine("|cFF999999_______________________")
					if self.HasSkillLevelDescription(skillIndex, skillLevel+skillLevelUpPoint):
						self.AppendSpace(5)
						if skillIndex == 141 or skillIndex == 142:
							self.AppendTextLine(localeInfo.TOOLTIP_NEXT_SKILL_LEVEL_3 % (skillLevel+1), self.DISABLE_COLOR)
						else:
							self.AppendTextLine(localeInfo.TOOLTIP_NEXT_SKILL_LEVEL_1 % (skillLevel+1, skillMaxLevelEnd), self.DISABLE_COLOR)

						self.AppendSkillLevelDescriptionBuffNPC(skillIndex, skillNextPercentage, self.DISABLE_COLOR, intPoints)
					
			self.AppendTextLine("|cFF999999_______________________")
			self.AppendSpace(5)
			self.AppendTextLine(localeInfo.PASSIVE_SKILLS_FULLED_SHAMAN_DESC_1, self.NORMAL_COLOR)
			self.AppendTextLine(localeInfo.PASSIVE_SKILLS_FULLED_SHAMAN_DESC_2, self.NORMAL_COLOR)
			if skillIndex == 94 or skillIndex == 96:
				self.AppendTextLine(localeInfo.PASSIVE_SKILLS_FULLED_SHAMAN_DESC_3.format("15%"), self.NORMAL_COLOR)
			else:
				self.AppendTextLine(localeInfo.PASSIVE_SKILLS_FULLED_SHAMAN_DESC_3.format(120), self.NORMAL_COLOR)

			self.AppendSpace(5)
			if constInfo.BuffSkillInfo_Master == 1:
				self.AppendTextLine(localeInfo.BUFF_IS_BONUS_ACTIVE, self.POSITIVE_COLOR)
			else:
				self.AppendTextLine(localeInfo.BUFF_IS_BONUS_INACTIVE, self.NEGATIVE_COLOR)

		def AppendSkillLevelDescriptionBuffNPC(self, skillIndex, skillPercentage, color, intPoints):

			affectDataCount = skill.GetNewAffectDataCount(skillIndex)
			
			for i in xrange(skill.GetSkillAffectDescriptionCount(skillIndex)):
				if skillIndex == 94:
					self.AppendTextLine(localeInfo.BUFF_SKILL_LEVEL_DESC_1.format(skill.GetBuffNPCSkillAffectDescription(skillIndex, i, skillPercentage, intPoints)), color)
					if constInfo.BuffSkillInfo_Master == 1:
						self.AppendTextLine(localeInfo.BUFF_SKILL_LEVEL_DESC_2.format(skill.GetBuffNPCSkillAffectDescription(skillIndex, i, skillPercentage, intPoints+14)), color)

						self.AppendTextLine(localeInfo.AUTOBUFF_CHANCE_1+skill.GetBuffNPCSkillAffectDescription(skillIndex, i, skillPercentage, intPoints+14)+"%|r)", self.NORMAL_COLOR)
					self.AppendSpace(5)
				elif skillIndex == 96:
					self.AppendTextLine(localeInfo.AUTOBUFF_CHANCE+skill.GetBuffNPCSkillAffectDescription(skillIndex, i, skillPercentage, intPoints)+"%", color)
					if constInfo.BuffSkillInfo_Master == 1:
						self.AppendTextLine(localeInfo.AUTOBUFF_BONUS_2+skill.GetBuffNPCSkillAffectDescription(skillIndex, i, skillPercentage, intPoints+14)+"%|r)", self.NORMAL_COLOR)
					self.AppendSpace(5)
				else:
					self.AppendTextLine(localeInfo.AUTOBUFF_ATTACK_VALUE+skill.GetBuffNPCSkillAffectDescription(skillIndex, i, skillPercentage, intPoints), color)
					if constInfo.BuffSkillInfo_Master == 1:
						self.AppendTextLine(localeInfo.AUTOBUFF_BONUS_3+skill.GetBuffNPCSkillAffectDescription(skillIndex, i, skillPercentage, intPoints+50)+"|r)", self.NORMAL_COLOR)
					self.AppendSpace(5)

			duration = skill.GetDuration(skillIndex, skillPercentage)
			if duration > 0:
				self.AppendTextLine(localeInfo.TOOLTIP_SKILL_DURATION % (duration), color)

			# coolTime = skill.GetSkillCoolTime(skillIndex, skillPercentage)
			# if coolTime > 0:
			# 	self.AppendTextLine(localeInfo.TOOLTIP_SKILL_COOL_TIME + str(coolTime), color)

	def AppendSkillLevelDescriptionNew(self, skillIndex, skillPercentage, color):

		affectDataCount = skill.GetNewAffectDataCount(skillIndex)
		if affectDataCount > 0:
			for i in xrange(affectDataCount):
				type, minValue, maxValue = skill.GetNewAffectData(skillIndex, i, skillPercentage)

				if not self.AFFECT_NAME_DICT.has_key(type):
					continue

				minValue = int(minValue)
				maxValue = int(maxValue)
				affectText = self.AFFECT_NAME_DICT[type]

				if "HP" == type:
					if minValue < 0 and maxValue < 0:
						minValue *= -1
						maxValue *= -1

					else:
						affectText = localeInfo.TOOLTIP_SKILL_AFFECT_HEAL

				affectText += str(minValue)
				if minValue != maxValue:
					affectText += " - " + str(maxValue)
				affectText += self.AFFECT_APPEND_TEXT_DICT.get(type, "")

				#import debugInfo
				#if debugInfo.IsDebugMode():
				#	affectText = "!!" + affectText

				self.AppendTextLine(affectText, color)

		else:
			for i in xrange(skill.GetSkillAffectDescriptionCount(skillIndex)):
				self.AppendTextLine(skill.GetSkillAffectDescription(skillIndex, i, skillPercentage), color)


		## Duration
		duration = skill.GetDuration(skillIndex, skillPercentage)
		if duration > 0:
			self.AppendTextLine(localeInfo.TOOLTIP_SKILL_DURATION % (duration), color)

		## Cooltime
		coolTime = skill.GetSkillCoolTime(skillIndex, skillPercentage)
		if coolTime > 0:
			self.AppendTextLine(localeInfo.TOOLTIP_SKILL_COOL_TIME + str(coolTime), color)

		## SP
		needSP = skill.GetSkillNeedSP(skillIndex, skillPercentage)
		if needSP != 0:
			continuationSP = skill.GetSkillContinuationSP(skillIndex, skillPercentage)

			if skill.IsUseHPSkill(skillIndex):
				self.AppendNeedHP(needSP, continuationSP, color)
			else:
				self.AppendNeedSP(needSP, continuationSP, color)

	def AppendSkillRequirement(self, skillIndex, skillLevel):

		skillMaxLevel = skill.GetSkillMaxLevel(skillIndex)

		if skillLevel >= skillMaxLevel:
			return

		isAppendHorizontalLine = False

		## Requirement
		if skill.IsSkillRequirement(skillIndex):

			if not isAppendHorizontalLine:
				isAppendHorizontalLine = True
				self.AppendHorizontalLine()

			requireSkillName, requireSkillLevel = skill.GetSkillRequirementData(skillIndex)

			color = self.CANNOT_LEVEL_UP_COLOR
			if skill.CheckRequirementSueccess(skillIndex):
				color = self.CAN_LEVEL_UP_COLOR
			self.AppendTextLine(localeInfo.TOOLTIP_REQUIREMENT_SKILL_LEVEL % (requireSkillName, requireSkillLevel), color)

		## Require Stat
		requireStatCount = skill.GetSkillRequireStatCount(skillIndex)
		if requireStatCount > 0:

			for i in xrange(requireStatCount):
				type, level = skill.GetSkillRequireStatData(skillIndex, i)
				if self.POINT_NAME_DICT.has_key(type):

					if not isAppendHorizontalLine:
						isAppendHorizontalLine = True
						self.AppendHorizontalLine()

					name = self.POINT_NAME_DICT[type]
					color = self.CANNOT_LEVEL_UP_COLOR
					if player.GetStatus(type) >= level:
						color = self.CAN_LEVEL_UP_COLOR
					self.AppendTextLine(localeInfo.TOOLTIP_REQUIREMENT_STAT_LEVEL % (name, level), color)

	def HasSkillLevelDescription(self, skillIndex, skillLevel):
		if skill.GetSkillAffectDescriptionCount(skillIndex) > 0:
			return True
		if skill.GetSkillCoolTime(skillIndex, skillLevel) > 0:
			return True
		if skill.GetSkillNeedSP(skillIndex, skillLevel) > 0:
			return True

		return False

	def AppendMasterAffectDescription(self, index, desc, color):
		self.AppendTextLine(desc, color)

	def AppendNextAffectDescription(self, index, desc):
		self.AppendTextLine(desc, self.DISABLE_COLOR)

	def AppendNeedHP(self, needSP, continuationSP, color):

		self.AppendTextLine(localeInfo.TOOLTIP_NEED_HP % (needSP), color)

		if continuationSP > 0:
			self.AppendTextLine(localeInfo.TOOLTIP_NEED_HP_PER_SEC % (continuationSP), color)

	def AppendNeedSP(self, needSP, continuationSP, color):

		if -1 == needSP:
			self.AppendTextLine(localeInfo.TOOLTIP_NEED_ALL_SP, color)

		else:
			self.AppendTextLine(localeInfo.TOOLTIP_NEED_SP % (needSP), color)

		if continuationSP > 0:
			self.AppendTextLine(localeInfo.TOOLTIP_NEED_SP_PER_SEC % (continuationSP), color)

	def AppendPartySkillData(self, skillGrade, skillLevel):
		# @fixme008 BEGIN
		def comma_fix(vl):
			return vl.replace("%,0f", "%.0f")
		# @fixme008 END

		if 1 == skillGrade:
			skillLevel += 19
		elif 2 == skillGrade:
			skillLevel += 29
		elif 3 == skillGrade:
			skillLevel =  40

		if skillLevel <= 0:
			return

		skillIndex = player.SKILL_INDEX_TONGSOL
		slotIndex = player.GetSkillSlotIndex(skillIndex)
		skillPower = player.GetSkillCurrentEfficientPercentage(slotIndex)
		k = skillPower # @fixme008
		self.AppendSpace(5)
		self.AutoAppendTextLine(localeInfo.TOOLTIP_PARTY_SKILL_LEVEL % skillLevel, self.NORMAL_COLOR)

		# @fixme008 BEGIN
		if skillLevel>=10:
			self.AutoAppendTextLine(comma_fix(localeInfo.PARTY_SKILL_ATTACKER) % chop( 10 + 60 * k ))

		if skillLevel>=20:
			self.AutoAppendTextLine(comma_fix(localeInfo.PARTY_SKILL_BERSERKER) 	% chop(1 + 5 * k))
			self.AutoAppendTextLine(comma_fix(localeInfo.PARTY_SKILL_TANKER) 	% chop(50 + 1450 * k))

		if skillLevel>=25:
			self.AutoAppendTextLine(comma_fix(localeInfo.PARTY_SKILL_BUFFER) % chop(5 + 45 * k ))

		if skillLevel>=35:
			self.AutoAppendTextLine(comma_fix(localeInfo.PARTY_SKILL_SKILL_MASTER) % chop(25 + 600 * k ))

		if skillLevel>=40:
			self.AutoAppendTextLine(comma_fix(localeInfo.PARTY_SKILL_DEFENDER) % chop( 5 + 30 * k ))
		# @fixme008 END

		self.AlignHorizonalCenter()

	def __AppendSummonDescription(self, skillLevel, color):
		if skillLevel > 1:
			self.AppendTextLine(localeInfo.SKILL_SUMMON_DESCRIPTION % (skillLevel * 10), color)
		elif 1 == skillLevel:
			self.AppendTextLine(localeInfo.SKILL_SUMMON_DESCRIPTION % (15), color)
		elif 0 == skillLevel:
			self.AppendTextLine(localeInfo.SKILL_SUMMON_DESCRIPTION % (10), color)

_instanceItemToolTip = None

def GetItemToolTipInstance():
	global _instanceItemToolTip

	if not _instanceItemToolTip:
		SetItemToolTipInstance(ItemToolTip())

	return _instanceItemToolTip

def SetItemToolTipInstance(instance):
	global _instanceItemToolTip

	if _instanceItemToolTip:
		del _instanceItemToolTip

	_instanceItemToolTip = instance

