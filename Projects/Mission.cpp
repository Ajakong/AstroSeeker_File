#include "Mission.h"
#include"Player.h"
#include"UI.h"
#include<string>
#include<list>

#include"Pad.h"


namespace
{
	constexpr int kMissionClearFrameMove = 100;
}

Mission::Mission() : 
	m_missionFlag(false),
	m_moveFrame(0)
{
	m_missionUpdate = &Mission::StartUpdate;
	m_missionDraw = &Mission::MoveDraw;
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
	if (Pad::IsTrigger(PAD_INPUT_7))
	{
		if (m_missionUpdate != &Mission::SkipUpdate)
		{
			UI::GetInstance().SetTalkObjectHandle(UI::TalkGraphKind::TakasakiTaisa);
			UI::GetInstance().ClearText();
			UI::GetInstance().InText("貴様には必要ないか・・・");
			m_missionUpdate = &Mission::SkipUpdate;
			m_missionDraw = &Mission::SkipDraw;
		}
		else
		{
			UI::GetInstance().SetTalkObjectHandle(UI::TalkGraphKind::TakasakiTaisa);
			UI::GetInstance().ClearText();
			std::list<std::string> texts1;
			texts1.push_back("やはり必要か？");
			texts1.push_back("ではまず左スティックで歩いてみろ");
			texts1.push_back("基本を厳かにするんじゃないぞ");
			UI::GetInstance().InTexts(texts1);
			m_missionUpdate = &Mission::MoveUpdate;
			m_missionDraw = &Mission::MoveDraw;
		}
	}
}

void Mission::Draw()
{
	(this->*m_missionDraw)();
}

void Mission::StartUpdate()
{
	std::list<std::string> texts1;
	texts1.push_back("そういえばドレイク、貴様は軍から抜けて長いだろう");
	texts1.push_back("体がなまってるんじゃないか？");


	UI::GetInstance().InTexts(texts1);

	std::list<std::string> mission;
	mission.push_back("鍛えなおしてやる");
	mission.push_back("まずは左スティックを動かして歩いてみろ");
	UI::GetInstance().InTexts(mission);
	m_missionUpdate = &Mission::MoveUpdate;
	m_missionDraw = &Mission::MoveDraw;
}

void Mission::MoveUpdate()
{
	//歩き中
	if (m_player->GetState() == MyEngine::Collidable::State::Walk)
	{
		m_moveFrame++;
		//歩いたフレームが一定数を超えたら
		if (m_moveFrame > kMissionClearFrameMove)
		{
			UI::GetInstance().SetTalkObjectHandle(UI::TalkGraphKind::TakasakiTaisa);
			UI::GetInstance().InText("これが歩き・・・だ！");
			std::list<std::string> nextMission;
			nextMission.push_back("次はジャンプだ！");
			nextMission.push_back("Aを押せばジャンプができると言い伝えられている。");
			UI::GetInstance().InTexts(nextMission);
			m_moveFrame = 0;
			m_missionUpdate = &Mission::JumpUpdate;
			m_missionDraw = &Mission::JumpDraw;
		}
	}
}

void Mission::MoveDraw()
{
}

void Mission::JumpUpdate()
{
	//状態がジャンプになったら
	if (m_player->GetPostState() == MyEngine::Collidable::State::Jump)
	{
		UI::GetInstance().SetTalkObjectHandle(UI::TalkGraphKind::TakasakiTaisa);
		UI::GetInstance().InText("完璧だな。");
		std::list<std::string> nextMission;
		nextMission.push_back("お次はダッシュだ！");
		nextMission.push_back("RB(R1)を押しながら移動することでダッシュできるぞ");
		nextMission.push_back("ダッシュができん奴から戦場で死んでいく。");
		UI::GetInstance().InTexts(nextMission);
		m_missionUpdate = &Mission::DashUpdate;
		m_missionDraw = &Mission::DashDraw;
	}
}

void Mission::JumpDraw()
{

}

void Mission::DashUpdate()
{
	//ダッシュ中
	if (m_player->GetState() == MyEngine::Collidable::State::Dash)
	{
		m_moveFrame++;
		//歩いたフレームが一定数を超えたら
		if (m_moveFrame > kMissionClearFrameMove)
		{
			UI::GetInstance().SetTalkObjectHandle(UI::TalkGraphKind::TakasakiTaisa);
			UI::GetInstance().InText("そうだ、危険が迫れば逃げることを忘れるな。");
			std::list<std::string> nextMission;
			nextMission.push_back("お次は大ジャンプだ！");
			nextMission.push_back("RB(R1)を押しながらジャンプをすれば高く飛べるぞ");
			nextMission.push_back("都合上という魔法の力だ。さあやってみろ！");
			UI::GetInstance().InTexts(nextMission);
			m_moveFrame = 0;
			m_missionUpdate = &Mission::DashJumpUpdate;
			m_missionDraw = &Mission::DashJumpDraw;
		}
	}
}

void Mission::DashDraw()
{
}

void Mission::DashJumpUpdate()
{
	//状態がジャンプになったら
	if (m_player->GetPostState() == MyEngine::Collidable::State::DashJump)
	{
		UI::GetInstance().SetTalkObjectHandle(UI::TalkGraphKind::TakasakiTaisa);
		UI::GetInstance().InText("よし！貴様にはもう教えることはない！");
		std::list<std::string> nextMission;
		nextMission.push_back("では最後にスピンだ！");
		nextMission.push_back("とりあえず回ればなんとかなるというのがこの世界の真理だ。");
		nextMission.push_back("Bボタンで回れるぞ、さあやってみろ！");
		UI::GetInstance().InTexts(nextMission);
		m_missionUpdate = &Mission::SpinUpdate;
		m_missionDraw = &Mission::SpinDraw;
	}
}

void Mission::DashJumpDraw()
{
	
}

void Mission::SpinUpdate()
{
	//状態がスピンになったら
	if (m_player->GetPostState() == MyEngine::Collidable::State::Spin)
	{
		UI::GetInstance().SetTalkObjectHandle(UI::TalkGraphKind::TakasakiTaisa);
		UI::GetInstance().InText("カンを取り戻してきたようだな。");
	
		UI::GetInstance().InText("この宇宙に生きる命は君にかかっている");

		std::list<std::string> text3;

		text3.push_back("私も随時サポートするからよろしく頼む");
		text3.push_back("そうだ、最後に伝えなければならないことがある。");
		UI::GetInstance().InTexts(text3);
		UI::GetInstance().InText("笑顔を忘れるな！");


		m_missionUpdate = &Mission::EmptyUpdate;
		m_missionDraw = &Mission::EmptyDraw;
	}
}

void Mission::SpinDraw()
{
}

void Mission::EmptyUpdate()
{
}

void Mission::EmptyDraw()
{
}

void Mission::SkipUpdate()
{

}

void Mission::SkipDraw()
{
}
