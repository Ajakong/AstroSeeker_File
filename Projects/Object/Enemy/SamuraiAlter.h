#pragma once
#include "Enemy.h"

#include"ColliderSphere.h"
class SamuraiAlter : public Enemy
{
public:

	/// <summary>
	/// アニメーション番号
	/// </summary>
	enum AnimNum : int
	{
		AnimationNumIdle,
		AnimationNumRun,
	};

	SamuraiAlter(Vec3 pos);
	~SamuraiAlter();

	void Init();
	void Update();
	void SetMatrix();
	void Draw();

	void SetTarget(std::shared_ptr<Collidable> target) { m_target = target; }
	void OnBossPlanet() {};
	bool GetIsFind() { return m_isFindTarget; }
	Vec3 GetNeckPos() const { return m_neckPos; }
	
	//メンバ関数ポインタ
	using emperorState_t = void(SamuraiAlter::*)();
	emperorState_t m_update;
private:

	void DoNothingUpdate() {};
	void IntroUpdate();
	void IdleUpdate();
	void AttackUpdate();
	void HitUpdate() {};

	//アニメーションの進行
	//ループしたかどうかを返す
	bool UpdateAnim(int attachNo);
	//アニメーションの変更
	void ChangeAnim(int animIndex, float speed = 1.f);

	std::shared_ptr<MyEngine::ColliderSphere> m_armCol;

	bool m_isFindTarget;

	int m_currentAnimNo;//現在のアニメーション
	int m_prevAnimNo;//変更前のアニメーション
	float m_animBlendRate;//アニメーションの合成割合

	int m_neckFrameIndex;//モデルの首のフレームのインデックス

	float m_animSpeed;
	Vec3 m_hitDir;
	Vec3 m_neckNowDir;
	Vec3 m_neckPos;
	Vec3 m_armMoveDir;
	Vec3 m_moveDir;
	std::shared_ptr<Collidable> m_target;

};

