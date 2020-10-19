#include "framework.h"
#include "sprite.h"
#include <WICTextureLoader.h>
#pragma  comment(lib,"d3d11.lib")



Sprite::Sprite(ID3D11Device* p_device, const wchar_t* p_texturefile)
{
	vertex vertices[] = {
		{ DirectX::XMFLOAT3(-0.5, +0.5,0),DirectX::XMFLOAT4(1,1,1,1), DirectX::XMFLOAT2(0.0f, 0.0f)},
		{ DirectX::XMFLOAT3(+0.5, +0.5,0),DirectX::XMFLOAT4(1,0,0,1), DirectX::XMFLOAT2(1.0f, 0.0f)},
		{ DirectX::XMFLOAT3(-0.5, -0.5,0),DirectX::XMFLOAT4(0,1,0,1), DirectX::XMFLOAT2(0.0f, 1.0f)},
		{ DirectX::XMFLOAT3(+0.5, -0.5,0),DirectX::XMFLOAT4(0,0,1,1), DirectX::XMFLOAT2(1.0f, 1.0f)},
	};


	//  ���_�o�b�t�@�[�̎��̐錾
	D3D11_BUFFER_DESC      buffrdesc = {};

	// �����ȏ�̎�������ꍇ��
	// ��������O�ɏ����������Ă�������
	// ����͐����݂̂Ȃ̂ŏ��������Ȃ��Ă���
	// ���̂̃|�C���^�[�Ȃ̂ł��ׂĂ�&��t���Ă����
	// �������̎Q�l��framework.cpp�̃o�b�t�@�[�Ȃǂ��Q�l
	buffrdesc.ByteWidth = sizeof(vertices);
	buffrdesc.Usage = D3D11_USAGE_DYNAMIC;
	buffrdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffrdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffrdesc.MiscFlags = 0;
	buffrdesc.StructureByteStride = 0;


	D3D11_SUBRESOURCE_DATA  subresource_data = {};
	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;

	p_device->CreateBuffer(&buffrdesc, &subresource_data, &p_verbuffer);


	// ���̓��C�A�E�g�I�u�W�F�N�g�̐���
	// ���_shader�̃f�[�^���~��������
	// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̑O�ɂ���
	//D3D11_INPUT_ELEMENT_DESC    input_elementdesc;
	// ���_���C�A�E�g
	D3D11_INPUT_ELEMENT_DESC input_elementdesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA ,0 },
		{ "TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA ,0 },
	};

	//�@���_�V�F�[�_�[�I�u�W�F�N�g�������邽�߂�
	//  CSO�t�@�C���̓ǂݍ��݂�����
	//  sprite_vs.cso�̃��[�h
	FILE* fp = 0;
	fopen_s(&fp, "./Shaders/sprite_vs.cso", "rb");
	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	unsigned char* cso_data = new unsigned char[cso_sz];
	fread(cso_data, cso_sz, 1, fp);

	// ���_�V�F�[�_�[�I�u�W�F�b�N�g����
	p_device->CreateVertexShader(cso_data, cso_sz, NULL, &p_vertexshader);

	// ���̓��C�A�E�g�̍쐬
	p_device->CreateInputLayout(input_elementdesc, ARRAYSIZE(input_elementdesc), cso_data, cso_sz, &p_Inputlayout); // ��2�������Ⴄ������ �����O��1������
	// ��3�����͒��_�f�[�^���~���� 

	// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�𐶐����邽�߂�
	// CSO�t�@�C����ǂݍ���
	//  sprite_ps.cso�̃��[�h
	fp = 0;
	fopen_s(&fp, "./Shaders/sprite_ps.cso", "rb");
	fseek(fp, 0, SEEK_END);
	cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	cso_data = new unsigned char[cso_sz];
	fread(cso_data, cso_sz, 1, fp);


	// �s�N�Z���V�F�[�_�[�I�u�W�F�b�N�g����
	p_device->CreatePixelShader(cso_data, cso_sz, NULL, &p_pixelshader);


	// ���X�^���C�U�[�X�e�[�g�̐ݒ�
	D3D11_RASTERIZER_DESC   rasterrizerdesc;
	p_device->CreateRasterizerState(&rasterrizerdesc, &p_rasterizerstate);


	// �摜�t�@�C���̃��[�h�ƃV�F�[�_�[�@���\�[�X�@�r���[�@�I�u�W�F�N�g�̐���
	//ID3D11ShaderResourceView* p_shader_resource_view;  ���ʂɍ쐬���Ă���(����sprite.h�ɐ錾���Ă���)
	ID3D11Resource* p_resource;

	DirectX::CreateWICTextureFromFile(p_device, p_texturefile, &p_resource, &p_shader_resource_view);

	// �e�N�X�`�����̎擾
	//D3D11_TEXTURE2D_DESC texture2d_desc;
	ID3D11Texture2D* p_texture2D;
	p_resource->QueryInterface(&p_texture2D);
	p_texture2D->GetDesc(&texture2d_desc);

	// �T���v���[ �X�e�[�g �I�u�W�F�N�g�̐���
	D3D11_SAMPLER_DESC sampler_desc;

	::ZeroMemory(&sampler_desc, sizeof(D3D11_SAMPLER_DESC));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	p_device->CreateSamplerState(&sampler_desc, &p_sampler_state);

	// �[�x�X�e���V���X�e�[�g�I�u�W�F�N�g�̐ݒ�
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {}; // ������
	//ZeroMemory(&depthStencilDesc, 0); ������
	depthStencilDesc.DepthEnable = false;       // �[�x�e�X�g
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;  // �[�x��������
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	// stencil�̐ݒ�
	depthStencilDesc.StencilEnable = false; // �O��֌W�������������͂��̎q��false�����ĂȂ��\������
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// �쐬
	p_device->CreateDepthStencilState(&depthStencilDesc, &p_depthStencilState);

	// �u�����f�B���O�X�e�[�g�I�u�W�F�N�g�̐ݒ�
	//D3D11_BLEND_DESC blendDesc = {};
	//// �쐬
	//p_device->CreateBlendState(&blendDesc,&p_blendState);
}


void Sprite::render(ID3D11DeviceContext* p_context,
	float _dx, float _dy,
	float _dw, float _dh,
	float _sx, float _sy,
	float _sw, float _sh,
	float _angle,
	DirectX::XMFLOAT4 col)
	//void Sprite::render(ID3D11DeviceContext* p_context)
{

	// ���_�f�[�^�̍X�V�e�X�g
	{
		vertex vertexs[4] = {
		{ DirectX::XMFLOAT3(0.0f, 0.0f, .0f), col,DirectX::XMFLOAT2(_sx,_sy)},
		{ DirectX::XMFLOAT3(1.0f, 0.0f, .0f), col,DirectX::XMFLOAT2(_sx + _sw, _sy)},
		{ DirectX::XMFLOAT3(0.0f, 1.0f, .0f), col,DirectX::XMFLOAT2(_sx,_sy + _sh)},
		{ DirectX::XMFLOAT3(1.0f, 1.0f, .0f), col,DirectX::XMFLOAT2(_sx + _sw, _sy + _sh)},
		};

		//	(dw,dh)�̊g��
		for (int i = 0; i < 4; i++) {
			vertexs[i].position.x *= _dw;
			vertexs[i].position.y *= _dh;
		}

		//	��]���̕ύX�i��`�����ցj
		for (int i = 0; i < 4; i++) {
			vertexs[i].position.x -= _dw / 2.0f;
			vertexs[i].position.y -= _dh / 2.0f;
		}

		//	angle�x�̉�]
		float degree = DirectX::XMConvertToRadians(_angle);
		float sn = sinf(degree);
		float cs = cosf(degree);
		for (int i = 0; i < 4; i++) {
			float bx = vertexs[i].position.x;
			float by = vertexs[i].position.y;
			vertexs[i].position.x = bx * cs - by * sn;
			vertexs[i].position.y = bx * sn + by * cs;
		}

		//	��]���̕ύX�i���̈ʒu�ցj
		for (int i = 0; i < 4; i++) {
			vertexs[i].position.x += _dw / 2.0f;
			vertexs[i].position.y += _dh / 2.0f;
		}

		//	(dx,dy)�̕��s�ړ�
		for (int i = 0; i < 4; i++) {
			vertexs[i].position.x += _dx;
			vertexs[i].position.y += _dy;
		}

		vertex vertexsNDC[4];
		for (int n = 0; n < 4; n++)
		{
			//	NDC�ϊ�
			vertexsNDC[n].position.x = (vertexs[n].position.x / SCREEN_WIDTH) * 2.0f - 1.0f;
			vertexsNDC[n].position.y = -(vertexs[n].position.y / SCREEN_HEIGHT) * 2.0f + 1.0f;
			vertexsNDC[n].position.z = .0f;
			vertexsNDC[n].color = vertexs[n].color;

			// UV���W���K��
			vertexsNDC[n].texcoord.x = vertexs[n].texcoord.x / texture2d_desc.Width;
			vertexsNDC[n].texcoord.y = vertexs[n].texcoord.y / texture2d_desc.Height;
		}

		// �}�b�v�Ɋւ��邱��

		// ���_�o�b�t�@�[�ʒu���X�V������o�C���h���Ĉʒu���ς�������̂������Ă���
		D3D11_MAPPED_SUBRESOURCE mapped_subresource;
		p_context->Map(p_verbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);   // �\�[�X�̈Ӗ���[���ƁA��]�Ƃ����Ӗ�������
		// �������[�Ƀ}�b�v�����R�s�[
		CopyMemory(mapped_subresource.pData, vertexsNDC, sizeof(vertex) * 4);
		//�v�Z���ʂł̒��_�o�b�t�@�[�I�u�W�F�b�N�g���X�V
		p_context->Unmap(p_verbuffer, 0);

	}

	// �i�[���邽�߂̃o�[�e�b�N�X�̒��g������������
	UINT stride = sizeof(vertex);        // �i�[���鑤�̏�����
	UINT offset = 0;                     // �ǂݍ��݂̈ʒu

	// ���_�o�b�t�@�[�̃o�C���h
	p_context->IASetVertexBuffers(0, 1, &p_verbuffer, &stride, &offset);

	// �v���~�e�B�u�f�[�^�̃o�C���h
	p_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // �`�悳��Ȃ������Ƃ��͈������Ⴄ���̂������@

	// ���̓��C�A�E�g�@�I�u�W�F�N�g�̃o�C���h
	p_context->IASetInputLayout(p_Inputlayout);

	// ���X�^���C�U�[�̃X�e�[�g
	p_context->RSSetState(p_rasterizerstate);


	// �V�F�[�_�[�̐ݒ�
	p_context->VSSetShader(p_vertexshader, NULL, 0);
	p_context->PSSetShader(p_pixelshader, NULL, 0);

	// �V�F�[�_�[���\�[�X�̐ݒ�
	p_context->PSSetShaderResources(0, 1, &p_shader_resource_view);

	// �T���v���[�X�e�[�g�̐ݒ�
	p_context->PSSetSamplers(0, 1, &p_sampler_state);

	// �[�x�X�e���V���X�e�[�g�I�u�W�F�N�g�̐ݒ�
	p_context->OMSetDepthStencilState(p_depthStencilState, 0);

	// �u�����f�B���O�X�e�[�g�I�u�W�F�N�g�̐ݒ�
	//p_context->OMSetBlendState(p_blendState,nullptr,0xFFFFFFFF);

	// �v���~�e�B�u�̕`��
	p_context->Draw(4, 0);
}

void Sprite::textout(ID3D11DeviceContext* p_deviceContext,
	std::string s,
	float x, float y,
	float w, float h,
	float r, float g, float b, float a)
{
	float sw = static_cast<float>(texture2d_desc.Width / 16);
	float sh = static_cast<float>(texture2d_desc.Height / 16);
	float cursor = 0;
	for (auto c : s) // �����R�[�h������
	{
		render(p_deviceContext,
			x + cursor, y,
			w, h,
			sw * (c % 16), sh * (c / 16), sw, sh, 0, DirectX::XMFLOAT4(r, g, b, a));
		cursor += w;
	}
}

float Sprite::textoutD(ID3D11DeviceContext* p_deviceContext,
	std::string s,
	float x, float y,
	float w, float h,
	float r, float g, float b, float a)
{
	float sw = static_cast<float>(texture2d_desc.Width / 16);
	float sh = static_cast<float>(texture2d_desc.Height / 16);
	float cursor = 0;
	for (auto c : s) // �����R�[�h������
	{
		render(p_deviceContext,
			x + cursor, y,
			w, h,
			sw * (c % 16), sh * (c / 16), sw, sh, 0, DirectX::XMFLOAT4(r, g, b, a));
		cursor += w;
	}

	return sh;

}
//
//SpriteBatch::SpriteBatch(ID3D11Device* device, const wchar_t* fileName, size_t maxInstance)
//{
//	MAX_INSTANCES = maxInstance;
//
//
//
//}
//
