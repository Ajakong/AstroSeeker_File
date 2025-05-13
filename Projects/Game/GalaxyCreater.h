#pragma once
#include"DxLib.h"
#include"Vec3.h"
#include"Collidable.h"

#include"Booster.h"
#include"Enemy.h"

#include<string>
#include<list>
#include<map>

class Camera;
class Player;


class GalaxyCreater
{
public:
	GalaxyCreater();
	virtual ~GalaxyCreater();

	static GalaxyCreater& GetInstance();

	void SetCamera(std::shared_ptr<Camera>camera) { m_camera = camera; };


	/// <summary>
	/// ステージやオブジェクトを一気に生成(未実装)
	/// </summary>
	/// <param name="player">プレイヤーのポインタ</param>
	/// <param name="galaxyName">ステージの名前</param>
	void GalaxyCreate(std::shared_ptr<Player> player, std::string galaxyName);

	/// <summary>
	/// オブジェクトの生成
	/// </summary>
	/// <param name="player">プレイヤーのポインタ</param>
	void ObjectCreate(std::shared_ptr<Player> player);

	/// <summary>
	/// 惑星(ステージ)の生成
	/// </summary>
	void PlanetCreate();

	/// <summary>
	/// 会話するオブジェクトの生成
	/// </summary>
	void TalkObjectCreate();

	/// <summary>
	/// 条件達成時に起動するオブジェクトの生成(カギのかかったオブジェクト)
	/// </summary>
	/// <returns>生成されたオブジェクトのポインタ</returns>
	std::vector<std::shared_ptr<MyEngine::Collidable>> LockedObjectCreate();

	/// <summary>
	/// 敵の生成
	/// </summary>
	/// <param name="player">プレイヤーのポインタ</param>
	/// <returns>敵のポインタ</returns>
	std::vector<std::shared_ptr<Enemy>> EnemyCreate(std::shared_ptr<Player>player);
	
	/// <summary>
	/// 解除する条件となるオブジェクトの生成
	/// </summary>
	/// <returns>オブジェクトのポインタ</returns>
	std::vector<std::shared_ptr<Enemy>> KeyLockObjectCreate();

	void Clear();
	int GetSize() { return static_cast<int>(m_lockedObjects.size()); }

	/// <summary>
	/// 引数の番号のカギのかかったオブジェクトの取得
	/// </summary>
	/// <param name="connectNumber">解除されたオブジェクトの番号</param>
	/// <returns>そのオブジェクトのポインタ</returns>
	std::shared_ptr<MyEngine::Collidable> GetCollidable(int connectNumber);

	std::vector<std::vector<Vec3>> GetCoinPosition() { return m_coinPositions; };

private:
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

	struct TalkObject
	{
		std::string name;
		std::string tag;
		Vec3 pos;
	};

private:
	std::shared_ptr<Camera> m_camera;
	std::vector<Location> m_objectData;
	std::vector<LocationPlanet> m_planetData;
	std::vector<int> m_planetModelData;
	std::vector<StandardObject> m_enemyData;
	std::vector<KeyLockObject> m_keyLockObjectData;

	std::vector<LockedObject> m_lockedData;
	std::vector<std::vector<Vec3>> m_coinPositions;
	std::vector<std::shared_ptr<MyEngine::Collidable>> m_lockedObjects;

	std::string m_galaxyName;

	int m_createObjectSEHandle;
};

