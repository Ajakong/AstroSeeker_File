#include "DekaHead_Green.h"
#include"ModelManager.h"
#include"GraphManager.h"
#include"Mission.h"

namespace
{
	const char* kModelName = "CuteCreature_green";
	const char* kGraphName = "cuteCreature_green.png";
}
DekaHead_Green::DekaHead_Green(Vec3 pos) :
    TalkObject(pos,ModelManager::GetInstance().GetModelData(kModelName),GraphManager::GetInstance().GetGraphData(kGraphName)),
	m_useFlag(false)
{
}

DekaHead_Green::~DekaHead_Green()
{
}

std::list<std::list<std::string>> DekaHead_Green::GetTexts()
{
	std::list<std::list<std::string>> talkText;
	if (m_useFlag)
	{
		std::list<std::string> one;
		one.push_back("加速装置はバーン君の近くにあるはずだよ");
		talkText.push_back(one);
	}
	else if (!Mission::GetInstance().Clear())
	{
		std::list<std::string> one;
		one.push_back("あの赤いヤツに僕のカードキーを奪われたんだ");
		one.push_back("だから来たくなかったのに。。。どうしよう");
		talkText.push_back(one);

		std::list<std::string> two;
		two.push_back("あのカードキーがないと惑星から脱出できる加速装置が設置されないんだ");
		talkText.push_back(two);
	}
	else
	{
		std::list<std::string> one;
		one.push_back("カードキーを取り返してくれたんだね！");
		one.push_back("ありがとう！！助かったよ！！隊長さんは頼りになるなぁ。");
		talkText.push_back(one);

		std::list<std::string> two;
		two.push_back("ちょっと待っててね");
		talkText.push_back(two);

		std::list<std::string> three;
		three.push_back("よし、これで加速装置が設置されたはずだよ！");
		three.push_back("バーン君の近くにあるはず。。。");
		talkText.push_back(three);

		m_useFlag = true;
	}
	

	return talkText;
}
