#pragma once
#include "Boss.h"
class DecoyBoss : public Boss
{
public:
	DecoyBoss(Vec3 pos, std::shared_ptr<Player>player);
	virtual ~DecoyBoss();

	void Init();
	void Update();
	void Draw();

	void NeutralUpdate();
};

