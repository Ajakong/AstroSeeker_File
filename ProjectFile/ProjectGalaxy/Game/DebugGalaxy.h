#pragma once
#include "Galaxy.h"

class Camera;
class Player;
class Takobo;
class Planet;
class SamuraiAlter;
class Boss;
class UI;

class DebugGalaxy : public World::Galaxy
{
public:
	DebugGalaxy(std::shared_ptr<Player> playerPointer);
	virtual ~DebugGalaxy();

	void Init();
	void Update();
	void Draw();

	using managerState_t = void(DebugGalaxy::*)();
	managerState_t m_managerUpdate;

	using managerState_t = void(DebugGalaxy::*)();
	managerState_t m_managerDraw;

private:
	std::shared_ptr<UI> m_ui;
	std::shared_ptr<Camera> camera;
	std::vector<std::shared_ptr<Planet>> m_planet;
	std::vector<std::shared_ptr<Takobo>> takobo;
	std::vector<std::shared_ptr<SamuraiAlter>> samuraiAlter;
	std::vector<std::shared_ptr<Boss>> boss;

};

