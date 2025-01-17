#include "EnemyManager.h"
#include"Physics.h"
#include"Enemy.h"

EnemyManager::EnemyManager()
{
	
}
EnemyManager::~EnemyManager()
{
}

void EnemyManager::Update()
{
	for (auto& enemy : m_enemies)
	{
		enemy->Update();
	}
}

void EnemyManager::Draw()
{
	for (auto& enemy : m_enemies)
	{
		enemy->Draw();
	}
}

