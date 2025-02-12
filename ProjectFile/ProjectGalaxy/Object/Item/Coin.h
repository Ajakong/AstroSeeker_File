#pragma once
#include "Item.h"

#include"ColliderSphere.h"
#include"Physics.h"
#include"Quaternion.h"

class Coin :
    public Item
{
public:
    Coin(Vec3 pos, bool antiGravity = false);
    virtual ~Coin();

    void Init();
    void Update();
    void Draw();

    virtual void OnTriggerEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag);
private:
    Quaternion m_myQ;

    int m_getSoundEffectHandle;
    int m_modelHandle;
    int m_rotateYAngle;
};

