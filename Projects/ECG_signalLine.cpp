#include "ECG_signalLine.h"
namespace
{
	
}

ECG_signalLine::ECG_signalLine(Vec3 start, Vec3 end):
	m_signalLine({start,end})
{
	m_divisionNum = (m_signalLine.start - m_signalLine.end).Length();
	m_dir = (end - start).GetNormalized();
	
	m_signal.resize(m_divisionNum);
}

ECG_signalLine::~ECG_signalLine()
{
}

void ECG_signalLine::Update()
{
	int i = 0;
	for (auto& item : m_signal)
	{
		item = m_signalLine.start + m_dir * i;
		i++;
	}
}

void ECG_signalLine::Draw()
{
	
}
