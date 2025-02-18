#include "EnemySphere.h"
#include "Enemy.h"
#include "ColliderSphere.h"
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
	const char* name = "Sphere";
}
EnemySphere::EnemySphere(MyEngine::Collidable::Priority priority, ObjectTag tag, std::shared_ptr<MyEngine::Collidable>enemy, Vec3 pos, Vec3 velocity, Vec3 targetPos, int moveNum, int color,float impactTime) : SphereBase(priority,tag,pos,velocity,color,kSphereRadius),
m_enemy(std::dynamic_pointer_cast<Enemy>(enemy))
{
	m_impactTime = impactTime;
	m_velocity = velocity;
	m_rigid->SetPos(pos);
	AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);
	auto item = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
	item->radius = m_radius;
	m_color = color;
	m_targetPos = targetPos;

	SetAntiGravity(true);
	//moveNumによって挙動が変わるのかもしれない(実装するかわからん)
	//if (moveNum == 0)
	{
		m_moveUpdate = &EnemySphere::ChaseUpdate;
	}
}

EnemySphere::~EnemySphere()
{
}

void EnemySphere::Init()
{
}

void EnemySphere::Update()
{
	(this->*m_moveUpdate)();
}

void EnemySphere::Draw()
{
	DrawSphere3D(m_rigid->GetPos().VGet(), kSphereRadius, 10, 0xffff00, m_color, false);
}

void EnemySphere::Hit()
{
	
}

void EnemySphere::OnCollideEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag)
{
	m_isDestroyFlag = true;
}

void EnemySphere::StraightUpdate()
{
	m_rigid->SetVelocity(m_velocity);
}

void EnemySphere::ChaseUpdate()
{
	Vec3 correctionVec = (m_targetPos - m_rigid->GetPos());

	float mag = 7.f/m_impactTime;
	printf("EnemySphere:Verocity(%f,%f,%f)\n", m_velocity.x, m_velocity.y, m_velocity.z);
	if (correctionVec.Length() <= 0.5f)
	{
		m_moveUpdate = &EnemySphere::StraightUpdate;
	}

	m_velocity += m_enemy->GetShotUpVec() *-1*(mag);
	
	m_rigid->SetVelocity(m_velocity);
}