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


	//  頂点バッファーの実体宣言
	D3D11_BUFFER_DESC      buffrdesc = {};

	// 生成以上の事をする場合は
	// 生成する前に初期化をしておくこと
	// 今回は生成のみなので初期化を省いている
	// 実体のポインターなのですべてに&を付けているよ
	// 初期化の参考はframework.cppのバッファーなどを参考
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


	// 入力レイアウトオブジェクトの生成
	// 頂点shaderのデータが欲しいため
	// ピクセルシェーダーオブジェクトの前にする
	//D3D11_INPUT_ELEMENT_DESC    input_elementdesc;
	// 頂点レイアウト
	D3D11_INPUT_ELEMENT_DESC input_elementdesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA ,0 },
		{ "TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA ,0 },
	};

	//　頂点シェーダーオブジェクト生成するための
	//  CSOファイルの読み込みをする
	//  sprite_vs.csoのロード
	FILE* fp = 0;
	fopen_s(&fp, "./Shaders/sprite_vs.cso", "rb");
	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	unsigned char* cso_data = new unsigned char[cso_sz];
	fread(cso_data, cso_sz, 1, fp);

	// 頂点シェーダーオブジェックト生成
	p_device->CreateVertexShader(cso_data, cso_sz, NULL, &p_vertexshader);

	// 入力レイアウトの作成
	p_device->CreateInputLayout(input_elementdesc, ARRAYSIZE(input_elementdesc), cso_data, cso_sz, &p_Inputlayout); // 第2引数も違うかった 直す前は1だった
	// 第3引数は頂点データが欲しい 

	// ピクセルシェーダーオブジェクトを生成するために
	// CSOファイルを読み込む
	//  sprite_ps.csoのロード
	fp = 0;
	fopen_s(&fp, "./Shaders/sprite_ps.cso", "rb");
	fseek(fp, 0, SEEK_END);
	cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	cso_data = new unsigned char[cso_sz];
	fread(cso_data, cso_sz, 1, fp);


	// ピクセルシェーダーオブジェックト生成
	p_device->CreatePixelShader(cso_data, cso_sz, NULL, &p_pixelshader);


	// ラスタライザーステートの設定
	D3D11_RASTERIZER_DESC   rasterrizerdesc;
	p_device->CreateRasterizerState(&rasterrizerdesc, &p_rasterizerstate);


	// 画像ファイルのロードとシェーダー　リソース　ビュー　オブジェクトの生成
	//ID3D11ShaderResourceView* p_shader_resource_view;  無駄に作成していた(既にsprite.hに宣言している)
	ID3D11Resource* p_resource;

	DirectX::CreateWICTextureFromFile(p_device, p_texturefile, &p_resource, &p_shader_resource_view);

	// テクスチャ情報の取得
	//D3D11_TEXTURE2D_DESC texture2d_desc;
	ID3D11Texture2D* p_texture2D;
	p_resource->QueryInterface(&p_texture2D);
	p_texture2D->GetDesc(&texture2d_desc);

	// サンプラー ステート オブジェクトの生成
	D3D11_SAMPLER_DESC sampler_desc;

	::ZeroMemory(&sampler_desc, sizeof(D3D11_SAMPLER_DESC));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	p_device->CreateSamplerState(&sampler_desc, &p_sampler_state);

	// 深度ステンシルステートオブジェクトの設定
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {}; // 初期化
	//ZeroMemory(&depthStencilDesc, 0); 初期化
	depthStencilDesc.DepthEnable = false;       // 深度テスト
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;  // 深度書き込み
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	// stencilの設定
	depthStencilDesc.StencilEnable = false; // 前後関係がおかしい時はこの子がfalseさせてない可能性あり
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
	// 作成
	p_device->CreateDepthStencilState(&depthStencilDesc, &p_depthStencilState);

	// ブレンディングステートオブジェクトの設定
	//D3D11_BLEND_DESC blendDesc = {};
	//// 作成
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

	// 頂点データの更新テスト
	{
		vertex vertexs[4] = {
		{ DirectX::XMFLOAT3(0.0f, 0.0f, .0f), col,DirectX::XMFLOAT2(_sx,_sy)},
		{ DirectX::XMFLOAT3(1.0f, 0.0f, .0f), col,DirectX::XMFLOAT2(_sx + _sw, _sy)},
		{ DirectX::XMFLOAT3(0.0f, 1.0f, .0f), col,DirectX::XMFLOAT2(_sx,_sy + _sh)},
		{ DirectX::XMFLOAT3(1.0f, 1.0f, .0f), col,DirectX::XMFLOAT2(_sx + _sw, _sy + _sh)},
		};

		//	(dw,dh)の拡大
		for (int i = 0; i < 4; i++) {
			vertexs[i].position.x *= _dw;
			vertexs[i].position.y *= _dh;
		}

		//	回転軸の変更（矩形中央へ）
		for (int i = 0; i < 4; i++) {
			vertexs[i].position.x -= _dw / 2.0f;
			vertexs[i].position.y -= _dh / 2.0f;
		}

		//	angle度の回転
		float degree = DirectX::XMConvertToRadians(_angle);
		float sn = sinf(degree);
		float cs = cosf(degree);
		for (int i = 0; i < 4; i++) {
			float bx = vertexs[i].position.x;
			float by = vertexs[i].position.y;
			vertexs[i].position.x = bx * cs - by * sn;
			vertexs[i].position.y = bx * sn + by * cs;
		}

		//	回転軸の変更（元の位置へ）
		for (int i = 0; i < 4; i++) {
			vertexs[i].position.x += _dw / 2.0f;
			vertexs[i].position.y += _dh / 2.0f;
		}

		//	(dx,dy)の平行移動
		for (int i = 0; i < 4; i++) {
			vertexs[i].position.x += _dx;
			vertexs[i].position.y += _dy;
		}

		vertex vertexsNDC[4];
		for (int n = 0; n < 4; n++)
		{
			//	NDC変換
			vertexsNDC[n].position.x = (vertexs[n].position.x / SCREEN_WIDTH) * 2.0f - 1.0f;
			vertexsNDC[n].position.y = -(vertexs[n].position.y / SCREEN_HEIGHT) * 2.0f + 1.0f;
			vertexsNDC[n].position.z = .0f;
			vertexsNDC[n].color = vertexs[n].color;

			// UV座標正規化
			vertexsNDC[n].texcoord.x = vertexs[n].texcoord.x / texture2d_desc.Width;
			vertexsNDC[n].texcoord.y = vertexs[n].texcoord.y / texture2d_desc.Height;
		}

		// マップに関すること

		// 頂点バッファー位置を更新した後バインドして位置が変わったものを見せている
		D3D11_MAPPED_SUBRESOURCE mapped_subresource;
		p_context->Map(p_verbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);   // ソースの意味は[もと、源]という意味をもつ
		// メモリーにマップ情報をコピー
		CopyMemory(mapped_subresource.pData, vertexsNDC, sizeof(vertex) * 4);
		//計算結果での頂点バッファーオブジェックトを更新
		p_context->Unmap(p_verbuffer, 0);

	}

	// 格納するためのバーテックスの中身を初期化する
	UINT stride = sizeof(vertex);        // 格納する側の初期化
	UINT offset = 0;                     // 読み込みの位置

	// 頂点バッファーのバインド
	p_context->IASetVertexBuffers(0, 1, &p_verbuffer, &stride, &offset);

	// プリミティブデータのバインド
	p_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // 描画されなかったときは引数が違うものだった　

	// 入力レイアウト　オブジェクトのバインド
	p_context->IASetInputLayout(p_Inputlayout);

	// ラスタライザーのステート
	p_context->RSSetState(p_rasterizerstate);


	// シェーダーの設定
	p_context->VSSetShader(p_vertexshader, NULL, 0);
	p_context->PSSetShader(p_pixelshader, NULL, 0);

	// シェーダーリソースの設定
	p_context->PSSetShaderResources(0, 1, &p_shader_resource_view);

	// サンプラーステートの設定
	p_context->PSSetSamplers(0, 1, &p_sampler_state);

	// 深度ステンシルステートオブジェクトの設定
	p_context->OMSetDepthStencilState(p_depthStencilState, 0);

	// ブレンディングステートオブジェクトの設定
	//p_context->OMSetBlendState(p_blendState,nullptr,0xFFFFFFFF);

	// プリミティブの描画
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
	for (auto c : s) // 文字コードを入れる
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
	for (auto c : s) // 文字コードを入れる
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
