#pragma once
#include "Collidable.h"
class Crystal : public MyEngine::Collidable
{
public:
	Crystal(Vec3 pos,Vec3 norm,Vec3 size);
	virtual ~Crystal();

	void Init();
	void Update();
	void Draw();

	
	virtual void OnCollideEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag);
private:
	int m_hp;
	Vec3 m_size;

};

