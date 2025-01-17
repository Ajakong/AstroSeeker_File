#include "DumbbellPlanet.h"
#include"ColliderSphere.h"
#include"ColliderBox.h"

namespace
{
	constexpr float kGroundRadius = 25;
	constexpr float  kGravityRange = 150;
	constexpr float  kGravityPower = 0.098f;


	const char* name = "planet";
	const char* atom = "atomosphere";

}

DumbbellPlanet::DumbbellPlanet(Vec3 pos, int color,  int modelHandle): Planet(),
m_modelHandle(modelHandle)
{
	m_color = color;
	m_rigid->SetPos(pos);

	m_pointLightHandle = CreatePointLightHandle(m_rigid->GetPos().VGet(), 50.0f, 0.0f, 0.0002f, 0.0f);
	MV1SetScale(m_modelHandle, VGet(0.5f, 0.5f, 0.5f));
	MV1SetPosition(m_modelHandle, m_rigid->GetPos().VGet());

	//当たり判定の追加
	AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);//ここで入れたのは重力の影響範囲
	m_colliders.back()->col->m_isTrigger = true;
	auto item = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
	item->radius = kGravityRange;
	AddThroughTag(ObjectTag::Stage);
	AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::one);//マップの当たり判定
	auto item2 = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
	item2->radius = kGroundRadius;
	AddCollider(MyEngine::ColliderBase::Kind::Box, ColideTag::two);
	auto item3 = dynamic_pointer_cast<MyEngine::ColliderBox>(m_colliders.back()->col);
	item3->size = Vec3();
	AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::three);//マップの当たり判定
	auto item4 = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
	item4->radius = kGroundRadius;



}

DumbbellPlanet::~DumbbellPlanet()
{
}

void DumbbellPlanet::Init()
{
}

void DumbbellPlanet::Update()
{
}

void DumbbellPlanet::Draw()
{
}

Vec3 DumbbellPlanet::GravityEffect(std::shared_ptr<Collidable> obj)
{
	return m_gravityDir;
}

Vec3 DumbbellPlanet::FrictionEffect(std::shared_ptr<Collidable> obj)
{
	return obj->PlanetOnlyGetRigid()->GetVelocity() * (1.f - m_coefficientOfFriction);
}

Vec3 DumbbellPlanet::GetNormVec(Vec3 pos)
{
	return m_gravityDir;
}

void DumbbellPlanet::OnCollideEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{

}

void DumbbellPlanet::OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
}

void DumbbellPlanet::OnTriggerExit(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
}
