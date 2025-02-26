#include "Killer.h"
#include"Enemy.h"
#include"Player.h"
namespace
{
	/// <summary>
	/// 球の当たり判定半径
	/// </summary1
	constexpr float kSphereRadius = 2.0f;
	/// <summary>
	/// 球の生成間隔
	/// </summary>
	constexpr int kSphereCreateFrame = 50;

	constexpr int kRisingFrameMax = 30;
	constexpr int kSurviveFrameMax = 200;

	const char* name = "Killer";
}
Killer::Killer(MyEngine::Collidable::Priority priority, ObjectTag tag, std::shared_ptr<MyEngine::Collidable>enemy, std::shared_ptr<MyEngine::Collidable>target, Vec3 pos, Vec3 velocity, int moveNum, int color):EnemySphere(priority,tag,enemy,pos,velocity,target->GetRigidbody()->GetPos(),moveNum,color),
  m_risingFrame(0)
{
	m_radius = kSphereRadius;
	m_counterFlag = false;
	m_target = std::dynamic_pointer_cast<Player>(target);
	m_moveUpdate = &Killer::RisingUpdate;
	//m_pointeLightHandle = CreatePointLightHandle(pos.VGet(), 2000.0f, 0.0f, 0.002f, 0.0f);

	m_rigid->SetVelocity(velocity);
	{
		m_rigid->SetPos(pos);
		AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);
		auto item = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
		item->radius = m_radius;
	}
}

Killer::~Killer()
{
	DeleteLightHandle(m_pointeLightHandle);
}

void Killer::Init()
{
}

void Killer::Update()
{
	SetLightPositionHandle(m_pointeLightHandle, m_rigid->GetPos().VGet());
	if (m_counterFlag)
	{
		m_moveUpdate = &Killer::CounterUpdate;
	}
	(this->*m_moveUpdate)();
}

void Killer::Draw()
{
	DrawSphere3D(m_rigid->GetPos().VGet(), kSphereRadius, 10, 0x440044, m_color, true);
}

void Killer::Hit()
{
}

void Killer::OnCollideEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag)
{
	if (colider->GetTag() == ObjectTag::Player&& colider->GetState()==State::Spin)
	{

		m_counterFlag = true;
	}
	else
	{
		//m_isDestroyFlag = true;
	}
	if (m_counterFlag && colider->GetTag() == ObjectTag::Takobo)
	{
		m_isDestroyFlag = true;
	}
}

void Killer::RisingUpdate()
{
	m_risingFrame++;
	if (m_risingFrame > kRisingFrameMax)
	{
		m_moveUpdate = &Killer::SearchUpDate;
	}
	
}

void Killer::SearchUpDate()
{
	m_surviveFrame++;
	if (m_surviveFrame >= kSurviveFrameMax)
	{
		m_isDestroyFlag = true;
	}
	Vec3 toVec = m_target->GetRigidbody()->GetPos() - m_rigid->GetPos();
	m_velocity = toVec.GetNormalized();

	m_rigid->SetVelocity(m_velocity);
	if (toVec.Length() <= 15)
	{
		m_moveUpdate = &Killer::StraightUpdate;
	}
}

void Killer::StraightUpdate()
{
	m_rigid->SetVelocity(m_velocity *3);
}

void Killer::CounterUpdate()
{
	Vec3 toVec = m_enemy->GetRigidbody()->GetPos() - m_rigid->GetPos();
	m_velocity = toVec.GetNormalized();

	m_rigid->SetVelocity(m_velocity * 3);
}
