#include "scene.h"
#include "hover.h"
#include "blender.h"

void SceneSelect::Initialize()
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

    // ブロック設定
    block = std::make_unique<GroundBlockManager>();
    std::shared_ptr<SkinndeCube> cube = std::make_shared<SkinndeCube>(device, cube_texture, 6);

    block = std::make_unique<GroundBlockManager>();
    block->SetStageNum(1);
    block->Initialize();
    block->SetPrimitive(cube);

    // プレイヤー
    //player = std::make_unique<Player>();
    //player->Initialize(new GeometricCube(pFramework->getDevice()));

    player = std::make_unique<Player>();
    player->Initialize("./Data/cube/cube_setM.fbx");
    player->SetPos(FLOAT3(2.0f, 0.0f, 0.0f));

    // ビュー設定
    camera1->SetEye(DirectX::XMFLOAT3(0.0f, 20.0f, -10.0f));
    camera1->SetFocus(DirectX::XMFLOAT3(5, 0, 5));
    camera1->SetUp(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));

    //　プロジェクションの設定
    camera1->SetPerspective(DirectX::XMConvertToRadians(30), pFramework->GetScreenWidth() / pFramework->GetScreenHeight(), 0.1f, 1000.0f);

}

void SceneSelect::Update(float elapsedTime)
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
    camera1->SetmodeTarget(DirectX::XMFLOAT3(player->GetPos().x, player->GetPos().y, player->GetPos().z));

    if (GetAsyncKeyState('V') & 1)
    {
        SceneManager::Instance().ChangeScene(SceneGame::getInstance());
        return;
    }
}

void SceneSelect::Render(float elapsedTime)
{
    ID3D11DeviceContext* context = pFramework->getDeviceContext();

    blender::Set(blender::BS_ALPHA);

    DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&camera1->GetView());
    DirectX::XMMATRIX projection = DirectX::XMLoadFloat4x4(&camera1->GetProjection());

    player->Render(view, projection, lightDirection, false, elapsedTime);
    block->Render(context, view, projection);

}

void SceneSelect::Finalize()
{

}