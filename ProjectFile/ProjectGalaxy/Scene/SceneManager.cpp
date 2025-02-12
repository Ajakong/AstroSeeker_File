#include "SceneManager.h"
#include "Scene.h"

namespace NextScene
{

}

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::Update()
{
	// 末尾のみ実行
	m_scenes.back()->Update();
}

void SceneManager::Draw()
{
	// 先頭から順に描画(最後に積んだものが最後に描画される)
	for (auto& scene : m_scenes)
	{
		scene->Draw();
	}
}

void SceneManager::ChangeScene(std::shared_ptr<Scene> nextScene)
{
	if (m_scenes.empty()) {	// リストが空っぽだったら入れ替えるのではなく
		m_scenes.push_back(nextScene);	// 末尾に追加する
	}
	else {
		m_scenes.back() = nextScene;	// すでに一つ以上あったら末尾のものを入れ替える
	}
}

void SceneManager::InsertScene(std::shared_ptr<Scene> nextScene)
{

	m_scenes.insert(m_scenes.begin(), nextScene);
}



void SceneManager::PushScene(std::shared_ptr<Scene> scene)
{
	m_scenes.push_back(scene);
}

void SceneManager::PopScene()
{
	m_scenes.pop_back();
}

void SceneManager::ResetScene(std::shared_ptr<Scene> scene)
{
	m_scenes.clear();
	m_scenes.push_back(scene);
}

std::shared_ptr<Scene> SceneManager::LookBack()
{
	return m_scenes.back();
}
