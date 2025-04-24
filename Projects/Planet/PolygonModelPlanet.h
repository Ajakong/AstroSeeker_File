#pragma once
#include "Planet.h"
#include"ColliderPolygonModel.h"

using namespace MyEngine;
class PolygonModelPlanet : public Planet
{
public:
	PolygonModelPlanet(int modelHandle,Vec3 pos, float m_gravityPower, float coefficientOfFriction = 1, float scale = 1);
	virtual ~PolygonModelPlanet();

	virtual void Init();
	virtual void Update();
	virtual void Draw();



	virtual Vec3 GravityEffect(std::shared_ptr<Collidable> obj);
	virtual Vec3 FrictionEffect(std::shared_ptr<Collidable> obj);
	virtual Vec3 GetNormVec(Vec3 pos);
	Vec3 NearestNormVec(Vec3 pos, Vec3 upVec);
	virtual void OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);
	virtual void OnTriggerExit(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);

protected:
	Vec3 ClosestPointOnTriangle(const Vec3& P, const Vec3& V0, const Vec3& V1, const Vec3& V2);

protected:
	std::shared_ptr<ColliderPolygonModel> m_polygons;
	int m_enemyCount;
	int m_color;
	int m_modelHandle;

	float m_scale;
};

