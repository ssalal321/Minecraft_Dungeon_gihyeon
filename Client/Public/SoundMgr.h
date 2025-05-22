#pragma once

#include <map>

#include "DataEnum.h"

class SoundMgr
{
public:
	static SoundMgr* Get_Instance()
	{
		if (nullptr == m_pInstance)
			m_pInstance = new SoundMgr; 

		return m_pInstance; 
	}
	static void Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance; 
			m_pInstance = nullptr; 
		}
	}

private:
	SoundMgr();
	~SoundMgr();

public:
	void Initialize(); 
	void Release(); 

public:
	void 			PlaySound(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);
	FMOD_CHANNEL* 	PlaySound(const TCHAR* pSoundKey, float fVolume);
	void 			PlayLoopSound(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);
	FMOD_CHANNEL* 	PlayLoopSound(const TCHAR* pSoundKey, float fVolume);
	void 			PlayBGM(const TCHAR* pSoundKey, float fVolume);
	void 			StopSound(CHANNELID eID);
	void			StopSound(FMOD_CHANNEL* channel);
	void 			StopAll();
	void 			SetChannelVolume(CHANNELID eID, float fVolume);
	bool 			IsPlaying(CHANNELID eID);
	bool			IsPlaying(FMOD_CHANNEL* channel);
	void 			UpdateChannelList();

private:
	void LoadSoundFile(); 

private:
	static SoundMgr* m_pInstance;        

	// 사운드 리소스 정보를 갖는 객체 
	std::map<TCHAR*, FMOD_SOUND*> m_mapSound; 
	
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];
	std::list<FMOD_CHANNEL*> m_ChannelList;

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem; 
};

