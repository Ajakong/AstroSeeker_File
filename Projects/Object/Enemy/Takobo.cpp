#include "Takobo.h"
#include"ColliderSphere.h"
#include"Physics.h"
#include"Player.h"
#include"Coin.h"
#include"SoundManager.h"
#include"ModelManager.h"

#include"Easing.h"
namespace
{
	constexpr float kScaleMag = 0.03f;
	constexpr float kCollisionRadius = 50.f/20.f;


	constexpr float kAnimFrameSpeed = 30.0f;//アニメーション進行速度

	//アニメーションの切り替えにかかるフレーム数
	constexpr float kAnimChangeFrame = 8.0f;
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;

	constexpr float kFrameParSecond = 60.0f;

	/// <summary>
	/// 最大HP
	/// </summary>
	constexpr int kHp = 50;

	constexpr int kStartPosX = 200;
	constexpr int kStartPosY = 50;
	constexpr int kStartPosZ = 0;

	/// <summary>
	/// 足元からモデルの中心までの距離
	/// </summary>
	constexpr int kFootToCenter = 30;

	constexpr float kFiringRange = 100;

	/// <summary>
	/// 攻撃クールタイム中の最低移動速度
	/// </summary>
	constexpr int kIdleSpeed = 20;

	/// <summary>
	/// 球の生成間隔
	/// </summary>
	constexpr int kSphereCreateFrame = 50;

	/// <summary>
	/// 再攻撃までのクールタイム
	/// </summary>
	constexpr int kAttackCoolDownTime = 300;
	constexpr float kTimeToStrike = 200.f;

	/// <summary>
	/// ステージモデルの縦横サイズ/2
	/// </summary>
	constexpr int kStageSizeHalf = 200;

	const char* kShotSEhandlePath = "Shot.mp3";

	const char* kTakoboFileName = "shotMonster";

	const char* name = "takobo";
}

Takobo::Takobo(Vec3 pos,std::shared_ptr<MyEngine::Collidable> target) :Enemy( Priority::Low, ObjectTag::Takobo),

m_hp(kHp),

m_attackCoolDownCount(0),
m_centerToEnemyAngle(0)
{
	m_power = 10;

	m_target=target;
	SetCreate3DSoundFlag(true);
	m_shotSEHandle = SoundManager::GetInstance().GetSoundData(kShotSEhandlePath);
	SetCreate3DSoundFlag(false);
	Set3DRadiusSoundMem(1000, m_shotSEHandle);
	m_enemyUpdate = &Takobo::IdleUpdate;
	m_rigid->SetPos(pos);
	AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);
	auto item = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
	item->radius = kCollisionRadius;
	m_moveShaftPos = m_rigid->GetPos();
	AddThroughTag(ObjectTag::Takobo);
	AddThroughTag(ObjectTag::Kuribo);
	AddThroughTag(ObjectTag::Gorori);
	AddThroughTag(ObjectTag::WarpGate);
	m_modelHandle=ModelManager::GetInstance().GetModelData(kTakoboFileName);
	MV1SetScale(m_modelHandle, VGet(kScaleMag, kScaleMag, kScaleMag));
	ChangeAnim(Idle);

	m_modelHeadIndex= MV1SearchFrame(m_modelHandle, "Head");

}

Takobo::~Takobo()
{
	{
		auto result = remove_if(m_sphere.begin(), m_sphere.end(), [this](const auto& sphere)
		{
			return true;
		});
		m_sphere.erase(result, m_sphere.end());

	}

	{
		auto result = remove_if(m_killer.begin(), m_killer.end(), [this](const auto& sphere)
		{
			return true;
		});
		m_killer.erase(result, m_killer.end());
	}
	
}

void Takobo::Init()
{
}

void Takobo::Update()
{
	//ローカル上方向ベクトルいい感じに線形保管
	m_upVec = Slerp(m_upVec, m_nextUpVec, 1.f);
	(this->*m_enemyUpdate)();

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

void Takobo::SetMatrix()
{
	MV1SetRotationZYAxis(m_modelHandle, (m_attackDir * -1).VGet(), m_upVec.GetNormalized().VGet(), 0);
	MV1SetPosition(m_modelHandle, m_rigid->GetPos().VGet());
}

void Takobo::DeleteManage()
{
	{
		auto result = remove_if(m_sphere.begin(), m_sphere.end(), [this](const auto& sphere)
		{
			bool isOut = sphere->IsDestroy();
			if (isOut)
			{
				m_sphereNum--;
				sphere->OnDestroy();
				MyEngine::Physics::GetInstance().Exit(sphere);
			}
			return isOut;
		});
		m_sphere.erase(result, m_sphere.end());
	}

	{
		auto result = remove_if(m_killer.begin(), m_killer.end(), [this](const auto& sphere)
		{
			bool isOut = sphere->IsDestroy();
			if (isOut)
			{
				m_sphereNum--;
				sphere->OnDestroy();
				MyEngine::Physics::GetInstance().Exit(sphere);
			}
			return isOut;
		});
		m_killer.erase(result, m_killer.end());
	}
	
}

void Takobo::Draw()
{
	//DrawSphere3D(m_rigid->GetPos().VGet(), kCollisionRadius, 10, 0xff0000, 0xff0000, false);
	MV1DrawModel(m_modelHandle);

#ifdef DEBUG
	DrawLine3D(m_rigid->GetPos().VGet(), (m_rigid->GetPos() + m_attackDir * 1000).VGet(), 0xff0000);
	DrawLine3D(m_rigid->GetPos().VGet(), (m_rigid->GetPos() + m_upVec * 60).VGet(), 0xff0000);
	DrawSphere3D(m_strikePoint.VGet(), 6, 8, 0xff0000, 0x000000, true);
#endif
}

void Takobo::OnCollideEnter(std::shared_ptr<MyEngine::Collidable> colider,ColideTag ownTag,ColideTag targetTag)
{
	if (colider->GetTag() == ObjectTag::Stage)
	{
		m_nowPlanetPos = colider->GetRigidbody()->GetPos();
	}
	if (colider->GetTag() == ObjectTag::Player)
	{
		//スピンを食らえばお前は死ぬ！
		if (colider->GetState()==State::Spin)
		{
			Death();
		}
	}
	if (colider->GetTag() == ObjectTag::EnemyAttack)
	{
		auto attack= dynamic_pointer_cast<EnemySphere>(colider);

		if (attack->GetCounterFlag())
		{
			m_hp -= colider->GetPower();
			Vec3 dir = m_rigid->GetPos() - colider->GetRigidbody()->GetPos();
			dir.Normalize();
			m_rigid->AddVelocity(dir * 2);
		}
	}
}

void Takobo::OnTriggerEnter(std::shared_ptr<MyEngine::Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
	if (colider->GetTag() == ObjectTag::PlayerImpact)
	{
		m_rigid->AddVelocity(m_upVec);
		m_hp -= colider->GetPower();
	}
}

Vec3 Takobo::GetPos() const
{
	return  VGet(m_rigid->GetPos().x, m_rigid->GetPos().y + kFootToCenter, m_rigid->GetPos().z);
}

Vec3 Takobo::GetUpVec() const
{
	return m_upVec;
}

Vec3 Takobo::GetFrontVec() const
{
	return m_attackDir;
}

Vec3 Takobo::GetHeadPos() const
{
	return MV1GetFramePosition(m_modelHandle, m_modelHeadIndex);
}

void Takobo::SetTarget(std::shared_ptr<MyEngine::Collidable> target)
{
	m_target = target;
}

void Takobo::Death()
{
	m_enemyUpdate = &Takobo::DeathUpdate;
}

bool Takobo::UpdateAnim(int attachNo)
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

void Takobo::ChangeAnim(int animIndex, int speed)
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

void Takobo::IdleUpdate()
{
	if ((m_rigid->GetPos()- m_target->GetRigidbody()->GetPos()).Length() > kFiringRange) return;
	//攻撃インターバルの更新
	m_attackCoolDownCount++;

	if (m_attackCoolDownCount > kAttackCoolDownTime)
	{
		int attackState = GetRand(1);
		switch (attackState)
		{
		case 0:
		{
			
			//プレイヤーが視界に入っているか
			
			//float a = acos(Dot(norm, toTarget.GetNormalized())) * 180 / DX_PI_F;

			//if (a < 120)//視界に入っていたら
			{
				//インターバルを初期化して
				m_attackCoolDownCount = 0;
				//攻撃
				m_attackDir = GetAttackDir();//オブジェクトに向かうベクトルを正規化したもの
				ChangeAnim(Shot);
				m_enemyUpdate = &Takobo::AttackSphereUpdate;
			}
			break;
		}
		default:
			m_attackCoolDownCount = 250;
			break;
		}
	}
}

void Takobo::DeathUpdate()
{
	m_dropItem = std::make_shared<Coin>(m_rigid->GetPos(), true);
	Physics::GetInstance().Entry(m_dropItem);

	m_isDestroy = true;
}

void Takobo::AttackSphereUpdate()
{
	float now = MV1GetAttachAnimTime(m_modelHandle, m_currentAnimNo);//現在の再生カウント
	
	m_rigid->SetVelocity(VGet(0, 0, 0));

	if (now > 35)//モーションが発射モーションになった時
	{
		if (m_createFrameCount == 0)//一度も弾を生成してなければ
		{
			//音の更新
			Set3DPositionSoundMem(m_rigid->GetPos().VGet(), m_shotSEHandle);
			PlaySoundMem(m_shotSEHandle, DX_PLAYTYPE_BACK);
			
			Vec3 headPos = MV1GetFramePosition(m_modelHandle, m_modelHeadIndex);
			//着弾地点の設定
			m_strikePoint = m_target->GetRigidbody()->GetPos();
			Vec3 toVec = m_rigid->GetPos()- m_strikePoint;
			//攻撃方向の確定
			m_attackDir = toVec.GetNormalized();
			m_shotUpVec = m_target->GetUpVec();

			Vec3 dir = m_attackDir + m_upVec * 2;
			dir.Normalize();
			m_killer.push_back(std::make_shared<Killer>(Priority::Low, ObjectTag::EnemyAttack, shared_from_this(), m_target, headPos, dir, 1, 0xff0000));
			MyEngine::Physics::GetInstance().Entry(m_killer.back());
		
			m_sphereNum++;
			m_createFrameCount = 1;
		}
	}
	if (now > 100)
	{
		m_createFrameCount = 0;
		ChangeAnim(Idle);
		m_enemyUpdate = &Takobo::IdleUpdate;
	}
}

Vec3 Takobo::GetAttackDir() const
{
	Vec3 toVec = m_rigid->GetPos()- m_target->GetRigidbody()->GetPos();
	Vec3 vec = (m_rigid->GetPos()- m_target->GetRigidbody()->GetPos()).GetNormalized();
	vec = VGet(vec.x * abs(toVec.x), vec.y * abs(toVec.y), vec.z * abs(toVec.z));
	return vec;
}
