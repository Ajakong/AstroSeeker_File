#include "Planet.h"
#include"MyLib/Physics/ColliderBase.h"


Planet::Planet(int modelHandle):Collidable(Priority::Static, ObjectTag::Stage),
m_isSearch(false),
	gravityPower(0),
	modelH(modelHandle),
	m_scale(0)
{
	
}

Planet::~Planet()
{
}

void Planet::Init()
{
}

void Planet::Update()
{
}

void Planet::Draw()
{
	
}