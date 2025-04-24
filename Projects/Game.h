#pragma once

// ゲーム全体で使用する定数を定義する
class Game
{
public:
	
	static constexpr int kScreenWidth = 1920;	// 画面の横幅
	static constexpr int kScreenHeight = 1080;	// 画面の縦幅

	static constexpr int kColorDepth = 32;		// 16 or 32

	static constexpr int kFPS = 60;			// フレームレート

	static constexpr int kMaxFPS = 120;		// 最大フレームレート

	static constexpr float PI_F=3.14159265358979f;	// 最大フレームレート

	// 度数法を弧度法に変更
	static constexpr float DEG_2_RAD = PI_F / 180.0f;
	// 弧度法を度数法に変更
	static constexpr float RAD_2_DEG = 180.0f / PI_F;

};
