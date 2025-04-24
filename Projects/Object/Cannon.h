#pragma once
#include"Collidable.h"
#include"ColliderSphere.h"
#include<memory>
#include<vector>
#include"Quaternion.h"


class Killer;
class CannonSphere;


class Cannon : public MyEngine::Collidable
{
public:
	Cannon(Vec3 pos,ObjectTag shotTag, int bulletColor);
	virtual ~Cannon();

	void Init();
	void Update();
	void Draw();

	void SetMatrix();
	
	virtual void OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);

private:
	ObjectTag m_bulletTag;
	std::vector<std::shared_ptr<CannonSphere>> m_cannonSphere;
	int m_createBulletFrame;
	int m_modelHandle;
	int m_cylinderIndex;
	int m_bulletColor;

	Quaternion myQ;

	std::shared_ptr<MyEngine::ColliderSphere> m_col;

	Vec3 m_cylinderPos;
};

