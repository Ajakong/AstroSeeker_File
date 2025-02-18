#include "DxLib.h"
#include "Pad.h"

#include <array>
#include <deque>
#include <cassert>

namespace
{
	// ログ記録フレーム数
	constexpr int kLogNum = 16;
	// 
	constexpr int kUsePadData[] =
	{
		DX_INPUT_KEY_PAD1,
		DX_INPUT_PAD2,
		DX_INPUT_PAD3,
		DX_INPUT_PAD4,
		DX_INPUT_PAD5,
		DX_INPUT_PAD6,
		DX_INPUT_PAD7,
		DX_INPUT_PAD8,
	};
	// パッド最大数
	constexpr int kMaxPad = _countof(kUsePadData);

	// 毎フレーム収集するパッド入力情報
	struct PadLog
	{
		int key;	// ボタンのONOFF情報
		int stickX;	// アナログパッド入力情報
		int stickY;
	};

	std::array<std::deque<PadLog>, kMaxPad>	padLog;
}

namespace Pad
{
	int getPadNum()
	{
		return GetJoypadNum();
	}
	// パッドの入力状態取得
	void update()
	{
		for (int padNo = 0; padNo < kMaxPad; padNo++)
		{
			// 現在のパッドの状態を取得
			PadLog input;
			input.key = GetJoypadInputState(kUsePadData[padNo]);
			GetJoypadAnalogInput(&input.stickX, &input.stickY, kUsePadData[padNo]);

			padLog[padNo].push_front(input);
			if (padLog[padNo].size() > kLogNum)
			{
				padLog[padNo].pop_back();
			}
		}
	}

	// 押し下げ判定
	bool isPress(int button, int padNo)
	{
		assert(padNo < kMaxPad);
		return (padLog[padNo][0].key & button);
	}
	// トリガー判定
	bool isTrigger(int button, int padNo)
	{
		if (padLog[padNo].size() < 2)	return false;

		assert(padNo < kMaxPad);
		bool isNow = (padLog[padNo][0].key & button);	// 現在の状態
		bool isLast = (padLog[padNo][1].key & button);	// １フレーム前の状態
		return (isNow && !isLast);
	}
	int getAnalogX(int padNo)
	{
		assert(padNo < kMaxPad);
		return padLog[padNo][0].stickX;
	}
	int getAnalogY(int padNo)
	{
		assert(padNo < kMaxPad);
		return padLog[padNo][0].stickY;
	}
}