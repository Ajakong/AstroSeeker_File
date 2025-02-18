#pragma once
#include"ColliderBase.h"
namespace MyEngine
{
	class ColliderLine3D : public ColliderBase
	{
		struct Line
		{
			Vec3 start;
			Vec3 end;
		};
	public:
		/// <summary>
		/// コンストラクタでTriggerがOnになります
		/// </summary>
		/// <param name="startPoint">始点</param>
		/// <param name="endPoint">終点</param>
		ColliderLine3D();

		Line GetLine() { return line; }
		Vec3 GetDir() { return line.end - line.start; }
		
	public:
		Line line;
	};
}

