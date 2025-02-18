#pragma once

namespace Pad
{
	// 接続しているパッドの数取得
	int getPadNum();

	// パッドの入力状態取得
	void update();

	// 押し下げ判定
	bool isPress(int button, int padNo = 0);
	// トリガー判定
	bool isTrigger(int button, int padNo = 0);
	// アナログスティック入力状態
	int getAnalogX(int padNo = 0);
	int getAnalogY(int padNo = 0);

}
