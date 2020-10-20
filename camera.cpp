#include	"camera.h"
#include <stdlib.h>
#include "framework.h"


void Camera::Active()
{
    // 視野変更行列作成
    DirectX::XMMATRIX v = DirectX::XMLoadFloat4x4(&view);
    DirectX::XMVECTOR e = DirectX::XMVectorSet(eye.x, eye.y, eye.z, 0.0f);
    DirectX::XMVECTOR f = DirectX::XMVectorSet(focus.x, focus.y, focus.z, 0.0f);
    DirectX::XMVECTOR u = DirectX::XMVectorSet(up.x, up.y, up.z, 0.0f);
    v = DirectX::XMMatrixLookAtLH(e, f, u);
    DirectX::XMStoreFloat4x4(&view, v);

    // 投影変換行列作成
    DirectX::XMMATRIX p = DirectX::XMLoadFloat4x4(&projection);
    p = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ);
    DirectX::XMStoreFloat4x4(&projection, p);
    framework::getInstance()->debug->setString("camera_pos:%f,%f,%f", eye.x, eye.y, eye.z);
    framework::getInstance()->debug->setString("camera_focus:%f,%f,%f", focus.x, focus.y, focus.z);
}

void Camera::SetPerspective(float fov, float aspect, float nearZ, float farZ)
{
    this->fov = fov;
    this->aspect = aspect;
    this->nearZ = nearZ;
    this->farZ = farZ;
}

void Camera::SetEye(const DirectX::XMFLOAT3& eye)
{
    this->eye = eye;
}

void Camera::SetFocus(const DirectX::XMFLOAT3& focus)
{
    this->focus = focus;
}

void Camera::SetUp(const DirectX::XMFLOAT3& up)
{
    this->up = up;
}

//Maincamera

void MainCamera::Vibrate(float elapsedTime)
{
    if (time > 0)
    {
        time -= elapsedTime;
        DirectX::XMFLOAT3 focus;
        float dx = (rand() % 3 - 1) * range * time;
        float dy = (rand() % 3 - 1) * range * time;
        float dz = (rand() % 3 - 1) * range * time;
        focus.x = moveTarget.x + dx;
        focus.y = moveTarget.y + dy;
        focus.z = moveTarget.z + dz;
        SetFocus(focus);
    }
}

void MainCamera::Chase(float elapsedTime)
{
    DirectX::XMFLOAT3 eye = GetEye();
    float dx = eye.x - moveTarget.x;
    float dz = eye.z - moveTarget.z;

    // 距離を測って(Y方向は入れない)正規化
    float d = sqrtf(dx * dx + dz * dz);
    // 距離:10
    //framework::getInstance()->debug->setString("camera_distance:%f", d);

    if (d > 0) {
        dx /= d;
        dz /= d;
    }

    //カメラ位置を逆算する
    if (d > FAR_DIST) {
        eye.x = moveTarget.x + dx * FAR_DIST;
        eye.z = moveTarget.z + dz * FAR_DIST;
    }

    if (d < NEAR_DIST) {
        eye.x = moveTarget.x + dx * NEAR_DIST;
        eye.z = moveTarget.z + dz * NEAR_DIST;
    }
    //カメラ情報セット
    SetEye(eye);
    SetFocus(moveTarget);

    // 130度
    //framework::getInstance()->debug->setString("camera_angle:%f", XMConvertToDegrees(atan2f(moveTarget.z-eye.z,moveTarget.x-eye.y)));
}

void MainCamera::Fix(float elapsedTime)
{
    SetFocus(GetFocus());
}

void MainCamera::Side(float elapsedTime)
{
    float radian = XMConvertToRadians(130);
    float dist = 10.0f;

    XMFLOAT3 camera_pos = { moveTarget.x+cosf(radian)*dist, 10.0f, moveTarget.z-sinf(radian)*dist };

    SetEye(camera_pos);
    SetFocus(XMFLOAT3(moveTarget.x,0,moveTarget.z));
}

void MainCamera::Updata(float elapsedTime)
{
    switch (mode) {
    case MODE::MODE_FIX:
        Fix(elapsedTime);
        break;
    case MODE::MODE_CHASE:
        Chase(elapsedTime);
        break;
    case MODE::MODE_SIDE:
        Side(elapsedTime);
        break;
    }
    //Vibrate(elapsedTime); //振動カメラ
    Active();
}

void MainCamera::SetMode(MODE mode)
{
    this->mode = mode;
}

void MainCamera::SetVibration(float range, float timer)
{
    this->range = range;
    this->time = timer;
}