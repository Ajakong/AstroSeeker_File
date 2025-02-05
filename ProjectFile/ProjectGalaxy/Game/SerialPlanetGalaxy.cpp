#include "SerialPlanetGalaxy.h"
#include"DxLib.h"
// EffekseerForDXLib.hをインクルードします。
#include "EffekseerForDXLib.h"
#include"Camera.h"
#include"UI.h"
#include"Physics.h"
#include"Player.h"
#include"WarpGate.h"
#include"Booster.h"
#include"StarCapture.h"
#include"SeekerLine.h"
#include"Crystal.h"
#include"BossPlanet.h"
#include"BoxPlanet.h"
#include"PolygonModelPlanet.h"
#include"Takobo.h"
#include"KillerTheSeeker.h"
#include"SpaceEmperor.h"
#include"Boss.h"
#include"Gorori.h"
#include"Kuribo.h"
#include"Item.h"
#include"Cannon.h"
#include"StickStarItem.h"
#include"FullPowerDropItem.h"
#include"Coin.h"
#include"Key.h"
#include"ClearObject.h"
#include<cassert>
#include"Pad.h"
#include"SoundManager.h"
#include"GraphManager.h"
#include"FontManager.h"
#include"ScreenManager.h"
#include"ModelManager.h"
#include"GalaxyCreater.h"
#include"Game.h"

#include"Mission.h"

#include"TalkObject.h"
#include"DekaHead_Red.h"
#include"DekaHead_Blue.h"
#include"DekaHead_Green.h"


using namespace std;

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
	constexpr float kCameraDistanceFront = 30.f;
	constexpr float kCameraDistanceAddFrontInJump = 15.f;
	constexpr float kCameraDistanceUp = 30.f;

	const char* kModelScreenName = "ModelScreen";


	constexpr float kGravityRange = 150.f;
}

SerialPlanetGalaxy::SerialPlanetGalaxy(std::shared_ptr<Player> playerPointer) : Galaxy(playerPointer),
m_bgmHandle(SoundManager::GetInstance().GetSoundData("WarOfAstron_Intro.mp3")),
m_bossBattleBgmHandle(SoundManager::GetInstance().GetSoundData("SpaceEmperor_battle.mp3")),
m_warpEffectHandle(-1)
{
	m_camera = make_shared<Camera>();
	m_camera->SetCameraPoint(player->GetPos() + player->GetNormVec().GetNormalized() * kCameraDistanceUp - player->GetFrontVec() * (kCameraDistanceFront));
	m_camera->SetAimCamera(Vec3(0, 0, 0));
	MyEngine::Physics::GetInstance().Entry(player);//物理演算クラスに登録

	GalaxyCreater::GetInstance().SetCamera(m_camera);
	
	//惑星の配置
	GalaxyCreater::GetInstance().PlanetCreate();
	//敵の配置
	m_enemies=GalaxyCreater::GetInstance().EnemyCreate(player);
	//その他オブジェクトの配置
	GalaxyCreater::GetInstance().ObjectCreate(player);
	//その他オブジェクトの配置
	GalaxyCreater::GetInstance().LockedObjectCreate();
	m_keyLockEnemies=GalaxyCreater::GetInstance().KeyLockObjectCreate();
	GalaxyCreater::GetInstance().TalkObjectCreate();

#ifdef _DEBUG

	//オブジェクトやギミックの配置(のちのちUnityのデータを読み込んで配置するので今は仮配置)

	////ギミック
	////ブースター

	//m_warpGate.push_back(make_shared<WarpGate>(Vec3(0, -50, -70), Vec3(-200, -300, 0), -1));
	//MyEngine::Physics::GetInstance().Entry(m_warpGate.back());
	//スターキャプチャー
	//m_starCapture.push_back(make_shared<StarCapture>(Vec3(0, 50, 40)));
	//MyEngine::Physics::GetInstance().Entry(m_starCapture.back());

#endif

	m_skyDomeH = ModelManager::GetInstance().GetModelData("Skybox");

	MV1SetScale(m_skyDomeH, VGet(1.3f, 1.3f, 1.3f));

	m_managerUpdate = &SerialPlanetGalaxy::GamePlayingUpdate;
	m_managerDraw = &SerialPlanetGalaxy::GamePlayingDraw;

	for (auto& item : m_item)
	{
		MyEngine::Physics::GetInstance().Entry(item);
	}
	//それぞれのオブジェクトの上方向ベクトルなどの更新
	MyEngine::Physics::GetInstance().Update();

	m_modelScreenHandle = ScreenManager::GetInstance().GetScreenData(kModelScreenName, Game::kScreenWidth, Game::kScreenHeight);
	std::list<std::string> m_texts;
	

	UI::GetInstance().Init();
	UI::GetInstance().SetTalkObjectHandle(UI::TalkGraphKind::TakasakiTaisa);
	UI::GetInstance().InText("よし、現地に着いたようだな。ドレイク");
	
	std::list<std::string> texts1;
	texts1.push_back("そういえばドレイク、貴様は軍から抜けて長いだろう");
	texts1.push_back("体がなまってるんじゃないか？");


	UI::GetInstance().InTexts(texts1);

	std::list<std::string> mission;
	mission.push_back("鍛えなおしてやる");
	mission.push_back("まずは左スティックを動かして歩いてみろ");
	UI::GetInstance().InTexts(mission);

	std::list<std::string> panparn;
	panparn.push_back("赤いパンパーンには都合上、まだ話しかけるなよ");
	UI::GetInstance().InTexts(panparn);



	

}

SerialPlanetGalaxy::~SerialPlanetGalaxy()
{
	m_planet.clear();
	m_enemies.clear();
	m_keyLockEnemies.clear();
	m_poworStone.clear();
	m_warpGate.clear();
	m_talkObjects.clear();

	
}

void SerialPlanetGalaxy::Init()
{
	ChangeVolumeSoundMem(100, m_bgmHandle);
	//PlaySoundMem(m_bgmHandle, DX_PLAYTYPE_LOOP);
	SetGlobalAmbientLight(GetColorF(1.0f, 1.0f, 1.0f, 1.0f));

	player->SetMatrix();//モデルに行列を反映

	// 深度値記録バッファ用RT
	DxLib::SetCreateGraphChannelBitDepth(32);
	DxLib::SetCreateDrawValidGraphChannelNum(1);
	//for (auto& item : m_planet)
	//{
	//	item->Init();
	//}
	//
	//for (auto& item : m_booster)item->Init();
	//
	//for (auto& item : m_warpGate)item->Init();
	//for (auto& item : m_seekerLine) { item->Init(); }
	//for (auto& item : m_crystal) { item->Init(); }
	////エネミー
	//for (auto& item : m_kuribo) { item->Init(); }
	//for (auto& item : m_coin)item->Init();
}

void SerialPlanetGalaxy::Update()
{
	Mission::GetInstance().UpDate();
	(this->*m_managerUpdate)();
	
}

void SerialPlanetGalaxy::Draw()
{
	(this->*m_managerDraw)();
}

void SerialPlanetGalaxy::IntroUpdate()
{
}

void SerialPlanetGalaxy::IntroDraw()
{
}

void SerialPlanetGalaxy::GamePlayingUpdate()
{
	//相対的な軸ベクトルの設定

	//player->SetUpVec(planetToPlayer);

	//本当はカメラとプレイヤーの角度が90度以内になったときプレイヤーの頭上を見たりできるようにしたい。
	m_camera->SetUpVec(player->GetUpVec());
	//Vec3 planetToPlayer = player->GetPos() - player->GetNowPlanetPos();
	Vec3 sideVec = player->GetSideVec();
	Vec3 front = player->GetFrontVec();//-1をかけて逆ベクトルにしている
	m_camera->Setting(player->GetBoostFlag(), player->GetIsAiming());

	if (player->GetBoostFlag())
	{
		m_camera->SetCameraPoint(player->GetPos() + player->GetNormVec().GetNormalized() * (kCameraDistanceUp - 40) - front * ((kCameraDistanceFront - 70) + kCameraDistanceAddFrontInJump * player->GetJumpFlag()));
		//m_camera->SetCameraPoint(player->GetPos() + Vec3::Left() *30);
	}
	else
	{
		Vec3 upVec = player->GetNormVec().GetNormalized();
		if (player->GetIsAiming())
		{
			m_camera->SetCameraPoint(player->GetPos() + player->GetShotDir() * -5 + player->GetNormVec() * 8 + player->GetSideVec() * 2);
		}
		/*if (!player->GetJumpFlag())
		{
			m_camera->SetCameraPoint(player->GetPos() + upVec * kCameraDistanceUp - front * kCameraDistanceFront);
		}*/
		else
		{
			m_camera->SetCameraPoint(player->GetPos() + player->GetUpVec()* kCameraDistanceUp - front * (kCameraDistanceFront + kCameraDistanceAddFrontInJump * player->GetJumpFlag()));
			//m_camera->SetCameraPoint(player->GetPos() + Vec3::Left() * 30);
		}
	}


	if(m_camera->m_cameraUpdate!=&Camera::WatchThisUpdate)m_camera->SetEasingSpeed(player->GetCameraEasingSpeed());
	if (player->GetIsAiming())m_camera->Update(player->GetShotDir());
	else m_camera->Update(player->GetLookPoint());

	userData->dissolveY = player->GetRegenerationRange();//シェーダー用プロパティ

	if (player->GetDeathFlag())
	{
		m_isGameOverFlag = true;
	}
	if (player->IsClear())
	{
		m_isClearFlag = true;
	}
	
	MyEngine::Physics::GetInstance().Update();
	player->SetMatrix();//行列を反映
	for (auto& item : m_enemies) { item->SetMatrix(); }
	for (auto& item : m_keyLockEnemies) { item->SetMatrix(); }
	for (auto& item : m_talkObjects) { item->SetMatrix(); }
	for (auto& item : m_cannon) { item->SetMatrix(); }
	//敵の削除管理
	DeleteObject(m_enemies);
	DeleteObject(m_item);
    /*for (auto& item : m_kuribo) { item->SetMatrix(); }
	for (auto& item : m_takobo) { item->SetMatrix(); }
	for (auto& item : m_spaceEmperor) { item->SetMatrix(); }
	DeleteObject(m_kuribo);
	DeleteObject(m_takobo);
	DeleteObject(m_boss);*/

}

void SerialPlanetGalaxy::BossBattleUpdate()
{

}

void SerialPlanetGalaxy::GamePlayingDraw()
{
	DxLib::MV1DrawModel(m_skyDomeH);

	MyEngine::Physics::GetInstance().Draw();


	for (auto& item : m_planet)
	{
		item->SetIsSearch(player->IsSearch());
	}
	
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
	DrawLine3D(UIPos.VGet(), Vec3(UIPos + player->GetRigidbody()->GetVelocity() * 20).VGet(), 0x00ffff);

	DrawCircle(200, 500, 100, 0xffff00,0);
	DrawLine(200, 500, static_cast <int>( 200 + player->GetInputVec().x * 70), static_cast < int>( 500 + player->GetInputVec().z * 70), 0xff0000);
	DrawLine(200, 500, static_cast <int>( 200 + player->GetPostMoveDir().x * 70), static_cast < int>( 500 + player->GetPostMoveDir().z * 70), 0x0000ff);
	DrawCircle(static_cast < int>(200 + player->GetInputVec().x *((player->GetInputVec().Length()*70.f))), static_cast < int>(500 + player->GetInputVec().z * ((player->GetInputVec().Length() * 70.f))), 30, 0xffff00, 0);


	
	DxLib::SetDrawBlendMode(DX_BLENDMODE_MULA, alpha);
	DxLib::DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0xff4444, true);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawFormatString(0, 25 * 0, 0xffffff, "HP:%f", player->GetHp());

	DrawFormatString(0, 25*1, 0xffffff, "UpVec(%f,%f,%f)", player->GetUpVec().x, player->GetUpVec().y, player->GetUpVec().z);
	
	DrawFormatString(0, 25*2, 0xffffff, "FrontVec(%f,%f,%f)", player->GetFrontVec().x, player->GetFrontVec().y, player->GetFrontVec().z);
	DrawFormatString(0, 25*3, 0xffffff, "SideVec(%f,%f,%f)", player->GetSideVec().x, player->GetSideVec().y, player->GetSideVec().z);
	DrawFormatString(0, 25*4, 0xffffff, "shotDir(%f,%f,%f)", player->GetShotDir().x, player->GetShotDir().y, player->GetShotDir().z);
	DrawFormatString(0, 25*5, 0xffffff, "Camera(%f,%f,%f),Length(%f)",m_camera->GetPos().x, m_camera->GetPos().y, m_camera->GetPos().z,(m_camera->GetPos() - player->GetPos()).Length());

	auto cameraNear = GetCameraNear();
	auto cameraFar = GetCameraFar();
	
	DrawFormatString(0, 25 * 6, 0xffffff, "CameraNearFar(%f,%f)", cameraNear, cameraFar);
	
	DrawFormatString(0, 25*7, 0xffffff, "PlayerPos(%f,%f,%f)", player->GetPos().x, player->GetPos().y, player->GetPos().z);
	DrawFormatString(0, 25*8, 0xffffff, player->GetStateName().c_str());
	DrawFormatString(0, 25*9, 0xffffff, "EasingSpeed:%f", player->GetCameraEasingSpeed());
	DrawFormatString(0, 25 * 10, 0xffffff, "FootNowOnHit:%d", player->GetFootOnHit());
	DrawFormatString(0, 25 * 11, 0xffffff, "PlayerVelocity(%f,%f,%f)", player->GetRigidbody()->GetVelocity());

	DrawFormatString(0, 25 * 12, 0xffffff, "size(%d)",GalaxyCreater::GetInstance().GetSize());
	DrawFormatString(0, 25 * 13, 0xffffff, "JumpFlag:%d", player->GetJumpFlag());

	DrawFormatString(0, 25 * 14, 0xffffff, Pad::GetState().c_str());
	DrawFormatString(0, 25 * 15, 0xffffff, "CameraPoint(%f,%f,%f)", m_camera->GetCameraPoint().x, m_camera->GetCameraPoint().y, m_camera->GetCameraPoint().z);
	DrawFormatString(0, 25 * 17, 0xffffff, "state:%d", player->GetPostState());

#endif
	//
	//SetScreenFlipTargetWindow(NULL);

	//m_camera->Set();
	//ScreenFlip();

	//// 少し時間の経過を待つ
	//WaitTimer(2);

	//ClearDrawScreen();
	//
	//DrawRotaGraph(800,450,1.0f,0, m_modelScreenHandle, true);
	//
}

void SerialPlanetGalaxy::BossBattleDraw()
{
}

template <typename T>
void SerialPlanetGalaxy::DeleteObject(std::vector<std::shared_ptr<T>>& objects)
{
	//remove_ifは移動させた要素を未定義(empty)にするみたいです。
	auto result = remove_if(objects.begin(), objects.end(), [this](const auto& object)
		{
			auto flag= object->IsDestroy();
	/*if (flag)
	{
		auto collidable = std::static_pointer_cast<MyEngine::Collidable>(object);
		MyEngine::Physics::GetInstance().Exit(collidable);
	}*/
	return  flag;// IsDestroy() が true の場合削除
		});
	objects.erase(result, objects.end());
}
