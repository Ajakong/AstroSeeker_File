#include <DxLib.h>
#include <cassert>
#include "Application.h"
#include "SceneManager.h"
#include "ClearScene.h"
#include "TitleScene.h"
#include"FontManager.h"

#include"Game.h"
#include"Pad.h"
#include"UI.h"

namespace
{
	constexpr int kAppeaInterval = 60;
	constexpr int kMenuMargin = 50;
}

ClearScene::ClearScene(SceneManager& mgr) :
	Scene(mgr),
	m_numFontHandle(FontManager::GetInstance().GetFontData("disital.TTF", "Pocket Calculator", 60, 7, DX_FONTTYPE_NORMAL)),
	m_fontHandle(FontManager::GetInstance().GetFontData("SF_font.ttf", "廻想体 ネクスト UP B", 60, 7, DX_FONTTYPE_NORMAL))
{
	m_frame = 60;
	m_updateFunc = &ClearScene::FadeInUpdate;
	m_drawFunc = &ClearScene::FadeDraw;


}

ClearScene::~ClearScene()
{

}

void ClearScene::Load()
{
}

void ClearScene::Update()
{
	UI::GetInstance().Update();
	(this->*m_updateFunc)();

	Pad::Update();
}

void ClearScene::Draw()
{
	(this->*m_drawFunc)();
	UI::GetInstance().Draw();
}

void ClearScene::FadeInUpdate()
{
	m_frame++;
	if (kAppeaInterval <= m_frame)
	{
		m_updateFunc = &ClearScene::NormalUpdate;
		m_drawFunc = &ClearScene::NormalDraw;
	}
}

void ClearScene::NormalUpdate()
{
	m_btnFrame++;
	if (Pad::IsTrigger(PAD_INPUT_1))
	{
		UI::GetInstance().InText("Congratulations!");
		
		std::list<std::string> clear;
		clear.push_back("よくやった");
		clear.push_back("これで宇宙は救われた");

		UI::GetInstance().InTexts(clear);
		UI::GetInstance().InText("俺もようやく一息つけるってわけだ");

		std::list<std::string> thanks;
		thanks.push_back("これも全部君のおかげだ");
		thanks.push_back("ありがとう");
		UI::GetInstance().InTexts(thanks);

		std::list<std::string> info;
		info.push_back("そうだ、君にも4月まで休暇を与えている。");
		info.push_back("この機にゆっくり休むといい");
		UI::GetInstance().InTexts(info);

		UI::GetInstance().InText("次はAstro Seeker2で会おう");

		m_updateFunc = &ClearScene::FadeOutUpdate;
		m_drawFunc = &ClearScene::FadeDraw;
	}
	
}

void ClearScene::FadeOutUpdate()
{
	m_frame--;
	if (m_frame == 0)
	{
		m_manager.ResetScene(std::make_shared<TitleScene>(m_manager));
	}
}

void ClearScene::ChangeScene(std::shared_ptr<Scene> nextScene)
{
}

void ClearScene::FadeDraw()
{
	Application& app = Application::GetInstance();
	const auto& size = app.GetWindowSize();

	int halfHeight = (size.h - 100) / 2;
	int centerY = size.h / 2;

	float rate = static_cast<float>(m_frame) / kAppeaInterval;	// 現在の時間の割合(0.0～1.0)
	int currentHalfHeight = static_cast<int>(rate * halfHeight);

	SetDrawBlendMode(DX_BLENDMODE_MUL, 255);
	// ちょっと暗い矩形を描画
	DrawBox(kMenuMargin, centerY - currentHalfHeight, size.w - kMenuMargin, centerY + currentHalfHeight,
		0x888888, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawBox(kMenuMargin, centerY - currentHalfHeight, size.w - kMenuMargin, centerY + currentHalfHeight,
		0xffffff, false);
	DrawFormatStringToHandle(Game::kScreenWidth / 2-50, Game::kScreenHeight / 2, 0x00ffff, m_fontHandle , "Clear");

}

void ClearScene::NormalDraw()
{
	Application& app = Application::GetInstance();
	const auto& size = app.GetWindowSize();
	SetDrawBlendMode(DX_BLENDMODE_MUL, 255);
	// ちょっと暗い矩形を描画
	DrawBox(kMenuMargin, kMenuMargin, size.w - kMenuMargin, size.h - kMenuMargin,
		0x888888, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawFormatStringToHandle(Game::kScreenWidth / 2-50, Game::kScreenHeight / 2, 0x00ffff, m_fontHandle, "Clear");
	int idx = m_btnFrame / 10 % 3;
	constexpr int kButtonSize = 16;
	constexpr float kBtnScale = 3.0f;
	
	
}
