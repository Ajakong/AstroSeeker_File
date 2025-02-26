#pragma once
#include "Collidable.h"
class Player;

class SeekerLine : public MyEngine::Collidable
{
public:
	SeekerLine(std::vector<Vec3> points, int color,bool isActive=false);
	virtual ~SeekerLine();

	void Init();
	void Update();
	void Draw();
	Vec3 GetPos() { return m_rigid->GetPos(); }

	virtual void OnTriggerEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag);

private:
	std::vector<Vec3> m_points;
	std::weak_ptr<Player> m_player;
	Vec3 m_playerStartPos;
	Vec3 m_velocity;

	int m_endPointSoundHandle;
	int m_movePlayerSoundHandle;

	//ライントレースのエフェクトのインデックス
	int m_lineEffectIndex;


	int m_color;

	//分岐点の番号
	int m_hitPointNum;

	//プレイヤーを移動させる速度
	float m_speed;

	//プレイヤーを移動させた総量
	float m_totalPlayerMoveNum;
	
	//シーカーラインの長さ
	float m_length;

	int m_gaussFilterScreen;
	int m_highBrightScreen;
};