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
#include "TitlePlayer.h"
#include "Physics.h"
#include "SpherePlanet.h"
#include "Camera.h"
#include"UI.h"
namespace
{
    constexpr int kFadeFrameMax = 60;
    constexpr int kStandByFrame = 120;

    // 画面の幅と高さ
    constexpr int screenWidth = 1600;
    constexpr int screenHeight = 900;

    constexpr int kLightningFrameMax = 200;

    const char* kTitleGraphName = "galaxy_titleLogo_pro.png";
    const char* kFrameName = "Frame.png";
    const char* kTitleBGMName = "Title.mp3";

    const char* kStickName = "parry.mp3";
    const char* kGameStartSEName = "StartGame_SE.mp3";
    const char* kGameBGMName = "GamePlaying.mp3";
    const char* kPlayerModelName = "SpaceHarrier";
    const char* kPlanetModelName = "GoldenBall";
    const char* kNextPlanetModelName = "Moon";
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
    constexpr int kTitleTextX = 350;
    constexpr int kTitleTextY = 600;
    constexpr int kFadeBoxWidth = 1600;
    constexpr int kFadeBoxHeight = 900;
    constexpr int kFadeBoxColor = 0x001111;
    constexpr int kLineColor = 0x44ffff;
    constexpr int kLineX = 30;
    constexpr int kLineY = 900;

    const Vec3 cameraFirstPosition = { -200,-45,120 };
    const Vec3 cameraSecondPosition = { -200,-45,80 };

}

TitleScene::TitleScene(SceneManager& manager) :
    Scene(manager),
    m_titleBGMHandle(SoundManager::GetInstance().GetSoundData(kTitleBGMName)),
    m_gameStartSEHandle(SoundManager::GetInstance().GetSoundData(kGameStartSEName)),
    m_btnFrame(0),
    m_fadeSpeed(1),
    m_titleHandle(GraphManager::GetInstance().GetGraphData(kTitleGraphName)),
    player(std::make_shared<TitlePlayer>(ModelManager::GetInstance().GetModelData(kPlayerModelName))),
    planet(std::make_shared<SpherePlanet>(Vec3(0, -50, 0), 0xaadd33, 3.f, ModelManager::GetInstance().GetModelData(kPlanetModelName), 1.0f, 1)),
    nextPlanet(std::make_shared<SpherePlanet>(Vec3(-300, -50, 200), 0xaadd33, 3.f, ModelManager::GetInstance().GetModelData(kNextPlanetModelName), 1.0f, 1)),
	emeraldPlanet(std::make_shared<SpherePlanet>(Vec3(400, -50, 200), 0xaadd33, 3.f, ModelManager::GetInstance().GetModelData(kEmeraldPlanetModelName), 1.0f, 1)),
    redPlanet(std::make_shared<SpherePlanet>(Vec3(200, -150,350), 0xaadd33, 3.f, ModelManager::GetInstance().GetModelData(kRedPlanetModelName), 1.0f, 1)),
	camera(std::make_shared<Camera>(cameraFirstPosition)),
    m_skyDomeH(0),
    m_skyDomeRotationAngle(0),
	m_count(0)
{
    
    camera->Update(VGet(0, 0, 150));

    PlaySoundMem(m_titleBGMHandle, DX_PLAYTYPE_LOOP);
    //SetCameraPositionAndTarget_UpVecY(VGet(-200, -45, 80), VGet(0, -45, 80));
    //SetCameraNearFar(kCameraNear, kCameraFar);

    MyEngine::Physics::GetInstance().Entry(planet);
    MyEngine::Physics::GetInstance().Entry(nextPlanet);
    MyEngine::Physics::GetInstance().Entry(emeraldPlanet);
    MyEngine::Physics::GetInstance().Entry(redPlanet);
    MyEngine::Physics::GetInstance().Entry(player);

    m_updateFunc = &TitleScene::NormalUpdate;
    m_drawFunc = &TitleScene::NormalDraw;

    m_skyDomeH = ModelManager::GetInstance().GetModelData(kSkyboxModelName);
    MV1SetPosition(m_skyDomeH, Vec3::Zero().VGet());
    MV1SetPosition(m_skyDomeH, VGet(0, 0, 0));
    MV1SetScale(m_skyDomeH, VGet(kSkyDomeScale, kSkyDomeScale, kSkyDomeScale));

    UI::GetInstance().Init();
    std::list<string> ImTakasaki;
    ImTakasaki.push_back( "私はタカサキ大佐だ");
    ImTakasaki.push_back("この通信が届いていたらAボタンで応答してくれ");
    UI::GetInstance().InTexts(ImTakasaki);

    std::list<string> mainMessage;
    mainMessage.push_back("よし、届いてるみたいだな");
    mainMessage.push_back("では本題に移ろう");
    UI::GetInstance().InTexts(mainMessage);

    UI::GetInstance().InText("Astro Seeker第一部隊隊長である君に宇宙の存亡をかけた超重要任務を授ける");

    std::list<string> yabai;
    yabai.push_back("諜報部隊が入手した情報によると");
    yabai.push_back("宇宙最大のエネルギー持つ物体スーパーマテリアルが何者かに盗まれた");
    UI::GetInstance().InTexts(yabai);
    UI::GetInstance().InText("この宇宙を守るためにはスーパーマテリアルを取り戻す必要がある");
    UI::GetInstance().InText("この任務は死線を乗り越えた君にしか成し遂げられないことだと判断した");
    UI::GetInstance().InText("スーパーマテリアルを取り戻し、全宇宙の平和を取り戻してくれ");
    UI::GetInstance().InText("君には全宇宙の未来がかかっている");
    UI::GetInstance().InText("さあ、Astro Seeker第一部隊隊長として宇宙の未来を切り開け！");


    camera->SetEasingSpeed(15.f);
}

TitleScene::~TitleScene()
{
   
    MyEngine::Physics::GetInstance().Exit(player);
    MyEngine::Physics::GetInstance().Exit(planet);
    MyEngine::Physics::GetInstance().Exit(nextPlanet);
    MyEngine::Physics::GetInstance().Exit(emeraldPlanet);
    MyEngine::Physics::GetInstance().Exit(redPlanet);

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
    (this->*m_updateFunc)();
    m_skyDomeRotationAngle += kSkyDomeRotationSpeed;
    MV1SetRotationXYZ(m_skyDomeH, VGet(0, m_skyDomeRotationAngle, 0));

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
    if (Pad::IsTrigger(PAD_INPUT_1))
    {
        
        camera->SetCameraPoint(cameraSecondPosition);
        player->MoveToTargetPosWithSticker(nextPlanet->GetRigidbody()->GetPos());
        m_updateFunc = &TitleScene::DirectionUpdate;
    }

   
    

    m_btnFrame += m_fadeSpeed;
    if (m_btnFrame > kFadeFrameMax)m_fadeSpeed *= -1;
    if (m_btnFrame < 0)m_fadeSpeed *= -1;
}

void TitleScene::FadeOutUpdate()
{
    camera->Update(player->GetRigidbody()->GetPos());
    camera->SetCameraPoint(player->GetPos() + positioningPlayerToCamera * 10);
    player->MoveToTargetWithStickStar(Vec3(0, 0, 0));
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
    m_count++;
    camera->Update(player->GetRigidbody()->GetPos());

    if (m_count == 70)
    {
        positioningPlayerToCamera = camera->GetPos() - player->GetPos();
        positioningPlayerToCamera.Normalize();
    }

    if (m_count > 70)
    {
        
        camera->SetCameraPoint(player->GetPos() + positioningPlayerToCamera * 10);
        player->DoNotMove();
        UI::GetInstance().Update();
        if (UI::GetInstance().TextRemaining() == 0)
        {
            PlaySoundMem(m_gameStartSEHandle,DX_PLAYTYPE_BACK);
            m_updateFunc = &TitleScene::FadeOutUpdate;
            m_drawFunc = &TitleScene::FadeDraw;
        }
    }
    
}

void TitleScene::ChangeScene(std::shared_ptr<Scene> next)
{
    StopSoundMem(m_titleBGMHandle);
    SetCameraPositionAndTarget_UpVecY(VGet(0, 0, 0), VGet(0, 0, 1));
    m_manager.ChangeScene(next);
}

void TitleScene::FadeDraw()
{
    int alpha = static_cast<int>(255 * (static_cast<float>(m_frame) / kFadeFrameMax));
    if (!(m_updateFunc == &TitleScene::FadeOutUpdate))
    {
        int alpha = static_cast<int>(255 * (static_cast<float>(m_frame) / kFadeFrameMax));

        DrawExtendGraph(kTitleGraphX, kTitleGraphY, kTitleGraphWidth, kTitleGraphHeight, m_titleHandle, true);
        int btnalpha = static_cast<int>(255 * (static_cast<float>(m_btnFrame) / kFadeFrameMax));
        SetDrawBlendMode(DX_BLENDMODE_ADD, btnalpha);

        DrawFormatString(kTitleTextX, kTitleTextY, 0xffffff, "Push A to Start");
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        SetDrawBlendMode(DX_BLENDMODE_MULA, alpha);
        DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
        DrawBox(0, 0, m_frame * kLineX, kFadeBoxWidth, kFadeBoxColor, true);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    }

    SetDrawBlendMode(DX_BLENDMODE_MULA, alpha);
    DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
    DrawBox(0, 0, m_frame * kLineX, kFadeBoxWidth, kFadeBoxColor, true);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    if (m_isGamePlaying)
    {
        ModelManager::GetInstance().Clear();
        UI::GetInstance().Init();
        Pad::Init();
        ChangeScene(std::make_shared<GamePlayingScene>(m_manager));
    }
    DrawLine(m_frame * kLineX, 0, m_frame * kLineX, kLineY, kLineColor);
}

void TitleScene::NormalDraw()
{
    if (!(m_updateFunc == &TitleScene::LoadingUpdate))
    {
        int alpha = static_cast<int>(255 * (static_cast<float>(m_frame) / kFadeFrameMax));

        DrawRotaGraph2(400, 375, 615, 350,0.5f,0, m_titleHandle, true);
        int btnalpha = static_cast<int>(255 * (static_cast<float>(m_btnFrame) / kFadeFrameMax));
        SetDrawBlendMode(DX_BLENDMODE_ADD, btnalpha);

        DrawFormatString(kTitleTextX, kTitleTextY, 0xffffff, "Push A to Start");
        
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        SetDrawBlendMode(DX_BLENDMODE_MULA, alpha);
        DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
        DrawBox(0, 0, m_frame * kLineX, kFadeBoxWidth, kFadeBoxColor, true);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    }
   
    DrawLine(m_frame * kLineX, 0, m_frame * kLineX, kLineY, kLineColor);
    if(m_count > 70)
    {
        UI::GetInstance().Draw();

    }
}