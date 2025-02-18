#include "ColliderLine3D.h"

MyEngine::ColliderLine3D::ColliderLine3D():ColliderBase(Kind::Line)
{
	
	//線は位置補正をしないしされない
	m_isTrigger = true;
}
