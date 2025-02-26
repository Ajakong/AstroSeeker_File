#pragma once
#include "PlayerSphere.h"
class PlayerStickSphere : public PlayerSphere
{
public:
	PlayerStickSphere(MyEngine::Collidable::Priority priority, ObjectTag tag, std::shared_ptr<MyEngine::Collidable>player, Vec3 pos, Vec3 velocity, Vec3 sideVec, int moveNum, int color = 0xff0000);
	virtual ~PlayerStickSphere();

	void Init()override;
	void Update()override;
	void Draw()override;

	void Effect();


	virtual void OnCollideEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);
	virtual void OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);
	//std::shared_ptr<Player>m_player;

protected:
	virtual void StraightUpdate();//球を直線状に飛ばす
	virtual void StickUpdate();
	virtual void ComeBackUpdate();
	virtual void ComeBackWithObjectUpdate();
protected:
	Vec3 m_sideVec;
	int m_lifeTime;
	int m_pushCount;
	int m_operationHandle;

	int m_gaussFilterScreen;
	int m_highBrightScreen;
	/// <summary>
	/// 接触しているオブジェクトを動かせるか
	/// </summary>
	bool m_isMoving;

private:
	/// <summary>
	/// 接触しているオブジェクト
	/// </summary>
	std::shared_ptr<MyEngine::Collidable> m_contactedCollidable;
	Vec3 m_collidableContactPosition;
	//メンバ関数ポインタ
	using MoveState_t = void(PlayerStickSphere::*)();
	MoveState_t m_moveUpdate;
};

