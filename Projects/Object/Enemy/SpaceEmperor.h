#pragma once
#include "Enemy.h"
#include"ColliderSphere.h"
using namespace MyEngine;

class SpaceEmperor : public Enemy
{
public:
	SpaceEmperor(Vec3 pos);
	virtual ~SpaceEmperor();

	void ShaderInit();
	
	void Init();
	void Update();
	void SetMatrix();
	void Draw();

	void SetTarget(std::shared_ptr<Collidable> target) { m_target = target; }
	void OnBossPlanet();
	bool GetIsFind() { return m_isFindTarget; }
	Vec3 GetNeckPos() const{ return m_neckPos; }
	virtual void OnCollideEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag);

private:
	void DoNothingUpdate();
	void IntroUpdate();
	void IdleUpdate();
	void AttackUpdate();
	void HitUpdate();
	void DeathUpdate();

	//アニメーションの進行
	//ループしたかどうかを返す
	bool UpdateAnim(int attachNo);
	//アニメーションの変更
	void ChangeAnim(int animIndex, float speed = 1.f);

private:
	//メンバ関数ポインタ
	using emperorState_t = void(SpaceEmperor::*)();
	emperorState_t m_update;

	std::shared_ptr<MyEngine::ColliderSphere> m_armCol;

	bool m_isFindTarget;
	int m_vsH;//バーテックスシェーダーハンドル
	int m_currentAnimNo;//現在のアニメーション
	int m_prevAnimNo;//変更前のアニメーション
	int m_neckFrameIndex;//モデルの首のフレームのインデックス
	float m_animBlendRate;//アニメーションの合成割合
	float m_armExtensionSpeed;
	float m_armExtensionDistance;
	float m_animSpeed;
	Vec3 m_hitDir;
	Vec3 m_neckNowDir;
	Vec3 m_neckPos;
	Vec3 m_armMoveDir;
	std::shared_ptr<Collidable> m_target;

};

