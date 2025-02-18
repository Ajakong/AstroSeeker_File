#include "CannonSphere.h"
#include"ColliderSphere.h"

namespace
{
	

	constexpr float kNeutralRadius = 2.f;

	constexpr int kSurviveFrameMax=120;
}

CannonSphere::CannonSphere(ObjectTag tag, Vec3 pos, Vec3 velocity,int color): SphereBase(Priority::Middle,tag,pos,velocity,color,0),
m_surviveFrame(0),
m_radiusExpansionSpeed(1)
{
	m_color = color;
	m_velocity = velocity;
	m_rigid->SetPos(pos);
	m_bulletUpdate = &CannonSphere::SettingUpdate;

	AddThroughTag(ObjectTag::Cannon);
	
}

CannonSphere::~CannonSphere()
{
}

void CannonSphere::Init()
{
}

void CannonSphere::Update()
{
	(this->*m_bulletUpdate)();

	if (m_surviveFrame > kSurviveFrameMax)
	{
		m_isDestroyFlag = true;
	}
}

void CannonSphere::Draw()
{
	DrawSphere3D(m_rigid->GetPos().VGet(), m_radius, 7, m_color, m_color, true);
}

void CannonSphere::SetRadius(float radius)
{
	m_radius = radius;
}

void CannonSphere::Shot()
{
	m_bulletUpdate = &CannonSphere::StraightUpdate;
	
	{
		AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Head);
		auto col = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
		col->radius = kNeutralRadius;
	}
}

void CannonSphere::SettingUpdate()
{
	m_radius += 0.1f;
}

void CannonSphere::StraightUpdate()
{
	m_surviveFrame++;

	m_rigid->SetVelocity(m_velocity);
}
