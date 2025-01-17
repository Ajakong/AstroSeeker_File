#include "ObjectTag.h"

std::string ObjectTag_String(ObjectTag tag)
{
	{
		switch (tag)
		{
		case ObjectTag::Stage: return "Stage";
			break;
		case ObjectTag::Player: return"Player";
			break;
		case ObjectTag::Enemy: return "Enemy";
			break;
		case ObjectTag::Takobo: return "Takobo";
			break;
		case ObjectTag::Kuribo: return "Kuribo";
			break;
		case ObjectTag::Gorori: return "Gorori";
			break;
		case ObjectTag::KillerTheSeeker: return "KillerTheSeeker";
			break;
		case ObjectTag::SpaceEmperor: return "SpaceEmperor";
			break;
		case ObjectTag::EnemyAttack: return "EnemyAttack";
			break;
		case ObjectTag::PlayerBullet: return "PlayerBullet";
			break;
		case ObjectTag::PlayerImpact: return "PlayerImpact";
			break;
		case ObjectTag::EnemyBullet: return "EnemyBullet";
			break;
		case ObjectTag::WarpGate: return "WarpGate";
			break;
		case ObjectTag::FullPowerDropItem: return "FullPowerDropItem";
			break;
		case ObjectTag::StickStarItem: return "StickStarItem";
			break;
		case ObjectTag::Coin: return "Coin";
			break;
		case ObjectTag::ClearObject: return "ClearObject";
			break;
		case ObjectTag::StarCapture: return "StarCapture";
			break;
		case ObjectTag::SeekerLine: return "SeekerLine";
			break;
		case ObjectTag::Crystal: return "Crystal";
			break;
		case ObjectTag::InvisibleObject: return "InvisibleObject";
			break;
		case ObjectTag::End: return "End";
			break;
		default: return"このオブジェクトはObjectTag_Stringに定義されていません";
			break;

		}
		return"エラー : 関数ObjectTag_Stringを確認してください";
	}
}
