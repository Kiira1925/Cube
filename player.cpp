
#include "player.h"

#include "framework.h"

float NormalizeAngle(float radian)
{
    while (radian > 3.141592654f) radian -= 3.141592654f * 2;
    while (radian < -3.141592654f) radian += 3.141592654f * 2;
    return radian;
}

void Player::Initialize(GeometricPrimitive* _primitive)
{
    obj.Initialize();
    obj.SetPrimitive(_primitive);
}


void Player::Move()
{
    const float dangle = DirectX::XMConvertToRadians(90.0f / 60);		//	1度
    const float moveSpeed = 1.0f / 60;									//	プレイヤーの速度
    // pushStateFlg = false;
    if (speed.x == 0 && speed.y == 0 && speed.z == 0)
    {
        //	回転
        if (GetAsyncKeyState(VK_LEFT) < 0)
        {
            if (angle.x != XMConvertToRadians(90) && angle.x != XMConvertToRadians(-90))
            {
                moveAngle.z += dangle;
                speed.x -= moveSpeed;
            }
            else
            {
                moveAngle.y += dangle;
                speed.x -= moveSpeed;
            }
        }
        else if (GetAsyncKeyState(VK_RIGHT) < 0)
        {
            if (angle.x != XMConvertToRadians(90) && angle.x != XMConvertToRadians(-90))
            {
                moveAngle.z -= dangle;
                speed.x += moveSpeed;
            }
            else
            {
                moveAngle.y -= dangle;
                speed.x += moveSpeed;
            }
        }
        else if (GetAsyncKeyState(VK_UP) < 0)
        {
            // if (pushStateFlg != beforPushStateFlg)
            {
                moveAngle.x += dangle;
                speed.z += moveSpeed;
            }
        }
        else if (GetAsyncKeyState(VK_DOWN) < 0)
        {
            // if (pushStateFlg != beforPushStateFlg)
            {
                moveAngle.x -= dangle;
                speed.z -= moveSpeed;
            }
        }
    }

    if (speed.x != 0 || speed.y != 0 || speed.z != 0)
    {
        angle.x += moveAngle.x;
        angle.y += moveAngle.y;
        angle.z += moveAngle.z;
        pos.x += speed.x;
        pos.y += speed.y;
        pos.z += speed.z;
        timer++;
        if (timer >= 60)
        {
            moveAngle = VECTOR3(0.0f, 0.0f, 0.0f);
            speed = VECTOR3(0.0f, 0.0f, 0.0f);
            timer = 0;
        }
    }
    angle.x = NormalizeAngle(angle.x);
    angle.y = NormalizeAngle(angle.y);
    angle.z = NormalizeAngle(angle.z);

}

void Player::Release()
{
    obj.Release();
}

void Player::Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& lightDir, bool wireframe)
{
    obj.pos = this->pos;
    obj.angle = this->angle;

    obj.Render(view, projection, lightDir, wireframe);
}