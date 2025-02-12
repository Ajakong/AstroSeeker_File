#pragma once
#include<vector>
#include<memory>

class Player;

class Mission
{
public:
	Mission();
	virtual ~Mission();

	static Mission& GetInstance();

	void UpDate();
	void Draw();

	void SetPlayer(std::shared_ptr<Player> pPlayer) {  m_player = pPlayer; };

	void StartUpdate();

	void MoveUpdate();
	void MoveDraw();

	void JumpUpdate();
	void JumpDraw();

	void DashUpdate();
	void DashDraw();

	void DashJumpUpdate();
	void DashJumpDraw();

	void SpinUpdate();
	void SpinDraw();

	void EmptyUpdate();
	void EmptyDraw();

	void SkipUpdate();
	void SkipDraw();

	void MissionClear() { m_missionFlag = true; }
	bool Clear() { return m_missionFlag; }

	//メンバ関数ポインタ
	using missionState_t = void(Mission::*)();
	missionState_t m_missionUpdate;
	missionState_t m_missionDraw;

	std::shared_ptr<Player> m_player;

	bool m_missionFlag;
	
	int m_moveFrame;

};

