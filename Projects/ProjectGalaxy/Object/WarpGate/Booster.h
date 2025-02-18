#pragma once
#include "Collidable.h"

class Booster : public MyEngine::Collidable
{
public:
	Booster(Vec3 pos, Vec3 Dir, int handle,float power = 2.f,bool isActive=false);
	virtual ~Booster();

	void SetWarpPos(Vec3 warpPos) { m_warpPos = warpPos; }

	void Init();
	void Update();
	void SetEffectPos();
	void Draw();
	Vec3 GetNowPlanetPos() { return m_nowPlanetPos; }

	virtual void OnCollideEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag);
	virtual void OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);
private:
	//アニメーションの進行
	//ループしたかどうかを返す
	bool UpdateAnim(int attachNo);
	//アニメーションの変更
	void ChangeAnim(int animIndex);

	Vec3 m_dir;
	Vec3 m_warpPos;
	bool m_isActive;
	float m_power;
	Vec3 m_nowPlanetPos;
	int m_gaussScreenHandle;
	int m_colorScreenHandle;
	int m_highBrightScreenHandle;
	int m_screenHandle;
	int m_modelHandle;

	//アニメーション変数

	//0.0f:prevが再生
	//1.0:currentが再生
	int m_currentAnimNo;//現在のアニメーション
	int m_prevAnimNo;//変更前のアニメーション
	float m_animBlendRate;//アニメーションの合成割合
};

