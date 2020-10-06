#pragma once
#include <DirectXMath.h>
#include "myMesh.h"
#include "vector.h"
#include "collision.h"

class Player
{
public:
    MyMesh              obj;
    VECTOR3   pos;
    VECTOR3   angle;
    VECTOR3   scale;

    DirectX::XMFLOAT4 color;

    int animetion;

    //OBB playerObb;
    //bool              exist;
    VECTOR3 speed = {};
    VECTOR3 moveAngle = {};
    int timer = 0;

    void Initialize(GeometricPrimitive* _primitive);

    void Move();
    void Release();
    void Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& lightDir, bool wireframe);

};