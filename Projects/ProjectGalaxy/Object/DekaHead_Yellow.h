#pragma once
#include "TalkObject.h"
class DekaHead_Yellow :
    public TalkObject
{
public:
	DekaHead_Yellow(Vec3 pos);
	virtual ~DekaHead_Yellow();

	virtual std::list<std::list<std::string>> GetTexts();
};

