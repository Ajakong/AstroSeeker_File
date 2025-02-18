#pragma once
#include "Planet.h"
class SpherePlanet : public Planet
{
public:
	SpherePlanet(Vec3 pos,int color,float m_gravityPower, int modelHandle,float coefficientOfFriction=1,float scale=1);
	virtual ~SpherePlanet();

	virtual void Init();
	virtual void Update();
	virtual void Draw();

	virtual Vec3 GravityEffect(std::shared_ptr<Collidable> obj);
	virtual Vec3 FrictionEffect(std::shared_ptr<Collidable> obj);
	virtual Vec3 GetNormVec(Vec3 pos);
	virtual void OnTriggerEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag);
	virtual void OnTriggerExit(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag);
	
	void ModelRotation(int dir=1);
	void IntroPlanet();
protected:

	int m_enemyCount;
	int m_color;
	int m_modelHandle;

	float m_rotationAngle;

};

