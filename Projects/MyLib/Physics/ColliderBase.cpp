#include "ColliderBase.h"

MyEngine::ColliderBase::ColliderBase(Kind kind) :
	m_isTrigger(false),
	m_kind(kind),
	m_posShift(Vec3::Zero()),
	m_isNowOnHit(false),
	m_isPreOnHit(false),
	m_isHitResult(false)
{
	m_isPreHit.clear();
	m_isHit.clear();
}

void MyEngine::ColliderBase::UpdateHit(const std::shared_ptr<ColliderBase> collider, bool isHit)
{
	m_isPreHit[collider] = m_isHit[collider];
	m_isHit[collider] = isHit;
}
