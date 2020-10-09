#include "scene.h"

//*********************************
//
//  ƒV[ƒ“ŠÇ—
//
//*********************************

void SceneManager::Update()
{
    currentScene->Update(pFramework->getElapsedTime());
}

void SceneManager::Render()
{
    currentScene->Render(pFramework->getElapsedTime());
}

void SceneManager::ChangeScene(Scene *newScene)
{
    currentScene = newScene;

    currentScene->Initialize();
}