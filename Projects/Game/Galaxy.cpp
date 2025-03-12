#include"Galaxy.h"
#include"ScreenManager.h"
#include"Game.h"

using namespace World;

namespace
{
	const char* kModelScreenName = "ModelScreen";
}

Galaxy::Galaxy(std::shared_ptr<Player> playerPointer) :
	//フラグの初期化
	m_isClear(false),
	m_isGameOver(false),
	m_managerUpdate(nullptr),
	m_managerDraw(nullptr)
{
#ifdef _DEBUG
	//モデルのみを描画するスクリーンハンドル(デバッグ用)
	m_modelScreenHandle = ScreenManager::GetInstance().GetScreenData(kModelScreenName, Game::kScreenWidth, Game::kScreenHeight);
#endif

	player = playerPointer;
}

Galaxy::~Galaxy()
{
}
