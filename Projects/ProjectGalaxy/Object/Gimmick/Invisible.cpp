#include "Invisible.h"
#include"ColliderSphere.h"

namespace
{
	constexpr float kRadius = 50.f;
	constexpr float kChaseSpeed = 2.f;

}
Invisible::Invisible(Vec3 pos) : Collidable(Priority::Low,ObjectTag::InvisibleObject)
{
	m_rigid->SetPos(pos);
	AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);
	auto item = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
	item->radius = kRadius;
}

Invisible::~Invisible()
{
}

void Invisible::Init()
{
}

void Invisible::Update()
{
}

void Invisible::Draw()
{
	SetDrawBlendMode(DX_BLENDMODE_MULA, 100);
	DrawSphere3D(m_rigid->GetPos().VGet(), kRadius, 8, 0xffaa00, 0xffffff, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
