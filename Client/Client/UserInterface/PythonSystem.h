#pragma once

class CPythonSystem : public CSingleton<CPythonSystem>
{
	public:
		enum EWindow
		{
			WINDOW_STATUS,
			WINDOW_INVENTORY,
			WINDOW_ABILITY,
			WINDOW_SOCIETY,
			WINDOW_JOURNAL,
			WINDOW_COMMAND,

			WINDOW_QUICK,
			WINDOW_GAUGE,
			WINDOW_MINIMAP,
			WINDOW_CHAT,

			WINDOW_MAX_NUM,
		};

		enum
		{
			FREQUENCY_MAX_NUM  = 30,
			RESOLUTION_MAX_NUM = 100
		};

		typedef struct SResolution
		{
			DWORD	width;
			DWORD	height;
			DWORD	bpp;		// bits per pixel (high-color = 16bpp, true-color = 32bpp)

			DWORD	frequency[20];
			BYTE	frequency_count;
		} TResolution;

		typedef struct SWindowStatus
		{
			int		isVisible;
			int		isMinimized;

			int		ixPosition;
			int		iyPosition;
			int		iHeight;
		} TWindowStatus;

		typedef struct SConfig
		{
			DWORD			width;
			DWORD			height;
			DWORD			bpp;
			DWORD			frequency;

			bool			is_software_cursor;
			bool			is_object_culling;
			int				iDistance;
			int				iShadowLevel;

			FLOAT			music_volume;
			uint8_t			voice_volume;
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
			FLOAT			effects_volume;
#endif

			int				gamma;

			int				isSaveID;
			char			SaveID[20];

			bool			bWindowed;
			bool			bDecompressDDS;
			bool			bNoSoundCard;
			bool			bUseDefaultIME;
			BYTE			bSoftwareTiling;
			bool			bViewChat;
			bool			bAlwaysShowName;
			bool			bShowDamage;
			bool			bShowSalesText;
#ifdef ENABLE_GRAPHIC_ON_OFF
			int				iEffectLevel;
			int				iPrivateShopLevel;
			int				iDropItemLevel;

			bool			bPetStatus;
			bool			bNpcNameStatus;
#endif
#ifdef ENABLE_DOG_MODE
			bool			bDogMode;
#endif
#if defined(ENABLE_FOV_OPTION)
			float 			fFOV;
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
			bool			bShowAutoBuff;
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
			bool			bShowPet;
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
			bool			bShowMount;
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
			float			fOfflineShopRange;
#endif
#ifdef ENABLE_OFFLINE_SHOP
			float			fOfflineShopRange;
#endif
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
			float			fPrivateShopViewDistance;
#endif
#ifdef ENABLE_SKYBOX_SELECT
			BYTE			iSkyBoxSelected;
#endif
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_ENABLE_FOV_OPTION)
			int				iShadowQualityLevel;
			int				iTerrainShadow;
			int				iTreeShadow;
			int				iRainLevel;
			int				iSpecularLevel;
			int				iBloodLevel;
			int				iAmbianceEffectsLevel;
			int				iGraphicsLevel;
			int				iGrassType;
			float			fSharpnessPower;
			float			fLeaves;
#endif

#ifdef STONE_SCALE
			float			fStoneScale;
#endif
#ifdef BOSS_SCALE
			float			fBossScale;
#endif
			int				iQueueEffectType;
		} TConfig;

	public:
		CPythonSystem();
		virtual ~CPythonSystem();

		void Clear();
		void SetInterfaceHandler(PyObject * poHandler);
		void DestroyInterfaceHandler();

		// Config
		void							SetDefaultConfig();
		bool							LoadConfig();
		bool							SaveConfig();
		void							ApplyConfig();
		void							SetConfig(TConfig * set_config);
		TConfig *						GetConfig();
		void							ChangeSystem();

		// Interface
		bool							LoadInterfaceStatus();
		void							SaveInterfaceStatus();
		bool							isInterfaceConfig();
		const TWindowStatus &			GetWindowStatusReference(int iIndex);

		DWORD							GetWidth();
		DWORD							GetHeight();
		DWORD							GetBPP();
		DWORD							GetFrequency();
		bool							IsSoftwareCursor();
		bool							IsWindowed();
		bool							IsViewChat();
		bool							IsAlwaysShowName();
		bool							IsShowDamage();
		bool							IsShowSalesText();
		bool							IsUseDefaultIME();
		bool							IsNoSoundCard();
		bool							IsAutoTiling();
		bool							IsSoftwareTiling();
		void							SetSoftwareTiling(bool isEnable);
		void							SetViewChatFlag(int iFlag);
		void							SetAlwaysShowNameFlag(int iFlag);
		void							SetShowDamageFlag(int iFlag);
		void							SetShowSalesTextFlag(int iFlag);
#ifdef ENABLE_GRAPHIC_ON_OFF
		int	GetEffectLevel();
		void SetEffectLevel(unsigned int level);

		int GetPrivateShopLevel();
		void SetPrivateShopLevel(unsigned int level);

		int GetDropItemLevel();
		void SetDropItemLevel(unsigned int level);

		bool IsPetStatus();
		void SetPetStatusFlag(int iFlag);

		bool IsNpcNameStatus();
		void SetNpcNameStatusFlag(int iFlag);
#endif
#ifdef ENABLE_DOG_MODE
		void							SetDogMode(int iFlag);
		bool							GetDogMode();
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
		void							SetShowAutoBuffMode(int iFlag);
		bool							GetShowAutoBuffMode();
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
		void							SetShowPetMode(int iFlag);
		bool							GetShowPetMode();
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
		void							SetShowMountMode(int iFlag);
		bool							GetShowMountMode();
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		void							SetShowOfflineShopRange(float c_fValue);
		float							GetShowOfflineShopRange();
#endif
#ifdef ENABLE_OFFLINE_SHOP
		void							SetOfflineShopRange(float fValue);
		float							GetOfflineShopRange();
#endif
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_ENABLE_FOV_OPTION)
		float							GetSharpnessPower();
		void							SetSharpnessPower(float fsharpness);
#endif

		// Window
		void							SaveWindowStatus(int iIndex, int iVisible, int iMinimized, int ix, int iy, int iHeight);

		// SaveID
		int								IsSaveID();
		const char *					GetSaveID();
		void							SetSaveID(int iValue, const char * c_szSaveID);

		/// Display
		void							GetDisplaySettings();

		int								GetResolutionCount();
		int								GetFrequencyCount(int index);
		bool							GetResolution(int index, OUT DWORD *width, OUT DWORD *height, OUT DWORD *bpp);
		bool							GetFrequency(int index, int freq_index, OUT DWORD *frequncy);
		int								GetResolutionIndex(DWORD width, DWORD height, DWORD bpp);
		int								GetFrequencyIndex(int res_index, DWORD frequency);
		bool							isViewCulling();

		// Sound
		float							GetMusicVolume();
		int								GetSoundVolume();
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
		int								GetEffectsVolume();
#endif
		void							SetMusicVolume(float fVolume);
		void							SetSoundVolumef(float fVolume);
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
		void							SetEffectsVolumef(float fVolume);
#endif

		int								GetDistance();
#if defined(ENABLE_FOV_OPTION)
		// FoV
		float 							GetFOV();
		void 							SetFOV(float c_fValue);
#endif
		int								GetShadowLevel();
		void							SetShadowLevel(unsigned int level);
#ifdef ENABLE_SKYBOX_SELECT
		void							SetSkyBoxType(BYTE iFlag);
		BYTE							GetSkyBox();
#endif

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	public:
		float							GetPrivateShopViewDistance() { return m_Config.fPrivateShopViewDistance; }
		void							SetPrivateShopViewDistance(float fDistance) { m_Config.fPrivateShopViewDistance = fDistance; }
#endif

	protected:
		TResolution						m_ResolutionList[RESOLUTION_MAX_NUM];
		int								m_ResolutionCount;

		TConfig							m_Config;
		TConfig							m_OldConfig;

		bool							m_isInterfaceConfig;
		PyObject *						m_poInterfaceHandler;
		TWindowStatus					m_WindowStatus[WINDOW_MAX_NUM];

#ifdef STONE_SCALE
	public:
		float GetStoneScale();
		void  SetStoneScale(float fScaleVal);
#endif
#ifdef BOSS_SCALE
	public:
		float GetBossScale();
		void  SetBossScale(float fScaleVal);
#endif

	public:
		int  GetQueueEffectType();
		void SetQueueEffectType(int iType);
};
//martysama0134's ceqyqttoaf71vasf9t71218
