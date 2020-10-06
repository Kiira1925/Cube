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
    // ���_�t�H�[�}�b�g
    struct vertex
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT2 texcoord;
    };

    // �萔�o�b�t�@
    struct cbuffer
    {
        DirectX::XMFLOAT4X4 world_view_projection;      //���[���h�E�r���[�E�v���W�F�N�V���������s��  
        DirectX::XMFLOAT4X4 world;                      //���[���h�ϊ��s�� 
        DirectX::XMFLOAT4   material_color;             //�ގ��F 
        DirectX::XMFLOAT4   light_direction;            //���C�g�i�s����
    };

    struct subset
    {
        std::wstring usemtl;   // usemtl�̌�ɑ����̂��}�e���A����
        u_int index_start = 0; // start number of index buffer
        u_int index_count = 0; // number of vertices (indices)
    };
    std::vector<subset> subsets;

    struct material
    {
        std::wstring newmtl; 
        DirectX::XMFLOAT3 Ka = { 0.2f,0.2f,0.2f }; // Ka ����
        DirectX::XMFLOAT3 Kd = { 0.8f,0.8f,0.8f }; // Kd �g�U��
        DirectX::XMFLOAT3 Ks = { 1.0f,1.0f,1.0f }; // Ks ���ʌ�
        u_int illum = 1;        // illum �Ɩ����f��
        // u_int Ns = 0;           // Ns ���ʌ��̏œ_�͈�
        std::wstring map_Kd;    // map Kd �摜��
        std::wstring map_bump;  // �o���v�}�b�v�@�ʉ��\�����邽�߂̉摜
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
    };
    std::vector<material> materials;

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader>       p_vertexShader = NULL;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>        p_pixelShader = NULL;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>        p_inputLayout = NULL;
    Microsoft::WRL::ComPtr<ID3D11Buffer>             p_vertexBuffer = NULL;          // ���_�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer>             p_indexBuffer = NULL;           // �C���f�b�N�X�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer>             p_constBuffer = NULL;           // �萔�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    p_rasterizerState[2];
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  p_depthStancilState = NULL;

    // �e�N�X�`�����g�p����ۂɊO��
    //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView = NULL;
    Microsoft::WRL::ComPtr<ID3D11SamplerState>       pSamplerState = NULL;
    Microsoft::WRL::ComPtr<ID3D11Resource>           pResource;
    Microsoft::WRL::ComPtr<ID3D11Texture2D>          pTexture2D;
    D3D11_TEXTURE2D_DESC                             texture2dDesc;

    int                 numIndices = 0;

public:

    void render(
        ID3D11DeviceContext*,   //�f�o�C�X�R���e�L�X�g 
        const DirectX::XMFLOAT4X4&, //���[���h�E�r���[�E�v���W�F�N�V���������s�� 
        const DirectX::XMFLOAT4X4&, //���[���h�ϊ��s��
        const DirectX::XMFLOAT4&,  //���C�g�i�s���� 
        const DirectX::XMFLOAT4&,  //�ގ��F 
        bool       //���E�h��Ԃ��`��t���O 
    );
public:
    bool CreateBuffers(ID3D11Device* device, vertex* vertices, int numVertex, unsigned int* indices, int numIndexndex);


public:
    Static_mesh(ID3D11Device* device, const wchar_t* objfile ,bool flippingCoordinates);
    Static_mesh(ID3D11Device* device, const wchar_t* objfile , const char* shaderfile ,bool flippingCoordinates);
    //Static_mesh(ID3D11Device* device, const char* objfile,const wchar_t* p_texturefile);
    ~Static_mesh() {};
    
};