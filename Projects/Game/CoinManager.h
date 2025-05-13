#pragma once
#include<vector>
#include"Vec3.h"
#include"Coin.h"

class CoinManager
{
public:

	static CoinManager& GetInstance();


	CoinManager();
	virtual ~CoinManager();

	void SetPositionList(std::vector<std::vector<Vec3>> positions);
	void Create(int listNum);


private:
	//道しるべとして配置されるコイン群のリスト
	std::vector<std::vector<Vec3>> m_coinPositionList;
};

