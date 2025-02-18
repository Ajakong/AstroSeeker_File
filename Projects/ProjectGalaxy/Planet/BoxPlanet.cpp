#include "BoxPlanet.h"
#include"ColliderBox.h"
namespace
{
	const Vec3 kBoxPlanetSize = Vec3(30, 50, 50);
	constexpr float  kGravityPower = 0.098f;
}
BoxPlanet::BoxPlanet(Vec3 pos, int color, float coefficientOfFriction, Vec3 size)
{
	m_color = color;
	gravityPower = 0.3f;
	{
		AddCollider(MyEngine::ColliderBase::Kind::Box, ColideTag::Body);//ここで入れたのは重力の影響範囲
		
		auto item = dynamic_pointer_cast<MyEngine::ColliderBox>(m_colliders.back()->col);
		item->norm = Vec3(0, 1, 0);
		item->size = size;
		item->rotation = Quaternion();
		m_norm = item->norm;
		m_size = item->size;
	}
	
	{
		AddCollider(MyEngine::ColliderBase::Kind::Box, ColideTag::Body);//ここで入れたのは重力の影響範囲
		m_colliders.back()->col->m_isTrigger = true;
		auto item = dynamic_pointer_cast<MyEngine::ColliderBox>(m_colliders.back()->col);
		item->norm = Vec3(0, 1, 0);
		item->size = Vec3(size.x,size.y*5,size.z);
		item->rotation = Quaternion();
		item->m_isTrigger = true;
	}
	AddThroughTag(ObjectTag::Stage);
	AddCollider(MyEngine::ColliderBase::Kind::Box, ColideTag::Body);//マップの当たり判定
	m_rigid->SetPos(pos);

	m_pointLightHandle = CreatePointLightHandle(m_rigid->GetPos().VGet(), 500.0f, 0.0f, 0.002f, 0.0f);

	m_coefficientOfFriction = coefficientOfFriction;
}

BoxPlanet::~BoxPlanet()
{
}

void BoxPlanet::Init()
{
}

void BoxPlanet::Update()
{
}

void BoxPlanet::Draw()
{
	if (m_isSearch)
	{
		DrawCube3D((m_rigid->GetPos() - m_size).VGet(), (m_rigid->GetPos() + m_size).VGet(), 0xffffff, 0xffffff, false);
		DrawCube3D((m_rigid->GetPos() - m_size * 2).VGet(), (m_rigid->GetPos() + m_size * 2).VGet(), 0x00ffff, 0xffffff, false);
	}
	else
	{
		DrawCube3D((m_rigid->GetPos() - m_size).VGet(), (m_rigid->GetPos() + m_size).VGet(), 0x00ffff, 0xffffff, true);
		DrawCube3D((m_rigid->GetPos() - m_size*2).VGet(), (m_rigid->GetPos() + m_size*2).VGet(), 0x00ffff, 0xffffff, false);
	}
	
}

Vec3 BoxPlanet::GravityEffect(std::shared_ptr<Collidable> obj)
{
	obj->SetNextUpVec(m_norm);
	Vec3 ans = m_norm*-1;
	return ans*kGravityPower;
}

Vec3 BoxPlanet::FrictionEffect(std::shared_ptr<Collidable> obj)
{
	return obj->PlanetOnlyGetRigid()->GetVelocity() * (1.f - m_coefficientOfFriction);
}

Vec3 BoxPlanet::GetNormVec(Vec3 pos)
{
	return m_norm;
}

void BoxPlanet::OnTriggerEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag)
{
}

void BoxPlanet::OnTriggerExit(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag)
{
}
