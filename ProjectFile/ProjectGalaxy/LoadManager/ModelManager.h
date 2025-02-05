#pragma once
#include<map>
#include<string>
#include<vector>
#include"DxLib.h"

struct ModelInfo {
	int handle;//ハンドル
	bool used;//一度以上使用済みか？
};
class ModelManager
{
private:
	std::map<std::string, ModelInfo> m_pathAndModelInfoes;

public:
	ModelManager();
	~ModelManager();

	static ModelManager& GetInstance();

	int GetModelData(const char* filepath,bool isShadow=false);
	void DeleteModelData(const char* filepath);
	void Clear();
	void ClearShadowModel();

	std::vector<int>m_duplicateModelHandles;
	std::vector<int>m_shadowModelHandle;
};

