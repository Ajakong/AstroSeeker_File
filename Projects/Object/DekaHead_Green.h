#pragma once
#include "TalkObject.h"
class DekaHead_Green :
    public TalkObject
{
public:
	DekaHead_Green(Vec3 pos);
	virtual ~DekaHead_Green();

	virtual std::list<std::list<std::string>> GetTexts();

	bool m_isUsed;
};

