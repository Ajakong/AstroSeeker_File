#include "Booster.h"
#include"ColliderSphere.h"
#include"Player.h"
#include"SoundManager.h"
#include"EffectManager.h"
#include"ScreenManager.h"
#include"Physics.h"
#include"Easing.h"
#include"ModelManager.h"
namespace
{
	const char* kGaussScreenName = "Gauss";
	const char* kColorScreenName = "Color";
	const char* kBrightScreenName = "HighBright";

	constexpr int kRadius = 2;
	constexpr float kScale = 0.01f;

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
}

namespace
{
	const char* name = "Booster";
	const char* effectname = "warpEffect.efk";
}

Booster::Booster(Vec3 pos, Vec3 Dir, int handle, float power, bool isActive) :Collidable(Priority::StageGimmick, ObjectTag::WarpGate),
m_power(power)
{
	m_isActive = isActive;
	m_dir = Dir;
	SetAntiGravity();
	AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);//ここで入れたのは重力の影響範囲
	auto item = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
	item->radius = kRadius;
	m_rigid->SetPos(pos);

	m_gaussScreenHandle = ScreenManager::GetInstance().GetScreenData(kGaussScreenName, 1600, 900);
	m_colorScreenHandle = ScreenManager::GetInstance().GetScreenData(kColorScreenName, 1600, 900);

	m_modelHandle = ModelManager::GetInstance().GetModelData(name);

	MV1SetScale(m_modelHandle, VGet(kScale, kScale, kScale));

	m_currentAnimNo = 0;
	ChangeAnim(m_currentAnimNo);
}

Booster::~Booster()
{
	
}

void Booster::Init()
{
	m_sideVec = Cross(m_upVec, m_dir);
	m_sideVec.Normalize();
	MyEngine::Physics::GetInstance().Initialize(shared_from_this());
	SetIsActive(m_isActive);
}

void Booster::Update()
{

	UpdateAnim(m_currentAnimNo);

	MV1SetPosition(m_modelHandle, m_rigid->GetPos().VGet());
}

void Booster::SetEffectPos()
{
	m_upVec = (m_warpPos - m_rigid->GetPos()).GetNormalized();

	MATRIX mat;

	Vec3 axis = Cross(Vec3::Up(), m_upVec);
	mat = MGetRotAxis(axis.VGet(), acos(Dot(Vec3::Up(), m_upVec)));

	mat = MMult(mat, MGetTranslate(Vec3(m_rigid->GetPos() + m_upVec * 20).VGet()));
}

void Booster::Draw()
{
	//m_screenHandle=GetDrawScreen();
	//SetDrawScreen(m_colorScreenHandle);
	//ClearDrawScreen();
	//DrawLine3D(m_rigid->GetPos().VGet(),(m_dir*6 + m_rigid->GetPos()).VGet(), 0xff00ff);

	//GraphFilterBlt(m_colorScreenHandle, m_gaussScreenHandle, DX_GRAPH_FILTER_GAUSS, 16,900);
	//
	//SetDrawScreen(m_screenHandle);

	//// 描画ブレンドモードを加算にする
	//SetDrawBlendMode(DX_BLENDMODE_ADD, 255);

	//
	//DrawExtendGraph(0, 0, 1600, 900, m_gaussScreenHandle,true);
	//DrawExtendGraph(0, 0, 1600, 900, m_gaussScreenHandle, true);

	//// 描画ブレンドモードをブレンド無しに戻す
	//SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

	/*DrawSphere3D(m_rigid->GetPos().VGet(), kRadius, 7, 0xff00ff, 0x0000ff, false);
	DrawCube3D(Vec3(m_rigid->GetPos() + Vec3(2, 2, 2)).VGet(), Vec3(m_rigid->GetPos() + Vec3(-2, -2, -2)).VGet(), 0xffff00, 0xffff00, false);
	DrawLine3D((m_rigid->GetPos()).VGet(), (m_rigid->GetPos() + m_upVec * 50).VGet(), 0xffffff);*/

	MV1DrawModel(m_modelHandle);
}

void Booster::OnCollideEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag)
{
	if (colider->GetTag() == ObjectTag::Stage)
	{
		m_nowPlanetPos = colider->GetRigidbody()->GetPos();
	}
	if (colider->GetTag() == ObjectTag::Player)
	{
		PlaySoundMem(SoundManager::GetInstance().GetSoundData("boost.mp3"), DX_PLAYTYPE_BACK);

		//colider->GetRigidbody()->SetVelocity(Vec3(m_rigid->GetPos()-colider->GetRigidbody()->GetPos()).GetNormalized()*15);

		colider->GetRigidbody()->SetVelocity(m_dir * m_power);
		auto player = std::dynamic_pointer_cast<Player>(colider);
		player->SetPos(m_rigid->GetPos());
		player->SetBoostUpdate();
		player->SetBoost(m_sideVec);
		SetIsActive(false);
	}
}

void Booster::OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
	if (colider->GetTag() == ObjectTag::Stage)
	{
		m_upVec = m_rigid->GetPos() - colider->GetRigidbody()->GetPos();
		m_upVec.VGet();

	}
}

bool Booster::UpdateAnim(int attachNo)
{
	//アニメーションが設定されていないので終了
	if (attachNo == -1) return false;

	//アニメーションを進行させる
	float now = MV1GetAttachAnimTime(m_modelHandle, attachNo);//現在の再生カウント
	now += kAnimFrameSpeed * 72 / kFrameParSecond;//アニメーションカウントを進める


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
void Booster::ChangeAnim(int animIndex)
{
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


