#pragma once
#include"Vec3.h"
namespace UtilFunc
{
	
	/// <summary>
	/// 法線を表に三角形を描画する関数
	/// </summary>
	/// <param name="center">資格の中心</param>
	/// <param name="normal">法線方向</param>
	/// <param name="size">大きさ</param>
	/// <param name="color">色</param>
	void DrawTriangle(const Vec3& center, const Vec3& normal, float size, unsigned int color,int GrHandle=-1);
	
	/// <summary>
	/// 法線を表に円形を描画する関数
	/// </summary>
	void DrawCircle(const Vec3& center, const Vec3& normal, float size, unsigned int color, int GrHandle, int vertexCount = 30);
}
 

