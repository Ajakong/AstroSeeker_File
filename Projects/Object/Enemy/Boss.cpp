#include "Boss.h"
#include"Planet.h"
#include"Player.h"
#include"Physics.h"
#include"SoundManager.h"
#include"Easing.h"

#include"GalaxyCreater.h"
#include"UI.h"
namespace
{
	constexpr int kHPFull = 170;
	constexpr float kBodyRadiusSize = 10.f;
	constexpr int kKnockBackFrameMax = 50;

	constexpr float kAnimFrameSpeed = 30.0f;//アニメーション進行速度

	//アニメーションの切り替えにかかるフレーム数
	constexpr float kAnimChangeFrame = 8.0f;
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;

	constexpr float kFrameParSecond = 60.0f;
	constexpr float kRunningSpeed = 3.0f;

	constexpr float kOnPhaseTwoHp = 100;


	constexpr int kTackleMaxChargeFrame = 80;
	constexpr int kRunningFrameMax = 400;
	constexpr int kCreateShotFrame = 60;
	constexpr int kActionFrame = 100;

	constexpr int kTackleTime = 50;
	constexpr int kTackleSpeed = 2;
	constexpr int kSearchRange = 100;

	constexpr int kTackleLength = kTackleSpeed *kTackleTime;

	const char* kDamageSEName = "Parry.mp3";
	const char* kCriticalHitSEName = "CounterHit.mp3";
	const char* kDropSEName = "BossDropSE.mp3";

	const char* kGamePlayBGMName = "BattleOfAstro.mp3";
	const char* kBossBattleBGMName = "bossbattle.mp3";
	const char* kSuperMatrialBGMName = "SuperMaterial.mp3";

	const char* kBossModelName = "SpaceMutant.mv1";
}
Boss::Boss(Vec3 pos, std::shared_ptr<Player>player):Enemy(Priority::Boss,ObjectTag::Boss),
	//アニメーションを初期化
	m_prevAnimNo(0),
	m_currentAnimNo(0),
	m_animBlendRate(0),
	m_animationSpeed(0),
	//フレームを初期化
	m_jumpCount(0),
	m_actionFrame(0),
	m_runningFrame(0),
	m_knockBackFrame(0),
	m_shotCreateFrame(0),
	m_tackleChargeFrame(0),
	//ハンドルを初期化
	m_criticalHandle(-1),
	m_dropSoundHandle(-1),
	m_damageSoundHandle(-1),
	//フラグを初期化
	m_isHit(false),
	m_isTalk(false),
	m_isWakeUp(false),
	m_isTackle(false),
	m_isBattle(false),
	m_onColStage(false)
{
	m_power = 20.f;
	m_player = player;
	m_hp = kHPFull;
	m_rigid->SetPos(pos);

	AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);
	m_collision = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
	m_collision->radius = kBodyRadiusSize;

	m_color = 0xff00ff;
	m_bossUpdate = &Boss::InitUpdate;

	m_modelHandle = MV1LoadModel(kBossModelName);
	m_damageSoundHandle=SoundManager::GetInstance().GetSoundData(kDamageSEName);
	m_criticalHandle = SoundManager::GetInstance().GetSoundData(kCriticalHitSEName);
	m_dropSoundHandle = SoundManager::GetInstance().GetSoundData(kDropSEName);

	m_phaseUpdate = &Boss::PhaseOneUpdate;
}

Boss::~Boss()
{
}

void Boss::Init()
{
	m_runawayPos = Vec3(562, 650, 1953);
}

void Boss::Update()
{
	//上方向ベクトルをいい感じに線形保管
	
	(this->*m_bossUpdate)();

	//ノックバック中
	if (m_isHit)
	{
		m_knockBackFrame++;
		m_color = 0xff0000;
	}

	//ノックバック時間が一定になったら
	if (m_knockBackFrame > 30)
	{
		m_isHit = false;
	}

	//HPが0以下になったら
	if (m_hp <= 0)
	{
		m_isBattle = false;
		SoundManager::GetInstance().ChangeBGM(SoundManager::GetInstance().GetSoundData(kSuperMatrialBGMName));
		m_dropItem = std::make_shared<ClearObject>(m_rigid->GetPos(), true);
		Physics::GetInstance().Entry(m_dropItem);
		m_isDestroyFlag = true;
		UI::GetInstance().SetTalkObjectHandle(UI::TalkGraphKind::Boss);
		std::list<std::string> text1;
		text1.push_back("ばかなぁこのおれが。。。");
		UI::GetInstance().InTexts(text1);

		UI::GetInstance().SetNextTalkObjectHandle(UI::TalkGraphKind::TakasakiTaisa);
		UI::GetInstance().InNextText("やったぞ！お前の勝ちだ！");

		std::list<std::string> text2;
		text2.push_back("さぁ奴が吐き出したスーパーマテリアルを取り返すんだ！");
		UI::GetInstance().InNextTexts(text2);
	}
	DeleteObject(m_impacts);

	m_onColStage = false;
}

void Boss::Draw()
{
	DrawSphere3D(m_rigid->GetPos().VGet(), kBodyRadiusSize,8,m_color,0x000000,true);
	MV1DrawModel(m_modelHandle);
}

void Boss::SetMatrix()
{

}

void Boss::PhaseOneUpdate()
{
	//最初は衝撃波一回のみ

	m_actionFrame++;

	//行動のクールダウンが完了したら
	if (m_actionFrame < kActionFrame)return;
	m_actionFrame = 0;
	switch (GetRand(1))
	{
	case(0):
		m_rigid->AddVelocity(m_upVec * 4);
		m_bossUpdate = &Boss::FullpowerJumpUpdate;
		break;
	case(1):
		m_rigid->AddVelocity(m_upVec * 4);
		m_bossUpdate = &Boss::FullpowerJumpUpdate;
		break;
	}

	//HPが一定以下になったら次のフェーズに移行
	if (m_hp <= kOnPhaseTwoHp)
	{
		

		UI::GetInstance().SetTalkObjectHandle(UI::TalkGraphKind::Boss);

		std::list<std::string> texts1;
		texts1.push_back("くそぉ！おまえなんなんだよ！");
		texts1.push_back("スーパーマテリアルは渡さねぇぞ！");
		UI::GetInstance().InTexts(texts1);

		UI::GetInstance().InText("逃げるってわけじゃねぇからな！");

		m_phaseUpdate = &Boss::PhaseTwoUpdate;
		m_bossUpdate = &Boss::RunawayUpdate;
	}

}

void Boss::PhaseTwoUpdate()
{
	m_actionFrame++;
	//行動のクールダウンが完了したら
	if (m_actionFrame < kActionFrame)return;
	m_actionFrame = 0;

	Vec3 ToTargetVec = (m_player->GetPos() - m_rigid->GetPos());
	//タックルのレンジ内に入ったら
	if (ToTargetVec.Length() < kTackleLength)
	{
		switch (GetRand(3))
		{
		case(0):
			m_bossUpdate = &Boss::TackleUpdate;
			break;
		case(1):
			m_bossUpdate = &Boss::TackleUpdate;
			break;
		case(2):
			m_rigid->AddVelocity(m_upVec * 2);
			m_bossUpdate = &Boss::JumpingUpdate;
			break;
		case(3):
			m_rigid->AddVelocity(m_upVec * 4);
			m_bossUpdate = &Boss::FullpowerJumpUpdate;
			break;
		}
	}
	else
	{
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
	

}

void Boss::PhaseThreeUpdate()
{
	m_actionFrame++;
	if (m_actionFrame < kActionFrame)return;
	m_actionFrame = 0;
	Vec3 ToTargetVec = (m_player->GetPos() - m_rigid->GetPos());
	if (ToTargetVec.Length() < kTackleLength)
	{
		switch (GetRand(3))
		{
		case(0):
			m_bossUpdate = &Boss::TackleUpdate;
			break;
		case(1):
			m_bossUpdate = &Boss::TackleUpdate;
			break;
		case(2):
			m_rigid->AddVelocity(m_upVec * 2);
			m_bossUpdate = &Boss::JumpingUpdate;
			break;
		case(3):
			m_rigid->AddVelocity(m_upVec * 4);
			m_bossUpdate = &Boss::FullpowerJumpUpdate;
			break;
		}
	}
	else
	{
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
}

void Boss::InitUpdate()
{
	m_isBattle = false;
	//今は簡単にプレイヤーとの距離をみて起動

	float lenge = (m_player->GetRigidbody()->GetPos() - m_rigid->GetPos()).Length();

	//プレイヤーが索敵範囲内に入ったら起動
	m_isWakeUp = lenge < kSearchRange;

	//起動したら
	if (m_isWakeUp)
	{

		//バトルフェーズ1の時
		if (m_phaseUpdate == &Boss::PhaseOneUpdate)
		{
			UI::GetInstance().SetTalkObjectHandle(UI::TalkGraphKind::Boss);
			std::list<std::string> text1;
			text1.push_back("やっと来たか。");
			text1.push_back("お前だな、俺様のことをこそこそかぎまわってる奴は");
			UI::GetInstance().InTexts(text1);

			UI::GetInstance().SetNextTalkObjectHandle(UI::TalkGraphKind::TakasakiTaisa);
			UI::GetInstance().InNextText("なに！？気づかれていたのか！");

			UI::GetInstance().SetNextTalkObjectHandle(UI::TalkGraphKind::Boss);

			std::list<std::string> text2;
			text2.push_back("めざわりなんだよ！");
			text2.push_back("消えてもらうぜ");
			UI::GetInstance().InNextTexts(text2);

			UI::GetInstance().SetNextTalkObjectHandle(UI::TalkGraphKind::TakasakiTaisa);
			UI::GetInstance().InNextText("ドレイク！ヤツは戦うつもりみたいだ");
			UI::GetInstance().InNextText("ここで決めてしまうぞ！");
			m_isBattle = true;
			SoundManager::GetInstance().ChangeBGM(SoundManager::GetInstance().GetSoundData(kBossBattleBGMName));
			m_bossUpdate = &Boss::NeutralUpdate;
		}

		//バトルフェーズ2の時
		if (m_phaseUpdate == &Boss::PhaseTwoUpdate)
		{
			UI::GetInstance().SetTalkObjectHandle(UI::TalkGraphKind::Boss);
			std::list<std::string> text1;
			text1.push_back("しつけぇ奴らだな");
			text1.push_back("今度は本気の本気で消えてもらう");
			UI::GetInstance().InTexts(text1);

			UI::GetInstance().SetNextTalkObjectHandle(UI::TalkGraphKind::TakasakiTaisa);
			std::list<std::string> text2;
			text2.push_back("ドレイク！ここが決戦の場になる。");
			text2.push_back("ここでヤツを倒してスーパーマテリアルを取り返せ！");
			UI::GetInstance().InNextTexts(text2);

			UI::GetInstance().SetNextTalkObjectHandle(UI::TalkGraphKind::Boss);

			std::list<std::string> text3;
			text3.push_back("さっきみたいに甘くはやらねぇぞ！");
			UI::GetInstance().InNextTexts(text3);

			UI::GetInstance().SetNextTalkObjectHandle(UI::TalkGraphKind::TakasakiTaisa);
			UI::GetInstance().InNextText("すべてを出し切るぞ！");
			m_isBattle = true;
			SoundManager::GetInstance().ChangeBGM(SoundManager::GetInstance().GetSoundData(kBossBattleBGMName));
			m_bossUpdate = &Boss::NeutralUpdate;
		}
	}
}

void Boss::RestUpdate()
{

}

void Boss::NeutralUpdate()
{
	m_state = State::Neutral;
	(this->*m_phaseUpdate)();
}

void Boss::AnglyUpdate()
{
}

void Boss::DestroyPlanetUpdate()
{
}

void Boss::KnockBackUpdate()
{
	m_color = 0xffff00;
	m_state = State::Attack;
	m_knockBackFrame++;
	
	//ノックバックが終わったら
	if (m_knockBackFrame > kKnockBackFrameMax)
	{
		m_color = 0xff00ff;
		m_knockBackFrame = 0;
		m_bossUpdate = &Boss::NeutralUpdate;
	}
}

void Boss::JumpingUpdate()
{
	m_color = 0xffff00;
	m_state = State::Attack;

	//ステージに着地したら
	if (m_onColStage)
	{
		PlaySoundMem(m_dropSoundHandle, DX_PLAYTYPE_BACK);
		//ジャンプ回数が2回以上だったら
		if (m_jumpCount > 2)
		{
			m_jumpCount = 0;
			switch (GetRand(1))
			{
			case(0):
				m_bossUpdate = &Boss::NeutralUpdate;
				break;
			case(1):
				m_rigid->AddVelocity(m_upVec * 3);
				m_bossUpdate = &Boss::FullpowerJumpUpdate;
				break;
			}

		}
		else
		{
			m_jumpCount++;
			m_impacts.push_back(std::make_shared<StampImpact>(m_rigid->GetPos() + m_upVec * -kBodyRadiusSize, m_nowPlanet->GetScale(), m_upVec * -1, ObjectTag::Electronic, 0.5f));
			MyEngine::Physics::GetInstance().Entry(m_impacts.back());
			m_rigid->AddVelocity(m_upVec * 2);
		}
		
	}
	
}

void Boss::FullpowerJumpUpdate()
{
	m_color = 0xffff00;
	m_state = State::Attack;

	//ステージに着地したら
	if (m_onColStage)
	{
		PlaySoundMem(m_dropSoundHandle, DX_PLAYTYPE_BACK);

		m_impacts.push_back(std::make_shared<StampImpact>(m_rigid->GetPos() + m_upVec * -kBodyRadiusSize, m_nowPlanet->GetScale(), m_upVec * -1, ObjectTag::Electronic,0.8f));
		MyEngine::Physics::GetInstance().Entry(m_impacts.back());

		//HPが少ないほど隙がなくなる
		m_actionFrame = -m_hp-20;
		m_bossUpdate = &Boss::LandingUpdate;

	}
}

void Boss::TackleUpdate()
{
	
	m_color = 0xffff00;
	m_state = State::Tackle;

	Vec3 targetDir = m_player->GetPos() - m_rigid->GetPos();
	targetDir.Normalize();

	m_tackleChargeFrame++;
	m_rigid->SetVelocity(m_upVec * 0.5f);

	//タックルのチャージが最大になったら
	if (m_tackleChargeFrame > kTackleMaxChargeFrame)
	{
		//タックルを一度もしていなかったら
		if (!m_isTackle)
		{
			m_isTackle = true;
			UI::GetInstance().SetTalkObjectHandle(UI::TalkGraphKind::TakasakiTaisa);
			std::list<std::string> text2;
			text2.push_back("ヤツが突っ込んでくるぞ！");
			text2.push_back("危ないと思ったらスピンで身を守れ！");
			UI::GetInstance().InTexts(text2);
		}
		m_rigid->SetVelocity(targetDir * kTackleSpeed);

		//タックルが終了したら
		if (m_tackleChargeFrame - kTackleMaxChargeFrame > kTackleTime)
		{
			m_tackleChargeFrame = 0;
			//HPが少ないほど隙がなくなる
			m_actionFrame = -m_hp;
			m_bossUpdate = &Boss::LandingUpdate;
		}
	}
}

void Boss::RunningUpdate()
{
	m_color = 0xffff00;
	m_state = State::Running;

	m_sideVec = Cross(m_upVec, m_runningDir);
	m_rigid->SetVelocity(m_sideVec*kRunningSpeed);

	m_runningFrame++;

	//走っている時間が上限の半分になったら方向反転
	if (m_runningFrame > kRunningFrameMax / 2)
	{
		m_rigid->SetVelocity(m_rigid->GetVelocity() * -1);
	}
	//走り終わったら
	if (m_runningFrame > kRunningFrameMax)
	{
		m_runningFrame = 0;
		//HPが少ないほど隙がなくなる
		m_actionFrame = -m_hp;
		m_bossUpdate = &Boss::LandingUpdate;
	}

}

void Boss::LandingUpdate()
{
	m_color = 0x00ff00;
	m_state = State::Land;

	//話していなかったら
	if (!m_isTalk)
	{
		m_isTalk = true;
		UI::GetInstance().SetTalkObjectHandle(UI::TalkGraphKind::Boss);
		UI::GetInstance().InText("はぁはぁ");

		UI::GetInstance().SetNextTalkObjectHandle(UI::TalkGraphKind::TakasakiTaisa);
		UI::GetInstance().InNextText("なんだ、敵の様子がおかしいぞ！");

		std::list<std::string> text1;
		text1.push_back("緑になった時がヤツの防御が低くなった合図だ！");
		text1.push_back("なぜかわからんが、俺の直感がそう言っている。");
		UI::GetInstance().InNextTexts(text1);

		std::list<std::string> text2;
		text2.push_back("敵が緑色になった時に");
		text2.push_back("近づいてスピンで攻撃するか、");
		text2.push_back("L1で狙ってXで遠距離から攻撃するんだ！");
		UI::GetInstance().InNextTexts(text2);

	}

	//行動のクールタイムがマイナスの時はひるんでいる
	m_actionFrame++;
	if (m_actionFrame > 0)
	{
		//怯み状態から戻す
		m_color = 0xff00ff;
		m_bossUpdate = &Boss::NeutralUpdate;
	}
}

void Boss::RunawayUpdate()
{
	Vec3 velo = m_runawayPos - m_rigid->GetPos();

	//目的地に一定距離近づいたら
	if (velo.Length() < 1200&&m_isBattle)
	{
		//戦闘状態から離脱
		SoundManager::GetInstance().ChangeBGM(SoundManager::GetInstance().GetSoundData(kGamePlayBGMName));

		auto obj = GalaxyCreater::GetInstance().GetCollidable(1);
		obj->SetIsActive(true);
		m_isBattle = false;
	}
	//目的地に到達したら
	if (velo.Length() < 15)
	{
		
		m_rigid->SetVelocity(Vec3::Zero());
		m_bossUpdate = &Boss::InitUpdate;
		
	}
	velo.Normalize();
	m_rigid->SetVelocity(velo*3);
	
}

void Boss::BallAttackUpdate()
{
	m_color = 0xffff00;
	m_state = State::Attack;

	m_shotCreateFrame++;

	//if(m_shotCreateFrame>)
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
	//ステージに衝突したら
	if (colider->GetTag() == ObjectTag::Stage)
	{
		m_onColStage = true;
	}

	//ボスが怯み中だったら
	if (m_bossUpdate == &Boss::LandingUpdate)
	{
		//プレイヤーの弾と衝突したら
		if (colider->GetTag() == ObjectTag::PlayerBullet)
		{
			m_isHit = true;
			PlaySoundMem(m_damageSoundHandle, DX_PLAYTYPE_BACK);
			m_hp -= 1;
		}
	}

	//プレイヤーと衝突したら
	if (colider->GetTag() == ObjectTag::Player)
	{
		auto state = GetState();

		//ボスが突進中または走っている時にプレイヤーがスピンしていたら
		if ((state == State::Running || state == State::Tackle) && colider->GetState() == State::Spin)
		{
			PlaySoundMem(m_criticalHandle, DX_PLAYTYPE_BACK);
			Vec3 dir = colider->GetRigidbody()->GetPos();
			dir.Normalize();
			m_rigid->SetVelocity((dir + m_upVec) * 2);
			//HPが少ないほど隙がなくなる
			m_actionFrame = -m_hp-200;
			m_bossUpdate = &Boss::LandingUpdate;
		}

		//ボスが怯み中にプレイヤーがスピンしていたら
		if (state == State::Land && colider->GetState() == State::Spin)
		{
			m_rigid->SetVelocity(m_upVec * 2);
			m_hp -= 20;
			m_color = 0xff00ff;
			m_bossUpdate = &Boss::NeutralUpdate;
			PlaySoundMem(m_criticalHandle, DX_PLAYTYPE_BACK);
		}
	}
}

void Boss::OnCollideStay(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
	//ステージに衝突している間
	if (colider->GetTag() == ObjectTag::Stage)
	{
		m_onColStage = true;
	}

	//ボスが怯み中だったら
	if (m_bossUpdate == &Boss::LandingUpdate)
	{
		//プレイヤーの弾と衝突している間
		if (colider->GetTag() == ObjectTag::PlayerBullet)
		{
			m_isHit = true;
			PlaySoundMem(m_damageSoundHandle, DX_PLAYTYPE_BACK);
			m_hp -= 1;
		}
	}

	//プレイヤーと衝突している間
	if (colider->GetTag() == ObjectTag::Player)
	{
		auto state = GetState();

		//ボスが突進中または走っている時にプレイヤーがスピンしていたら
		if ((state == State::Running || state == State::Tackle) && colider->GetState() == State::Spin)
		{
			PlaySoundMem(m_criticalHandle, DX_PLAYTYPE_BACK);
			Vec3 dir = colider->GetRigidbody()->GetPos();
			dir.Normalize();
			m_rigid->SetVelocity((dir + m_upVec) * 2);
			//HPが少ないほど隙がなくなる
			m_actionFrame = -m_hp - 200;
			m_bossUpdate = &Boss::LandingUpdate;
		}

		//ボスが怯み中にプレイヤーがスピンしていたら
		if (state == State::Land && colider->GetState() == State::Spin)
		{
			m_rigid->SetVelocity(m_upVec * 2);
			m_hp -= 20;
			m_color = 0xff00ff;
			m_bossUpdate = &Boss::NeutralUpdate;
			PlaySoundMem(m_criticalHandle, DX_PLAYTYPE_BACK);
		}
	}
}

void Boss::OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
	//重力圏に入ったら
	if (colider->GetTag() == ObjectTag::Stage)
	{
		m_nowPlanet = std::dynamic_pointer_cast<Planet>(colider);
	}

	//ボスが怯み中だったら
	if (m_bossUpdate == &Boss::LandingUpdate)
	{
		//プレイヤーの衝撃波と衝突したら
		if (colider->GetTag() == ObjectTag::PlayerImpact)
		{
			m_rigid->SetVelocity(m_upVec * 2);
			m_hp -= 20;
			m_color = 0xff00ff;
			m_bossUpdate = &Boss::NeutralUpdate;
			PlaySoundMem(m_criticalHandle, DX_PLAYTYPE_BACK);
		}

		//プレイヤーの弾と衝突したら
		if (colider->GetTag() == ObjectTag::PlayerBullet)
		{
			m_hp -= 1;
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


