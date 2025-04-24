#pragma once
#include<string>
#include<memory>
#include<list>
#include<vector>
class TextManager;
class TalkObject;

//UIの情報群
struct UIinfo
{
	int x;//画像上の表示したいものの左端
	int y;//画像上の表示したいものの上端
	int width;//画像上の表示したいものの横の長さ
	int height;//画像上の表示したいものの縦の長さ
	float extrate;//拡大率
};

class UI
{
public:
	//話すオブジェクトの種類
	enum class TalkGraphKind
	{
		TakasakiTaisa,
		Dekahead_Red,
		Dekahead_Green,
		Dekahead_Yellow,
		Dekehead_Blue,
		Dekahead_White,
		Boss
	};

	using UIstate_t = void(UI::*)();
public:
	UI();
	virtual ~UI();
	static UI& GetInstance();
	void Init();
	void Update();
	void Draw(float hp=0.f,int coinNum=0,bool aimFlag=false, bool isDeath=false);

	
	/// <summary>
	/// 話すオブジェクトのハンドルを設定
	/// </summary>
	/// <param name="handle"></param>
	void SetTalkObjectHandle(TalkGraphKind obj);

	void SetNextTalkObjectHandle(TalkGraphKind obj);

	/// <summary>
	/// 新しいテキストをぶち込む
	/// </summary>
	/// <param name="text">1行で表示したいテキスト</param>
	void InText(const std::string text);

	/// <summary>
	/// テキストデータ群に次に表示したいテキストをぶち込む(全角でぶち込んでください)
	/// </summary>
	/// <param name="text">表示したいテキスト</param>
	void InNextText(const std::string text);

	/// <summary>
	/// 新しいテキスト群をぶち込む
	/// </summary>
	/// <param name="texts">同じテキストボックス内で表示したいテキスト</param>
	void InTexts(const std::list<std::string> texts);
	/// <summary>
	/// 同じテキストボックス内に次に表示したいテキスト群をぶち込む
	/// </summary>
	/// <param name="text">表示したいテキスト群</param>
	void InNextTexts(const std::list<std::string> text);

	/// <summary>
	/// TalkObjectが話す
	/// </summary>
	/// <param name="obj">話させたいオブジェクト</param>
	void WannaTalk(std::shared_ptr<TalkObject> obj);
	/// <summary>
	/// 会話にならない
	/// </summary>
	void TalkExit();
	/// <summary>
	/// 入力を受け付けたら表示しているテキストを削除して次のテキストに移る
	/// </summary>
	void DeleteText();

	void ClearText();
	/// <summary>
	/// 残りテキスト量を取得
	/// </summary>
	/// <returns>残りテキスト量</returns>
	int TextRemaining();

	/// <summary>
	/// 現在のモード
	/// </summary>
	/// <returns>モード</returns>
	UIstate_t GetState() { return m_uiUpdate; }

	void TextBoxUpdate();
	void InputAUpdate();

private:
	

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

	/// <summary>
	/// 通常モード用
	/// </summary>
	void NormalUpdate();
	void NormalDraw();
	
	/// <summary>
	/// フェード用
	/// </summary>
	void AppaerUpdate();
	void FadeOutUpdate();

	/// <summary>
	/// Aボタン表示用
	/// </summary>
	void InputAFadeDraw();
	void InputADraw();
	
	/// <summary>
	/// テキスト表示用
	/// </summary>
	void TextBoxFadeDraw();
	void TextBoxDraw();

	/// <summary>
	/// ミッション描画用
	/// </summary>
	void MissionUpdate();
	void MissionDraw();

private:

	UIstate_t m_uiUpdate;
	UIstate_t m_uiDraw;

	float m_playerHp;

	int m_appearFrame;

	int m_fadeSpeed;

	int m_HPColor;

	int m_changeFrame;

	/// <summary>
	/// 今話している人の顔画像ハンドル
	/// </summary>
	int m_uiTalkingCharaHandle;
	/// <summary>
	/// しゃべる人のハンドル順
	/// </summary>
	std::list<int>m_uiNextTalkCharaHandle;

	/// <summary>
	/// 大佐の顔画像ハンドル
	/// </summary>
	int m_uiTakasakiTaisaHandle;
	
	/// <summary>
	/// パンパーンの色別顔写真集
	/// </summary>
	int m_uiDekahead_RedHandle;
	int m_uiDekahead_GreenHandle;
	int m_uiDekahead_YellowHandle;
	int m_uiDekahead_BlueHandle;
	int m_uiDekahead_WhiteHandle;

	//ボスの顔写真
	int m_uiBossHandle;

	/// <summary>
	/// 常駐UIアセットハンドル
	/// </summary>
	int m_uiAssetHandle;
	/// <summary>
	/// Aボタン画像ハンドル
	/// </summary>
	int m_uiInputAHandle;

	/// <summary>
	/// A連打画像の1つ目のハンドル
	/// </summary>
	int m_uiPushLushInputAButtonHandle;
	/// <summary>
	/// A連打画像の2つ目のハンドル
	/// </summary>
	int m_uiPushLushInputAButton2Handle;

	/// <summary>
	/// 照準画像ハンドル
	/// </summary>
	int m_uiAimGraphHandle;

	int m_uiStarHandle;

	/// <summary>
	/// テキストボックス出現音
	/// </summary>
	int m_textBoxSEHandle;

	/// <summary>
	/// 会話UI出現音
	/// </summary>
	int m_chatAppearSEHandle;

	/// <summary>
	/// HPが少ないことを知らせる音
	/// </summary>
	int m_hpLowerSEHandle;

	std::shared_ptr<TextManager> m_textManager;
	std::shared_ptr<TalkObject> m_nowTalkObject;

};

