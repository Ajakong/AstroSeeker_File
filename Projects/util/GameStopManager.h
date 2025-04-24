#pragma once
#include<memory>
class GameManager;

class GameStopManager
{
public:
	
	~GameStopManager();
	static GameStopManager& GetInstance();

	void SetGameManager(std::shared_ptr<GameManager> manager);
	void SetStopFrame(int stopFrame);
private:
	GameStopManager();

private:
	
	std::shared_ptr<GameManager> m_gameManager;

};

