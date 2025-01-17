#include "PlayerStickSphere.h"
#include"ColliderSphere.h"
#include"Player.h"
#include"Item.h"

namespace
{
	/// <summary>
	/// 球の当たり判定半径
	/// </summary>
	constexpr float kSphereRadius = 1.0f;
	/// <summary>
	/// 球の生成間隔
	/// </summary>
	constexpr int kSphereCreateFrame = 50;

	constexpr int kLifeTimeMax = 100;
	const char* name = "Sphere";

}

PlayerStickSphere::PlayerStickSphere(MyEngine::Collidable::Priority priority, ObjectTag tag, std::shared_ptr<MyEngine::Collidable> player, Vec3 pos, Vec3 velocity, Vec3 sideVec, int moveNum, int color) : PlayerSphere(priority, tag, player, pos, velocity, sideVec, moveNum, color),
m_player(std::dynamic_pointer_cast<Player>(player)),
m_sideVec(sideVec),
m_lifeTime(0),
m_stickFlag(false)
{
	/*m_rigid->SetVelocity(VGet(m_velocity.x * 2, m_velocity.y * 2, m_velocity.z * 2));
	m_rigid->SetPos(pos);
	AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);
	auto item = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
	item->radius = m_radius;
	m_color = color;*/
	//moveNumによって挙動が変わるのかもしれない(実装するかわからん)
	//if (moveNum == 0)
	{
		m_moveUpdate = &PlayerStickSphere::StraightUpdate;
	}
}


PlayerStickSphere::~PlayerStickSphere()
{
}

void PlayerStickSphere::Init()
{

}

void PlayerStickSphere::Update()
{
	

	m_startPos = m_player->GetLeftHandPos();
	(this->*m_moveUpdate)();
}

void PlayerStickSphere::Draw()
{
	DrawSphere3D(m_rigid->GetPos().VGet(), kSphereRadius, 10, 0xffff00, m_color, false);
	DrawLine3D(m_startPos.VGet(), m_rigid->GetPos().VGet(), 0xffffff);
}

void PlayerStickSphere::Effect()
{
	if (m_player->GetOperationFlag())
	{
		m_player->SetIsOperation(false);
		m_isDestroyFlag = true;
	}
	if (m_stickFlag)
	{
		if (m_isMoving)
		{
			m_moveUpdate = &PlayerStickSphere::ComeBackWithObjectUpdate;
		}
		else if ((m_player->GetPos() - m_rigid->GetPos()).Length() < 20)
		{
			m_moveUpdate = &PlayerStickSphere::ComeBackUpdate;
		}
		else
		{
			m_player->SetIsOperation(true);
			m_player->SetVelocity((m_rigid->GetPos() - m_player->GetPos()).GetNormalized() * 3);
		}
		
	}
	else m_moveUpdate = &PlayerStickSphere::ComeBackUpdate;
}

void PlayerStickSphere::OnCollideEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
	m_stickFlag = true;
	auto priority = colider->GetPriority();
	m_isMoving = priority == Priority::Lowest;
	m_moveUpdate = &PlayerStickSphere::StickUpdate;
	m_contactedCollidable = colider;
	m_collidableContactPosition = colider->GetRigidbody()->GetPos() - m_rigid->GetPos();
}

void PlayerStickSphere::OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
	auto ptr = std::dynamic_pointer_cast<Item>(colider);
	if (ptr != nullptr)
	{
		m_stickFlag = true;
		auto priority = colider->GetPriority();
		m_isMoving = priority == Priority::Lowest;
		m_moveUpdate = &PlayerStickSphere::StickUpdate;
		m_contactedCollidable = colider;
		m_collidableContactPosition = colider->GetRigidbody()->GetPos() - m_rigid->GetPos();
	}
}

void PlayerStickSphere::StraightUpdate()
{
	m_rigid->SetVelocity(m_velocity*4);
}

void PlayerStickSphere::StickUpdate()
{
	SetAntiGravity(true);
	m_rigid->SetVelocity(Vec3::Zero());
	if ((m_rigid->GetPos() - m_startPos).Length() <= 5.0f)
	{
		m_player->SetIsOperation(false);
		m_moveUpdate = &PlayerStickSphere::ComeBackUpdate;
	}
}

void PlayerStickSphere::ComeBackUpdate()
{
	float speed = (m_startPos - m_rigid->GetPos()).Length()/10;
	m_rigid->SetVelocity((m_startPos - m_rigid->GetPos()).GetNormalized() * speed);
	if ((m_rigid->GetPos() - m_startPos).Length() <= 2.f)
	{
		m_isDestroyFlag = true;
	}
}

void PlayerStickSphere::ComeBackWithObjectUpdate()
{

	m_rigid->SetVelocity((m_startPos - m_rigid->GetPos()).GetNormalized() * 3);
	m_contactedCollidable->GetRigidbody()->SetPos(m_rigid->GetPos()+m_collidableContactPosition);
	if ((m_rigid->GetPos() - m_startPos).Length() <= 1.2f)
	{
		m_isDestroyFlag = true;
	}
}
