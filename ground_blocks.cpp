#include "ground_blocks.h"
#include <fstream>
#include <iostream>
#include <sstream>

int map_data[15][20] =
{
    /*     00  01  02  03  04  05  06  07  08  09  10  11  12  13  14  15  16  17  18  19   */
    /*00*/{ 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64  },
    /*01*/{ 64,  0,  0,  0,  0, 67, 68,  0,  0, 128, 0,  0,  0,  0,  0,  0,  0,  0,  0, 64  },
    /*02*/{ 64,128,128,  0,  0, 67, 68,  0,  0, 128, 0, 128,128,128,128, 0, 128,128,128,64  },
    /*03*/{ 64,  0,128,  0,  0, 65, 66,  0,  0, 128, 0,  0,  0,  0,  0,  0,  0,  0,  0, 64  },
    /*04*/{ 64,  0,128,  0,  0,  0,  0,  0,  0, 128, 0,  0, 128,128,128,128,128,128, 0, 64  },
    /*05*/{ 64,  0,128,  0,  0,  0,  0,  0,  0,  0,  0,  0, 128, 0,  0, 128, 0,  0,  0, 64  },
    /*06*/{ 64,  0,128,  0, 128,128,128,128, 0,  0,  0,  0, 128, 0,  0, 128, 0, 128, 0, 64  },
    /*07*/{ 64,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 128, 0, 128,128, 0, 128, 0, 64  },
    /*08*/{ 64,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 128, 0,  0,  0,  0, 128, 0, 64  },
    /*09*/{ 64,  0,128,128,128,128,  0,  0,  0, 65, 66,  0, 128, 0,128,128,128,128,  0, 64  },
    /*10*/{ 64,  0,128,  0,  0,  0,  0,  0,  0, 67, 68,  0,  0,  0,  0,  0,  0,  0,  0, 64  },
    /*11*/{ 64,  0,128,  0,128,  0,128,128,  0, 67, 68,  0,  0,128,  0,  0,  0,  0,  0, 64  },
    /*12*/{ 64,  0,128,  0,128,  0,128,  0,  0, 67, 68,  0,128,128,  0, 65, 66,  0,  0, 64  },
    /*13*/{ 64,  0,  0,  0,  0,  0,128,  0,  0, 67, 68,  0,  0,  0,  0, 67, 68,  0,  0,  0, },
    /*14*/{ 64, 64, 64, 64, 64, 64, 64, 64,  0, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64  },
};

int map_data2[5][5] =
{
    /*     00  01  02  03  04  05  06  07  08  09  10  11  12  13  14  15  16  17  18  19   */
    /*00*/{ 64, 64, 64, 64, 64, },
    /*01*/{ 64,  1,  1,  1,  1, },
    /*02*/{ 64,128,128,  1,  1, },
    /*03*/{ 64,  1,128,  1,  1, },
    /*04*/{ 64,  1,128,  1,  1, },
 
};

GroundBlock::GroundBlock(std::shared_ptr<GeometricPrimitive>& primitive)
{
    count = 0;
    type = Block::D_block;
    pos = { 0.0,-1.0f,0.0f };

    obj = primitive;
}

void GroundBlock::Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& wvp, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& light_dir, const DirectX::XMFLOAT4& materialColor, bool wireframe)
{
    obj->render(context,wvp,world,light_dir,materialColor,wireframe);
}

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
}

void GroundBlockManager::Update()
{
}

void GroundBlockManager::Render(ID3D11DeviceContext* context, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir, bool wireframe)
{
    static int count = 0;
    for (int y = 0; y < mapY; y++) {
        for (int x = 0; x < mapX; x++) {
            if (mapData[y][x] < 1) continue;
            DirectX::XMMATRIX W, S, R, T;
            DirectX::XMFLOAT4X4 wvp;
            DirectX::XMFLOAT4X4 world;

            S = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
            R = DirectX::XMMatrixRotationRollPitchYaw(0, 0, 0);
            T = DirectX::XMMatrixTranslation(x * 1.0f, 0, y * 1.0f);
            W = S * R * T;
            // ★MATRIX -> FLOAT4X4
            DirectX::XMStoreFloat4x4(&world, W);
            DirectX::XMStoreFloat4x4(&wvp, W * view * projection);
            DirectX::XMFLOAT4 material_color = { 0.0f,0.6f,0.5f,1.0f };

            obj[count]->Render(context,wvp, world, light_dir, material_color, wireframe);
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
