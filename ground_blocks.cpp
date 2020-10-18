#include "ground_blocks.h"
#include "scene.h"
#include <fstream>
#include <iostream>
#include <sstream>

GroundBlock::GroundBlock(std::shared_ptr<GeometricPrimitive>& primitive)
{
    count = 0;
    type = Block::I_block;
    pos = { 0.0,-1.0f,0.0f };

    obj = primitive;
}

void GroundBlock::Update()
{
    switch (type)
    {
    case Block::D_block_1:
    case Block::D_block_2:
    case Block::D_block_3:
        DestroyBlock();
        break;
    case Block::C_block:
        SelectBlock();
        break;
    }

}

void GroundBlock::Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& wvp, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& light_dir, const DirectX::XMFLOAT4& materialColor, bool wireframe)
{
    obj->render(context,wvp,world,light_dir,materialColor,wireframe);
}

void GroundBlock::DestroyBlock()
{
    if (oldhover == true && hoverflg == false)
    {
        leaveflg = true;
    }
    if (leaveflg) { count--,leaveflg = false; }

    if (count <= 0)
    {
        pos.y -= 1.0f;
    }
}

void GroundBlock::SelectBlock()
{
    if (hoverflg)
    {
        SceneManager::Instance().ChangeScenePerformance(SceneGame::getInstance());
    }
}


//*********************************************************
//
// ブロック管理
//
//*********************************************************
void GroundBlockManager::Initialize(int y, int x, std::shared_ptr<GeometricPrimitive>& primitive)
{
    static int count; // このカウント数分objを生成する
    // 中に値が入っている可能性を考えて初期化する
    if (mapX != 0 || mapY != 0)
    {
        mapX, mapY = 0;
    }
    for (auto& obj : obj)
    {
        if(obj != nullptr)
        obj = nullptr;
    }

    // マップデータを読み込む
    mapX = x;
    mapY = y;
    

    for (auto& obj : obj)
    {
        obj = std::make_unique<GroundBlock>(primitive);
    }
}

void GroundBlockManager::Initialize()
{
    if (mapX != 0 || mapY != 0 || mea)
    {
        mapX = 0, mapY = 0, mea = 0;
    }
    for (auto& obj : obj)
    {
        if (obj != nullptr)
            obj = nullptr;
    }

    // ファイルを読み込み用変数に入れる
    sprintf_s(fileNameStage, sizeof(fileNameStage), "./Data/Stage/stage%d.csv", stageNum); // 最後の引数は%dの数字分です

    // ファイル内の行列を取得
    GetCsvSize(fileNameStage, &mapX, &mapY);

    // マップのメモリを確保する
    // mencopy使えばもっと楽になるかもしれない。
    mapData = new char* [mapY];
    for (uint32_t y = 0; y < mapY; ++y)
    {
        mapData[y] = new char[mapX];
    }

    if (!LoadMapData(fileNameStage, mapData))
    {
        assert(!"ステージデータのロードに失敗");
    }

    // オブジェクト数を取得
    mea = mapX * mapY;

}

void GroundBlockManager::SetPrimitive(std::shared_ptr<GeometricPrimitive> primitive)
{
    for (int i = 0; i < mea;i++)
    {
        obj[i] = std::make_unique<GroundBlock>(primitive);
    }

    // マップからブロックの種類を取得
    int count = 0;
    for (int y = 0; y < mapY; y++)
    {
        for (int x = 0; x < mapX; x++)
        {
            obj[count]->SetType(mapData[y][x]);
            count++;
        }
    }
}

void GroundBlockManager::Update()
{
    for (int count = 0 ; count < mea ;count++)
    {
        obj[count]->Update();
    }
}

void GroundBlockManager::Render(ID3D11DeviceContext* context, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir, bool wireframe)
{
    int count = 0;
    for (int y = 0; y < mapY; y++) {
        for (int x = 0; x < mapX; x++) {
            if (mapData[y][x] < 0) continue;
            DirectX::XMMATRIX W, S, R, T;
            DirectX::XMFLOAT4X4 wvp;
            DirectX::XMFLOAT4X4 world;

            int posx = x * 1.0f, posz = y * 1.0f;

            S = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
            R = DirectX::XMMatrixRotationRollPitchYaw(0, 0, 0);
            T = DirectX::XMMatrixTranslation(posx, obj[count]->GetPosition().y,posz );
            W = S * R * T;

            obj[count]->SetBlockPosXZ(posx, posz);

            // ★MATRIX -> FLOAT4X4
            DirectX::XMStoreFloat4x4(&world, W);
            DirectX::XMStoreFloat4x4(&wvp, W * view * projection);
            DirectX::XMFLOAT4 material_color = { 0.0f,0.6f,0.5f,1.0f };

            obj[count]->Render(context,wvp, world, light_dir, material_color, wireframe);
            count++;
        }
    }
}

void GroundBlockManager::Relese()
{
    for (uint32_t y = 0; y < mapY; ++y)
    {
        delete[] mapData[y];
    }
    delete[] mapData;
}

bool GroundBlockManager::LoadMapData(const char* fileName, char** map)
{
    std::ifstream input(fileName);
    if (input.fail())
    {
        return false;
    }

    uint32_t y = 0, x = 0;
    std::string output;
    while (std::getline(input, output))
    {
        x = 0;
        std::istringstream iss(output);
        while (std::getline(iss, output, ','))
        {
            assert(y < mapY);
            assert(x < mapX);
            map[y][x] = static_cast<char>(std::stoi(output));
            ++x;
        }
        ++y;
    }

    return true;
}

bool GroundBlockManager::GetCsvSize(std::string path, uint32_t* mapX, uint32_t* mapY)
{
    std::ifstream inputFile(path); // 持ってきたファイルデータを開いている fopen
    if (inputFile.fail())return false;

    std::string output;
    uint32_t x = 0 , y = 0;
    while (std::getline(inputFile, output))
    {
        if (!y) x = std::count(output.cbegin(), output.cend(), ',') + 1;
        ++y;
    }

    *mapX = x;
    *mapY = y;

    return true;

}
