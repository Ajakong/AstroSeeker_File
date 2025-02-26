#include <DxLib.h>
#include <cassert>
#include "Application.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "GameOverScene.h"
#include "GamePlayingScene.h"
#include "Game.h"
#include "Pad.h"
#include "GraphManager.h"
#include "SoundManager.h"
#include "ModelManager.h"
#include"EffectManager.h"
#include "TitlePlayer.h"
#include "Physics.h"
#include "SpherePlanet.h"
#include "Camera.h"
#include"UI.h"

namespace
{
    constexpr int kFadeFrameMax = 60;
    constexpr int kStandByFrame = 120;

    constexpr int kCameraMoveFrameMax = 70;

    constexpr int kLightningFrameMax = 200;

    const char* kTitleGraphName = "galaxy_titleLogo_pro.png";
    const char* kPushAToStartGraphName = "PushAToStart.png";
    const char* kFrameName = "Frame.png";
    const char* kTitleBGMName = "AstroSeeker_Theme_free.mp3";
    const char* kGamePlayBGMName = "BattleOfAstro.mp3";
    
    const char* kTitleFadeSEName = "TitleSE_Fade.mp3";



    const char* kStickName = "parry.mp3";
    const char* kGameStartSEName = "StartGame_SE.mp3";
    const char* kGameBGMName = "GamePlaying.mp3";
    const char* kPlayerModelName = "SpaceHarrier";
    const char* kPlanetModelName = "GoldenBall";
    const char* kNextPlanetModelName = "Neptune";
	const char* kEmeraldPlanetModelName = "GreenMoon";
    const char* kRedPlanetModelName = "RedMoon";
    const char* kSkyboxModelName = "Skybox";

    constexpr float kCameraNear = 10.0f;
    constexpr float kCameraFar = 3000.0f;
    constexpr float kSkyDomeScale = 0.8f;
    constexpr float kSkyDomeRotationSpeed = 0.001f;

    constexpr int kTitleGraphX = 0;
    constexpr int kTitleGraphY = 150;
    constexpr int kTitleGraphWidth = 800;
    constexpr int kTitleGraphHeight = 600;
    constexpr int kTitleTextX = 650;
    constexpr int kTitleTextY = 700;
  
    constexpr int kFadeBoxColor = 0x001111;
    constexpr int kLineColor = 0x44ffff;
    constexpr int kLineX = 30;

    const Vec3 cameraFirstPosition = { -50,0,200 };
    const Vec3 cameraSecondPosition = { -5,10,10 };
    const Vec3 cameraThirdPosition = { -200,-45,80 };

}

/// <summary>
/// 後々はタイトルでもオブジェクトの位置をUnityで設定して、それを読み込むようにする
/// </summary>
/// <param name="manager"></param>
TitleScene::TitleScene(SceneManager& manager) :
    Scene(manager),
    
    m_cameraMoveCount(0),
    m_btnFrame(0),
    m_fadeSpeed(1),
    m_skyDomeRotationAngle(0),
    m_cameraAngleRotateSpeed(0),

    m_skyDomeH(-1),
    m_titleHandle(-1),
    m_fadeSEHandle(-1),
    m_titleBGMHandle(-1),
    m_gameStartSEHandle(-1),
    m_PushAToStartHandle(-1),
    
    //タイトル用のPlayerの生成
    player(std::make_shared<TitlePlayer>()),
    
    //Playerが上に乗る惑星を(0,-50,0)の位置に生成
    planet(std::make_shared<SpherePlanet>(Vec3(0, -50, 0), 0xaadd33, 3.f, ModelManager::GetInstance().GetModelData(kPlanetModelName), 1.0f, 1)),
    
    //赤い惑星を(50,-50,300)の位置に生成
    redPlanet(std::make_shared<SpherePlanet>(Vec3(50, -50, 300), 0xaadd33, 3.f, ModelManager::GetInstance().GetModelData(kRedPlanetModelName), 1.0f, 1)),
    
    //プレイヤーが移動する先の惑星の生成
    nextPlanet(std::make_shared<SpherePlanet>(Vec3(-300, -50, 200), 0xaadd33, 3.f, ModelManager::GetInstance().GetModelData(kNextPlanetModelName), 1.0f, 1)),
	
    //緑の惑星を(200,100,200)の位置に生成
    emeraldPlanet(std::make_shared<SpherePlanet>(Vec3(200, 100, 200), 0xaadd33, 3.f, ModelManager::GetInstance().GetModelData(kEmeraldPlanetModelName), 1.0f, 1)),
    
    //初期位置にカメラインスタンスの生成
    camera(std::make_shared<Camera>(cameraFirstPosition))
{
    
    m_titleBGMHandle = SoundManager::GetInstance().GetSoundData(kTitleBGMName);
    
    //フェード時に流れるSEのロード
    m_fadeSEHandle = SoundManager::GetInstance().GetSoundData(kTitleFadeSEName);
    
    //タイトルロゴの画像ロード
    m_titleHandle = GraphManager::GetInstance().GetGraphData(kTitleGraphName);
    
    //ゲーム開始時に流れるSEのロード
    m_gameStartSEHandle = SoundManager::GetInstance().GetSoundData(kGameStartSEName);
    
    //"PUSH A TO START"の画像のロード
    m_PushAToStartHandle = GraphManager::GetInstance().GetGraphData(kPushAToStartGraphName);

    camera->Update(VGet(0, 0, 150));
    SoundManager::GetInstance().ChangeBGM(m_titleBGMHandle);

    MyEngine::Physics::GetInstance().Entry(player);
    MyEngine::Physics::GetInstance().Entry(planet);
    MyEngine::Physics::GetInstance().Entry(redPlanet);
    MyEngine::Physics::GetInstance().Entry(nextPlanet);
    MyEngine::Physics::GetInstance().Entry(emeraldPlanet);
  

    m_updateFunc = &TitleScene::NormalUpdate;
    m_drawFunc = &TitleScene::NormalDraw;

    m_skyDomeH = ModelManager::GetInstance().GetModelData(kSkyboxModelName);
    MV1SetPosition(m_skyDomeH, Vec3::Zero().VGet());
    MV1SetPosition(m_skyDomeH, VGet(0, 0, 0));
    MV1SetScale(m_skyDomeH, VGet(kSkyDomeScale, kSkyDomeScale, kSkyDomeScale));

    UI::GetInstance().Init();
    UI::GetInstance().SetTalkObjectHandle(UI::TalkGraphKind::TakasakiTaisa);
    std::list<std::string> ImTakasaki;
    ImTakasaki.push_back( "聞こえるか、ドレイク！　私だ、タカサキ大佐だ。");
    ImTakasaki.push_back("聞こえていたらAボタンを押してくれ。");
    UI::GetInstance().InTexts(ImTakasaki);

    std::list<std::string> mainMessage;
    mainMessage.push_back("よし、届いてるみたいだな。");
    mainMessage.push_back("では本題に移ろう。");
    UI::GetInstance().InTexts(mainMessage);



    std::list<std::string> yabai;
    yabai.push_back("宇宙最大のエネルギー持つ物体、");
    yabai.push_back("スーパーマテリアルが何者かに盗まれた。");
    UI::GetInstance().InTexts(yabai);
    UI::GetInstance().InText("スーパーマテリアルはこの宇宙の安定には必須のものだ。");
    std::list < std::string > mission;
    mission.push_back("頼む、ドレイク。");
    mission.push_back("スーパーマテリアルを取り戻してくれ！");
    UI::GetInstance().InTexts(mission);



    camera->SetEasingSpeed(35.f);

    //カメラが回転するスピード
    m_cameraAngleRotateSpeed = 0.02f;
}

TitleScene::~TitleScene()
{
   
    
    MyEngine::Physics::GetInstance().Exit(player);
    int a = player.use_count();
    MyEngine::Physics::GetInstance().Exit(planet);
    MyEngine::Physics::GetInstance().Exit(nextPlanet);
    
    EffectManager::GetInstance().Clear();

}

void TitleScene::Load()
{
}

void TitleScene::Update()
{
   

    //planet->ModelRotation(-1);
	nextPlanet->ModelRotation();
	emeraldPlanet->ModelRotation();
    redPlanet->ModelRotation();
    MyEngine::Physics::GetInstance().Update();
    player->SetMatrix();
    camera->Setting(player->GetBoostFlag(), player->GetIsAiming());
    (this->*m_updateFunc)();
    m_skyDomeRotationAngle += kSkyDomeRotationSpeed;
    MV1SetRotationXYZ(m_skyDomeH, VGet(0, m_skyDomeRotationAngle, 0));

    EffectManager::GetInstance().Update();
    Pad::Update();
}

void TitleScene::Draw()
{
    // 画面とZバッファのクリア
    ClearDrawScreen();


    // スカイドームの描画
    DxLib::MV1DrawModel(m_skyDomeH);
    // 物理エンジンの描画
    MyEngine::Physics::GetInstance().Draw();
    // シーン固有の描画
    (this->*m_drawFunc)();
  
  

}


void TitleScene::FadeInUpdate()
{
    m_fps = GetFPS();
    m_frame--;
    if (m_frame <= 0)
    {
        m_updateFunc = &TitleScene::NormalUpdate;
        m_drawFunc = &TitleScene::NormalDraw;
    }
}

void TitleScene::NormalUpdate()
{
    m_fps = GetFPS();

    player->SetMatrix();
    
    Quaternion q;
    q.SetQuaternion(camera->GetPos());
    q.SetMove(m_cameraAngleRotateSpeed, Vec3::Up());
   

    camera->SetCameraPoint(q.Move(camera->GetPos(), Vec3::Zero()));
    camera->Update(VGet(0, 0, 150));

    //Aボタンが押されたら
    if (Pad::IsTrigger(PAD_INPUT_1))
    {
       
        camera->SetCameraPoint(cameraSecondPosition);
        m_updateFunc = &TitleScene::WatchPlayerUpdate;
    }

    m_btnFrame += m_fadeSpeed;
    if (m_btnFrame > kFadeFrameMax)m_fadeSpeed *= -1;
    if (m_btnFrame < 0)m_fadeSpeed *= -1;
}

void TitleScene::WatchPlayerUpdate()
{
    m_fps = GetFPS();

    player->SetMatrix();
    Quaternion q;
   
    q.SetQuaternion(camera->GetPos());
    q.SetMove(m_cameraAngleRotateSpeed, Vec3::Up());

    //camera->SetCameraPoint(q.Move(camera->GetPos(), Vec3::Zero()));
    camera->Update(player->GetRigidbody()->GetPos());


    if ((camera->GetPos()-camera->GetCameraPoint()).Length()<=5.f)
    {
        player->SetShot();
        if (player->GetAnimBlendRate() >= 1.f)
        {
            MyEngine::Physics::GetInstance().Exit(emeraldPlanet);
            MyEngine::Physics::GetInstance().Exit(redPlanet);
            camera->SetCameraPoint(cameraThirdPosition);
            player->MoveToTargetPosWithSticker(nextPlanet->GetRigidbody()->GetPos());
            m_updateFunc = &TitleScene::DirectionUpdate;
        }
        
    }

    m_btnFrame += m_fadeSpeed;
    if (m_btnFrame > kFadeFrameMax)m_fadeSpeed *= -1;
    if (m_btnFrame < 0)m_fadeSpeed *= -1;
}

void TitleScene::FadeOutUpdate()
{
    /*camera->Update(player->GetRigidbody()->GetPos());
    camera->SetCameraPoint(player->GetPos() + positioningPlayerToCamera * 10);*/
    m_fps = GetFPS();
    m_frame++;
   
    if (m_frame >= kStandByFrame) {
        m_isGamePlaying = true;
    }
}

void TitleScene::DirectionUpdate()
{
    camera->Update(player->GetRigidbody()->GetPos());
    bool moveFlag = player->MoveToTargetPosWithSticker(nextPlanet->GetRigidbody()->GetPos());
    if (moveFlag)
    {
        m_updateFunc = &TitleScene::LoadingUpdate;
        //m_drawFunc = &TitleScene::FadeDraw;
    }
}

void TitleScene::LoadingUpdate()
{
    //ロード中
    m_cameraMoveCount++;
    
    //プレイヤーを映す
    camera->Update(player->GetRigidbody()->GetPos());

	//カメラがプレイヤーに向けて近づいてる時間が一定になったら
    if (m_cameraMoveCount == kCameraMoveFrameMax)
    {
		//現時点でのプレイヤーの位置からカメラの位置を格納
        positioningPlayerToCamera = camera->GetPos() - player->GetPos();
        positioningPlayerToCamera.Normalize();
    }
    
    //カメラがプレイヤーに向けて近づいてる時間が一定以上の場合
    if (m_cameraMoveCount > kCameraMoveFrameMax)
    {
		//カメラの位置を固定
        camera->SetCameraPoint(player->GetPos() + positioningPlayerToCamera * 10);
       
        //プレイヤーの位置を固定
        player->DoNotMove();
		
        //UIの更新処理をして会話を入れる
        UI::GetInstance().Update();
		
        //残りの会話テキストがなくなったら
        if (UI::GetInstance().TextRemaining() == 0)
        {
			//フェードアウト処理に移行
            //フェードアウトのSEを再生
            PlaySoundMem(m_fadeSEHandle, DX_PLAYTYPE_BACK);
            
            //止めていた位置を動かすように設定
            player->Move();
            
            //ゲームスタートのSEも同時に再生する
            PlaySoundMem(m_gameStartSEHandle,DX_PLAYTYPE_BACK);
            m_updateFunc = &TitleScene::FadeOutUpdate;
            m_drawFunc = &TitleScene::FadeDraw;
        }
    }
    
}

void TitleScene::ChangeScene(std::shared_ptr<Scene> next)
{
    m_manager.ChangeScene(next);
}

void TitleScene::FadeDraw()
{
    //透明度の計算
    int alpha = static_cast<int>(255 * (static_cast<float>(m_frame) / kFadeFrameMax));
	
    //フェードアウトの処理以外なら
    if (!(m_updateFunc == &TitleScene::FadeOutUpdate))
    {
		//タイトルロゴの描画
        DrawExtendGraph(kTitleGraphX, kTitleGraphY, kTitleGraphWidth, kTitleGraphHeight, m_titleHandle, true);
        
        int btnalpha = static_cast<int>(255 * (static_cast<float>(m_btnFrame) / kFadeFrameMax));
        SetDrawBlendMode(DX_BLENDMODE_ADD, btnalpha);
		
        //"PUSH A TO START"の描画
        DrawExtendGraph(1100, 790, 1850, 1000, m_PushAToStartHandle, true);
        
        //DrawFormatString(kTitleTextX, kTitleTextY, 0xffffff, "Push A to Start");
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    }

    //フェードパネルの描画
    SetDrawBlendMode(DX_BLENDMODE_MULA, alpha);
    DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
    DrawBox(0, 0, m_frame * kLineX, Game::kScreenWidth, kFadeBoxColor, true);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    //次にゲームシーンに移行する場合
    if (m_isGamePlaying)
    {
		//タイトルで使用したシングルトンの初期化
        ModelManager::GetInstance().Clear();
        UI::GetInstance().Init();
        Pad::Init();
		//ゲームプレイシーンに移行
        ChangeScene(std::make_shared<GamePlayingScene>(m_manager));
    }
    
    //タイトル遷移時のフェード演出で使用する青い線の描画
    DrawLine(m_frame * kLineX, 0, m_frame * kLineX, Game::kScreenHeight, kLineColor);
}

void TitleScene::NormalDraw()
{
    //ロードが完了していたら
    if (!(m_updateFunc == &TitleScene::LoadingUpdate))
    {
       
        int alpha = static_cast<int>(255 * (static_cast<float>(m_frame) / kFadeFrameMax));
        if (m_updateFunc == &TitleScene::NormalUpdate)
        {
            DrawRotaGraph2(600, 375, 615, 350, 1.f, 0, m_titleHandle, true);


            int btnalpha = static_cast<int>(255 * (static_cast<float>(m_btnFrame) / kFadeFrameMax));
            SetDrawBlendMode(DX_BLENDMODE_ADD, btnalpha);

            DrawExtendGraph(1100, 790, 1850, 1000, m_PushAToStartHandle, true);
           
            
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }
        SetDrawBlendMode(DX_BLENDMODE_MULA, alpha);
        DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
        DrawBox(0, 0, m_frame * kLineX, Game::kScreenWidth, kFadeBoxColor, true);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
       
    }
    //タイトル遷移時のフェード演出で使用する青い線の描画
    DrawLine(m_frame * kLineX, 0, m_frame * kLineX, Game::kScreenHeight, kLineColor);
    
    if(m_cameraMoveCount > 70)
    {
        UI::GetInstance().Draw();

    }

}