#include "TitlePlayer.h"
#include"Pad.h"

#include"Camera.h"
#include"SoundManager.h"
#include"Gorori.h"
#include"EnemySphere.h"
#include"PlayerSphere.h"
#include"PlayerStickSphere.h"
#include"KillerTheSeeker.h"
#include"ModelManager.h"
#include"GraphManager.h"
#include"Planet.h"

#include"Easing.h"

#include"Physics.h"


TitlePlayer::TitlePlayer() : Player(),
m_titleUpdateNum(0)
{
	//アイドル状態に設定
	ChangeAnim(AnimNum::AnimationNumIdle);
	m_titlePlayerUpdate = &TitlePlayer::IdleUpdate;

	//グラッピング弾に設定
	m_shotUpdate = &Player::ShotTheStickStar;
}

TitlePlayer::~TitlePlayer()
{

	for (auto& item : m_sphere)
	{
		MyEngine::Physics::GetInstance().Exit(item);
	}
	m_sphere.clear();
}

bool TitlePlayer::MoveToTargetPosWithSticker(Vec3 targetPos)
{
	//弾を撃っていないとき
	if (m_sphere.size() != 0)if (!m_sphere.back()->GetStickFlag())return false;
	m_shotDir = (targetPos - m_rigid->GetPos()).GetNormalized();
	(this->*m_shotUpdate)();

	if (m_isOperation)
	{
		return true;
	}
	return false;
}

void TitlePlayer::SetShot()
{
	if (MV1GetAttachAnimTotalTime(m_modelHandle, m_currentAnimNo) != 120)
	{
		ChangeAnim(AnimNum::AnimationNumShotPose);
	}
	
}

void TitlePlayer::Update()
{
	m_upVec = Slerp(m_upVec, m_nextUpVec, 0.1f);
	m_isSearch = false;

	(this->*m_titlePlayerUpdate)();


	int index = MV1SearchFrame(m_modelHandle, "mixamorig:Spine");
	MATRIX shotDirMat = MGetRotVec2(m_postShotVec.VGet(), m_shotDir.VGet());
	m_postShotVec = m_shotDir.VGet();

	DeleteManage();


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

void TitlePlayer::IdleUpdate()
{
	m_upVec = Vec3::Up();

	m_stateName = "Idle";
	m_state = State::Neutral;

}

void TitlePlayer::StopUpdate()
{
	m_upVec = Vec3::Up();
	m_rigid->SetVelocity(Vec3::Zero());
}

void TitlePlayer::DoNotMove()
{
	m_titlePlayerUpdate = &TitlePlayer::StopUpdate;
}

void TitlePlayer::Move()
{
	m_titlePlayerUpdate = &TitlePlayer::OperationUpdate;
}

void TitlePlayer::MoveToTargetWithStickStar(Vec3 targetPos)
{
	if (m_titleUpdateNum == 1)
	{
		if (m_sphere.size() == 0)
		{
			Vec3 targetVec = (targetPos - m_rigid->GetPos()).GetNormalized();
			Vec3 shotPos = MV1GetFramePosition(m_modelHandle, m_leftHandFrameIndex);
			m_sphere.push_back(std::make_shared<PlayerStickSphere>(Priority::Low, ObjectTag::PlayerBullet, shared_from_this(), shotPos, targetVec, m_sideVec, 1, 0xff0000));
			MyEngine::Physics::GetInstance().Entry(m_sphere.back());
			m_sphere.back()->Init();

			Player::SetNeutralUpdate();
		}
		else
		{
			//何かに衝突しているか
			auto isColid = m_sphere.back()->GetStickFlag();
			if (isColid)
			{
				m_titleUpdateNum = 2;
				m_sphere.back()->Effect();

			}
		}

	}

	if (m_titleUpdateNum == 0)
	{
		if (m_sphere.size() == 0)
		{
			Vec3 targetVec = (targetPos - m_rigid->GetPos()).GetNormalized();
			Vec3 shotPos = MV1GetFramePosition(m_modelHandle, m_leftHandFrameIndex);
			m_sphere.push_back(std::make_shared<PlayerStickSphere>(Priority::Low, ObjectTag::PlayerBullet, shared_from_this(), shotPos, targetVec, m_sideVec, 1, 0xff0000));
			MyEngine::Physics::GetInstance().Entry(m_sphere.back());
			m_sphere.back()->Init();

			MV1SetScale(m_modelHandle, VGet(0.01f, 0.01f, 0.01f));
			m_moveDir = Cross(GetCameraRightVector(), m_upVec);
			ChangeAnim(AnimNum::AnimationNumIdle);
			Player::SetNeutralUpdate();
		}
		else
		{

			auto isColid = m_sphere.back()->GetStickFlag();
			if (isColid)
			{
				m_titleUpdateNum = 1;
				m_sphere.back()->Effect();

			}
		}
	}
}
