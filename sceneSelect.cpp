#include "scene.h"
#include "hover.h"
#include "blender.h"

void SceneSelect::Initialize()
{
    ID3D11Device* device = pFramework->getDevice();

    // 光源(平行光)
    lightDirection = DirectX::XMFLOAT4(0, -1, 0, 0);
    camera1 = std::make_unique<MainCamera>();

    sky.Initialize();
    sky.skinnedLoad("./Data/cube/cube_0.fbx", true);
    sky.scale = VECTOR3(50.0f, 50.0f, 50.0f);

    //箱
    cube_texture[0] = L"./Data/Floor/FloorS.png";
    cube_texture[1] = L"./Data/Floor/Floor0.png";  
    cube_texture[2] = L"./Data/Floor/Floor1.png";
    cube_texture[3] = L"./Data/Floor/Floor2.png";
    cube_texture[4] = L"./Data/Floor/Floor3.png";
    cube_texture[5] = L"./Data/Floor/FloorG.png";
    cube_texture[6] = L"./Data/Floor/FloorN.png";
    cube_texture[7] = L"./Data/Floor/stage01.png";
    cube_texture[8] = L"./Data/Floor/stage02.png";
    cube_texture[9] = L"./Data/Floor/stage03.png";
    cube_texture[10] = L"./Data/Floor/stage04.png";
    cube_texture[11] = L"./Data/Floor/stage05.png";
    cube_texture[12] = L"./Data/Floor/stage06.png";
    cube_texture[13] = L"./Data/Floor/stage07.png";
    cube_texture[14] = L"./Data/Floor/stage08.png";
    cube_texture[15] = L"./Data/Floor/stage09.png";
    cube_texture[16] = L"./Data/Floor/stage10.png";

    // ブロック設定
    block = std::make_unique<GroundBlockManager>();
    std::shared_ptr<SkinnedCube> cube = std::make_shared<SkinnedCube>(device, cube_texture, 17);

    block = std::make_shared<GroundBlockManager>();
    block->SetStageNum(1);
    block->Initialize();
    block->SetPrimitive(cube);

    // プレイヤー
    //player = std::make_unique<Player>();
    //player->Initialize(new GeometricCube(pFramework->getDevice()));

    player = std::make_unique<Player>();
    player->Initialize("./Data/cube/cube_setM.fbx");
    if (GroundBlockManager::getInstance()->isSelect01) { player->SetPos(FLOAT3(2.0f, 0.0f, -2.0f)); }
    else { player->SetPos(FLOAT3(20.0f, 0.0f, -2.0f)); }

    // ビュー設定
    camera1->SetEye(DirectX::XMFLOAT3(0.0f, 10.0f, -1.0f));
    camera1->SetFocus(DirectX::XMFLOAT3(player->pos.x , 0, player->pos.z));
    camera1->SetUp(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
    camera1->SetMode(MainCamera::MODE_SIDE);

    //　プロジェクションの設定
    camera1->SetPerspective(DirectX::XMConvertToRadians(30), pFramework->GetScreenWidth() / pFramework->GetScreenHeight(), 0.1f, 1000.0f);

}

void SceneSelect::Update(float elapsedTime)
{

    if (Front(player->GetPos(), block) &&
        Back(player->GetPos(), block) &&
        Right(player->GetPos(), block) &&
        Left(player->GetPos(), block) && player->pos.y <= 0)
    {
        SceneManager::Instance().ChangeScenePerformance(SceneSelect::getInstance()); return;
    }

    // 場外に行かないための判定
    player->SetFront(Front(player->GetPos(), block));
    player->SetBack( Back(player->GetPos(),  block));
    player->SetRight(Right(player->GetPos(), block));
    player->SetLeft(Left(player->GetPos(),   block));

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
}

void SceneSelect::Render(float elapsedTime)
{
    ID3D11DeviceContext* context = pFramework->getDeviceContext();

    blender::Set(blender::BS_ALPHA);

    DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&camera1->GetView());
    DirectX::XMMATRIX projection = DirectX::XMLoadFloat4x4(&camera1->GetProjection());

    sky.Render(view, projection, lightDirection, false, elapsedTime);
    player->Render(view, projection, lightDirection, false, elapsedTime);
    block->Render(context, view, projection);

    pFramework->sprites[5]->render(pFramework->getDeviceContext(), 20, 170, 360, 72, 0, 0, 600, 120, 0, XMFLOAT4(1, 1, 1, 1));
}

void SceneSelect::Finalize()
{

}