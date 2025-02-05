#include "StarCapture.h"

#include"PlayerSphere.h"
#include"Player.h"
#include"Easing.h"

namespace
{
	constexpr int kCaptureMaxFrame = 200;
	constexpr int kRecastTimeMax = 300;
	constexpr int kCaptureColLifeTime = 500;

}

StarCapture::StarCapture(Vec3 pos, float radius, float captureRadius) : Collidable(Priority::StageGimmick, ObjectTag::StarCapture),
m_isCapturePlayer(false),
m_captureFrame(0),
m_recastTime(0),
m_captureColLifeTime(0),
m_isCreateCol(false),
m_modelhandle(-1)
{
	m_rigid->SetPos(pos);
	m_radius = radius;
	m_captureRadius = captureRadius;
	SetAntiGravity();
	m_stateUpdate = nullptr;
	AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);
	auto item = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
	item->radius = m_radius;
	m_rigid->SetVelocity(Vec3(0, 0, 0));
	m_ratio = 0;
}

StarCapture::~StarCapture()
{
}

void StarCapture::Init()
{
}

void StarCapture::Update()
{
	if (m_isCapturePlayer)
	{
		m_captureFrame++;
		//if (m_captureFrame > kCaptureMaxFrame)
		{
			m_isCapturePlayer = false;
			m_captureFrame = 0;
			m_recastTime = 0;
		}
		
	}
	else
	{
		m_recastTime++;
	}
}

void StarCapture::Draw()
{
	MV1DrawModel(m_modelhandle);
	
#ifdef DEBUG
	for (auto& col : m_colliders)
	{
		int color = 0x0000ff*65000*(m_recastTime/kRecastTimeMax);
		auto item = dynamic_pointer_cast<MyEngine::ColliderSphere>(col->col);
		DrawSphere3D(m_rigid->GetPos().VGet(), item->radius, 20, color, 0xffffff, false);
		DrawFormatString(0, 200, 0xddaa00, "Velocity(%f,%f,%f)", m_rigid->GetVelocity().x, m_rigid->GetVelocity().y, m_rigid->GetVelocity().z);
		DrawFormatString(0, 250, 0xddaa00,"Pos(%f,%f,%f)",m_rigid->GetPos().x, m_rigid->GetPos().y, m_rigid->GetPos().z);
	
	}
	
#endif
}

void StarCapture::OnCollideEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag)
{
	if (colider->GetTag() == ObjectTag::PlayerBullet)
	{
		if (m_recastTime < kRecastTimeMax)return;
		if (m_colliders.size() > 1)return;
		AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);
		auto item= dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
		item->radius = m_captureRadius;
		m_captureCol->col = item;
		m_captureCol->col->m_isTrigger = true;

		auto sphere = dynamic_pointer_cast<PlayerSphere>(colider);

		m_player = sphere->m_player.lock();
	}

	if (m_colliders.size() <= 1)return;

	if (colider->GetTag() == ObjectTag::Player)
	{
		m_isCapturePlayer = true;
		RemoveCollider(m_captureCol);
		m_player.lock()->SetIsOperation(false);
		
	}
}

void StarCapture::OnTriggerEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag)
{
	if (colider->GetTag() == ObjectTag::Player)
	{
		m_ratio = 0;
		m_playerStartPos = colider->GetRigidbody()->GetPos();
		m_player.lock()->SetIsOperation(true);
	}
}

void StarCapture::OnTriggerStay(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag)
{
	if (colider->GetTag() == ObjectTag::Player)
	{
		float lenge = (m_rigid->GetPos() - m_player.lock()->GetPos()).Length();
		float ratio = ((static_cast<float>(m_captureRadius) -lenge) / static_cast<float>(m_captureRadius));
		m_ratio+=0.002f;
		if (m_ratio > 1)
		{
			m_ratio = 1;
		}
		m_player.lock()->AddVelocity(EaseInOut(m_playerStartPos, m_rigid->GetPos(), m_ratio, 2) - m_player.lock()->GetPos());
	}
}

void StarCapture::OnTriggerExit(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag)
{
	/*if (colider->GetTag() == ObjectTag::Player)
	{
		m_player->SetIsOperation(false);
	}*/
}

void StarCapture::WaitingUpdate()
{
}

void StarCapture::CaptureUpdate()
{
}
