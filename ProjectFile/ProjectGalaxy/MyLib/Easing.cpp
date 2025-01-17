#include "Easing.h"
#include <cmath>
#include <algorithm>
#include<cassert>

Vec3 Lerp(const Vec3& start, const Vec3& end, float t)
{
	return start + (end - start) * t;
}

Vec3 EaseIn(const Vec3& start, const Vec3& end, float t, float effect)
{
	float rate = std::powf(t, effect);
	return Lerp(start, end, rate);
}

Vec3 EaseOut(const Vec3& start, const Vec3& end, float t, float effect)
{
	float rate = 1.0f - std::powf(1.0f - t, effect);
	return Lerp(start, end, rate);
}

Vec3 EaseInOut(const Vec3& start, const Vec3& end, float t, float effect)
{
	float rate;
	if (t < 0.5f)
	{
		rate = 2 * (effect - 1.0f) * powf(t, effect);
	}
	else
	{
		rate = 1.0f - powf(-2 * t + 2, effect) * 0.5f;
	}
	return Lerp(start, end, rate);
}

Vec3 Slerp(const Vec3& start, const Vec3& end, float t)
{
	// 正規化したベクトル
	Vec3 u = start.GetNormalized();
	Vec3 v = end.GetNormalized();

	// 内積を計算して角度を求める
	float dot = Dot(u, v);

	// 内積の制約（-1, 1）を確保
	dot = std::fmax(-1.0f, std::fmin(1.0f, dot));

	// 回転角度を計算
	float theta = std::acos(dot);

	// 補間パラメータ
	if (theta < 1e-6f) {
		return start; // 回転がほとんどない場合はそのまま返す
	}

	// 補間を行う
	float sinTheta = std::sin(theta);
	float startWeight = std::sin((1 - t) * theta) / sinTheta;
	float endWeight = std::sin(t * theta) / sinTheta;

	// 補間したベクトルを返す
	return Vec3(
		startWeight * u.x + endWeight * v.x,
		startWeight * u.y + endWeight * v.y,
		startWeight * u.z + endWeight * v.z
	);
}

Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t)
{
	float dot = Dot(q1, q2);

	if (dot >= 1.0f)
	{
		return q1;
	}

	float angle = std::acosf(dot);

	angle = std::abs(angle);

	float st = std::sinf(angle);

	float s1 = std::sinf(angle * t) / st;
	float s2 = std::sinf(angle * (1.0f - t)) / st;

	Quaternion res;
	res.x = s1 * q1.x + s2 * q2.x;
	res.y = s1 * q1.y + s2 * q2.y;
	res.z = s1 * q1.z + s2 * q2.z;
	res.w = s1 * q1.w + s2 * q2.w;

	assert(!isnan(res.x));
	assert(!isnan(res.y));
	assert(!isnan(res.z));

	return res;
}
