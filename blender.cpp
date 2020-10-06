#include "blender.h"

Microsoft::WRL::ComPtr<ID3D11BlendState> blender::states[BS_END] = { 0 };
bool								     blender::bLoad = false;
blender::BLEND_STATE					 blender::nowState;
ID3D11DeviceContext*					 blender::keepContext;


void blender::Init(ID3D11Device* device, ID3D11DeviceContext* context)
{
	if (bLoad) return;


	HRESULT hr = S_OK;
	D3D11_BLEND_DESC blend_desc = {};
	// BS_NONE	‚È‚É‚à‚µ‚È‚¢
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = FALSE;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blend_desc, states[BS_NONE].GetAddressOf());
	if (FAILED(hr))assert("NONE_BLEND ERROR");

	// BS_ALPHA	“§–¾‰»
	//D3D11_BLEND_DESC blendDesc = {};
	blend_desc.AlphaToCoverageEnable = false;
	blend_desc.IndependentBlendEnable = false;
	// blendDesc.RenderTarget‚ÌÝ’è
	blend_desc.RenderTarget->BlendEnable = true;
	blend_desc.RenderTarget->SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc.RenderTarget->DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blend_desc.RenderTarget->BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget->SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget->DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blend_desc.RenderTarget->BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget->RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	// ì¬
	hr = device->CreateBlendState(&blend_desc, states[BS_ALPHA].GetAddressOf());
	if (FAILED(hr))assert("NONE_BLEND ERROR");

	// BS_ADD ‰ÁŽZ
	blend_desc.AlphaToCoverageEnable = false;
	blend_desc.IndependentBlendEnable = false;
	// blendDesc.RenderTarget‚ÌÝ’è
	blend_desc.RenderTarget->BlendEnable = TRUE;
	blend_desc.RenderTarget->SrcBlend = D3D11_BLEND_SRC_ALPHA; //D3D11_BLEND_ONE
	blend_desc.RenderTarget->DestBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget->BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget->SrcBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget->DestBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget->BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget->RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	// ì¬
	hr = device->CreateBlendState(&blend_desc, states[BS_ADD].GetAddressOf());
	if (FAILED(hr))assert("NONE_BLEND ERROR");

	// BS_SUBTRACT Œ¸ŽZ
	blend_desc.AlphaToCoverageEnable = false;
	blend_desc.IndependentBlendEnable = false;
	// blendDesc.RenderTarget‚ÌÝ’è
	blend_desc.RenderTarget->BlendEnable = TRUE;
	blend_desc.RenderTarget->SrcBlend = D3D11_BLEND_ZERO; //D3D11_BLEND_SRC_ALPHA
	blend_desc.RenderTarget->DestBlend = D3D11_BLEND_INV_SRC_COLOR; //D3D11_BLEND_
	blend_desc.RenderTarget->BlendOp = D3D11_BLEND_OP_ADD; //D3D11_BLEND_OP_SUBTRA
	blend_desc.RenderTarget->SrcBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget->DestBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget->BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget->RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	// ì¬
	hr = device->CreateBlendState(&blend_desc, states[BS_SUBTRACT].GetAddressOf());
	if (FAILED(hr))assert("NONE_BLEND ERROR");

	// BS_REPLACE 
	blend_desc.AlphaToCoverageEnable = false;
	blend_desc.IndependentBlendEnable = false;
	// blendDesc.RenderTarget‚ÌÝ’è
	blend_desc.RenderTarget->BlendEnable = TRUE;
	blend_desc.RenderTarget->SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc.RenderTarget->DestBlend = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget->BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget->SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget->DestBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget->BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget->RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	// ì¬
	hr = device->CreateBlendState(&blend_desc, states[BS_REPLACE].GetAddressOf());
	if (FAILED(hr))assert("NONE_BLEND ERROR");

	// BS_MULTIPLY
	blend_desc.AlphaToCoverageEnable = false;
	blend_desc.IndependentBlendEnable = false;
	// blendDesc.RenderTarget‚ÌÝ’è
	blend_desc.RenderTarget->BlendEnable = TRUE;
	blend_desc.RenderTarget->SrcBlend = D3D11_BLEND_ZERO; //D3D11_BLEND_DEST_COLOR
	blend_desc.RenderTarget->DestBlend = D3D11_BLEND_SRC_COLOR; //D3D11_BLEND_SRC_
	blend_desc.RenderTarget->BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget->SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
	blend_desc.RenderTarget->DestBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget->BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget->RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	// ì¬
	hr = device->CreateBlendState(&blend_desc, states[BS_MULTIPLY].GetAddressOf());
	if (FAILED(hr))assert("NONE_BLEND ERROR");

	// BS_LIGHTEN
	blend_desc.AlphaToCoverageEnable = false;
	blend_desc.IndependentBlendEnable = false;
	// blendDesc.RenderTarget‚ÌÝ’è
	blend_desc.RenderTarget->BlendEnable = TRUE;
	blend_desc.RenderTarget->SrcBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget->DestBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget->BlendOp = D3D11_BLEND_OP_MAX;
	blend_desc.RenderTarget->SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget->DestBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget->BlendOpAlpha = D3D11_BLEND_OP_MAX;
	blend_desc.RenderTarget->RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	// ì¬
	hr = device->CreateBlendState(&blend_desc, states[BS_LIGHTEN].GetAddressOf());
	if (FAILED(hr))assert("NONE_BLEND ERROR");

	// BS_DARKEN
	blend_desc.AlphaToCoverageEnable = false;
	blend_desc.IndependentBlendEnable = false;
	// blendDesc.RenderTarget‚ÌÝ’è
	blend_desc.RenderTarget->BlendEnable = TRUE;
	blend_desc.RenderTarget->SrcBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget->DestBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget->BlendOp = D3D11_BLEND_OP_MIN;
	blend_desc.RenderTarget->SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget->DestBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget->BlendOpAlpha = D3D11_BLEND_OP_MIN;
	blend_desc.RenderTarget->RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	// ì¬
	hr = device->CreateBlendState(&blend_desc, states[BS_DARKEN].GetAddressOf());
	if (FAILED(hr))assert("NONE_BLEND ERROR");

	// BS_SCREEN
	blend_desc.AlphaToCoverageEnable = false;
	blend_desc.IndependentBlendEnable = false;
	// blendDesc.RenderTarget‚ÌÝ’è
	blend_desc.RenderTarget->BlendEnable = TRUE;
	blend_desc.RenderTarget->SrcBlend = D3D11_BLEND_SRC_ALPHA; //D3DBLEND_INVDESTC
	blend_desc.RenderTarget->DestBlend = D3D11_BLEND_INV_SRC_COLOR; //D3DBLEND_ONE
	blend_desc.RenderTarget->BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget->SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget->DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blend_desc.RenderTarget->BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget->RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	// ì¬
	hr = device->CreateBlendState(&blend_desc, states[BS_SCREEN].GetAddressOf());
	if (FAILED(hr))assert("NONE_BLEND ERROR");

	keepContext = context;
	nowState = BS_END;

	bLoad = true;
}

void blender::Set(BLEND_STATE state, ID3D11DeviceContext* context)
{
	if (!bLoad) return;
	if (state < 0 || state >= BS_END) return;
	if (state == nowState) return;

	// Ž–‘O‚ÉÝ’è‚µ‚Ä‚¢‚écontext‚ðŽg—p
	if (!context) context = keepContext;


	context->OMSetBlendState(states[state].Get(), nullptr, 0xFFFFFFFF);
	nowState = state;
}

void blender::Release()
{
	keepContext = nullptr;
	nowState = BS_END;
	bLoad = false;
}
