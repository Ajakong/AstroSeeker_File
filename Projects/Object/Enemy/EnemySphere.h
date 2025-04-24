#pragma once
#include"SphereBase.h"

class Enemy;

/// <summary>
/// Enemyが攻撃の際に生成する球体オブジェクト
/// </summary>
class EnemySphere : public SphereBase
{
public:
	EnemySphere(MyEngine::Collidable::Priority priority, ObjectTag tag,std::shared_ptr<MyEngine::Collidable>enemy, Vec3 pos, Vec3 velocity,Vec3 targetPos, int moveNum, int color = 0xff0000,float impactTime=0);
	virtual ~EnemySphere();

	virtual void Init();
	virtual void Update();
	virtual void Draw();

	virtual void Hit();

	virtual void OnCollideEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag);



protected:
	virtual void  StraightUpdate();//球を直線状に飛ばす
	virtual void  ChaseUpdate();

protected:
	//メンバ関数ポインタ
	using MoveState_t = void(EnemySphere::*)();
	MoveState_t m_moveUpdate;

	std::shared_ptr<Enemy>m_enemy;
	Vec3 m_targetPos;

	float m_impactTime;
private:


private:

};