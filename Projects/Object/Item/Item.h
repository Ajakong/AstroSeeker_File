#pragma once
#include "Collidable.h"
#include"ColliderSphere.h"

using namespace MyEngine;

class Item : public MyEngine::Collidable
{
public:
	/// <summary>
	/// プレイヤーは引数のObjectTagをみて効果を使い分ける。ItemでPlayerのプロパティをいじる処理は書かない。そうそれはプラシーボ効果
	/// </summary>
	/// <param name="pos"></param>
	/// <param name="tag">アイテムの種類</param>
	/// <param name="antiGravity"></param>
	Item(Vec3 pos, ObjectTag tag, bool antiGravity=false);
	virtual ~Item();

	virtual void Init();
	virtual void Update();
	virtual void Draw();

	Vec3 GetUpVec() { return m_upVec; }
	virtual void OnCollideEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag);
	virtual void OnTriggerEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag);
	virtual void OnTriggerStay(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);

protected:
	std::shared_ptr<ColliderSphere> m_col;
	float m_angle;
	Vec3 m_nowPlanetPos;
};

