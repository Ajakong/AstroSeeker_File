#pragma once
#include "ColliderBase.h"

namespace MyEngine
{
	class ColliderSphere : public ColliderBase
	{
	public:
		ColliderSphere();

		void DebugDraw(Vec3 pos);
		float GetRadius()const { return radius; }

	public:
		float radius;
	};
}

