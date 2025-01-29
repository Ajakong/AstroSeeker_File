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
m_bossBattleBgmHandle(SoundManager::GetInstance().GetSoundData("SpaceEmperor_battle.mp3"))
{

	MyEngine::Physics::GetInstance().Entry(player);//物理演算クラスに登録

	
	
	//惑星の配置
	GalaxyCreater::GetInstance().PlanetCreate();
	//敵の配置
	m_enemies=GalaxyCreater::GetInstance().EnemyCreate(player);
	//その他オブジェクトの配置
	GalaxyCreater::GetInstance().ObjectCreate(player);
	//その他オブジェクトの配置
	GalaxyCreater::GetInstance().LockedObjectCreate();
	m_keyLockEnemies=GalaxyCreater::GetInstance().KeyLockObjectCreate();

	m_planet.push_back(make_shared<PolygonModelPlanet>(ModelManager::GetInstance().GetModelData("UFO_GreenMan"), Vec3(0, -1000, 200), 1, 1.0f, 5.f));
#ifdef _DEBUG

	//オブジェクトやギミックの配置(のちのちUnityのデータを読み込んで配置するので今は仮配置)

	////ギミック
	////ブースター


	//m_booster.push_back(make_shared<Booster>(Vec3(0,15,0),Vec3(0,1,1).GetNormalized(), -1,4.5f));
	//MyEngine::Physics::GetInstance().Entry(m_booster.back());
	//m_booster.push_back(make_shared<Booster>(Vec3(0, -20, 53), Vec3(0,1,0).GetNormalized(), -1));
	//MyEngine::Physics::GetInstance().Entry(m_booster.back());

	m_warpGate.push_back(make_shared<WarpGate>(Vec3(0, -50, -70), Vec3(-200, -300, 0), -1));
	MyEngine::Physics::GetInstance().Entry(m_warpGate.back());
	//スターキャプチャー
	m_starCapture.push_back(make_shared<StarCapture>(Vec3(0, 50, 40)));
	MyEngine::Physics::GetInstance().Entry(m_starCapture.back());

	////シーカーライン
	//std::vector<Vec3>seekerLine1Points;
	//seekerLine1Points.push_back(Vec3(-50, -25,0));
	//seekerLine1Points.push_back(Vec3(-20, 50, 0));
	//seekerLine1Points.push_back(Vec3(-20, 100, 0));
	//seekerLine1Points.push_back(Vec3(0, 30, 0));
	//seekerLine1Points.push_back(Vec3(100, 200, 0));
	//seekerLine1Points.push_back(Vec3(50, 450, 100));
	//m_seekerLine.push_back(make_shared<SeekerLine>(seekerLine1Points,0x00aaff));

	//MyEngine::Physics::GetInstance().Entry(m_seekerLine.back());


	//クリスタル
	//m_crystal.push_back(make_shared<Crystal>(Vec3(0, 0, 20),Vec3(0,1,0) ,Vec3(10, 10, 10)));
	//MyEngine::Physics::GetInstance().Entry(m_crystal.back());

#endif
	
	m_talkObjects.push_back(std::make_shared<DekaHead_Red>(Vec3(-10, 0, 50)));
	MyEngine::Physics::GetInstance().Entry(m_talkObjects.back());

	m_talkObjects.push_back(std::make_shared<DekaHead_Green>(Vec3(0, -300, 50)));
	MyEngine::Physics::GetInstance().Entry(m_talkObjects.back());

	m_talkObjects.push_back(std::make_shared<DekaHead_Blue>(Vec3(-123, 481, 1463)));
	MyEngine::Physics::GetInstance().Entry(m_talkObjects.back());

	m_camera = make_shared<Camera>();
	m_camera->SetCameraPoint(player->GetPos() + player->GetNormVec().GetNormalized() * kCameraDistanceUp - player->GetFrontVec() * (kCameraDistanceFront));
	m_camera->SetAimCamera(Vec3(0, 0, 0));
	/*m_planet.push_back(std::make_shared<SpherePlanet>(Vec3(0, -50, 0), 0xaadd33, 3.f, ModelManager::GetInstance().GetModelData("GoldenBall.mv1")));
	m_planet.push_back(std::make_shared<SpherePlanet>(Vec3(-100, 50, 400), 0xaa0000, 3.f, ModelManager::GetInstance().GetModelData("Sphere/planet_daia.mv1"),4));
	m_planet.push_back(std::make_shared<SpherePlanet>(Vec3(-200, -300, 0), 0xaa0000, 3.f, ModelManager::GetInstance().GetModelData("Sphere/planet_daia.mv1")));
	m_planet.push_back(std::make_shared<BoxPlanet>(Vec3(0, -50, 200), 0x00ffff, 1.0f, Vec3(30, 30, 50)));
	m_planet.push_back(std::make_shared<SpherePlanet>(Vec3(0, 500, 0), 0xaa0000, 3.f, ModelManager::GetInstance().GetModelData("Sphere/planet_daia.mv1")));
	m_planet.push_back(std::make_shared<SpherePlanet>(Vec3(300, 200, 100), 0xaadd33, 3.f, ModelManager::GetInstance().GetModelData("Sphere/planet_red.mv1")));
	*/


	m_skyDomeH = ModelManager::GetInstance().GetModelData("Skybox");

	////エネミー
	//m_kuribo.push_back(make_shared<Kuribo>(Vec3(0, 0, 30)));
	//
	//MyEngine::Physics::GetInstance().Entry(m_kuribo.back());
	//m_takobo.push_back(make_shared<Takobo>(Vec3(0, 500, -60), player));
	//MyEngine::Physics::GetInstance().Entry(m_takobo.back());
	//m_takobo.push_back(make_shared<Takobo>(Vec3(0, 500, 60), player));
	//MyEngine::Physics::GetInstance().Entry(m_takobo.back());
	//m_takobo.push_back(make_shared<Takobo>(Vec3(60, 500, -10), player));
	//MyEngine::Physics::GetInstance().Entry(m_takobo.back());
	//m_boss.push_back(make_shared<Boss>(Vec3(300, 250, 100)));
	//MyEngine::Physics::GetInstance().Entry(m_boss.back());
	

	MV1SetScale(m_skyDomeH, VGet(1.3f, 1.3f, 1.3f));

	////アイテム
	//m_coin.push_back(make_shared<Coin>(Vec3(0, -105, 0), true));
	//MyEngine::Physics::GetInstance().Entry(m_coin.back());
	//m_item.push_back(make_shared<StickStarItem>(Vec3(0, 450, 0),true));
	//m_item.push_back(make_shared <FullPowerDropItem>(Vec3(-550, 300, 0),true));
	
	m_managerUpdate = &SerialPlanetGalaxy::GamePlayingUpdate;
	m_managerDraw = &SerialPlanetGalaxy::GamePlayingDraw;

	//for (auto& item : m_planet)
	//{
	//	MyEngine::Physics::GetInstance().Entry(item);//物理演算クラスに登録
	//}
	for (auto& item : m_item)
	{
		MyEngine::Physics::GetInstance().Entry(item);
	}
	//それぞれのオブジェクトの上方向ベクトルなどの更新
	MyEngine::Physics::GetInstance().Update();

	m_modelScreenHandle = ScreenManager::GetInstance().GetScreenData(kModelScreenName, Game::kScreenWidth, Game::kScreenHeight);
	std::list<std::string> m_texts;
	
	/*UI::GetInstance().InText("隊長！Astro Seeker　諜報部隊が入手した情報によると");
	UI::GetInstance().InText("宇宙帝国の軍勢が我々の拠点に迫っているとのことです");
	UI::GetInstance().InText("我々の拠点を守るためには宇宙帝国の軍勢を撃退する必要があります");
	
	UI::GetInstance().InText("ともに");
	UI::GetInstance().InText("宇宙帝国の軍勢を撃退しましょう");*/

	UI::GetInstance().Init();
	UI::GetInstance().InText("そういえば久しぶりの実働任務になるな");
	UI::GetInstance().InText("体がなまってるんじゃないか？");

	std::list<std::string> mission;
	mission.push_back("目の前に謎の金色の球があるだろう");
	mission.push_back("道順においてあるはずだから順にとってみろ");
	UI::GetInstance().InTexts(mission);

	/*UI::GetInstance().InText("赤いヤツがキーを持ってる");
	UI::GetInstance().InText("Aでジャンプしてヤツを踏みつけてキーを奪い取れ");
	UI::GetInstance().InText("加速装置が見えるようになるはずだ");*/

	

}

SerialPlanetGalaxy::~SerialPlanetGalaxy()
{
	m_planet.clear();
	m_enemies.clear();
	m_keyLockEnemies.clear();
	m_poworStone.clear();
	m_warpGate.clear();
	m_talkObjects.clear();

	GalaxyCreater::GetInstance().Clear();
}

void SerialPlanetGalaxy::Init()
{
	ChangeVolumeSoundMem(100, m_bgmHandle);
	PlaySoundMem(m_bgmHandle, DX_PLAYTYPE_LOOP);
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
	m_camera->SetEasingSpeed(player->GetCameraEasingSpeed());
	
	if (player->OnAiming())m_camera->Update(player->GetShotDir());
	else m_camera->Update(player->GetLookPoint());

	//Vec3 planetToPlayer = player->GetPos() - player->GetNowPlanetPos();
	Vec3 sideVec = player->GetSideVec();
	Vec3 front = player->GetFrontVec();//-1をかけて逆ベクトルにしている

	//相対的な軸ベクトルの設定

	//player->SetUpVec(planetToPlayer);

	m_camera->SetBoost(player->GetBoostFlag());
	//本当はカメラとプレイヤーの角度が90度以内になったときプレイヤーの頭上を見たりできるようにしたい。
	m_camera->SetUpVec(player->GetNormVec());

	////エネミー
	//for (auto& item : m_kuribo) { item->Update(); }


	//for (auto& item : planet)item->Update();//ステージの更新
	////位置固定ギミック
	//for (auto& item : m_booster) { item->Update(); }
	//for (auto& item : m_starCapture) { item->Update(); }
	//for (auto& item : m_seekerLine) { item->Update(); }
	//for (auto& item : m_crystal) { item->Update();}
	//for (auto& item : m_coin)item->Update();

	userData->dissolveY = player->GetRegenerationRange();//シェーダー用プロパティ

	

	if (player->GetBoostFlag())
	{
		m_camera->SetCameraPoint(player->GetPos() + player->GetNormVec().GetNormalized() * (kCameraDistanceUp - 40) - front * ((kCameraDistanceFront - 70) + kCameraDistanceAddFrontInJump * player->GetJumpFlag()));
		//m_camera->SetCameraPoint(player->GetPos() + Vec3::Left() *30);
	}
	else
	{
		Vec3 upVec = player->GetNormVec().GetNormalized();
		if (player->OnAiming())
		{
			m_camera->SetCameraPoint(player->GetPos() + player->GetShotDir() * -5 + player->GetNormVec() * 8 + player->GetSideVec() * 2);
		}
		/*if (!player->GetJumpFlag())
		{
			m_camera->SetCameraPoint(player->GetPos() + upVec * kCameraDistanceUp - front * kCameraDistanceFront);
		}*/
		else
		{
			m_camera->SetCameraPoint(player->GetPos() + player->GetNormVec().GetNormalized() * kCameraDistanceUp - front * (kCameraDistanceFront + kCameraDistanceAddFrontInJump * player->GetJumpFlag()));
			//m_camera->SetCameraPoint(player->GetPos() + Vec3::Left() * 30);
		}
	}

	if (player->GetHp()<=0)
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
	DrawFormatString(0, 25*8, 0xffffff, player->GetState().c_str());
	DrawFormatString(0, 25*9, 0xffffff, "EasingSpeed:%f", player->GetCameraEasingSpeed());
	DrawFormatString(0, 25 * 10, 0xffffff, "FootNowOnHit:%d", player->GetFootOnHit());
	DrawFormatString(0, 25 * 11, 0xffffff, "PlayerVelocity(%f,%f,%f)", player->GetRigidbody()->GetVelocity());

	DrawFormatString(0, 25 * 12, 0xffffff, "size(%d)",GalaxyCreater::GetInstance().GetSize());
	DrawFormatString(0, 25 * 13, 0xffffff, "JumpFlag:%d", player->GetJumpFlag());

	DrawFormatString(0, 25 * 14, 0xffffff, Pad::GetState().c_str());
#endif

	SetCameraNearFar(1.0f, 30000.0f);
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
