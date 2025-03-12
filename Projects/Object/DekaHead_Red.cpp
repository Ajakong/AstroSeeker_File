#include "DekaHead_Red.h"
#include"ModelManager.h"
#include"GraphManager.h"

namespace
{
	const char* kModelName = "CuteCreature_red";
	
}

DekaHead_Red::DekaHead_Red(Vec3 pos) : 
	TalkObject(pos,ModelManager::GetInstance().GetModelData(kModelName)),
	m_isUsed(false)
{

}

DekaHead_Red::~DekaHead_Red()
{

}

std::list<std::list<std::string>> DekaHead_Red::GetTexts()
{
	UI::GetInstance().SetTalkObjectHandle(UI::TalkGraphKind::Dekahead_Red);
	std::list<std::list<std::string>> talkText;
	if (m_isUsed)
	{
		std::list<std::string> one;
		one.push_back("まずは金色のタマをとってウォーミングアップだな");
		talkText.push_back(one);
	}
	else
	{
		std::list<std::string> one;
		one.push_back("俺はバンバーンだぜ");
		one.push_back("おまえが隊長だな");
		talkText.push_back(one);

		std::list<std::string> two;
		two.push_back("タカサキ大佐から話は聞いてるぜ");
		talkText.push_back(two);

		std::list<std::string> three;
		three.push_back("久しぶりの任務で体がなまってるだろうから");
		three.push_back("まずは金色のスターをとって体をうごかせってよ");
		talkText.push_back(three);
	
		m_isUsed = true;
	}
	
	return talkText;
}
