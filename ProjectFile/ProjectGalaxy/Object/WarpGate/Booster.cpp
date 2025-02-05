#include "Booster.h"
#include"ColliderSphere.h"
#include"Player.h"
#include"SoundManager.h"
#include"EffectManager.h"
#include"ScreenManager.h"
#include"Physics.h"
#include"Easing.h"
namespace
{
	const char* kGaussScreenName = "Gauss";
	const char* kColorScreenName = "Color";
	const char* kBrightScreenName = "HighBright";

	constexpr int kRadius = 2;
}

namespace
{
	const char* name = "booster";
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
	DrawLine3D(m_rigid->GetPos().VGet(),(m_dir*6 + m_rigid->GetPos()).VGet(), 0xff00ff);

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

	DrawSphere3D(m_rigid->GetPos().VGet(), kRadius, 7, 0xff00ff, 0x0000ff, false);
	DrawCube3D(Vec3(m_rigid->GetPos() + Vec3(2, 2, 2)).VGet(), Vec3(m_rigid->GetPos() + Vec3(-2, -2, -2)).VGet(), 0xffff00, 0xffff00, false);
	DrawLine3D((m_rigid->GetPos()).VGet(), (m_rigid->GetPos() + m_upVec * 50).VGet(), 0xffffff);
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
		player->m_playerUpdate = &Player::BoostUpdate;
		player->SetBoost(m_sideVec);
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

