#include "scene.h"
#include "framework.h"

#include "blender.h"
#include "myMesh.h"

#include "camera.h"
#include "player.h"
#include "pause.h"

#include "ground_blocks.h"
#include "map_data.h"

#include "SkinndeCube.h"

#include <memory>
#include <DirectXMath.h>


MyMesh field;
MyMesh sky;
Camera camera;
Player  player;
GroundBlockManager block;

SkinndeCube* scube;
FileName cube_texture[5];

void SceneGame::Initialize()
{
    //pFramework->getDevice();
// ���e�ϊ��s��
    ID3D11Device* device = pFramework->getDevice();
    // ���e�ϊ��s��
    projection = camera.GetProjectionMatrix();

    // ����(���s��)
    lightDirection = DirectX::XMFLOAT4(10, 10, 10, 1.0f);

    // ���n��
    field.Initialize();
    field.SetPrimitive(new GeometricCube(device));
    field.pos = VECTOR3(40.0f, 0.0f, 40.0f);
    field.color = VECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
    field.scale = VECTOR3(10.0f, 10.0f, 10.0f);

    sky.Initialize();
    sky.staticLoad(L"./Data/Sky/sky.obj", "not_light");
    sky.scale = VECTOR3(30.0f, 30.0f, 30.0f);

    //��
    cube_texture[0] = L"./Data/1.jpg";
    cube_texture[1] = L"./Data/Floor/Floor0.png";
    cube_texture[2] = L"./Data/Floor/Floor1.png";
    cube_texture[3] = L"./Data/Floor/Floor2.png";
    cube_texture[4] = L"./Data/Floor/Floor3.png";
    scube = new SkinndeCube(device, cube_texture, 5);

    player.Initialize(new GeometricCube(pFramework->getDevice()));
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
    // �r���[�ϊ��s��
    view = camera.GetViewMatrix();

    //field.Render(view, projection, lightDirection,wireframe);
    player.Render(view, projection, lightDirection, wireframe);
    sky.Render(view, projection, lightDirection, wireframe);
    field.Render(view, projection, lightDirection, wireframe);
    player.Render(view, projection, lightDirection, wireframe);
    sky.Render(view, projection, lightDirection, wireframe);

    //block.Render(context, view, projection, lightDirection, wireframe);

    //��
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX Scale, Rotation, Rx, Ry, Rz, Translation;

    //	������
    world = DirectX::XMMatrixIdentity();
    //	�g��E�k��
    Scale = DirectX::XMMatrixScaling(1, 0.5, 1);

    //	��]
    Rx = DirectX::XMMatrixRotationX(0);				//	X������Ƃ�����]�s��
    Ry = DirectX::XMMatrixRotationY(0);				//	Y������Ƃ�����]�s��
    Rz = DirectX::XMMatrixRotationZ(0);				//	Z������Ƃ�����]�s��
    Rotation = Rz * Ry * Rx;

    //	���s�ړ�
    Translation = DirectX::XMMatrixTranslation(-10,0,0);

    //	���[���h�ϊ��s��
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
    pPause->Draw();
}

