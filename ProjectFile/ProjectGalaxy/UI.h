#pragma once
#include<string>
#include<memory>
#include<list>
class TextManager;
class TalkObject;
class UI
{
	
public:
	struct UIinfo
	{
		float x;
		float y;
		float width;
		float height;
	};
	UI();
	virtual ~UI();
	static UI& GetInstance();
	void Init();
	void Update();
	
	
	void Draw(float m_hp=0);
	
	/// <summary>
	/// 新しいテキストをぶち込む
	/// </summary>
	/// <param name="text">1行で表示したいテキスト</param>
	void InText(const std::string text);
	/// <summary>
	/// 新しいテキスト群をぶち込む
	/// </summary>
	/// <param name="texts">同じテキストボックス内で表示したいテキスト</param>
	void InTexts(const std::list<std::string> texts);
	/// <summary>
	/// TalkObjectが話す
	/// </summary>
	/// <param name="obj">話させたいオブジェクト</param>
	/// <param name="graphHandle">オブジェクトの画像</param>
	void WannaTalk(std::shared_ptr<TalkObject> obj, int graphHandle);
	/// <summary>
	/// 会話にならない
	/// </summary>
	void TalkExit();
	/// <summary>
	/// 入力を受け付けたら表示しているテキストを削除して次のテキストに移る
	/// </summary>
	void DeleteText();
	/// <summary>
	/// 残りテキスト量を取得
	/// </summary>
	/// <returns>残りテキスト量</returns>
	int TextRemaining();


	using UIstate_t = void(UI::*)();
	UIstate_t m_uiUpdate;
	UIstate_t m_uiDraw;
	
	/// <summary>
	/// 現在のモード
	/// </summary>
	/// <returns>モード</returns>
	UIstate_t GetState() { return m_uiUpdate; }

	void TextBoxUpdate();
	void InputAUpdate();

private:
	/// <summary>
	/// テキストモードに移行
	/// </summary>
	void TextMode();
	/// <summary>
	/// Aボタン表示モードに移行
	/// </summary>
	void InputAMode();
	/// <summary>
	/// 通常UIモードに移行
	/// </summary>
	void NormalMode();

	//通常モード用
	void NormalUpdate();
	void NormalDraw();
	//フェード用
	void AppaerUpdate();
	void FadeOutUpdate();
	//Aボタン表示用
	void InputAFadeDraw();
	void InputADraw();
	//テキスト表示用
	void TextBoxFadeDraw();
	void TextBoxDraw();

	float m_playerHp;

	int m_appearFrame;

	int m_fadeSpeed;

	int m_talkingCharaHandle;
	int m_takasakiTaisaHandle;

	int m_uiAssetHandle;
	int m_uiInputAHandle;

	int m_textBoxSEHandle;

	std::shared_ptr<TextManager> m_textManager;
	std::shared_ptr<TalkObject> m_nowTalkObject;

};

