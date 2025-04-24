#include "PlayerStickSphere.h"
#include"ColliderSphere.h"
#include"Player.h"
#include"Item.h"

#include"SoundManager.h"
#include"ScreenManager.h"
#include"EffectManager.h"

#include"Game.h"
namespace
{
	/// <summary>
	/// 球の当たり判定半径
	/// </summary>
	constexpr float kSphereRadius = 0.3f;
	/// <summary>
	/// 球の生成間隔
	/// </summary>
	constexpr int kSphereCreateFrame = 50;

	constexpr float kComeBackLength = 20.f;
	constexpr float kComeBackSpeedMag = 10.f;
	constexpr float kComeBackDeleteLength = 2.f;
	constexpr float kStickCancelLength = 5.f;
	constexpr float kSphereMoveSpeed = 3.f;

	constexpr int kLifeTimeMax = 100;
	const char* kName = "Sphere";
	const char* kLineEffectName = "Line.efk";
	const char* kSphereEffectName = "StickSphere.efk";
	const char* kBoostEffectName = "BoostEffect.efk";
	const char* kGravityWaveEffectName = "GravityWave.efk";

	const char* kOperationSEName = "Fastener.mp3";

	constexpr float kTensionSpeed = 3.f;

	const char* kGaussFilterScreenName = "GaussOfSeekerLine";
	const char* kHighBrightScreenName = "HighBrightSeekerLine";
}

PlayerStickSphere::PlayerStickSphere(MyEngine::Collidable::Priority priority, ObjectTag tag, std::shared_ptr<MyEngine::Collidable> player, Vec3 pos, Vec3 velocity, Vec3 sideVec, int moveNum, int color) : PlayerSphere(priority, tag, player, pos, velocity, sideVec, moveNum, color),
m_sideVec(sideVec),
m_lifeTime(0),
m_pushCount(0),
m_lineEffectIndex(0),
m_sphereEffectIndex(0),
m_boostEffectIndex(0),
m_operationHandle(SoundManager::GetInstance().GetSoundData(kOperationSEName))
{
	
	m_player.lock() = std::dynamic_pointer_cast<Player>(player);

	//moveNumによって挙動が変わるのかもしれない(実装するかわからん)
	m_moveUpdate = &PlayerStickSphere::StraightUpdate;

	SetAntiGravity(true);

	m_emitPoint = m_player.lock()->GetLeftHandPos();
}


PlayerStickSphere::~PlayerStickSphere()
{
	//エフェクト情報の削除
	EffectManager::GetInstance().StopEffect(kLineEffectName, m_lineEffectIndex);
	EffectManager::GetInstance().StopEffect(kSphereEffectName, m_sphereEffectIndex);
	EffectManager::GetInstance().StopEffect(kBoostEffectName, m_boostEffectIndex);
	EffectManager::GetInstance().StopEffect(kGravityWaveEffectName, m_gravityWaveEffectIndex);
}

void PlayerStickSphere::Init()
{
	m_lineEffectIndex = EffectManager::GetInstance().PlayEffect(kLineEffectName, true, 0, shared_from_this());
	m_sphereEffectIndex = EffectManager::GetInstance().PlayEffect(kSphereEffectName, true, 0, shared_from_this());
	

}

void PlayerStickSphere::Update()
{
	
	m_startPos = m_player.lock()->GetLeftHandPos();
	
	(this->*m_moveUpdate)();
	//弾の描画
	EffectManager::GetInstance().SetInformationEffect(kSphereEffectName, m_sphereEffectIndex, m_rigid->GetPos(), Quaternion::GetIdentity(), Vec3(kSphereRadius, kSphereRadius, kSphereRadius));

	//m_isStickがtrueの時に赤くなる
	//DrawLine3D(m_startPos.VGet(), m_rigid->GetPos().VGet(), 0xffffff - (0x00ffff * m_isStick));

	//ラインの描画
	EffectManager::GetInstance().SetInformationEffect(kLineEffectName, m_lineEffectIndex, m_player.lock()->GetPos(), Quaternion::GetIdentity(), Vec3(kSphereRadius, kSphereRadius, kSphereRadius));

	if (m_player.lock()->GetOperationFlag())
	{
		auto rotQuaternion = Quaternion::GetQuaternion(m_player.lock()->GetFrontVec() * -1,Vec3::Back());
		//プレイヤーの移動軌跡の描画
		EffectManager::GetInstance().SetInformationEffect(kBoostEffectName, m_boostEffectIndex, m_player.lock()->GetPos(), rotQuaternion, Vec3(1, 1, 1));
		Vec3 dir = (m_player.lock()->GetPos() - m_rigid->GetPos()).GetNormalized();
		rotQuaternion = Quaternion::GetQuaternion(Vec3::Front(), dir);
		//EffectManager::GetInstance().SetInformationEffect(kGravityWaveEffectName, m_gravityWaveEffectIndex,m_rigid->GetPos() + dir * 200 + Vec3::Up() * 5, rotQuaternion, Vec3(10.f, 10.f, 10.f));
	}
}

void PlayerStickSphere::Draw()
{
}

void PlayerStickSphere::Effect()
{
	if (m_isStick)
	{
		if (m_isMoving)
		{
			m_moveUpdate = &PlayerStickSphere::ComeBackWithObjectUpdate;
		}
		else if ((m_player.lock()->GetPos() - m_rigid->GetPos()).Length() < kComeBackLength)
		{
			m_moveUpdate = &PlayerStickSphere::ComeBackUpdate;
		}
		else
		{
			//Aが入力された回数(押された分速度が上がる)
			m_pushCount++;
			
			if (!m_player.lock()->GetOperationFlag())
			{
				m_boostEffectIndex = EffectManager::GetInstance().PlayEffect(kBoostEffectName, true, 0, shared_from_this());
				m_gravityWaveEffectIndex = EffectManager::GetInstance().PlayEffect(kGravityWaveEffectName, true, 0, shared_from_this());

			}
			//プレイヤーを操作されている状態に移行する
			m_player.lock()->SetIsOperation(true);
			
			//Playerを引っ張る方向
			Vec3 tensionDir = (m_rigid->GetPos() - m_player.lock()->GetPos()).GetNormalized();
			m_player.lock()->SetVelocity(tensionDir * kTensionSpeed * static_cast<float>(m_pushCount));
		}
	}
	else m_moveUpdate = &PlayerStickSphere::ComeBackUpdate;
}

void PlayerStickSphere::OnCollideEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
	m_isStick = true;
	auto priority = colider->GetPriority();
	m_isMoving = priority == Priority::Lowest;
	m_moveUpdate = &PlayerStickSphere::StickUpdate;
	m_contactedCollidable = colider;
	m_collidableContactPosition = colider->GetRigidbody()->GetPos() - m_rigid->GetPos();
}

void PlayerStickSphere::OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
	auto ptr = std::dynamic_pointer_cast<Item>(colider);
	if (ptr != nullptr)
	{
		m_isStick = true;
		auto priority = colider->GetPriority();
		m_isMoving = priority == Priority::Lowest;
		m_moveUpdate = &PlayerStickSphere::StickUpdate;
		m_contactedCollidable = colider;
		m_collidableContactPosition = colider->GetRigidbody()->GetPos() - m_rigid->GetPos();
	}
}

void PlayerStickSphere::StraightUpdate()
{
	m_rigid->SetVelocity(m_velocity* kSphereMoveSpeed);
}

void PlayerStickSphere::StickUpdate()
{
	SetAntiGravity(true);
	m_rigid->SetVelocity(Vec3::Zero());
	
	if ((m_rigid->GetPos() - m_startPos).Length() <= kStickCancelLength)
	{
		m_player.lock()->SetIsOperation(false);
		m_moveUpdate = &PlayerStickSphere::ComeBackUpdate;
	}
}

void PlayerStickSphere::ComeBackUpdate()
{
	//弾の帰還速度
	float speed = (m_startPos - m_rigid->GetPos()).Length()/ kComeBackSpeedMag;
	m_rigid->SetVelocity((m_startPos - m_rigid->GetPos()).GetNormalized() * speed);
	
	//弾とプレイヤーの距離が近づいたら削除
	if ((m_rigid->GetPos() - m_startPos).Length() <= kComeBackDeleteLength)
	{
		StopSoundMem(m_operationHandle);
		m_isDestroy = true;
	}
}

void PlayerStickSphere::ComeBackWithObjectUpdate()
{
	m_rigid->SetVelocity((m_startPos - m_rigid->GetPos()).GetNormalized() * kSphereMoveSpeed);
	//衝突しているオブジェクトとの位置関係の固定
	m_contactedCollidable->GetRigidbody()->SetPos(m_rigid->GetPos()+m_collidableContactPosition);
	if ((m_rigid->GetPos() - m_startPos).Length() <= kComeBackDeleteLength)
	{
		m_isDestroy = true;
	}
}
