#pragma once
#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>

#include <wrl.h>		// ComPtr

using namespace DirectX;

class GeometricPrimitive
{
protected:
    // 頂点フォーマット
    struct vertex
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT2 texcoord;
    };

    struct Texture
    {
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> data;
        D3D11_TEXTURE2D_DESC desc;
    };

    Texture texture;

    // 定数バッファ
    struct cbuffer
    {
        DirectX::XMFLOAT4X4 world_view_projection;      //ワールド・ビュー・プロジェクション合成行列  
        DirectX::XMFLOAT4X4 world;                      //ワールド変換行列 
        DirectX::XMFLOAT4   material_color;             //材質色 
        DirectX::XMFLOAT4   light_direction;            //ライト進行方向
    };

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader>       p_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>        p_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>        p_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>             p_vertexBuffer;       // 頂点バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer>             p_indexBuffer;        // インデックスバッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer>             p_constBuffer;        // 定数バッファ
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    p_rasterizerState[2];
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  p_depthStancilState;
   
    // テクスチャを使用する際に外す
    // Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
    // D3D11_TEXTURE2D_DESC texture2dDesc;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> pSamplerState;

    int                 numIndices;

public:

    void render(
        ID3D11DeviceContext*,   //デバイスコンテキスト 
        const DirectX::XMFLOAT4X4&, //ワールド・ビュー・プロジェクション合成行列 
        const DirectX::XMFLOAT4X4&, //ワールド変換行列
        const DirectX::XMFLOAT4&,  //ライト進行方向 
        const DirectX::XMFLOAT4&,  //材質色 
        bool       //線・塗りつぶし描画フラグ 
    );
    void render(
        ID3D11DeviceContext*,   //デバイスコンテキスト 
        const DirectX::XMFLOAT4X4&, //ワールド・ビュー・プロジェクション合成行列 
        const DirectX::XMFLOAT4X4&, //ワールド変換行列
        const DirectX::XMFLOAT4& color = DirectX::XMFLOAT4(1, 1, 1, 1),  //材質色 
        bool wireframe = false      //線・塗りつぶし描画フラグ 
    );
public:
    bool CreateBuffers(ID3D11Device* device,vertex* vertices, int numVertex,unsigned int* indices,int numIndex);
    bool TextureCreateBuffers(ID3D11Device* device, vertex* vertices, int numVertex, unsigned int* indices, int numIndex, const wchar_t* filename);
    GeometricPrimitive() :p_vertexShader(NULL), p_pixelShader(NULL), p_inputLayout(NULL), p_vertexBuffer(NULL),
        p_indexBuffer(NULL), p_constBuffer(NULL), p_rasterizerState(), p_depthStancilState(NULL),numIndices(0)
    {}
    ~GeometricPrimitive() {};

};

class GeometricBoard : public GeometricPrimitive
{
public:
    GeometricBoard(ID3D11Device* p_device, const wchar_t*);
    ~GeometricBoard() {};
};

class GeometricRect : public GeometricPrimitive
{
public:
    GeometricRect(ID3D11Device* p_device);
    GeometricRect(ID3D11Device* p_device, const wchar_t*);
    ~GeometricRect() {};
};

// 立方体生成
class GeometricCube : public GeometricPrimitive
{
public:
    GeometricCube(ID3D11Device* p_device);
    ~GeometricCube() {};
};

// 円柱生成
class GeometricCylinder : public GeometricPrimitive
{
public:
    GeometricCylinder(ID3D11Device* p_device);
    ~GeometricCylinder() {};
};

// 球生成
class GeometricSphere : public GeometricPrimitive
{
public:
    GeometricSphere(ID3D11Device* p_device,u_int slices,u_int stacks);
    ~GeometricSphere() {};
};

// カプセル生成
class GeometricCapsule : public GeometricPrimitive
{
public:
    GeometricCapsule(ID3D11Device* p_device);
    ~GeometricCapsule() {};
};
