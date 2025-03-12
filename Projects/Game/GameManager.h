#pragma once
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

	bool GetGameOver() const { return m_isGameOver; }
	bool GetClear() const { return m_isClear; }

	/// <summary>
	/// 代入された正の数の数だけオブジェクトやステージの更新を止める(ヒットストップなどに使う)
	/// </summary>
	/// <param name="stopFrame">止めるフレーム</param>
	void SetUpdateStopFrame(int stopFrame) { m_updateStopFrame = stopFrame; }

private:

	//ステージ
	std::vector<std::shared_ptr<World::Galaxy>> m_galaxy;
	//プレイヤー
	std::shared_ptr<Player> m_player;

	int m_updateStopFrame;
	//このフラグを見てシーンを移行する
	bool m_isGameOver = false;
	bool m_isClear = false;
};

