#pragma once
#include "Item.h"

class FragmentOfStar : public Item
{
public:
	FragmentOfStar(Vec3 pos, bool antiGravity = false);
	virtual ~FragmentOfStar();

	void Update();
	void Draw();
	

private:
	int m_modelHandle;
};

