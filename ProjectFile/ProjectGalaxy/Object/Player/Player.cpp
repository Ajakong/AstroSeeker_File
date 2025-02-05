#include "Player.h"

#include"Pad.h"
#include"Camera.h"

#include"Gorori.h"
#include"Kuribo.h"
#include"EnemySphere.h"
#include"PlayerSphere.h"
#include"PlayerStickSphere.h"
#include"KillerTheSeeker.h"

#include"SoundManager.h"
#include"ModelManager.h"
#include"GraphManager.h"
#include"EffectManager.h"

#include"Easing.h"
#include"Effect.h"

#include"Mission.h"
#include"Physics.h"
#include"Planet.h"
#include"GalaxyCreater.h"


#include"UI.h"
/// <summary>
/// スピン専用の当たり判定を生成・体より半径が大きい当たり判定にし、スピン中にだけ出現
/// </summary>

namespace
{
	constexpr float kNeutralRadius = 2.f;
	constexpr float kNeutralHeadRadius = 1.f;//通常時の当たり半径
	constexpr float kNeutralBodyRadius = 2.f;//通常時の当たり半径
	constexpr float kNeutralFootRadius = 1.f;//通常時の当たり半径
	constexpr float kNeutralSpinRadius = 3.f;//通常時の当たり半径

	constexpr int kPlayerHPMax = 50;

	constexpr int kDamageFrameMax = 20;
	//アニメーション番号
	constexpr int kIdleAnimIndex = 1;
	//constexpr int kIdleAnimIndex = 2;//待機テスト
	constexpr int kAttackAnimIndex = 30;

	constexpr float kAnimFrameSpeed = 30.0f;//アニメーション進行速度

	//アニメーションの切り替えにかかるフレーム数
	constexpr float kAnimChangeFrame = 8.0f;
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;

	//アナログスティックによる移動関連
	constexpr float kMaxSpeed = 0.5f;//プレイヤーの最大速度
	constexpr float kAnalogRangeMin = 0.1f;//アナログスティックの入力判定範囲
	constexpr float kAnalogRangeMax = 0.8f;
	constexpr float kAnalogInputMax = 1000.0f;//アナログスティックから入力されるベクトルの最大値

	constexpr float kFrameParSecond = 60.0f;
	constexpr float kDashMag = 1.5f;


	constexpr int kAvoidFrame = 60;

	constexpr float kJumpPower = 2.f;


	constexpr int kSearchRemainTimeMax = 28;
	constexpr int kChargeRemainCoolTime = 10;

	const char* kGororiHitSEName = "Hit.mp3";
	const char* kElectroSEName = "Electro.mp3";
	const char* kShotStickSEName = "ShotSound.mp3";
	const char* kShotTheStarSEName = "ShotTheStar.mp3";
	const char* kGetItemSEName = "GetItemSE.mp3";
	const char* kSpecialItemGetSEName = "SpecialItemGetSE.mp3";
	const char* kPowerUpItemGetSEName = "PowerUpItemGetSE.mp3";
	const char* kOnParrySEName = "Parry.mp3";
	const char* kJumpDropGroundSEName = "JumpDrop_ground.mp3";

	const char* kGetSearchSEName = "Search.mp3";
	const char* name = "Player";
	const char* kFileName = "SpaceHarrier";

	const char* kLandingEffectname = "Landing.efk";
	const char* kStarEffectName = "StarEffect.efk";


}




void MTransCopy(MATRIX& in, const MATRIX& src) {
	in.m[3][0] = src.m[3][0]; in.m[3][1] = src.m[3][1]; in.m[3][2] = src.m[3][2]; in.m[3][3] = 1.f;
}


Player::Player(Vec3 pos) : Collidable(Priority::High, ObjectTag::Player),
m_modelHandle(MV1DuplicateModel(ModelManager::GetInstance().GetModelData(kFileName))),
m_parrySEHandle(SoundManager::GetInstance().GetSoundData(kOnParrySEName)),
m_getItemHandle(SoundManager::GetInstance().GetSoundData(kGetItemSEName)),
m_powerUpItemGetSEHandle(SoundManager::GetInstance().GetSoundData(kPowerUpItemGetSEName)),
m_specialItemGetSEHandle(SoundManager::GetInstance().GetSoundData(kSpecialItemGetSEName)),
m_searchSEHandle(SoundManager::GetInstance().GetSoundData(kGetSearchSEName)),
m_hitSEHandle(SoundManager::GetInstance().GetSoundData(kGororiHitSEName)),
m_elecSEHandle(SoundManager::GetInstance().GetSoundData(kElectroSEName)),
m_shotStickStarSEHandle(SoundManager::GetInstance().GetSoundData(kShotStickSEName)),
m_shotTheStarSEHandle(SoundManager::GetInstance().GetSoundData(kShotTheStarSEName)),
m_postUpVec(Vec3::Up()),
m_shotDir(Vec3::Front()),
m_moveDir(Vec3::Front()),
m_frontVec(Vec3::Front()),
m_playerUpdate(&Player::StartUpdate),
m_prevUpdate(&Player::StartUpdate),
m_hp(kPlayerHPMax),
m_radius(kNeutralRadius),
m_visibleCount(0),
m_damageFrame(0),
m_regeneRange(0),
m_angle(0),
m_spinAngle(0),
m_animBlendRate(0),
m_currentAnimNo(-1),
m_prevAnimNo(0),
m_isDeathFlag(false),
m_isJumpFlag(false),
m_isSpinFlag(false),
m_isOperationFlag(false),
m_color(0x00ffff),
m_attackRadius(0),
m_fullPowerChargeCount(0),
m_landingStanFrame(0),
m_modelBodyRotate(m_frontVec),
m_inputVec(Vec3::Front() * -1),
m_postMoveDir(Vec3::Front()),
m_modelDirAngle(0),
m_currentOxygen(0),
m_shotAnimCount(0),
m_shotAnimFlag(false),
m_titleUpdateNum(0),
m_fragmentCount(0),
m_coinCount(0)
{
	m_postUpVec = m_upVec;
	m_rigid->SetPos(pos);
	{
		AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Head);
		m_headCol = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
		m_headCol->radius = kNeutralHeadRadius - 0.2f;
	}
	{
		AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);
		m_bodyCol = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
		m_bodyCol->radius = kNeutralBodyRadius - 0.2f;
	}
	{
		AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Foot);
		m_footCol = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
		m_footCol->radius = kNeutralFootRadius - 0.2f;
	}

	{
		AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Spin);
		m_spinCol = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
		m_spinCol->radius = m_attackRadius;
	}


	m_shotUpdate = &Player::ShotTheStar;

	m_cameraEasingSpeed = 5.f;

	AddThroughTag(ObjectTag::PlayerBullet);

	DxLib::MV1SetScale(m_modelHandle, VGet(0.005f, 0.005f, 0.005f));
	SetMatrix();
	m_initMat = MV1GetLocalWorldMatrix(m_modelHandle);

	m_handFrameIndex = MV1SearchFrame(m_modelHandle, "mixamorig:LeftHand");

	m_jumpActionUpdate = &Player::JumpingSpinUpdate;
	m_dropAttackUpdate = &Player::FullPowerDropAttackUpdate;
	m_spinAttackUpdate = &Player::SpiningUpdate;

}

Player::~Player()
{
	MV1DeleteModel(m_modelHandle);
}

void Player::Init()
{

	EffectManager::GetInstance().PlayEffect(kStarEffectName, true, 0, shared_from_this());
	Mission::GetInstance().SetPlayer(std::dynamic_pointer_cast<Player>(shared_from_this()));
}

void Player::Update()
{

	m_isSearchFlag = false;
	m_radius = 0;

	if (!Pad::IsState("PlayerInput"))
	{
		if(m_playerUpdate!=&Player::TalkingUpdate)m_postUpdate = m_playerUpdate;
		
		//ChangeAnim(AnimNum::AnimationNumIdle);
		m_playerUpdate = &Player::TalkingUpdate;
	}

	(this->*m_playerUpdate)();


	if (Pad::IsTrigger(PAD_INPUT_Y))
	{
		if (m_isAimFlag)
		{
			m_isAimFlag = false;
		}
		else
		{
			m_shotDir = m_frontVec;

			m_isAimFlag = true;
		}
	}
	if ((Pad::IsTrigger(PAD_INPUT_4)))PlaySoundMem(m_searchSEHandle, DX_PLAYTYPE_BACK);
	if ((Pad::IsPress(PAD_INPUT_4)))
	{
		m_isSearchFlag = true;
	}

	SetShotDir();

	int index = MV1SearchFrame(m_modelHandle, "mixamorig:Spine");
	MATRIX shotDirMat = MGetRotVec2(m_nowVec.VGet(), m_shotDir.VGet());
	m_nowVec = m_shotDir.VGet();

	if (Pad::IsTrigger(PAD_INPUT_3))
	{
		m_shotAnimFlag = true;
		if (!m_state == State::Boosting)m_rigid->SetVelocity(Vec3::Zero());
		(this->*m_shotUpdate)();
	}
	if (m_shotAnimFlag)
	{
		m_shotAnimCount++;
		if (m_shotAnimCount > 10)
		{
			m_shotAnimFlag = false;
			m_shotAnimCount = 0;
			//ChangeAnim(MV1GetAttachAnim(m_modelHandle, m_prevAnimNo));
		}
	}

	for (auto& item : m_sphere)
	{
		item->Update();
	}
	DeleteManage();
	DeleteObject(m_impacts);

	if (m_visibleCount > 200)
	{
		m_isVisibleFlag = false;
		m_visibleCount = 0;
	}
	if (m_isVisibleFlag)
	{
		m_visibleCount++;
	}
	else
	{
		//m_spinCol->radius = m_radius;
	}

	if (m_isOnDamageFlag)
	{
		m_damageFrame--;
		if (m_damageFrame < 0)
		{
			m_damageFrame = 0;
			m_isOnDamageFlag = false;
		}

	}

	if (m_hp <= 0)
	{
		m_visibleCount = 0;
		m_isVisibleFlag = true;
		if (m_playerUpdate != &Player::DeathUpdate)
		{
			ChangeAnim(AnimNum::AnimationNumDeath);
			m_playerUpdate = &Player::DeathUpdate;
		}
		
	}

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

void Player::SetMatrix()
{

#ifdef DEBUG
	////回転軸のデバッグ表示(紫)
	//DrawLine3D(m_rigid->GetPos().VGet(), Vec3(m_rigid->GetPos() + RotateYAxis * 100).VGet(), 0xff0000);

	//回転軸のデバッグ表示(紫)
	//DrawLine3D(m_rigid->GetPos().VGet(), Vec3(m_rigid->GetPos() + axis * 100).VGet(), 0xff00ff);

	//上方向ベクトルのデバッグ表示(赤)
	DrawLine3D(m_rigid->GetPos().VGet(), Vec3(m_rigid->GetPos() + m_upVec * 100).VGet(), 0xff0000);

	//1フレーム前の上ベクトルの表示(暗赤)
	DrawLine3D(m_rigid->GetPos().VGet(), Vec3(m_rigid->GetPos() + m_postUpVec * 100).VGet(), 0xaa0000);

	//進行方向ベクトルのデバッグ表示(黄色)
	DrawLine3D(m_rigid->GetPos().VGet(), Vec3(m_rigid->GetPos() + m_moveDir * 100).VGet(), 0xffff00);

	//右側ベクトルのデバッグ表示(緑)
	DrawLine3D(m_rigid->GetPos().VGet(), Vec3(m_rigid->GetPos() + m_sideVec * 100).VGet(), 0x00ff00);

#endif 
	m_postUpVec = m_upVec;//上方向ベクトルを前のフレームの上方向ベクトルにする

	//MV1SetRotationMatrix(m_modelHandle, rotatemat);//回転行列を反映
	MV1SetPosition(m_modelHandle, m_rigid->GetPos().VGet());

	MV1SetRotationZYAxis(m_modelHandle, (m_moveDir * -1).VGet(), m_upVec.GetNormalized().VGet(), 0);

	//当たり判定の更新
	m_headCol->SetShiftPosNum(m_upVec * (m_footCol->GetRadius() * 2 + m_bodyCol->GetRadius() * 2 + m_headCol->GetRadius()));
	m_bodyCol->SetShiftPosNum(m_upVec * (m_footCol->GetRadius() * 2 + m_bodyCol->GetRadius()));
	m_footCol->SetShiftPosNum(m_upVec * m_footCol->GetRadius());
	//m_spinCol->SetShiftPosNum(m_upVec * (m_footCol->GetRadius()*2+m_bodyCol->GetRadius()));
	m_lookPoint = m_rigid->GetPos() + m_upVec * 10;


}

void Player::Draw()
{
	if (m_visibleCount % 3 == 0 || m_visibleCount % 2 == 0)
	{
		MV1DrawModel(m_modelHandle);
	}
	//MV1DrawModel(m_modelHandle);
	/*m_headCol->DebugDraw(m_rigid->GetPos());
	m_bodyCol->DebugDraw(m_rigid->GetPos());
	m_footCol->DebugDraw(m_rigid->GetPos());*/
	m_spinCol->DebugDraw(m_rigid->GetPos());

#if DEBUG
	//DrawLine3D(m_rigid->GetPos().VGet(), Vec3(m_rigid->GetPos() + m_shotDir * 100).VGet(), 0x0000ff);
	Vec3 axis = Cross(m_upVec, m_moveDir);//上方向ベクトルと進行方向ベクトルの外積から回転軸を生成
	axis.Normalize();//単位ベクトル化

#ifdef DEBUG
	//回転軸のデバッグ表示(紫)
	//DrawLine3D(m_rigid->GetPos().VGet(), Vec3(m_rigid->GetPos() + RotateYAxis * 100).VGet(), 0xff0000);

	//回転軸のデバッグ表示(紫)
	DrawLine3D(m_rigid->GetPos().VGet(), Vec3(m_rigid->GetPos() + axis * 100).VGet(), 0xff00ff);

	//上方向ベクトルのデバッグ表示(赤)
	DrawLine3D(m_rigid->GetPos().VGet(), Vec3(m_rigid->GetPos() + m_upVec * 100).VGet(), 0xff0000);

	//1フレーム前の上ベクトルの表示(暗赤)
	DrawLine3D(m_rigid->GetPos().VGet(), Vec3(m_rigid->GetPos() + m_postUpVec * 100).VGet(), 0xaa0000);

	//進行方向ベクトルのデバッグ表示(黄色)
	DrawLine3D(m_rigid->GetPos().VGet(), Vec3(m_rigid->GetPos() + m_moveDir * 100).VGet(), 0xffff00);

	//右側ベクトルのデバッグ表示(緑)
	//DrawLine3D(m_rigid->GetPos().VGet(), Vec3(m_rigid->GetPos() + m_sideVec * 100).VGet(), 0x00ff00);

#endif 

#endif

}

void Player::SetCameraToPlayer(Vec3 cameraToPlayer)
{
	m_cameraToPlayer = cameraToPlayer;
}

void Player::SetBoost(Vec3 sideVec)
{
	m_sideVec = sideVec * -1;
	m_isBoostFlag = true;
	ChangeAnim(AnimNum::AnimationNumFall);
}

void Player::SetIsOperation(bool flag)
{
	SetVelocity(Vec3::Zero());
	if (flag)
	{
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumRolling);
		SetAntiGravity();
		m_playerUpdate = &Player::OperationUpdate;
		//ChangeAnim(kAnimationNumFall);
		m_isOperationFlag = true;
	}
	else
	{
		SetAntiGravity(false);
		ChangeAnim(AnimNum::AnimationNumIdle);
		m_isOperationFlag = false;
	}
}
void Player::SetCameraAngle(float cameraAngle)
{
	m_cameraAngle = cameraAngle;
}

void Player::OnCollideEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
	printf("CollideEnter");


	if (colider->GetTag() == ObjectTag::Stage)
	{
		printf("Stage\n");
		m_spinCount = 0;
		m_isOperationFlag = false;
		m_isJumpFlag = false;
		if (m_playerUpdate == &Player::DropAttackUpdate)
		{

			PlaySoundMem(SoundManager::GetInstance().GetSoundData(kJumpDropGroundSEName), DX_PLAYTYPE_BACK);
		}
		if (m_playerUpdate == &Player::BoostUpdate)
		{
			m_postState = m_state;
			ChangeAnim(AnimNum::AnimationNumIdle);
			m_playerUpdate = &Player::JumpingUpdate;
		}
	}
	if (colider->GetTag() == ObjectTag::Crystal)
	{
		printf("Crystal\n");
		m_spinCount = 0;
		m_isJumpFlag = false;
		m_isBoostFlag = false;

	}
	if (colider->GetTag() == ObjectTag::Kuribo)
	{
		printf("Kuribo\n");
		if (m_isSpinFlag)
		{
			auto kuribo = std::dynamic_pointer_cast<Kuribo>(colider);
			kuribo->Stan(60);
			PlaySoundMem(m_parrySEHandle, DX_PLAYTYPE_BACK);
		}

		if (ownTag == ColideTag::Body)
		{
			if (m_isVisibleFlag)return;
			m_postState = m_state;
			//HPを減らす
			PlaySoundMem(m_hitSEHandle, DX_PLAYTYPE_BACK);
			//ノックバック
			Vec3 enemyAttackDir = m_rigid->GetPos() - colider->GetRigidbody()->GetPos();
			enemyAttackDir.Normalize();
			m_rigid->SetVelocity(enemyAttackDir * 2);
			StartJoypadVibration(DX_INPUT_PAD1, 300, 600);
			m_prevUpdate = m_playerUpdate;
			m_playerUpdate = &Player::DamegeUpdate;
			m_hp -= 10;
			m_isOnDamageFlag = true;
			m_isVisibleFlag = true;
			m_damageFrame = kDamageFrameMax;
			ChangeAnim(AnimNum::AnimationNumHit);
		}
		else if (ownTag == ColideTag::Foot)
		{
			CommandJump();
		}
	}
	if (colider->GetTag() == ObjectTag::Takobo)
	{
		printf("Takobo\n");
		//スピンを当てたら
		if (m_isSpinFlag)
		{
			//ぶっ飛ばす
			PlaySoundMem(m_parrySEHandle, DX_PLAYTYPE_BACK);
			colider->GetRigidbody()->SetVelocity(Vec3(m_rigid->GetPos() - colider->GetRigidbody()->GetPos()).GetNormalized() * -3);

		}
		else
		{
			if (m_isVisibleFlag)return;
			PlaySoundMem(m_hitSEHandle, DX_PLAYTYPE_BACK);
			StartJoypadVibration(DX_INPUT_PAD1, 600, 600);
			m_hp -= 10;
			m_prevUpdate = m_playerUpdate;
			m_playerUpdate = &Player::DamegeUpdate;
			m_rigid->AddVelocity(Vec3(m_rigid->GetPos() - colider->GetRigidbody()->GetPos()).GetNormalized() * 3);
			m_isOnDamageFlag = true;

			m_isVisibleFlag = true;
			m_damageFrame = kDamageFrameMax;

			ChangeAnim(AnimNum::AnimationNumHit);
		}
	}
	if (colider->GetTag() == ObjectTag::Gorori)
	{
		printf("Gorori\n");
		if (m_isSpinFlag)
		{
			PlaySoundMem(m_parrySEHandle, DX_PLAYTYPE_BACK);
			auto gorori = dynamic_pointer_cast<Gorori>(colider);

			gorori->SetAttackDir(Vec3(gorori->GetRigidbody()->GetPos() - m_rigid->GetPos()).GetNormalized());
		}
		else
		{
			if (m_isVisibleFlag)return;
			PlaySoundMem(m_hitSEHandle, DX_PLAYTYPE_BACK);
			StartJoypadVibration(DX_INPUT_PAD1, 600, 600);
			m_hp -= 10;
			m_prevUpdate = m_playerUpdate;
			m_playerUpdate = &Player::DamegeUpdate;
			m_rigid->AddVelocity(Vec3(m_rigid->GetPos() - colider->GetRigidbody()->GetPos()).GetNormalized() * 4);
			m_isOnDamageFlag = true;

			m_isVisibleFlag = true;
			m_damageFrame = kDamageFrameMax;

			ChangeAnim(AnimNum::AnimationNumHit);
		}
	}
	if (colider->GetTag() == ObjectTag::EnemyAttack)
	{
		printf("EnemyAttack\n");
		if (m_isSpinFlag)
		{
			PlaySoundMem(m_parrySEHandle, DX_PLAYTYPE_BACK);
			auto attackSphere = dynamic_pointer_cast<EnemySphere>(colider);
			attackSphere->SetVelocity(attackSphere->GetVelocity() * -1);
			attackSphere->SetCounterFlag();
		}
		else
		{
			if (m_isVisibleFlag)return;
			m_postState = m_state;
			PlaySoundMem(m_hitSEHandle, DX_PLAYTYPE_BACK);
			colider->GetRigidbody()->AddVelocity((colider->GetRigidbody()->GetVelocity()) * -1);
			StartJoypadVibration(DX_INPUT_PAD1, 300, 600);
			m_prevUpdate = m_playerUpdate;
			m_playerUpdate = &Player::DamegeUpdate;
			m_hp -= 10;
			m_isOnDamageFlag = true;
			
			m_isVisibleFlag = true;
			m_damageFrame = kDamageFrameMax;
			ChangeAnim(AnimNum::AnimationNumHit);
		}
	}
	if (colider->GetTag() == ObjectTag::Electronic)
	{
		printf("Electro\n");

		if (m_isVisibleFlag)return;
		m_postState = m_state;
		PlaySoundMem(m_elecSEHandle, DX_PLAYTYPE_BACK);
		colider->GetRigidbody()->AddVelocity((colider->GetRigidbody()->GetVelocity()) * -1);
		StartJoypadVibration(DX_INPUT_PAD1, 300, 600);
		m_prevUpdate = m_playerUpdate;
		m_playerUpdate = &Player::DamegeUpdate;
		m_hp -= 10;
		m_isOnDamageFlag = true;

		m_isVisibleFlag = true;
		m_damageFrame = kDamageFrameMax;
		ChangeAnim(AnimNum::AnimationNumHit);

	}

	if (colider->GetTag() == ObjectTag::Boss)
	{
		if (!(m_state == State::Spin))
		{
			if (colider->GetState() == State::Running || colider->GetState() == State::Tackle)
			{
				if (m_isVisibleFlag)return;
				m_postState = m_state;
				Vec3 dir = m_rigid->GetPos() - colider->GetRigidbody()->GetPos();
				dir.Normalize();
				dir = dir + m_upVec*2;
				dir.Normalize();
				m_rigid->AddVelocity(dir * 3);
				StartJoypadVibration(DX_INPUT_PAD1, 300, 600);
				m_prevUpdate = m_playerUpdate;
				m_playerUpdate = &Player::DamegeUpdate;
				m_hp -= 20;
				m_isOnDamageFlag = true;

				m_isVisibleFlag = true;
				m_damageFrame = kDamageFrameMax;
				ChangeAnim(AnimNum::AnimationNumHit);
			}
		}
	}
	
	if (colider->GetTag() == ObjectTag::ClearObject)
	{
		printf("ClearObject\n");
		m_isClearFlag = true;
	}
	if (m_hp <= 0)
	{
		m_hp = 0;
		m_color = 0xff0000;
	}
}

void Player::OnCollideStay(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
}

void Player::OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
	printf("TriggerEnter\n");
	if (colider->GetTag() == ObjectTag::Stage)
	{
		if (m_playerUpdate == &Player::BoostUpdate)
		{
			ChangeAnim(AnimNum::AnimationNumIdle);
			m_playerUpdate = &Player::JumpingUpdate;
		}
		m_nowPlanet = std::dynamic_pointer_cast<Planet>(colider);

	}
	if (colider->GetTag() == ObjectTag::EnemyAttack)
	{
		if (m_playerUpdate == &Player::DamegeUpdate)return;
		m_postState = m_state;
		PlaySoundMem(m_hitSEHandle, DX_PLAYTYPE_BACK);
		colider->GetRigidbody()->AddVelocity((colider->GetRigidbody()->GetVelocity()) * -1);
		StartJoypadVibration(DX_INPUT_PAD1, 300, 600);
		m_prevUpdate = m_playerUpdate;
		m_playerUpdate = &Player::DamegeUpdate;
		m_hp -= 10;
		m_isOnDamageFlag = true;
		m_damageFrame = kDamageFrameMax;
		ChangeAnim(AnimNum::AnimationNumHit);
	}
	if (colider->GetTag() == ObjectTag::Electronic)
	{
		printf("Electro\n");
		m_postState = m_state;
		if (m_isVisibleFlag)return;
		PlaySoundMem(m_elecSEHandle, DX_PLAYTYPE_BACK);
		colider->GetRigidbody()->AddVelocity((colider->GetRigidbody()->GetVelocity()) * -1);
		StartJoypadVibration(DX_INPUT_PAD1, 300, 600);
		m_prevUpdate = m_playerUpdate;
		m_playerUpdate = &Player::DamegeUpdate;
		m_hp -= 10;
		m_isOnDamageFlag = true;

		m_isVisibleFlag = true;
		m_damageFrame = kDamageFrameMax;
		ChangeAnim(AnimNum::AnimationNumHit);

	}
	if (colider->GetTag() == ObjectTag::StickStarItem)
	{
		printf("StickStarItem\n");
		m_shotUpdate = &Player::ShotTheStickStar;

		PlaySoundMem(m_powerUpItemGetSEHandle, DX_PLAYTYPE_BACK);

	}
	if (colider->GetTag() == ObjectTag::FullPowerDropItem)
	{
		printf("FullPowerDropItem\n");
		m_dropAttackUpdate = &Player::FullPowerDropAttackUpdate;
		PlaySoundMem(m_powerUpItemGetSEHandle, DX_PLAYTYPE_BACK);
	}
	if (colider->GetTag() == ObjectTag::Coin)
	{
		printf("Coin\n");
		m_hp += 10;
		m_coinCount++;
		if (m_hp > kPlayerHPMax)
		{
			m_hp = kPlayerHPMax;
		}
	}
	if (colider->GetTag() == ObjectTag::FragmentOfStar)
	{

		PlaySoundMem(m_specialItemGetSEHandle, DX_PLAYTYPE_BACK);
		m_fragmentCount++;
		if (m_fragmentCount >= 5)
		{
			//スターシェードを5つ集めたらブースター生成
			auto obj = GalaxyCreater::GetInstance().GetCollidable(3);
			obj->SetIsActive(true);
		}
	}
	if (colider->GetTag() == ObjectTag::ClearObject)
	{
		m_isClearFlag = true;
	}
}

void Player::OnTriggerStay(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
	
}

Vec3 Player::GetCameraToPlayer() const
{
	return m_cameraToPlayer;
}

void Player::InitDush()
{
	ChangeAnim(AnimNum::AnimationNumRun, kDashMag);
	m_playerUpdate = &Player::DashUpdate;
}

void Player::InitJump()
{
	ChangeAnim(AnimNum::AnimationNumJump);
	m_isJumpFlag = true;
	m_playerUpdate = &Player::JumpingUpdate;
}

void Player::Landing(int recast)
{
	ChangeAnim(AnimNum::AnimationNumJumpAttack);
	//57:着地アニメーションの終了時間
	MV1SetAttachAnimTime(m_modelHandle, m_currentAnimNo, static_cast<float>(57 - recast));
	m_playerUpdate = &Player::LandingUpdate;
}

bool Player::UpdateAnim(int attachNo)
{
	//アニメーションが設定されていないので終了
	if (attachNo == -1) return false;

	//アニメーションを進行させる
	float now = MV1GetAttachAnimTime(m_modelHandle, attachNo);//現在の再生カウント
	now += kAnimFrameSpeed * m_speed / kFrameParSecond;//アニメーションカウントを進める


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

void Player::ChangeAnim(int animIndex, float speed)
{
	m_speed = speed;
	//さらに古いアニメーションがアタッチされている場合はこの時点で削除しておく
	if (m_prevAnimNo != -1)
	{
		MV1DetachAnim(m_modelHandle, m_prevAnimNo);
	}

	int nextAnimNo = MV1AttachAnim(m_modelHandle, animIndex, -1, false);

	if (m_currentAnimNo != nextAnimNo)
	{
		//現在再生中の待機アニメーションは変更前のアニメーション扱いに
		m_prevAnimNo = m_currentAnimNo;

	}

	//変更後のアニメーションとして攻撃アニメーションを改めて設定する
	m_currentAnimNo = nextAnimNo;

	//切り替えの瞬間は変更前のアニメーションが再生される状態にする
	m_animBlendRate = 0.0f;

	//変更前のアニメーション100%
	DxLib::MV1SetAttachAnimBlendRate(m_modelHandle, m_prevAnimNo, 1.0f - m_animBlendRate);
	//変更後のアニメーション0%
	DxLib::MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAnimNo, m_animBlendRate);
}

void Player::StartUpdate()
{
	m_stateName = "Start";
	m_postState = m_state;
	m_state = State::Intro;
	m_rigid->SetVelocity(Vec3::Zero());
	Landing();
	m_regeneRange += 0.01f;
	if (m_regeneRange > 2)
	{

	}
}

void Player::NeutralUpdate()
{
	m_stateName = "Neutral";
	m_state = State::Neutral;

	//アナログスティックを使って移動

	Vec3 move = Move();

	if (std::abs(move.Length()) >= 0.2f * kMaxSpeed)
	{
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumRun);
		m_playerUpdate = &Player::WalkingUpdate;
	}
	//プレイヤーの最大移動速度は0.01f/frame
	if (Pad::IsTrigger(PAD_INPUT_1))//XBoxのAボタン
	{
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumJump);
		m_isJumpFlag = true;
		move += m_upVec.GetNormalized() * kJumpPower;
		m_playerUpdate = &Player::JumpingUpdate;
	}
	if (Pad::IsTrigger(PAD_INPUT_B))//XBoxの
	{
		ChangeAnim(AnimNum::AnimationNumSpin, 5.f);
		m_postState = m_state;
		m_playerUpdate = &Player::SpinActionUpdate;
	}

	m_rigid->AddVelocity(move);

	if (m_playerUpdate != &Player::BoostUpdate && !m_footCol->OnHit())
	{
		m_postState = m_state;
		m_playerUpdate = &Player::JumpingUpdate;
	}
}

void Player::WalkingUpdate()
{
	m_stateName = "Walking";

	m_state = State::Walk;
	Vec3 ans;

	ans = Move();

	if (std::abs(ans.Length()) < 0.2f * kMaxSpeed)
	{
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumIdle);
		m_playerUpdate = &Player::NeutralUpdate;
	}

	if ((Pad::IsPress(PAD_INPUT_Z)))
	{
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumRun, kDashMag);
		m_playerUpdate = &Player::DashUpdate;
	}

	if (Pad::IsTrigger(PAD_INPUT_1))//XBoxのAボタン
	{
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumJump);
		m_isJumpFlag = true;
		ans += m_upVec.GetNormalized() * kJumpPower;
		m_playerUpdate = &Player::JumpingUpdate;
	}
	if (Pad::IsTrigger(PAD_INPUT_B))//XBoxの
	{
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumSpin, 5.f);
		m_playerUpdate = &Player::SpinActionUpdate;
	}

	m_rigid->AddVelocity(ans);

	/*if (m_playerUpdate != &Player::BoostUpdate && !m_footCol->OnHit())
	{
		m_playerUpdate = &Player::JumpingUpdate;
	}*/
	if (!m_footCol->OnHit())
	{
		m_postState = m_state;
		m_playerUpdate = &Player::JumpingUpdate;
	}
}

void Player::DashUpdate()
{
	m_cameraEasingSpeed = 30.f;
	m_stateName = "Dash";
	m_state = State::Dash;
	Vec3 ans;

	ans = Move();
	if ((Pad::IsRelase(PAD_INPUT_Z)))
	{
		m_postState = m_state;
		m_cameraEasingSpeed = 15.f;
		ChangeAnim(AnimNum::AnimationNumRun);
		m_playerUpdate = &Player::WalkingUpdate;
	}


	if (std::abs(ans.Length()) < 0.2f * kMaxSpeed)
	{
		m_postState = m_state;
		m_cameraEasingSpeed = 15.f;
		ChangeAnim(AnimNum::AnimationNumIdle);
		m_playerUpdate = &Player::NeutralUpdate;
	}


	if (Pad::IsTrigger(PAD_INPUT_1))//XBoxのAボタン
	{
		m_postState = m_state;
		m_cameraEasingSpeed = 15.f;
		ChangeAnim(AnimNum::AnimationNumJump);
		m_isJumpFlag = true;
		ans += m_upVec.GetNormalized() * kJumpPower;
		m_playerUpdate = &Player::DashJumpUpdate;
	}
	if (Pad::IsTrigger(PAD_INPUT_B))//XBoxの
	{
		m_postState = m_state;
		m_cameraEasingSpeed = 15.f;
		ChangeAnim(AnimNum::AnimationNumSpin, 5.f);
		m_playerUpdate = &Player::SpinActionUpdate;
	}

	m_rigid->AddVelocity(ans * kDashMag);

	/*if (m_playerUpdate != &Player::BoostUpdate && !m_footCol->OnHit())
	{
		m_playerUpdate = &Player::JumpingUpdate;
	}*/

	if (!m_footCol->OnHit())
	{
		m_postState = m_state;
		m_playerUpdate = &Player::JumpingUpdate;
	}
}

void Player::JumpingUpdate()
{
	m_stateName = "Jumping";
	m_state = State::Jump;
	if (Pad::IsTrigger(PAD_INPUT_1))//XBoxのAボタン
	{
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumJumpAttack);
		m_rigid->SetVelocity(Vec3::Zero());
		m_playerUpdate = &Player::DropAttackUpdate;
	}
	if (Pad::IsTrigger(PAD_INPUT_B))//XBoxの
	{
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumSpin, 5.f);
		/*if (m_spinCount >= 1)return;
		m_isSpinFlag = true;
		m_spinCount++;*/
		m_rigid->AddVelocity(m_frontVec * kJumpPower);
		m_playerUpdate = &Player::JumpActionUpdate;
	}


	if (m_footCol->OnHit())
	{
		m_postState = m_state;
		m_moveDir = Cross(GetCameraRightVector(), m_upVec);
		ChangeAnim(AnimNum::AnimationNumIdle);
		m_playerUpdate = &Player::NeutralUpdate;
	}
}

void Player::DashJumpUpdate()
{
	m_stateName = "DashJumping";
	m_state = State::DashJump;
	if (Pad::IsTrigger(PAD_INPUT_1))//XBoxのAボタン
	{
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumJumpAttack);
		m_rigid->SetVelocity(Vec3::Zero());
		m_playerUpdate = &Player::DropAttackUpdate;
	}
	if (Pad::IsTrigger(PAD_INPUT_B))//XBoxの
	{
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumSpin, 5.f);
		/*if (m_spinCount >= 1)return;
		m_isSpinFlag = true;
		m_spinCount++;*/
		m_rigid->AddVelocity(m_frontVec * kJumpPower);
		m_playerUpdate = &Player::JumpActionUpdate;
	}


	if (m_footCol->OnHit())
	{
		m_postState = m_state;
		m_moveDir = Cross(GetCameraRightVector(), m_upVec);
		ChangeAnim(AnimNum::AnimationNumIdle);
		m_playerUpdate = &Player::NeutralUpdate;
	}
}

void Player::JumpActionUpdate()
{
	(this->*m_jumpActionUpdate)();
}

void Player::JumpBoostUpdate()
{
	m_stateName = "JumpBoost";
	m_state = State::JumpBoost;
	if (Pad::IsTrigger(PAD_INPUT_1))//XBoxのAボタン
	{
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumJumpAttack);
		m_rigid->SetVelocity(Vec3::Zero());
		m_playerUpdate = &Player::DropAttackUpdate;
	}
	if (m_footCol->OnHit())
	{
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumIdle);
		m_playerUpdate = &Player::NeutralUpdate;
	}
}


void Player::DropAttackUpdate()
{
	//落下攻撃の更新
	(this->*m_dropAttackUpdate)();
}

void Player::NormalDropAttackUpdate()
{
	m_stateName = "DropAttack";

	m_state = State::JumpDrop;
	float now = MV1GetAttachAnimTime(m_modelHandle, m_currentAnimNo);//現在のアニメーション再生カウント
	m_rigid->AddVelocity(m_upVec * -0.8f);

	if (now < 16)
	{
		m_angle += (DX_PI_F * 2) / 16;
		m_rigid->SetVelocity(Vec3::Zero());
	}
	if (m_footCol->OnHit())
	{
		m_postState = m_state;
		m_landingStanFrame = 20;

		Landing();
	}
}

void Player::FullPowerDropAttackUpdate()
{
	m_stateName = "FullPowerDrop";

	m_state = State::FullpowerJumpDrop;
	float now = MV1GetAttachAnimTime(m_modelHandle, m_currentAnimNo);//現在の再生カウント
	m_rigid->AddVelocity(m_upVec * -0.8f);
	if (Pad::IsPress(PAD_INPUT_1))
	{
		if (m_fullPowerChargeCount < 3)m_fullPowerChargeCount += 0.1f;

		m_angle += (DX_PI_F * 2) / 16;
		m_rigid->SetVelocity(Vec3::Zero());
		if (now < 16)
		{
			MV1SetAttachAnimTime(m_modelHandle, m_currentAnimNo, 0);
			m_angle += (DX_PI_F * 2) / 16;
			m_rigid->SetVelocity(Vec3::Zero());
		}
	}

	if (m_footCol->OnHit())
	{
		m_postState = m_state;
		//ここで衝撃波を発生させる予定
		m_impacts.push_back(std::make_shared<StampImpact>(m_rigid->GetPos() + m_upVec * -m_footCol->radius, m_nowPlanet->GetScale(), m_upVec * -1, ObjectTag::PlayerImpact, m_fullPowerChargeCount));
		MyEngine::Physics::GetInstance().Entry(m_impacts.back());
		m_fullPowerChargeCount = 0;
		Landing(57);
		m_landingStanFrame = 60;
	}
}

void Player::LandingUpdate()
{
	m_stateName = "Landing";
	m_state = State::Land;
	m_landingStanFrame--;

	if (m_landingStanFrame < 0)
	{
		if (MV1GetAttachAnimTotalTime(m_modelHandle, m_currentAnimNo) != 57)
		{
			m_postState = m_state;
			ChangeAnim(AnimNum::AnimationNumIdle);
			m_playerUpdate = &Player::NeutralUpdate;
		}
		float Time = MV1GetAttachAnimTime(m_modelHandle, m_currentAnimNo);
		if (Time >= 40.f)
		{
			m_postState = m_state;
			ChangeAnim(AnimNum::AnimationNumIdle);
			m_playerUpdate = &Player::NeutralUpdate;

		}

	}
	else
	{
		if (MV1GetAttachAnimTime(m_modelHandle, m_currentAnimNo) >= 40)
		{
			MV1SetAttachAnimTime(m_modelHandle, m_currentAnimNo, 40);
		}
	}
}

void Player::AimingUpdate()
{
	m_stateName = "Aiming";


	Vec3 move;
	move = Move();
	//プレイヤーの最大移動速度は0.01f/frame
	if (Pad::IsTrigger(PAD_INPUT_1))//XBoxのAボタン
	{
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumJump);
		m_isJumpFlag = true;
		move += m_upVec.GetNormalized() * kJumpPower;
		ChangeAnim(AnimNum::AnimationNumJump);
		m_playerUpdate = &Player::JumpingUpdate;
	}
	if (Pad::IsTrigger(PAD_INPUT_B))//XBoxの
	{
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumSpin);

		ChangeAnim(AnimNum::AnimationNumSpin);
		m_playerUpdate = &Player::SpinActionUpdate;
	}
	if (Pad::IsTrigger(PAD_INPUT_Y))
	{
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumIdle);
		m_playerUpdate = &Player::NeutralUpdate;
	}

	m_rigid->AddVelocity(move);

}

void Player::SpinActionUpdate()
{
	m_isSpinFlag = true;

	(this->*m_spinAttackUpdate)();
}

void Player::SpiningUpdate()
{
	m_radius = kNeutralSpinRadius;
	m_stateName = "Spining";

	m_state = State::Spin;
	Vec3 move;

	move = Move();

	m_rigid->AddVelocity(move);

	m_spinAngle += DX_PI_F / 15;

	if (m_spinAngle >= DX_PI_F * 2)
	{
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumIdle);
		m_isSpinFlag = false;
		m_playerUpdate = &Player::NeutralUpdate;
		m_spinAngle = 0;
	}
}

void Player::RollingAttackUpdate()
{
	m_stateName = "RollingAttack";
	m_state = State::Roll;
	m_frontVec = Cross(m_sideVec, m_upVec);
	m_rigid->SetVelocity(m_frontVec);
	if (Pad::IsTrigger(PAD_INPUT_B))//XBoxの
	{
		m_postState = m_state;
		m_cameraEasingSpeed = 15.f;
		ChangeAnim(AnimNum::AnimationNumIdle);
		m_isSpinFlag = false;
		m_playerUpdate = &Player::NeutralUpdate;
	}
}

void Player::JumpingSpinUpdate()
{
	m_radius = kNeutralSpinRadius;
	m_stateName = "JumpSpin";

	m_state = State::Spin;
	//アナログスティックを使って移動

	Vec3 move;
	move = Move();

	m_rigid->SetVelocity(move);

	m_spinAngle += DX_PI_F / 15;
	m_angle += DX_PI_F / 15;
	m_spinAngle += DX_PI_F / 15;
	m_angle += DX_PI_F / 15;
	if (m_spinAngle >= DX_PI_F * 2)
	{
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumJump);
		m_isSpinFlag = false;
		m_playerUpdate = &Player::JumpingUpdate;
		m_spinAngle = 0;
	}
}

void Player::CommandJump()
{
	ChangeAnim(AnimNum::AnimationNumJump);
	m_isJumpFlag = true;
	m_rigid->AddVelocity(m_upVec.GetNormalized() * kJumpPower);
	m_playerUpdate = &Player::JumpingUpdate;
}

void Player::BoostUpdate()
{
	m_stateName = "BoostingGolira";

	m_state = State::Boosting;
	m_frontVec = Cross(m_upVec, m_sideVec);
	m_moveDir = m_frontVec;
	if (!m_isBoostFlag)
	{
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumIdle);
		m_playerUpdate = &Player::NeutralUpdate;
	}
}

void Player::OperationUpdate()
{
	m_rigid->SetVelocity(m_velocity);
	m_stateName = "NowControl";

	m_state = State::Operation;
	m_moveDir = m_rigid->GetPos() - m_postPos;
	m_moveDir.Normalize();
	m_postPos = m_rigid->GetPos();
	m_sideVec = Cross(m_upVec, m_moveDir);
	m_upVec = Cross(m_moveDir, m_sideVec);
	if (!m_isOperationFlag)
	{
		m_postState = m_state;
		SetAntiGravity(false);
		ChangeAnim(AnimNum::AnimationNumIdle);
		m_playerUpdate = &Player::JumpingUpdate;
	}
}

Vec3 Player::Move()
{
	int analogX = 0, analogY = 0;
	GetJoypadAnalogInput(&analogX, &analogY, DX_INPUT_PAD1);
	analogY = -analogY;

	m_inputVec.x = static_cast<float>(analogX);
	m_inputVec.z = static_cast<float>(-analogY);

	Vec3 ans;  // 初期化はそのままに
	Vec3 modelDir;
	Vec3 front;
	// アナログスティックの入力を反映
	m_sideVec = GetCameraRightVector();
	front = Cross(m_sideVec, m_upVec).GetNormalized();
	m_sideVec = Cross(m_upVec, m_frontVec).GetNormalized();
	ans = front * static_cast<float>(analogY);
	ans += m_sideVec * static_cast<float>(analogX);

	modelDir = front * static_cast<float>(analogY);
	modelDir -= m_sideVec * static_cast<float>(analogX);

	if (ans.Length() > 0)
	{
		m_moveDir = ans.GetNormalized();
	}
	// 無効範囲の確認
	float len = ans.Length();
	if (len < kAnalogRangeMin) {
		return Vec3(0, 0, 0);  // 無効な場合はゼロベクトルを返す
	}

	if (ans.Length() > 0)
	{
		m_moveDir = ans.GetNormalized();
	}
	m_frontVec = front;
	m_inputVec.Normalize();
	ans = ans.GetNormalized() * kMaxSpeed; // 正規化し、速度を掛ける
	return ans;
}

void Player::ShotTheStar()
{
	PlaySoundMem(m_shotTheStarSEHandle, DX_PLAYTYPE_BACK);
	Vec3 shotPos = MV1GetFramePosition(m_modelHandle, m_handFrameIndex);
	m_sphere.push_back(std::make_shared<PlayerSphere>(Priority::Low, ObjectTag::PlayerBullet, shared_from_this(), shotPos, m_shotDir, m_sideVec, 1, 0xff0000));
	MyEngine::Physics::GetInstance().Entry(m_sphere.back());
	m_sphere.back()->Init();
}

void Player::ShotTheStickStar()
{
	if (m_sphere.size() == 0)
	{
		PlaySoundMem(m_shotStickStarSEHandle, DX_PLAYTYPE_BACK);
		Vec3 shotPos = MV1GetFramePosition(m_modelHandle, m_handFrameIndex);
		m_sphere.push_back(std::make_shared<PlayerStickSphere>(Priority::Low, ObjectTag::PlayerBullet, shared_from_this(), shotPos, m_shotDir, m_sideVec, 1, 0xff0000));
		MyEngine::Physics::GetInstance().Entry(m_sphere.back());
		m_sphere.back()->Init();
	}
	else
	{
		m_sphere.back()->Effect();
	}
}

void Player::DamegeUpdate()
{
	m_stateName = "Damege";
	m_state=State::Damage;
	m_rigid->SetVelocity(m_rigid->GetVelocity());
	if (m_rigid->GetVelocity().Length() < 7.0f)
	{
		if (m_prevUpdate != m_playerUpdate)
		{
			ChangeAnim(AnimNum::AnimationNumRun);
			if (m_prevUpdate == &Player::TalkingUpdate)
			{
				m_playerUpdate = &Player::NeutralUpdate;
			}
			else
			{
				//ダメージアニメーションのみ
				m_playerUpdate = m_prevUpdate;
			}
			
		}
		else
		{
			m_postState = m_state;
			ChangeAnim(AnimNum::AnimationNumIdle);
			m_playerUpdate = &Player::NeutralUpdate;
			m_prevUpdate = m_playerUpdate;
		}
	}
}
void Player::AvoidUpdate()
{
	m_actionFrame++;

	if (m_actionFrame > kAvoidFrame)
	{
		m_actionFrame = 0;
		m_radius = kNeutralRadius;
		ChangeAnim(AnimNum::AnimationNumIdle);
		m_playerUpdate = &Player::NeutralUpdate;
	}
}

void Player::DeathUpdate()
{
	m_stateName = "Death";
	m_state = State::Death;

	float animTime = MV1GetAttachAnimTime(m_modelHandle, m_currentAnimNo);
	float totalTime = MV1GetAttachAnimTotalTime(m_modelHandle, m_currentAnimNo);
	
	if (animTime >= totalTime-2)
	{
		UI::GetInstance().InText("ドレイク？");
		UI::GetInstance().InText("ドレイィィィィク！！！！！");

		m_isDeathFlag = true;
	}
}

void Player::SetShotDir()
{
	// 入力された左スティック方向
	int directX = 0, directY = 0;
	GetJoypadAnalogInputRight(&directX, &directY, DX_INPUT_PAD1);
	directY = -directY; // Y軸反転

	// 静的変数で入力時間を記録
	static float inputDuration = 0.0f; // 入力が続いた時間（秒）
	static const float deltaTime = 0.016f; // フレーム時間（例: 60FPS）

	// 左スティック方向をベクトルに変換
	Vec3 inputDir = m_sideVec * static_cast<float>(directX) * 0.001f;
	inputDir += m_upVec * static_cast<float>(directY) * 0.001f;

	// 入力された方向ベクトルの正規化と入力時間の管理
	if (inputDir.Length() > 1e-6f) {
		inputDir = inputDir.GetNormalized();
		inputDuration += deltaTime; // 入力がある間、時間を加算
	}
	else {
		inputDir = m_frontVec; // 初期値として前方ベクトルを使用
		inputDuration = 0.0f; // 入力がない場合、時間をリセット
		
	}

	// 入力の強度を計算（0～1の範囲）
	float inputStrength = std::clamp(
		sqrt(static_cast<float>(directX * directX + directY * directY)) * 0.001f, 0.0f, 1.0f);

	// m_frontVecとの角度を計算
	float dotProduct = std::clamp(Dot(m_frontVec, inputDir), -1.0f, 1.0f);
	float angle = acos(dotProduct); // 安全な角度計算

	// 60度（ラジアンで計算）を超えないように制限
	float maxAngle = 60.0f * (3.14159f / 180.0f); // 60度をラジアンに変換
	if (angle > maxAngle) {
		Vec3 clampedDir = m_frontVec * cos(maxAngle) + (inputDir - m_frontVec * dotProduct).GetNormalized() * sin(maxAngle);
		inputDir = clampedDir.GetNormalized();
	}

	// `m_shotDir`を入力方向に徐々に向かせる
	float lerpSpeed = 0.1f * inputStrength; // 入力強度に応じて補間速度を変化

	// 短時間入力時の速度調整
	lerpSpeed *= std::clamp(inputDuration / 0.5f, 0.0f, 1.0f); // 0.5秒以内の入力で速度を抑制

	m_shotDir = m_shotDir * (1.0f - lerpSpeed) + inputDir * lerpSpeed;
	if (!m_isAimFlag)
	{
		m_shotDir = m_frontVec;
		return;
	}
}

void Player::DeleteManage()
{
	auto result = remove_if(m_sphere.begin(), m_sphere.end(), [this](const auto& sphere)
		{
			bool isOut = sphere->IsDestroy() == true;
			if (isOut == true)
			{
				sphere->OnDestroy();
				MyEngine::Physics::GetInstance().Exit(sphere);
			}
			return isOut;
		});
	if (result != m_sphere.end())
	{
		int a = 0;
	}
	m_sphere.erase(result, m_sphere.end());
}

void Player::TalkingUpdate()
{
	m_stateName = "Talking";
	
	m_state = State::Talk;

	if (Pad::IsState("PlayerInput"))
	{
		m_postState = m_state;
		m_playerUpdate = m_postUpdate;
	}
}

void Player::MoveToTargetWithStickStar(Vec3 targetPos)
{
	if (m_titleUpdateNum == 1)
	{
		if (m_sphere.size() == 0)
		{
			Vec3 targetVec = (targetPos - m_rigid->GetPos()).GetNormalized();
			Vec3 shotPos = MV1GetFramePosition(m_modelHandle, m_handFrameIndex);
			m_sphere.push_back(std::make_shared<PlayerStickSphere>(Priority::Low, ObjectTag::PlayerBullet, shared_from_this(), shotPos, targetVec, m_sideVec, 1, 0xff0000));
			MyEngine::Physics::GetInstance().Entry(m_sphere.back());
			m_sphere.back()->Init();

			m_playerUpdate = &Player::NeutralUpdate;
		}
		else
		{
			auto colidFlag = m_sphere.back()->GetStickFlag();
			if (colidFlag)
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
			Vec3 shotPos = MV1GetFramePosition(m_modelHandle, m_handFrameIndex);
			m_sphere.push_back(std::make_shared<PlayerStickSphere>(Priority::Low, ObjectTag::PlayerBullet, shared_from_this(), shotPos, targetVec, m_sideVec, 1, 0xff0000));
			MyEngine::Physics::GetInstance().Entry(m_sphere.back());
			m_sphere.back()->Init();

			MV1SetScale(m_modelHandle, VGet(0.01f, 0.01f, 0.01f));
			m_moveDir = Cross(GetCameraRightVector(), m_upVec);
			ChangeAnim(AnimNum::AnimationNumIdle);
			m_playerUpdate = &Player::NeutralUpdate;
		}
		else
		{

			auto colidFlag = m_sphere.back()->GetStickFlag();
			if (colidFlag)
			{
				m_titleUpdateNum = 1;
				m_sphere.back()->Effect();

			}
		}
	}


}

