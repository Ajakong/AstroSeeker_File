                                          #include"ColliderSphere.h"
#include "SpherePlanet.h"
#include"ModelManager.h"
#include"Quaternion.h"

namespace
{
	constexpr float kGroundRadius = 50;
	constexpr float  kGravityRange = 150;
	constexpr float  kGravityPower = 0.098f;

	

	const char* name = "planet";
	const char* atom = "atomosphere";

}

SpherePlanet::SpherePlanet(Vec3 pos,int color,float gravity,int modelHandle,float coefficientOfFriction,float scale) :Planet(modelHandle),
m_enemyCount(3),
m_modelHandle(modelHandle),
m_rotationAngle(0)
{
	
	auto ref=MV1GetReferenceMesh(m_modelHandle,-1,true);
	auto size = VScale(VSub(ref.MaxPosition,ref.MinPosition),0.5f);

	//惑星の基本的な大きさで割る
	auto num = size.x / kGroundRadius;
	//取得した値を2で割る
	auto mag = num / scale;
	MV1SetScale(m_modelHandle, VGet(1/mag, 1/mag, 1/mag));


	
	m_scale = scale;
	m_coefficientOfFriction = coefficientOfFriction;
	m_color = color;
	m_rigid->SetPos(pos);
	gravityPower = gravity;
	//m_pointLightHandle = CreatePointLightHandle(m_rigid->GetPos().VGet(), 50.0f, 0.0f, 0.0002f, 0.0f);
	MV1SetPosition(m_modelHandle,m_rigid->GetPos().VGet());

	//当たり判定の追加
	AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);//ここで入れたのは重力の影響範囲
	m_colliders.back()->col->m_isTrigger = true;
	auto item = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
	item->radius = kGroundRadius *m_scale*3;
	AddThroughTag(ObjectTag::Stage);
	AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);//マップの当たり判定
	auto item2 = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
	item2->radius = kGroundRadius*m_scale;
}

SpherePlanet::~SpherePlanet()
{
}

void SpherePlanet::Init()
{
	
}

void SpherePlanet::Update()
{
	m_rotationAngle += 0.001f;
}

void SpherePlanet::Draw()
{
#ifdef DEBUG
	DrawSphere3D(m_rigid->GetPos().VGet(), kGravityRange*m_scale, 10, 0xddddff, 0x0000ff, false);
#endif
	if (m_isSearch)
	{
		DrawSphere3D(m_rigid->GetPos().VGet(), kGroundRadius*m_scale, 50, m_color, 0x0000ff, false);
	}
	else
	{
		MV1DrawModel(m_modelHandle);
		//DrawSphere3D(m_rigid->GetPos().VGet(), kGroundRadius * m_scale, 50, m_color, 0x0000ff, true);
	}
}

Vec3 SpherePlanet::GravityEffect(std::shared_ptr<Collidable> obj)//成分ごとに計算し、補正後のベクトルを返す
{
	Vec3 objVelocity = obj->PlanetOnlyGetRigid()->GetVelocity();
	
	Vec3 ansVelocity;
	Vec3 objPos = obj->PlanetOnlyGetRigid()->GetNextPos();
	Vec3 toObj = objPos-m_rigid->GetPos();
	toObj = toObj.GetNormalized();
	Vec3 GravityDir = toObj * -1;
	///一番重力が強い惑星の影響を受ける
	obj->AddNextUpVec(toObj* gravityPower);
	if (obj->IsAntiGravity())
	{
		return Vec3::Zero();
	}

	if (obj->GetTag() == ObjectTag::EnemyAttack)
	{
		return objVelocity;
	}

	if (obj->GetTag() == ObjectTag::PlayerBullet)
	{
		return GravityDir * kGravityPower + objVelocity;
	}
	/*
	Quaternion q ;
	q.SetQuaternion(obj->GetRigidbody()->GetPos());
	q=AngleAxis(m_rotationAngle, Vec3::Right());
	obj->GetRigidbody()->SetPos(q.ToVec3());*/
	
	//重力のみ
	GravityDir = GravityDir * kGravityPower;

	return GravityDir;
}

Vec3 SpherePlanet::FrictionEffect(std::shared_ptr<Collidable> obj)
{
	return obj->PlanetOnlyGetRigid()->GetVelocity()*(1.f-m_coefficientOfFriction);
}

Vec3 SpherePlanet::GetNormVec(Vec3 pos)
{
	Vec3 norm = pos - m_rigid->GetPos();
	norm.Normalize();
	return norm;
}

void SpherePlanet::OnTriggerEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag)
{
	if (colider->GetTag() == ObjectTag::Takobo)
	{
		m_enemyCount++;
	}
}

void SpherePlanet::OnTriggerExit(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag)
{
	if (colider->GetTag() == ObjectTag::Takobo)
	{
		m_enemyCount--;
	}
	if (m_enemyCount <= 0)
	{
		clearFlag = true;
	}
}

void SpherePlanet::ModelRotation(int dir)
{
	MV1SetRotationXYZ(m_modelHandle, VGet(m_rotationAngle*dir, 0, 0));

}

void SpherePlanet::IntroPlanet()
{
	auto velocity = Vec3(0.f, m_rotationAngle, 0.f);
	m_rigid->SetVelocity(velocity);
}
