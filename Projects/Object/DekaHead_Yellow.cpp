#include "DekaHead_Yellow.h"
#include"ModelManager.h"
#include"GraphManager.h"

namespace
{
	const char* kModelName = "CuteCreature_yellow";
	const char* kGraphName = "cuteCreature_yellow.png";
}
DekaHead_Yellow::DekaHead_Yellow(Vec3 pos) : 
	TalkObject(pos,ModelManager::GetInstance().GetModelData(kModelName))
{
}

DekaHead_Yellow::~DekaHead_Yellow()
{
}

std::list<std::list<std::string>> DekaHead_Yellow::GetTexts()
{
	UI::GetInstance().SetTalkObjectHandle(UI::TalkGraphKind::Dekahead_Yellow);
	std::list<std::list<std::string>> talkText;
	std::list<std::string> one;
	one.push_back("隊長じゃん！いいところに！元気？お願い聞いて？");
	one.push_back("実はさっき走り回ってる敵に");
	one.push_back("スピンしたら跳ね返せるって聞いたから試したら、");
	one.push_back("うちが持ってた加速装置を壊されちゃって");
	talkText.push_back(one);
	std::list<std::string> two;
	two.push_back("５個ぐらいにバラバラになってどっか行ったから");
	two.push_back("探してくれない？");
	two.push_back("足ケガしちゃって動けないんだよねー");
	talkText.push_back(two);
	std::list<std::string> three;
	three.push_back("集めたら勝手にくっついて勝手に加速装置になるから");
	three.push_back("そのままスーパーマテリアルを取り返しちゃってよ！");
	talkText.push_back(three);
	std::list<std::string> four;
	four.push_back("青いやつだよ！じゃぁ、よろしくねー。");
	talkText.push_back(four);

	return talkText;
}
