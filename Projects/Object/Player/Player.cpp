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
#include"Game.h"

///Collidableにpowerを持たせて衝突時はそのパワー分のダメージを受けるようにする(現在はPlayer側が直値を書き込んでいる)
///その他直値の解決

namespace
{
	constexpr float kNeutralRadius = 2.f;
	constexpr float kNeutralHeadRadius = 1.f;//通常時の当たり半径
	constexpr float kNeutralBodyRadius = 2.f;//通常時の当たり半径
	constexpr float kNeutralFootRadius = 1.f;//通常時の当たり半径
	constexpr float kNeutralSpinRadius = 3.f;//通常時の当たり半径
	constexpr float kCollisionRadiusGap = 0.2f;//当たり判定同士のギャップ

	constexpr int kPlayerHPMax = 50;

	constexpr int kDamageFrameMax = 20;
	//アニメーション番号
	constexpr int kIdleAnimIndex = 1;
	//constexpr int kIdleAnimIndex = 2;//待機テスト
	constexpr int kAttackAnimIndex = 30;

	constexpr int kLandingStanFrameNeutral = 20;

	constexpr float kPlayerModelScaleMag = 0.005f;

	constexpr float kAnimFrameSpeed = 30.0f;//アニメーション進行速度

	constexpr int kLandingAnimationTotalCount = 57;

	constexpr float kSpinAnimationSpeed = 5.f;

	//アニメーションの切り替えにかかるフレーム数
	constexpr float kAnimChangeFrame = 20.0f;
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;

	//死ぬモーションの初期速度
	constexpr float kDeathAnimationInitialSpeed = 0.1f;

	//1回の入力で溜まる蘇生ポイントの大きさ
	constexpr int kRevivalInputPower = 5;

	//アナログスティックによる移動関連
	constexpr float kMaxSpeed = 0.5f;//プレイヤーの最大速度
	constexpr float kAnalogRangeMin = 0.1f;//アナログスティックの入力判定範囲
	constexpr float kAnalogRangeMax = 0.8f;
	constexpr float kAnalogInputMax = 1000.0f;//アナログスティックから入力されるベクトルの最大値

	constexpr float kFrameParSecond = 60.0f;
	constexpr float kDashMag = 1.5f;

	constexpr float kOnNeutralCameraEasingSpeed = 15.f;
	constexpr float kOnDashCameraEasingSpeed = 30.f;

	constexpr int kVisibleCountMax = 100;

	constexpr int kAvoidFrame = 60;

	constexpr int kFullPowerChargeTimeMax = 3;
	constexpr float kFullPowerChargeSpeed = 0.1f;

	constexpr float kJumpPower = 2.f;

	//スティックの入力の歩きと判定する範囲
	constexpr float kWalkInputVecMag = 0.2f;

	//アングルが回転する速度
	constexpr float kAngleRotateSpeed = (DX_PI_F * 2) / 16;

	/// <summary>
	/// プレイヤーが帰ってこれなくなる長さ
	/// </summary>
	constexpr float kGalaxyLength = 3000.f;

	constexpr float kCamerLookPointHeight = 10.0f;


	constexpr int kSearchRemainTimeMax = 28;
	constexpr int kChargeRemainCoolTime = 10;

	constexpr int kOnDamageJoypadVibrationPower = 300;
	constexpr int kOnDamageJoypadVibrationTime = 600;

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
//ハンドルの初期化
m_modelHandle(-1),
m_hitSEHandle(-1),
m_elecSEHandle(-1),
m_parrySEHandle(-1),
m_getItemHandle(-1),
m_searchSEHandle(-1),
m_shotTheStarSEHandle(-1),
m_shotStickStarSEHandle(-1),
m_powerUpItemGetSEHandle(-1),
m_specialItemGetSEHandle(-1),

//ベクトルの初期設定
m_postUpVec(Vec3::Up()),
m_shotDir(Vec3::Front()),
m_moveDir(Vec3::Front()),
m_frontVec(Vec3::Front()),
m_inputVec(Vec3::Front() * -1),
m_postMoveDir(Vec3::Front()),

//ステートの初期化
m_prevUpdate(&Player::StartUpdate),
m_playerUpdate(&Player::StartUpdate),

//カウントの初期化
m_coinCount(0),
m_damageFrame(0),
m_regeneRange(0),
m_revivalCount(0),
m_visibleCount(0),
m_fragmentCount(0),
m_landingStanFrame(0),
m_fullPowerChargeCount(0),

//アニメーションフレームの初期化
m_animBlendRate(0),
m_prevAnimNo(0),
m_currentAnimNo(-1),

//フラグの初期化
m_isJump(false),
m_isDead(false),
m_isAiming(false),
m_isOperation(false),
m_isVisible(false),
m_isSearch(false),
m_isClear(false),

//プレイヤーステートプロパティの初期化
m_hp(kPlayerHPMax),
m_spinAngle(0),
m_currentOxygen(0)
{
	//ハンドルの取得
	m_modelHandle = ModelManager::GetInstance().GetModelData(kFileName);
	m_parrySEHandle = SoundManager::GetInstance().GetSoundData(kOnParrySEName);
	m_getItemHandle = SoundManager::GetInstance().GetSoundData(kGetItemSEName);
	m_powerUpItemGetSEHandle = SoundManager::GetInstance().GetSoundData(kPowerUpItemGetSEName);
	m_specialItemGetSEHandle = SoundManager::GetInstance().GetSoundData(kSpecialItemGetSEName);
	m_searchSEHandle = SoundManager::GetInstance().GetSoundData(kGetSearchSEName);
	m_hitSEHandle = SoundManager::GetInstance().GetSoundData(kGororiHitSEName);
	m_elecSEHandle = SoundManager::GetInstance().GetSoundData(kElectroSEName);
	m_shotStickStarSEHandle = SoundManager::GetInstance().GetSoundData(kShotStickSEName);
	m_shotTheStarSEHandle = SoundManager::GetInstance().GetSoundData(kShotTheStarSEName);

	m_postUpVec = m_upVec;//もしかしたらプレイヤーの初期上方向ベクトルが0,1,0ではないかもしれないので

	//初期位置設定
	m_rigid->SetPos(pos);

	//当たり判定の設定
	{
		AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Head);
		m_headCol = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
		m_headCol->radius = kNeutralHeadRadius - kCollisionRadiusGap;
	}
	{
		AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);
		m_bodyCol = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
		m_bodyCol->radius = kNeutralBodyRadius - kCollisionRadiusGap;
	}
	{
		AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Foot);
		m_footCol = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
		m_footCol->radius = kNeutralFootRadius - kCollisionRadiusGap;
	}

	{
		AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Spin);
		m_spinCol = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
	}

	//メンバ関数の初期設定
	m_shotUpdate = &Player::ShotTheStickStar;
	m_jumpActionUpdate = &Player::JumpingSpinUpdate;
	m_dropAttackUpdate = &Player::NormalDropAttackUpdate;
	m_spinAttackUpdate = &Player::SpiningUpdate;

	//カメラの追ってきてほしい速度の初期設定
	m_cameraEasingSpeed = 5.f;

	//プレイヤーの弾との当たり判定を無視
	AddThroughTag(ObjectTag::PlayerBullet);

	//プレイヤーモデルの拡大率の設定
	DxLib::MV1SetScale(m_modelHandle, VGet(kPlayerModelScaleMag, kPlayerModelScaleMag, kPlayerModelScaleMag));

	//モデルの回転などの設定
	SetMatrix();
	//モデルの左手のフレームインデックスの取得
	m_leftHandFrameIndex = MV1SearchFrame(m_modelHandle, "mixamorig:LeftHand");
}

Player::~Player()
{
}

void Player::Init()
{
	//エフェクトの再生
	EffectManager::GetInstance().PlayEffect(kStarEffectName, true, 0, shared_from_this());
	//ミッションを管理するクラスにプレイヤーのポインタを設定
	Mission::GetInstance().SetPlayer(std::dynamic_pointer_cast<Player>(shared_from_this()));
}

void Player::Update()
{

	m_isSearch = false;

	//コントローラーの入力状態がプレイヤー操作状態じゃなかったら
	if (!Pad::IsState("PlayerInput"))
	{
		//会話中状態に移行
		if (m_playerUpdate != &Player::TalkingUpdate)m_postUpdate = m_playerUpdate;
		m_playerUpdate = &Player::TalkingUpdate;
	}

	//状態関数の実行
	(this->*m_playerUpdate)();

	//LBが入力されたら
	if (Pad::IsTrigger(PAD_INPUT_Y))
	{
		//エイム中の時
		if (m_isAiming)
		{
			//エイム状態を解除する
			m_isAiming = false;
		}
		else
		{
			//弾の射撃方向をプレイヤーの正面ベクトルに設定して
			m_shotDir = m_frontVec;

			//エイム状態にする
			m_isAiming = true;
		}
	}

	//XBoxコントローラーでYボタンが入力されたら
	if ((Pad::IsTrigger(PAD_INPUT_4)))PlaySoundMem(m_searchSEHandle, DX_PLAYTYPE_BACK);

	//XBoxコントローラーでYボタンが入力されていたら
	if ((Pad::IsPress(PAD_INPUT_4)))
	{
		//デバッグ描画モード
		m_isSearch = true;
	}

	//弾の射撃方向を操作
	SetShotDir();

	//XBoxコントローラーでXボタンが入力されていたら
	if (Pad::IsTrigger(PAD_INPUT_3))
	{

		//Playerが惑星移動しているか
		bool isBoosting = m_state == State::Boosting;
		if (!isBoosting)m_rigid->SetVelocity(Vec3::Zero());

		(this->*m_shotUpdate)();

	}
	m_postShotVec = m_shotDir;

	//不要になった弾の削除関数
	DeleteManage();
	//不要になった衝撃波の削除関数
	DeleteObject(m_impacts);

	//無敵時間が上限を超えたら
	if (m_visibleCount > kVisibleCountMax)
	{
		m_isVisible = false;
		m_visibleCount = 0;
	}
	//無敵状態だったら
	if (m_isVisible)
	{
		m_visibleCount++;
	}

	//ダメージを受けていたら
	if (m_state==State::Damage)
	{
		m_damageFrame--;
		if (m_damageFrame < 0)
		{
			m_damageFrame = 0;
		}

	}

	//体力が残っていなかったら
	if (m_hp <= 0)
	{
		m_visibleCount = 0;
		m_isVisible = true;

		//死亡状態または会話状態でなかったら
		if (m_playerUpdate != &Player::DeathUpdate && m_playerUpdate != &Player::TalkingUpdate)
		{
			UI::GetInstance().SetTalkObjectHandle(UI::TalkGraphKind::TakasakiTaisa);
			UI::GetInstance().InText("Aボタンを連打して自分で心肺蘇生するんだ！");
			m_isAiming = false;
			//死亡するごとに志望アニメーションの進行速度を上げる
			ChangeAnim(AnimNum::AnimationNumDeath, kDeathAnimationInitialSpeed + m_revivalCount / 2);

			m_playerUpdate = &Player::DeathUpdate;
		}

	}

	UpdateAnim(m_currentAnimNo);
	//変更前のアニメーション100%
	DxLib::MV1SetAttachAnimBlendRate(m_modelHandle, m_prevAnimNo, 1.0f - m_animBlendRate);
	//変更後のアニメーション0%
	DxLib::MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAnimNo, m_animBlendRate);
	m_animBlendRate += kAnimChangeRateSpeed;

	if (m_animBlendRate > 1.0f)
	{
		m_animBlendRate = 1.0f;
	}
	if (m_nowPlanet != nullptr)
	{
		float planetDistance = (m_nowPlanet->GetRigidbody()->GetPos() - m_rigid->GetPos()).Length();
		if (planetDistance > kGalaxyLength)
		{
			UI::GetInstance().SetTalkObjectHandle(UI::TalkGraphKind::TakasakiTaisa);
			UI::GetInstance().InText("吹き飛ばしだぁ！！");

			m_isDead = true;
		}
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

	//回転は難しいのでモデルの向き(Y,Z)を無理やり設定
	MV1SetRotationZYAxis(m_modelHandle, (m_moveDir * -1).VGet(), m_upVec.GetNormalized().VGet(), 0);


	//当たり判定の更新
	//※直径分ずらすため半径を2倍にしている
	m_headCol->SetShiftPosNum(m_upVec * (m_footCol->GetRadius() * 2 + m_bodyCol->GetRadius() * 2 + m_headCol->GetRadius()));
	m_bodyCol->SetShiftPosNum(m_upVec * (m_footCol->GetRadius() * 2 + m_bodyCol->GetRadius()));
	m_footCol->SetShiftPosNum(m_upVec * m_footCol->GetRadius());

	m_lookPoint = m_rigid->GetPos() + m_upVec * kCamerLookPointHeight;

}

void Player::Draw()
{
	//無敵状態の間は2と3の公倍数だけ描画してチカチカさせる
	if (m_visibleCount % 3 == 0 || m_visibleCount % 2 == 0)
	{
		MV1DrawModel(m_modelHandle);
	}

#if DEBUG
	m_spinCol->DebugDraw(m_rigid->GetPos());
	//DrawLine3D(m_rigid->GetPos().VGet(), Vec3(m_rigid->GetPos() + m_shotDir * 100).VGet(), 0x0000ff);
	Vec3 axis = Cross(m_upVec, m_moveDir);//上方向ベクトルと進行方向ベクトルの外積から回転軸を生成
	axis.Normalize();//単位ベクトル化

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


}

void Player::SetBoost(Vec3 sideVec)
{
	//加速中は加速方向の右方向にローカル座標系の右方向を固定
	m_sideVec = sideVec * -1;
	//浮遊アニメーションに変換
	ChangeAnim(AnimNum::AnimationNumFall);
}

void Player::SetIsOperation(bool flag)
{
	//移動ベクトルを初期化
	SetVelocity(Vec3::Zero());

	if (flag)//他のオブジェクトに操作される場合
	{
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumRolling);
		//重力の影響をうけないようにする
		SetAntiGravity();
		//状態の変更
		m_playerUpdate = &Player::OperationUpdate;
		//フラグを立てる
		m_isOperation = true;
	}
	else//他のオブジェクトに操作されている状態を解除する場合
	{
		//重力の影響を受けるようにする
		SetAntiGravity(false);
		//アイドル状態に移行
		ChangeAnim(AnimNum::AnimationNumIdle);
		//フラグを下ろす
		m_isOperation = false;
	}
}

void Player::OnDamege(Vec3 knockBackVec, float damage)
{
	m_state = State::Damage;

	//ノックバック
	m_rigid->SetVelocity(knockBackVec);
	StartJoypadVibration(DX_INPUT_PAD1, kOnDamageJoypadVibrationPower, kOnDamageJoypadVibrationTime);
	m_prevUpdate = m_playerUpdate;
	m_playerUpdate = &Player::DamegeUpdate;

	//ダメージを受ける
	m_hp -= damage;

	m_isVisible = true;
	m_damageFrame = kDamageFrameMax;
	ChangeAnim(AnimNum::AnimationNumHit);
}

void Player::OnCollideEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
	printf("CollideEnter");

	//ステージと衝突
	if (colider->GetTag() == ObjectTag::Stage)
	{
		printf("Stage\n");
		m_isOperation = false;
		m_isJump = false;

		//ヒップドロップ中
		if (m_playerUpdate == &Player::DropAttackUpdate)
		{
			//着地音を流す
			PlaySoundMem(SoundManager::GetInstance().GetSoundData(kJumpDropGroundSEName), DX_PLAYTYPE_BACK);
		}
	}

	//クリスタルと衝突
	if (colider->GetTag() == ObjectTag::Crystal)
	{
		printf("Crystal\n");
		m_isJump = false;

	}

	//クリボーと衝突
	if (colider->GetTag() == ObjectTag::Kuribo)
	{
		printf("Kuribo\n");

		//スピンしていたら
		if (m_state=State::Spin)
		{
			auto kuribo = std::dynamic_pointer_cast<Kuribo>(colider);
			//ノックバックするベクトルを計算
			Vec3 enemyAttackDir = m_rigid->GetPos() - colider->GetRigidbody()->GetPos();
			enemyAttackDir.Normalize();
			Vec3 knockBackVec = enemyAttackDir * 2;

			//ダメージを受ける
			OnDamege(knockBackVec, colider->GetPower());

			//60フレームスタンさせる
			kuribo->Stan(60);
			PlaySoundMem(m_parrySEHandle, DX_PLAYTYPE_BACK);
		}

		//胴体と衝突
		if (ownTag == ColideTag::Body)
		{
			//無敵なら返す
			if (m_isVisible)return;

			m_postState = m_state;
			//HPを減らす
			PlaySoundMem(m_hitSEHandle, DX_PLAYTYPE_BACK);

		}
		else if (ownTag == ColideTag::Foot)//足と衝突
		{
			//プレイヤーをジャンプさせる
			CommandJump();
		}
	}

	//タコボーと衝突
	if (colider->GetTag() == ObjectTag::Takobo)
	{
		printf("Takobo\n");

		//ノックバックのベクトル計算
		Vec3 knockBackVec = Vec3(m_rigid->GetPos() - colider->GetRigidbody()->GetPos()).GetNormalized() * 3;

		//スピンを当てたら
		if (m_state = State::Spin)
		{
			//パリィSEを流す
			PlaySoundMem(m_parrySEHandle, DX_PLAYTYPE_BACK);
			//タコボーをぶっ飛ばす
			colider->GetRigidbody()->SetVelocity(knockBackVec * -1);
		}
		else
		{
			//無敵なら返す
			if (m_isVisible)return;

			//衝突SEを流す
			PlaySoundMem(m_hitSEHandle, DX_PLAYTYPE_BACK);
			//ダメージを受ける
			OnDamege(knockBackVec, colider->GetPower());
		}
	}
	if (colider->GetTag() == ObjectTag::Gorori)
	{

		printf("Gorori\n");

		//ノックバックのベクトル計算
		Vec3 knockBackVec = (m_rigid->GetPos() - colider->GetRigidbody()->GetPos()).GetNormalized() * 4;
		if (m_state = State::Spin)
		{
			//パリィSEを流す
			PlaySoundMem(m_parrySEHandle, DX_PLAYTYPE_BACK);

			//ゴロリをぶっ飛ばす
			colider->GetRigidbody()->SetVelocity(knockBackVec.GetNormalized());
			auto gorori = dynamic_pointer_cast<Gorori>(colider);
			gorori->SetAttackDir(knockBackVec * -1);
		}
		else
		{
			//無敵なら返す
			if (m_isVisible)return;

			//衝突SEを流す
			PlaySoundMem(m_hitSEHandle, DX_PLAYTYPE_BACK);
			//ダメージを受ける
			OnDamege(knockBackVec, colider->GetPower());
		}
	}

	//敵の攻撃
	if (colider->GetTag() == ObjectTag::EnemyAttack)
	{
		printf("EnemyAttack\n");

		//ノックバックのベクトルの計算
		Vec3 knockBackVec = (colider->GetRigidbody()->GetVelocity());
		if (m_state = State::Spin)
		{
			//パリィSEを流す
			PlaySoundMem(m_parrySEHandle, DX_PLAYTYPE_BACK);

			//敵の攻撃をぶっ飛ばして返す
			auto attackSphere = dynamic_pointer_cast<EnemySphere>(colider);
			attackSphere->SetVelocity(knockBackVec * -1);
			attackSphere->SetCounterFlag();
		}
		else
		{
			//無敵なら返す
			if (m_isVisible)return;

			m_postState = m_state;
			//衝突SEを流す
			PlaySoundMem(m_hitSEHandle, DX_PLAYTYPE_BACK);
			OnDamege(knockBackVec, colider->GetPower());
		}
	}

	//衝撃波に衝突時
	if (colider->GetTag() == ObjectTag::Electronic)
	{
		printf("Electro\n");

		//無敵なら返す
		if (m_isVisible)return;

		m_postState = m_state;
		//衝撃波に衝突時のSEを流す
		PlaySoundMem(m_elecSEHandle, DX_PLAYTYPE_BACK);

		//ノックバックのベクトルの計算
		Vec3 knockBackVec = (colider->GetRigidbody()->GetVelocity()) * -1;
		//ダメージを受ける
		OnDamege(knockBackVec, colider->GetPower());
	}

	//ボスと衝突時
	if (colider->GetTag() == ObjectTag::Boss)
	{
		//スピンをしていない時
		if (!(m_state == State::Spin))
		{
			//ボスがダッシュ中またはタックル中の時
			if (colider->GetState() == State::Running || colider->GetState() == State::Tackle)
			{
				if (m_isVisible)return;//無敵なら返す
				m_postState = m_state;

				//ノックバックのベクトルの計算
				Vec3 dir = m_rigid->GetPos() - colider->GetRigidbody()->GetPos();
				dir.Normalize();
				dir = dir + m_upVec * 2;
				dir.Normalize();
				Vec3 knockBackVec = dir * 3;
				//ダメージを受ける
				OnDamege(knockBackVec, colider->GetPower());
			}
		}
	}

	//クリアオブジェクトと衝突
	if (colider->GetTag() == ObjectTag::ClearObject)
	{
		printf("ClearObject\n");

		//クリア条件を満たす
		m_isClear = true;
	}
}

void Player::OnCollideStay(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
}

void Player::OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
	printf("TriggerEnter\n");

	//重力圏と衝突
	if (colider->GetTag() == ObjectTag::Stage)
	{
		//加速中なら
		if (m_playerUpdate == &Player::BoostUpdate)
		{
			//ジャンプ中状態に移行
			ChangeAnim(AnimNum::AnimationNumIdle);
			m_playerUpdate = &Player::JumpingUpdate;
		}
		//現在の惑星の更新
		m_nowPlanet = std::dynamic_pointer_cast<Planet>(colider);
	}

	//敵の攻撃の衝突時
	if (colider->GetTag() == ObjectTag::EnemyAttack)
	{
		//無敵なら返す
		if (m_isVisible)return;
		m_postState = m_state;
		//衝突SEを流す
		PlaySoundMem(m_hitSEHandle, DX_PLAYTYPE_BACK);
		Vec3 knockBackVec = (colider->GetRigidbody()->GetVelocity()) * -1;

		//ダメージを受ける
		OnDamege(knockBackVec, colider->GetPower());
	}

	//衝撃波に衝突時
	if (colider->GetTag() == ObjectTag::Electronic)
	{
		printf("Electro\n");
		m_postState = m_state;

		//無敵なら返す
		if (m_isVisible)return;
		//衝撃波衝突SEを流す
		PlaySoundMem(m_elecSEHandle, DX_PLAYTYPE_BACK);
		Vec3 knockBackVec = (colider->GetRigidbody()->GetVelocity()) * -1;
		//ダメージを受ける
		OnDamege(knockBackVec, colider->GetPower());
	}

	//弾の種類をグラップルに変えるアイテムと衝突時
	if (colider->GetTag() == ObjectTag::StickStarItem)
	{
		printf("StickStarItem\n");

		//弾の種類をグラップルに変える
		m_shotUpdate = &Player::ShotTheStickStar;

		//パワーアップアイテム取得時のSEを流す
		PlaySoundMem(m_powerUpItemGetSEHandle, DX_PLAYTYPE_BACK);
	}

	//ヒップドロップを強化し、衝撃波を発生させるようにするアイテムと衝突時
	if (colider->GetTag() == ObjectTag::FullPowerDropItem)
	{
		printf("FullPowerDropItem\n");
		//衝撃波を発生させるヒップドロップに強化
		m_dropAttackUpdate = &Player::FullPowerDropAttackUpdate;

		//パワーアップアイテム取得時のSEを流す
		PlaySoundMem(m_powerUpItemGetSEHandle, DX_PLAYTYPE_BACK);
	}

	//スターコインと衝突時
	if (colider->GetTag() == ObjectTag::Coin)
	{
		printf("Coin\n");

		//スターコインのパワー分回復
		m_hp += colider->GetPower();
		//スターコイン所持数を増やす
		m_coinCount++;
		//HPが上限を超えたら
		if (m_hp > kPlayerHPMax)
		{
			//満タンにする
			m_hp = kPlayerHPMax;
		}
	}

	//星の欠片と衝突時
	if (colider->GetTag() == ObjectTag::FragmentOfStar)
	{
		//特殊なアイテム取得時のSEを流す
		PlaySoundMem(m_specialItemGetSEHandle, DX_PLAYTYPE_BACK);
		//星の欠片の所持数を増やす
		m_fragmentCount++;
		//星の欠片を5つ集めたら
		if (m_fragmentCount >= 5)
		{
			//ブースター生成
			auto obj = GalaxyCreater::GetInstance().GetCollidable(3);
			obj->SetIsActive(true);
		}
	}

	//クリアオブジェクトと衝突時
	if (colider->GetTag() == ObjectTag::ClearObject)
	{
		//クリア条件を満たす
		m_isClear = true;
	}
}

void Player::OnTriggerStay(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{

}

void Player::Landing(int recast)
{
	ChangeAnim(AnimNum::AnimationNumJumpAttack);

	MV1SetAttachAnimTime(m_modelHandle, m_currentAnimNo, static_cast<float>(57 - recast));//57:着地アニメーションの終了時間
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

	//着地する
	Landing();
}

void Player::NeutralUpdate()
{
	m_stateName = "Neutral";
	m_state = State::Neutral;

	//アナログスティックを使って移動
	Vec3 move = Move();

	//スティックの歩き範囲を超えてるか
	if (std::abs(move.Length()) >= kWalkInputVecMag * kMaxSpeed)
	{
		//歩き状態に移行
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumRun);
		m_playerUpdate = &Player::WalkingUpdate;
	}

	//RBが入力されている間にAボタンが入力されたか
	if ((Pad::IsPress(PAD_INPUT_Z)) && Pad::IsTrigger(PAD_INPUT_1))
	{
		m_postState = m_state;
		//カメラのイージング速度を設定
		m_cameraEasingSpeed = kOnDashCameraEasingSpeed;
		ChangeAnim(AnimNum::AnimationNumJump);

		//ジャンプフラグを立てる
		m_isJump = true;
		//通常ジャンプの2倍の強さでジャンプ
		move += m_upVec.GetNormalized() * kJumpPower * 2;
		//大ジャンプ状態に移行
		m_playerUpdate = &Player::DashJumpUpdate;
	}
	//プレイヤーの最大移動速度は0.01f/frame
	else if (Pad::IsTrigger(PAD_INPUT_1))//XBoxのAボタン
	{
		//Aボタンだけ入力されていたら

		//ただのジャンプ
		m_postState = m_state;
		CommandJump();
	}

	//XBoxのBボタンが入力されているか
	if (Pad::IsTrigger(PAD_INPUT_B))
	{
		//スピンさせる
		ChangeAnim(AnimNum::AnimationNumSpin, kSpinAnimationSpeed);
		m_postState = m_state;
		m_playerUpdate = &Player::SpinActionUpdate;
	}

	//ベクトルを加算
	m_rigid->AddVelocity(move);

	//加速中じゃないかつ地面に着地していないとき
	if (m_playerUpdate != &Player::BoostUpdate && !m_footCol->OnHit())
	{
		//ジャンプ中状態に移行
		m_postState = m_state;
		m_playerUpdate = &Player::JumpingUpdate;
	}
}

void Player::WalkingUpdate()
{
	m_stateName = "Walking";

	m_state = State::Walk;

	//移動ベクトル
	Vec3 ans;
	ans = Move();

	//スティックの歩き範囲より小さい入力か
	if (std::abs(ans.Length()) < kWalkInputVecMag * kMaxSpeed)
	{
		//アイドル状態に移行
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumIdle);
		m_playerUpdate = &Player::NeutralUpdate;
	}

	//LBが入力中か
	if ((Pad::IsPress(PAD_INPUT_Z)))
	{
		//ダッシュ状態に移行
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumRun, kDashMag);
		m_playerUpdate = &Player::DashUpdate;
	}

	//Aボタンが入力されているか
	if (Pad::IsTrigger(PAD_INPUT_1))//XBoxのAボタン
	{
		//ジャンプされる
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumJump);
		m_isJump = true;
		ans += m_upVec.GetNormalized() * kJumpPower;
		m_playerUpdate = &Player::JumpingUpdate;
	}

	//Bボタンが入力されているか
	if (Pad::IsTrigger(PAD_INPUT_B))
	{
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumSpin, kSpinAnimationSpeed);
		m_playerUpdate = &Player::SpinActionUpdate;
	}

	//移動ベクトル
	m_rigid->AddVelocity(ans);

	//着地しているか
	if (!m_footCol->OnHit())
	{
		//ジャンプ中状態に移行
		m_postState = m_state;
		m_playerUpdate = &Player::JumpingUpdate;
	}
}

void Player::DashUpdate()
{
	m_cameraEasingSpeed = kOnDashCameraEasingSpeed;
	m_stateName = "Dash";
	m_state = State::Dash;

	//移動ベクトル
	Vec3 ans;
	ans = Move();

	//LBボタンを放したら
	if ((Pad::IsRelase(PAD_INPUT_Z)))
	{
		//歩き状態に移行
		m_postState = m_state;
		m_cameraEasingSpeed = kOnNeutralCameraEasingSpeed;
		ChangeAnim(AnimNum::AnimationNumRun);
		m_playerUpdate = &Player::WalkingUpdate;
	}

	//スティックの歩き範囲より小さい入力か
	if (std::abs(ans.Length()) < kWalkInputVecMag * kMaxSpeed)
	{
		//アイドル状態に移行
		m_postState = m_state;
		m_cameraEasingSpeed = kOnNeutralCameraEasingSpeed;
		ChangeAnim(AnimNum::AnimationNumIdle);
		m_playerUpdate = &Player::NeutralUpdate;
	}

	//Aボタンが入力されているか
	if (Pad::IsTrigger(PAD_INPUT_1))
	{
		//ジャンプ状態に以降
		m_postState = m_state;
		m_cameraEasingSpeed = kOnNeutralCameraEasingSpeed;
		ChangeAnim(AnimNum::AnimationNumJump);
		m_isJump = true;
		ans += m_upVec.GetNormalized() * kJumpPower;
		m_playerUpdate = &Player::DashJumpUpdate;
	}

	//Bボタンが入力されているか
	if (Pad::IsTrigger(PAD_INPUT_B))
	{
		//スピン状態に移行
		m_postState = m_state;
		m_cameraEasingSpeed = kOnNeutralCameraEasingSpeed;
		ChangeAnim(AnimNum::AnimationNumSpin, kSpinAnimationSpeed);
		m_playerUpdate = &Player::SpinActionUpdate;
	}

	//移動ベクトルに加算
	m_rigid->AddVelocity(ans * kDashMag);

	//着地していないなら
	if (!m_footCol->OnHit())
	{
		//ジャンプ中状態に移行
		m_postState = m_state;
		m_playerUpdate = &Player::JumpingUpdate;
	}
}

void Player::JumpingUpdate()
{
	m_stateName = "Jumping";
	m_state = State::Jump;

	//Aボタンが入力されているか
	if (Pad::IsTrigger(PAD_INPUT_1))
	{
		//ヒップドロップ攻撃をする
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumJumpAttack);
		m_rigid->SetVelocity(Vec3::Zero());
		m_playerUpdate = &Player::DropAttackUpdate;
	}

	//Bボタンが入力されているか
	if (Pad::IsTrigger(PAD_INPUT_B))
	{
		//ジャンプ中のアクションを実行
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumSpin, kSpinAnimationSpeed);
		m_rigid->AddVelocity(m_frontVec * kJumpPower);
		m_playerUpdate = &Player::JumpActionUpdate;
	}

	//着地しているか
	if (m_footCol->OnHit())
	{
		//アイドル状態に移行
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

	//Aボタンが入力されているか
	if (Pad::IsTrigger(PAD_INPUT_1))
	{
		//ヒップドロップ状態に移行
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumJumpAttack);
		m_rigid->SetVelocity(Vec3::Zero());
		m_playerUpdate = &Player::DropAttackUpdate;
	}

	//Bボタンが入力されているか
	if (Pad::IsTrigger(PAD_INPUT_B))
	{
		//スピンさせる
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumSpin, kSpinAnimationSpeed);
		m_rigid->AddVelocity(m_frontVec * kJumpPower);
		m_playerUpdate = &Player::JumpActionUpdate;
	}

	//着地しているか
	if (m_footCol->OnHit())
	{
		//アイドル状態に移行
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

	//XBoxのAボタンの入力があったら
	if (Pad::IsTrigger(PAD_INPUT_1))
	{
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumJumpAttack);
		m_rigid->SetVelocity(Vec3::Zero());
		m_playerUpdate = &Player::DropAttackUpdate;
	}

	//何かに着地した時
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

	//現在のアニメーションの時間を取得
	float now = MV1GetAttachAnimTime(m_modelHandle, m_currentAnimNo);//現在のアニメーション再生カウント
	m_rigid->AddVelocity(m_upVec * -1);

	//アニメーションがチャージ中か
	bool isCharging = now < 16;//アニメーションの16フレーム目から落下アニメーションに入る

	//アニメーションがまだチャージ中の場合
	if (isCharging)
	{
		m_rigid->SetVelocity(Vec3::Zero());
	}

	//何かに着地したとき
	if (m_footCol->OnHit())
	{
		//着地にさせる
		m_postState = m_state;
		m_landingStanFrame = kLandingStanFrameNeutral;
		Landing();
	}
}

void Player::FullPowerDropAttackUpdate()
{
	m_stateName = "FullPowerDrop";
	m_state = State::FullpowerJumpDrop;


	float now = MV1GetAttachAnimTime(m_modelHandle, m_currentAnimNo);//現在の再生カウント
	m_rigid->AddVelocity(m_upVec * -kDashMag);//ちょっとずつ落下

	//Aボタンが入力されていたら
	if (Pad::IsPress(PAD_INPUT_1))
	{
		//チャージ量が上限に達していなかったら
		if (m_fullPowerChargeCount < kFullPowerChargeTimeMax)m_fullPowerChargeCount += kFullPowerChargeSpeed;
		//ベクトルの初期化
		m_rigid->SetVelocity(Vec3::Zero());

		if (now < 16)//アニメーションの16フレーム目から落下アニメーションに入る
		{
			MV1SetAttachAnimTime(m_modelHandle, m_currentAnimNo, 0);
			m_rigid->SetVelocity(Vec3::Zero());
		}
	}

	//着地していたら
	if (m_footCol->OnHit())
	{
		m_postState = m_state;
		//ここで衝撃波を発生させる
		m_impacts.push_back(std::make_shared<StampImpact>(m_rigid->GetPos() + m_upVec * -m_footCol->radius, m_nowPlanet->GetScale(), m_upVec * -1, ObjectTag::PlayerImpact, m_fullPowerChargeCount));
		MyEngine::Physics::GetInstance().Entry(m_impacts.back());
		m_fullPowerChargeCount = 0;
		//着地アニメーションを完全に再生
		Landing(kLandingAnimationTotalCount);
		m_landingStanFrame = 60;//60フレームの隙を作る
	}
}

void Player::LandingUpdate()
{
	m_stateName = "Landing";
	m_state = State::Land;
	m_landingStanFrame--;

	//着地の隙の残数がなくなったら
	if (m_landingStanFrame < 0)
	{
		//着地アニメーションが終了していない場合
		if (MV1GetAttachAnimTotalTime(m_modelHandle, m_currentAnimNo) != kLandingAnimationTotalCount)
		{
			//アイドルアニメーションに移行
			m_postState = m_state;
			ChangeAnim(AnimNum::AnimationNumIdle);
			m_playerUpdate = &Player::NeutralUpdate;
		}

		//アニメーションの現在の再生カウントを取得
		float Time = MV1GetAttachAnimTime(m_modelHandle, m_currentAnimNo);
		//40フレーム以上再生されていたら
		if (Time >= 40.f)
		{
			//アイドル状態に移行
			m_postState = m_state;
			ChangeAnim(AnimNum::AnimationNumIdle);
			m_playerUpdate = &Player::NeutralUpdate;

		}

	}
	else
	{
		//着地の隙がまだ残っていたら

		//もしアニメーションの現在のフレームが40フレーム以上だった場合
		if (MV1GetAttachAnimTime(m_modelHandle, m_currentAnimNo) >= 40)
		{
			//40フレーム目のアニメーションに固定
			MV1SetAttachAnimTime(m_modelHandle, m_currentAnimNo, 40);
		}
	}
}

void Player::AimingUpdate()
{
	m_stateName = "Aiming";

	//移動ベクトル
	Vec3 move;
	move = Move();

	//Aボタンが入力されているか
	if (Pad::IsTrigger(PAD_INPUT_1))
	{
		//ジャンプさせる
		m_postState = m_state;
		CommandJump();
	}

	//Bボタンが入力されているか
	if (Pad::IsTrigger(PAD_INPUT_B))
	{
		//スピンさせる
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumSpin, kSpinAnimationSpeed);
		m_playerUpdate = &Player::SpinActionUpdate;
	}

	//LBボタンが入力されているか
	if (Pad::IsTrigger(PAD_INPUT_Y))
	{
		//アイドル状態に移行
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumIdle);
		m_playerUpdate = &Player::NeutralUpdate;
	}

	//移動ベクトルを加算
	m_rigid->AddVelocity(move);

}

void Player::SpinActionUpdate()
{

	//スピンアクションを実行
	(this->*m_spinAttackUpdate)();
}

void Player::SpiningUpdate()
{
	m_state = State::Spin;
	m_stateName = "Spining";

	//移動ベクトル
	Vec3 move;
	move = Move();

	//移動ベクトルを加算
	m_rigid->AddVelocity(move);

	//回転させる
	m_spinAngle += kAngleRotateSpeed;
	if (m_spinAngle >= DX_PI_F * 2)
	{
		//アイドル状態に移行
		m_spinAngle = 0;
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumIdle);
		m_playerUpdate = &Player::NeutralUpdate;
	}
}

void Player::RollingAttackUpdate()
{
	m_state = State::Spin;
	m_stateName = "RollingAttack";

	//正面ベクトルの更新
	m_frontVec = Cross(m_sideVec, m_upVec);
	m_rigid->SetVelocity(m_frontVec);

	//Bボタンが入力されているか
	if (Pad::IsTrigger(PAD_INPUT_B))
	{
		
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumIdle);
		m_playerUpdate = &Player::NeutralUpdate;
		m_cameraEasingSpeed = kOnNeutralCameraEasingSpeed;
	}
}

void Player::JumpingSpinUpdate()
{
	m_state = State::Spin;
	m_stateName = "JumpSpin";

	//アナログスティックを使って移動

	//移動ベクトル
	Vec3 move;
	move = Move();

	//移動ベクトルを設定
	m_rigid->SetVelocity(move);

	//回転させる
	m_spinAngle += kAngleRotateSpeed*5;
	if (m_spinAngle >= DX_PI_F * 2)
	{
		//ジャンプ状態に移行
		m_spinAngle = 0;
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumJump);
		m_playerUpdate = &Player::JumpingUpdate;
	}
}

void Player::CommandJump()
{
	//ジャンプフラグを立てる
	m_isJump = true;
	//アニメーションを変更
	ChangeAnim(AnimNum::AnimationNumJump);
	//プレイヤーをジャンプ状態に移行
	m_playerUpdate = &Player::JumpingUpdate;
	//移動ベクトルを加算
	m_rigid->AddVelocity(m_upVec.GetNormalized() * kJumpPower);
}

void Player::BoostUpdate()
{
	m_stateName = "BoostingGolira";
	m_state = State::Boosting;

	//正面ベクトルの計算
	m_frontVec = Cross(m_upVec, m_sideVec);
	//正面ベクトルを移動方向ベクトルにする
	m_moveDir = m_frontVec;

	//加速が終了したら
	if (!m_state==State::Boosting)
	{
		//アイドル状態に移行
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumIdle);
		m_playerUpdate = &Player::NeutralUpdate;
	}
}

void Player::OperationUpdate()
{
	m_stateName = "NowControl";

	//移動ベクトルを設定
	m_rigid->SetVelocity(m_velocity);

	m_state = State::Operation;

	//移動方向ベクトルを算出
	m_moveDir = m_rigid->GetPos() - m_postPos;
	m_moveDir.Normalize();

	//1フレ前の位置を設定
	m_postPos = m_rigid->GetPos();

	//ローカル座標系を算出
	m_sideVec = Cross(m_upVec, m_moveDir);
	m_upVec = Cross(m_moveDir, m_sideVec);

	//他のオブジェクトに操作されているオブジェクトが解除されたとき
	if (!m_isOperation)
	{
		//ジャンプ中状態に変換
		SetAntiGravity(false);
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumIdle);
		m_playerUpdate = &Player::JumpingUpdate;
	}
}

Vec3 Player::Move()
{
	int analogX = 0, analogY = 0;
	//右スティックの入力を受け付ける
	GetJoypadAnalogInput(&analogX, &analogY, DX_INPUT_PAD1);
	analogY = -analogY;

	m_inputVec.x = static_cast<float>(analogX);
	m_inputVec.z = static_cast<float>(-analogY);

	//移動ベクトル
	Vec3 ans;

	//正面ベクトル
	Vec3 front;
	// アナログスティックの入力を反映
	m_sideVec = GetCameraRightVector();
	front = Cross(m_sideVec, m_upVec).GetNormalized();
	m_sideVec = Cross(m_upVec, m_frontVec).GetNormalized();
	ans = front * static_cast<float>(analogY);
	ans += m_sideVec * static_cast<float>(analogX);

	//入力がされていたら
	if (ans.Length() > 0)
	{
		m_moveDir = ans.GetNormalized();
	}
	// 無効範囲の確認
	float len = ans.Length();
	if (len < kAnalogRangeMin) {
		return Vec3(0, 0, 0);  // 無効な場合はゼロベクトルを返す
	}

	//正面ベクトルの設定
	m_frontVec = front;
	m_inputVec.Normalize();
	ans = ans.GetNormalized() * kMaxSpeed; // 正規化し、速度を掛ける

	//移動ベクトルを返す
	return ans;
}

void Player::ShotTheStar()
{
	//コインを持っていたら
	if (m_coinCount > 0)
	{
		//弾を発射
		m_coinCount--;
		m_sphere.back()->Init();
		PlaySoundMem(m_shotTheStarSEHandle, DX_PLAYTYPE_BACK);

		//弾の発生位置
		Vec3 shotPos = MV1GetFramePosition(m_modelHandle, m_leftHandFrameIndex);
		m_sphere.push_back(std::make_shared<PlayerSphere>(
			Priority::Low, //位置補正優先度
			ObjectTag::PlayerBullet,//タグをプレイヤーの弾に設定
			shared_from_this(),//プレイヤーのポインタを渡す
			shotPos,//弾の発生位置を渡す
			m_shotDir,//弾の発射方向を渡す
			m_sideVec,//弾の横方向ベクトルを設定
			1,//弾の動きの指定 1: 直進移動
			0xff0000//色
		));
		MyEngine::Physics::GetInstance().Entry(m_sphere.back());
	}
}

void Player::ShotTheStickStar()
{
	//弾が発射されていなかったら
	if (m_sphere.size() == 0)
	{
		//弾の発生位置
		Vec3 shotPos = MV1GetFramePosition(m_modelHandle, m_leftHandFrameIndex);
		//弾の生成
		m_sphere.push_back(std::make_shared<PlayerStickSphere>(
			Priority::Low, //位置補正優先度
			ObjectTag::PlayerBullet,//タグをプレイヤーの弾に設定
			shared_from_this(), //プレイヤーのポインタを渡す
			shotPos, //弾の発生位置を渡す
			m_shotDir, //弾の発射方向を渡す
			m_sideVec, //弾の横方向ベクトルを設定
			1,//弾の動きの指定 1: 直進移動
			0xff0000//色
		));

		//グラッピング弾を発射
		m_coinCount--;
		m_sphere.back()->Init();
		MyEngine::Physics::GetInstance().Entry(m_sphere.back());
		PlaySoundMem(m_shotStickStarSEHandle, DX_PLAYTYPE_BACK);
	}
	else
	{
		//すでに弾を発射していたら弾の状況ごとに効果を発生させる
		m_sphere.back()->Effect();
	}
}

void Player::DamegeUpdate()
{
	m_stateName = "Damege";
	m_state = State::Damage;


	//プレイヤーの状態が前のプレイヤーの状態と異なった場合
	if (m_prevUpdate != m_playerUpdate)
	{
		ChangeAnim(AnimNum::AnimationNumIdle);
		if (m_prevUpdate == &Player::TalkingUpdate)
		{
			//アイドル状態に移行
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
		//アイドル状態に移行
		m_postState = m_state;
		ChangeAnim(AnimNum::AnimationNumIdle);
		m_playerUpdate = &Player::NeutralUpdate;
		m_prevUpdate = m_playerUpdate;
	}

}

void Player::DeathUpdate()
{

	m_stateName = "Death";
	m_state = State::Death;

	//現在のアニメーションの時間の取得
	float animTime = MV1GetAttachAnimTime(m_modelHandle, m_currentAnimNo);
	//死亡アニメーションの総再生時間の取得
	float totalTime = MV1GetAttachAnimTotalTime(m_modelHandle, m_currentAnimNo);

	//Aボタンが入力されたか
	if (Pad::IsTrigger(PAD_INPUT_1))
	{
		//入力されていたら
		MV1SetAttachAnimTime(m_modelHandle, m_currentAnimNo, animTime - kRevivalInputPower);//蘇生ポイント分アニメーションを巻き戻し

		//アニメーションが完全に巻き戻ったら蘇生される
		if (MV1GetAttachAnimTime(m_modelHandle, m_currentAnimNo) <= 0)
		{
			//蘇生回数を足す
			m_revivalCount++;
			//HPが半分の状態で回復
			m_hp = kPlayerHPMax / 2;
			//アイドル状態に移行
			m_playerUpdate = &Player::NeutralUpdate;
			ChangeAnim(AnimNum::AnimationNumIdle);
		}
	}
	if (animTime >= totalTime - 2)
	{
		//死亡したら
		UI::GetInstance().InText("ドレイク？");
		UI::GetInstance().InText("ドレイィィィィク！！！！！");
		m_isDead = true;
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
	if (!m_isAiming)
	{
		m_shotDir = m_frontVec;
		return;
	}
}

void Player::DeleteManage()
{
	//デストロイフラグが立っている弾を削除する
	auto result = remove_if(m_sphere.begin(), m_sphere.end(), [this](const auto& sphere)
		{
			bool isOut = sphere->IsDestroy();
			if (isOut)
			{
				sphere->OnDestroy();
				MyEngine::Physics::GetInstance().Exit(sphere);
			}
			return isOut;
		});
	m_sphere.erase(result, m_sphere.end());
}

void Player::TalkingUpdate()
{
	m_stateName = "Talking";
	m_state = State::Talk;

	//コントローラー入力を管理するクラスの状態がプレイヤーの入力受付状態だったら
	if (Pad::IsState("PlayerInput"))
	{
		//元の状態に戻す
		m_postState = m_state;
		m_playerUpdate = m_postUpdate;
	}
}