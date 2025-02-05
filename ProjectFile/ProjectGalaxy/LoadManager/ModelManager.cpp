#include "ModelManager.h"
#include<string>
#include<vector>

using namespace std;
ModelManager::ModelManager()
{
}

ModelManager::~ModelManager()
{
	for (auto& item : m_pathAndModelInfoes)
	{
		MV1DeleteModel(item.second.handle);
	}
}

ModelManager& ModelManager::GetInstance()
{
	static ModelManager info;
	return info;
}

int ModelManager::GetModelData(const char* modelname, bool isShadow)
{
	if (m_pathAndModelInfoes.find(modelname) == m_pathAndModelInfoes.end())
	{
		ModelInfo m = ModelInfo();
		string fileName = "Data/Model/";
		string fileFormat = ".mv1";
		m.handle = DxLib::MV1LoadModel((fileName + (string)modelname+ fileFormat).c_str());
		m.used = false;
		m_pathAndModelInfoes[modelname] = m;
		return m.handle;
	}
	else {
		if (isShadow)
		{
			m_shadowModelHandle.push_back(MV1DuplicateModel(m_pathAndModelInfoes[modelname].handle));
			return m_shadowModelHandle.back();
		}
		else
		{
			m_pathAndModelInfoes[modelname].used = true;
			m_duplicateModelHandles.push_back(MV1DuplicateModel(m_pathAndModelInfoes[modelname].handle));
			return m_duplicateModelHandles.back();
		}
		
	}
}

void ModelManager::DeleteModelData(const char* modelname)
{
	DxLib::DeleteGraph(m_pathAndModelInfoes[modelname].handle);
	m_pathAndModelInfoes.erase(modelname);
}

void ModelManager::Clear()
{
	for (auto& info : m_pathAndModelInfoes)
	{
		MV1DeleteModel(info.second.handle);
	}
	for (auto& handle : m_duplicateModelHandles)
	{
		MV1DeleteModel(handle);
	}
	m_duplicateModelHandles.clear();
	m_pathAndModelInfoes.clear();
	ClearShadowModel();
}

void ModelManager::ClearShadowModel()
{
	for (auto& handle : m_shadowModelHandle)
	{
		MV1DeleteModel(handle);
	}
	m_shadowModelHandle.clear();
}
