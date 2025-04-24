#pragma once
#include"Vec3.h"
#include<list>

struct SignalLine
{
	Vec3 start;
	Vec3 end;
};

class ECG_signalLine
{
public:
	ECG_signalLine(Vec3 start,Vec3 end);
	virtual ~ECG_signalLine();

	void Update();
	void Draw();
private:
	SignalLine m_signalLine;
	std::list<Vec3> m_signal;
	Vec3 m_dir;
	int m_divisionNum;
};

