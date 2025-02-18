#pragma once
#include "Item.h"
class StickStarItem : public Item
{

public:
	StickStarItem(Vec3 pos, bool antiGravity = false);
	virtual ~StickStarItem();
};

