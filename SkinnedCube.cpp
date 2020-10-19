#include "SkinnedCube.h"

void SkinnedCube::CreateBuffers(ID3D11Device * device, UINT num, Vertex * vertices, UINT numV, UINT * indices, UINT numI)
{
	HRESULT hr;
	D3D11_BUFFER_DESC buffer_desc;
	D3D11_SUBRESOURCE_DATA subresource_data;

	//	Vertex Buffer
	ZeroMemory(&buffer_desc, sizeof(buffer_desc));
	ZeroMemory(&subresource_data, sizeof(subresource_data));

	buffer_desc.ByteWidth = numV * sizeof(Vertex);
	buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;

	device->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer[num]);
	//	Index Buffer
	if (indices)
	{
		ZeroMemory(&buffer_desc, sizeof(buffer_desc));
		ZeroMemory(&subresource_data, sizeof(subresource_data));

		buffer_desc.ByteWidth = numI * sizeof(unsigned int);
		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		subresource_data.pSysMem = indices;
		subresource_data.SysMemPitch = 0;
		subresource_data.SysMemSlicePitch = 0;
		device->CreateBuffer(&buffer_desc, &subresource_data, &index_buffer[num]);
	}

	//	Constant Buffer
	ZeroMemory(&buffer_desc, sizeof(buffer_desc));

	buffer_desc.ByteWidth = sizeof(Cbuffer);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	device->CreateBuffer(&buffer_desc, nullptr, &constant_buffer);
}


/*追加のインクルードディレクトリに追加されてないなら相対パスに変更*/
#include <WICTextureLoader.h>

void SkinnedCube::LoadTexture(ID3D11Device * device,FileName texture)
{
	this->texture.push_back(Texture());
	UINT num = this->texture.size() - 1;
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	DirectX::CreateWICTextureFromFile(device, texture, 
		resource.GetAddressOf(),this->texture.at(num).data.GetAddressOf());
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
	texture2d->GetDesc(&this->texture.at(num).desc);

}

SkinnedCube::SkinnedCube(ID3D11Device * device, FileName texture[], UINT size)
{
	HRESULT hr = S_OK;

	{
		FILE* fp = nullptr;
		//シェーダーのファイルのアドレスは好きに変更して
		fopen_s(&fp, "./Shaders/cube_vs.cso", "rb");
		_ASSERT_EXPR(fp, "CSO File not found");
		fseek(fp, 0, SEEK_END);
		long cso_sz = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		unsigned char* cso_data = new unsigned char[cso_sz];
		fread(cso_data, cso_sz, 1, fp);
		fclose(fp);

		HRESULT hr = device->CreateVertexShader(
			cso_data,
			cso_sz,
			nullptr,
			&vertex_shader
		);
		

		D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// 入力レイアウトの生成
		hr = device->CreateInputLayout(
			input_element_desc,			
			ARRAYSIZE(input_element_desc),
			cso_data,					
			cso_sz,						
			input_layout.GetAddressOf()	
		);

		delete[] cso_data;
	}
	{
		FILE* fp = nullptr;
		//シェーダーのファイルのアドレスは好きに変更して
		fopen_s(&fp, "./Shaders/cube_ps.cso", "rb");
		fseek(fp, 0, SEEK_END);
		long cso_sz = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		unsigned char* cso_data = new unsigned char[cso_sz];
		fread(cso_data, cso_sz, 1, fp);
		fclose(fp);	

		HRESULT hr = device->CreatePixelShader(
			cso_data,
			cso_sz,	
			nullptr,
			pixel_shader.GetAddressOf()
		);
		
		delete[] cso_data;
	}

	// create rasterizer state : solid mode
	{
		D3D11_RASTERIZER_DESC rasterizer_desc = {};
		rasterizer_desc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
		rasterizer_desc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
		rasterizer_desc.FrontCounterClockwise = FALSE;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0;
		rasterizer_desc.SlopeScaledDepthBias = 0;
		rasterizer_desc.DepthClipEnable = TRUE;
		rasterizer_desc.ScissorEnable = FALSE;
		rasterizer_desc.MultisampleEnable = FALSE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizer_desc, &rasterizer_state);
	}
	
	// create depth stencil state
	{
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
		depth_stencil_desc.DepthEnable = TRUE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
		depth_stencil_desc.StencilEnable = FALSE;
		depth_stencil_desc.StencilReadMask = 0xFF;
		depth_stencil_desc.StencilWriteMask = 0xFF;
		depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state);
	}
	// create constant buffer
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.ByteWidth = sizeof(Cbuffer);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&buffer_desc, nullptr, &constant_buffer);
	}
	{
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
		hr = device->CreateSamplerState(&sampler_desc, &sampler_state);
	}
	static XMFLOAT2 UV[4U] = 
	{ 
		{0.0f,0.0f },
		{1.0f,0.0f },
		{0.0f,1.0f },
		{1.0f,1.0f } 
	};

	{
		Vertex vertices[4U] = {};
		UINT indices[6U] = {};
		{
			vertices[0U].position = XMFLOAT3(-0.5f, +0.5f, +0.5f);
			vertices[1U].position = XMFLOAT3(+0.5f, +0.5f, +0.5f);
			vertices[2U].position = XMFLOAT3(-0.5f, +0.5f, -0.5f);
			vertices[3U].position = XMFLOAT3(+0.5f, +0.5f, -0.5f);
			vertices[0U].normal = XMFLOAT3(+0.0f, +1.0f, +0.0f);
			vertices[1U].normal = XMFLOAT3(+0.0f, +1.0f, +0.0f);
			vertices[2U].normal = XMFLOAT3(+0.0f, +1.0f, +0.0f);
			vertices[3U].normal = XMFLOAT3(+0.0f, +1.0f, +0.0f);
			vertices[0U].texcoord = UV[0U];
			vertices[1U].texcoord = UV[1U];
			vertices[2U].texcoord = UV[2U];
			vertices[3U].texcoord = UV[3U];
		}
		indices[0U] = 0U;
		indices[1U] = 1U;
		indices[2U] = 2U;
		indices[3U] = 1U;
		indices[4U] = 3U;
		indices[5U] = 2U;
		this->CreateBuffers(device, 0U, vertices, 4U, indices, 6U);
	}

	{
		Vertex vertices[20U] = {};
		UINT indices[30U] = {};
		int face = 0;
		// bottom-side
		// 0---------1
		// |         |
		// |   -Y    |
		// |         |
		// 2---------3
		vertices[face * 4 + 0].position = XMFLOAT3(-0.5f, -0.5f, +0.5f);
		vertices[face * 4 + 1].position = XMFLOAT3(+0.5f, -0.5f, +0.5f);
		vertices[face * 4 + 2].position = XMFLOAT3(-0.5f, -0.5f, -0.5f);
		vertices[face * 4 + 3].position = XMFLOAT3(+0.5f, -0.5f, -0.5f);
		vertices[face * 4 + 0].normal = XMFLOAT3(+0.0f, -1.0f, +0.0f);
		vertices[face * 4 + 1].normal = XMFLOAT3(+0.0f, -1.0f, +0.0f);
		vertices[face * 4 + 2].normal = XMFLOAT3(+0.0f, -1.0f, +0.0f);
		vertices[face * 4 + 3].normal = XMFLOAT3(+0.0f, -1.0f, +0.0f);
		vertices[face * 4 + 0].texcoord = UV[1U];
		vertices[face * 4 + 1].texcoord = UV[0U];
		vertices[face * 4 + 2].texcoord = UV[3U];
		vertices[face * 4 + 3].texcoord = UV[2U];
		indices[face * 6 + 0] = face * 4 + 0;
		indices[face * 6 + 1] = face * 4 + 2;
		indices[face * 6 + 2] = face * 4 + 1;
		indices[face * 6 + 3] = face * 4 + 1;
		indices[face * 6 + 4] = face * 4 + 2;
		indices[face * 6 + 5] = face * 4 + 3;

		// front-side
		// 0---------1
		// |         |
		// |   +Z    |
		// |         |
		// 2---------3
		face += 1;
		vertices[face * 4 + 0].position = XMFLOAT3(-0.5f, +0.5f, -0.5f);
		vertices[face * 4 + 1].position = XMFLOAT3(+0.5f, +0.5f, -0.5f);
		vertices[face * 4 + 2].position = XMFLOAT3(-0.5f, -0.5f, -0.5f);
		vertices[face * 4 + 3].position = XMFLOAT3(+0.5f, -0.5f, -0.5f);
		vertices[face * 4 + 0].normal = XMFLOAT3(+0.0f, +0.0f, -1.0f);
		vertices[face * 4 + 1].normal = XMFLOAT3(+0.0f, +0.0f, -1.0f);
		vertices[face * 4 + 2].normal = XMFLOAT3(+0.0f, +0.0f, -1.0f);
		vertices[face * 4 + 3].normal = XMFLOAT3(+0.0f, +0.0f, -1.0f);
		vertices[face * 4 + 0].texcoord = UV[0U];
		vertices[face * 4 + 1].texcoord = UV[1U];
		vertices[face * 4 + 2].texcoord = UV[2U];
		vertices[face * 4 + 3].texcoord = UV[3U];
		indices[face * 6 + 0] = face * 4 + 0;
		indices[face * 6 + 1] = face * 4 + 1;
		indices[face * 6 + 2] = face * 4 + 2;
		indices[face * 6 + 3] = face * 4 + 1;
		indices[face * 6 + 4] = face * 4 + 3;
		indices[face * 6 + 5] = face * 4 + 2;

		// back-side
		// 0---------1
		// |         |
		// |   +Z    |
		// |         |
		// 2---------3
		face += 1;
		vertices[face * 4 + 0].position = XMFLOAT3(-0.5f, +0.5f, +0.5f);
		vertices[face * 4 + 1].position = XMFLOAT3(+0.5f, +0.5f, +0.5f);
		vertices[face * 4 + 2].position = XMFLOAT3(-0.5f, -0.5f, +0.5f);
		vertices[face * 4 + 3].position = XMFLOAT3(+0.5f, -0.5f, +0.5f);
		vertices[face * 4 + 0].normal = XMFLOAT3(+0.0f, +0.0f, +1.0f);
		vertices[face * 4 + 1].normal = XMFLOAT3(+0.0f, +0.0f, +1.0f);
		vertices[face * 4 + 2].normal = XMFLOAT3(+0.0f, +0.0f, +1.0f);
		vertices[face * 4 + 3].normal = XMFLOAT3(+0.0f, +0.0f, +1.0f);
		vertices[face * 4 + 0].texcoord = UV[1U];
		vertices[face * 4 + 1].texcoord = UV[0U];
		vertices[face * 4 + 2].texcoord = UV[3U];
		vertices[face * 4 + 3].texcoord = UV[2U];
		indices[face * 6 + 0] = face * 4 + 0;
		indices[face * 6 + 1] = face * 4 + 2;
		indices[face * 6 + 2] = face * 4 + 1;
		indices[face * 6 + 3] = face * 4 + 1;
		indices[face * 6 + 4] = face * 4 + 2;
		indices[face * 6 + 5] = face * 4 + 3;

		// right-side
		// 0---------1
		// |         |      
		// |   -X    |
		// |         |
		// 2---------3
		face += 1;
		vertices[face * 4 + 0].position = XMFLOAT3(+0.5f, +0.5f, -0.5f);
		vertices[face * 4 + 1].position = XMFLOAT3(+0.5f, +0.5f, +0.5f);
		vertices[face * 4 + 2].position = XMFLOAT3(+0.5f, -0.5f, -0.5f);
		vertices[face * 4 + 3].position = XMFLOAT3(+0.5f, -0.5f, +0.5f);
		vertices[face * 4 + 0].normal = XMFLOAT3(+1.0f, +0.0f, +0.0f);
		vertices[face * 4 + 1].normal = XMFLOAT3(+1.0f, +0.0f, +0.0f);
		vertices[face * 4 + 2].normal = XMFLOAT3(+1.0f, +0.0f, +0.0f);
		vertices[face * 4 + 3].normal = XMFLOAT3(+1.0f, +0.0f, +0.0f);
		vertices[face * 4 + 0].texcoord = UV[0U];
		vertices[face * 4 + 1].texcoord = UV[1U];
		vertices[face * 4 + 2].texcoord = UV[2U];
		vertices[face * 4 + 3].texcoord = UV[3U];
		indices[face * 6 + 0] = face * 4 + 0;
		indices[face * 6 + 1] = face * 4 + 1;
		indices[face * 6 + 2] = face * 4 + 2;
		indices[face * 6 + 3] = face * 4 + 1;
		indices[face * 6 + 4] = face * 4 + 3;
		indices[face * 6 + 5] = face * 4 + 2;

		// left-side
		// 0---------1
		// |         |      
		// |   -X    |
		// |         |
		// 2---------3
		face += 1;
		vertices[face * 4 + 0].position = XMFLOAT3(-0.5f, +0.5f, -0.5f);
		vertices[face * 4 + 1].position = XMFLOAT3(-0.5f, +0.5f, +0.5f);
		vertices[face * 4 + 2].position = XMFLOAT3(-0.5f, -0.5f, -0.5f);
		vertices[face * 4 + 3].position = XMFLOAT3(-0.5f, -0.5f, +0.5f);
		vertices[face * 4 + 0].normal = XMFLOAT3(-1.0f, +0.0f, +0.0f);
		vertices[face * 4 + 1].normal = XMFLOAT3(-1.0f, +0.0f, +0.0f);
		vertices[face * 4 + 2].normal = XMFLOAT3(-1.0f, +0.0f, +0.0f);
		vertices[face * 4 + 3].normal = XMFLOAT3(-1.0f, +0.0f, +0.0f);
		vertices[face * 4 + 0].texcoord = UV[1U];
		vertices[face * 4 + 1].texcoord = UV[0U];
		vertices[face * 4 + 2].texcoord = UV[3U];
		vertices[face * 4 + 3].texcoord = UV[2U];
		indices[face * 6 + 0] = face * 4 + 0;
		indices[face * 6 + 1] = face * 4 + 2;
		indices[face * 6 + 2] = face * 4 + 1;
		indices[face * 6 + 3] = face * 4 + 1;
		indices[face * 6 + 4] = face * 4 + 2;
		indices[face * 6 + 5] = face * 4 + 3;

		CreateBuffers(device,1U, vertices, 20U, indices, 30U);
	}

	for (UINT i = 0; i < size; i++)
	{
		LoadTexture(device, texture[i]);
	}
}

SkinnedCube::~SkinnedCube()
{
	if (constant_buffer)constant_buffer->Release();
	if (depth_stencil_state)depth_stencil_state->Release();
	if (vertex_buffer[0])vertex_buffer[0]->Release();
	if (index_buffer[0])index_buffer[0]->Release();
	if (vertex_buffer[1])vertex_buffer[1]->Release();
	if (index_buffer[1])index_buffer[1]->Release();
	if (sampler_state)sampler_state->Release();
}
void SkinnedCube::Render(ID3D11DeviceContext *context, UINT num,
	const XMFLOAT4X4 & wvp,
	const XMFLOAT4X4 & world,
	const XMFLOAT4 & material_color)
{
	if (num >= texture.size())num = 0;
	Cbuffer cb;
	cb.wvp = wvp;
	cb.world = world;
	//光は無効化しているので設定しなくてよい
	cb.light_direction = {};
	cb.material_color = material_color;
	context->UpdateSubresource(constant_buffer, 0, nullptr, &cb, 0, 0);
	context->VSSetConstantBuffers(0, 1, &constant_buffer);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(input_layout.Get());
	context->RSSetState(rasterizer_state);
	context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	context->PSSetShader(pixel_shader.Get(), nullptr, 0);
	context->OMSetDepthStencilState(depth_stencil_state, 0);
	context->PSSetSamplers(0, 1, &sampler_state);
	{
		context->IASetVertexBuffers(0, 1, &vertex_buffer[0U], &stride, &offset);
		//	インデックスバッファのバインド
		context->IASetIndexBuffer(index_buffer[0U], DXGI_FORMAT_R32_UINT, 0);
		context->PSSetShaderResources(0, 1, texture.at(num).data.GetAddressOf());
		D3D11_BUFFER_DESC buffer_desc;
		index_buffer[0U]->GetDesc(&buffer_desc);
		context->DrawIndexed(buffer_desc.ByteWidth / sizeof(u_int), 0, 0);
	}
	{
		context->IASetVertexBuffers(0, 1, &vertex_buffer[1U], &stride, &offset);
		//	インデックスバッファのバインド
		context->IASetIndexBuffer(index_buffer[1U], DXGI_FORMAT_R32_UINT, 0);
		context->PSSetShaderResources(0, 1, texture.at(0).data.GetAddressOf());
		D3D11_BUFFER_DESC buffer_desc;
		index_buffer[1U]->GetDesc(&buffer_desc);
		context->DrawIndexed(buffer_desc.ByteWidth / sizeof(u_int), 0, 0);
	}
}
