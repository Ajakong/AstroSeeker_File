#include "Coin.h"
#include"Physics.h"
#include"SoundManager.h"
namespace
{
	const char* kGetSoundEffectName = "GetCoin.mp3";
}

Coin::Coin(Vec3 pos, bool antiGravity) : Item(pos,ObjectTag::Coin,antiGravity),
m_getSoundEffectHandle()
{
	m_getSoundEffectHandle=SoundManager::GetInstance().GetSoundData(kGetSoundEffectName);
}

Coin::~Coin()
{
}

void Coin::Init()
{
	SetObjectTag(ObjectTag::Coin);
	AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);
	m_col = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
	m_col->radius = 2.5f;
	m_col->m_isTrigger = true;
}

void Coin::Draw()
{
	Vec3 zero = { 0,0,0 };
	m_rigid->SetVelocity(Vec3(0, 0, 0));
	Vec3 offSetVec = GetCameraRightVector();
	offSetVec -= GetCameraUpVector();
	offSetVec *= 0.9f;
	Quaternion myQ;
	angle += 0.05f;

	Vec3 front = GetCameraFrontVector();
	for (int i = 0; i < 3; i++)
	{
		myQ.SetQuaternion(offSetVec);
		myQ.SetMove(DX_PI_F * 2 / 3 * i + angle, front);
		Vec3 offSet = myQ.Move(offSetVec, zero);
		DrawSphere3D((m_rigid->GetPos() + offSet).VGet(), 1.0, 8, 0x0000aa, 0x000000, false);
	}

	DrawSphere3D(m_rigid->GetPos().VGet(), m_col->radius, 8, 0xffff00, 0xffffff, false);
}

void Coin::OnTriggerEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag)
{
	if (colider->GetTag() == ObjectTag::Player)
	{
		PlaySoundMem(m_getSoundEffectHandle, DX_PLAYTYPE_BACK);
		MyEngine::Physics::GetInstance().Exit(shared_from_this());
	}
}
