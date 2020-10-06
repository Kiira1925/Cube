#pragma once

#include <DirectXMath.h>

#include "mesh.h"
#include "geometric_primitive.h"
#include "collision.h"

#include "vector.h"


class MyMesh
{
private:
    bool        bLoad;
    int         MaxModel;

public:
    Static_mesh*  staticObj;
    
    GeometricPrimitive* primitive;

    VECTOR3 pos;          // �u�ʒu�v
    VECTOR3 angle;        // �u(��])�p�x�v
    VECTOR3 scale;        // �u�傫���v

    DirectX::XMFLOAT4 color;

    //OBB     obb;

    void    Initialize();
    bool    staticLoad(const wchar_t* objfilename, const char* shaderfilename);
    void    SetPrimitive(GeometricPrimitive* _primitive);
    void    SetMesh(MyMesh& org);
    void    Release();
    void    Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir,bool wireframe , size_t animation = 0);

    DirectX::XMMATRIX GetWorldMatrix();
};