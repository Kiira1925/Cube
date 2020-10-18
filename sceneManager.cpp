#include "scene.h"
#include "blender.h"

//*********************************
//
//  ƒV[ƒ“ŠÇ—
//
//*********************************

SceneManager::SceneManager()
{
    change_timer = 0;
    change_flg = false;
    currentProcess = CHANGE_PROCESS::Set;
}

void SceneManager::changeProcessing()
{
    if (!change_flg) return;

    static float alpha_wb;
    switch (currentProcess)
    {
    case Set:
        change_timer = 120;
        currentProcess = Out;
        alpha_wb = 0;
        break;
    case Out:
        alpha_wb = (100.0f - change_timer) / 100.0f;
        blender::Set(blender::BS_ALPHA);
        pFramework->sprites[0]->render(pFramework->getDeviceContext(), 0, 0, 1920, 1080, 0, 0, 1920, 1080, 0, DirectX::XMFLOAT4(1, 1, 1, alpha_wb));
        blender::Set(blender::BS_NONE);
        change_timer--;
        if (change_timer < 0) 
        {
            change_timer = 120;
            currentProcess = Fin;
            ChangeScene(nextScene);
        }
        break;
    case Fin:
        change_flg = false;
        currentProcess = Set;
    }
}

void SceneManager::changeProcessDraw()
{
    static float alpha_wb;
    switch (currentProcess)
    {
    case Set:
        alpha_wb = 0;
        break;
    case Out:
        alpha_wb = (100.0f - change_timer) / 100.0f;
        blender::Set(blender::BS_ALPHA);
        pFramework->sprites[0]->render(pFramework->getDeviceContext(), 0, 0, 1920, 1080, 0, 0, 1920, 1080, 0, DirectX::XMFLOAT4(1, 1, 1, alpha_wb));
        blender::Set(blender::BS_NONE);
        break;
    case Fin:
        change_flg = false;
        currentProcess = Set;
    }
}

void SceneManager::Update()
{
    if (change_flg) { changeProcessing(); return; }
    currentScene->Update(pFramework->getElapsedTime());
}

void SceneManager::Render()
{
    currentScene->Render(pFramework->getElapsedTime());
    if (change_flg) { changeProcessDraw(); }
}

void SceneManager::SetScene(Scene* newScene)
{
    currentScene = newScene;

    currentScene->Initialize();
}

void SceneManager::ChangeScene(Scene *newScene)
{
    currentScene->Finalize();
    currentScene = newScene;

    currentScene->Initialize();
}

void SceneManager::ChangeScenePerformance(Scene* newScene)
{
    nextScene = newScene;
    change_flg = true;
}