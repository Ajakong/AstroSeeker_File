#include "SamuraiAlter.h"
#include"ModelManager.h"

namespace
{
	const char* kModelFileName = "AlienSoldier.mv1";

	constexpr float kFrameParSecond = 60.0f;

	constexpr float kScaleMag = 0.3f;\

	constexpr float kAnimFrameSpeed = 30.0f;//アニメーション進行速度

	//アニメーションの切り替えにかかるフレーム数
	constexpr float kAnimChangeFrame = 8.0f;
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;
}

SamuraiAlter::SamuraiAlter(Vec3 pos) : Enemy(Priority::High,ObjectTag::Enemy),
m_animBlendRate(0),
m_currentAnimNo(-1),
m_prevAnimNo(0),
m_neckNowDir(Vec3(0, 0, -1)),
m_isFindTarget(false),
m_armMoveDir(Vec3::Zero())
{
	{
		AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);
		auto item = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
		item->radius = 6;

	}
	m_rigid->SetPos(pos);
	m_modelHandle = ModelManager::GetInstance().GetModelData(kModelFileName);
	ChangeAnim(AnimNum::AnimationNumIdle);
	m_update = &SamuraiAlter::IntroUpdate;
}

SamuraiAlter::~SamuraiAlter()
{
}

void SamuraiAlter::Init()
{
}

void SamuraiAlter::Update()
{
	(this->*m_update)();
	//アニメーションの更新
	UpdateAnim(m_currentAnimNo);
	//変更前のアニメーション100%
	DxLib::MV1SetAttachAnimBlendRate(m_modelHandle, m_prevAnimNo, 1.0f - m_animBlendRate);
	//変更後のアニメーション0%
	DxLib::MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAnimNo, m_animBlendRate);
	m_animBlendRate += 0.05f;
	if (m_animBlendRate > 1.0f)
	{
		m_animBlendRate = 1.0f;
	}
}

void SamuraiAlter::SetMatrix()
{
	MV1SetRotationZYAxis(m_modelHandle, (m_moveDir * -1).VGet(), m_upVec.GetNormalized().VGet(), 0);
	MV1SetPosition(m_modelHandle, m_rigid->GetPos().VGet());
}

void SamuraiAlter::Draw()
{
	MV1DrawModel(m_modelHandle);
}

void SamuraiAlter::IntroUpdate()
{
	m_update = &SamuraiAlter::IdleUpdate;
}

void SamuraiAlter::IdleUpdate()
{
	
}

void SamuraiAlter::AttackUpdate()
{
}

bool SamuraiAlter::UpdateAnim(int attachNo)
{
	//アニメーションが設定されていないので終了
	if (attachNo == -1) return false;

	//アニメーションを進行させる
	float now = MV1GetAttachAnimTime(m_modelHandle, attachNo);//現在の再生カウント
	now += kAnimFrameSpeed * m_animSpeed / kFrameParSecond;//アニメーションカウントを進める

	//現在再生中のアニメーションの総カウントを取得する
	float total = MV1GetAttachAnimTotalTime(m_modelHandle, attachNo);
	bool isLoop = false;
	while (now >= total)
	{
		now -= total;
		isLoop = true;
	}

	MV1SetAttachAnimTime(m_modelHandle, attachNo, now);

	return isLoop;
}

void SamuraiAlter::ChangeAnim(int animIndex, float speed)
{
	m_animSpeed = speed;
	//さらに古いアニメーションがアタッチされている場合はこの時点で削除しておく
	if (m_prevAnimNo != -1)
	{
		MV1DetachAnim(m_modelHandle, m_prevAnimNo);
	}

	//現在再生中の待機アニメーションは変更前のアニメーション扱いに
	m_prevAnimNo = m_currentAnimNo;

	//変更後のアニメーションとして攻撃アニメーションを改めて設定する
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, animIndex, -1, false);

	//切り替えの瞬間は変更前のアニメーションが再生される状態にする
	m_animBlendRate = 0.0f;

	//変更前のアニメーション100%
	DxLib::MV1SetAttachAnimBlendRate(m_modelHandle, m_prevAnimNo, 1.0f - m_animBlendRate);
	//変更後のアニメーション0%
	DxLib::MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAnimNo, m_animBlendRate);
}
