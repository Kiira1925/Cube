#pragma once
#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>

#include <wrl.h>		// ComPtr

using namespace DirectX;

class GeometricPrimitive
{
protected:
    // ���_�t�H�[�}�b�g
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

    // �萔�o�b�t�@
    struct cbuffer
    {
        DirectX::XMFLOAT4X4 world_view_projection;      //���[���h�E�r���[�E�v���W�F�N�V���������s��  
        DirectX::XMFLOAT4X4 world;                      //���[���h�ϊ��s�� 
        DirectX::XMFLOAT4   material_color;             //�ގ��F 
        DirectX::XMFLOAT4   light_direction;            //���C�g�i�s����
    };

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader>       p_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>        p_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>        p_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>             p_vertexBuffer;       // ���_�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer>             p_indexBuffer;        // �C���f�b�N�X�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer>             p_constBuffer;        // �萔�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    p_rasterizerState[2];
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  p_depthStancilState;
   
    // �e�N�X�`�����g�p����ۂɊO��
    // Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
    // D3D11_TEXTURE2D_DESC texture2dDesc;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> pSamplerState;

    int                 numIndices;

public:

    void render(
        ID3D11DeviceContext*,   //�f�o�C�X�R���e�L�X�g 
        const DirectX::XMFLOAT4X4&, //���[���h�E�r���[�E�v���W�F�N�V���������s�� 
        const DirectX::XMFLOAT4X4&, //���[���h�ϊ��s��
        const DirectX::XMFLOAT4&,  //���C�g�i�s���� 
        const DirectX::XMFLOAT4&,  //�ގ��F 
        bool       //���E�h��Ԃ��`��t���O 
    );
    void render(
        ID3D11DeviceContext*,   //�f�o�C�X�R���e�L�X�g 
        const DirectX::XMFLOAT4X4&, //���[���h�E�r���[�E�v���W�F�N�V���������s�� 
        const DirectX::XMFLOAT4X4&, //���[���h�ϊ��s��
        const DirectX::XMFLOAT4& color = DirectX::XMFLOAT4(1, 1, 1, 1),  //�ގ��F 
        bool wireframe = false      //���E�h��Ԃ��`��t���O 
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

// �����̐���
class GeometricCube : public GeometricPrimitive
{
public:
    GeometricCube(ID3D11Device* p_device);
    ~GeometricCube() {};
};

// �~������
class GeometricCylinder : public GeometricPrimitive
{
public:
    GeometricCylinder(ID3D11Device* p_device);
    ~GeometricCylinder() {};
};

// ������
class GeometricSphere : public GeometricPrimitive
{
public:
    GeometricSphere(ID3D11Device* p_device,u_int slices,u_int stacks);
    ~GeometricSphere() {};
};

// �J�v�Z������
class GeometricCapsule : public GeometricPrimitive
{
public:
    GeometricCapsule(ID3D11Device* p_device);
    ~GeometricCapsule() {};
};
