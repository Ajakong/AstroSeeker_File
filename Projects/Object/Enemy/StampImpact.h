#pragma once
#include<cmath>
#include<list>
#include"ColliderLine3D.h"
#include"Collidable.h"
class Player;

class StampImpact :public MyEngine::Collidable 
{
public:
	/// <summary>
	/// 衝撃波コンストラクタ
	/// </summary>
	/// <param name="pos">発生位置</param>
	/// <param name="planetRadius">衝撃波が走る惑星の半径</param>
	/// <param name="dir">衝撃波の進む道</param>
	/// <param name="tag">衝撃波のタグ</param>
	/// <param name="speed">衝撃波のスピード</param>
	StampImpact(Vec3 pos,float planetRadius,Vec3 dir,ObjectTag tag,float speed=1.f);
	virtual ~StampImpact();

	void Init();
	void Update();
	void Draw();

    bool GetDeleteFlag() { return m_deleteFlag; }
	virtual void OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);
   
private:
    void Draw3DCircle(Vec3 center, float radius, int num_segments);
    void ExpansionUpdate();
    void ReductionUpdate();

private:
	//メンバ関数ポインタ
	using impactState_t = void(StampImpact::*)();
	impactState_t m_impactUpdate;

	Vec3 m_dir;
	std::vector<std::shared_ptr<MyEngine::ColliderLine3D>>m_lines;

	int m_lifeTime;
	float m_radiusMax;
	float m_nowRadius;
	float m_speed;

    bool m_deleteFlag;
};