#pragma once
#include "Collidable.h"
class SphereBase : public MyEngine::Collidable
{
public:

	bool GetCounterFlag() { return m_counterFlag; }

	void SetCounterFlag() { m_counterFlag = true; }
	Vec3 GetVelocity() { return m_velocity; }
	void SetVelocity(Vec3 vel) { m_velocity = vel; }

	void OnDestroy() { m_isDestroyFlag = true; }

	virtual void OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);
protected:
	SphereBase(MyEngine::Collidable::Priority priority, ObjectTag tag,Vec3 pos, Vec3 velocity,int color,float radius);
	virtual ~SphereBase();

	virtual void Init()=0;
	virtual void Update()=0;
	virtual void Draw()=0;

	

protected:
	int m_color = 0;

	bool m_counterFlag;

	float m_radius = 0;

	Vec3 m_velocity;
};

