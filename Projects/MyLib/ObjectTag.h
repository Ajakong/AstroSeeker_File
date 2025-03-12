#pragma once
#include<string>

enum class ObjectTag
{
	Stage,
	Player,
	Enemy,
	Boss,
	Takobo,
	Kuribo,
	Gorori,
	KillerTheSeeker,
	SpaceEmperor,
	TalkObject,
	EnemyAttack,
	Electronic,
	Killer,
	PlayerBullet,
	Cannon,
	PlayerImpact,
	EnemyBullet,
	WarpGate,
	FullPowerDropItem,
	StickStarItem,
	Coin,
	FragmentOfStar,
	Key,
	ClearObject,
	StarCapture,
	SeekerLine,
	Crystal,
	InvisibleObject,
	End//一番最後のタグ
};

/// <summary>
/// 受け取ったタグをStringで返す
/// </summary>
/// <param name="tag"></param>
/// <returns></returns>
std::string ObjectTag_String(ObjectTag tag);
