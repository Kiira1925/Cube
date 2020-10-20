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

    /*title = std::make_unique<MyMesh>();
    title->Initialize();
    std::unique_ptr<GeometricPrimitive> board = std::make_unique<GeometricBoard>(device, L"./Data/title/title.png");*/
    title = std::make_unique<MyMesh>();
    title->Initialize();
    title->SetPrimitive(new GeometricBoard(device, L"./Data/flipImage/title.png"));
    title->pos.x = 3.0f;
    title->pos.z = 3.0f;
    title->angle.x = XMConvertToRadians(60);
    title->scale.x = 10.0f;
    title->scale.y = 10.0f;
    title->scale.z = 10.0f;

    selectGuide = std::make_unique<MyMesh>();
    selectGuide->Initialize();
    selectGuide->SetPrimitive(new GeometricRect(device, L"./Data/flipImage/stageselect.png"));
    selectGuide->pos.x = 3.0f;
    selectGuide->pos.z = 0.5f;
    selectGuide->scale.x = 5.0f;
    selectGuide->scale.y = 1.0f;
    selectGuide->scale.z = 2.0f;

    // 背景
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

    // ブロック設定
    block = std::make_unique<GroundBlockManager>();
    std::shared_ptr<SkinnedCube> cube = std::make_shared<SkinnedCube>(device, cube_texture, 8);

    block = std::make_shared<GroundBlockManager>();
    block->SetStageNum(0 );
    block->Initialize();
    block->SetPrimitive(cube);

    // プレイヤー
    player = std::make_unique<Player>();
    player->Initialize("./Data/cube/cube_setM.fbx");
    player->SetPos(FLOAT3(3.0f, 0.0f, -1.0f));

    // ビュー設定
    camera1->SetEye(DirectX::XMFLOAT3(block->GetMapX()/2, 20.0f, (-block->GetMapY()/2)-8));
    camera1->SetFocus(DirectX::XMFLOAT3(block->GetMapX()/2, 0, -block->GetMapY()/2 + 2));
    camera1->SetUp(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));

    //　プロジェクションの設定
    camera1->SetPerspective(DirectX::XMConvertToRadians(30), pFramework->GetScreenWidth() / pFramework->GetScreenHeight(), 0.1f, 1000.0f);

}

void SceneTitle::Update(float elapsedTime)
{

    if (Front(player->GetPos(), block) &&
        Back(player->GetPos(), block) &&
        Right(player->GetPos(), block) &&
        Left(player->GetPos(), block) && player->pos.y <= 0)
    {
        SceneManager::Instance().ChangeScenePerformance(SceneTitle::getInstance()); return;
    }

    // 場外に行かないための判定
    player->SetFront(Front(player->GetPos(), block));
    player->SetBack(Back(player->GetPos(), block));
    player->SetRight(Right(player->GetPos(), block));
    player->SetLeft(Left(player->GetPos(), block));

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
    sky.Render(view, projection, lightDirection, false, elapsedTime);
    block->Render(context, view, projection);
    player->Render(view, projection, lightDirection, false, elapsedTime);
    title->Render(view, projection, lightDirection, false, elapsedTime);
    selectGuide->Render(view, projection, lightDirection, false, elapsedTime);
    //block->Render(context, view, projection, lightDirection, false);

    pFramework->sprites[5]->render(pFramework->getDeviceContext(), 20, 170, 360, 72, 0, 0, 600, 120, 0, XMFLOAT4(1, 1, 1, 1));

    //pFramework->sprites[12] ->render(pFramework->getDeviceContext(), 490+190*0, 510-274, 190, 274, 0, 0, 190, 274, 0, XMFLOAT4(1, 1, 1, 1));//A
    //pFramework->sprites[11] ->render(pFramework->getDeviceContext(), 490+190*1, 510-274, 190, 274, 0, 0, 190, 274, 0, XMFLOAT4(1, 1, 1, 1));//L
    //pFramework->sprites[11] ->render(pFramework->getDeviceContext(), 490+190*2, 510-274, 190, 274, 0, 0, 190, 274, 0, XMFLOAT4(1, 1, 1, 1));//L

    //pFramework->sprites[8]  ->render(pFramework->getDeviceContext(), 490+190*0, 510, 190, 274, 0, 0, 190, 274, 0, XMFLOAT4(1, 1, 1, 1));//C
    //pFramework->sprites[11] ->render(pFramework->getDeviceContext(), 490+190*1, 510, 190, 274, 0, 0, 190, 274, 0, XMFLOAT4(1, 1, 1, 1));//L
    //pFramework->sprites[9]  ->render(pFramework->getDeviceContext(), 490+190*2, 510, 190, 274, 0, 0, 190, 274, 0, XMFLOAT4(1, 1, 1, 1));//E
    //pFramework->sprites[12] ->render(pFramework->getDeviceContext(), 490+190*3, 510, 190, 274, 0, 0, 190, 274, 0, XMFLOAT4(1, 1, 1, 1));//A
    //pFramework->sprites[10] ->render(pFramework->getDeviceContext(), 490+190*4, 510, 190, 274, 0, 0, 190, 274, 0, XMFLOAT4(1, 1, 1, 1));//R
}

void SceneTitle::Finalize()
{

}