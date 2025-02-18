#pragma once

#include <vector>
#include <string>

#include "SoundId.h"

// Excelから出力したデータを読み取って管理するクラス
class ExcelSoundManager
{
private:
	ExcelSoundManager() = default;
	virtual ~ExcelSoundManager() = default;

	static ExcelSoundManager* m_instance;

public:
	// コピーコンストラクタの禁止
	ExcelSoundManager(const ExcelSoundManager&) = delete;
	ExcelSoundManager& operator=(const ExcelSoundManager&) = delete;
	ExcelSoundManager(ExcelSoundManager&&) = delete;
	ExcelSoundManager& operator=(ExcelSoundManager&&) = delete;

	static ExcelSoundManager& getInstance()
	{
		if (!m_instance)
		{
			m_instance = new ExcelSoundManager;
		}
		return *m_instance;
	}
	static void destroy()
	{
		delete m_instance;
		m_instance = nullptr;
	}

public:

	// csv読み込み、サウンドのロードを行う
	void init();
	// サウンドの解放を行う
	void end();
	// サウンドの再生
	void play(int id);
	// サウンドの停止
	void stop(int id);

private:
	struct SoundData
	{
		// csvから読み取った情報
		bool isBgm;				// これがBGMか効果音か
		//std::string filename;	// ファイル名
		float volume;

		// 再生データ
		int handle;
	};
private:
	std::vector<SoundData>		m_data;
};

