#include	"camera.h"
#include <stdlib.h>


void Camera::Active()
{
    // Ž‹–ì•ÏXs—ñì¬
    DirectX::XMMATRIX v = DirectX::XMLoadFloat4x4(&view);
    DirectX::XMVECTOR e = DirectX::XMVectorSet(eye.x, eye.y, eye.z, 0.0f);
    DirectX::XMVECTOR f = DirectX::XMVectorSet(focus.x, focus.y, focus.z, 0.0f);
    DirectX::XMVECTOR u = DirectX::XMVectorSet(up.x, up.y, up.z, 0.0f);
    v = DirectX::XMMatrixLookAtLH(e, f, u);
    DirectX::XMStoreFloat4x4(&view, v);

    // “Š‰e•ÏŠ·s—ñì¬
    DirectX::XMMATRIX p = DirectX::XMLoadFloat4x4(&projection);
    p = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ);
    DirectX::XMStoreFloat4x4(&projection, p);

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

    // ‹——£‚ð‘ª‚Á‚Ä(Y•ûŒü‚Í“ü‚ê‚È‚¢)³‹K‰»
    float d = sqrtf(dx * dx + dz * dz);

    if (d > 0) {
        dx /= d;
        dz /= d;
    }

    //ƒJƒƒ‰ˆÊ’u‚ð‹tŽZ‚·‚é
    if (d > FAR_DIST) {
        eye.x = moveTarget.x + dx * FAR_DIST;
        eye.z = moveTarget.z + dz * FAR_DIST;
    }

    if (d < NEAR_DIST) {
        eye.x = moveTarget.x + dx * NEAR_DIST;
        eye.z = moveTarget.z + dz * NEAR_DIST;
    }
    //ƒJƒƒ‰î•ñƒZƒbƒg
    SetEye(eye);
    SetFocus(moveTarget);

}

void MainCamera::Fix(float elapsedTime)
{
    SetFocus(moveTarget);
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
    }
    //Vibrate(elapsedTime); //U“®ƒJƒƒ‰
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