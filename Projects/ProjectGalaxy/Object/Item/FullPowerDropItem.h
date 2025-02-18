#pragma once
#include "Item.h"
class FullPowerDropItem : public Item
{
public:
	FullPowerDropItem(Vec3 pos, bool antiGravity = false);
	virtual ~FullPowerDropItem();

	void Draw();
};

