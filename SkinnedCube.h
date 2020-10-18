#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <vector>
#include <memory>
#include <DirectXMath.h>

using namespace DirectX;
using namespace std;
typedef const wchar_t* FileName;

class SkinnedCube final
{
private:
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 texcoord;
	};

	struct Texture
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> data;
		D3D11_TEXTURE2D_DESC desc;
	};

	vector<Texture> texture;
	struct Cbuffer
	{
		DirectX::XMFLOAT4X4	wvp;
		DirectX::XMFLOAT4X4	world;
		DirectX::XMFLOAT4	material_color;
		DirectX::XMFLOAT4	light_direction;
	};

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	ID3D11Buffer* vertex_buffer[2];
	ID3D11Buffer* index_buffer[2];
	ID3D11Buffer* constant_buffer;
	ID3D11SamplerState* sampler_state;
	ID3D11RasterizerState* rasterizer_state;
	ID3D11DepthStencilState* depth_stencil_state;
	//ID3D11DeviceContext* KeepContext;
	void CreateBuffers(ID3D11Device* device,UINT num,Vertex* vertices, UINT numV, UINT* indices, UINT numI);
	void LoadTexture(ID3D11Device * device,FileName texture);
public:
	//�R���X�g���N�^
	//FileName texture[] -> ���[�h���Ă���e�N�X�`���̃t�@�C���A�h���X�̔z��
	//UINT size -> �z��̗v�f��
	SkinnedCube(ID3D11Device* device,FileName texture[],UINT size);
	~SkinnedCube();

	//�`��֐�
	//UINT num -> �g�p����e�N�X�`���ԍ�
	//const XMFLOAT4X4& wvp -> ���[���h�r���[�v���W�F�N�V�����s��
	//const XMFLOAT4X4& world -> ���[���h�s��
	//const XMFLOAT4& material_color ->�@�}�e���A���J���[
	void Render(ID3D11DeviceContext* context,
		UINT num,
		const XMFLOAT4X4& wvp,
		const XMFLOAT4X4& world,
		const XMFLOAT4& material_color = XMFLOAT4(1, 1, 1, 1));
};

