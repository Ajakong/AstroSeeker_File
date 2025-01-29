#include "Mission.h"

Mission::Mission() : 
	m_missionFlag(false)
{
}

Mission::~Mission()
{
}

Mission& Mission::GetInstance()
{
	static Mission info;
	return info;
}

void Mission::UpDate()
{
	(this->*m_missionUpdate)();
}

void Mission::Draw()
{
	(this->*m_missionDraw)();
}

void Mission::MoveUpdate()
{
}

void Mission::MoveDraw()
{
}
