#include "scene.h"
#include "framework.h"

#include "blender.h"
#include "myMesh.h"

#include "camera.h"
#include "player.h"
#include "pause.h"

#include "hover.h"

#include "SkinndeCube.h"

#include <memory>
#include <DirectXMath.h>


MyMesh sky;
Camera camera;
Player  player;
GroundBlockManager block;

SkinndeCube* scube;
FileName cube_texture[5];

void SceneGame::Initialize()
{
    ID3D11Device* device = pFramework->getDevice();

    // 光源(平行光)
    lightDirection = DirectX::XMFLOAT4(0, -1, 0, 0);
    lightBG = DirectX::XMFLOAT4(0, 1, -1, 0);

    // カメラの設定
    camera = std::make_unique<MainCamera>();
    // ビュー設定
    camera->SetEye(DirectX::XMFLOAT3(0.0f, 10.0f, -30.0f));
    camera->SetFocus(DirectX::XMFLOAT3(25,1,25));
    camera->SetUp(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
    camera->SetMode(camera->MODE_FIX);

    //　プロジェクションの設定
    camera->SetPerspective(DirectX::XMConvertToRadians(45), pFramework->GetScreenWidth() / pFramework->GetScreenHeight(), 0.1f, 1000.0f);

    sky.Initialize();
    sky.skinnedLoad("./Data/cube/cube_0.fbx",true);
    sky.scale = VECTOR3(50.0f, 50.0f, 50.0f);

    //箱
    cube_texture[0] = L"./Data/Floor/FloorS.png";
    cube_texture[1] = L"./Data/Floor/Floor0.png";
    cube_texture[2] = L"./Data/Floor/Floor1.png";
    cube_texture[3] = L"./Data/Floor/Floor2.png";
    cube_texture[4] = L"./Data/Floor/Floor3.png";
    scube = new SkinndeCube(device, cube_texture, 5);

   // player.Initialize(new GeometricCube(pFramework->getDevice()));
   // player.Initialize(new GeometricCube(device));
    player = std::make_unique<Player>();
    player->Initialize("./Data/cube/cube_setM.fbx");
    std::shared_ptr<GeometricPrimitive> cube = std::make_shared<GeometricCube>(device);

    blocks = std::make_unique<GroundBlockManager>();
    blocks->SetStageNum(2);
    blocks->Initialize();
    blocks->SetPrimitive(cube);


    pPause->Initialize();
} 

void SceneGame::Update(float elapsedTime)
{
    if (pPause->Update())return;
    player->Move();
    camera->Updata(elapsedTime);
    blocks->Update();

    for (int i = 0; i < blocks->GetMea(); i++)
    {
        if (hover(player->GetPos(), blocks->GetBlockPos(i)))
        {
            blocks->SetBlockHover(i, true);
        }
        else blocks->SetBlockHover(i, false);
    }

    if (GetAsyncKeyState('V') & 1)
    {
        SceneManager::Instance().ChangeScene(SceneTitle::getInstance());
        blocks->Relese();
        return;
    }
}

void SceneGame::Render(float elapsedTime)
{
    blender::Set(blender::BS_ALPHA);

    ID3D11DeviceContext* context = pFramework->getDeviceContext();

    static bool wireframe = false;
    //if (GetAsyncKeyState(' ') & 1)
    //{
    //    wireframe = !wireframe;
    //}
    static int anime = 0;
    if (GetAsyncKeyState('X') & 1)
    {
        anime += 1;
        if (anime > 2) anime = 0;
    }
    // ビュー変換行列
    //view = camera.GetViewMatrix();

    DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&camera->GetView());
    DirectX::XMMATRIX projection = DirectX::XMLoadFloat4x4(&camera->GetProjection());

    player->Render(view, projection, lightDirection, wireframe, elapsedTime);
    sky.Render(view, projection, lightBG, wireframe, elapsedTime);

    framework::getInstance()->debug->setString("sky.Pos.x:%f", sky.pos.x);
    framework::getInstance()->debug->setString("sky.Pos.y:%f", sky.pos.y);
    framework::getInstance()->debug->setString("sky.Pos.z:%f", sky.pos.z);

    //block.Render(context, view, projection, lightDirection, wireframe);

    //箱
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX Scale, Rotation, Rx, Ry, Rz, Translation;

    //	初期化
    world = DirectX::XMMatrixIdentity();
    //	拡大・縮小
    Scale = DirectX::XMMatrixScaling(1, 0.5, 1);

    //	回転
    Rx = DirectX::XMMatrixRotationX(0);				//	X軸を基準とした回転行列
    Ry = DirectX::XMMatrixRotationY(0);				//	Y軸を基準とした回転行列
    Rz = DirectX::XMMatrixRotationZ(0);				//	Z軸を基準とした回転行列
    Rotation = Rz * Ry * Rx;

    //	平行移動
    Translation = DirectX::XMMatrixTranslation(-10,0,0);

    //	ワールド変換行列
    world = Scale * Rotation * Translation;

    DirectX::XMFLOAT4X4 wvp;
    DirectX::XMFLOAT4X4 W;
    XMStoreFloat4x4(&wvp, (world * view * projection));
    XMStoreFloat4x4(&W, world);

    static int FloorNUM = 1;
    if (GetAsyncKeyState('N') & 1)
    {
        FloorNUM++;
    }
    if (FloorNUM > 4) { FloorNUM = 1; }

    scube->Render(context, FloorNUM, wvp, W);
    blocks->Render(context, view, projection, lightDirection, wireframe);
    pPause->Draw();
}

