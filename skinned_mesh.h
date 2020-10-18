#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include "vector.h"
#include <vector>
#include <fstream>
#include <string>

#define DEBUG 0
#define MAX_BONE_INFLUNCES 4
#define MAX_BONES 128

class Skinned_Mesh
{
public:
    // 頂点フォーマット
    struct vertex
    {
        VECTOR3 pos;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT2 texcoord;
        FLOAT boneWeights[MAX_BONE_INFLUNCES] = { 1,0,0,0 };
        INT   boneIndices[MAX_BONE_INFLUNCES] = {};
    };

    struct cbuffer
    {
        DirectX::XMFLOAT4X4 worldViewProjection;
        DirectX::XMFLOAT4X4 world;
        DirectX::XMFLOAT4   materialColor;
        DirectX::XMFLOAT4   lightDirection;
        // UNIT.21
        DirectX::XMFLOAT4X4 bone_transforms[MAX_BONES] = { { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
    };

    struct material
    {
        DirectX::XMFLOAT4 color = { 0.8f,0.8f,0.8f,1.0f };
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
    };
    //material material;

    struct subset
    {
        u_int indexStart = 0;   // start number of index buffer
        u_int indexCount = 0;   // number of vertices(indices)
        material material;
    };
    //std::vector<subset> subsets;

    struct bone
    {
        DirectX::XMFLOAT4X4 transform;
    };
    typedef std::vector<bone> skeletal;
    // スケルタル(skeletal)の集合をスケルタル・アニメーション(skeletal_animation)と定義
    struct skeletalAnimation : public std::vector<skeletal>
    {
        float samplingTime = 1 / 24.0f;
        float animationTick = 0.0f;
    };

    struct mesh
    {
        Microsoft::WRL::ComPtr<ID3D11Buffer>vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>indexBuffer;
        std::vector<subset> subsets;

        DirectX::XMFLOAT4X4 globalTransform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

        std::vector<skeletalAnimation> skeletalAnimation;

        void CreateBuffers(ID3D11Device* device, vertex* vertices, int numVertices, u_int* indices, int numIndices);
    };
    std::vector<mesh> meshes;

private:
    // 変数
    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>  inputLayout;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState>   rasterizerState[2];
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

    //int       num_indices;

public:
    Skinned_Mesh(ID3D11Device* device, const char* fbxFilename, bool clockflg = false);
    virtual ~Skinned_Mesh() {}

    void render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& wvp, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4 lightDirection, const DirectX::XMFLOAT4 materialColor, bool wireframe, float elapsedTime, size_t animation = 0);

protected:

    //skinned_mesh クラスのメンバ変数に座標軸変換行列を追加
    // 右手座標系から左手座標系へ
    // convert coordinate system from 'UP:+Z FRONT:+Y RIGHT-HAND' to 'UP:+Y FRONT:+Z LEFT-HAND'
    DirectX::XMFLOAT4X4 coordinateConversion = {
        1, 0, 0, 0,
        0, 0, 1, 0,
        0, 1, 0, 0,
        0, 0, 0, 1
    };
    //※Y 軸と Z 軸を入れ替える
};