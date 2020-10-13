#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>

#include <d3d11.h>
#include <wrl.h>		// ComPtr
#include <memory>		// スマートポインタ

#include "sprite.h"
#include "geometric_primitive.h"
#include "mesh.h"

#include "debug.h"
#include "misc.h"
#include "high_resolution_timer.h"


#include "./imGui/imgui.h"
#include "./imGui/imgui_impl_dx11.h"
#include "./imGui/imgui_impl_win32.h"

#define SAFE_DELETE(x) if((x)){delete (x);(x)=NULL;}

const LONG SCREEN_WIDTH = 1280;
const LONG SCREEN_HEIGHT = 720;


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static bool Hflg = false;
static bool Window1Flg = true;

class framework
{
private:

	Microsoft::WRL::ComPtr<ID3D11Device> p_device = NULL;

	Microsoft::WRL::ComPtr<ID3D11DeviceContext>    p_deviceContext    = NULL;	// レンダリングパイプライン(出力マネージャー)
	Microsoft::WRL::ComPtr<IDXGISwapChain>		   p_swapChain	      = NULL;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> p_backBuffer_RTV   = NULL;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> p_depthStencilView = NULL;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>        p_DepthStencilTexture;
	Microsoft::WRL::ComPtr<ID3D11BlendState>	   p_BlendState       = NULL;

	static framework* instance;

	std::unique_ptr<Sprite>		sprites[1024];
	// std::unique_ptr<Sprite>		text;
	std::unique_ptr<Sprite>		particle;
	std::unique_ptr<GeometricPrimitive> cube;
	std::unique_ptr<GeometricPrimitive> sphere;

	std::unique_ptr<Static_mesh> obj;
	

public:
	const HWND hwnd;
	std::unique_ptr<Debug> debug;

	static framework* getInstance()
	{
		return instance;
	}

    // フレームにでているFPSやWindow全体に関するクラス
    // フレームワークのFPSとか出ているところのことをすぐ下のコードがやっているのではないか？
	framework(HWND hwnd) : hwnd(hwnd)
	{
		instance = this;
	}
	~framework()
	{
		// 解放
		//scene->Release();
	}

	ID3D11Device* getDevice() { return p_device.Get(); }
	ID3D11DeviceContext* getDeviceContext() { return p_deviceContext.Get(); }
	float getElapsedTime() { return timer.time_interval(); }

	int run()
	{
		MSG msg = {};

		if (!initialize()) return 0;
		timer.reset();

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				timer.tick();
				calculate_frame_stats(); 

				//ImGui_ImplDX11_NewFrame();
				//ImGui_ImplWin32_NewFrame();

				update(timer.time_interval());

				render(timer.time_interval());
			}
		}
		return static_cast<int>(msg.wParam);
	}


	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }

		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE) PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case WM_ENTERSIZEMOVE:
			// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
			timer.stop();
			break;
		case WM_EXITSIZEMOVE:
			// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			// Here we reset everything based on the new window dimensions.
			timer.start();
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}
private:

	bool initialize();
	void update(float elapsed_time/*Elapsed seconds from last frame*/);
	void render(float elapsed_time/*Elapsed seconds from last frame*/);

public:
	float	clearColor[4] = { 0.0f, 0.8f, 0.0f ,1.0f };

	
private:
	high_resolution_timer timer;
    void calculate_frame_stats()
    {
        // Code computes the average frames per second, and also the 
        // average time it takes to render one frame.  These stats 
        // are appended to the window caption bar.
        static int frames = 0;
        static float time_tlapsed = 0.0f;

        frames++;

        // Compute averages over one second period.
        if ((timer.time_stamp() - time_tlapsed) >= 1.0f)
        {
            float fps = static_cast<float>(frames); // fps = frameCnt / 1
            float mspf = 1000.0f / fps;
            std::ostringstream outs;
            outs.precision(6);
            outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
            SetWindowTextA(hwnd, outs.str().c_str());
			
            // Reset for next average.
            frames = 0;
            time_tlapsed += 1.0f;
        }
    }
};

#define pFramework framework::getInstance()
