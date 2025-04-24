#include "GameManager.h"
#include"Physics.h"
#include"SerialPlanetGalaxy.h"
#include"DebugGalaxy.h"
#include"Player.h"
#include"ModelManager.h"
#include"EffectManager.h"
#include"GameStopManager.h"
#include"UI.h"
#include"Mission.h"
using namespace MyEngine;

namespace
{

	constexpr float kFirstPosY = 20.f;

	const char* kPlayerFileName = "SpaceHarrier.mv1";


}

GameManager::GameManager() : 
	m_updateStopFrame(0),
	m_player(std::make_shared<Player>(Vec3(0.f, kFirstPosY,0.f)))
{
	//ステージの追加
	m_galaxy.push_back(std::make_shared<SerialPlanetGalaxy>(m_player));
}

GameManager::~GameManager()
{
}

void GameManager::Init()
{
	//このシーンをどこからでも一時停止できるようにする
	GameStopManager::GetInstance().SetGameManager(shared_from_this());
	//ステージの初期化
	m_galaxy.back()->Init();
}

void GameManager::Update()
{
	//UIの更新
	UI::GetInstance().Update();
	//チュートリアルの更新(そのアクションが必要になったときに都度割り込んでくる)
	Mission::GetInstance().UpDate();
	//エフェクトの更新
	EffectManager::GetInstance().Update();
	
	//整数の間ゲームの更新処理が止まる(普段は負の数で止まる際に止まる時間正の数が代入される)
	m_updateStopFrame--;

	//更新処理を止める時間が残っていないかつテキスト描画モードではない
	if (m_updateStopFrame < 0&&UI::GetInstance().GetState()!=&UI::TextBoxUpdate)
	{
		//ステージやオブジェクトの更新
		m_galaxy.back()->Update();

	}
	
	//ステージでゲームオーバーフラグがたったら
	if (m_galaxy.back()->GetGameOver())
	{
		//ゲームオーバーにする
		m_isGameOver = true;
	}

	//ステージでクリアフラグがたったら
	if (m_galaxy.back()->GetClear())
	{
		//クリアにする
		m_isClear = true;
	}


	//これは後々ステージを追加したときよう
	/*if (m_galaxy.size() == 0)
	{
		m_isClear = true;
		m_galaxy.pop_back();
	}*/
	
}

void GameManager::Draw()
{
	//ステージやオブジェクトの描画
	m_galaxy.back()->Draw();

	//UIの描画
	float hp = m_player->GetHp();
	int coinNum = m_player->GetStarNum();
	bool aim = m_player->GetIsAiming();
	bool becameDeath = m_player->GetState() == MyEngine::Collidable::State::Death;

	DrawEffekseer3D();
	UI::GetInstance().Draw(hp, coinNum, aim,becameDeath);
}