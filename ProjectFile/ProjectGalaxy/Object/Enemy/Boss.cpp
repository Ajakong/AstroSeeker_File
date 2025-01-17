#include "Boss.h"
#include"Planet.h"
#include"Physics.h"
#include"Easing.h"
namespace
{
	constexpr int kHPFull = 60;
	constexpr float kBodyRadiusSize = 20.f;
	constexpr int kKnockBackFrameMax = 50;

	constexpr float kAnimFrameSpeed = 30.0f;//アニメーション進行速度

	//アニメーションの切り替えにかかるフレーム数
	constexpr float kAnimChangeFrame = 8.0f;
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;

	constexpr float kFrameParSecond = 60.0f;

	constexpr int kActionFrame = 100;
}
Boss::Boss(Vec3 pos):Enemy(Priority::Boss,ObjectTag::Enemy),
	m_jumpCount(0),
	m_actionFrame(0),
	m_isHit(false),
	m_knockBackFrame(0)
{
	m_hp = kHPFull;
	m_rigid->SetPos(pos);
	AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);
	m_collision = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
	m_collision->radius = kBodyRadiusSize;

	m_color = 0xff00ff;
	m_bossUpdate = &Boss::NeutralUpdate;
}

Boss::~Boss()
{
}

void Boss::Init()
{
}

void Boss::Update()
{
	//上方向ベクトルをいい感じに線形保管
	m_upVec = Slerp(m_upVec, m_nextUpVec, 1.f);
	(this->*m_bossUpdate)();
	if (m_isHit)
	{
		m_knockBackFrame++;
		m_color = 0xff0000;
	}
	if (m_knockBackFrame > 30)
	{
		m_isHit = false;
		m_color = 0xff00ff;
	}
	if (m_hp <= 0)
	{
		m_dropItem = std::make_shared<ClearObject>(m_rigid->GetPos(), true);
		Physics::GetInstance().Entry(m_dropItem);
		m_isDestroyFlag = true;
	}
	DeleteObject(m_impacts);
}

void Boss::Draw()
{
	DrawSphere3D(m_rigid->GetPos().VGet(), kBodyRadiusSize,8,m_color,0x000000,true);
	
}

void Boss::InitUpdate()
{
}

void Boss::RestUpdate()
{

}

void Boss::NeutralUpdate()
{
	m_actionFrame++;
	if (m_actionFrame < kActionFrame)return;
	m_actionFrame = 0;
	switch (GetRand(1))
	{
	case(0):
		m_rigid->AddVelocity(m_upVec * 2);
		m_bossUpdate = &Boss::JumpingUpdate;
		break;
	case(1):
		m_rigid->AddVelocity(m_upVec * 4);
		m_bossUpdate = &Boss::FullpowerJumpUpdate;
		break;
	}
	
}

void Boss::AnglyUpdate()
{
}

void Boss::DestroyPlanetUpdate()
{
}

void Boss::KnockBackUpdate()
{
	m_color = 0xff0000;
	m_knockBackFrame++;
	if (m_knockBackFrame > kKnockBackFrameMax)
	{
		m_color = 0xff00ff;
		m_knockBackFrame = 0;
		m_bossUpdate = &Boss::NeutralUpdate;
	}
}

void Boss::JumpingUpdate()
{
	
	if (m_collision->OnHit())
	{
		if (m_jumpCount > 2)
		{
			m_jumpCount = 0;
			switch (GetRand(1))
			{
			case(0):
				m_bossUpdate = &Boss::NeutralUpdate;
				break;
			case(1):
				m_rigid->AddVelocity(m_upVec * 4);
				m_bossUpdate = &Boss::FullpowerJumpUpdate;
				break;
			}

		}
		else
		{
			m_jumpCount++;
			m_impacts.push_back(std::make_shared<StampImpact>(m_rigid->GetPos() + m_upVec * -kBodyRadiusSize, m_nowPlanet->GetScale(), m_upVec * -1, ObjectTag::EnemyAttack));
			MyEngine::Physics::GetInstance().Entry(m_impacts.back());
			m_rigid->AddVelocity(m_upVec * 2);
		}
		
	}
	
}

void Boss::FullpowerJumpUpdate()
{
	if (m_collision->OnHit())
	{
		m_impacts.push_back(std::make_shared<StampImpact>(m_rigid->GetPos() + m_upVec * -kBodyRadiusSize, m_nowPlanet->GetScale(), m_upVec * -1, ObjectTag::EnemyAttack,2.f));
		MyEngine::Physics::GetInstance().Entry(m_impacts.back());

		//HPが少ないほど隙がなくなる
		m_actionFrame = -m_hp;
		m_bossUpdate = &Boss::LandingUpdate;

	}
}

void Boss::LandingUpdate()
{
	m_actionFrame++;
	m_color = 0x00ff00;
	if (m_actionFrame > 0)
	{
		m_color = 0xff00ff;;
		m_bossUpdate = &Boss::NeutralUpdate;
	}
}


bool Boss::UpdateAnim(int attachNo)
{
	//アニメーションが設定されていないので終了
	if (attachNo == -1) return false;

	//アニメーションを進行させる
	float now = MV1GetAttachAnimTime(m_modelHandle, attachNo);//現在の再生カウント
	now += kAnimFrameSpeed * m_animationSpeed / kFrameParSecond;//アニメーションカウントを進める


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

void Boss::ChangeAnim(int animIndex, int speed)
{
	m_animationSpeed = speed;
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

void Boss::OnCollideEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
	if (colider->GetTag() == ObjectTag::PlayerBullet)
	{
		m_isHit = true;
	}
}

void Boss::OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
	if (colider->GetTag() == ObjectTag::Stage)
	{
		m_nowPlanet = std::dynamic_pointer_cast<Planet>(colider);
	}
	if (m_bossUpdate == &Boss::LandingUpdate)
	{
		if (colider->GetTag() == ObjectTag::PlayerImpact)
		{
			m_rigid->AddVelocity(m_upVec * 4);
			m_bossUpdate = &Boss::KnockBackUpdate;
			m_hp -= 20;
		}
		if (colider->GetTag() == ObjectTag::PlayerBullet)
		{
			m_knockBackFrame = 44;
			m_bossUpdate = &Boss::KnockBackUpdate;
			m_hp -= 2;
		}
	}
}

template <typename T>
void Boss::DeleteObject(std::vector<std::shared_ptr<T>>& objects)
{
	auto result = remove_if(objects.begin(), objects.end(), [this](const auto& object)
		{
			if (object->GetDeleteFlag())
			{
				MyEngine::Physics::GetInstance().Exit(object);
			}
			return object->GetDeleteFlag(); // IsDestroy() が true の場合削除
		});
	objects.erase(result, objects.end());
}


