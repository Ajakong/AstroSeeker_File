#include "DekaHead_Green.h"
#include"ModelManager.h"
#include"GraphManager.h"
#include"Mission.h"

namespace
{
	const char* kModelName = "CuteCreature_green";
}
DekaHead_Green::DekaHead_Green(Vec3 pos) :
    TalkObject(pos,ModelManager::GetInstance().GetModelData(kModelName)),
	m_isUsed(false)
{
}

DekaHead_Green::~DekaHead_Green()
{
}

std::list<std::list<std::string>> DekaHead_Green::GetTexts()
{
	UI::GetInstance().SetTalkObjectHandle(UI::TalkGraphKind::Dekahead_Green);
	std::list<std::list<std::string>> talkText;
	if (m_isUsed)
	{
		std::list<std::string> one;
		one.push_back("加速装置はバーン君の近くにあるはずだよ");
		talkText.push_back(one);
	}
	else if (!Mission::GetInstance().Clear())
	{
		std::list<std::string> one;
		one.push_back("あの赤いヤツに僕のカードキーを奪われたんだ");
		one.push_back("カードキーの管理者が狙われてるみたい。。。");
		one.push_back("どうしよう");
		talkText.push_back(one);

		std::list<std::string> two;
		two.push_back("あのカードキーがないと");
		two.push_back("惑星から脱出できる加速装置が設置されないんだ。");
		talkText.push_back(two);

		std::list<std::string> three;
		three.push_back("あのての生き物は踏みつけたら");
		two.push_back("つぶれていなくなるんだけど、僕の体重じゃ軽くて無理なんだ");
		talkText.push_back(two);
	}
	else
	{
		std::list<std::string> one;
		one.push_back("カードキーを取り返してくれたんだね！");
		one.push_back("ありがとう！！助かったよ！！");
		one.push_back("隊長さんは頼りになるなぁ。");
		talkText.push_back(one);

		std::list<std::string> two;
		two.push_back("ちょっと待っててね");
		talkText.push_back(two);

		std::list<std::string> three;
		three.push_back("よし、これで加速装置が設置されたはずだよ！");
		three.push_back("バーン君の近くにあるはず。。。");
		three.push_back("ほかの惑星の人たちも困ってるだろうから");
		three.push_back("助けてあげて！");
		talkText.push_back(three);

		m_isUsed = true;
	}
	

	return talkText;
}
