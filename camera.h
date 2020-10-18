#pragma once

#include	<DirectXMath.h>


class Camera
{
private:
    DirectX::XMFLOAT4X4 view; // 視野変換行列
    DirectX::XMFLOAT4X4 projection; // 投影変換行列
    DirectX::XMFLOAT3 eye; // 視点
    DirectX::XMFLOAT3 focus; // 注視点
    DirectX::XMFLOAT3 up; // 上ベクトル
    float fov; // 画角
    float aspect; // アスペクト比
    float nearZ; // ニアクリップ面までの距離
    float farZ; // ファークリップ面までの距離
public:
    // 行列更新
    void Active();

    // setter
    void SetPerspective(float fov, float aspect,
        float nearZ, float farZ);
    void SetEye(const DirectX::XMFLOAT3& eye);
    void SetFocus(const DirectX::XMFLOAT3& focus);
    void SetUp(const DirectX::XMFLOAT3& up);

    // getter
    const DirectX::XMFLOAT4X4& GetView() const { return view; }
    const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }
    const DirectX::XMFLOAT3& GetEye() const { return eye; }
    const DirectX::XMFLOAT3& GetFocus() const { return focus; }
    const DirectX::XMFLOAT3& GetUp() const { return up; }
};


class MainCamera : public Camera
{
public:
    enum MODE
    {
        MODE_FIX,
        MODE_CHASE,
        MODE_NUM
    };

private:
    MODE mode = MODE_FIX; //カメラモード
    float time = 0.0f; //振動時間
    float range = 0.0f; //振動幅
    DirectX::XMFLOAT3 moveTarget; // ターゲット
    void Vibrate(float elapsedTime); //振動カメラ
    void Chase(float elapsedTime); //追跡カメラ
    void Fix(float elapsedTime); //固定カメラ

public:
    static const constexpr float FAR_DIST = 150.0f;
    static const constexpr float NEAR_DIST = 50.0f;
    void Updata(float elapsedTime); //カメラ更新
    void SetMode(MODE mode); //モード設定
    void SetVibration(float range, float timer); //振動開始
    void SetmodeTarget(DirectX::XMFLOAT3 modeTarget) { this->moveTarget = modeTarget; }
};