#include "scene.h"

#include "camera.h"
#include "player.h"

Camera camera1;
Player player1;

void SceneTitle::Initialize()
{
    // 投影変換行列
    projection = camera1.GetProjectionMatrix();

    // 光源(平行光)
    lightDirection = DirectX::XMFLOAT4(1, 10, 2, 0);

    player1.Initialize(new GeometricCube(pFramework->getDevice()));
}

void SceneTitle::Update(float elapsedTime)
{
    player1.Move();
    camera1.Updata();

    if (GetAsyncKeyState('V') & 1)
    {
        SceneManager::Instance().ChangeScene(SceneGame::getInstance());
        return;
    }
}
void SceneTitle::Render(float elapsedTime)
{

    // ビュー変換行列
    view = camera1.GetViewMatrix();

    player1.Render(view, projection, lightDirection,false);
}
