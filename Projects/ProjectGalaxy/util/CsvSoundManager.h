#pragma once

#include <vector>
#include <string>
#include <map>


// cvsファイルからデータを読み取って管理するクラス
class CsvSoundManager
{
private:
	CsvSoundManager() = default;
	virtual ~CsvSoundManager() = default;

	static CsvSoundManager* m_instance;

public:
	// コピーコンストラクタの禁止
	CsvSoundManager(const CsvSoundManager&) = delete;
	CsvSoundManager& operator=(const CsvSoundManager&) = delete;
	CsvSoundManager(CsvSoundManager&&) = delete;
	CsvSoundManager& operator=(CsvSoundManager&&) = delete;

	static CsvSoundManager& getInstance()
	{
		if (!m_instance)
		{
			m_instance = new CsvSoundManager;
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
	void play(std::string id);
	// サウンドの停止
	void stop(std::string id);
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
	std::map<std::string, SoundData>		m_data;
};

