#pragma once
#include "Enemy.h"
#include"ColliderSphere.h"

class Item;

using namespace MyEngine;
class Kuribo : public Enemy
{
public:

	/// <summary>
/// アニメーション番号
/// </summary>
	enum AnimNum : int
	{
		AnimationNumAttack,
		AnimationNumIdle,
		AnimationNumFullPowerAttack,
		AnimationNumRoar,
		AnimationNumRun,
		AnimationNumSleep,
		AnimationNumWalk,
	};

	Kuribo(Vec3 pos);
	virtual ~Kuribo();

	virtual void Init();
	virtual void Update();
	virtual void SetMatrix();
	virtual void Draw();


	virtual void OnCollideEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag);
	virtual void OnTriggerStay(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag);

	//メンバ関数ポインタ
	using MoveState_t = void(Kuribo::*)();
	MoveState_t m_moveUpdate;

protected:
	/// <summary>
	/// アイドル時
	/// </summary>
	void SearchUpdate();
	/// <summary>
	/// ジャンプ時
	/// </summary>
	void JumpUpdate();
	/// <summary>
	/// 敵発見時
	/// </summary>
	void ChaseUpdate();
	/// <summary>
	/// 初期位置に戻る時
	/// </summary>
	void ComebackUpdate();
	/// <summary>
	///　気絶時
	/// </summary>
	void StanUpdate();
	/// <summary>
	/// つぶれ状態時
	/// </summary>
	void CrushUpdate();
	/// <summary>
	/// 死亡時
	/// </summary>
	virtual void DeathUpdate();

	//アニメーションの進行
	//ループしたかどうかを返す
	bool UpdateAnim(int attachNo);
	//アニメーションの変更
	void ChangeAnim(int animIndex, float speed = 1.f);

	template <typename T>
	void DeleteObject(std::vector<std::shared_ptr<T>>& objects);

protected:
	Vec3 m_vec;
	Vec3 m_attackDir;
	Vec3 m_moveShaftPos;
	Vec3 m_normVec;
	Vec3 m_nowPlanetPos;
	Vec3 m_comebackPoint;
	Vec3 m_targetPoint;
	std::shared_ptr<Collidable> m_player;
	std::shared_ptr<ColliderSphere> m_bodyCol;
	std::shared_ptr<ColliderSphere> m_searchCol;
	std::shared_ptr<Item>m_dropItem;


	int m_chaseFrameCount;
	int m_initTime;

	float m_animationSpeed;
	int m_stanCount;
	
	//0.0f:prevが再生
	//1.0:currentが再生
	int m_currentAnimNo;//現在のアニメーション
	int m_prevAnimNo;//変更前のアニメーション
	float m_animBlendRate;//アニメーションの合成割合
	
};

