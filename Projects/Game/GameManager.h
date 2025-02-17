﻿#pragma once
#include"Vec3.h"
#include<memory>
#include<vector>
#include"SerialPlanetGalaxy.h"


class Vec3;
class Player;

class GameManager : public std::enable_shared_from_this<GameManager>
{
public:
	GameManager();
	~GameManager();

	void Init();
	void Update();
	void Draw();

	bool GetGameOver() const { return m_isGameOverFlag; }
	bool GetClear() const { return m_isClearFlag; }

	void SetUpdateStopFrame(int stopFrame) { m_updateStopFrame = stopFrame; }

private:

	std::vector<std::shared_ptr<World::Galaxy>> m_galaxy;
	std::shared_ptr<Player> m_player;

	int m_updateStopFrame;
	//このフラグを見てシーンを移行する
	bool m_isGameOverFlag = false;
	bool m_isClearFlag = false;
};

