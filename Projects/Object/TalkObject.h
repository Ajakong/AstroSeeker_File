#pragma once
#include"Collidable.h"
#include<list>
#include<string>
#include"Quaternion.h"
#include"UI.h"

class Quaternion;

class TalkObject : public MyEngine::Collidable
{
public:
	TalkObject(Vec3 pos,int modelHandle);
	virtual ~TalkObject();

	virtual void Init();
	virtual void Update();
	virtual void Draw();
	virtual void SetMatrix();

	virtual std::list<std::list<std::string>> GetTexts();

	virtual void CanWeJustTalk();
	virtual void WaitingUpdate();

	virtual void NormalDraw();

	virtual void OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);
	virtual void OnTriggerStay(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);
	virtual void OnTriggerExit(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);

protected:
	std::list<std::list<std::string>> m_talkTexts;

	using talkObjectState_t = void(TalkObject::*)();
	talkObjectState_t m_update;
	talkObjectState_t m_draw;

	Quaternion m_myQ;
	int m_modelHandle;
	bool m_canTalk;
};

