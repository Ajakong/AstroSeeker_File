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

    //ゲームが開始したか
    bool m_isGamePlaying = false;

    // タイトルロゴの画像ハンドル
    int m_titleHandle = 0;


    // "PUSH A TO START"の画像ハンドル
    int m_PushAToStartHandle;


    // タイトルのBGMハンドル
    int m_titleBGMHandle;

    // ゲーム開始時に流れるSEハンドル
    int m_gameStartSEHandle;

    // フェード時の演出のSEハンドル
    int m_fadeSEHandle;


    // カメラが動いている時間のカウント
    int m_cameraMoveCount;


    // カメラの回転速度
    float m_cameraAngleRotateSpeed;


    //ハンドル集
    int m_skyDomeH;
    float m_skyDomeRotationAngle;


    // タイトル用のPlayerインスタンス
    std::shared_ptr<TitlePlayer> player;

	// Playerの初期惑星

	std::shared_ptr<SpherePlanet> planet;
    
    // 演出上の赤い惑星
    std::shared_ptr<SpherePlanet> redPlanet;

   /// 演出上の緑の惑星
    std::shared_ptr<SpherePlanet> emeraldPlanet;
    
    // Playerの移動先の惑星
    std::shared_ptr<SpherePlanet> nextPlanet;
    
   
   
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
    /// <summary>
	/// 何も入力していないときの更新
    /// </summary>
    void NormalUpdate();
    /// <summary>
    /// 入力された際にカメラがプレイヤーに近づく演出の更新
    /// </summary>
    void WatchPlayerUpdate();
    /// <summary>
    /// シーンのフェードアウト
    /// </summary>
    void FadeOutUpdate();
    void DirectionUpdate();
    /// <summary>
    /// 非同期ロードを実行して、完了次第フェードアウトに遷移したい
    /// </summary>
    void LoadingUpdate();

    void ChangeScene(std::shared_ptr<Scene> next);

    // 描画関数
    void FadeDraw();
    /// <summary>
	/// フェード以外で呼ばれる描画関数
    /// </summary>
    void NormalDraw();

};



