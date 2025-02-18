#include "GameManager.h"
#include"Physics.h"
#include"SerialPlanetGalaxy.h"
#include"DebugGalaxy.h"
#include"Player.h"
#include"ModelManager.h"
#include"GameStopManager.h"
#include"UI.h"
#include"Mission.h"
using namespace MyEngine;

namespace
{
	const char* kPlayerFileName = "SpaceHarrier.mv1";
}

GameManager::GameManager() : 
	m_managerUpdate(nullptr),
	m_managerDraw(nullptr),
	m_updateStopFrame(0),
	m_player(std::make_shared<Player>(Vec3(0,20,0)))
{
	m_galaxy.push_back(std::make_shared<SerialPlanetGalaxy>(m_player));
	
}

GameManager::~GameManager()
{
}

void GameManager::Init()
{
	GameStopManager::GetInstance().SetGameManager(shared_from_this());
	m_galaxy.back()->Init();
}

void GameManager::Update()
{
	UI::GetInstance().Update();
	Mission::GetInstance().UpDate();
	m_updateStopFrame--;
	if (m_updateStopFrame < 0&&UI::GetInstance().GetState()!=&UI::TextBoxUpdate)
	{
		m_galaxy.back()->Update();

	}
	
	if (m_galaxy.back()->GetGameOver())
	{
		m_isGameOverFlag = true;
	}
	if (m_galaxy.back()->GetClear())
	{
		m_isClearFlag = true;
		
		//galaxy.push_back();/*ここでステージ選択のフィールドを入れなおす*/
	}
	if (m_galaxy.size() == 0)
	{
		m_isClearFlag = true;
		m_galaxy.pop_back();
	}
	
}

void GameManager::Draw()
{
	m_galaxy.back()->Draw();
	float hp = m_player->GetHp();
	int coinNum = m_player->GetStarNum();
	bool aim = m_player->GetIsAiming();
	bool becameDeath = m_player->GetState() == MyEngine::Collidable::State::Death;

	UI::GetInstance().Draw(hp, coinNum, aim,becameDeath);
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

void GameManager::End()
{
}
