﻿#include"Galaxy.h"
#include"ScreenManager.h"
#include"Game.h"

using namespace World;

namespace
{
	const char* kModelScreenName = "ModelScreen";
}

Galaxy::Galaxy(std::shared_ptr<Player> playerPointer) :
	m_isClearFlag(false),
	m_isGameOverFlag(false),
	m_managerUpdate(nullptr),
	m_managerDraw(nullptr)
{
	m_modelScreenHandle = ScreenManager::GetInstance().GetScreenData(kModelScreenName, Game::kScreenWidth, Game::kScreenHeight);
	player = playerPointer;
}

Galaxy::~Galaxy()
{
}
