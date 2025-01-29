#pragma once
#include<string>
#include<list>
#include <tchar.h>

class TextManager
{
	
public:
	TextManager();
	~TextManager();

	void Init();
	void Update();
	void Draw();

	/// <summary>
	/// テキストデータ群に表示したいテキストをぶち込む(全角でぶち込んでください)
	/// </summary>
	/// <param name="text">表示したいテキスト</param>
	void InText(const std::string text);
	/// <summary>
	/// 同じテキストボックス内に表示したいテキスト群をぶち込む
	/// </summary>
	/// <param name="text">表示したいテキスト群</param>
	void InTexts(const std::list<std::string> text);
	/// <summary>
	/// 表示したいテキスト群を全部ぶち込む
	/// </summary>
	/// <param name="texts">テキストボックスごとに分けたテキスト群</param>
	void SetTexts(const std::list<std::list<std::string>> texts) { m_texts = texts; }

	/// <summary>
	/// 入力を受け付けたら表示しているテキストを削除して次のテキストに移る
	/// </summary>
	void DeleteText();

	/// <summary>
	/// 入ってるテキストデータの残量
	/// </summary>
	/// <returns></returns>
	int GetTextDataSize() { return m_texts.size(); }


private:
	int m_postFrameTextSize;
	int m_fontHandle;
	int m_drawTextLength;
	int m_drawTextFrame;

	int m_inputTextSEHandle;
	/// <summary>
	/// 一つ目のリストがテキストボックス。二つ目が行ごとに分けられる
	/// </summary>
	std::list<std::list<std::string>> m_texts;

};

