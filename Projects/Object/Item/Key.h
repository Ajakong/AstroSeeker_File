#pragma once
#include "Item.h"
class Key : public Item
{
public:
	Key(Vec3 pos,Vec3 velocity,int connectObjectNumber,bool antigravity = true);
	virtual ~Key();

	void Init();
	void Update();
	void Draw();

	virtual void OnCollideEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);
	virtual void OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);

private:
	int m_connectObjectNumber;
	int m_modelHandle;

	int m_getSoundEffectHandle;
};

