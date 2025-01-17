#pragma once
#include"DxLib.h"
#include"Vec3.h"
#include<string>
#include<list>
#include<map>
#include"Booster.h"
#include"Collidable.h"
#include"Enemy.h"

class Player;

class GalaxyCreater
{
	struct Location
	{
		std::string name;
		std::string tag;
		Vec3 pos;
		Vec3 rot;
		Vec3 scale;
	};
	struct LocationPlanet
	{
		std::string name;
		Vec3 pos;
		int color;
		float gravityPower;
		std::string modelName;
		float coefficientOfFriction;
		float scale;
	};
	struct LocationSeekerLine
	{
		std::string name;
		std::vector<Vec3> points;
		int color;
	};
	struct StandardObject
	{
		std::string name;
		std::string tag;
		Vec3 pos;
	};
public:
	GalaxyCreater(std::string galaxyname);
	virtual ~GalaxyCreater();

	void ObjectCreate(std::shared_ptr<Player> player);
	void SeekerLineCreate();
	void PlanetCreate();
	std::vector<std::shared_ptr<Enemy>> EnemyCreate(std::shared_ptr<Player>player);

	
private:
	std::string m_galaxyName;
	std::vector<Location> m_objectData;
	std::vector<LocationPlanet> m_planetData;
	std::vector<int> m_planetModelData;
	//std::vector<LocationSeekerLine> m_seekerLineData;
	std::vector<StandardObject> m_enemyData;
};

