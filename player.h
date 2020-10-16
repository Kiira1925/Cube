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
    VECTOR3   angle; // ��l�̃J�����Ŏg�p����Ă����̂ňꉞ�c���Ă����܂�

    DirectX::XMFLOAT4 axis;  // ���̕ۑ��p
    DirectX::XMFLOAT4 orientation; // ��]�������ʂ̕ۑ��p (����,��������)
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