#include "mesh.h"
#include "geometric_primitive.h"
#include "texture.h"

#include <vector>
#include <fstream>
#include <string>

#include <WICTextureLoader.h>

#define BUFFER_LENGTH 1024


Static_mesh::Static_mesh(ID3D11Device* device, const wchar_t* objfile,bool flippingCoordinates)
{
    std::vector<vertex> vertices; 
    std::vector<u_int> indices;
    u_int current_index = 0;

    std::vector<DirectX::XMFLOAT3> positions;
    std::vector<DirectX::XMFLOAT3> normalse;
    std::vector<DirectX::XMFLOAT2> texcoords;

    std::vector<std::wstring> mtlFilename;

    // v vertex, vt vertexTexture, vn 法線, f 面 
    
    //========================================
    // objFile open
    //========================================
    std::wifstream fin(objfile);
    
    _ASSERT_EXPR(fin, L"'OBJ file not fuund.");

    wchar_t command[256] = {0};
    while (fin)
    {
        fin >> command;
        if (0 == wcscmp(command, L"v")) // 頂点情報
        {
            float x, y, z;
            fin >> x >> y >> z; // ビットを右に移動させている。
            positions.push_back(DirectX::XMFLOAT3(x, y, z));
            fin.ignore(1024, L'\n'); // 行ごとの\nを読み取り次の行に移動させる動きです
        }
        else if (0 == wcscmp(command, L"vt"))
        {
            // OBJファイルはY成分が逆なので合わせる
            float u, v;
            fin >> u >> v;
            texcoords.push_back(DirectX::XMFLOAT2(u,flippingCoordinates ? 1.0f - v : v));
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"vn")) // 法線情報
        {
            float  i, j, k;
            fin >> i >> j >> k;
            normalse.push_back(DirectX::XMFLOAT3(i, j, k));
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"f")) // 面情報
        {
            for (u_int i = 0; i < 3; i++)
            {
                vertex vertex;
                u_int v, vt, vn;

                fin >> v;
                vertex.pos = positions[v - 1];
                if (L'/' == fin.peek())
                {
                    fin.ignore();
                    if (L'/' != fin.peek())
                    {
                        fin >> vt;
                        vertex.texcoord = texcoords[vt - 1];
                    }
                    if (L'/' == fin.peek())
                    {
                        fin.ignore();
                        fin >> vn;
                        vertex.normal = normalse[vn - 1];
                    }
                }
                vertices.push_back(vertex);
                indices.push_back(current_index++);
            }
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"mtllib"))
        {
            wchar_t mtlname[256];
            fin >> mtlname;
            mtlFilename.push_back(mtlname);
            //fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"usemtl"))
        {
            // 事前にヘッダーで作成したsubsetsにデータを入れることで            
            // CreateBuffersの引数を増やす手間をなくす
            wchar_t usemtl[MAX_PATH] = { 0 }; // windows標準マクロ windows用のプログラムなら使える
            fin >> usemtl;

            subset currentSubset = {};
            currentSubset.usemtl = usemtl;
            currentSubset.index_start = indices.size();
            subsets.push_back(currentSubset);
        }
        else
        {
            fin.ignore(1024,L'\n');
        }
    }
    fin.close();

    //========================================
    // マトリクスファイル読み込み
    //========================================
    // 変数置き場

    wchar_t mtlfile[256];

    SetDirectoryPath(mtlfile, objfile, mtlFilename[0].c_str()); // ファイルパスへと変換
    // open
    std::wifstream mfin(mtlfile);
    _ASSERT_EXPR(mfin, L"'MTL file not fuund.");

    wchar_t mCommand[256] = {0};
    while (mfin)
    {
        mfin >> mCommand;

        //newmtl 新しいマテリアル名, Ka 環境光, Kd 拡散光, Ks 鏡面光, Ns 鏡面光の焦点範囲, map Kd 画像名
        if (0 == wcscmp(mCommand, L"#"))
        {
            mfin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(mCommand, L"newmtl"))
        {
            mfin.ignore();
            wchar_t newmtl[256];
            material material;
            mfin >> newmtl;
            material.newmtl = newmtl;
            materials.push_back(material);
            
            mfin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(mCommand, L"Ka"))
        {
            float r, g, b;
            mfin >> r >> g >> b;
            materials.rbegin()->Ka = DirectX::XMFLOAT3(r, g, b);
            mfin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(mCommand, L"Kd"))
        {
            float r, g, b;
            mfin >> r >> g >> b;
            materials.rbegin()->Kd = DirectX::XMFLOAT3(r, g, b);
            mfin.ignore(1024,L'\n');
        }
        
        else if (0 == wcscmp(mCommand, L"Ks"))
        {
            float r, g, b;
            mfin >> r >> g >> b;
            materials.rbegin()->Ks = DirectX::XMFLOAT3(r, g, b);
            mfin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(mCommand, L"illum"))
        {
            u_int illum;
            mfin >> illum;
            materials.rbegin()->illum = illum;
            mfin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(mCommand, L"map_Kd"))
        {
            mfin.ignore();
            wchar_t map_Kd[256];
            mfin >> map_Kd;
            SetDirectoryPath(map_Kd, objfile, map_Kd);
            materials.rbegin()->map_Kd = map_Kd;
            mfin.ignore(1024, L'\n');
        }
        else
        {
            mfin.ignore(1024, L'\n');
        }
    }
    mfin.close();

    //textutefile = L"Cup/cup.jpg";
    // numIndices = current_index; これだと頂点情報が限られてしまうため変える

    // 頂点データを下から上に読んでいっている
    std::vector<subset>::reverse_iterator iterator = subsets.rbegin(); // subsets.rbegin()のデータが一番最後なる
    iterator->index_count = indices.size() - iterator->index_start;
    for (iterator = subsets.rbegin() + 1; iterator != subsets.rend(); ++iterator)
    {
        iterator->index_count = (iterator - 1)->index_start - iterator->index_start;
    }
    
    // buffer作成
    CreateBuffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());

    // 入力レイアウトオブジェックト生成
    D3D11_INPUT_ELEMENT_DESC input_element_desc[] = {
        { "POSITION",0, DXGI_FORMAT_R32G32B32_FLOAT,0, 0,D3D11_INPUT_PER_VERTEX_DATA,0},
        { "NORMAL" , 0, DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
        { "TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT,   0,24,D3D11_INPUT_PER_VERTEX_DATA,0 },
    };

    //　頂点シェーダーオブジェクト生成するための
    //  CSOファイルの読み込みをする
    //  geometric_primitive_vs.csoのロード
    FILE* fp = 0;
    fopen_s(&fp, "static_mesh_vs.cso", "rb");
    fseek(fp, 0, SEEK_END);
    long cso_sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    unsigned char* cso_data = new unsigned char[cso_sz];
    fread(cso_data, cso_sz, 1, fp);

    // 頂点シェーダーオブジェックト生成
    device->CreateVertexShader(cso_data, cso_sz, NULL, p_vertexShader.GetAddressOf());

    device->CreateInputLayout(input_element_desc, ARRAYSIZE(input_element_desc), cso_data, cso_sz, p_inputLayout.GetAddressOf());

    // ピクセルシェーダーオブジェクトを生成するために
    // CSOファイルを読み込む
    // geometric_primitive_ps.csoのロード
    fp = 0;
    fopen_s(&fp, "static_mesh_ps.cso", "rb");
    fseek(fp, 0, SEEK_END);
    cso_sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    cso_data = new unsigned char[cso_sz];
    fread(cso_data, cso_sz, 1, fp);


    // ピクセルシェーダーオブジェックト生成
    device->CreatePixelShader(cso_data, cso_sz, NULL, p_pixelShader.GetAddressOf());

    // ラスタライザーステート(ワイヤーフレーム)の作成
    D3D11_RASTERIZER_DESC rsDesc;
    ZeroMemory(&rsDesc, sizeof(rsDesc));
    rsDesc.FillMode = D3D11_FILL_WIREFRAME;
    rsDesc.CullMode = D3D11_CULL_BACK;
    rsDesc.FrontCounterClockwise = FALSE;
    rsDesc.DepthClipEnable = TRUE;
    rsDesc.AntialiasedLineEnable = FALSE;
    device->CreateRasterizerState(&rsDesc, p_rasterizerState[1].GetAddressOf());


    //	ラスタライザーステート(ソリッド)の作成
    //D3D11_RASTERIZER_DESC rsDesc;
    ZeroMemory(&rsDesc, sizeof(rsDesc));
    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.CullMode = D3D11_CULL_BACK;
    rsDesc.FrontCounterClockwise = FALSE;
    rsDesc.DepthClipEnable = TRUE;
    device->CreateRasterizerState(&rsDesc, p_rasterizerState[0].GetAddressOf());


    // シェーダーリソースビューの生成 マトリクスのシェーダーリソースを生成
    for (auto& material : materials)
    {
        LoadTextureFromFile(device, material.map_Kd.c_str(), &material.shaderResourceView, &texture2dDesc);
    }


    // 定数バッファオブジェクトの生成
    D3D11_BUFFER_DESC constBufferDesc;
    ZeroMemory(&constBufferDesc, sizeof(constBufferDesc));
    constBufferDesc.ByteWidth = sizeof(cbuffer);
    constBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constBufferDesc.CPUAccessFlags = 0;
    constBufferDesc.MiscFlags = 0;
    constBufferDesc.StructureByteStride = 0;

    device->CreateBuffer(&constBufferDesc, NULL, p_constBuffer.GetAddressOf());


    // サンプラーステートオブジェクトの生成
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0;
    samplerDesc.MaxAnisotropy = 16;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    //memcpy(samplerDesc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    device->CreateSamplerState(&samplerDesc, pSamplerState.GetAddressOf());

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, 0); // 初期化
    depthStencilDesc.DepthEnable = TRUE;       // 深度テスト
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;  // 深度書き込み
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    // stencilの設定
    depthStencilDesc.StencilEnable = FALSE; // 前後関係がおかしい時はこの子がfalseさせてない可能性あり

    // 作成
    device->CreateDepthStencilState(&depthStencilDesc, p_depthStancilState.GetAddressOf());

}

Static_mesh::Static_mesh(ID3D11Device* device, const wchar_t* objfile, const char* shaderfile, bool flippingCoordinates)
{

    std::vector<vertex> vertices;
    std::vector<u_int> indices;
    u_int current_index = 0;

    std::vector<DirectX::XMFLOAT3> positions;
    std::vector<DirectX::XMFLOAT3> normalse;
    std::vector<DirectX::XMFLOAT2> texcoords;

    std::vector<std::wstring> mtlFilename;

    // v vertex, vt vertexTexture, vn 法線, f 面 

    //========================================
    // objFile open
    //========================================
    std::wifstream fin(objfile);

    _ASSERT_EXPR(fin, L"'OBJ file not fuund.");

    wchar_t command[256] = { 0 };
    while (fin)
    {
        fin >> command;
        if (0 == wcscmp(command, L"v")) // 頂点情報
        {
            float x, y, z;
            fin >> x >> y >> z; // ビットを右に移動させている。
            positions.push_back(DirectX::XMFLOAT3(x, y, z));
            fin.ignore(1024, L'\n'); // 行ごとの\nを読み取り次の行に移動させる動きです
        }
        else if (0 == wcscmp(command, L"vt"))
        {
            // OBJファイルはY成分が逆なので合わせる
            float u, v;
            fin >> u >> v;
            texcoords.push_back(DirectX::XMFLOAT2(u, flippingCoordinates ? 1.0f - v : v));
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"vn")) // 法線情報
        {
            float  i, j, k;
            fin >> i >> j >> k;
            normalse.push_back(DirectX::XMFLOAT3(i, j, k));
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"f")) // 面情報
        {
            for (u_int i = 0; i < 3; i++)
            {
                vertex vertex;
                u_int v, vt, vn;

                fin >> v;
                vertex.pos = positions[v - 1];
                if (L'/' == fin.peek())
                {
                    fin.ignore();
                    if (L'/' != fin.peek())
                    {
                        fin >> vt;
                        vertex.texcoord = texcoords[vt - 1];
                    }
                    if (L'/' == fin.peek())
                    {
                        fin.ignore();
                        fin >> vn;
                        vertex.normal = normalse[vn - 1];
                    }
                }
                vertices.push_back(vertex);
                indices.push_back(current_index++);
            }
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"mtllib"))
        {
            wchar_t mtlname[256];
            fin >> mtlname;
            mtlFilename.push_back(mtlname);
            //fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"usemtl"))
        {
            // 事前にヘッダーで作成したsubsetsにデータを入れることで            
            // CreateBuffersの引数を増やす手間をなくす
            wchar_t usemtl[MAX_PATH] = { 0 }; // windows標準マクロ windows用のプログラムなら使える
            fin >> usemtl;

            subset currentSubset = {};
            currentSubset.usemtl = usemtl;
            currentSubset.index_start = indices.size();
            subsets.push_back(currentSubset);
        }
        else
        {
            fin.ignore(1024, L'\n');
        }
    }
    fin.close();

    //========================================
    // マトリクスファイル読み込み
    //========================================
    // 変数置き場

    wchar_t mtlfile[256];

    SetDirectoryPath(mtlfile, objfile, mtlFilename[0].c_str()); // ファイルパスへと変換
    // open
    std::wifstream mfin(mtlfile);
    _ASSERT_EXPR(mfin, L"'MTL file not fuund.");

    wchar_t mCommand[256] = { 0 };
    while (mfin)
    {
        mfin >> mCommand;

        //newmtl 新しいマテリアル名, Ka 環境光, Kd 拡散光, Ks 鏡面光, Ns 鏡面光の焦点範囲, map Kd 画像名
        if (0 == wcscmp(mCommand, L"#"))
        {
            mfin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(mCommand, L"newmtl"))
        {
            mfin.ignore();
            wchar_t newmtl[256];
            material material;
            mfin >> newmtl;
            material.newmtl = newmtl;
            materials.push_back(material);

            mfin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(mCommand, L"Ka"))
        {
            float r, g, b;
            mfin >> r >> g >> b;
            materials.rbegin()->Ka = DirectX::XMFLOAT3(r, g, b);
            mfin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(mCommand, L"Kd"))
        {
            float r, g, b;
            mfin >> r >> g >> b;
            materials.rbegin()->Kd = DirectX::XMFLOAT3(r, g, b);
            mfin.ignore(1024, L'\n');
        }

        else if (0 == wcscmp(mCommand, L"Ks"))
        {
            float r, g, b;
            mfin >> r >> g >> b;
            materials.rbegin()->Ks = DirectX::XMFLOAT3(r, g, b);
            mfin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(mCommand, L"illum"))
        {
            u_int illum;
            mfin >> illum;
            materials.rbegin()->illum = illum;
            mfin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(mCommand, L"map_Kd"))
        {
            mfin.ignore();
            wchar_t map_Kd[256];
            mfin >> map_Kd;
            SetDirectoryPath(map_Kd, objfile, map_Kd);
            materials.rbegin()->map_Kd = map_Kd;
            mfin.ignore(1024, L'\n');
        }
        else
        {
            mfin.ignore(1024, L'\n');
        }
    }
    mfin.close();

    //textutefile = L"Cup/cup.jpg";
    // numIndices = current_index; これだと頂点情報が限られてしまうため変える

    // 頂点データを下から上に読んでいっている
    std::vector<subset>::reverse_iterator iterator = subsets.rbegin(); // subsets.rbegin()のデータが一番最後なる
    iterator->index_count = indices.size() - iterator->index_start;
    for (iterator = subsets.rbegin() + 1; iterator != subsets.rend(); ++iterator)
    {
        iterator->index_count = (iterator - 1)->index_start - iterator->index_start;
    }

    // buffer作成
    CreateBuffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());

    // 入力レイアウトオブジェックト生成
    D3D11_INPUT_ELEMENT_DESC input_element_desc[] = {
        { "POSITION",0, DXGI_FORMAT_R32G32B32_FLOAT,0, 0,D3D11_INPUT_PER_VERTEX_DATA,0},
        { "NORMAL" , 0, DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
        { "TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT,   0,24,D3D11_INPUT_PER_VERTEX_DATA,0 },
    };

    //　頂点シェーダーオブジェクト生成するための
    //  CSOファイルの読み込みをする
    //  geometric_primitive_vs.csoのロード
    FILE* fp = 0;

    char cso_name[128] = {}; 
    strcpy_s(cso_name, shaderfile);//　後ろの変数内を1つ目の引数にコピー
    strcat_s(cso_name, "_vs.cso"); // 第1引数の文字列の後ろに第2引数を付け足す

    fopen_s(&fp, cso_name, "rb");
    fseek(fp, 0, SEEK_END);
    long cso_sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    unsigned char* cso_data = new unsigned char[cso_sz];
    fread(cso_data, cso_sz, 1, fp);

    // 頂点シェーダーオブジェックト生成
    device->CreateVertexShader(cso_data, cso_sz, NULL, p_vertexShader.GetAddressOf());

    device->CreateInputLayout(input_element_desc, ARRAYSIZE(input_element_desc), cso_data, cso_sz, p_inputLayout.GetAddressOf());

    // ピクセルシェーダーオブジェクトを生成するために
    // CSOファイルを読み込む
    // geometric_primitive_ps.csoのロード
    fp = 0;
    
    strcpy_s(cso_name, shaderfile);//　後ろの変数内を1つ目の引数にコピー
    strcat_s(cso_name, "_ps.cso"); // 第1引数の文字列の後ろに第2引数を付け足す

    fopen_s(&fp, "static_mesh_ps.cso", "rb");
    fseek(fp, 0, SEEK_END);
    cso_sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    cso_data = new unsigned char[cso_sz];
    fread(cso_data, cso_sz, 1, fp);


    // ピクセルシェーダーオブジェックト生成
    device->CreatePixelShader(cso_data, cso_sz, NULL, p_pixelShader.GetAddressOf());

    // ラスタライザーステート(ワイヤーフレーム)の作成
    D3D11_RASTERIZER_DESC rsDesc;
    ZeroMemory(&rsDesc, sizeof(rsDesc));
    rsDesc.FillMode = D3D11_FILL_WIREFRAME;
    rsDesc.CullMode = D3D11_CULL_BACK;
    rsDesc.FrontCounterClockwise = FALSE;
    rsDesc.DepthClipEnable = TRUE;
    rsDesc.AntialiasedLineEnable = FALSE;
    device->CreateRasterizerState(&rsDesc, p_rasterizerState[1].GetAddressOf());


    //	ラスタライザーステート(ソリッド)の作成
    //D3D11_RASTERIZER_DESC rsDesc;
    ZeroMemory(&rsDesc, sizeof(rsDesc));
    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.CullMode = D3D11_CULL_BACK;
    rsDesc.FrontCounterClockwise = FALSE;
    rsDesc.DepthClipEnable = TRUE;
    device->CreateRasterizerState(&rsDesc, p_rasterizerState[0].GetAddressOf());


    // シェーダーリソースビューの生成 マトリクスのシェーダーリソースを生成
    for (auto& material : materials)
    {
        LoadTextureFromFile(device, material.map_Kd.c_str(), &material.shaderResourceView, &texture2dDesc);
    }


    // 定数バッファオブジェクトの生成
    D3D11_BUFFER_DESC constBufferDesc;
    ZeroMemory(&constBufferDesc, sizeof(constBufferDesc));
    constBufferDesc.ByteWidth = sizeof(cbuffer);
    constBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constBufferDesc.CPUAccessFlags = 0;
    constBufferDesc.MiscFlags = 0;
    constBufferDesc.StructureByteStride = 0;

    device->CreateBuffer(&constBufferDesc, NULL, p_constBuffer.GetAddressOf());


    // サンプラーステートオブジェクトの生成
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0;
    samplerDesc.MaxAnisotropy = 16;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    //memcpy(samplerDesc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    device->CreateSamplerState(&samplerDesc, pSamplerState.GetAddressOf());

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, 0); // 初期化
    depthStencilDesc.DepthEnable = TRUE;       // 深度テスト
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;  // 深度書き込み
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    // stencilの設定
    depthStencilDesc.StencilEnable = FALSE; // 前後関係がおかしい時はこの子がfalseさせてない可能性あり

    // 作成
    device->CreateDepthStencilState(&depthStencilDesc, p_depthStancilState.GetAddressOf());

}



bool Static_mesh::CreateBuffers(ID3D11Device* device, vertex* vertices, int numVertex, unsigned int* indices, int numIndex)
{
    // 頂点バッファオブジェクトの生成
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
    device->CreateBuffer(&vertexBufferDesc, &vertexSubResource, p_vertexBuffer.ReleaseAndGetAddressOf());

    if (indices) {
        // インデックスバッファオブジェクトの生成
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

        device->CreateBuffer(&indexBufferDesc, &indexSubResource, p_indexBuffer.ReleaseAndGetAddressOf());
    }

    return true;
}

void Static_mesh::render(ID3D11DeviceContext* p_context, const DirectX::XMFLOAT4X4& wvp, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color, bool fsolid)
{
    // 格納するためのバーテックスの中身を初期化する
    UINT stride = sizeof(vertex);        // 格納する側の初期化
    UINT offset = 0;                     // 読み込みの位置

    // 頂点バッファのバインド
    p_context->IASetVertexBuffers(0, 1, p_vertexBuffer.GetAddressOf(), &stride, &offset);

    // インデックスバッファのバインド
    p_context->IASetIndexBuffer(p_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);


    p_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 入力レイアウトのバインド
    p_context->IASetInputLayout(p_inputLayout.Get());

    // ラスタライザーの設定
    if (fsolid)
    {
        p_context->RSSetState(p_rasterizerState[1].Get());
    }
    else p_context->RSSetState(p_rasterizerState[0].Get());

    // シェーダーの設定
    p_context->VSSetShader(p_vertexShader.Get(), nullptr, 0);
    p_context->PSSetShader(p_pixelShader.Get(), nullptr, 0);

    // 深度テストの設定
    p_context->OMSetDepthStencilState(p_depthStancilState.Get(), 1);

    for (material& material : materials)
    {
        cbuffer cdata;
        cdata.world_view_projection = wvp;
        cdata.world = world;
        cdata.light_direction = light_direction;
        //cdata.material_color = material_color;
        cdata.material_color.x = material_color.x * material.Kd.x;
        cdata.material_color.y = material_color.y * material.Kd.y;
        cdata.material_color.z = material_color.z * material.Kd.z;
        cdata.material_color.w = material_color.w;
        p_context->UpdateSubresource(p_constBuffer.Get(), 0, 0, &cdata, 0, 0);
        p_context->VSSetConstantBuffers(0, 1, p_constBuffer.GetAddressOf());

        p_context->PSSetShaderResources(0, 1, material.shaderResourceView.GetAddressOf());
        p_context->PSSetSamplers(0, 1, pSamplerState.GetAddressOf());
        for (subset& subset : subsets)
        {
            if (material.newmtl == subset.usemtl)
            {
                p_context->DrawIndexed(subset.index_count, subset.index_start, 0);
            }
        }

    }
}
