#pragma once
#include "TalkObject.h"
class DekaHead_White :
    public TalkObject
{
public:
	DekaHead_White(Vec3 pos);
	virtual ~DekaHead_White();

	virtual std::list<std::list<std::string>> GetTexts();
};

