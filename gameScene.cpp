#include "scene.h"
#include "framework.h"

#include "blender.h"
#include "myMesh.h"

#include "camera.h"
#include "player.h"
#include "pause.h"

#include "hover.h"

#include "SkinnedCube.h"

#include <memory>
#include <DirectXMath.h>


MyMesh sky;

void SceneGame::Initialize()
{
    ID3D11Device* device = pFramework->getDevice();

    // 光源(平行光)
    lightDirection = DirectX::XMFLOAT4(0, -1, 0, 0);
    lightBG = DirectX::XMFLOAT4(0, 1, -1, 0);

    // カメラの設定
    camera = std::make_unique<MainCamera>();
    // ビュー設定
    camera->SetEye(DirectX::XMFLOAT3(10.0f, 20.0f, -15.0f));
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
    cube_texture[5] = L"./Data/Floor/FloorG.png";
    cube_texture[6] = L"./Data/Floor/FloorN.png";

    //scube = new SkinndeCube(device, cube_texture, 5);

   // player.Initialize(new GeometricCube(pFramework->getDevice()));
   // player.Initialize(new GeometricCube(device));
    player = std::make_unique<Player>();
    player->Initialize("./Data/cube/cube_setM.fbx");

    std::shared_ptr<SkinnedCube> cube = std::make_shared<SkinnedCube>(device, cube_texture, 7);
    blocks = std::make_unique<GroundBlockManager>();
    blocks->SetStageNum(SceneManager::Instance().GetStageNum());
    blocks->Initialize();
    blocks->SetPrimitive(cube);

    camera->SetFocus(DirectX::XMFLOAT3(blocks->GetMapX() / 2, 1, -blocks->GetMapY() / 2.0f));
    camera->SetEye(DirectX::XMFLOAT3(camera->GetFocus().x + 5.0f, 15.0f, camera->GetFocus().z - 10.0f));

    //player = std::make_unique<Player>();
    //player->Initialize(new GeometricCube(pFramework->getDevice()));
    SetPosflg = true;
    clearTimer = 0;

    pPause->Initialize();
} 

void SceneGame::Update(float elapsedTime)
{
    if (clearTimer == 10) { pFramework->soundSE[3]->Play(false); }
    if (clearTimer > 120) 
    {
        Reload(SceneManager::Instance().GetStageNum() + 1); clearTimer = 0; return;
    }
    if (clearFlg()) { clearTimer++; return; }
    if (pPause->Update())return;
    player->Move();
    camera->Updata(elapsedTime);
    blocks->Update();

    for (int i = 0; i < blocks->GetMea(); i++)
    {
        if (hover(player->pos, blocks->GetBlockPos(i)))
        {
            blocks->SetBlockHover(i, true);
        }
        else blocks->SetBlockHover(i, false);
    }

    if (GetAsyncKeyState('V') & 1)
    {
        SceneManager::Instance().ChangeScenePerformance(SceneTitle::getInstance());
        //blocks->Relese();
        return;
    }

    //if (clearFlg()) { Reload(SceneManager::Instance().GetStageNum() + 1); }
}

void SceneGame::Render(float elapsedTime)
{
    blender::Set(blender::BS_ALPHA);

    ID3D11DeviceContext* context = pFramework->getDeviceContext();

    static bool wireframe = false;


    DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&camera->GetView());
    DirectX::XMMATRIX projection = DirectX::XMLoadFloat4x4(&camera->GetProjection());

    blocks->Render(context, view, projection);
    player->Render(view, projection, lightDirection, wireframe, elapsedTime);
    sky.Render(view, projection, lightBG, wireframe, elapsedTime);

    framework::getInstance()->debug->setString("sky.Pos.x:%f", sky.pos.x);
    framework::getInstance()->debug->setString("sky.Pos.y:%f", sky.pos.y);
    framework::getInstance()->debug->setString("sky.Pos.z:%f", sky.pos.z);

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
        SetPosflg = false;
    }

    //player->Render(view, projection, lightDirection, wireframe);
    //sky.Render(view, projection, lightDirection, wireframe);

    // UI表示
    pFramework->sprites[3]->render(pFramework->getDeviceContext(), 10, 10, 564, 150, 0, 0, 789, 210, 0, XMFLOAT4(1, 1, 1, 1));
    pFramework->sprites[4]->render(pFramework->getDeviceContext(), 450, 60, 128, 50, 0 + (SceneManager::Instance().GetStageNum()-10)*128, 0, 128, 50, 0, XMFLOAT4(1, 1, 1, 1));

    pFramework->sprites[5]->render(pFramework->getDeviceContext(), 20, 170, 360, 72, 0, 0, 600, 120, 0, XMFLOAT4(1, 1, 1, 1));

    clearDraw();

    pPause->Draw();
}

void SceneGame::Finalize()
{
    blocks->Relese();
}

void SceneGame::Reload(int stage_num)
{
    ID3D11Device* device = pFramework->getDevice();
    std::shared_ptr<SkinnedCube> cube = std::make_shared<SkinnedCube>(device, cube_texture, 7);

    blocks->Relese();

    blocks = std::make_unique<GroundBlockManager>();
    blocks->SetStageNum(stage_num);
    SceneManager::Instance().SetStageNum(stage_num);
    blocks->Initialize();
    blocks->SetPrimitive(cube);
    player->InitStatus();

    //camera->SetEye(DirectX::XMFLOAT3(blocks->GetMapX() / 2 + 5.0f, 15.0f, (blocks->GetMapY() / 2) - 10.0f));
    camera->SetFocus(DirectX::XMFLOAT3(blocks->GetMapX() / 2, 1, -blocks->GetMapY() / 2.0f));
    camera->SetEye(DirectX::XMFLOAT3(camera->GetFocus().x + 5.0f, 15.0f, camera->GetFocus().z - 10.0f));

    for (int i = 0; i < blocks->GetMea(); i++)
    {
        if (blocks->GetBlockObj(i)->GetType() == Block::S_block)
        {
            player->SetPos(blocks->GetBlockPos(i).x, 0.0f, blocks->GetBlockPos(i).z);
            SetPosflg = true;
            break;
        }
        else player->SetPos(0.0f, 0.0f, 0.0f);
    }

    pPause->Initialize();
}

bool SceneGame::clearFlg()
{
    if (!blocks->checkBlockExist())
    {
        if (player->speed.x == 0 && player->speed.z == 0)
        {
            return true;
        }
    }
    return false;
}

void SceneGame::clearDraw()
{
    if (clearFlg())
    {
        float alpha = clearTimer / 60.0f;
        blender::Set(blender::BS_ADD);
        pFramework->sprites[6]->render(pFramework->getDeviceContext(), 280 + 190 * 0, 400, 190, 274, 0, 0, 190, 274, 0, XMFLOAT4(1, 1, 1, alpha-0.05));//S
        pFramework->sprites[7]->render(pFramework->getDeviceContext(), 280 + 190 * 1, 400, 190, 274, 0, 0, 190, 274, 0, XMFLOAT4(1, 1, 1, alpha-0.10));//U
        pFramework->sprites[8]->render(pFramework->getDeviceContext(), 280 + 190 * 2, 400, 190, 274, 0, 0, 190, 274, 0, XMFLOAT4(1, 1, 1, alpha-0.15));//C
        pFramework->sprites[8]->render(pFramework->getDeviceContext(), 280 + 190 * 3, 400, 190, 274, 0, 0, 190, 274, 0, XMFLOAT4(1, 1, 1, alpha-0.20));//C
        pFramework->sprites[9]->render(pFramework->getDeviceContext(), 280 + 190 * 4, 400, 190, 274, 0, 0, 190, 274, 0, XMFLOAT4(1, 1, 1, alpha-0.25));//E
        pFramework->sprites[6]->render(pFramework->getDeviceContext(), 280 + 190 * 5, 400, 190, 274, 0, 0, 190, 274, 0, XMFLOAT4(1, 1, 1, alpha-0.30));//S
        pFramework->sprites[6]->render(pFramework->getDeviceContext(), 280 + 190 * 6, 400, 190, 274, 0, 0, 190, 274, 0, XMFLOAT4(1, 1, 1, alpha-0.35));//S
        blender::Set(blender::BS_NONE);
    }
}