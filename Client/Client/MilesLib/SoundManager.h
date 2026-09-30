#pragma once

#include "../eterBase/Singleton.h"

#include "SoundManagerStream.h"
#include "SoundManager2D.h"
#include "SoundManager3D.h"
#include "Type.h"

class CSoundManager : public CSingleton<CSoundManager>
{
public:
	CSoundManager();
	virtual ~CSoundManager();

	BOOL Create();
	void Destroy();

	void SetPosition(float fx, float fy, float fz);
	void SetDirection(float fxDir, float fyDir, float fzDir, float fxUp, float fyUp, float fzUp);
	void Update();

	float GetSoundScale();
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
	float GetEffectsScale();
#endif
	void SetSoundScale(float fScale);
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
	void SetEffectsScale(float fScale);
#endif
	void SetAmbienceSoundScale(float fScale);
	void SetSoundVolume(float fVolume);
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
	void SetEffectsVolume(float fVolume);
#endif
	void SetSoundVolumeRatio(float fRatio);
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
	void SetEffectsVolumeRatio(float fRatio);
#endif
	void SetMusicVolume(float fVolume);
	void SetMusicVolumeRatio(float fRatio);
	void SetSoundVolumeGrade(int iGrade);
	void SetMusicVolumeGrade(int iGrade);
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
	void SetEffectsVolumeGrade(int iGrade);
#endif
	void SaveVolume();
	void RestoreVolume();
	float GetSoundVolume();
	float GetMusicVolume();
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
	float GetEffectsVolume();
#endif

	// Sound
	void PlaySound2D(const char * c_szFileName);
	void PlaySound3D(float fx, float fy, float fz, const char * c_szFileName, int iPlayCount = 1);
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
	void PlayAmbientSound3D(float fx, float fy, float fz, const char* c_szFileName, int iPlayCount = 1);
#endif
	void StopSound3D(int iIndex);
	int  PlayAmbienceSound3D(float fx, float fy, float fz, const char * c_szFileName, int iPlayCount = 1);
	void PlayCharacterSound3D(float fx, float fy, float fz, const char * c_szFileName, BOOL bCheckFrequency = FALSE);
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
	void SetSoundVolume3D(float fx, float fy, float fz, int iIndex, float fVolume);
#else
	void SetSoundVolume3D(int iIndex, float fVolume);
#endif
	void StopAllSound3D();

	// Music
	void PlayMusic(const char * c_szFileName);
	void FadeInMusic(const char * c_szFileName, float fVolumeSpeed = 0.016f);
	void FadeOutMusic(const char * c_szFileName, float fVolumeSpeed = 0.016f);
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
	void PlayMusic3D(DWORD dwIndex, float fx, float fy, float fz, const char* c_szFileName);
	void AdjustActiveSoundsVolume();
#endif
	void FadeLimitOutMusic(const char * c_szFileName, float fLimitVolume, float fVolumeSpeed = 0.016f);
	void FadeOutAllMusic();
	void FadeAll();

	// Sound Node
	void UpdateSoundData(DWORD dwcurFrame, const NSound::TSoundDataVector * c_pSoundDataVector);
	void UpdateSoundData(float fx, float fy, float fz, DWORD dwcurFrame, const NSound::TSoundDataVector * c_pSoundDataVector);
	void UpdateSoundInstance(float fx, float fy, float fz, DWORD dwcurFrame, const NSound::TSoundInstanceVector * c_pSoundInstanceVector, BOOL bCheckFrequency = FALSE);
	void UpdateSoundInstance(DWORD dwcurFrame, const NSound::TSoundInstanceVector * c_pSoundInstanceVector);
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
	void StopMusic(DWORD dwIndex);
#endif

protected:
	enum EMusicState
	{
		MUSIC_STATE_OFF,
		MUSIC_STATE_PLAY,
		MUSIC_STATE_FADE_IN,
		MUSIC_STATE_FADE_OUT,
		MUSIC_STATE_FADE_LIMIT_OUT,
	};
	typedef struct SMusicInstance
	{
		DWORD dwMusicFileNameCRC;
		EMusicState MusicState;
		float fVolume;
		float fLimitVolume;
		float fVolumeSpeed;
	} TMusicInstance;

	void PlayMusic(DWORD dwIndex, const char * c_szFileName, float fVolume, float fVolumeSpeed);
#if !defined(ANK_SYSTEM_GRAPHICS) || !defined(ANKIRA_GRAPHICS_SOUND)
	void StopMusic(DWORD dwIndex);
#endif
	BOOL GetMusicIndex(const char * c_szFileName, DWORD * pdwIndex);

protected:
	float __ConvertGradeVolumeToApplyVolume(int nVolumeGrade);
	float __ConvertRatioVolumeToApplyVolume(float fVolumeRatio);
	void __SetMusicVolume(float fVolume);
	BOOL GetSoundInstance2D(const char * c_szSoundFileName, ISoundInstance ** ppInstance);
	BOOL GetSoundInstance3D(const char * c_szFileName, ISoundInstance ** ppInstance);

protected:
	BOOL							m_bInitialized;
	BOOL							m_isSoundDisable;

	float							m_fxPosition;
	float							m_fyPosition;
	float							m_fzPosition;

	float							m_fSoundScale;
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
	float							m_fEffectsScale;
#endif
	float							m_fAmbienceSoundScale;
	float							m_fSoundVolume;
	float							m_fMusicVolume;
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
	float							m_fEffectsVolume;
#endif

	float							m_fBackupMusicVolume;
	float							m_fBackupSoundVolume;
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_GRAPHICS_SOUND)
	float							m_fEffectsSoundVolume;
#endif

	TMusicInstance					m_MusicInstances[CSoundManagerStream::MUSIC_INSTANCE_MAX_NUM];
	std::map<std::string, float>	m_PlaySoundHistoryMap;

	static CSoundManager2D			ms_SoundManager2D;
	static CSoundManager3D			ms_SoundManager3D;
	static CSoundManagerStream		ms_SoundManagerStream;
};
//martysama0134's ceqyqttoaf71vasf9t71218
