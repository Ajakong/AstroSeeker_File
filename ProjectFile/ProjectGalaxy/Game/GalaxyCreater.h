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
	struct KeyLockObject
	{
		std::string name;
		std::string tag;
		Vec3 pos;
		int connectObjectNumber;
	};
	struct LockedObject
	{
		std::string name;
		std::string tag;
		Vec3 pos;
		
	};
public:
	GalaxyCreater();
	virtual ~GalaxyCreater();

	static GalaxyCreater& GetInstance();

	void ObjectCreate(std::shared_ptr<Player> player);
	void SeekerLineCreate();
	void PlanetCreate();
	std::vector<std::shared_ptr<Enemy>> EnemyCreate(std::shared_ptr<Player>player);
	void LockedObjectCreate();
	std::vector<std::shared_ptr<Enemy>> KeyLockObjectCreate();
	void Clear();
	int GetSize() { return m_lockedObjects.size(); }
	std::shared_ptr<MyEngine::Collidable> GetCollidable(int connectNumber) { return m_lockedObjects[connectNumber]; }
	
private:
	std::vector<Location> m_objectData;
	std::vector<LocationPlanet> m_planetData;
	std::vector<int> m_planetModelData;
	std::vector<StandardObject> m_enemyData;
	std::vector<KeyLockObject> m_keyLockObjectData;

	std::vector<LockedObject> m_lockedData;
	std::vector<std::shared_ptr<MyEngine::Collidable>> m_lockedObjects;
};

