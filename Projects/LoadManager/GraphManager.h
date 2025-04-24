#pragma once
#include<map>
#include<string>
#include"DxLib.h"
struct GraphInfo 
{
	int handle;//ハンドル
	bool used;//一度以上使用済みか？
};
class GraphManager
{
public:
	GraphManager();
	~GraphManager();

	static GraphManager& GetInstance();

	int GetGraphData(const char* filepath);
	void DeleteGraphData(const char* filepath);
	void Clear();

private:
	std::map<std::string, GraphInfo> m_pathAndGraphInfoes;

};

