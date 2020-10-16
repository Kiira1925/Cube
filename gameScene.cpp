#include "scene.h"
#include "framework.h"

#include "blender.h"
#include "myMesh.h"

#include "camera.h"
#include "player.h"
#include "pause.h"

#include "ground_blocks.h"
#include "map_data.h"

#include <memory>


MyMesh sky;
Camera camera;
Player  player;
GroundBlockManager block;


void SceneGame::Initialize()
{
    //pFramework->getDevice();
// 投影変換行列
    ID3D11Device* device = pFramework->getDevice();
    // 投影変換行列
    projection = camera.GetProjectionMatrix();

    // 光源(平行光)
    lightDirection = DirectX::XMFLOAT4(10, 10, 10, 1.0f);

    sky.Initialize();
    sky.staticLoad(L"./Data/Sky/sky.obj", "not_light");
    sky.scale = VECTOR3(30.0f, 30.0f, 30.0f);

    // player.Initialize(L"./Data/cube/cube_setM.obj", "not_light");
    player.Initialize(L"./Data/Bison/Bison.obj", "not_light");
   // player.Initialize(new GeometricCube(device));
    std::shared_ptr<GeometricPrimitive> cube = std::make_shared<GeometricCube>(device);
    block.SetStageNum(00);
    block.Initialize();
    block.SetPrimitive(cube);
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

    ID3D11DeviceContext* context = pFramework->getDeviceContext();

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
    player.Render(view, projection, lightDirection, wireframe);
    sky.Render(view, projection, lightDirection, wireframe);

    block.Render(context, view, projection, lightDirection, wireframe);
    pPause->Draw();
}

