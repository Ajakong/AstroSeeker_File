#pragma once
#include<string>

enum class ObjectTag
{
	Stage,
	Player,
	Enemy,
	Takobo,
	Kuribo,
	Gorori,
	KillerTheSeeker,
	SpaceEmperor,
	EnemyAttack,
	PlayerBullet,
	PlayerImpact,
	EnemyBullet,
	WarpGate,
	FullPowerDropItem,
	StickStarItem,
	Coin,
	ClearObject,
	StarCapture,
	SeekerLine,
	Crystal,
	InvisibleObject,
	End//一番最後のタグ
};

 
std::string ObjectTag_String(ObjectTag tag);
