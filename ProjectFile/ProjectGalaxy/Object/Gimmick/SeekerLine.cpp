﻿#include "SeekerLine.h"
#include"Player.h"
#include"Easing.h"
#include"ColliderSphere.h"

#include"SoundManager.h"

namespace
{
	constexpr int kSpeedMax = 2;
	constexpr int kRadius = 5;

	const char* kEndPointSEName = "SeekerLineEndSE.mp3";
	const char* kPlayerMoveSEName = "Fastener.mp3";
}


float EaseInOutNum(float x)
{
	return -1.f * (cos(x * DX_PI_F) - 1.f) / 2.f;
}


SeekerLine::SeekerLine(std::vector<Vec3> points, int color, bool isActive) : Collidable(Priority::StageGimmick, ObjectTag::SeekerLine),
m_endPointSoundHandle(SoundManager::GetInstance().GetSoundData(kEndPointSEName)),
m_movePlayerSoundHandle(SoundManager::GetInstance().GetSoundData(kPlayerMoveSEName)),
m_hitPointNum(0),
m_length(0),
m_speed(0),
m_num(0),
m_color(color),
m_ratio(0)
{
	SetIsActive(isActive);
	for (auto& point : points)
	{
		m_points.push_back(point);
	}
	m_rigid->SetPos(m_points.front());
	AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);
	auto item = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
	item->radius = kRadius;
	item->m_isTrigger = true;
	SetAntiGravity();
	for (int i = 1; i < m_points.size(); i++)
	{
		m_length += (m_points[i-1] - m_points[i]).Length();
	}
	m_speed = 1.f;
	m_isIgnore = true;
}

SeekerLine::~SeekerLine()
{
}

void SeekerLine::Init()
{
}

void SeekerLine::Update()
{
	if (!m_player.lock().get())return;
	
	//ポイントの更新
	m_num += m_speed;
	if (m_num > m_length/2)
	{
		m_speed *= -1 ;
	}
	float lenge = (m_rigid->GetPos() - m_player.lock()->GetPos()).Length();
	float ratio = (lenge / m_playerStartPos.Length());
	m_ratio += 0.002f;

	if (m_ratio > 1)
	{
		m_ratio = 1;
	}

	m_velocity = m_points[m_hitPointNum+1] - m_player.lock()->GetPos();
	m_velocity.Normalize();
	//m_player->SetPos(EaseInOut(m_points[m_hitPointNum], m_points[m_hitPointNum + 1], 1, 2));
	m_player.lock()->SetPos(m_player.lock()->GetPos() + m_velocity * m_num);
	m_player.lock()->SetVelocity(m_velocity * m_num);
	PlaySoundMem(m_movePlayerSoundHandle, DX_PLAYTYPE_BACK);
	//m_player->SetUpVec(Cross(m_velocity,Vec3::Front()*-1));

	//m_player->SetPos((m_points[m_hitPointNum+1]));

	if ((m_player.lock()->GetPos() - m_points[m_hitPointNum + 1]).Length() <= 5)//次のポイントに到達したら
	{
		m_hitPointNum++;
		if (m_points[m_hitPointNum] == m_points.back())//次のポイントが最後だったら
		{
			PlaySoundMem(m_endPointSoundHandle, DX_PLAYTYPE_BACK);
			m_player.lock()->SetIsOperation(false);
			//プレイヤーをジャンプさせる
			m_player.lock()->CommandJump();

			m_hitPointNum = 0;
			m_player.reset();
		}
	}

}

void SeekerLine::Draw()
{
	DrawSphere3D(m_points[0].VGet(), kRadius, 8, 0xffff00, 0xffffff, false);
	for (int i = 0; i < m_points.size(); i++)
	{
		if (i <= 0)continue;
		DrawLine3D((m_points.begin() + i-1)->VGet(), (m_points.begin() + i)->VGet(), m_color);
		if ((m_points.begin() + i) == m_points.end())return;
	}
}

void SeekerLine::OnTriggerEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag)
{
	if (colider->GetTag() == ObjectTag::Player)
	{
		
		m_num = 0;
		m_speed = 0.02f;
		m_player = std::dynamic_pointer_cast<Player>(colider);
		m_player.lock()->SetIsOperation(true);
		m_player.lock()->SetPos(m_rigid->GetPos());
	}
}

