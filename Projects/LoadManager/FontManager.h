#pragma once
#include<map>
#include<string>
#include"DxLib.h"
struct FontInfo 
{
	int handle;//ハンドル
	bool used;//一度以上使用済みか？
};
class FontManager
{
public:
	FontManager();
	~FontManager();

	static FontManager& GetInstance();
	/// <summary>
	/// フォントのハンドルを取得
	/// </summary>
	/// <param name="fontfilename">フォントのファイル名 : ○○.ttfなど</param>
	/// <param name="fontname">フォント自体の名前 : 廻想体 ネクスト UP B</param>
	/// <param name="size">文字の大きさ</param>
	/// <param name="thick">文字の太さ</param>
	/// <param name="fonttype">フォントのタイプ</param>
	/// <returns></returns>
	int GetFontData(const char* fontfilename, const char* fontname, int size = 60, int thick = 7, int fonttype = DX_FONTTYPE_NORMAL);

	void Clear();

private:
	std::map<std::string, FontInfo> m_pathAndFontInfoes;

};
