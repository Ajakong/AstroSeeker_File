#include "GameManager.h"
#include"Physics.h"
#include"SerialPlanetGalaxy.h"
#include"DebugGalaxy.h"
#include"Player.h"
#include"ModelManager.h"
#include"GameStopManager.h"
#include"UI.h"
using namespace MyEngine;

namespace
{
	const char* kPlayerFileName = "SpaceHarrier.mv1";
}

GameManager::GameManager() : 
	m_updateStopFrame(0),
	player(std::make_shared<Player>(ModelManager::GetInstance().GetModelData(kPlayerFileName),Vec3(0,20,0)))
{
	galaxy.push_back(std::make_shared<SerialPlanetGalaxy>(player));
	
}

GameManager::~GameManager()
{
}

void GameManager::Init()
{
	GameStopManager::GetInstance().SetGameManager(shared_from_this());
	galaxy.back()->Init();
}

void GameManager::Update()
{
	UI::GetInstance().Update();
	m_updateStopFrame--;
	if (m_updateStopFrame < 0&&UI::GetInstance().GetState()!=&UI::TextBoxUpdate)
	{
		galaxy.back()->Update();

	}
	
	if (galaxy.back()->GetGameOver())
	{
		m_isGameOverFlag = true;
	}
	if (galaxy.back()->GetClear())
	{
		m_isClearFlag = true;
		
		//galaxy.push_back();/*ここでステージ選択のフィールドを入れなおす*/
	}
	if (galaxy.size() == 0)
	{
		m_isClearFlag = true;
		galaxy.pop_back();
	}
	
}

void GameManager::Draw()
{
	galaxy.back()->Draw();

	UI::GetInstance().Draw((player->GetHp()));
}

void GameManager::IntroUpdate()
{
}

void GameManager::IntroDraw()
{
}

void GameManager::GamePlayingUpdate()
{
}

void GameManager::GamePlayingDraw()
{
}
