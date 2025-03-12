#include "UI.h"
#include"GraphManager.h"
#include"SoundManager.h"
#include"Game.h"
#include"TextManager.h"
#include"Pad.h"
#include"GameStopManager.h"
#include"TalkObject.h"

#include"Mission.h"
namespace
{
	const char* kGraphUIAssetName = "Designer_ui.png";
	const char* kInputAUIName = "PushAbottonForTalk.png";
	const char* kPushLushInputAUIName = "PushLushAbotton.png";
	const char* kPushLushInputAUI2Name = "PushLushAbotton2.png";
	const char* kGraphUIStarName = "Star.png";
	const char* kTakasakiTaisaGraphName = "TakasakiTaisa_talk.png";

	
	const char* kDekahead_RedGraphName = "cuteCreature_red.png";
	const char* kDekahead_GreenGraphName = "cuteCreature_green.png";
	const char* kDekahead_YellowGraphName = "cuteCreature_yellow.png";
	const char* kDekahead_BlueGraphName = "cuteCreature_blue.png";
	const char* kDekahead_WhiteGraphName = "cuteCreature_white.png";
	const char* kBossGraphName = "Boss.png";

	const char* kAimGraphName = "Elements_pro.png";

	const char* kTextBoxIntroSEName = "Mission.mp3";
	const char* kChatAppearSEName = "ChatAppear.mp3";
	const char* kHPLowerSEName = "HPLower.mp3";

	
	const UI::UIinfo kIdemBoxUIInfo{ 0,0,255,255,1.f };
	const UI::UIinfo kHPBarUIInfo { 125,730,820,140,1.f };
	const UI::UIinfo kWindowScreenUIInfo{ 620,15,400,500,0.7f };
	const UI::UIinfo kPushLushInputAUIInfo{ 90,100,835,850,0.3f };
	const UI::UIinfo kPushLushInputAUI2Info{ 90,100,835,850,0.3f };
	const UI::UIinfo kInputAUIInfo{ 200,105,620,695,0.002f };
	const UI::UIinfo kStarUIInfo{ 0,0,810,810,0.1f };

	const UI::UIinfo kTalkingCharaGraph{ 0,0,775,890,0.3f };

	const UI::UIinfo kAimGraph{ 3140 ,200,400,370,0.3f };

	constexpr int kHPBarUIOffsetX = 40;
	constexpr int kHPBarUIOffsetY = 40;
	constexpr int kHPBarUIWidth = 780;
	const int kHPBarUIHeight = static_cast<int>(kHPBarUIInfo.height / 2 + 10);
	constexpr int kHPBarColor = 0x0000044;
	constexpr int kHPBarFillWidthMultiplier = 15;

	constexpr int kTextBoxBackGroundAlpha = 150;//テキスト表示するボックスの透明度　高0～255低


	const int kCoinUIOffsetX = 10;
	const int kCoinUIOffsetY = 15;

	const int kDebugBoxOffsetX = 170;
	const int kDebugBoxOffsetY = 50;

	constexpr int kDangerousHPNum = 20;

	constexpr float kHpDecreaseSpeed = 0.3f;


	/// <summary>
	/// フェード時の描画インターバル
	/// </summary>
	constexpr int kDrawInterval = 60;


	/// <summary>
	/// テキストボックスのフェード速度
	/// </summary>
	constexpr float kTextBoxFadeFrameSpeed = 5.f;
	/// <summary>
	/// Aボタンのフェード速度
	/// </summary>
	constexpr float kInputAFadeFrameSpeed = 20.f;

	constexpr int kTextBoxFrameUp = 90;
	constexpr int kTextBoxFrameDown = 440;
	constexpr int kTextBoxFadeSpeedX = 9;

	constexpr int kNeutralHPGaugeColor = 0x0044ff;
	constexpr int kDangerousHPGaugeColor = 0xff0000;
}

UI::UI():
	//画像ハンドルの初期化
	m_uiStarHandle(-1),
	m_uiAssetHandle(-1),
	m_uiInputAHandle(-1),
	m_uiAimGraphHandle(-1),
	m_uiPushLushInputAButtonHandle(-1),
	m_uiPushLushInputAButton2Handle(-1),

	//キャラクターの画像ハンドルの初期化
	m_uiDekahead_RedHandle(-1),
	m_uiDekahead_BlueHandle(-1),
	m_uiDekahead_GreenHandle(-1),
	m_uiDekahead_WhiteHandle(-1),
	m_uiDekahead_YellowHandle(-1),
	m_uiTalkingCharaHandle(-1),
	m_uiTakasakiTaisaHandle(-1),
	m_uiBossHandle(-1),

	//SEハンドルの初期化
	m_textBoxSEHandle(-1),
	m_hpLowerSEHandle(-1),
	m_chatAppearSEHandle(-1),

	m_fadeSpeed(1),
	m_appearFrame(0),
	m_changeFrame(0),
	m_uiDraw(&UI::NormalDraw),
	m_uiUpdate(&UI::NormalUpdate),

	m_HPColor(0),
	m_playerHp(0)
{
	
}

UI::~UI()
{
}

UI& UI::GetInstance()
{
	static UI info;
	return info;
}

void UI::Init()
{
	//キャラクターの画像ハンドル
	m_uiAssetHandle = GraphManager::GetInstance().GetGraphData(kGraphUIAssetName);
	m_uiInputAHandle = GraphManager::GetInstance().GetGraphData(kInputAUIName);
	m_uiPushLushInputAButtonHandle = GraphManager::GetInstance().GetGraphData(kPushLushInputAUIName);
	m_uiPushLushInputAButton2Handle = GraphManager::GetInstance().GetGraphData(kPushLushInputAUI2Name);
	m_uiStarHandle = GraphManager::GetInstance().GetGraphData("star.png");

	m_uiTakasakiTaisaHandle = GraphManager::GetInstance().GetGraphData(kTakasakiTaisaGraphName);
	
	m_uiDekahead_RedHandle = GraphManager::GetInstance().GetGraphData(kDekahead_RedGraphName);
	m_uiDekahead_GreenHandle = GraphManager::GetInstance().GetGraphData(kDekahead_GreenGraphName);
	m_uiDekahead_BlueHandle = GraphManager::GetInstance().GetGraphData(kDekahead_BlueGraphName);
	m_uiDekahead_WhiteHandle = GraphManager::GetInstance().GetGraphData(kDekahead_WhiteGraphName);
	m_uiDekahead_YellowHandle = GraphManager::GetInstance().GetGraphData(kDekahead_YellowGraphName);

	m_uiBossHandle = GraphManager::GetInstance().GetGraphData(kBossGraphName);

	//UIのデフォルト画像ハンドル
	m_hpLowerSEHandle = SoundManager::GetInstance().GetSoundData(kHPLowerSEName);
	m_uiAimGraphHandle = GraphManager::GetInstance().GetGraphData(kAimGraphName);
	m_textBoxSEHandle = SoundManager::GetInstance().GetSoundData(kTextBoxIntroSEName);
	m_chatAppearSEHandle = SoundManager::GetInstance().GetSoundData(kChatAppearSEName);
	

	m_textManager = std::make_shared<TextManager>();
	NormalMode();
	m_appearFrame = 0;
	m_fadeSpeed = 1;
	m_changeFrame = 0;
	m_HPColor = kNeutralHPGaugeColor;
}

void UI::Update()
{
	if (m_HPColor == kNeutralHPGaugeColor && m_playerHp <= kDangerousHPNum &&m_playerHp!=0)
	{
		PlaySoundMem(m_hpLowerSEHandle, DX_PLAYTYPE_BACK);
		m_HPColor = kDangerousHPGaugeColor;
	}
	else if (m_HPColor == kDangerousHPGaugeColor && m_playerHp > kDangerousHPNum)
	{
		m_HPColor = kNeutralHPGaugeColor;
	}
	(this->*m_uiUpdate)();
	m_changeFrame++;
}

void UI::NormalMode()
{
	Pad::Init();
	m_uiUpdate = &UI::NormalUpdate;
	m_uiDraw = &UI::NormalDraw;
}

void UI::NormalUpdate()
{
	//テキストデータがぶち込まれていたら表示する
	if (m_textManager->GetTextDataSize() != 0)
	{
		//テキスト表示モードに移行
		TextMode();
	}
	
}

void UI::AppaerUpdate()
{
	m_appearFrame+=m_fadeSpeed;

	//テキストモードに移行
	if (m_appearFrame >= kDrawInterval &&m_uiDraw==&UI::TextBoxFadeDraw)
	{
		
		m_uiUpdate = &UI::TextBoxUpdate;
		m_uiDraw = &UI::TextBoxDraw;
	}
	//Aボタン表示モードに移行
	if (m_appearFrame >= kDrawInterval && m_uiDraw == &UI::InputAFadeDraw)
	{
		
		m_uiUpdate = &UI::InputAUpdate;
		m_uiDraw = &UI::InputADraw;
	}
}

void UI::TextBoxUpdate()
{
	//Aでテキストを進める
	if (Pad::IsTrigger(PAD_INPUT_1))
	{
		DeleteText();
	}
	//表示させるテキストがない場合
	if (m_textManager->GetTextDataSize() == 0)
	{
		
		m_textManager->TextUpdate();
		if (m_uiNextTalkCharaHandle.size() != 0)
		{
			m_uiTalkingCharaHandle = m_uiNextTalkCharaHandle.front();
			m_uiNextTalkCharaHandle.pop_front();

		}
		if (m_textManager->GetTextDataSize() == 0)
		{
			//テキストボックスのフェードアウトヘ以降
			m_uiUpdate = &UI::FadeOutUpdate;
			m_uiDraw = &UI::TextBoxFadeDraw;
		}
		
	}
}

void UI::InputAUpdate()
{
	if (Pad::IsTrigger(PAD_INPUT_1))
	{
		//パッドをテキスト入力モードに移行
		Pad::SetState("TextInput");
		m_textManager->SetTexts(m_nowTalkObject->GetTexts());
		m_uiUpdate = &UI::FadeOutUpdate;
		m_uiDraw = &UI::InputAFadeDraw;
	}
}

void UI::TextMode()
{
	PlaySoundMem(m_textBoxSEHandle,DX_PLAYTYPE_BACK);

	//パッドをテキスト入力モードに移行
	Pad::SetState("TextInput");
	m_uiUpdate = &UI::AppaerUpdate;
	m_uiDraw = &UI::TextBoxFadeDraw;
	m_fadeSpeed = kTextBoxFadeFrameSpeed;
}

void UI::InputAMode()
{
	PlaySoundMem(m_chatAppearSEHandle, DX_PLAYTYPE_BACK);

	//Aボタンで話しかけるUI表示モード
	m_uiUpdate = &UI::AppaerUpdate;
	m_uiDraw = &UI::InputAFadeDraw;
	m_fadeSpeed = kInputAFadeFrameSpeed;
}


void UI::FadeOutUpdate()
{
	m_appearFrame-=m_fadeSpeed;

	//Aボタン表示モードなら
	if (m_uiDraw == &UI::InputAFadeDraw)
	{
		//フェード終了時
		if (m_appearFrame <= 0)
		{
			//テキストデータがあればテキストモード
			if (m_textManager->GetTextDataSize() != 0)TextMode();
			//なければ通常UIモード
			else NormalMode();
		}
	}
	//テキストモードなら
	else
	{
		//フェード終了時
		if (m_appearFrame <= 0)
		{
			NormalMode();
		}
	}
	
}

void UI::Draw(float hp, int coinNum, bool aimFlag,bool isDeath)
{
	m_playerHp = hp;

	//プレイヤーのHPが残っているなら
	if (m_playerHp > 0)
	{
		DrawBox(kHPBarUIOffsetX, kHPBarUIOffsetY, kHPBarUIWidth, kHPBarUIHeight, 0x0000044, true);
		
		DrawBox(kHPBarUIOffsetX, kHPBarUIOffsetY,
			kHPBarUIOffsetX + kHPBarFillWidthMultiplier * static_cast<int>(m_playerHp),//プレイヤーのHP残量に合わせてHPバーの長さを調節
			kHPBarUIHeight, m_HPColor, true);

		DrawRectRotaGraphF(static_cast<float>(kHPBarUIInfo.width / 2), static_cast<float>(kHPBarUIInfo.height / 2), kHPBarUIInfo.x, kHPBarUIInfo.y, kHPBarUIInfo.width, kHPBarUIInfo.height, kHPBarUIInfo.extrate, 0, m_uiAssetHandle, true);
#ifdef DEBUG
		DrawRectRotaGraph(Game::kScreenWidth - kIdemBoxUIInfo.width / 2 - 170, kIdemBoxUIInfo.height / 2 + 50, kIdemBoxUIInfo.x, kIdemBoxUIInfo.y, kIdemBoxUIInfo.width, kIdemBoxUIInfo.height, 1, 0, m_uiAssetHandle, true);
#endif

	}

	//プレイヤーがコインを持っているなら
	if (coinNum > 0)
	{
		DrawRectRotaGraphF(static_cast<float>(kStarUIInfo.width / 2)/ kCoinUIOffsetX + static_cast<float>(kHPBarUIInfo.width), static_cast<float>(kStarUIInfo.height / 2)/ (kStarUIInfo.extrate / 1) + kCoinUIOffsetY, kStarUIInfo.x, kStarUIInfo.y, kStarUIInfo.width, kStarUIInfo.height, kStarUIInfo.extrate, 0, m_uiStarHandle, true);
		DrawFormatString(static_cast<int>(kStarUIInfo.width)/ kCoinUIOffsetX + static_cast<int>(kHPBarUIInfo.width), static_cast<float>(kStarUIInfo.height / 2) / (kStarUIInfo.extrate/1) + kCoinUIOffsetY, 0xffffff, "x%d", coinNum);
	}

	(this->*m_uiDraw)();

	//エイム時なら
	if (aimFlag)DrawRectRotaGraph(Game::kScreenWidth/2, Game::kScreenHeight/2, kAimGraph.x, kAimGraph.y, kAimGraph.width, kAimGraph.height, kAimGraph.extrate, 0, m_uiAimGraphHandle, true);
	
	//死んでいたら
	if (isDeath)
	{
		if (m_changeFrame % 2 == 0)
		{
			DrawRectRotaGraph(
				Game::kScreenWidth / 2,//画面に表示したいX軸上の場所(画像の中心)
				Game::kScreenHeight / 2,//画面に表示したいY軸上の場所(画像の中心)
				kPushLushInputAUIInfo.x,//表示したいのモノの画像データ上の左端
				kPushLushInputAUIInfo.y,//表示したいのモノの画像データ上の上端
				kPushLushInputAUIInfo.width,//表示したいモノの画像データ上の横の長さ
				kPushLushInputAUIInfo.height,//表示したいモノの画像データ上の縦の長さ
				kPushLushInputAUIInfo.extrate,//表示したいモノの拡大率
				0,//角度
				m_uiPushLushInputAButtonHandle,//画像ハンドル
				true//透過するか
			);
		}
		else
		{
			DrawRectRotaGraph(
				Game::kScreenWidth / 2,//画面に表示したいX軸上の場所(画像の中心)
				Game::kScreenHeight / 2,//画面に表示したいY軸上の場所(画像の中心)
				kPushLushInputAUI2Info.x,//表示したいのモノの画像データ上の左端
				kPushLushInputAUI2Info.y,//表示したいのモノの画像データ上の上端
				kPushLushInputAUI2Info.width,//表示したいモノの画像データ上の横の長さ
				kPushLushInputAUI2Info.height,//表示したいモノの画像データ上の縦の長さ
				kPushLushInputAUI2Info.extrate,//表示したいモノの拡大率
				0,//角度
				m_uiPushLushInputAButton2Handle,//画像ハンドル
				true//透過するか
			);
		}
		
	}
}

void UI::NormalDraw()
{
}

void UI::InputAFadeDraw()
{
	DrawRectRotaGraphF(static_cast<float>(Game::kScreenWidth/2), static_cast<float>(Game::kScreenHeight/2), kInputAUIInfo.x, kInputAUIInfo.y, kInputAUIInfo.width, kInputAUIInfo.height, m_appearFrame* kInputAUIInfo.extrate, 0, m_uiInputAHandle, true);
}

void UI::InputADraw()
{
	DrawRectRotaGraphF(static_cast<float>(Game::kScreenWidth / 2), static_cast<float>(Game::kScreenHeight / 2), kInputAUIInfo.x, kInputAUIInfo.y, kInputAUIInfo.width, kInputAUIInfo.height, m_appearFrame * kInputAUIInfo.extrate, 0, m_uiInputAHandle, true);
}

void UI::TextBoxFadeDraw()
{
	DrawBox(0, 0, Game::kScreenWidth, m_appearFrame, 0x000000, true);
	DrawBox(0, Game::kScreenHeight, Game::kScreenWidth, Game::kScreenHeight-m_appearFrame, 0x000000, true);


	SetDrawBlendMode(DX_BLENDMODE_MULA, kTextBoxBackGroundAlpha);
	DrawBox(Game::kScreenWidth / 2 - m_appearFrame * kTextBoxFadeSpeedX, kTextBoxFrameUp, Game::kScreenWidth / 2 + m_appearFrame * kTextBoxFadeSpeedX, kTextBoxFrameDown, 0x111111, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawBox(Game::kScreenWidth / 2 - m_appearFrame * kTextBoxFadeSpeedX, kTextBoxFrameUp, Game::kScreenWidth / 2 + m_appearFrame * kTextBoxFadeSpeedX, kTextBoxFrameDown, 0x0000ff, false);
}

void UI::TextBoxDraw()
{
	DrawBox(0, 0, Game::kScreenWidth, m_appearFrame, 0x000000, true);
	DrawBox(0, Game::kScreenHeight, Game::kScreenWidth, Game::kScreenHeight - m_appearFrame, 0x000000, true);
	
	SetDrawBlendMode(DX_BLENDMODE_MULA, kTextBoxBackGroundAlpha);
	DrawBox(Game::kScreenWidth / 2 - m_appearFrame * kTextBoxFadeSpeedX, kTextBoxFrameUp, Game::kScreenWidth / 2 + m_appearFrame * kTextBoxFadeSpeedX, kTextBoxFrameDown, 0x111111, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawBox(Game::kScreenWidth / 2 - m_appearFrame * kTextBoxFadeSpeedX, kTextBoxFrameUp, Game::kScreenWidth / 2 + m_appearFrame * kTextBoxFadeSpeedX, kTextBoxFrameDown, 0x0000ff, false);

	DrawRectRotaGraph((kWindowScreenUIInfo.width / 2) + (Game::kScreenWidth*0.6f), (kWindowScreenUIInfo.height / 2)+20, kTalkingCharaGraph.x, kTalkingCharaGraph.y, kTalkingCharaGraph.width, kTalkingCharaGraph.height, kTalkingCharaGraph.extrate, 0, m_uiTalkingCharaHandle, true);

	DrawRectRotaGraph((kWindowScreenUIInfo.width / 2)+ (Game::kScreenWidth*0.6f), (kWindowScreenUIInfo.height / 2)+50, kWindowScreenUIInfo.x, kWindowScreenUIInfo.y, kWindowScreenUIInfo.width, kWindowScreenUIInfo.height, kWindowScreenUIInfo.extrate, 0, m_uiAssetHandle, true);
	m_textManager->Draw();
}

void UI::MissionUpdate()
{
}

void UI::MissionDraw()
{
}

void UI::SetTalkObjectHandle(TalkGraphKind obj)
{
	switch (obj)
	{
	case TalkGraphKind::TakasakiTaisa:m_uiTalkingCharaHandle = m_uiTakasakiTaisaHandle;
		break;
	case TalkGraphKind::Dekahead_Red:m_uiTalkingCharaHandle = m_uiDekahead_RedHandle;
		break;
	case TalkGraphKind::Dekahead_Green:m_uiTalkingCharaHandle = m_uiDekahead_GreenHandle;
		break;
	case TalkGraphKind::Dekahead_Yellow:m_uiTalkingCharaHandle = m_uiDekahead_YellowHandle;
		break;
	case TalkGraphKind::Dekehead_Blue:m_uiTalkingCharaHandle = m_uiDekahead_BlueHandle;
		break;
	case TalkGraphKind::Dekahead_White:m_uiTalkingCharaHandle = m_uiDekahead_WhiteHandle;
		break;
	case TalkGraphKind::Boss:m_uiTalkingCharaHandle = m_uiBossHandle;
		break;
	}
}

void UI::SetNextTalkObjectHandle(TalkGraphKind obj)
{
	switch (obj)
	{
	case TalkGraphKind::TakasakiTaisa:m_uiNextTalkCharaHandle.push_back(m_uiTakasakiTaisaHandle);
		break;
	case TalkGraphKind::Dekahead_Red:m_uiNextTalkCharaHandle.push_back(m_uiDekahead_RedHandle);
		break;
	case TalkGraphKind::Dekahead_Green:m_uiNextTalkCharaHandle.push_back(m_uiDekahead_GreenHandle);
		break;
	case TalkGraphKind::Dekahead_Yellow:m_uiNextTalkCharaHandle.push_back(m_uiDekahead_YellowHandle);
		break;
	case TalkGraphKind::Dekehead_Blue:m_uiNextTalkCharaHandle.push_back(m_uiDekahead_BlueHandle);
		break;
	case TalkGraphKind::Dekahead_White:m_uiNextTalkCharaHandle.push_back(m_uiDekahead_WhiteHandle);
		break;
	case TalkGraphKind::Boss:m_uiNextTalkCharaHandle.push_back(m_uiBossHandle);
		break;
	}
}

void UI::InText(const std::string text)
{
	m_textManager->InText(text);
}

void UI::InNextText(const std::string text)
{
	m_textManager->InNextText(text);
}

void UI::InTexts(const std::list<std::string> texts)
{
	m_textManager->InTexts(texts);
}

void UI::InNextTexts(const std::list<std::string> text)
{
	m_textManager->InNextTexts(text);
}

void UI::WannaTalk(std::shared_ptr<TalkObject> obj)
{
	m_nowTalkObject = obj;
	InputAMode();
}

void UI::TalkExit()
{
	if (m_appearFrame <= 0)
	{
		NormalMode();
	}
	else
	{
		//テキストボックスのフェードアウトヘ以降
		m_uiUpdate = &UI::FadeOutUpdate;
		m_uiDraw = &UI::InputAFadeDraw;
	}
}

void UI::DeleteText()
{
	m_textManager->DeleteText();
}

void UI::ClearText()
{
	m_textManager->ClearText();
}

int UI::TextRemaining()
{
	return m_textManager->GetTextDataSize();
}
