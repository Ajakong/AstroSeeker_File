#include "DekaHead_Blue.h"
#include"ModelManager.h"
#include"GraphManager.h"

namespace
{
	const char* kModelName = "CuteCreature_blue";
	const char* kGraphName = "cuteCreature_blue.png";
}
DekaHead_Blue::DekaHead_Blue(Vec3 pos) :
	TalkObject(pos,ModelManager::GetInstance().GetModelData(kModelName),GraphManager::GetInstance().GetGraphData(kGraphName))
{
}

DekaHead_Blue::~DekaHead_Blue()
{
}

std::list<std::list<std::string>> DekaHead_Blue::GetTexts()
{
	std::list<std::list<std::string>> talkText;
	std::list<std::string> one;
	one.push_back("Hey!Brother");
	one.push_back("調子はどうだ");
	talkText.push_back(one);

	std::list<std::string> two;
	two.push_back("俺は最近BLUEなんだ");
	two.push_back("どうやら最近宇宙で悪さしようとしている奴がいるらしい");
	talkText.push_back(two);

	std::list<std::string> three;
	three.push_back("宇宙が壊れるのは困るぜ、どうにかしたいんだが");
	three.push_back("ソイツは惑星の正面を這う衝撃波で攻撃するそうだ");
	three.push_back("普通に戦ってるだけじゃダメージを与えられないらしい");
	three.push_back("あの浮かんでる赤いタマが取れれば弱い衝撃波が出せるようになるらしいんだが");

	talkText.push_back(three);

	std::list<std::string> four;
	four.push_back("いったいどうやってとればいいんだ");
	four.push_back("そいえば俺のばあちゃんがとりたいものがあるときはLBで狙ってXって言ってたな");
	four.push_back("LBってなんだよ。ばあちゃんボケてたからな");

	talkText.push_back(four);

	return talkText;
}
