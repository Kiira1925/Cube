#pragma once

#include <DirectXMath.h>

#include "skinned_mesh.h"
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
    Skinned_Mesh* skinnedobj;
    // Skinned_Mesh* model[10] = { 0 };
    Static_mesh*  staticObj;
    
    GeometricPrimitive* primitive;

    VECTOR3 pos;          // 「位置」
    VECTOR3 angle;        // 「(回転)角度」
    VECTOR3 scale;        // 「大きさ」

    DirectX::XMFLOAT4 color;

    //OBB     obb;

    void    Initialize();
    // void    Initialize();
    bool    skinnedLoad(const char* fbxFilename, bool clockflg = false);
    bool    staticLoad(const wchar_t* objfilename, const char* shaderfilename);
    // bool    Loads(int i, const wchar_t* filename);
    void    SetPrimitive(GeometricPrimitive* _primitive);
    void    SetMesh(MyMesh& org);
    void    Release();
    void    Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir,bool wireframe,float elapsedTime);

    DirectX::XMMATRIX GetWorldMatrix();
};