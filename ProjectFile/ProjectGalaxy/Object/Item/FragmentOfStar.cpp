#include "FragmentOfStar.h"
#include"ModelManager.h"
namespace
{
	const char* kModelName = "Shards";

	constexpr float kScale = 500.f;
}

FragmentOfStar::FragmentOfStar(Vec3 pos, bool antiGravity) : Item(pos,ObjectTag::FragmentOfStar,antiGravity)
{
	m_modelHandle = ModelManager::GetInstance().GetModelData(kModelName);
	MV1SetScale(m_modelHandle,VGet(kScale, kScale, kScale));
}

FragmentOfStar::~FragmentOfStar()
{
}

void FragmentOfStar::Update()
{
	m_postUpVec = m_upVec;//上方向ベクトルを前のフレームの上方向ベクトルにする

	MV1SetRotationZYAxis(m_modelHandle, (m_frontVec * -1).VGet(), m_upVec.GetNormalized().VGet(), 0);
	MV1SetPosition(m_modelHandle, m_rigid->GetPos().VGet());
	auto modelMat = MV1GetMatrix(m_modelHandle);
}


void FragmentOfStar::Draw()
{
	MV1DrawModel(m_modelHandle);
	DrawSphere3D(m_rigid->GetPos().VGet(), 2.5f, 7, 0x0044ff, 0xffffff, false);
}

