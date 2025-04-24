#pragma once
#include "Player.h"
class TitlePlayer : public Player
{
public:
    TitlePlayer();
    virtual ~TitlePlayer();


    /// <summary>
    /// ターゲット地点に向けてStickerSphereを発射し移動する
    /// </summary>
    /// <param name="targetPos">目標地点</param>
    /// <returns>移動開始フラグ</returns>
    bool MoveToTargetPosWithSticker(Vec3 targetPos);

    void SetShot();

    float GetAnimBlendRate() { return m_animBlendRate; }

    void Update();
    
    /// <summary>
    /// 立ち状態
    /// </summary>
    void IdleUpdate();
    void StopUpdate();

    /// <summary>
    /// 位置を固定
    /// </summary>
    void DoNotMove();
    /// <summary>
    /// 位置を動かすように設定
    /// </summary>
    void Move();

    /// <summary>
    /// StickStarで引数に向けて移動
    /// </summary>
    void MoveToTargetWithStickStar(Vec3 targetPos);

    void SetTitleUpdateNum(int num) { m_titleUpdateNum = num; }

private:
    using titlePlayerState_t = void(TitlePlayer::*)();
    titlePlayerState_t m_titlePlayerUpdate;

    int m_titleUpdateNum;
};