#include "Enemy.h"

namespace
{
	constexpr int kMaxHp = 50;
}

Enemy::Enemy(Priority priority, ObjectTag tag):Collidable(priority,tag),
m_hp(0),
m_modelHandle(-1)
{
}

Enemy::~Enemy()
{
}
