#pragma once
#include "Item.h"
class Key : public Item
{
public:
	Key(Vec3 pos,int connectObjectNumber,bool antigravity = true);
	virtual ~Key();

	void Draw();

	virtual void OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);
private:
	int m_connectObjectNumber;
	int m_modelHandle;
};

