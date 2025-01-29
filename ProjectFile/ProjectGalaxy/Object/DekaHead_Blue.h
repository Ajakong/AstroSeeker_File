#pragma once
#include "TalkObject.h"
class DekaHead_Blue : public TalkObject
{
public:
	DekaHead_Blue(Vec3 pos);
	virtual ~DekaHead_Blue();

	virtual std::list<std::list<std::string>> GetTexts();

};

