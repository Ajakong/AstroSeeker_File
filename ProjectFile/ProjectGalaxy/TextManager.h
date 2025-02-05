#pragma once
#include<string>
#include<list>
#include <tchar.h>

class TextManager
{
	///本来はここで話してるオブジェクトと話してる内容の情報を入れたい
	struct TalkInfo
	{
		std::string text;
		int talkObjectHandle;
	};
	
public:
	TextManager();
	~TextManager();

	void Init();
	void Update();
	void Draw();

	void TextUpdate() {
		if (m_texts.size() == 0)
		{
			if (m_nextTexts.size() != 0)
			{
				m_texts = m_nextTexts.front();
				m_nextTexts.pop_front();
			}
		}
	};

	/// <summary>
	/// テキストデータ群に表示したいテキストをぶち込む(全角でぶち込んでください)
	/// </summary>
	/// <param name="text">表示したいテキスト</param>
	void InText(const std::string text);
	/// <summary>
	/// テキストデータ群に次に表示したいテキストをぶち込む(全角でぶち込んでください)
	/// </summary>
	/// <param name="text">表示したいテキスト</param>
	void InNextText(const std::string text);

	/// <summary>
	/// 同じテキストボックス内に表示したいテキスト群をぶち込む
	/// </summary>
	/// <param name="text">表示したいテキスト群</param>
	void InTexts(const std::list<std::string> text);
	/// <summary>
	/// 同じテキストボックス内に次に表示したいテキスト群をぶち込む
	/// </summary>
	/// <param name="text">表示したいテキスト群</param>
	void InNextTexts(const std::list<std::string> text);
	/// <summary>
	/// 表示したいテキスト群を全部ぶち込む
	/// </summary>
	/// <param name="texts">テキストボックスごとに分けたテキスト群</param>
	void SetTexts(const std::list<std::list<std::string>> texts);
	/// <summary>
	/// 表示したい次のテキスト群を全部ぶち込む
	/// </summary>
	/// <param name="texts">テキストボックスごとに分けたテキスト群</param>
	void SetNextTexts(const std::list<std::list<std::string>> texts);
	/// <summary>
	/// 入力を受け付けたら表示しているテキストを削除して次のテキストに移る
	/// </summary>
	void DeleteText();

	/// <summary>
	/// 入ってるテキストデータの残量
	/// </summary>
	/// <returns></returns>
	int GetTextDataSize() { return static_cast<int>(m_texts.size()); }


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
	std::list<std::list<std::list<std::string>>> m_nextTexts;

};

