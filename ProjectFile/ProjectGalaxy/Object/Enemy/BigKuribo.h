#pragma once
#include "Kuribo.h"
class BigKuribo : public Kuribo
{
public:
	BigKuribo(Vec3 pos,int connectNum);
	virtual ~BigKuribo();

	void DeathUpdate();

protected:
	int m_connectObjectNumber;
};

