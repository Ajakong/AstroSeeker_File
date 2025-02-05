#include "Key.h"
#include"GalaxyCreater.h"
#include"Physics.h"
#include"ModelManager.h"
#include"Mission.h"
#include"SoundManager.h"

namespace
{
	const char* kGetSoundEffectName = "GetCardKey.mp3";
}

Key::Key(Vec3 pos, Vec3 velocity, int connectObjectNumber, bool antigravity) : Item(pos,ObjectTag::Key,antigravity)
{
	m_rigid->SetVelocity(velocity);
	m_getSoundEffectHandle = SoundManager::GetInstance().GetSoundData(kGetSoundEffectName);
	m_connectObjectNumber = connectObjectNumber;
	m_modelHandle=ModelManager::GetInstance().GetModelData("KeyCard");
	MV1SetScale(m_modelHandle, VGet(0.02f, 0.02f, 0.02f));
	MV1SetPosition(m_modelHandle, pos.VGet());
}

Key::~Key()
{
}

void Key::Init()
{
	AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);
	m_col = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
	m_col->radius = 2.5f;
	
}

void Key::Update()
{
	m_col->SetShiftPosNum(m_upVec * -1*2);
	MV1SetPosition(m_modelHandle, m_rigid->GetPos().VGet());
}

void Key::Draw()
{
	MV1DrawModel(m_modelHandle);

	//m_col->DebugDraw(m_rigid->GetPos());
}

void Key::OnCollideEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
	if (colider->GetTag() == ObjectTag::Player)
	{
		PlaySoundMem(m_getSoundEffectHandle, DX_PLAYTYPE_BACK);
		auto obj = GalaxyCreater::GetInstance().GetCollidable(m_connectObjectNumber);
		obj->SetIsActive(true);
		//MyEngine::Physics::GetInstance().Entry(obj);
		m_isDestroyFlag = true;
		Mission::GetInstance().MissionClear();
	}
}

void Key::OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
	if (colider->GetTag() == ObjectTag::Stage)
	{
		m_upVec = m_rigid->GetPos() - colider->GetRigidbody()->GetPos();
		m_upVec.Normalize();
	}
	if (colider->GetTag() == ObjectTag::Player)
	{
		PlaySoundMem(m_getSoundEffectHandle,DX_PLAYTYPE_BACK);
		auto obj = GalaxyCreater::GetInstance().GetCollidable(m_connectObjectNumber);
		obj->SetIsActive(true);
		//MyEngine::Physics::GetInstance().Entry(obj);
		m_isDestroyFlag = true;
		Mission::GetInstance().MissionClear();
	}
}
