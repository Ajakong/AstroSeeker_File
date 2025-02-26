#include"Game.h"
#include"UI.h"
#include "DebugGalaxy.h"
#include"Player.h"
#include"Takobo.h"
#include"Boss.h"
#include"SamuraiAlter.h"
#include"Camera.h"
#include"BoxPlanet.h"
#include"SpherePlanet.h"
#include"PolygonModelPlanet.h"
#include"Physics.h"
#include"ModelManager.h"
#include"SeekerLine.h"

using namespace std;
using namespace World;

namespace
{
	//UI
	//ミッション
	constexpr int kUiText_SrkX = 150;
	constexpr int kUiText_SrkY = 170;
	constexpr int kUiText_Width = 1050;
	constexpr int kUiText_Height = 450;
	constexpr float kUiText_Exrate = 0.2f;

	//HPバーの枠
	constexpr int kUiHpbarFrame_PosX = 150;
	constexpr int kUiHpbarFrame_PosY = 35;
	constexpr int kUiHpbarFrame_SrkX = 2600;
	constexpr int kUiHpbarFrame_SrkY = 2560;
	constexpr int kUiHpbarFrame_Width = 1000;
	constexpr int kUiHpbarFrame_Height = 144;
	constexpr float kUiHpbarFrame_Exrate = 0.3f;

	//ChargeRemainTimeのバー
	constexpr int kUiCRT_PosX = 20;
	constexpr int kUiCRT_PosY = 70;
	constexpr int kUiCRT_SrkX = 2655;
	constexpr int kUiCRT_SrkY = 2720;
	constexpr int kUiCRT_Width = 30;
	constexpr int kUiCRT_DisX = 9;
	constexpr int kUiCRT_Height = 80;
	constexpr float kUiCRT_Exrate = 0.3f;

	//HPバー
	constexpr int kUiHpbar_PosX = 15;
	constexpr int kUiHpbar_PosY = 25;
	constexpr int kUiHpbar_Height = 23;
	constexpr float kUiHpbar_mag = 5.35f;//HPに掛けたらいい感じになる倍率

	//タイマーの枠
	constexpr int kUiTimeCountFrame_PosX = 1400;
	constexpr int kUiTimeCountFrame_PosY = 100;
	constexpr int kUiTimeCountFrame_SrkX = 1280;
	constexpr int kUiTimeCountFrame_SrkY = 130;
	constexpr int kUiTimeCountFrame_Width = 1040;
	constexpr int kUiTimeCountFrame_Height = 410;
	constexpr float kUiTimeCountFrame_Exrate = 0.3f;

	//タイマー
	constexpr int kUiTimeCount_PosX = 1350;
	constexpr int kUiTimeCount_PosY = 90;

	//カメラ
	constexpr float kCameraDistanceFront = 80.f;
	constexpr float kCameraDistanceAddFrontInJump = 3.f;
	constexpr float kCameraDistanceUp = 50.f;


	const char* kMiniMapScreenName = "MiniMap";
}

DebugGalaxy::DebugGalaxy(std::shared_ptr<Player> playerPointer) : Galaxy(playerPointer),
m_managerUpdate(nullptr),
m_managerDraw(nullptr)
{
	m_ui = make_shared<UI>();
	player = playerPointer;
	m_planet.push_back(make_shared<BoxPlanet>(Vec3(0, -50, 0), 0x00ffff, 1.f, Vec3(30, 30, 50)));

	std::vector<Vec3>points;
	points.push_back(Vec3(0, 0, 10));
	points.push_back(Vec3(0, 10, 20));
	points.push_back(Vec3(0, 20, 10));
	points.push_back(Vec3(0, 10, 0));
	points.push_back(Vec3(0, 0, 10));

	m_seekerLine.push_back(make_shared<SeekerLine>(points, 0xff0000, true));
	MyEngine::Physics::GetInstance().Entry(m_seekerLine.back());
	
	//m_planet.push_back(make_shared<SpherePlanet>(Vec3(0, -50, 0),0x00ff00, 1, 1));
	//m_planet.push_back(make_shared<PolygonModelPlanet>(ModelManager::GetInstance().GetModelData("MechSpiderM1.mv1"), Vec3(0, -400, 0), 1, 1, 400.f));
	//m_planet.push_back(make_shared<PolygonModelPlanet>(ModelManager::GetInstance().GetModelData("MechSpiderM1.mv1"), Vec3(0, -400, 0), 1, 1, 400.f));
	//m_planet.push_back(make_shared<PolygonModelPlanet>(ModelManager::GetInstance().GetModelData("UFO_GreenMan.mv1"), Vec3(0, -400, 0), 1, 1, 10.f));
	//takobo.push_back(make_shared<Takobo>(Vec3(0, 0, -30),player));

	/*boss.push_back(make_shared<Boss>(Vec3(0, 50, 100)));
	MyEngine::Physics::GetInstance().Entry(boss.back());*/
	
	//samuraiAlter.push_back(make_shared<SamuraiAlter>(Vec3(0, 0, -30)));
	//MyEngine::Physics::GetInstance().Entry(samuraiAlter.back());
	camera = make_shared<Camera>();
	for (auto& item : m_planet)
	{
		MyEngine::Physics::GetInstance().Entry(item);//物理演算クラスに登録
	}
	MyEngine::Physics::GetInstance().Entry(player);//物理演算クラスに登録
	
	//それぞれのオブジェクトの上方向ベクトルなどの更新
	MyEngine::Physics::GetInstance().Update();
}

DebugGalaxy::~DebugGalaxy()
{
}

void DebugGalaxy::Init()
{
	//MyEngine::Physics::GetInstance().Entry(player);//物理演算クラスに登録

	//for (auto& item : m_planet)
	//{
	//	item->Init();
	//	MyEngine::Physics::GetInstance().Entry(item);//物理演算クラスに登録
	//}

	SetGlobalAmbientLight(GetColorF(1.0f, 1.0f, 1.0f, 1.0f));

	player->SetMatrix();//モデルに行列を反映

	// 深度値記録バッファ用RT
	DxLib::SetCreateGraphChannelBitDepth(32);
	DxLib::SetCreateDrawValidGraphChannelNum(1);
}

void DebugGalaxy::Update()
{
	camera->SetEasingSpeed(player->GetCameraEasingSpeed());
	if (player->GetIsAiming())camera->Update(player->GetShotDir());
	else camera->Update(player->GetLookPoint());

	//Vec3 planetToPlayer = player->GetPos() - player->GetNowPlanetPos();
	Vec3 sideVec = player->GetSideVec();
	Vec3 front = player->GetFrontVec();//-1をかけて逆ベクトルにしている

	//相対的な軸ベクトルの設定

	camera->Setting(player->GetBoostFlag(),player->GetIsAiming());
	//本当はカメラとプレイヤーの角度が90度以内になったときプレイヤーの頭上を見たりできるようにしたい。
	camera->SetUpVec(player->GetNormVec());

	//PlayerがBoost中の時は
	if (player->GetBoostFlag())
	{
		//カメラの位置を遠ざける
		camera->SetCameraPoint(player->GetPos() + player->GetNormVec().GetNormalized() * (kCameraDistanceUp - 40) - front * ((kCameraDistanceFront - 70) + kCameraDistanceAddFrontInJump * player->GetJumpFlag()));
	}
	//そうではないとき
	else
	{
		//もしエイム中なら
		if (player->GetIsAiming())
		{
			///カメラの位置をPlayerの近くに
			camera->SetCameraPoint(player->GetPos() + player->GetShotDir() * -5 + player->GetNormVec() * 8 + player->GetSideVec() * 2);
		}
		else
		{
			//そうでもないなら通常モード
			camera->SetCameraPoint(player->GetPos() + player->GetNormVec().GetNormalized() * kCameraDistanceUp - front * (kCameraDistanceFront + kCameraDistanceAddFrontInJump * player->GetJumpFlag()));
		}
	}


	MyEngine::Physics::GetInstance().Update();//当たり判定の更新

	player->SetMatrix();//行列を反映

}

void DebugGalaxy::Draw()
{
	for (auto& item : m_planet)

	{
		item->SetIsSearch(player->IsSearch());
	}

	MyEngine::Physics::GetInstance().Draw();


	if (player->IsSearch())
	{
		DxLib::SetDrawBlendMode(DX_BLENDMODE_MUL, 255);
		// ちょっと暗い矩形を描画
		DxLib::DrawBox(0, 0, 1600, 900,
			0x444488, true);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	int alpha = static_cast<int>(255 * (static_cast<float>(player->GetDamageFrame()) / 60.0f));
#ifdef DEBUG
	Vec3 UIPos = ((Vec3(GetCameraPosition()) + Vec3(GetCameraFrontVector()) * 110) + Vec3(GetCameraLeftVector()) * -70 + Vec3(GetCameraUpVector()) * 37);
	DrawLine3D(UIPos.VGet(), Vec3(UIPos + Vec3::Up() * 20).VGet(), 0xff0000);
	DrawLine3D(UIPos.VGet(), Vec3(UIPos + Vec3::Right() * 20).VGet(), 0x00ff00);
	DrawLine3D(UIPos.VGet(), Vec3(UIPos + Vec3::Front() * 20).VGet(), 0x0000ff);

	DrawCircle(200, 500, 100, 0xffff00, 0);
	DrawLine(200, 500, static_cast < int>(200 + player->GetInputVec().x * 70), static_cast<int>(500 + player->GetInputVec().z * 70), 0xff0000);
	DrawLine(200, 500, static_cast < int> (200 + player->GetPostMoveDir().x * 70), static_cast < int>(500 + player->GetPostMoveDir().z * 70), 0x0000ff);
	DrawCircle(static_cast < int>(200 + player->GetInputVec().x * ((player->GetInputVec().Length() * 70.f))), static_cast < int>( 500 + player->GetInputVec().z * ((player->GetInputVec().Length() * 70.f))), 30, 0xffff00, 0);

#endif

	DxLib::SetDrawBlendMode(DX_BLENDMODE_MULA, alpha);
	DxLib::DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0xff4444, true);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawFormatString(0, 25 * 0, 0xffffff, "HP:%f", player->GetHp());

	DrawFormatString(0, 25 * 1, 0xffffff, "UpVec(%f,%f,%f)", player->GetUpVec().x, player->GetUpVec().y, player->GetUpVec().z);

	DrawFormatString(0, 25 * 2, 0xffffff, "FrontVec(%f,%f,%f)", player->GetFrontVec().x, player->GetFrontVec().y, player->GetFrontVec().z);
	DrawFormatString(0, 25 * 3, 0xffffff, "SideVec(%f,%f,%f)", player->GetSideVec().x, player->GetSideVec().y, player->GetSideVec().z);
	DrawFormatString(0, 25 * 4, 0xffffff, "shotDir(%f,%f,%f)", player->GetShotDir().x, player->GetShotDir().y, player->GetShotDir().z);


	DrawFormatString(0, 25 * 6, 0xffffff, "PlayerPos(%f,%f,%f)", player->GetPos().x, player->GetPos().y, player->GetPos().z);
	DrawFormatString(0, 25 * 7, 0xffffff, player->GetStateName().c_str());
	DrawFormatString(0, 25 * 8, 0xffffff, "EasingSpeed:%f", player->GetCameraEasingSpeed());
	DrawFormatString(0, 25 * 9, 0xffffff, "FootNowOnHit:%d", player->GetFootOnHit());
	DrawFormatString(0, 25 * 10, 0xffffff, "PlayerVelocity(%f,%f,%f)", player->GetRigidbody()->GetVelocity().x, player->GetRigidbody()->GetVelocity().y, player->GetRigidbody()->GetVelocity().z);


	auto cameraPos = GetCameraPosition();

	/*SetDrawScreen(m_modelScreenHandle);

	SetCameraNearFar(1.f, 10000.f);
	SetCameraPositionAndTarget_UpVecY((player->GetPos() + Vec3::Left() * -10 + Vec3::Front() * -10 + Vec3::Up() * 10).VGet(), (player->GetPos()).VGet());
	ClearDrawScreen();
	player->Draw();
	SetDrawScreen(DX_SCREEN_BACK);*/



	//SetScreenFlipTargetWindow(NULL);

	//camera->Set();
	//ScreenFlip();

	//// 少し時間の経過を待つ
	//WaitTimer(2);

	//ClearDrawScreen();

	//DrawRotaGraph(800, 450, 1.0f, 0, m_modelScreenHandle, true);

}
