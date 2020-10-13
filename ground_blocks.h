#pragma once
#include "geometric_primitive.h"
#include "vector.h"
#include <string>
#include <memory>

// 各ブロックをenumで管理することでプログラム内でもわかりやすくします。
enum Block
{
    N_block = -1, // ブロックを置かない場所
    D_block,    // 破壊可能ブロック
    I_block,    // 破壊不可ブロック
    C_block,    // セレクトブロック
    S_block,    // スタートブロック
};

// 地面ブロック
class GroundBlock
{
private:
    int count; // 落ちるまでのカウント
    int type;
    FLOAT3 pos;


    std::shared_ptr<GeometricPrimitive> obj;

public:
    GroundBlock(std::shared_ptr<GeometricPrimitive>& primitive);
    ~GroundBlock() {}
    //void Update();
    void Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& wvp, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& light_dir, const DirectX::XMFLOAT4& materialColor, bool wireframe);

    bool StartFlg(int type) {} // スタートブロックにいるか
};

// 地面ブロック管理
// マップ配置に関してもこのマネジャーがやる
class GroundBlockManager
{
private:
    static const int objMaX = 100;
    std::shared_ptr<GroundBlock> obj[objMaX];

    // マップデータ取得
    char** mapData;
    uint32_t mapX, mapY;
    int mea; // ハワイ語で個数

    int stageNum;               //  ステージ管理
    char fileNameStage[256];    // 名前読込み用

public:
    void Initialize(int x, int y, std::shared_ptr<GeometricPrimitive>& primitive);
    void Initialize();
    void Update();
    void Render(ID3D11DeviceContext* context, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir, bool wireframe);
    void Relese();

public:
    void SetPrimitive(std::shared_ptr<GeometricPrimitive> primitive);
    void SetStageNum(int stage) { stageNum = stage; }
    bool LoadMapData(const char* fileName, char** map);
    bool GetCsvSize(std::string path, uint32_t* mapX, uint32_t* mapY);
};