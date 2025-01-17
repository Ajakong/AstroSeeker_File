#include "UI.h"
#include"GraphManager.h"
#include"Game.h"
namespace
{
	const char* kGraphUIAssetName = "Designer_ui.png";
	const UI::UIinfo kIdemBoxUIInfo{ 0,0,255,255 };
	const UI::UIinfo kHPBarUIInfo { 125,730,820,140 };
	const UI::UIinfo kWindowScreenUIInfo{ 620,15,400,500 };

	constexpr float kHpDecreaseSpeed = 0.3f;
}
UI::UI()
{
	m_uiAssetHandle = GraphManager::GetInstance().GetGraphData(kGraphUIAssetName);

}

UI::~UI()
{
}

void UI::Init()
{
}

void UI::Update()
{
}

void UI::Draw(float m_hp)
{
	DrawBox(40, 40, 780, kHPBarUIInfo.height / 2+10, 0x0000044,true);
	DrawBox(40, 40, 40.f+15.f*static_cast<int>(m_hp), kHPBarUIInfo.height / 2 + 10, 0x00044ff, true);

	DrawRectRotaGraph(static_cast<int>(kHPBarUIInfo.width / 2), static_cast<int>(kHPBarUIInfo.height / 2), kHPBarUIInfo.x, kHPBarUIInfo.y, kHPBarUIInfo.width, kHPBarUIInfo.height, 1, 0, m_uiAssetHandle, true);
#ifdef DEBUG
	DrawRectRotaGraph(Game::kScreenWidth - kIdemBoxUIInfo.width / 2-170,kIdemBoxUIInfo.height / 2+50, kIdemBoxUIInfo.x, kIdemBoxUIInfo.y, kIdemBoxUIInfo.width, kIdemBoxUIInfo.height, 1, 0, m_uiAssetHandle, true);
#endif
	/*DrawRectRotaGraph( Game::kScreenWidth-kWindowScreenUIInfo.width / 2, kWindowScreenUIInfo.height / 2, kWindowScreenUIInfo.x, kWindowScreenUIInfo.y, kWindowScreenUIInfo.width, kWindowScreenUIInfo.height, 1, 0, m_uiAssetHandle, true);*/
}
