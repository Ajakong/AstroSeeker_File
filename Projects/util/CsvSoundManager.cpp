#include "CsvSoundManager.h"
#include "DxLib.h"

#include <cassert>
#include <fstream>
#include <sstream>

CsvSoundManager* CsvSoundManager::m_instance = nullptr;

namespace
{
	// inputした文字列をdelimiterで分割して
	// 分割後の文字列を返す
	std::vector<std::string> split(std::string& input, char delimiter)
	{
		std::istringstream stream(input);	// stringをgetlineで使えるデータに変換？
		std::string field;					// 分割した文字列1つ分を格納する？
		std::vector<std::string> result;	// 分割後の文字列の配列

		while (getline(stream, field, delimiter))
		{
			result.push_back(field);
		}
		return result;
	}
}

void CsvSoundManager::init()
{
	// cvsファイルの読み込み
	std::ifstream ifs("data/csv/CsvSoundData.csv");
	std::string line;

	// 1行目は読み捨て
	getline(ifs, line);

	while (getline(ifs, line))
	{
		// csvデータ1行を','で複数の文字列に分割
		std::vector<std::string>	strvec = split(line, ',');

		// strvec[0]	: id			string
		// strvec[1]	: BGMフラグ		string (BGM or SE)
		// strvec[2]	: ファイル名	string
		// strvec[3]	: vol			float

		SoundData data;
		if (strvec[1] == "SE")
		{
			data.isBgm = false;
		}
		else if (strvec[1] == "BGM")
		{
			data.isBgm = true;
		}
		else
		{
			assert(false);
		}

		data.volume = stof(strvec[3]);

		data.handle = LoadSoundMem(strvec[2].c_str());
		//printfDx(strvec[2].c_str());
		m_data[strvec[0]] = data;
	}
}

void CsvSoundManager::end()
{
	// サウンドをメモリから解放
	for (auto& data : m_data)
	{
		DeleteSoundMem(data.second.handle);
	}
}

void CsvSoundManager::play(std::string id)
{
//	int result = PlaySoundMem(m_data[id].handle, DX_PLAYTYPE_BACK);	// BGMならループ再生とかする
	int result = PlaySoundMem(m_data.at(id).handle, DX_PLAYTYPE_BACK);	// BGMならループ再生とかする
	// ボリュームの設定とかもやる

	if (result == -1)
	{
		printfDx(id.c_str());
		printfDx(" サウンドが再生できませんでした\n");
	}
}

void CsvSoundManager::stop(std::string id)
{
	StopSoundMem(m_data.at(id).handle);
}
