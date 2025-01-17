#include "ExcelSoundManager.h"
#include "DxLib.h"

#include <cassert>
#include <fstream>
#include <sstream>

ExcelSoundManager* ExcelSoundManager::m_instance = nullptr;

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

void ExcelSoundManager::init()
{
	// cvsファイルの読み込み
	std::ifstream ifs("data/csv/ExcelSoundData.csv");
	std::string line;

	// 見出し行は出力していないので読み捨ては不要
//	// 1行目は読み捨て
//	getline(ifs, line);

	while (getline(ifs, line))
	{
		// csvデータ1行を','で複数の文字列に分割
		std::vector<std::string>	strvec = split(line, ',');

		// strvec[0]	: BGMフラグ		string (0 or 1)
		// strvec[1]	: ファイル名	string
		// strvec[2]	: vol			float

		SoundData data;
		if (strvec[0] == "0")
		{
			data.isBgm = false;
		}
		else if (strvec[0] == "1")
		{
			data.isBgm = true;
		}
		else
		{
			assert(false);
		}
		data.volume = stof(strvec[2]);
		data.handle = LoadSoundMem(strvec[1].c_str());

		m_data.push_back(data);
	}
}

void ExcelSoundManager::end()
{
	// サウンドをメモリから解放
	for (auto& data : m_data)
	{
		DeleteSoundMem(data.handle);
	}
}

void ExcelSoundManager::play(int id)
{
	int result = PlaySoundMem(m_data.at(id).handle, DX_PLAYTYPE_BACK);	// BGMならループ再生とかする
	// ボリュームの設定とかもやる

	if (result == -1)
	{
		printfDx("サウンドが再生できませんでした id = %d\n", id);
	}
}

void ExcelSoundManager::stop(int id)
{
	StopSoundMem(m_data.at(id).handle);
}
