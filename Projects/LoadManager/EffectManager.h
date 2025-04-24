#pragma once
#include<map>
#include<string>
#include<list>
#include<EffekseerForDXLib.h>
#include"Collidable.h"
#include"Quaternion.h"
struct EffectInfo 
{
	int emitterhandle;//エミッターハンドル
	int playhandle;//プレイするハンドル
	int playingFrame;//再生中フレーム
	float lifeTime;//エフェクトの生存時間
	bool isLoop;//ループ再生するか
	bool used;//一度以上使用済みか？
	std::weak_ptr<MyEngine::Collidable> master;//私がこのエフェクトを生成しました。
};
class EffectManager
{
public:
	EffectManager();
	~EffectManager();

	static EffectManager& GetInstance();
	

	void Update();

	int PlayEffect(const char* effectname,bool playLoop,float lifeTime,std::weak_ptr<MyEngine::Collidable> master=std::weak_ptr<MyEngine::Collidable>());
	void SetInformationEffect(const char* effectname,int index, const Vec3& pos, const Quaternion& rot, const Vec3& scale);
	void StopEffect(const char* effectname,int index);

	void Clear();

private:
	/// <summary>
	/// 再生中エフェクトのデータ
	/// </summary>
	std::map<std::string, std::map<int, EffectInfo>>m_playingEffectInfoes;
	/// <summary>
	/// エフェクトの情報群
	/// </summary>
	std::map<std::string, std::map<int, EffectInfo>> m_pathAndEffectInfoes;
};

