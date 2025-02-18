#include "GameStopManager.h"
#include"GameManager.h"
GameStopManager::GameStopManager()
{
}

GameStopManager::~GameStopManager()
{
}

GameStopManager& GameStopManager::GetInstance()
{
	static GameStopManager info;
	return info;
}

void GameStopManager::SetGameManager(std::shared_ptr<GameManager> manager)
{
	m_gameManager = manager;
}

void GameStopManager::SetStopFrame(int stopFrame)
{
	m_gameManager->SetUpdateStopFrame(stopFrame);
}
