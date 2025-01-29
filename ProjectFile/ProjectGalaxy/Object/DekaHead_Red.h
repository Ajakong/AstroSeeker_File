#pragma once
#include "TalkObject.h"
class DekaHead_Red : public TalkObject
{
public:
	DekaHead_Red(Vec3 pos);
	virtual ~DekaHead_Red();

	virtual std::list<std::list<std::string>> GetTexts();

	bool m_useFlag;
};

