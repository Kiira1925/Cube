//*********************************************************
//
//		ブレンドオブジェクト
//
//*********************************************************
#pragma once

#include <d3d11.h>
#include <wrl.h>

#include <assert.h>

class blender
{
public:
	enum BLEND_STATE 
	{ 
		BS_NONE,
		BS_ALPHA,
		BS_ADD,
		BS_SUBTRACT,
		BS_REPLACE,
		BS_MULTIPLY,
		BS_LIGHTEN,
		BS_DARKEN,
		BS_SCREEN,
		BS_END 
	};
private:
	static Microsoft::WRL::ComPtr<ID3D11BlendState> states[BS_END]; // blendmodeを9個作製
	static bool  bLoad;
	static BLEND_STATE nowState;
	static ID3D11DeviceContext* keepContext;


	blender(){}

public:
	static void Init(ID3D11Device* device, ID3D11DeviceContext* context);

	static void Set(BLEND_STATE state, ID3D11DeviceContext* context = nullptr);

	static void Release();

};