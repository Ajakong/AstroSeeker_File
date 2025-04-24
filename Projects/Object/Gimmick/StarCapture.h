#pragma once
#include "Collidable.h"
#include"ColliderSphere.h"

class Player;

class StarCapture : public MyEngine::Collidable
{
public:
	StarCapture(Vec3 pos,float radius=3.f,float captureRadius=50.f);
	virtual ~StarCapture();

	void Init();
	void Update();
	void Draw();

	virtual void OnCollideEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag);
	virtual void OnTriggerEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag);
	virtual void OnTriggerStay(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag);
	virtual void OnTriggerExit(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag);

private:
	void WaitingUpdate();
	void CaptureUpdate();

private:
	using StarCaptureState_t = void(StarCapture::*)();
	StarCaptureState_t m_stateUpdate;
	float m_ratio;
	int m_captureColLifeTime;
	int m_recastTime;
	int m_modelhandle;
	float m_radius;
	float m_captureRadius;
	int m_captureFrame;
	std::shared_ptr<Collidable::CollideInfo> m_captureCol;
	std::shared_ptr<MyEngine::Collidable> m_target;
	std::weak_ptr<Player> m_player;
	Vec3 m_playerStartPos;
	bool m_isCapturePlayer;
	bool m_isCreateCol;
};

