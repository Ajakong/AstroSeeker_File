#include "EffectManager.h"
#include<string>
#include<vector>

using namespace std;

EffectManager::EffectManager()
{

}

EffectManager::~EffectManager()
{
}

EffectManager& EffectManager::GetInstance()
{
	static EffectManager info;
	return info;
}

void EffectManager::Update()
{
	///すべてのエフェクトのデータ群
	for (auto& effData : m_playingEffectInfoes)
	{
		///同じエフェクトの種類のデータ群
		for (auto& eff : effData.second)
		{
			//このエフェクトがループ再生かどうか
			if (eff.second.isLoop)
			{
				//ループ再生の場合はマスターがいなくなったら消す
				if (eff.second.master.expired())
				{
					StopEffect(effData.first.c_str(), eff.first);
				}
			}
			else
			{
				//生存時間を超過したら消す
				eff.second.playingFrame++;
				if (eff.second.playingFrame > eff.second.lifeTime)
				{
					StopEffect(effData.first.c_str(), eff.first);
				}
			}
		}
	}
}

void EffectManager::PlayEffect(const char* effectname,bool playLoop, float lifeTime, std::weak_ptr<MyEngine::Collidable> master)
{
	
	int size = m_pathAndEffectInfoes[effectname].size();
	EffectInfo m = EffectInfo();
	string fileName = "Data/Effect/";
	//SetUseASyncLoadFlag(false);
	m.emitterhandle = LoadEffekseerEffect((fileName + (string)effectname).c_str());
	//SetUseASyncLoadFlag(true);
	m.used = false;
		
	m_pathAndEffectInfoes[effectname][size+1] = m;
	m_pathAndEffectInfoes[effectname][size+1].isLoop=playLoop;
	m_pathAndEffectInfoes[effectname][size + 1].lifeTime = lifeTime;

	m_pathAndEffectInfoes[effectname][size + 1].playhandle= PlayEffekseer3DEffect(m_pathAndEffectInfoes[effectname][size+1].emitterhandle);
	m_pathAndEffectInfoes[effectname][size + 1].master = master;
	m_playingEffectInfoes[effectname] = m_pathAndEffectInfoes[effectname];
		
	
}

void EffectManager::StopEffect(const char* effectname, int index)
{

	StopEffekseer3DEffect(m_pathAndEffectInfoes[effectname][index].playhandle);
	DeleteEffekseerEffect(m_pathAndEffectInfoes[effectname][index].playhandle);

	auto& it=m_pathAndEffectInfoes[effectname];
	auto effect = it.find(index);
	if (effect != it.end())
	{
		it.erase(effect);
	}
	if (it.empty())
	{
		m_pathAndEffectInfoes.erase(effectname);
	}
}

void EffectManager::Clear()
{
	for (auto& info : m_pathAndEffectInfoes)
	{
		for (auto& eff : info.second)
		{
			StopEffekseer3DEffect(eff.second.playhandle);
			DeleteEffekseerEffect(eff.second.playhandle);
		}
	}
	m_pathAndEffectInfoes.clear();
}
