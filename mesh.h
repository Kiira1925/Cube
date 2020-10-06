#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <vector>
#include <fstream>
#include <string>



class Static_mesh
{
protected:
    // 頂点フォーマット
    struct vertex
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT2 texcoord;
    };

    // 定数バッファ
    struct cbuffer
    {
        DirectX::XMFLOAT4X4 world_view_projection;      //ワールド・ビュー・プロジェクション合成行列  
        DirectX::XMFLOAT4X4 world;                      //ワールド変換行列 
        DirectX::XMFLOAT4   material_color;             //材質色 
        DirectX::XMFLOAT4   light_direction;            //ライト進行方向
    };

    struct subset
    {
        std::wstring usemtl;   // usemtlの後に続くのがマテリアル名
        u_int index_start = 0; // start number of index buffer
        u_int index_count = 0; // number of vertices (indices)
    };
    std::vector<subset> subsets;

    struct material
    {
        std::wstring newmtl; 
        DirectX::XMFLOAT3 Ka = { 0.2f,0.2f,0.2f }; // Ka 環境光
        DirectX::XMFLOAT3 Kd = { 0.8f,0.8f,0.8f }; // Kd 拡散光
        DirectX::XMFLOAT3 Ks = { 1.0f,1.0f,1.0f }; // Ks 鏡面光
        u_int illum = 1;        // illum 照明モデル
        // u_int Ns = 0;           // Ns 鏡面光の焦点範囲
        std::wstring map_Kd;    // map Kd 画像名
        std::wstring map_bump;  // バンプマップ　凸凹表現するための画像
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
    };
    std::vector<material> materials;

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader>       p_vertexShader = NULL;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>        p_pixelShader = NULL;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>        p_inputLayout = NULL;
    Microsoft::WRL::ComPtr<ID3D11Buffer>             p_vertexBuffer = NULL;          // 頂点バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer>             p_indexBuffer = NULL;           // インデックスバッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer>             p_constBuffer = NULL;           // 定数バッファ
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    p_rasterizerState[2];
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  p_depthStancilState = NULL;

    // テクスチャを使用する際に外す
    //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView = NULL;
    Microsoft::WRL::ComPtr<ID3D11SamplerState>       pSamplerState = NULL;
    Microsoft::WRL::ComPtr<ID3D11Resource>           pResource;
    Microsoft::WRL::ComPtr<ID3D11Texture2D>          pTexture2D;
    D3D11_TEXTURE2D_DESC                             texture2dDesc;

    int                 numIndices = 0;

public:

    void render(
        ID3D11DeviceContext*,   //デバイスコンテキスト 
        const DirectX::XMFLOAT4X4&, //ワールド・ビュー・プロジェクション合成行列 
        const DirectX::XMFLOAT4X4&, //ワールド変換行列
        const DirectX::XMFLOAT4&,  //ライト進行方向 
        const DirectX::XMFLOAT4&,  //材質色 
        bool       //線・塗りつぶし描画フラグ 
    );
public:
    bool CreateBuffers(ID3D11Device* device, vertex* vertices, int numVertex, unsigned int* indices, int numIndexndex);


public:
    Static_mesh(ID3D11Device* device, const wchar_t* objfile ,bool flippingCoordinates);
    Static_mesh(ID3D11Device* device, const wchar_t* objfile , const char* shaderfile ,bool flippingCoordinates);
    //Static_mesh(ID3D11Device* device, const char* objfile,const wchar_t* p_texturefile);
    ~Static_mesh() {};
    
};