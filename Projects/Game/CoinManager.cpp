#include "CoinManager.h"
#include"Physics.h"

CoinManager& CoinManager::GetInstance()
{
	static CoinManager info;
	return info;
}

CoinManager::CoinManager()
{
}

CoinManager::~CoinManager()
{
}

void CoinManager::SetPositionList(std::vector<std::vector<Vec3>> positions)
{
	m_coinPositionList = positions;
}

void CoinManager::Create(int listNum)
{
	for (auto& item : m_coinPositionList[listNum])
	{
		auto coin = std::make_shared<Coin>(item, false);

		//エフェクト生成

		//コインの生成
		MyEngine::Physics::GetInstance().Entry(coin);
	}
}
