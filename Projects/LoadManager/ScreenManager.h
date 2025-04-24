#pragma once
#include<map>
#include<string>
#include"DxLib.h"

struct ScreenInfo
{
	int handle;
	bool used;
};
class ScreenManager
{
public:
	ScreenManager();
	~ScreenManager();

	static ScreenManager& GetInstance();
	int GetScreenData(const char* screenname, int width, int height);

	void Clear();

private:
	std::map<std::string, ScreenInfo> m_pathAndScreenInfoes;

};

