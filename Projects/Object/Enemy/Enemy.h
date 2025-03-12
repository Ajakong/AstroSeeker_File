#pragma once
#include "Collidable.h"
#include"Quaternion.h"

class EnemyMover;
class EnemyAI;

class Enemy : public MyEngine :: Collidable
{
public:
	Enemy(Priority priority,ObjectTag tag);
	virtual ~Enemy();

	virtual void Init() = 0;
	virtual void Draw() = 0;
	virtual void Update()=0;
	virtual void SetMatrix() {};
	virtual Vec3 GetShotUpVec() { return m_upVec; }
	virtual int GetHP() { return m_hp; }
protected:
	int m_hp;
	int m_modelHandle;
	Quaternion m_myQ;
};

