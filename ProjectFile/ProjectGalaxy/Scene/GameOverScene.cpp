#include"GameOverScene.h"
#include"GamePlayingScene.h"
#include"SceneManager.h"
#include"Vec3.h"
#include"Pad.h"
#include"DxLib.h"
#include"UI.h"
#include"SoundManager.h"
#include"Game.h"

namespace
{
	const char* kAstroSeekerThemeName = "AstroSeeker_Theme.mp3";
	//const char* kAstroSeekerRevivalBGMName=""
}

GameOverScene::GameOverScene(SceneManager& mgr) :
	Scene(mgr)
{
	m_themeHandle = SoundManager::GetInstance().GetSoundData(kAstroSeekerThemeName);
	Vec3 centerPos = Vec3(800, 450, 0);
	m_updateFunc = &GameOverScene::FadeInUpdate;
	m_drawFunc = &GameOverScene::FadeDraw;
}

GameOverScene::~GameOverScene()
{
}

void GameOverScene::Load()
{
}

void GameOverScene::Update()
{
	UI::GetInstance().Update();
	(this->*m_updateFunc)();

	

	Pad::Update();
}

void GameOverScene::Draw()
{
	(this->*m_drawFunc)();

	if (Pad::IsTrigger(PAD_INPUT_1)&&Pad::GetState()=="PlayerInput")
	{
		
		ChangeScene(std::make_shared<GamePlayingScene>(m_manager));
	}

	UI::GetInstance().Draw();
}

void GameOverScene::FadeInUpdate()
{
	m_frame--;

	if (m_frame <= 0)
	{

		m_updateFunc = &GameOverScene::NormalUpdate;
		m_drawFunc = &GameOverScene::NormalDraw;
	}
}

void GameOverScene::NormalUpdate()
{
	

}

void GameOverScene::FadeOutUpdate()
{

}

void GameOverScene::ChangeScene(std::shared_ptr<Scene> nextScene)
{
	//StopSoundMem(m_themeHandle);
	m_manager.ChangeScene(nextScene);
}

void GameOverScene::FadeDraw()
{
	DrawFormatString(Game::kScreenWidth/2-100, Game::kScreenHeight, 0xffdddd, "Aでもう一度プレイする");
}

void GameOverScene::FadeOutDraw()
{
	DrawFormatString(Game::kScreenWidth / 2 - 100, Game::kScreenHeight, 0xffdddd, "Aでもう一度プレイする");
}

void GameOverScene::NormalDraw()
{
	DrawFormatString(Game::kScreenWidth / 2 - 100, Game::kScreenHeight, 0xffdddd, "Aでもう一度プレイする");
}
