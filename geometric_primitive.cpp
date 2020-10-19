#include "geometric_primitive.h"
#include "shader.h"
#include "texture.h"

#include <vector>
#include <sstream>

bool GeometricPrimitive::CreateBuffers(ID3D11Device* p_device, vertex* vertices, int numVertex, unsigned int* indices, int numIndex)
{
	// ���_�o�b�t�@�I�u�W�F�N�g�̐���
	{
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.ByteWidth = sizeof(vertex) * numVertex;
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexSubResource;
		ZeroMemory(&vertexSubResource, sizeof(vertexSubResource));
		vertexSubResource.pSysMem = vertices;
		vertexSubResource.SysMemPitch = 0;
		vertexSubResource.SysMemSlicePitch = 0;
		p_device->CreateBuffer(&vertexBufferDesc, &vertexSubResource, p_vertexBuffer.GetAddressOf());
	}
	if (indices) {
		// �C���f�b�N�X�o�b�t�@�I�u�W�F�N�g�̐���
		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
		indexBufferDesc.ByteWidth = numIndex * sizeof(unsigned int);
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA indexSubResource;
		ZeroMemory(&indexSubResource, sizeof(indexSubResource));
		indexSubResource.pSysMem = indices;
		indexSubResource.SysMemPitch = 0;
		indexSubResource.SysMemSlicePitch = 0;

		numIndices = numIndex;
		p_device->CreateBuffer(&indexBufferDesc, &indexSubResource, p_indexBuffer.GetAddressOf());
	}
	{
		// �萔�o�b�t�@�I�u�W�F�N�g�̐���
		D3D11_BUFFER_DESC constBufferDesc;
		ZeroMemory(&constBufferDesc, sizeof(constBufferDesc));
		constBufferDesc.ByteWidth = sizeof(cbuffer);
		constBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constBufferDesc.CPUAccessFlags = 0;
		constBufferDesc.MiscFlags = 0;
		constBufferDesc.StructureByteStride = 0;

		p_device->CreateBuffer(&constBufferDesc, NULL, p_constBuffer.GetAddressOf());
	}
	// ���̓��C�A�E�g�I�u�W�F�b�N�g����
	D3D11_INPUT_ELEMENT_DESC input_element_desc[] = {
		{ "POSITION",0, DXGI_FORMAT_R32G32B32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL" , 0, DXGI_FORMAT_R32G32B32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	createVsFromCso(p_device, "geometric_primitive_vs.cso", p_vertexShader.GetAddressOf(), p_inputLayout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	createPsFromCso(p_device, "geometric_primitive_ps.cso", p_pixelShader.GetAddressOf());

	// ���X�^���C�U�[�X�e�[�g(���C���[�t���[��)�̍쐬
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(rsDesc));
	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthClipEnable = TRUE;
	rsDesc.AntialiasedLineEnable = TRUE;
	p_device->CreateRasterizerState(&rsDesc, p_rasterizerState[1].GetAddressOf());


	//	���X�^���C�U�[�X�e�[�g(�\���b�h)�̍쐬
	//D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(rsDesc));
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthClipEnable = TRUE;
	p_device->CreateRasterizerState(&rsDesc, p_rasterizerState[0].GetAddressOf());


	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, 0);   // ������
	depthStencilDesc.DepthEnable = TRUE;       // �[�x�e�X�g
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;  // �[�x��������
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	// stencil�̐ݒ�
	depthStencilDesc.StencilEnable = true; // �O��֌W�������������͂��̎q��false�����ĂȂ��\������

	// �쐬
	p_device->CreateDepthStencilState(&depthStencilDesc, p_depthStancilState.GetAddressOf());

	return true;
}

// �e�N�X�`������悤�� CreateBuffers �ł�
bool GeometricPrimitive::TextureCreateBuffers(ID3D11Device* p_device, vertex* vertices, int numVertex, unsigned int* indices, int numIndex, const wchar_t* filename)
{
	// ���_�o�b�t�@�I�u�W�F�N�g�̐���
	{
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.ByteWidth = sizeof(vertex) * numVertex;
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexSubResource;
		ZeroMemory(&vertexSubResource, sizeof(vertexSubResource));
		vertexSubResource.pSysMem = vertices;
		vertexSubResource.SysMemPitch = 0;
		vertexSubResource.SysMemSlicePitch = 0;
		p_device->CreateBuffer(&vertexBufferDesc, &vertexSubResource, p_vertexBuffer.GetAddressOf());
	}
	if (indices) {
		// �C���f�b�N�X�o�b�t�@�I�u�W�F�N�g�̐���
		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
		indexBufferDesc.ByteWidth = numIndex * sizeof(unsigned int);
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA indexSubResource;
		ZeroMemory(&indexSubResource, sizeof(indexSubResource));
		indexSubResource.pSysMem = indices;
		indexSubResource.SysMemPitch = 0;
		indexSubResource.SysMemSlicePitch = 0;

		numIndices = numIndex;
		p_device->CreateBuffer(&indexBufferDesc, &indexSubResource, p_indexBuffer.GetAddressOf());
	}
	{
		// �萔�o�b�t�@�I�u�W�F�N�g�̐���
		D3D11_BUFFER_DESC constBufferDesc;
		ZeroMemory(&constBufferDesc, sizeof(constBufferDesc));
		constBufferDesc.ByteWidth = sizeof(cbuffer);
		constBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constBufferDesc.CPUAccessFlags = 0;
		constBufferDesc.MiscFlags = 0;
		constBufferDesc.StructureByteStride = 0;

		p_device->CreateBuffer(&constBufferDesc, NULL, p_constBuffer.GetAddressOf());
	}
	// ���̓��C�A�E�g�I�u�W�F�b�N�g����
	D3D11_INPUT_ELEMENT_DESC input_element_desc[] = {
		{ "POSITION",0, DXGI_FORMAT_R32G32B32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL" , 0, DXGI_FORMAT_R32G32B32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	createVsFromCso(p_device, "not_light_vs.cso", p_vertexShader.GetAddressOf(), p_inputLayout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	createPsFromCso(p_device, "not_light_ps.cso", p_pixelShader.GetAddressOf());

	// ���X�^���C�U�[�X�e�[�g(���C���[�t���[��)�̍쐬
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(rsDesc));
	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthClipEnable = TRUE;
	rsDesc.AntialiasedLineEnable = TRUE;
	p_device->CreateRasterizerState(&rsDesc, p_rasterizerState[1].GetAddressOf());


	//	���X�^���C�U�[�X�e�[�g(�\���b�h)�̍쐬
	//D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(rsDesc));
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthClipEnable = TRUE;
	p_device->CreateRasterizerState(&rsDesc, p_rasterizerState[0].GetAddressOf());


	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, 0);   // ������
	depthStencilDesc.DepthEnable = TRUE;       // �[�x�e�X�g
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;  // �[�x��������
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	// stencil�̐ݒ�
	depthStencilDesc.StencilEnable = true; // �O��֌W�������������͂��̎q��false�����ĂȂ��\������

	// �쐬
	p_device->CreateDepthStencilState(&depthStencilDesc, p_depthStancilState.GetAddressOf());

	// create constant buffer
	D3D11_BUFFER_DESC buffer_desc = {};
	buffer_desc.ByteWidth = sizeof(cbuffer);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	p_device->CreateBuffer(&buffer_desc, nullptr, &p_constBuffer);

	// create samplerState
	D3D11_SAMPLER_DESC sampler_desc = {};
	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MipLODBias = 0;
	sampler_desc.MaxAnisotropy = 16;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	memcpy(sampler_desc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	p_device->CreateSamplerState(&sampler_desc, &pSamplerState);

	LoadTextureFromFile(p_device, filename, texture.data.GetAddressOf(), &texture.desc);

	return true;
}

//****************************************************************************************************************

GeometricBoard::GeometricBoard(ID3D11Device* p_device, const wchar_t* filename) // �|���S���p
{
	const static DirectX::XMFLOAT2 UV[4] =
	{
		{0.0f,0.0f},
		{1.0f,0.0f},
		{0.0f,1.0f},
		{1.0f,1.0f}
	};

	vertex vertices[4] = {};
	unsigned int indices[3 * 2] = {};

	vertices[0].pos = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.0f);
	vertices[1].pos = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.0f);
	vertices[2].pos = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.0f);
	vertices[3].pos = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.0f);
	vertices[0].normal = DirectX::XMFLOAT3(+0.0f, 0.0f, -1.0f);
	vertices[1].normal = DirectX::XMFLOAT3(+0.0f, 0.0f, -1.0f);
	vertices[2].normal = DirectX::XMFLOAT3(+0.0f, 0.0f, -1.0f);
	vertices[3].normal = DirectX::XMFLOAT3(+0.0f, 0.0f, -1.0f);
	vertices[0].texcoord = UV[0];
	vertices[1].texcoord = UV[1];
	vertices[2].texcoord = UV[2];
	vertices[3].texcoord = UV[3];

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 1;
	indices[4] = 3;
	indices[5] = 2;

	TextureCreateBuffers(p_device, vertices, ARRAYSIZE(vertices), indices, ARRAYSIZE(indices), filename);

}

GeometricRect::GeometricRect(ID3D11Device* p_device)
{

	vertex vertices[4] = {};
	unsigned int indices[3 * 2] = {};

	vertices[0].pos = DirectX::XMFLOAT3(-0.5f, .0f, +0.5f);
	vertices[1].pos = DirectX::XMFLOAT3(+0.5f, .0f, +0.5f);
	vertices[2].pos = DirectX::XMFLOAT3(-0.5f, .0f, -0.5f);
	vertices[3].pos = DirectX::XMFLOAT3(+0.5f, .0f, -0.5f);
	vertices[0].normal = vertices[1].normal =
		vertices[2].normal =
		vertices[3].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
	indices[0] = 0;	indices[1] = 1;	indices[2] = 2;
	indices[3] = 1;	indices[4] = 3;	indices[5] = 2;

	CreateBuffers(p_device, vertices, ARRAYSIZE(vertices), indices, ARRAYSIZE(indices));
}

GeometricRect::GeometricRect(ID3D11Device* p_device, const wchar_t* filename)
{
	const static DirectX::XMFLOAT2 UV[4] =
	{
		{0.0f,0.0f},
		{1.0f,0.0f},
		{0.0f,1.0f},
		{1.0f,1.0f}
	};

	vertex vertices[4] = {};
	unsigned int indices[3 * 2] = {};

	vertices[0].pos = DirectX::XMFLOAT3(-0.5f, 0.0f, +0.5f);
	vertices[1].pos = DirectX::XMFLOAT3(+0.5f, 0.0f, +0.5f);
	vertices[2].pos = DirectX::XMFLOAT3(-0.5f, 0.0f, -0.5f);
	vertices[3].pos = DirectX::XMFLOAT3(+0.5f, 0.0f, -0.5f);
	vertices[0].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, 0.0f);
	vertices[1].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, 0.0f);
	vertices[2].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, 0.0f);
	vertices[3].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, 0.0f);
	vertices[0].texcoord = UV[0];
	vertices[1].texcoord = UV[1];
	vertices[2].texcoord = UV[2];
	vertices[3].texcoord = UV[3];

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 1;
	indices[4] = 3;
	indices[5] = 2;

	TextureCreateBuffers(p_device, vertices, ARRAYSIZE(vertices), indices, ARRAYSIZE(indices), filename);
}

GeometricCube::GeometricCube(ID3D11Device* p_device)
{
	vertex vertices[4 * 6] = {}; // ���_��
	unsigned int indices[3 * 2 * 6] = {};

	int numVertex = 0, numIndex = 0;

	//	���
	vertices[numVertex + 0].pos = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
	vertices[numVertex + 1].pos = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
	vertices[numVertex + 2].pos = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
	vertices[numVertex + 3].pos = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
	vertices[numVertex + 0].normal = vertices[numVertex + 1].normal =
		vertices[numVertex + 2].normal =
		vertices[numVertex + 3].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
	indices[numIndex + 0] = numVertex + 0;	indices[numIndex + 1] = numVertex + 1;	indices[numIndex + 2] = numVertex + 2;
	indices[numIndex + 3] = numVertex + 1;	indices[numIndex + 4] = numVertex + 3;	indices[numIndex + 5] = numVertex + 2;
	numVertex += 4;	numIndex += 6;

	//	����
	vertices[numVertex + 0].pos = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
	vertices[numVertex + 1].pos = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
	vertices[numVertex + 2].pos = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
	vertices[numVertex + 3].pos = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
	vertices[numVertex + 0].normal = vertices[numVertex + 1].normal =
		vertices[numVertex + 2].normal =
		vertices[numVertex + 3].normal = DirectX::XMFLOAT3(+0.0f, -1.0f, +0.0f);
	indices[numIndex + 0] = numVertex + 0;	indices[numIndex + 1] = numVertex + 2;	indices[numIndex + 2] = numVertex + 1;
	indices[numIndex + 3] = numVertex + 1;	indices[numIndex + 4] = numVertex + 2;	indices[numIndex + 5] = numVertex + 3;
	numVertex += 4;	numIndex += 6;

	//	�E��
	vertices[numVertex + 0].pos = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
	vertices[numVertex + 1].pos = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
	vertices[numVertex + 2].pos = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
	vertices[numVertex + 3].pos = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
	vertices[numVertex + 0].normal = vertices[numVertex + 1].normal =
		vertices[numVertex + 2].normal =
		vertices[numVertex + 3].normal = DirectX::XMFLOAT3(+1.0f, +0.0f, +0.0f);
	indices[numIndex + 0] = numVertex + 0;	indices[numIndex + 1] = numVertex + 1;	indices[numIndex + 2] = numVertex + 2;
	indices[numIndex + 3] = numVertex + 1;	indices[numIndex + 4] = numVertex + 3;	indices[numIndex + 5] = numVertex + 2;
	numVertex += 4;	numIndex += 6;

	//	����
	vertices[numVertex + 0].pos = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
	vertices[numVertex + 1].pos = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
	vertices[numVertex + 2].pos = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
	vertices[numVertex + 3].pos = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
	vertices[numVertex + 0].normal = vertices[numVertex + 1].normal =
		vertices[numVertex + 2].normal =
		vertices[numVertex + 3].normal = DirectX::XMFLOAT3(-1.0f, +0.0f, +0.0f);
	indices[numIndex + 0] = numVertex + 0;	indices[numIndex + 1] = numVertex + 2;	indices[numIndex + 2] = numVertex + 1;
	indices[numIndex + 3] = numVertex + 1;	indices[numIndex + 4] = numVertex + 2;	indices[numIndex + 5] = numVertex + 3;
	numVertex += 4;	numIndex += 6;

	//	���
	vertices[numVertex + 0].pos = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
	vertices[numVertex + 1].pos = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
	vertices[numVertex + 2].pos = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
	vertices[numVertex + 3].pos = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
	vertices[numVertex + 0].normal = vertices[numVertex + 1].normal =
		vertices[numVertex + 2].normal =
		vertices[numVertex + 3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, +1.0f);
	indices[numIndex + 0] = numVertex + 0;	indices[numIndex + 1] = numVertex + 1;	indices[numIndex + 2] = numVertex + 2;
	indices[numIndex + 3] = numVertex + 1;	indices[numIndex + 4] = numVertex + 3;	indices[numIndex + 5] = numVertex + 2;
	numVertex += 4;	numIndex += 6;

	//	�O��
	vertices[numVertex + 0].pos = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
	vertices[numVertex + 1].pos = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
	vertices[numVertex + 2].pos = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
	vertices[numVertex + 3].pos = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
	vertices[numVertex + 0].normal = vertices[numVertex + 1].normal =
		vertices[numVertex + 2].normal =
		vertices[numVertex + 3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, -1.0f);
	indices[numIndex + 0] = numVertex + 0;	indices[numIndex + 1] = numVertex + 2;	indices[numIndex + 2] = numVertex + 1;
	indices[numIndex + 3] = numVertex + 1;	indices[numIndex + 4] = numVertex + 2;	indices[numIndex + 5] = numVertex + 3;
	numVertex += 4;	numIndex += 6;

	CreateBuffers(p_device, vertices, numVertex, indices, numIndex);
}

GeometricCylinder::GeometricCylinder(ID3D11Device* p_device)
{
}

GeometricSphere::GeometricSphere(ID3D11Device* p_device, u_int slices, u_int stacks)
{
	std::vector<vertex> vertices;   // �o�[�e�b�N�X�f�[�^�Ǘ�    
	std::vector<u_int> indices;     // �C���f�b�N�X���Ǘ�

	float radius = 0.5f; // ���a

	// Compute the vertices stating at the top pole and moving down the stacks.
	// ���_�̈�ԏ���N�_�Ƃ��ăX�^�b�N�����Ɉړ����钸�_�v�Z

	// Poles: note that there will be texture coordinate distortion as there is
	// not a unique point on the texture map to assign to the pole when mapping
	// a rectangular texture onto a sphere.

	// �ɓ_:�����`�̃e�N�X�`�������̂Ƀ}�b�s���O����Ƃ��ɓ_�Ɋ��蓖�Ă�e�N�X�`���}�b�v���  
	// ���m�ȓ_���Ȃ����߁A�e�N�X�`���[���W�̘c�݂����邱�Ƃ�����
	vertex topVertex;
	topVertex.pos = DirectX::XMFLOAT3(0.0f, +radius, 0.0f);
	topVertex.normal = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);

	vertex bottomVertex;
	bottomVertex.pos = DirectX::XMFLOAT3(0.0f, -radius, 0.0f);
	bottomVertex.normal = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);

	vertices.push_back(topVertex); //vertices�̔z���1,2,3�E�E�E�ƃf�[�^�����Ă���

	float phiStep = DirectX::XM_PI / stacks; // �t�@�C�v�Z �ӌv�Z
	float thetaStep = 2.0f * DirectX::XM_PI / slices;  // �V�[�^�v�Z�@�ƌv�Z

	// Compute vertices for each stack ring(do not count the poles as rings).
	// �e�X�^�b�N�����O�̒��_���v�Z����(�ɓ_�������O�Ƃ��Đ����Ȃ��l��)
	for (u_int i = 1; i <= stacks - 1; i++)
	{
		float phi = i * phiStep;
		float rsinPhi = radius * sinf(phi);
		float rcosPhi = radius * cosf(phi);

		// Vertices of ring.
		// �����O�̒��_�v�Z
		for (u_int j = 0; j <= slices; j++)
		{
			float theta = j * thetaStep;

			vertex v;

			// spherical to cartesian
			// ������f�J���g��
			v.pos.x = rsinPhi * cosf(theta);
			v.pos.y = rcosPhi;
			v.pos.z = rsinPhi * sinf(theta);

			DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&v.pos);
			DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));

			vertices.push_back(v);

		}
	}

	vertices.push_back(bottomVertex);

	// Compute indices for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring.
	//�g�b�v�X�X�^�b�N�̃C���f�b�N�X���v�Z���܂��B
	// ��ԏ�̃X�^�b�N�͍ŏ��ɒ��_�o�b�t�@�ɏ������܂�A��ԏ�̋ɓ_���ŏ��̃����O�ɂȂ���

	for (UINT i = 1; i <= slices; ++i)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}

	// Compute indices for inner stacks (not connected to poles).
	// �����X�^�b�N�̃C���f�b�N�X���v�Z����(�ɓ_�ɂȂ����Ă��Ȃ�)


	// Offset the indices to the index of the first vertex in the first ring.
	// This is just skipping the top pole vertex.
	// �C���f�b�N�X���ŏ��̃����O�̍ŏ��̒��_�̃C���f�b�N�X�ɃI�t�Z�b�g����
	// ����́A�㕔�ɓ_�̒��_���X�L�b�v���Ă��邾���B
	u_int base_index = 1;
	u_int ringVertexCount = slices + 1;
	for (u_int i = 0; i < stacks - 2; ++i)
	{
		u_int i_rvc = i * ringVertexCount;
		u_int i1_rvc = (i + 1) * ringVertexCount;

		for (u_int j = 0; j < slices; ++j)
		{
			indices.push_back(base_index + i_rvc + j);
			indices.push_back(base_index + i_rvc + j + 1);
			indices.push_back(base_index + i1_rvc + j);

			indices.push_back(base_index + i1_rvc + j);
			indices.push_back(base_index + i_rvc + j + 1);
			indices.push_back(base_index + i1_rvc + j + 1);
		}
	}

	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring.
	// �{�g���X�^�b�N�̃C���f�b�N�X���v�Z���܂��B 
	// �����X�^�b�N�͍Ō�ɒ��_�o�b�t�@�[�ɏ������܂�A�����ɂ����������O�ɐڑ����܂��B


	// South pole vertex was added last.
	//	��ɒ��_���Ō�ɒǉ�����܂����B��ԉ��̒��_
	u_int southPoleIndex = (u_int)vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	// �C���f�b�N�X���Ō�̃����O�̍ŏ��̒��_�̃C���f�b�N�X�ɃI�t�Z�b�g���܂��B
	base_index = southPoleIndex - ringVertexCount;

	for (u_int i = 0; i < slices; ++i)
	{
		indices.push_back(southPoleIndex);
		indices.push_back(base_index + i);
		indices.push_back(base_index + i + 1);
	}

	CreateBuffers(p_device, vertices.data(), vertices.size(), indices.data(), indices.size());

}

GeometricCapsule::GeometricCapsule(ID3D11Device* p_device)
{
}

void GeometricPrimitive::render(ID3D11DeviceContext* p_context, const DirectX::XMFLOAT4X4& wvp, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color, bool wireframe)
{
	// �i�[���邽�߂̃o�[�e�b�N�X�̒��g������������
	UINT stride = sizeof(vertex);        // �i�[���鑤�̏�����
	UINT offset = 0;                     // �ǂݍ��݂̈ʒu

	// ���_�o�b�t�@�̃o�C���h
	p_context->IASetVertexBuffers(0, 1, p_vertexBuffer.GetAddressOf(), &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃o�C���h
	p_context->IASetIndexBuffer(p_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// �萔�o�b�t�@�̃o�C���h
	//D3D11_MAPPED_SUBRESOURCE pdata;
	cbuffer constBuffer;
	constBuffer.world_view_projection = wvp;
	constBuffer.world = world;
	constBuffer.light_direction = light_direction;
	constBuffer.material_color = material_color;

	p_context->UpdateSubresource(p_constBuffer.Get(), 0, nullptr, &constBuffer, 0, 0);

	p_context->VSSetConstantBuffers(0, 1, p_constBuffer.GetAddressOf());

	p_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���̓��C�A�E�g�̃o�C���h
	p_context->IASetInputLayout(p_inputLayout.Get());

	// ���X�^���C�U�[�̐ݒ�
	if (wireframe)
	{
		p_context->RSSetState(p_rasterizerState[1].Get());
	}
	else p_context->RSSetState(p_rasterizerState[0].Get());

	// �V�F�[�_�[�̐ݒ�
	p_context->VSSetShader(p_vertexShader.Get(), nullptr, 0);
	p_context->PSSetShader(p_pixelShader.Get(), nullptr, 0);

	// �[�x�e�X�g�̐ݒ�
	p_context->OMSetDepthStencilState(p_depthStancilState.Get(), 0);


	p_context->DrawIndexed(numIndices, 0, 0);
}

void GeometricPrimitive::render(ID3D11DeviceContext* p_context, const DirectX::XMFLOAT4X4& wvp, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color, bool wireframe)
{
	// �i�[���邽�߂̃o�[�e�b�N�X�̒��g������������
	UINT stride = sizeof(vertex);        // �i�[���鑤�̏�����
	UINT offset = 0;                     // �ǂݍ��݂̈ʒu

	// ���_�o�b�t�@�̃o�C���h
 	p_context->IASetVertexBuffers(0, 1, p_vertexBuffer.GetAddressOf(), &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃o�C���h
	p_context->IASetIndexBuffer(p_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// �萔�o�b�t�@�̃o�C���h
	//D3D11_MAPPED_SUBRESOURCE pdata;
	cbuffer constBuffer;
	constBuffer.world_view_projection = wvp;
	constBuffer.world = world;
	// not_light ���g���̂� light �͕s�v
	constBuffer.light_direction = {};
	constBuffer.material_color = material_color;

	p_context->UpdateSubresource(p_constBuffer.Get(), 0, nullptr, &constBuffer, 0, 0);

	p_context->VSSetConstantBuffers(0, 1, p_constBuffer.GetAddressOf());

	

	p_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���̓��C�A�E�g�̃o�C���h
	p_context->IASetInputLayout(p_inputLayout.Get());

	// ���X�^���C�U�[�̐ݒ�
	if (wireframe)
	{
		p_context->RSSetState(p_rasterizerState[1].Get());
	}
	else p_context->RSSetState(p_rasterizerState[0].Get());

	// �V�F�[�_�[�̐ݒ�
	p_context->VSSetShader(p_vertexShader.Get(), nullptr, 0);
	p_context->PSSetShader(p_pixelShader.Get(), nullptr, 0);

	// �[�x�e�X�g�̐ݒ�
	p_context->OMSetDepthStencilState(p_depthStancilState.Get(), 0);

	p_context->PSSetSamplers(0, 1, pSamplerState.GetAddressOf());
	p_context->PSSetShaderResources(0, 1, texture.data.GetAddressOf());

	p_context->DrawIndexed(numIndices, 0, 0);
}
