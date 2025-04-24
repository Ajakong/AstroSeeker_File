#pragma once
#include"Galaxy.h"
#include"Collidable.h"
class Camera;
class UI;
class GalaxyCreater;

class Collidable;
class Player;
class Planet;
class BossPlanet;
class ClearObject;
class Coin;
class WarpGate;

//ギミック
class Booster;
class StarCapture;
class SeekerLine;
class Crystal;
class Item;
class Cannon;

class Enemy;
class Takobo;
class KillerTheSeeker;
class Gorori;
class Kuribo;
class SpaceEmperor;
class Boss;
class TalkObject;

class SerialPlanetGalaxy :public World::Galaxy//GameManagerが管理,惑星やオブジェクトの情報を持っている
{
public:
	SerialPlanetGalaxy(std::shared_ptr<Player> playerPointer);
	virtual ~SerialPlanetGalaxy();

	void Init();
	void Update();
	void Draw();

private:
	/// <summary>
	/// ステージ紹介の更新処理
	/// </summary>
	void IntroUpdate();
	/// <summary>
	/// ステージ紹介の描画処理
	/// </summary>
	void IntroDraw();

	/// <summary>
	/// ゲームプレイ時の更新処理
	/// </summary>
	void GamePlayingUpdate();

	/// <summary>
	/// ボスバトル時の更新処理
	/// </summary>
	void BossBattleUpdate();

	/// <summary>
	/// ゲームプレイ時の描画処理
	/// </summary>
	void GamePlayingDraw();

	/// <summary>
	/// ボスバトル時の描画処理
	/// </summary>
	void BossBattleDraw();

	template <typename T>
	void DeleteObject(std::vector<std::shared_ptr<T>>& objects);

private:

	struct UserData
	{
		float dissolveY;	// ディゾルヴしたい高さ
		float minY;
		float maxY;
		float dummy;
		float clickedU;
		float clickedV;
		float dummy2[2];
	};

private:
	int cbuffH = CreateShaderConstantBuffer(sizeof(UserData));
	UserData* userData = static_cast<UserData*>(GetBufferShaderConstantBuffer(cbuffH));

	using managerState_t = void(SerialPlanetGalaxy::*)();
	managerState_t m_managerUpdate;

	using managerState_t = void(SerialPlanetGalaxy::*)();
	managerState_t m_managerDraw;

	std::shared_ptr<Camera> m_camera;
	//ステージ
	std::vector<std::shared_ptr<Planet>> m_planet;
	//アイテム
	std::vector<std::shared_ptr<Item>> m_item;
	std::vector<std::shared_ptr<Coin>> m_coin;
	//ギミック
	std::vector<std::shared_ptr<WarpGate>> m_warpGate;
	std::vector<std::shared_ptr<Booster>> m_booster;
	std::vector<std::shared_ptr<StarCapture>> m_starCapture;
	std::vector<std::shared_ptr<SeekerLine>> m_seekerLine;
	std::vector<std::shared_ptr<Crystal>> m_crystal;
	std::vector<std::shared_ptr<Item>> m_poworStone;
	std::vector<std::shared_ptr<Cannon>>m_cannon;
	//エネミー
	std::vector<std::shared_ptr<Enemy>> m_enemies;
	std::shared_ptr<Boss> m_boss;
	std::vector<std::shared_ptr<Enemy>> m_keyLockEnemies;
	//条件を解除すると出現するオブジェクト
	std::vector<std::shared_ptr<MyEngine::Collidable>> m_lockedObject;
	//会話するオブジェクト
	std::vector<std::shared_ptr<TalkObject>> m_talkObjects;
	//ボスが出現したか
	bool m_isBossWatch = false;

	//ハンドル集
	int m_skyDomeH;
	int m_warpEffectHandle;
	int m_bgmHandle;
	int m_bossBattleBgmHandle;
};

