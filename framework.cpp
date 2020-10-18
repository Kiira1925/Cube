#include "framework.h"
#include "sprite.h"
#include "debug.h"
#include "blender.h"
#include "scene.h"

// frameworkのstatic変数宣言・初期化
// static変数はクラス別で宣言しなければいけない
framework* framework::instance = nullptr;

bool framework::initialize()
{
    HRESULT hr = S_OK;  // おそらく初期化するかの判定

    soundManager = std::make_unique<SoundManager>(pFramework->hwnd);

    RECT rc; // モデルオブジェクトの生成をしている
    GetClientRect(hwnd, &rc);   // 生成するための情報をいれている？
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    D3D_DRIVER_TYPE driverTypes[] = // これは調べてもよくわからなかったドライブというなにかに情報をいれているけど
    {                               // 一体なにをしているのかが調べても理解が難しかった
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));    // バッファーの設定をここでしている
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    // ここでドライブタイプのデバイスとスワップチェインを作成していると考えている
    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        D3D_DRIVER_TYPE driverType = driverTypes[driverTypeIndex];
        D3D_FEATURE_LEVEL featureLevel;
        hr = D3D11CreateDeviceAndSwapChain(NULL, driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &sd, p_swapChain.GetAddressOf(), p_device.GetAddressOf(), &featureLevel, p_deviceContext.GetAddressOf());
        if (SUCCEEDED(hr))                       // GetAddressOf()...Comptrの中にいるID3D11Device*のアドレスを返している
            break;
    }
        if (FAILED(hr))
            return false;

        // ここからはテクスチャデータの作成編集
        ID3D11Texture2D* pBackBuffer = NULL;
        hr = p_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
        if (FAILED(hr))
            return false;
       
        hr = p_device->CreateRenderTargetView(pBackBuffer, NULL, p_backBuffer_RTV.GetAddressOf());
        pBackBuffer->Release();
        if (FAILED(hr))
            return false;


        D3D11_TEXTURE2D_DESC txDesc;
        ZeroMemory(&txDesc, sizeof(txDesc));
        txDesc.Width = SCREEN_WIDTH;
        txDesc.Height = SCREEN_HEIGHT;
        txDesc.MipLevels = 1;
        txDesc.ArraySize = 1;
        txDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        txDesc.SampleDesc.Count = 1;
        txDesc.SampleDesc.Quality = 0;
        txDesc.Usage = D3D11_USAGE_DEFAULT;
        txDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        txDesc.CPUAccessFlags = 0;
        txDesc.MiscFlags = 0;
        hr = p_device->CreateTexture2D(&txDesc, NULL, p_DepthStencilTexture.GetAddressOf());
        if (FAILED(hr))		return false;

        D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
        ZeroMemory(&dsDesc, sizeof(dsDesc));
        dsDesc.Format = txDesc.Format;
        dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        dsDesc.Texture2D.MipSlice = 0;
        hr = p_device->CreateDepthStencilView(p_DepthStencilTexture.Get(), &dsDesc, p_depthStencilView.GetAddressOf());
        //pDepthStencilTexture->Release();

        blender::Init(p_device.Get(), p_deviceContext.Get());

        //*******************************
        // imgui導入
        //*******************************
        //ImGui_ImplWin32_Init(hwnd);
        //ImGui_ImplDX11_Init(p_device.Get(), p_deviceContext.Get());
        

        // text = std::make_unique<Sprite>(p_device.Get(), L"./fonts/font0.png");
        debug = std::make_unique<Debug>(p_device.Get(), L"./fonts/font0.png");

        // ソース読み込み
        sprites[0] = std::make_unique<Sprite>(pFramework->getDevice(), L"./Data/Menu/BB.png");
        sprites[1] = std::make_unique<Sprite>(pFramework->getDevice(), L"./Data/Menu/Pause.png");
        sprites[2] = std::make_unique<Sprite>(pFramework->getDevice(), L"./Data/Menu/Aicon.png");
        // sprites[3] = std::make_unique<Sprite>(pFramework->getDevice(), L"./Data/Menu/WB.png");

        soundSE[0] = soundManager->CreateSoundSource("Data/Sounds/SE/select.wav");
        soundSE[1] = soundManager->CreateSoundSource("Data/Sounds/SE/check.wav");
        // soundBGM->Play(true);

        SceneManager::Instance().SetScene(SceneTitle::getInstance());
        if (FAILED(hr))		return false;

        ///////////////////////////////////////////////////
        //	DirectX初期化終了 以降描画物体の初期化
        ////////////////////////////////////////////////////


        return true;
}
void framework::update(float elapsed_time/*Elapsed seconds from last frame*/)
{
    SceneManager::Instance().Update();
    //scene->Update(elapsed_time);
}
void framework::render(float elapsed_time/*Elapsed seconds from last frame*/)
{
    // 初期化で用意したキャンパス(ドライブかな？)に色を付けたり表示位置などを設定する
    //SetClearColor(clearColor);
    D3D11_VIEWPORT vp;  // カメラの位置設定(視点)
    vp.Width = (FLOAT)SCREEN_WIDTH;
    vp.Height = (FLOAT)SCREEN_HEIGHT;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;

    p_deviceContext->RSSetViewports(1, &vp);

    //// Setup the viewport
    ////左上
    //D3D11_VIEWPORT vp[4];  // カメラの位置設定(視点)
    //vp[0].Width = (FLOAT)SCREEN_WIDTH/2;
    //vp[0].Height = (FLOAT)SCREEN_HEIGHT/2;
    //vp[0].MinDepth = 0.0f;
    //vp[0].MaxDepth = 1.0f;
    //vp[0].TopLeftX = 0;
    //vp[0].TopLeftY = 0;


    ////左下
    //vp[1].Width = (FLOAT)SCREEN_WIDTH / 2;
    //vp[1].Height = (FLOAT)SCREEN_HEIGHT / 2;
    //vp[1].MinDepth = 0.0f;
    //vp[1].MaxDepth = 1.0f;
    //vp[1].TopLeftX = 0;
    //vp[1].TopLeftY = SCREEN_HEIGHT/2;

    ////右上
    //vp[2].Width = (FLOAT)SCREEN_WIDTH / 2;
    //vp[2].Height = (FLOAT)SCREEN_HEIGHT / 2;
    //vp[2].MinDepth = 0.0f;
    //vp[2].MaxDepth = 1.0f;
    //vp[2].TopLeftX = SCREEN_WIDTH/2;
    //vp[2].TopLeftY = 0;

    ////右下
    //vp[3].Width = (FLOAT)SCREEN_WIDTH / 2;
    //vp[3].Height = (FLOAT)SCREEN_HEIGHT / 2;
    //vp[3].MinDepth = 0.0f;
    //vp[3].MaxDepth = 1.0f;
    //vp[3].TopLeftX = SCREEN_WIDTH/2;
    //vp[3].TopLeftY = SCREEN_HEIGHT/2;

    // Just clear the backbuffer
    p_deviceContext->ClearRenderTargetView(p_backBuffer_RTV.Get(), clearColor);// この関数が色を付けている

    p_deviceContext->ClearDepthStencilView(// 場所を決定しているかも？
        p_depthStencilView.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f, 0);

    p_deviceContext->OMSetRenderTargets(1, p_backBuffer_RTV.GetAddressOf(), p_depthStencilView.Get());// 描画するものを設定する

   

    ///////////////////////////////////////////////////
    //		描画開始
    ////////////////////////////////////////////////////

    benchmark bm;
    bm.begin();

    // view port設定のループ
    //for (int i = 0; i < sizeof(vp) / sizeof(vp[0]); i++)
    //{
    //    p_deviceContext->RSSetViewports(1, &vp[i]);
    //    scene->Render(elapsed_time);
    //}
    SceneManager::Instance().Render();

    float bmTimer = bm.end();

    // データ保存
    static const int N = 10;
    static float time[N] = { 0 }; // バッファの初期化
    // バッファの更新
    time[0] = bmTimer;
    for (int i = N; i > 0; i--)
    {
        time[i] = time[i - 1];
    }

    // 最小値検出
    float min = FLT_MAX;

    for (int i = 0; i < N; i++)
    {
        if (time[i] < min)
        {
            min = time[i];
        }

    }

    // 整数を文字列に変換
    std::string s = std::to_string(min);
    // 合成
    s = "BenchMark:" + s;
    debug->setString("BenchMark:%d", min);
    blender::Set(blender::BS_ADD);
    // text->textout(p_deviceContext.Get(), s, 10, 10, 24, 32);
    debug->display(p_deviceContext.Get(), 1.0f, 1.0f, 1.0f, 10.0f, 10.0f);

    ///////////////////////////////////////////////////
    //		描画終了
    ////////////////////////////////////////////////////

    //*******************
    // ImGui描画
    //*******************
    //ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    p_swapChain->Present(1, 0);
}




