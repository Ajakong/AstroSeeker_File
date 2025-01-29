#include "Vec3.h"
#include <cmath>
#include<algorithm>
#include<optional>
#include <cassert>

Vec3::Vec3() :
	x(0.0f),
	y(0.0f),
	z(0.0f)
{
}

Vec3::Vec3(float inX, float inY, float inZ) :
	x(inX),
	y(inY),
	z(inZ)
{
}

Vec3::Vec3(int inX, int inY, int inZ):
	x(static_cast<float>(inX)),
	y(static_cast<float>(inY)),
	z(static_cast<float>(inZ))
{
}

Vec3::Vec3(DxLib::VECTOR vec) :
	x(vec.x),
	y(vec.y),
	z(vec.z)
{
}

Vec3 Vec3::Up()
{
	return Vec3(0.0f, 1.0f, 0.0f);
}

Vec3 Vec3::Down()
{
	return Vec3(0.0f, -1.0f, 0.0f);
}

Vec3 Vec3::Right()
{
	return Vec3(1.0f,0.0f,0.0f);
}

Vec3 Vec3::Left()
{
	return Vec3(-1.0f,0.0f,0.0f);
}

Vec3 Vec3::Front()
{
	return Vec3(0.0f,0.0f,1.0f);
}

Vec3 Vec3::Zero()
{
	return Vec3(0.0f,0.0f,0.0f);
}

Vec3 Vec3::operator+(const Vec3& val) const
{
	return Vec3(x + val.x, y + val.y, z + val.z);
}

void Vec3::operator+=(const Vec3& val)
{
	x += val.x;
	y += val.y;
	z += val.z;
}

Vec3 Vec3::operator-(const Vec3& val) const
{
	return Vec3(x - val.x, y - val.y, z - val.z);
}

void Vec3::operator-=(const Vec3& val)
{
	x -= val.x;
	y -= val.y;
	z -= val.z;
}

Vec3 Vec3::operator*(float scale) const
{
	return Vec3(x * scale, y * scale, z * scale);
}

void Vec3::operator*=(float scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
}

Vec3 Vec3::operator*(Vec3& val) const
{
	return Vec3(x*val.x,y*val.y,z*val.z);
}

Vec3 Vec3::operator*(Vec3 val)
{
	return Vec3(x * val.x, y * val.y, z * val.z);
}

void Vec3::operator*=(Vec3& val)
{
	x = x * val.x;
	y = y * val.y;
	z = z * val.z;
}

Vec3 Vec3::operator/(float scale) const
{
	if (SqLength() == 0.0f)
	{
		assert(false);
		return Vec3();
	}

	return Vec3(x / scale, y / scale, z / scale);
}

void Vec3::operator/=(float scale)
{
	if (SqLength() > 0.0f)
	{
		x /= scale;
		y /= scale;
		z /= scale;
	}
	else
	{
		assert(false);
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
}

bool Vec3::operator==(const Vec3& val) const
{
	bool isX = x == val.x;
	bool isY = y == val.y;
	bool isZ = z == val.z;
	return isX && isY && isZ;
}

bool Vec3::operator!=(const Vec3& val) const
{
	bool isX = x != val.x;
	bool isY = y != val.y;
	bool isZ = z != val.z;
	return isX || isY || isZ;
}

float Vec3::Length() const
{
	return std::hypot(x, y, z);
}

float Vec3::SqLength() const
{
	return x * x + y * y + z * z;
}

Vec3 Vec3::GetNormalized() const
{
	Vec3 ref(0.0f, 0.0f, 0.0f);
	float len = Length();

	if (len > 0.0f)
	{
		ref = *this / len;
	}

	return ref;
}

void Vec3::Normalize()
{
	float len = Length();

	if (len > 0.0f)
	{
		*this /= len;
	}
}

DxLib::VECTOR Vec3::VGet() const
{
	return DxLib::VGet(x, y, z);
}

void Vec3::SetVECTOR(DxLib::VECTOR vec)
{
	x = vec.x, y = vec.y, z = vec.z;
}

float Dot(const Vec3& item1, const Vec3& item2)
{
	return item1.x * item2.x + item1.y * item2.y + item1.z * item2.z;
}

Vec3 Cross(const Vec3& item1, const Vec3& item2)
{
	Vec3 result;

	result.x = item1.y * item2.z - item1.z * item2.y;
	result.y = item1.z * item2.x - item1.x * item2.z;
	result.z = item1.x * item2.y - item1.y * item2.x;

	return result;
}

Vec3 Projection(const Vec3& projection, const Vec3& base)
{
	auto dirN = base.GetNormalized();
	auto d = Dot(dirN, projection);
	return dirN * d;
}

Vec3 GetPerpendicular(const Vec3& vec)
{
	// 自身が Z軸に平行である場合
	if (vec.x == 0.0f && vec.y == 0.0f)
		return Vec3(1.0f, 0.0f, 0.0f);  // X軸に平行なベクトルを返す

	// 自身が X軸に平行である場合
	if (vec.y == 0.0f && vec.z == 0.0f)
		return Vec3(0.0f, 1.0f, 0.0f);  // Y軸に平行なベクトルを返す

	// 任意のベクトルに対して垂直なベクトルを計算（外積を利用）
	return Cross(vec,Vec3(1.0f, 0.0f, 0.0f));  // X軸と外積をとる
}

Vec3 MultiPly(Vec3& v, MATRIX& mat)
{
	return Vec3(
			mat.m[0][0] * v.x + mat.m[0][1] * v.y + mat.m[0][2] * v.z,
			mat.m[1][0] * v.x + mat.m[1][1] * v.y + mat.m[1][2] * v.z,
			mat.m[2][0] * v.x + mat.m[2][1] * v.y + mat.m[2][2] * v.z);
}

Vec3 RotateVector(Vec3& vec, MATRIX& rotationMatrix)
{
	return MultiPly(vec,rotationMatrix);
}

Vec3 GetClosestPtOnSegment(Vec3 pt, Vec3 start, Vec3 end)
{
	// 最近接点がstart側線分外領域の場合
	Vec3 startToPt = pt- start;
	Vec3 startToEnd = end- start;
	Vec3 startToEndN = startToEnd.GetNormalized();
	if (Dot(startToPt, startToEndN) < 0)
	{
		return start;
	}
	Vec3 endToPt = pt- end;
	Vec3 endToStart = start- end;
	Vec3 endToStartN = endToStart.GetNormalized();
	// 最近接点がend側線分外領域の場合
	if (Dot(endToPt, endToStartN) < 0)
	{
		return end;
	}
	// 中間領域の場合
	else
	{
		float t = Dot(startToEndN, startToPt);
		return start+startToEndN* t;
	}
}

Vec3 GetClosestPointOnTriangle(Vec3& spherePos, Vec3& A, Vec3& B, Vec3& C)
{
	// 三角形の法線を計算
	Vec3 edge1 = B - A;
	Vec3 edge2 = C - A;
	Vec3 normal = Cross(edge1, edge2).GetNormalized();

	// 球の中心から三角形面への垂直距離を求める
	float dist = Dot(spherePos - A, normal);

	// 球の中心から三角形面への垂直に投影した点を計算
	Vec3 projection = spherePos - normal * dist;

	// 数値誤差を考慮するための許容値
	

	// 投影された点が三角形内部かどうかを確認
	if (IsPointInsideTriangle(projection, A, B, C)) {
		// 投影点が三角形内部にある場合、その点が最近接点
		return projection;
	}
	else {
		// 投影点が三角形外部の場合、各辺上の最近接点を計算
		Vec3 edgeAB = GetClosestPointOnLineSegment(spherePos, A, B);
		Vec3 edgeBC = GetClosestPointOnLineSegment(spherePos, B, C);
		Vec3 edgeCA = GetClosestPointOnLineSegment(spherePos, C, A);

		// 辺上の最近接点を比較して最小距離を求める
		float distAB = (spherePos - edgeAB).SqLength();
		float distBC = (spherePos - edgeBC).SqLength();
		float distCA = (spherePos - edgeCA).SqLength();

		// 距離が最小の点を返す
		if (distAB <= distBC && distAB <= distCA)
			return edgeAB;
		else if (distBC <= distAB && distBC <= distCA)
			return edgeBC;
		else
			return edgeCA;
	}
}

Vec3 GetClosestPointOnLineSegment(Vec3& P, Vec3& A, Vec3& B)
{
	Vec3 segment = B - A;
	float t = Dot(P - A, segment) / segment.SqLength();
	t = std::clamp(t, 0.0f, 1.0f); // tを[0, 1]の範囲にクランプ
	return A + segment * t;
}

bool IsPointInsideTriangle(Vec3& point, Vec3& v0, Vec3& v1, Vec3& v2)
{
	Vec3 edge0 = v1 - v0;
	Vec3 edge1 = v2 - v1;
	Vec3 edge2 = v0 - v2;

	Vec3 c0 = point - v0;
	Vec3 c1 = point - v1;
	Vec3 c2 = point - v2;

	// クロス積を使って、三角形の外側かどうかをチェック
	Vec3 cross0 = Cross(edge0, c0);
	Vec3 cross1 = Cross(edge1, c1);
	Vec3 cross2 = Cross(edge2, c2);

	// すべてのクロス積が同じ方向（同じ符号）であれば点は内部にある
	if (Dot(cross0, cross1) >= -1e-6f && Dot(cross1, cross2) >= 0.0f && Dot(cross2, cross0) >= 0.0f) {
		return true;
	}

	return false;  // 三角形外部にある
}

bool RayIntersectsTriangle(Vec3& rayOrigin, Vec3& rayDir, Vec3& v0, Vec3& v1, Vec3& v2)
{
	float t=0.f;
	const float EPSILON = 1e-6f; // 計算誤差の許容範囲

	// 三角形の辺ベクトルを計算
	Vec3 edge1 = v1 - v0;
	Vec3 edge2 = v2 - v0;

	// レイ方向と edge2 の外積を計算
	Vec3 h = Cross(rayDir,edge2);
	float a = Dot(edge1,h);

	// レイが三角形の平面と平行である場合は交差しない
	if (std::fabs(a) < EPSILON)
		return false;

	// 逆行列の係数
	float f = 1.0f / a;

	// レイの始点から三角形の頂点へのベクトル
	Vec3 s = rayOrigin - v0;

	// u パラメータを計算
	float u = f * Dot(s,h);
	if (u < 0.0f || u > 1.0f)
		return false;

	// q ベクトルを計算
	Vec3 q = Cross(s,edge1);

	// v パラメータを計算
	float v = f * Dot(rayDir,q);
	if (v < 0.0f || u + v > 1.0f)
		return false;

	// レイが三角形の平面と交差する距離 t を計算
	t = f * Dot(edge2,q);
	if (t > EPSILON)
	{
		// t が正なら交差している
		return true;
	}

	// t が負またはゼロなら交差していない
	return false;
}
