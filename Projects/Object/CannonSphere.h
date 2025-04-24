#pragma once
#include "SphereBase.h"
class CannonSphere : public SphereBase
{
public:
	CannonSphere(ObjectTag tag,Vec3 pos,Vec3 velocity,int color);
	virtual ~CannonSphere();

	void Init();
	void Update();
	void Draw();

	void SetRadius(float radius);
	void Shot();

	float GetRadius() { return m_radius; }

private:
	void SettingUpdate();
	void StraightUpdate();

private:
	//メンバ関数ポインタ
	using bulletState_t = void(CannonSphere::*)();
	bulletState_t m_bulletUpdate;

	float m_radiusExpansionSpeed;

	int m_surviveFrame;


};

