#include "scene.h"
#include "hover.h"
#include "blender.h"
#include "SkinnedCube.h"

void SceneTitle::Initialize()
{
    ID3D11Device* device = pFramework->getDevice();

    // 光源(平行光)
    lightDirection = DirectX::XMFLOAT4(0, -1, 0, 0);
    camera1 = std::make_unique<MainCamera>();

    //箱
    cube_texture[0] = L"./Data/Floor/FloorS.png";
    cube_texture[1] = L"./Data/Floor/Floor0.png";
    cube_texture[2] = L"./Data/Floor/Floor1.png";
    cube_texture[3] = L"./Data/Floor/Floor2.png";
    cube_texture[4] = L"./Data/Floor/Floor3.png";
    cube_texture[5] = L"./Data/Floor/FloorG.png";
    cube_texture[6] = L"./Data/Floor/FloorN.png";

    // ブロック設定
    block = std::make_unique<GroundBlockManager>();
    std::shared_ptr<SkinnedCube> cube = std::make_shared<SkinnedCube>(device, cube_texture, 7);

    block = std::make_unique<GroundBlockManager>();
    block->SetStageNum(0 );
    block->Initialize();
    block->SetPrimitive(cube);

    // プレイヤー
    player = std::make_unique<Player>();
    player->Initialize("./Data/cube/cube_setM.fbx");
    player->SetPos(FLOAT3(2.0f, 0.0f, 0.0f));

    // ビュー設定
    camera1->SetEye(DirectX::XMFLOAT3(block->GetMapX()/2, 20.0f, (-block->GetMapY()/2)-10));
    camera1->SetFocus(DirectX::XMFLOAT3(block->GetMapX()/2, 0, -block->GetMapY()/2));
    camera1->SetUp(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));

    //　プロジェクションの設定
    camera1->SetPerspective(DirectX::XMConvertToRadians(30), pFramework->GetScreenWidth() / pFramework->GetScreenHeight(), 0.1f, 1000.0f);

}

void SceneTitle::Update(float elapsedTime)
{
    player->Move();
    block->Update();

    for (int i = 0; i < block->GetMea(); i++)
    {
        if (hover(player->GetPos(), block->GetBlockPos(i)))
        {
            block->SetBlockHover(i, true);
        }
        else block->SetBlockHover(i, false);
    }

    camera1->Updata(elapsedTime);
 
    if (GetAsyncKeyState('V') & 1)
    {
        SceneManager::Instance().SetStageNum(2);
        //SceneManager::Instance().ChangeScene(SceneGame::getInstance());
        SceneManager::Instance().ChangeScenePerformance(SceneGame::getInstance());
        return;
    }
}
void SceneTitle::Render(float elapsedTime)
{
    ID3D11DeviceContext* context = pFramework->getDeviceContext();

    blender::Set(blender::BS_ALPHA);

    DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&camera1->GetView());
    DirectX::XMMATRIX projection = DirectX::XMLoadFloat4x4(&camera1->GetProjection());

    //player->Render(view, projection, lightDirection, false);
    block->Render(context, view, projection);
    player->Render(view, projection, lightDirection, false, elapsedTime);
    //block->Render(context, view, projection, lightDirection, false);

    pFramework->sprites[5]->render(pFramework->getDeviceContext(), 20, 170, 360, 72, 0, 0, 600, 120, 0, XMFLOAT4(1, 1, 1, 1));

    //pFramework->sprites[6]->render(pFramework->getDeviceContext(), 280+190*0, 400, 190, 274, 0, 0, 190, 274, 0, XMFLOAT4(1, 1, 1, 1));//S
    //pFramework->sprites[7]->render(pFramework->getDeviceContext(), 280+190*1, 400, 190, 274, 0, 0, 190, 274, 0, XMFLOAT4(1, 1, 1, 1));//U
    //pFramework->sprites[8]->render(pFramework->getDeviceContext(), 280+190*2, 400, 190, 274, 0, 0, 190, 274, 0, XMFLOAT4(1, 1, 1, 1));//C
    //pFramework->sprites[8]->render(pFramework->getDeviceContext(), 280+190*3, 400, 190, 274, 0, 0, 190, 274, 0, XMFLOAT4(1, 1, 1, 1));//C
    //pFramework->sprites[9]->render(pFramework->getDeviceContext(), 280+190*4, 400, 190, 274, 0, 0, 190, 274, 0, XMFLOAT4(1, 1, 1, 1));//E
    //pFramework->sprites[6]->render(pFramework->getDeviceContext(), 280+190*5, 400, 190, 274, 0, 0, 190, 274, 0, XMFLOAT4(1, 1, 1, 1));//S
    //pFramework->sprites[6]->render(pFramework->getDeviceContext(), 280+190*6, 400, 190, 274, 0, 0, 190, 274, 0, XMFLOAT4(1, 1, 1, 1));//S
}

void SceneTitle::Finalize()
{

}