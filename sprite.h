#pragma once
#include <windows.h>
#include <sstream>
#include <DirectXMath.h>
#include <wrl.h>
#include <d3d11.h>

class Sprite
{
private:
    ID3D11VertexShader*         p_vertexshader;
    ID3D11PixelShader*          p_pixelshader;
    ID3D11InputLayout*          p_Inputlayout;
    ID3D11Buffer*               p_verbuffer;        // バッファーとは日本語で[要素] という意味
    ID3D11RasterizerState*      p_rasterizerstate;
    ID3D11ShaderResourceView*   p_shader_resource_view;
    ID3D11SamplerState*         p_sampler_state;
    ID3D11DepthStencilState*    p_depthStencilState; // 深度
    //ID3D11BlendState*           p_blendState; // 透過、重ね
    D3D11_TEXTURE2D_DESC        texture2d_desc;

public:
    void render(ID3D11DeviceContext *,
        // UNIT.03
        float,float, // dx,dy : Coordinate of sprite's left-top corner in screen space 
        float,float,    // dw,dh : Size of sprite in screen space
        float,float,    // sx,sy : Coordinate of sprite's left- top corner in texture space
        float,float,    // sw,sh : Sice of sprite in texture space
        float,      // angle : Raotation angle (Rotation centre is sprite's centre), Unit is degree
        DirectX::XMFLOAT4 // r,g,b,a : Color of sprite's each vertices
    );

    //void render(ID3D11DeviceContext*);

    void Sprite::textout(ID3D11DeviceContext* p_deviceContext,
        std::string s,
        float x, float y,
        float w, float h,
        float r = 1, float g = 1, float b = 1, float a = 1);

    float Sprite::textoutD(ID3D11DeviceContext* p_deviceContext,
        std::string s,
        float x, float y,
        float w, float h,
        float r = 1, float g = 1, float b = 1, float a = 1);


    Sprite(ID3D11Device * , const wchar_t* /*Texture file name*/);
    ~Sprite()
    {
        if (p_vertexshader)         p_vertexshader->Release();
        if (p_pixelshader)          p_pixelshader->Release();
        if (p_Inputlayout)          p_Inputlayout->Release();
        if (p_verbuffer)            p_verbuffer->Release();
        if (p_rasterizerstate)      p_rasterizerstate->Release();
        if (p_shader_resource_view) p_shader_resource_view->Release();
        if (p_sampler_state)        p_sampler_state->Release();
        if (p_depthStencilState)    p_depthStencilState->Release();
        //if (p_blendState)           p_blendState->Release();
    }

    struct vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
        // UNIT.04
        DirectX::XMFLOAT2 texcoord;
    };

};
//
//class SpriteBatch
//{
//private:
//    Microsoft::WRL::ComPtr<ID3D11VertexShader> vtShader;
//    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
//    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
//    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
//    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
//
//    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
//    D3D11_TEXTURE2D_DESC        tex2dDesc;
//    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
//    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
//
//    size_t MAX_INSTANCES;
//
//    struct instance
//    {
//        DirectX::XMFLOAT4X4 ndcTransform;
//        XMFLOAT4 texcoordTransform;
//        XMFLOAT4 color;
//    };
//
//public:
//    struct vertex
//    {
//        XMFLOAT3 position;
//        XMFLOAT2 texcoord;
//    };
//
//    SpriteBatch(ID3D11Device*, const wchar_t*,size_t);
//    ~SpriteBatch();
//
//    void begin(ID3D11DeviceContext*);
//    void render(float, float, float, float,
//        float, float, float, float,
//        float, float, float,
//        float, float, float, float);
//
//    float textout(std::string, float, float,
//        float, float, float, float, float, float);
//    void end(ID3D11DeviceContext*);
//
//private:
//    D3D11_VIEWPORT viewport;
//    UINT instanceCount = 0;
//    instance* instances = nullptr;
//
//};

