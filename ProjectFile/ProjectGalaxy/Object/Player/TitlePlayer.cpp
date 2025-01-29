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


TitlePlayer::TitlePlayer(int modelhandle) : Player(modelhandle)
{
	//ChangeAnim(AnimationNumRun);
	m_titlePlayerUpdate = &TitlePlayer::IdleUpdate;
}

TitlePlayer::~TitlePlayer()
{
}

bool TitlePlayer::MoveToTargetPosWithSticker(Vec3 targetPos)
{
	if (m_sphere.size() != 0)if (!m_sphere.back()->GetStickFlag())return false;
	m_shotDir = (targetPos - m_rigid->GetPos()).GetNormalized();
	(this->*m_shotUpdate)();

	if (m_isOperationFlag)
	{
		return true;
	}
	return false;
}

void TitlePlayer::Update()
{
	m_upVec = Slerp(m_upVec, m_nextUpVec, 0.1f);
	m_isSearchFlag = false;
	m_radius = 0;

	(this->*m_titlePlayerUpdate)();


	int index = MV1SearchFrame(m_modelHandle, "mixamorig:Spine");
	MATRIX shotDirMat = MGetRotVec2(m_nowVec.VGet(), m_shotDir.VGet());
	m_nowVec = m_shotDir.VGet();

	/*for (auto& item : m_sphere)
	{
		item->Update();
	}*/
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
	m_state = "Idle";

}

void TitlePlayer::StopUpdate()
{
	m_rigid->SetVelocity(Vec3::Zero());
}

void TitlePlayer::DoNotMove()
{
	m_titlePlayerUpdate = &TitlePlayer::StopUpdate;
}