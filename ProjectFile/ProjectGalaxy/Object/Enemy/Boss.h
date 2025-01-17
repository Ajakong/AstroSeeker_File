#pragma once
#include "Enemy.h"
#include"ColliderSphere.h"
#include"StampImpact.h"
#include"ClearObject.h"

class Planet;
class Boss : public Enemy
{
public:
	Boss(Vec3 pos);
	virtual ~Boss();

	virtual void Init();
	virtual void Update();
	virtual void Draw();

private:

	void InitUpdate();
	void RestUpdate();
	void NeutralUpdate();
	void AnglyUpdate();
	void DestroyPlanetUpdate();
	void KnockBackUpdate();
	void JumpingUpdate();
	void FullpowerJumpUpdate();
	void LandingUpdate();

	//アニメーションの進行
	//ループしたかどうかを返す
	bool UpdateAnim(int attachNo);
	//アニメーションの変更
	void ChangeAnim(int animIndex, int speed = 1.f);

	// 衝突したとき
	virtual void OnCollideEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);
	virtual void OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);

	template <typename T>
	void DeleteObject(std::vector<std::shared_ptr<T>>& objects);
private:
	//メンバ関数ポインタ
	using bossState_t = void(Boss::*)();
	bossState_t m_bossUpdate;

	int m_animationSpeed;

	//0.0f:prevが再生
	//1.0:currentが再生
	int m_currentAnimNo;//現在のアニメーション
	int m_prevAnimNo;//変更前のアニメーション
	float m_animBlendRate;//アニメーションの合成割合
	int m_knockBackFrame;
	int m_jumpCount;
	/// <summary>
	/// マイナスの時はひるみ、もしくは動けない状態。
	/// </summary>
	int m_actionFrame;


	int m_color;

	bool m_isHit;
	std::shared_ptr<Planet> m_nowPlanet;
	std::shared_ptr<MyEngine::ColliderSphere> m_collision;
	std::vector<std::shared_ptr<StampImpact>> m_impacts;
	std::shared_ptr<ClearObject>m_dropItem;
};

