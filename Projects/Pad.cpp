#include"Dxlib.h"
#include"Pad.h"

namespace
{
	// 前のフレームのパッド押し下げ状態
	int lastPad = 0;
	// 個のフレームのパッド押し下げ状態
	int nowPad = 0;

	std::string state = "PlayerInput";
}

namespace Pad
{
	void Pad::Init()
	{
		// 前のフレームのパッド押し下げ状態
		lastPad = 0;
		// 個のフレームのパッド押し下げ状態
		nowPad = 0;

		state = "PlayerInput";
	}
	void Pad::Update()
	{
		// 前のフレームに取得したパッド情報を一つ古い情報にする
		lastPad = nowPad;

		// 現在のパッドの情報を取得する
		nowPad = GetJoypadInputState(DX_INPUT_KEY_PAD1);
	}

	//状態を設定する
	void SetState(std::string statement)
	{
		state = statement;
	}

	//現在の状態を取得
	std::string GetState()
	{
		return state;
	}

	//引数の状態と一致しているか
	bool IsState(std::string statement)
	{
		return state==statement;
	}

	
	bool IsPress(int key)
	{
		return (nowPad & key);
	}

	bool IsTrigger(int key)
	{
		bool isNow = (nowPad & key);	// このフレーム
		bool isLast = (lastPad & key);	// 前のフレーム

		return (isNow && !isLast);
	}

	bool IsRelase(int key)
	{
		bool isNow = (nowPad & key);	// このフレーム
		bool isLast = (lastPad & key);	// 前のフレーム

		return (!isNow && isLast);
	}

	bool Pad::IsRepeat(int key, int& frame, int RepeatFrame)
	{
		if (IsTrigger(key)) return true;

		if (IsPress(key))
		{
			frame++;

			if (RepeatFrame <= frame)
			{
				frame = 0;

				return true;
			}
		}
		else
		{
			frame = 0;
		}

		return false;
	}
}
