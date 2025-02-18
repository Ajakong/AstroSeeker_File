#include "PolygonModelPlanet.h"
#include"ColliderSphere.h"
#include <cmath>
#include <algorithm>
namespace
{
	constexpr float kGroundRadius = 50;
	constexpr float  kGravityRange = 300;
	constexpr float  kGravityPower = 0.098f;



	const char* name = "planet";
	const char* atom = "atomosphere";

}

// clamp 関数の実装
template <typename T>
T clamp(T value, T min, T max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}


PolygonModelPlanet::PolygonModelPlanet(int modelHandle, Vec3 pos, float gravity, float coefficientOfFriction, float scale) : Planet(),
m_scale(scale),
m_enemyCount(3),
m_modelHandle(modelHandle)
{
	m_coefficientOfFriction = coefficientOfFriction;
	m_rigid->SetPos(pos);
	gravityPower = gravity;
	m_pointLightHandle = CreatePointLightHandle(m_rigid->GetPos().VGet(), 50.0f, 0.0f, 0.0002f, 0.0f);
	MV1SetScale(m_modelHandle, VGet(scale, scale, scale));

	MV1SetPosition(m_modelHandle, m_rigid->GetPos().VGet());

	//当たり判定の追加
	AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);//ここで入れたのは重力の影響範囲
	m_colliders.back()->col->m_isTrigger = true;
	auto item = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
	item->m_isTrigger = true;
	item->radius = kGravityRange;
	AddThroughTag(ObjectTag::Stage);
	AddCollider(MyEngine::ColliderBase::Kind::Polygons, ColideTag::Body);//マップの当たり判定
	m_polygons = dynamic_pointer_cast<MyEngine::ColliderPolygonModel>(m_colliders.back()->col);
	m_polygons->Init(modelHandle, scale);
}

PolygonModelPlanet::~PolygonModelPlanet()
{
}

void PolygonModelPlanet::Init()
{
}

void PolygonModelPlanet::Update()
{
}

void PolygonModelPlanet::Draw()
{
	MV1DrawModel(m_modelHandle);
	m_polygons->DebugDraw();
	printf("PlanetPos:%f,%f,%f\n", MV1GetScale(m_modelHandle).x, MV1GetPosition(m_modelHandle).y, MV1GetPosition(m_modelHandle).z);
}

Vec3 PolygonModelPlanet::GravityEffect(std::shared_ptr<Collidable> obj)
{
	Vec3 objVelocity = obj->PlanetOnlyGetRigid()->GetVelocity();

	Vec3 ansVelocity;
	Vec3 objPos = obj->PlanetOnlyGetRigid()->GetNextPos();
	Vec3 toObj = objPos - m_rigid->GetPos();
	toObj = toObj.GetNormalized();
	Vec3 GravityDir = NearestNormVec(objPos,obj->GetUpVec()) * -1;
	obj->SetNextUpVec(GravityDir*-1);

	if (obj->IsAntiGravity())
	{
		return Vec3::Zero();
	}

	if (obj->GetTag() == ObjectTag::EnemyAttack)
	{
		return objVelocity;
	}

	if (obj->GetTag() == ObjectTag::PlayerBullet)
	{
		return GravityDir * kGravityPower + objVelocity;
	}


	//重力のみ
	GravityDir = GravityDir * kGravityPower;


	return GravityDir;
}

Vec3 PolygonModelPlanet::FrictionEffect(std::shared_ptr<Collidable> obj)
{
	return obj->PlanetOnlyGetRigid()->GetVelocity() * (1.f - m_coefficientOfFriction);
}

Vec3 PolygonModelPlanet::GetNormVec(Vec3 pos)
{
	return Vec3::Zero();
}

Vec3 PolygonModelPlanet::NearestNormVec(Vec3 pos, Vec3 upVec)
{
	Vec3 closestPoint = { 0.0f, 0.0f, 0.0f };
	Vec3 closestNormal = { 0.0f, 0.0f, 0.0f };
	float minDistanceSq = FLT_MAX;

	// モデル内の三角形リストを処理
	for (const auto& triangle : m_polygons->GetTriangles()) {
		// 三角形の法線を計算
		Vec3 normal = triangle.Normal();
		normal.Normalize();

		// upVecと法線の角度を計算
		float dotProduct = Dot(upVec, normal); // 内積
		float angle = acosf(std::clamp(dotProduct, -1.0f, 1.0f)) * (180.0f / 3.141592653589793f); // 角度に変換

		// 角度が45度以下の場合のみ処理
		if (angle <= /*45.0f*/90.f) {
			// 三角形と点の最近接点を求める
			Vec3 currentClosest = ClosestPointOnTriangle(pos, triangle.vertex[0], triangle.vertex[1], triangle.vertex[2]);

			// 距離を比較して最小値を更新
			float distSq = (pos - currentClosest).SqLength();
			if (distSq < minDistanceSq)
			{
				minDistanceSq = distSq;
				closestPoint = currentClosest;
				closestNormal = normal;
			}
		}
	}

	closestNormal.Normalize();
	return closestNormal;
}

void PolygonModelPlanet::OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
	colider->SetNextUpVec(NearestNormVec(colider->GetRigidbody()->GetPos(), colider->GetUpVec()));
}

void PolygonModelPlanet::OnTriggerExit(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
}

Vec3 PolygonModelPlanet::ClosestPointOnTriangle(const Vec3& P, const Vec3& V0, const Vec3& V1, const Vec3& V2)
{
	Vec3 v0v1 = V1 - V0;
	Vec3 v0v2 = V2 - V0;
	Vec3 v0p = P - V0;

	// バリセントリック座標で内部判定
	float dot00 = Dot(v0v2,v0v2);
	float dot01 = Dot(v0v2,v0v1);
	float dot02 = Dot(v0v2,v0p);
	float dot11 = Dot(v0v1,v0v1);
	float dot12 = Dot(v0v1,v0p);

	float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	if (u >= 0 && v >= 0 && u + v <= 1) {
		// 点が三角形の内部にある場合
		return V0 + v0v1 * u + v0v2 * v;
	}

	// 三角形外の場合、エッジや頂点を考慮
	Vec3 closest = V0;
	float minDistanceSq = (P - V0).SqLength();

	auto checkEdge = [&](const Vec3& A, const Vec3& B)
		{
			Vec3 AB = B - A;
			float t = clamp(Dot(AB, (P - A)) / AB.SqLength(), 0.0f, 1.0f);
			Vec3 pointOnEdge = A + AB * t;
			float distSq = (P - pointOnEdge).SqLength();
			if (distSq < minDistanceSq) 
			{
				minDistanceSq = distSq;
				closest = pointOnEdge;
			}
		};

	checkEdge(V0, V1);
	checkEdge(V1, V2);
	checkEdge(V2, V0);

	return closest;
}
