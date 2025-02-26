#include "SeekerLine.h"
#include"Player.h"
#include"Easing.h"
#include"ColliderSphere.h"

#include"SoundManager.h"
#include"EffectManager.h"
#include"ScreenManager.h"

#include"Game.h"

namespace
{
	constexpr int kSpeedMax = 2;
	constexpr int kRadius = 5;

	constexpr float kInitialSpeed = 0.02f;
	
	const char* kEndPointSEName = "SeekerLineEndSE.mp3";
	const char* kPlayerMoveSEName = "Fastener.mp3";

	const char* kLineTraceName = "Line.efk";

	const char* kGaussFilterScreenName = "GaussOfSeekerLine";
	const char* kHighBrightScreenName = "HighBrightSeekerLine";

}


float EaseInOutNum(float x)
{
	return -1.f * (cos(x * DX_PI_F) - 1.f) / 2.f;
}


SeekerLine::SeekerLine(std::vector<Vec3> points, int color, bool isActive) : Collidable(Priority::StageGimmick, ObjectTag::SeekerLine),
m_speed(0),
m_length(0),
m_color(color),
m_hitPointNum(0),
m_lineEffectIndex(0),
m_totalPlayerMoveNum(0),
m_endPointSoundHandle(-1),
m_movePlayerSoundHandle(-1)
{
	//重力の影響を受けない
	SetAntiGravity();
	SetIsActive(isActive);

	for (auto& point : points)
	{
		m_points.push_back(point);
	}

	//初期位置設定と当たり判定の追加
	m_rigid->SetPos(m_points.front());
	AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);
	auto item = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
	item->radius = kRadius;
	item->m_isTrigger = true;
	
	//シーカーラインの全長を計算
	for (int i = 1; i < m_points.size(); i++)
	{
		m_length += (m_points[i-1] - m_points[i]).Length();
	}

	m_speed = 1.f;
	m_isIgnore = true;

	//Soundのロード
	m_endPointSoundHandle = SoundManager::GetInstance().GetSoundData(kEndPointSEName);
	m_movePlayerSoundHandle = SoundManager::GetInstance().GetSoundData(kPlayerMoveSEName);

	m_gaussFilterScreen=ScreenManager::GetInstance().GetScreenData(kGaussFilterScreenName,Game::kScreenWidth,Game::kScreenHeight);
	m_highBrightScreen = ScreenManager::GetInstance().GetScreenData(kHighBrightScreenName, Game::kScreenWidth, Game::kScreenHeight);
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
	m_totalPlayerMoveNum += m_speed;
	
	//移動総量が折り返し地点に来たら
	if (m_totalPlayerMoveNum > m_length/2)
	{
		m_speed *= -1 ;
	}

	//プレイヤーからシーカーラインの次の地点まで向かうベクトルを生成
	m_velocity = m_points[m_hitPointNum+1] - m_player.lock()->GetPos();
	m_velocity.Normalize();
	
	//プレイヤーをシーカーラインの進捗度合い分の位置に移動
	m_player.lock()->SetPos(m_player.lock()->GetPos() + m_velocity * m_totalPlayerMoveNum);
	
	//プレイヤーを徐々に加速させる
	m_player.lock()->SetVelocity(m_velocity * m_totalPlayerMoveNum);

	EffectManager::GetInstance().SetPositionEffect(kLineTraceName,m_lineEffectIndex, m_player.lock()->GetPos(), MGetIdent());
	PlaySoundMem(m_movePlayerSoundHandle, DX_PLAYTYPE_BACK);
	
	//次のポイントに到達したら
	if ((m_player.lock()->GetPos() - m_points[m_hitPointNum + 1]).Length() <= 5)
	{
		//次のポイントが最後だったら
		m_hitPointNum++;
		if (m_points[m_hitPointNum] == m_points.back())
		{
			EffectManager::GetInstance().StopEffect(kLineTraceName, m_lineEffectIndex);
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

	auto camerapos = GetCameraPosition();
	auto cameraTarget = GetCameraTarget();
	auto cameraUpVec = GetCameraUpVector();
	auto cameraMat = GetCameraViewMatrix();
	auto cameraNear = GetCameraNear();
	auto cameraFar = GetCameraFar();

	DrawSphere3D(m_points[0].VGet(), kRadius, 8, 0xffff00, 0xffffff, false);
	// 通常の描画結果を書き込むスクリーンを描画対象にする
	SetDrawScreen(m_gaussFilterScreen);
	// 画面をクリア
	ClearDrawScreen();

	SetCameraPositionAndTargetAndUpVec(camerapos, cameraTarget, cameraUpVec);
	for (int i = 0; i < m_points.size(); i++)
	{
		if (i <= 0)continue;
		DrawLine3D((m_points.begin() + i-1)->VGet(), (m_points.begin() + i)->VGet(), m_color);
		if ((m_points.begin() + i) == m_points.end())return;
	}
	
	GraphFilterBlt(m_gaussFilterScreen, m_highBrightScreen, DX_GRAPH_FILTER_GAUSS, 16, 200);
	GraphFilterBlt(m_highBrightScreen, DX_SCREEN_BACK, DX_GRAPH_FILTER_GAUSS, 16, 900);

	// 描画対象を裏画面にする
	SetDrawScreen(DX_SCREEN_BACK);

	SetCameraPositionAndTargetAndUpVec(camerapos, cameraTarget, cameraUpVec);
	SetCameraNearFar(cameraNear, cameraFar);

	DrawGraph(0, 0, m_highBrightScreen, false);
}

void SeekerLine::OnTriggerEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag)
{
	if (colider->GetTag() == ObjectTag::Player)
	{
		//初期化
		m_totalPlayerMoveNum = 0;
		m_speed = kInitialSpeed;
		m_player = std::dynamic_pointer_cast<Player>(colider);
		bool operate = m_player.lock()->GetOperationFlag();
		
		//一回のみとおる
		if (!operate)
		{
			m_lineEffectIndex = EffectManager::GetInstance().PlayEffect(kLineTraceName, true, 0, shared_from_this());

		}
		m_player.lock()->SetIsOperation(true);
		m_player.lock()->SetPos(m_rigid->GetPos());	
	}
}

