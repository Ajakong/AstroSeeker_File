#include "SoundManager.h"
#include<string>
#include<vector>

using namespace std;

SoundManager::SoundManager():
	m_soundVol(255)
{
}

SoundManager::~SoundManager()
{
}

void SoundManager::ChangeBGM(int handle)
{
	StopSoundMem(m_playingBGMHandle);
	PlaySoundMem(handle, DX_PLAYTYPE_LOOP);
	m_playingBGMHandle = handle;
}

SoundManager& SoundManager::GetInstance()
{
	static SoundManager info;
	return info;
}

void SoundManager::ChangeSoundVolume(int vol)
{
	m_soundVol = vol;
	for (auto& item : m_pathAndSoundInfoes)
	{
		ChangeVolumeSoundMem(vol,item.second.handle);
	}
}

int SoundManager::GetSoundData(const char* soundname)
{
	if (m_pathAndSoundInfoes.find(soundname) == m_pathAndSoundInfoes.end())
	{
		SoundInfo m = SoundInfo();
		string fileName = "Data/Sound/";

		m.handle = DxLib::LoadSoundMem((fileName + (string)soundname).c_str() );
		m.used = false;
		m_pathAndSoundInfoes[soundname] = m;
	}
	else 
	{
		m_pathAndSoundInfoes[soundname].used = true;
	
	}
	ChangeVolumeSoundMem(m_soundVol, m_pathAndSoundInfoes[soundname].handle);
	return m_pathAndSoundInfoes[soundname].handle;
}

void SoundManager::Clear()
{
	for (auto& info : m_pathAndSoundInfoes)
	{
		DeleteSoundMem(info.second.handle);
	}
	m_pathAndSoundInfoes.clear();
}

