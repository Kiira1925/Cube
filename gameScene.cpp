#include "scene.h"
#include "framework.h"

#include "blender.h"
#include "myMesh.h"

#include "camera.h"
#include "player.h"
#include "pause.h"

#include <memory>

MyMesh field;
MyMesh sky;
Camera camera;
Player  player;


void SceneGame::Initialize()
{
    //pFramework->getDevice();
// 投影変換行列
    projection = camera.GetProjectionMatrix();

    // 光源(平行光)
    lightDirection = DirectX::XMFLOAT4(1, 10, 2, 0);

    // 仮地面
    field.Initialize();
    field.SetPrimitive(new GeometricCube(pFramework->getDevice()));
    field.pos = VECTOR3(40.0f, 0.0f, 40.0f);
    field.color = VECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
    field.scale = VECTOR3(10.0f, 10.0f, 10.0f);

    sky.Initialize();
    sky.staticLoad(L"./Data/Sky/sky.obj", "not_light");
    sky.scale = VECTOR3(30.0f, 30.0f, 30.0f);

    player.Initialize(new GeometricCube(pFramework->getDevice()));
    pPause->Initialize();
}

void SceneGame::Update(float elapsedTime)
{
    if (pPause->Update())return;
    player.Move();
    camera.Updata();
    if (GetAsyncKeyState('V') & 1)
    {
        SceneManager::Instance().ChangeScene(SceneTitle::getInstance());
        return;
    }
}

void SceneGame::Render(float elapsedTime)
{
    blender::Set(blender::BS_ALPHA);

    static bool wireframe = false;
    if (GetAsyncKeyState(' ') & 1)
    {
        wireframe = !wireframe;
    }
    static int anime = 0;
    if (GetAsyncKeyState('X') & 1)
    {
        anime += 1;
        if (anime > 2) anime = 0;
    }
    // ビュー変換行列
    view = camera.GetViewMatrix();

    //field.Render(view, projection, lightDirection,wireframe);
    player.Render(view, projection, lightDirection, wireframe);
    sky.Render(view, projection, lightDirection, wireframe);
    pPause->Draw();
}

