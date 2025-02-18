#pragma once
#include <DxLib.h>

/// <summary>
/// ベクター
/// </summary>
class Vec3
{
public:
	float x;
	float y;
	float z;



	Vec3();
	Vec3(float inX, float inY, float inZ);
	Vec3(int inX, int inY, int inZ);
	Vec3(DxLib::VECTOR vec);
	static Vec3 Up();
	static Vec3 Down();
	static Vec3 Right();
	static Vec3 Left();
	static Vec3 Front();
	static Vec3 Zero();

	/* 演算 */
	Vec3 operator+(const Vec3& val) const;
	void operator+=(const Vec3& val);
	Vec3 operator-(const Vec3& val) const;
	void operator-=(const Vec3& val);
	Vec3 operator*(float scale) const;
	void operator*=(float scale);
	Vec3 operator*(Vec3& val)const;
	Vec3 operator*(Vec3 val);
	void operator*=(Vec3& val);
	Vec3 operator/(float scale) const;
	void operator/=(float scale);
	bool operator==(const Vec3& val) const;
	bool operator!=(const Vec3& val) const;


	// 長さ取得
	float Length() const;
	// 長さの2乗を取得
	float SqLength() const;
	// 正規化したベクトルを取得
	Vec3 GetNormalized() const;
	// 自身の正規化
	void Normalize();


	// DxLib用のVECTORを取得
	DxLib::VECTOR VGet() const;

	void SetVECTOR(DxLib::VECTOR vec);
};

// 内積
float Dot(const Vec3& item1, const Vec3& item2);
// 外積
Vec3 Cross(const Vec3& item1, const Vec3& item2);

// 射影
Vec3 Projection(const Vec3& projection, const Vec3& base);

Vec3 GetPerpendicular(const Vec3& vec);

Vec3 MultiPly(Vec3& vec, MATRIX& mat);

Vec3 RotateVector(Vec3& vec, MATRIX& rotationMatrix);

Vec3 GetClosestPtOnSegment(Vec3 pt, Vec3 start, Vec3 end);

Vec3 GetClosestPointOnTriangle(Vec3& spherePos, Vec3& A, Vec3& B, Vec3& C);

Vec3 GetClosestPointOnLineSegment(Vec3& P, Vec3& A, Vec3& B);

/// <summary>
/// 接触点が三角形の内側にあるかの判定
/// </summary>
/// <param name="p">接触点</param>
/// <param name="v0">三角形の頂点0</param>
/// <param name="v1">三角形の頂点1</param>
/// <param name="v2">三角形の頂点2</param>
bool IsPointInsideTriangle(Vec3& point,Vec3& v0, Vec3& v1, Vec3& v2);

bool RayIntersectsTriangle(Vec3& rayOrigin, Vec3& rayDir,Vec3& v0, Vec3& v1, Vec3& v2);