#include "ObjectTag.h"
#include<map>

namespace
{
	const std::map<ObjectTag, std::string> kString =
	{
		{ObjectTag::Stage,"Stage"},
		{ObjectTag::Player,"Player"},
		{ObjectTag::Enemy,"Enemy"},
		{ObjectTag::Boss,"Boss"},
		{ObjectTag::Takobo,"Takobo"},
		{ObjectTag::Kuribo,"Kuribo"},
		{ObjectTag::Gorori,"Gorori"},
		{ObjectTag::KillerTheSeeker,"KillerTheSeeker"},
		{ObjectTag::SpaceEmperor,"SpaceEmperor"},
		{ObjectTag::TalkObject,"TalkObject" },
		{ObjectTag::EnemyAttack,"EnemyAttack"},
		{ObjectTag::Electronic,"Electronic"},
		{ObjectTag::Killer,"Killer"},
		{ObjectTag::PlayerBullet,"PlayerBullet"},
		{ObjectTag::Cannon,"Cannon"},
		{ObjectTag::PlayerImpact,"PlayerImpact"},
		{ObjectTag::EnemyBullet,"EnemyBullet"},
		{ObjectTag::WarpGate,"WarpGate"},
		{ObjectTag::FullPowerDropItem,"FullPowerDropItem"},
		{ObjectTag::StickStarItem,"StickStarItem"},
		{ObjectTag::Coin,"Coin"},
		{ObjectTag::FragmentOfStar,"FragmentOfStar"},
		{ObjectTag::Key,"Key"},
		{ObjectTag::ClearObject,"ClearObject"},
		{ObjectTag::StarCapture,"StarCapture"},
		{ObjectTag::SeekerLine,"SeekerLine"},
		{ObjectTag::Crystal,"Crystal"},
		{ObjectTag::InvisibleObject,"InvisibleObject"},
		{ObjectTag::End,"End"},
	};
}

std::string ObjectTag_String(ObjectTag tag)
{
	//ここで止まったら定数内にオブジェクトが定義されていない
	return kString.at(tag);
}
