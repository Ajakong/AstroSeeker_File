﻿#pragma once
#include"Vec3.h"

#include<memory>
#include<vector>

class Player;

namespace World
{
	class Galaxy//GameManagerが管理,惑星やオブジェクトの情報を持っている
	{
	public:
		Galaxy(std::shared_ptr<Player> player);
		virtual ~Galaxy();

		virtual void Init() = 0;
		virtual void Update() = 0;
		virtual void Draw() = 0;

		/// <summary>
		/// ゲームオーバーフラグを取得する
		/// </summary>
		/// <returns>ゲームオーバーフラグ</returns>
		bool GetGameOver() const { return m_isGameOver; }
		/// <summary>
		/// クリアフラグを取得する
		/// </summary>
		/// <returns>クリアフラグ</returns>
		bool GetClear() const { return m_isClear; }

	protected:
		using managerState_t = void(Galaxy::*)();
		managerState_t m_managerUpdate;

		using managerState_t = void(Galaxy::*)();
		managerState_t m_managerDraw;

		std::shared_ptr<Player> player;

		//モデルのみを描画するスクリーン(回転などの確認用)
		int m_modelScreenHandle;

		//このフラグを見てシーンを移行する
		bool m_isGameOver = false;
		bool m_isClear = false;
	};
}


