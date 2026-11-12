#include "Scene.h"
std::unique_ptr<IScene>SceneManager::s_currentScene = nullptr;

void SceneManager::ChangeScene(IScene* nextScene)
{
	s_currentScene.reset(nextScene);

	//V‚µ‚¢ƒV[ƒ“‚Ì‰Šú‰»
	if (s_currentScene)
	{
		s_currentScene->Init();
	}
}

void SceneManager::Update()
{
	if (s_currentScene)
	{
		s_currentScene->Update();
	}
}

void SceneManager::Draw()
{
	if (s_currentScene)
	{
		s_currentScene->Draw();
	}
}