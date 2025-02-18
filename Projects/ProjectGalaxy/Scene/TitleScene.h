#pragma once
#include "Scene.h"
#include"Vec3.h"
#include <string>
#include <vector>
#include <map>

class SceneMain;
class TitlePlayer;
class SpherePlanet;
class Camera;


class TitleScene : public Scene
{
public:
    TitleScene(SceneManager& manager);
    ~TitleScene();
    void Load();
    void Update();
    void Draw();


    void getSlectNumber(int a) { m_selectNum = a; }

private:

    int m_btnFrame = 0;
    int m_fadeSpeed = 0;
    int m_frame = 0;

    float m_fps = 0.0f;

    int m_selectNum = 0;

    bool m_isGamePlaying = false;

    int m_titleHandle = 0;
    int m_PushAToStartHandle;

    int m_titleBGMHandle;
    int m_gameStartSEHandle;
    int m_fadeSEHandle;

	int m_stickOSTHandle = 0;
    int m_count;

    float m_cameraRotateAngle;


    //ハンドル集
    int m_skyDomeH;
    float m_skyDomeRotationAngle;

    std::shared_ptr<TitlePlayer> player;
	std::shared_ptr<SpherePlanet> planet;
    std::shared_ptr<SpherePlanet> nextPlanet;
    std::shared_ptr<SpherePlanet> emeraldPlanet;
    std::shared_ptr<SpherePlanet> redPlanet;
	std::shared_ptr<Camera> camera;

    Vec3 positioningPlayerToCamera;

    Vec3 rotationCenterPos;

    // メンバ関数ポインタの宣言
    using UpdateFunc_t = void (TitleScene::*)();
    using DrawFunc_t = void (TitleScene::*)();
    UpdateFunc_t m_updateFunc = nullptr;
    DrawFunc_t m_drawFunc = nullptr;



    // 更新関数
    void FadeInUpdate();
    void NormalUpdate();
    void WatchPlayerUpdate();
    void FadeOutUpdate();
    void DirectionUpdate();
    void LoadingUpdate();

    void ChangeScene(std::shared_ptr<Scene> next);

    // 描画関数
    void FadeDraw();
    void NormalDraw();

};



