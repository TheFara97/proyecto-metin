#ifndef __METIN_II_COMMON_BUILDING_H__
#define __METIN_II_COMMON_BUILDING_H__

namespace building
{
	enum
	{
		OBJECT_MATERIAL_MAX_NUM = 5,
	};

	typedef struct SLand
	{
		DWORD	dwID;
		int32_t	lMapIndex;
		int32_t	x, y;
		int32_t	width, height;
		DWORD	dwGuildID;
		BYTE	bGuildLevelLimit;
		DWORD	dwPrice;
	} TLand;

	typedef struct SObjectMaterial
	{
		DWORD	dwItemVnum;
		DWORD	dwCount;
	} TObjectMaterial;

	typedef struct SObjectProto
	{
		DWORD	dwVnum;
		DWORD	dwPrice;

		TObjectMaterial kMaterials[OBJECT_MATERIAL_MAX_NUM];

		DWORD	dwUpgradeVnum;
		DWORD	dwUpgradeLimitTime;
		int32_t	lLife;
		int32_t	lRegion[4];

		DWORD	dwNPCVnum;
		int32_t	lNPCX;
		int32_t	lNPCY;

		DWORD	dwGroupVnum;
		DWORD	dwDependOnGroupVnum;
	} TObjectProto;

	typedef struct SObject
	{
		DWORD	dwID;
		DWORD	dwLandID;
		DWORD	dwVnum;
		int32_t	lMapIndex;
		int32_t	x, y;

		float	xRot;
		float	yRot;
		float	zRot;
		int32_t	lLife;
	} TObject;
};

#endif
//martysama0134's ceqyqttoaf71vasf9t71218
