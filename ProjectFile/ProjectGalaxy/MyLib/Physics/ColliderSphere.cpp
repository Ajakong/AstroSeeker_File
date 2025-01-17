#include "ColliderSphere.h"

using namespace MyEngine;

ColliderSphere::ColliderSphere() :
	ColliderBase(ColliderBase::Kind::Sphere),
	radius(0.0f)
{

}

void MyEngine::ColliderSphere::DebugDraw(Vec3 pos)
{
#ifdef DEBUG
	DrawSphere3D((pos + m_posShift).VGet(), radius,static_cast<int>( 2.f * radius), 0xffff00-0x00ff00* m_isHitResult, 0xffffff, true);
#endif
}
