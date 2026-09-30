#ifndef __HEADER_VNUM_HELPER__
#define	__HEADER_VNUM_HELPER__

class CItemVnumHelper
{
public:

	static	const bool	IsPhoenix(DWORD vnum)				{ return 53001 == vnum; }

	static	const bool	IsRamadanMoonRing(DWORD vnum)		{ return 71135 == vnum; }

	static	const bool	IsHalloweenCandy(DWORD vnum)		{ return 71136 == vnum; }

	static	const bool	IsHappinessRing(DWORD vnum)		{ return 71143 == vnum; }

	static	const bool	IsLovePendant(DWORD vnum)		{ return 71145 == vnum; }
#ifdef ENABLE_SYSTEM_RUNE
	// effecte rune +9
	static	const bool	IsRunaWhite(DWORD vnum)		{ return 55049 == vnum; }
	static	const bool	IsRunaYellow(DWORD vnum)	{ return 55059 == vnum; }
	static	const bool	IsRunaBlack(DWORD vnum)		{ return 55069 == vnum; }
	static	const bool	IsRunaGreen(DWORD vnum)		{ return 55079 == vnum; }
	static	const bool	IsRunaRed(DWORD vnum)		{ return 55089 == vnum; }
	static	const bool	IsRunaBlue(DWORD vnum)		{ return 55098 == vnum; }
#endif

	// Blockable adding bonuses to costumes
	static const bool IsBlockedCostumes(uint32_t vnum) {
		switch (vnum)
		{
			case 53916:
			case 52913:
			case 41962:
			case 41963:
			case 45962:
			case 45963:
				return true;
		}

		return false;
	}
};

class CMobVnumHelper
{
public:

	static	bool	IsPhoenix(DWORD vnum)				{ return 34001 == vnum; }
	static	bool	IsIcePhoenix(DWORD vnum)				{ return 34003 == vnum; }

	static	bool	IsPetUsingPetSystem(DWORD vnum)	{ return (IsPhoenix(vnum) || IsReindeerYoung(vnum)) || IsIcePhoenix(vnum); }

	static	bool	IsReindeerYoung(DWORD vnum)	{ return 34002 == vnum; }

	static	bool	IsRamadanBlackHorse(DWORD vnum)		{ return 20119 == vnum || 20219 == vnum || 22022 == vnum; }

	static	bool	IsGuildDungeonBoss(uint32_t vnum)
	{
		switch (vnum)
		{
			// dung_1
			case 201025: return true;
			// dung_4
			// dung_5
			// dung_6
			// dung_7
			// dung_8
			// dung_9
			// dung_10
		}

		return false;
	}
};

class CVnumHelper
{
};

#endif	//__HEADER_VNUM_HELPER__
//martysama0134's ceqyqttoaf71vasf9t71218
