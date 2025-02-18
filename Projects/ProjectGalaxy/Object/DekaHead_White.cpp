#include "DekaHead_White.h"
#include"ModelManager.h"
#include"GraphManager.h"

namespace
{
	const char* kModelName = "CuteCreature_white";
	const char* kGraphName = "cuteCreature_white.png";
}
DekaHead_White::DekaHead_White(Vec3 pos) : 
	TalkObject(pos,ModelManager::GetInstance().GetModelData(kModelName))
{
}

DekaHead_White::~DekaHead_White()
{
}

std::list<std::list<std::string>> DekaHead_White::GetTexts()
{
	UI::GetInstance().SetTalkObjectHandle(UI::TalkGraphKind::Dekahead_White);
	std::list<std::list<std::string>> talkText;
	std::list<std::string> one;
	one.push_back("僕のカードキー奪われちゃった。");
	
	talkText.push_back(one);

	return talkText;
}
