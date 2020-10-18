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

void SceneGame::Initialize()
{
    ID3D11Device* device = pFramework->getDevice();

    // 光源(平行光)
    lightDirection = DirectX::XMFLOAT4(0, -1, 0, 0);

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
    sky.staticLoad(L"./Data/Sky/sky.obj", "not_light");
    sky.scale = VECTOR3(30.0f, 30.0f, 30.0f);

    //箱
    cube_texture[0] = L"./Data/Floor/FloorS.png";
    cube_texture[1] = L"./Data/Floor/Floor0.png";
    cube_texture[2] = L"./Data/Floor/Floor1.png";
    cube_texture[3] = L"./Data/Floor/Floor2.png";
    cube_texture[4] = L"./Data/Floor/Floor3.png";
    cube_texture[5] = L"./Data/Floor/FloorG.png";

    std::shared_ptr<SkinndeCube> cube = std::make_shared<SkinndeCube>(device, cube_texture, 6);
    blocks = std::make_unique<GroundBlockManager>();
    blocks->SetStageNum(SceneManager::Instance().GetStageNum());
    blocks->Initialize();
    blocks->SetPrimitive(cube);

    player = std::make_unique<Player>();
    player->Initialize(new GeometricCube(pFramework->getDevice()));
    SetPosflg = true;

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


    DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&camera->GetView());
    DirectX::XMMATRIX projection = DirectX::XMLoadFloat4x4(&camera->GetProjection());

    blocks->Render(context, view, projection);

    if (SetPosflg)
    {
        for (int i = 0; i < blocks->GetMea(); i++)
        {
            if (blocks->GetBlockObj(i)->GetType() == Block::S_block)
            {
                player->SetPos(blocks->GetBlockPos(i).x, 0.0f, blocks->GetBlockPos(i).z);
                SetPosflg = false;
                break;
            }
            else player->SetPos(0.0f, 0.0f, 0.0f);
        }
    }

    player->Render(view, projection, lightDirection, wireframe);
    sky.Render(view, projection, lightDirection, wireframe);

    pPause->Draw();
}

