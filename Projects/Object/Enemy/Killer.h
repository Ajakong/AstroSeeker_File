#pragma once
#include "EnemySphere.h"


class Enemy;
class Player;

class Killer : public EnemySphere
{
public:
	Killer(MyEngine::Collidable::Priority priority, ObjectTag tag, std::shared_ptr<MyEngine::Collidable>enemy,std::shared_ptr<MyEngine::Collidable>target, Vec3 pos, Vec3 velocity, int moveNum, int color);
	~Killer();
	virtual void Init();
	virtual void Update();
	virtual void Draw();

	virtual void Hit();

	virtual void OnCollideEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag);
	void SetCounterFlag() { m_counterFlag = true; }
	Vec3 GetVelocity() { return m_velocity; }
	void SetVelocity(Vec3 vel) { m_velocity = vel; }
	bool GetCounterFlag() { return m_counterFlag; }

protected:
	virtual void RisingUpdate();
	virtual void SearchUpDate();

	virtual void  StraightUpdate();//球を直線状に飛ばす

	virtual void CounterUpdate();

private:
	//メンバ関数ポインタ
	using MoveState_t = void(Killer::*)();
	MoveState_t m_moveUpdate;
	std::shared_ptr<Player> m_target;
	int m_pointeLightHandle;
	int m_risingFrame;

	int m_surviveFrame;
};

