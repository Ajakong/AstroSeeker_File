﻿#include "ScreenManager.h"

ScreenManager::ScreenManager()
{
}

ScreenManager::~ScreenManager()
{
	for (auto& item : m_pathAndScreenInfoes)
	{
		DxLib::DeleteGraph(item.second.handle);
	}
	m_pathAndScreenInfoes.clear();
}

ScreenManager& ScreenManager::GetInstance()
{
	static ScreenManager info;
	return info;
}

int ScreenManager::GetScreenData(const char* screenname,int width, int height)
{
	if (m_pathAndScreenInfoes.find(screenname) == m_pathAndScreenInfoes.end())
	{
		ScreenInfo m = ScreenInfo();
		
		m.handle = DxLib::MakeScreen(width, height,true);
		m.used = false;
		m_pathAndScreenInfoes[screenname] = m;
		return m.handle;
	}
	else 
	{
		m_pathAndScreenInfoes[screenname].used = true;
		return m_pathAndScreenInfoes[screenname].handle;
	}
}

void ScreenManager::Clear()
{
	m_pathAndScreenInfoes.clear();
}
