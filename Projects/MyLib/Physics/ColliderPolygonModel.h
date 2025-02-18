#pragma once
#include<vector>
#include "ColliderBase.h"
#include "Vec3.h"

namespace MyEngine
{
	class ColliderPolygonModel : public MyEngine::ColliderBase
	{
	public:
		struct ModelData
		{
			int modelHandle;
			int polygonNum;
		};
		struct Triangle
		{
			Vec3 vertex[3];
			Vec3 Normal() const { return Cross((vertex[1] - vertex[0]), (vertex[2] - vertex[0])).GetNormalized(); }
		};
		ColliderPolygonModel();
		virtual ~ColliderPolygonModel();

		void Init(int modelHandle,float scale);
		void DebugDraw();
		ModelData GetModelData() { return m_data; }
		std::vector<Triangle> GetTriangles() { return m_triangles; }
	private:
		ModelData m_data;
		std::vector<Triangle> m_triangles;
	};
}

