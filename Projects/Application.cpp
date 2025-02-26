#include "Application.h"
#include "SceneManager.h"
#include "GamePlayingScene.h"
#include"TitleScene.h"
#include"GameOverScene.h"
#include<iostream>
#include<vector>
#include"Game.h"
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include <cassert>


namespace
{
    constexpr int kScreenWidth = 1920;
    constexpr int kScreenHeight = 1080;

    constexpr int kColorbitDepth = 32;
    constexpr int kIconID = 573;
}

int MyLoadGraph(const TCHAR* path)
{
    int handle = LoadGraph(path);
    assert(handle != -1);
    return handle;
}



Application::Application()
{
    m_windowSize = Size{ kScreenWidth, kScreenHeight };
}

void Application::Terminate()
{
    Effkseer_End();
    DxLib_End();
}

int Application::GetScreenHandle()
{
    return m_screenHandle;
}

float Application::GetGravity() const
{
    return 0.5f;
}

bool Application::Init()
{

    ChangeWindowMode(true); // ウィンドウモードにします
    // VSYNC待ちをしない設定に変更
    SetWaitVSyncFlag(FALSE);
    SetUseDirect3DVersion(DX_DIRECT3D_11);


    SetWindowSizeChangeEnableFlag(TRUE, TRUE);
    SetChangeScreenModeGraphicsSystemResetFlag(false);


    SetGraphMode(kScreenWidth, kScreenHeight, kColorbitDepth);
    SetWindowSize(kScreenWidth, kScreenHeight);

    void* CallBack();

    

    SetWindowText("Astro Seeker");
    if (DxLib_Init() == -1)
    {
        return false;
    }

    SetWindowIconID(kIconID);

    SetUseASyncChangeWindowModeFunction(TRUE, NULL, NULL);

    //Zバッファを有効にする
    SetUseZBuffer3D(true);
    SetWriteZBuffer3D(true);

    //バックカリング(メッシュの裏側(メッシュの法線方向ではない面)を描画しない)
    SetUseBackCulling(true);

    Effekseer_Init(8000);
    Effekseer_InitDistortion();
    Effekseer_SetGraphicsDeviceLostCallbackFunctions();

   

    SetDrawScreen(DX_SCREEN_BACK);
    return true;
}

namespace TimeTool
{
    /// <summary>
    /// 時刻を入れる構造体変数
    /// </summary>
    struct DataTime
    {
        int hour;
        int minute;
        int second;
    };

    /// <summary>
    /// 現在時刻をDataTime型で取得。
    /// </summary>
    /// <returns>DataTime(構造体)</returns>
    DataTime GetNowTimeData()
    {
        //変数
        DataTime time;

        // 現在時刻をsystem_clockを用いて取得
        auto now = std::chrono::system_clock::now();

        // 現在時刻をtime_t形式に変換
        std::time_t t = std::chrono::system_clock::to_time_t(now);

        //現在時刻を作成
        time.hour = (t / 3600 + 9) % 24;//時間
        time.minute = t / 60 % 60;//分
        time.second = t % 60;//秒

        //結果を返す
        return time;
    }

    /// <summary>
    /// 現在時刻をコンソールに表示
    /// </summary>
    /// <returns>void</returns>
    void ShowNowTime()
    {
        //現在時刻の取得
        DataTime time = GetNowTimeData();

        //コンソールに表示
        printf("%d", time.hour);//日
        printf(":%d", time.minute);//分
        printf(":%d\n", time.second);//秒
    }

    /// <summary>
    /// 現在時刻をString型で返す。
    /// </summary>
    /// <returns>string</returns>
    std::string GetNowTimeString()
    {
        //現在時刻の取得
        DataTime time = GetNowTimeData();

        //現在時刻を文字列にまとめて返す
        return std::to_string(time.hour) + "," + std::to_string(time.minute) + "," + std::to_string(time.second);
    }
}

void Application::Run()
{
    ////printfがcmdに表示される
    //AllocConsole();                                      // コンソール
    //FILE* out = 0; freopen_s(&out, "CON", "w", stdout); // stdout
    //FILE* in = 0; freopen_s(&in, "CON", "r", stdin);   // stdin
    //// デバッグコンソールがアクティブウィンドウになるのでゲーム本体のウィンドウをアクティブにする
    //SetForegroundWindow(GetMainWindowHandle());

    {// スコープを強制的に作っている

        SceneManager sceneManager;
        sceneManager.ChangeScene(std::make_shared<TitleScene>(sceneManager));

        m_screenHandle = MakeScreen(Game::kScreenWidth, Game::kScreenHeight, true);

        LONGLONG time;
#ifdef DEBUG
        LONGLONG updateTime;
        LONGLONG drawTime;
#endif
        while (ProcessMessage() != -1)
        {
            // FPSの固定ように開始時の時間を取得
            time = GetNowHiPerformanceCount();
            // 現在時刻をsystem_clockを用いて取得
            auto now = std::chrono::system_clock::now();

            // 現在時刻をtime_t形式に変換
            std::time_t t = std::chrono::system_clock::to_time_t(now);
            printf("----------------\n");
            //現在時刻を表示
            TimeTool::ShowNowTime();
            ClearDrawScreen();
            if (CheckHitKey(KEY_INPUT_ESCAPE))
            {
                ChangeWindowMode(true);
            }

#ifdef DEBUG
            updateTime = GetNowHiPerformanceCount();
#endif

            Effekseer_Sync3DSetting();

            sceneManager.Update();

            UpdateEffekseer3D();

#ifdef DEBUG
            drawTime = GetNowHiPerformanceCount();
            updateTime = drawTime - updateTime;
#endif
           
            sceneManager.Draw();
            DrawEffekseer3D();

#ifdef DEBUG
            drawTime = GetNowHiPerformanceCount() - drawTime;

            DrawFormatString(16, 48, 0xff00ff, "FPS : %.2f", GetFPS());
            int uW = static_cast<int>(Game::kScreenWidth * (updateTime / 16666.6f));
            DrawBox(72, 684, 72 + uW, 700, 0xff0000, true);
            DrawFormatString(16, 684, 0x00ff00, "更新 : %d", updateTime);
            int dW = static_cast<int>(Game::kScreenWidth * (drawTime / 16666.6f));
            DrawBox(72, 700, 72 + dW, 716, 0x0000ff, true);
            DrawFormatString(16, 700, 0x00ff00, "描画 : %d", drawTime);

            DrawBox(72, 716, 72 + uW, 732, 0xff0000, true);
            DrawBox(72 + uW, 716, 72 + uW + dW, 732, 0x00ff00, true);
            DrawFormatString(16, 716, 0x00ff00, "合計 : %d", drawTime + updateTime);
#endif
           
            ScreenFlip();

            // escキーを押したら終了する
            if (CheckHitKey(KEY_INPUT_ESCAPE))	break;

            // 60FPSに固定
            while (16667 > GetNowHiPerformanceCount() - time) {};
        }
    }
    // コンソール解放
    /*fclose(out); fclose(in); FreeConsole();*/
    Terminate();

    bool memoryLeaksFlag=_CrtDumpMemoryLeaks();
   
}


const Size& Application::GetWindowSize() const
{
    return m_windowSize;
}