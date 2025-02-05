#include "DekaHead_Blue.h"
#include"ModelManager.h"
#include"GraphManager.h"

namespace
{
	const char* kModelName = "CuteCreature_blue";
}
DekaHead_Blue::DekaHead_Blue(Vec3 pos) :
	TalkObject(pos,ModelManager::GetInstance().GetModelData(kModelName))
{
}

DekaHead_Blue::~DekaHead_Blue()
{
}

std::list<std::list<std::string>> DekaHead_Blue::GetTexts()
{
	UI::GetInstance().SetTalkObjectHandle(UI::TalkGraphKind::Dekehead_Blue);
	std::list<std::list<std::string>> talkText;
	std::list<std::string> one;
	one.push_back("Hey!Brother");
	one.push_back("調子はどうだ");
	talkText.push_back(one);

	std::list<std::string> two;
	two.push_back("俺は今BLUEなんだ");
	two.push_back("さっきあのでっけぇタマが赤のマテリアルをこぼしていったんだ。");
	talkText.push_back(two);

	std::list<std::string> three;
	three.push_back("俺の考えでは、スーパーマテリアルを奪ったのはあいつだ。");
	three.push_back("赤のマテリアルがあったら衝撃波を出せるんだ");
	
	talkText.push_back(three);

	std::list<std::string> four;
	four.push_back("それであいつと戦えるようになるんだが、");
	four.push_back("俺じゃあの赤のマテリアルに届かねぇ");
	four.push_back("宇宙を守るために何とかしねぇと・・・！");

	talkText.push_back(four);

	return talkText;
}
