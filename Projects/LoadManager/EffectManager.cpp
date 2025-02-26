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
				//ループするとき
				auto ref=GetEffekseer3DManager();
				
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

int EffectManager::PlayEffect(const char* effectname,bool playLoop, float lifeTime, std::weak_ptr<MyEngine::Collidable> master)
{
	
	int size = static_cast<int>(m_pathAndEffectInfoes[effectname].size());
	EffectInfo m = EffectInfo();
	string fileName = "Data/Effect/";
	//SetUseASyncLoadFlag(false);
	m.emitterhandle = LoadEffekseerEffect((fileName + (string)effectname).c_str());
	//SetUseASyncLoadFlag(true);
	m.used = false;

	int index = size + 1;
		
	m_pathAndEffectInfoes[effectname][index] = m;
	m_pathAndEffectInfoes[effectname][index].isLoop=playLoop;
	m_pathAndEffectInfoes[effectname][index].lifeTime = lifeTime;

	m_pathAndEffectInfoes[effectname][index].playhandle= PlayEffekseer3DEffect(m_pathAndEffectInfoes[effectname][index].emitterhandle);
	m_pathAndEffectInfoes[effectname][index].master = master;
	m_playingEffectInfoes[effectname] = m_pathAndEffectInfoes[effectname];
		
	return index;
}

void EffectManager::SetPositionEffect(const char* effectname,int index, Vec3 position, MATRIX rotateMat)
{
	
	auto effData = m_pathAndEffectInfoes[effectname][index];
	SetPosPlayingEffekseer3DEffect(effData.playhandle, position.x, position.y, position.z);
	
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
		m_playingEffectInfoes.erase(effectname);
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
	m_playingEffectInfoes.clear();
}
