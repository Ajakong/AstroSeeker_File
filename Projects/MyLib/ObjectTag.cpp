#include "ObjectTag.h"
#include<map>

namespace
{
	const std::map<ObjectTag, std::string> kString =
	{
		{ObjectTag::Stage,"Stage"},
		{ObjectTag::Player,"Player"},
		{ObjectTag::Stage,"Enemy"},
		{ObjectTag::Stage,"Takobo"},
		{ObjectTag::Stage,"Kuribo"},
		{ObjectTag::Stage,"Gorori"},
		{ObjectTag::Stage,"KillerTheSeeker"},
		{ObjectTag::Stage,"SpaceEmperor"},
		{ObjectTag::Stage,"EnemyAttack"},
		{ObjectTag::Stage,"PlayerBullet"},
		{ObjectTag::Stage,"EnemyBullet"},
		{ObjectTag::Stage,"WarpGate"},
		{ObjectTag::Stage,"FullPowerDropItem"},
		{ObjectTag::Stage,"StickStarItem"},
		{ObjectTag::Stage,"Coin"},
		{ObjectTag::Stage,"ClearObject"},
		{ObjectTag::Stage,"StarCapture"},
		{ObjectTag::Stage,"SeekerLine"},
		{ObjectTag::Stage,"Crystal"},
		{ObjectTag::Stage,"InvisibleObject"},
		{ObjectTag::Stage,"End"},
	};

}

std::string ObjectTag_String(ObjectTag tag)
{
	//ここで止まったら定数内にオブジェクトが定義されていない
	return kString.at(tag);
}
