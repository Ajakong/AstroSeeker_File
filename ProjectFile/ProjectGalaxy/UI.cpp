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

	
	const UI::UIinfo kIdemBoxUIInfo{ 0,0,255,255 };
	const UI::UIinfo kHPBarUIInfo { 125,730,820,140 };
	const UI::UIinfo kWindowScreenUIInfo{ 620,15,400,500 };
	const UI::UIinfo kInputAUIInfo{ 200,105,620,695 };
	const UI::UIinfo kTalkingCharaGraph{ 0,0,775,890 };

	const UI::UIinfo kAimGraph{ 3140 ,200,400,370 };

	constexpr float kHpDecreaseSpeed = 0.3f;


	/// <summary>
	/// フェード時の描画インターバル
	/// </summary>
	constexpr int kDrawInterval = 60;


	/// <summary>
	/// テキストボックスのフェード速度
	/// </summary>
	constexpr int kTextBoxFadeFrameSpeed = 5.f;
	/// <summary>
	/// Aボタンのフェード速度
	/// </summary>
	constexpr int kInputAFadeFrameSpeed = 20.f;
}

UI::UI():
	m_fadeSpeed(1),
	m_appearFrame(0),
	m_uiUpdate(&UI::NormalUpdate),
	m_uiDraw(&UI::NormalDraw),
	m_uiAssetHandle(-1),
	m_uiInputAHandle(-1),
	m_uiAimGraphHandle(-1),
	m_uiTakasakiTaisaHandle(-1),
	m_uiTalkingCharaHandle(-1),
	m_textBoxSEHandle(-1),
	m_chatAppearSEHandle(-1),
	m_hpLowerSEHandle(-1),
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
	m_uiAssetHandle = GraphManager::GetInstance().GetGraphData(kGraphUIAssetName);
	m_uiInputAHandle = GraphManager::GetInstance().GetGraphData(kInputAUIName);

	m_uiTakasakiTaisaHandle = GraphManager::GetInstance().GetGraphData(kTakasakiTaisaGraphName);
	
	m_uiDekahead_RedHandle = GraphManager::GetInstance().GetGraphData(kDekahead_RedGraphName);
	m_uiDekahead_GreenHandle = GraphManager::GetInstance().GetGraphData(kDekahead_GreenGraphName);
	m_uiDekahead_YellowHandle = GraphManager::GetInstance().GetGraphData(kDekahead_YellowGraphName);
	m_uiDekahead_BlueHandle = GraphManager::GetInstance().GetGraphData(kDekahead_BlueGraphName);
	m_uiDekahead_WhiteHandle = GraphManager::GetInstance().GetGraphData(kDekahead_WhiteGraphName);

	m_uiBossHandle = GraphManager::GetInstance().GetGraphData(kBossGraphName);


	m_uiAimGraphHandle = GraphManager::GetInstance().GetGraphData(kAimGraphName);
	m_textBoxSEHandle = SoundManager::GetInstance().GetSoundData(kTextBoxIntroSEName);
	m_chatAppearSEHandle = SoundManager::GetInstance().GetSoundData(kChatAppearSEName);
	m_hpLowerSEHandle = SoundManager::GetInstance().GetSoundData(kHPLowerSEName);
	m_textManager = std::make_shared<TextManager>();
	NormalMode();
	m_appearFrame = 0;
	m_fadeSpeed = 1;
	m_HPColor = 0x00044ff;
}

void UI::Update()
{
	if (m_HPColor == 0x00044ff && m_playerHp <= 20&&m_playerHp!=0)
	{
		PlaySoundMem(m_hpLowerSEHandle, DX_PLAYTYPE_BACK);
		m_HPColor = 0xff0000;
	}
	else if (m_HPColor == 0xff0000 && m_playerHp > 20)
	{
		m_HPColor = 0x00044ff;
	}
	(this->*m_uiUpdate)();

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
		Pad::SetState("TextInput");
		m_textManager->SetTexts(m_nowTalkObject->GetTexts());
		m_uiUpdate = &UI::FadeOutUpdate;
		m_uiDraw = &UI::InputAFadeDraw;
	}
}

void UI::TextMode()
{
	PlaySoundMem(m_textBoxSEHandle,DX_PLAYTYPE_BACK);
	m_fadeSpeed = kTextBoxFadeFrameSpeed;
	Pad::SetState("TextInput");
	m_uiUpdate = &UI::AppaerUpdate;
	m_uiDraw = &UI::TextBoxFadeDraw;
}

void UI::InputAMode()
{
	PlaySoundMem(m_chatAppearSEHandle, DX_PLAYTYPE_BACK);
	m_fadeSpeed = kInputAFadeFrameSpeed;
	
	m_uiUpdate = &UI::AppaerUpdate;
	m_uiDraw = &UI::InputAFadeDraw;
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

void UI::Draw(float hp, bool aimFlag)
{
	m_playerHp = hp;

	if (m_playerHp > 0)
	{
		DrawBox(40, 40, 780, static_cast<int>(kHPBarUIInfo.height / 2 + 10), 0x0000044, true);
		
		DrawBox(40, 40, 40 + 15 * static_cast<int>(m_playerHp), kHPBarUIInfo.height / 2 + 10, m_HPColor, true);

		DrawRectRotaGraphF(static_cast<float>(kHPBarUIInfo.width / 2), static_cast<float>(kHPBarUIInfo.height / 2), kHPBarUIInfo.x, kHPBarUIInfo.y, kHPBarUIInfo.width, kHPBarUIInfo.height, 1, 0, m_uiAssetHandle, true);
#ifdef DEBUG
		DrawRectRotaGraph(Game::kScreenWidth - kIdemBoxUIInfo.width / 2 - 170, kIdemBoxUIInfo.height / 2 + 50, kIdemBoxUIInfo.x, kIdemBoxUIInfo.y, kIdemBoxUIInfo.width, kIdemBoxUIInfo.height, 1, 0, m_uiAssetHandle, true);
#endif

	}
	(this->*m_uiDraw)();

	if (aimFlag)DrawRectRotaGraph(Game::kScreenWidth/2, Game::kScreenHeight/2, kAimGraph.x, kAimGraph.y, kAimGraph.width, kAimGraph.height, 0.3, 0, m_uiAimGraphHandle, true);
}

void UI::NormalDraw()
{
	
#ifdef DEBUG
	DrawRectRotaGraph(Game::kScreenWidth - kIdemBoxUIInfo.width / 2 - 170, kIdemBoxUIInfo.height / 2 + 50, kIdemBoxUIInfo.x, kIdemBoxUIInfo.y, kIdemBoxUIInfo.width, kIdemBoxUIInfo.height, 1, 0, m_uiAssetHandle, true);
#endif
	/*DrawRectRotaGraph( Game::kScreenWidth-kWindowScreenUIInfo.width / 2, kWindowScreenUIInfo.height / 2, kWindowScreenUIInfo.x, kWindowScreenUIInfo.y, kWindowScreenUIInfo.width, kWindowScreenUIInfo.height, 1, 0, m_uiAssetHandle, true);*/

}

void UI::InputAFadeDraw()
{
	DrawRectRotaGraphF(static_cast<float>(Game::kScreenWidth/2), static_cast<float>(Game::kScreenHeight/2), kInputAUIInfo.x, kInputAUIInfo.y, kInputAUIInfo.width, kInputAUIInfo.height, m_appearFrame*0.002f, 0, m_uiInputAHandle, true);
}

void UI::InputADraw()
{
	DrawRectRotaGraphF(static_cast<float>(Game::kScreenWidth / 2), static_cast<float>(Game::kScreenHeight / 2), kInputAUIInfo.x, kInputAUIInfo.y, kInputAUIInfo.width, kInputAUIInfo.height, m_appearFrame * 0.002f, 0, m_uiInputAHandle, true);
}

void UI::TextBoxFadeDraw()
{
	DrawBox(0, 0, Game::kScreenWidth, m_appearFrame, 0x000000, true);
	DrawBox(0, Game::kScreenHeight, Game::kScreenWidth, Game::kScreenHeight-m_appearFrame, 0x000000, true);


	SetDrawBlendMode(DX_BLENDMODE_MULA, 150);
	DrawBox(Game::kScreenWidth / 2 - m_appearFrame * 9, 90, Game::kScreenWidth / 2 + m_appearFrame * 9, 440, 0x111111, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawBox(Game::kScreenWidth / 2 - m_appearFrame * 9, 90, Game::kScreenWidth / 2 + m_appearFrame * 9, 440, 0x0000ff, false);
}

void UI::TextBoxDraw()
{
	DrawBox(0, 0, Game::kScreenWidth, m_appearFrame, 0x000000, true);
	DrawBox(0, Game::kScreenHeight, Game::kScreenWidth, Game::kScreenHeight - m_appearFrame, 0x000000, true);
	
	SetDrawBlendMode(DX_BLENDMODE_MULA, 150);
	DrawBox(Game::kScreenWidth / 2 - m_appearFrame * 9, 90, Game::kScreenWidth / 2 + m_appearFrame * 9, 440, 0x111111, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawBox(Game::kScreenWidth / 2 - m_appearFrame * 9, 90, Game::kScreenWidth / 2 + m_appearFrame * 9, 440, 0x0000ff, false);

	DrawRectRotaGraph((kWindowScreenUIInfo.width / 2) + (Game::kScreenWidth*0.6f), (kWindowScreenUIInfo.height / 2)+20, kTalkingCharaGraph.x, kTalkingCharaGraph.y, kTalkingCharaGraph.width, kTalkingCharaGraph.height, 0.3f, 0, m_uiTalkingCharaHandle, true);

	DrawRectRotaGraph((kWindowScreenUIInfo.width / 2)+ (Game::kScreenWidth*0.6f), (kWindowScreenUIInfo.height / 2)+50, kWindowScreenUIInfo.x, kWindowScreenUIInfo.y, kWindowScreenUIInfo.width, kWindowScreenUIInfo.height, 0.7f, 0, m_uiAssetHandle, true);
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

int UI::TextRemaining()
{
	return m_textManager->GetTextDataSize();
}
