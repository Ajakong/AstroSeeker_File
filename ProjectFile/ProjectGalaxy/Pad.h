#pragma once
#include<string>
namespace Pad
{
	void Init();
	// パッドの更新処理 1フレームに1回行う
	void Update();

	/// <summary>
	/// 状態の設定: UIはテキストが終了したらPlayerInputに戻してください
	/// </summary>
	/// <param name="statement">PlayerInput/TextInput</param>
	void SetState(std::string statement);

	std::string GetState();
	/// <summary>
	/// ステートが合致しているかを取得
	/// </summary>
	/// <param name="statement">PlayerState/InputState</param>
	/// <returns>結果</returns>
	bool IsState(std::string statement);
	// 押されているかを取得
	bool IsPress(int key);
	// 押した瞬間を取得
	bool IsTrigger(int key);
	// 離した瞬間を取得
	bool IsRelase(int key);
	//スティック入力

	// リピート処理 (これは今後消すor変更する予定あり)
	bool IsRepeat(int key, int& frame, int RepeatFrame);

}