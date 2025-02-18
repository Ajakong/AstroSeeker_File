#include "StampImpact.h"
#include"Quaternion.h"

namespace
{
	constexpr float kSegment = 200;
	constexpr int kLifeTimeMax = 100;
}
using namespace MyEngine;
StampImpact::StampImpact(Vec3 pos, float planetRadius, Vec3 dir,ObjectTag tag, float speed) :Collidable(Priority::Static,tag),
	m_radiusMax(planetRadius),
	m_dir(dir),
	m_nowRadius(0),
	m_speed(speed),
	m_deleteFlag(false),
	m_lifeTime(0)
{
	m_impactUpdate = &StampImpact::ExpansionUpdate;
	m_rigid->SetPos(pos);
	for (int i = 0; i < kSegment; i++)
	{
		auto lineCol = std::dynamic_pointer_cast<ColliderLine3D>(AddCollider(ColliderBase::Kind::Line, ColideTag::Body)->col);
		m_lines.push_back(lineCol);
	}
	SetAntiGravity(true);
}

StampImpact::~StampImpact()
{
}

void StampImpact::Init()
{
}

void StampImpact::Update()
{
	//LifeTimeを超過するか直径分進んだら削除
	//移動総数は2Rになり、拡大するのがR、縮小するのもRなので拡大縮小変化総量は2Rとなり
	//1フレームごとの位置変化量と拡大縮小の変化量は等しくなる
	m_lifeTime++;
	if(m_lifeTime>kLifeTimeMax)m_deleteFlag = true;
	m_rigid->SetVelocity(m_dir*m_speed);
	(this->*m_impactUpdate)();
}

void StampImpact::Draw()
{
	Draw3DCircle(m_rigid->GetPos(), m_nowRadius, kSegment);
	DrawSphere3D(m_rigid->GetPos().VGet(),3,20,0xff0000, 0xff0000,true);
}

void StampImpact::OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
	
}

void StampImpact::Draw3DCircle(Vec3 center, float radius, int num_segments)
{
	// 円の中心からのベクトルを法線として正規化
	Vec3 normal = m_dir.GetNormalized(); // 向きのベクトルを正規化

	// 円の法線ベクトルに垂直な2つの直交ベクトルを求める
	Vec3 tangent1 = (std::abs(normal.x) < 0.1f) ? Vec3(1, 0, 0) : Vec3(0, 1, 0);
	tangent1 = Cross(tangent1, normal).GetNormalized();
	Vec3 tangent2 = Cross(normal, tangent1).GetNormalized();

	// 弦の長さの計算
	float radiusDiff = m_radiusMax - radius-2;
	float rad = (radiusDiff > m_radiusMax) ? m_radiusMax : sqrtf(m_radiusMax * m_radiusMax - radiusDiff * radiusDiff);

	int i = 0;
	// 2つの直交ベクトルを使って円周を描画
	for (auto line : m_lines)
	{
		// 円周の各点を計算
		float theta1 = 2.0f * DX_PI_F * i / num_segments;
		float theta2 = 2.0f * DX_PI_F * (i + 1) / num_segments;

		// 円周上の2点
		line->line.start = center + (tangent1 * cosf(theta1) + tangent2 * sinf(theta1)) * rad;
		line->line.end = center + (tangent1 * cosf(theta2) + tangent2 * sinf(theta2)) * rad;

		// 3D空間に円を描画
		DrawLine3D(line->line.start.VGet(), line->line.end.VGet(), GetColor(250, 50, 50)); // 青色で円周を描画
		i++;
	}
}

void StampImpact::ExpansionUpdate()
{
	m_nowRadius+=m_speed;
	if (m_nowRadius > m_radiusMax)
	{
		m_impactUpdate = &StampImpact::ReductionUpdate;
	}
}

void StampImpact::ReductionUpdate()
{
	m_nowRadius-= m_speed;
	if (m_nowRadius < 0)
	{
		m_deleteFlag = true;
	}
}
