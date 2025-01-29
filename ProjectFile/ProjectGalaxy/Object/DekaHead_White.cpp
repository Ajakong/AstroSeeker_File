#include "DekaHead_White.h"
#include"ModelManager.h"
#include"GraphManager.h"

namespace
{
	const char* kModelName = "CuteCreature_white";
	const char* kGraphName = "cuteCreature_white.png";
}
DekaHead_White::DekaHead_White(Vec3 pos) : 
	TalkObject(pos,ModelManager::GetInstance().GetModelData(kModelName),GraphManager::GetInstance().GetGraphData(kGraphName))
{
}

DekaHead_White::~DekaHead_White()
{
}

std::list<std::list<std::string>> DekaHead_White::GetTexts()
{
	std::list<std::list<std::string>> talkText;
	std::list<std::string> one;
	one.push_back("みんな死ぬ");
	one.push_back("");
	one.push_back("すべて無に帰す");
	talkText.push_back(one);

	return talkText;
}
