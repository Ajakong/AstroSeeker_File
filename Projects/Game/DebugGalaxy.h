#pragma once
#include "Galaxy.h"

class Camera;
class Player;
class Kuribo;
class Takobo;
class Planet;
class SamuraiAlter;
class Boss;
class UI;
class SeekerLine;

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
	std::shared_ptr<Camera> m_camera;
	std::vector<std::shared_ptr<Kuribo>> m_kuribo;

	std::vector<std::shared_ptr<Planet>> m_planet;
	std::vector<std::shared_ptr<Takobo>> m_takobo;
	std::vector<std::shared_ptr<SamuraiAlter>> m_samuraiAlter;
	std::vector<std::shared_ptr<Boss>> m_boss;

	std::vector<std::shared_ptr<SeekerLine>> m_seekerLine;

};

