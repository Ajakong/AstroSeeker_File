#pragma once
#include<vector>

class Mission
{
public:
	Mission();
	virtual ~Mission();

	static Mission& GetInstance();
	void UpDate();
	void Draw();

	void MoveUpdate();
	void MoveDraw();
	void MissionClear() { m_missionFlag = true; }
	bool Clear() { return m_missionFlag; }

	//メンバ関数ポインタ
	using missionState_t = void(Mission::*)();
	missionState_t m_missionUpdate;
	missionState_t m_missionDraw;

	bool m_missionFlag;

};

