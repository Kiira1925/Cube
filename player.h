#pragma once
#include <DirectXMath.h>
#include "myMesh.h"
#include "vector.h"
#include "collision.h"
#include "sprite.h"

class Player
{
private:
    
public:
    MyMesh              obj;
    VECTOR3   pos;
    VECTOR3   angle; // 一人称カメラで使用されていたので一応残しておきます

    DirectX::XMFLOAT4 axis;  // 軸の保存用
    DirectX::XMFLOAT4 orientation; // 回転した結果の保存用 (方位,方向ずけ)
    VECTOR3   scale;

    DirectX::XMFLOAT4 color;

    int animetion;

    //OBB playerObb;
    //bool              exist;
    VECTOR3 speed = {};
    FLOAT moveAngle = {};
    int timer = 0;

    void Initialize(const wchar_t* objfilename, const char* shaderfilename);

    void Move();
    void Release();
    void Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& lightDir, bool wireframe);
    DirectX::XMMATRIX GetWorldMatrix();

};