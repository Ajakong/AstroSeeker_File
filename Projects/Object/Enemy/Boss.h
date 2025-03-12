#pragma once
#include "Enemy.h"
#include"ColliderSphere.h"
#include"StampImpact.h"
#include"ClearObject.h"

class Planet;
class Player;
class Killer;

class Boss : public Enemy
{
public:
	//アニメーション番号
	enum AnimNum : int
	{
		AnimationNumIdle,//立ち
		AnimationNumWalk,//歩きアニメーション
		AnimationNumJump,//ジャンプアニメーション
		AnimationNumTackle,//タックルアニメーション
		AnimationNumStun,//怯み
		AnimationNumStandUp,//起立
		AnimationNumRoll,//屈み
		AnimationNumPumpUp,//バフ
		AnimationNumBackFlip
	};
	Boss(Vec3 pos,std::shared_ptr<Player>player);
	virtual ~Boss();

	virtual void Init();
	virtual void Update();
	virtual void Draw();
	virtual void SetMatrix();


	bool GetIsBattle() { return m_isBattle; }

	void WakeUp() { m_isWakeUp = true; }

	
private:
	//行動決定関数群

	/// <summary>
	/// 起動
	/// </summary>
	void Wake();
	/// <summary>
	/// ダメージ
	/// </summary>
	void Away();
	/// <summary>
	/// 立ちモードに移行
	/// </summary>
	void Neutral();
	/// <summary>
	/// 三連続ジャンプ攻撃
	/// </summary>
	void TripleJump();
	/// <summary>
	/// 後隙が生まれるジャンプ攻撃
	/// </summary>
	void FullPowerJump();
	/// <summary>
	/// 突進攻撃
	/// </summary>
	void Tackle();
	/// <summary>
	/// 隙
	/// </summary>
	/// <param name="stanFrame">隙の長さ</param>
	void Stan(int stanFrame);
	/// <summary>
	/// 第1フェーズの行動
	/// </summary>
	void PhaseOneUpdate();
	/// <summary>
	/// 第2フェーズの行動
	/// </summary>
	void PhaseTwoUpdate();
	/// <summary>
	/// 第3フェーズの行動
	/// </summary>
	void PhaseThreeUpdate();

	//プレイヤー待ち状態
	void InitUpdate();
	//スタン状態から立ち状態に変わる
	void WakeUpdate();
	/// <summary>
	/// 休憩
	/// </summary>
	void RestUpdate();
	/// <summary>
	/// 攻撃受け
	/// </summary>
	void DamageUpdate();
	/// <summary>
	/// フェーズを管理する更新処理
	/// </summary>
	void NeutralUpdate();
	/// <summary>
	/// 怒り
	/// </summary>
	void AnglyUpdate();
	/// <summary>
	/// 惑星破壊
	/// </summary>
	void DestroyPlanetUpdate();
	/// <summary>
	/// 衝撃波
	/// </summary>
	void JumpingUpdate();
	/// <summary>
	/// フルパワー衝撃波
	/// </summary>
	void FullpowerJumpUpdate();
	//第2フェーズで追加される攻撃
	
	/// <summary>
	/// 突進
	/// </summary>
	void TackleUpdate();
	/// <summary>
	/// 直進
	/// </summary>
	void RunningUpdate();
	/// <summary>
	/// 着地
	/// </summary>
	void LandingUpdate();

	/// <summary>
	/// 惑星移動
	/// </summary>
	void RunawayUpdate();

	void BallAttackUpdate();

	//アニメーションの進行
	//ループしたかどうかを返す
	bool UpdateAnim(int attachNo);
	//アニメーションの変更
	void ChangeAnim(int animIndex, int speed = 1.f);

	// 衝突したとき
	virtual void OnCollideEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);
	virtual void OnCollideStay(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);
	virtual void OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);

	template <typename T>
	void DeleteObject(std::vector<std::shared_ptr<T>>& objects);
private:
	//メンバ関数ポインタ
	using bossState_t = void(Boss::*)();
	bossState_t m_bossUpdate;
	bossState_t m_phaseUpdate;

	Vec3 m_runningDir;
	Vec3 m_runawayPos;

	int m_animationSpeed;

	//0.0f:prevが再生
	//1.0:currentが再生
	int m_currentAnimNo;//現在のアニメーション
	int m_prevAnimNo;//変更前のアニメーション
	float m_animBlendRate;//アニメーションの合成割合
	int m_knockBackFrame;
	int m_jumpCount;
	
	int m_damageSoundHandle;
	int m_criticalHandle;
	int m_dropSoundHandle;

	/// <summary>
	/// マイナスの時はひるみ、もしくは動けない状態。
	/// </summary>
	int m_actionFrame;
	/// <summary>
	/// タックルするためのチャージ時間
	/// </summary>
	int m_tackleChargeFrame;
	/// <summary>
	/// 走り回ってる時間カウント
	/// </summary>
	int m_runningFrame;
	int m_color;

	int m_shotCreateFrame;

	int m_animationLoopCount;
	

	bool m_isHit;
	bool m_onColStage;
	bool m_isWakeUp;
	bool m_isTalk;
	bool m_isTackle;
	bool m_isBattle;

	bool m_animationStop;
	bool m_animationLoop;
	

	std::shared_ptr<Player> m_player;
	std::shared_ptr<Planet> m_nowPlanet;
	std::shared_ptr<MyEngine::ColliderSphere> m_collision;
	std::vector<std::shared_ptr<StampImpact>> m_impacts;
	std::vector<std::shared_ptr<Killer>> m_killer;
	std::shared_ptr<ClearObject>m_dropItem;
};

