#include "BigKuribo.h"
#include"Physics.h"
#include"GalaxyCreater.h"
#include"ModelManager.h"
#include"Key.h"

namespace
{
	const char* kModelName = "RedKuribo";
}

BigKuribo::BigKuribo(Vec3 pos,int connectNum) : Kuribo(pos)
{
	/*MV1SetScale(m_modelHandle, Vec3(0.03f, 0.03f, 0.03f).VGet());
	MV1SetPosition(m_modelHandle, pos.VGet());*/
	m_connectObjectNumber = connectNum;
	m_comebackPoint = pos;
	m_rigid->SetPos(pos);
	
#ifdef DEBUG

#endif
	
	m_modelHandle = ModelManager::GetInstance().GetModelData(kModelName);
	MV1SetScale(m_modelHandle, VGet(0.03f, 0.03f, 0.03f));
	ChangeAnim(AnimNum::AnimationNumIdle);
	m_frontVec = Vec3(1, 0, 0);
	m_sideVec = Vec3(0, 0, 1);

}

BigKuribo::~BigKuribo()
{
	
}

void BigKuribo::DeathUpdate()
{

	SetAntiGravity(true);
	m_bodyCol->radius = -1;
	m_state = "Death";
	m_userData->dissolveY -= 0.01f;
	float animFrame = MV1GetAttachAnimTime(m_modelHandle, m_currentAnimNo);
	if (m_userData->dissolveY < 0)
	{
		auto obj = std::make_shared<Key>(m_rigid->GetPos(), m_connectObjectNumber);
		Physics::GetInstance().Entry(obj);
		m_isDestroyFlag = true;
	}
	if (animFrame > 60)
	{
		
		auto obj = std::make_shared<Key>(m_rigid->GetPos(), m_connectObjectNumber);
		Physics::GetInstance().Entry(obj);
		m_isDestroyFlag = true;
	}
}
