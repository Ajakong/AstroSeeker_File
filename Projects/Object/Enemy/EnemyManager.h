#pragma once
#include <memory>
#include<list>

class Colidable;
class Enemy;

class EnemyManager
{
public:
	EnemyManager();
	virtual ~EnemyManager();

	void Update();
	void Draw();

	
private:
	std::list<std::shared_ptr<Enemy>> m_enemies;

};

