#pragma once
#include "geometric_primitive.h"
#include "vector.h"
#include <string>
#include <memory>

// 各ブロックをenumで管理することでプログラム内でもわかりやすくします。
enum Block
{
    N_block = -1, // ブロックを置かない場所
    I_block,      // 破壊不可ブロック
    D_block_1,    // 破壊可能ブロック
    D_block_2,    // 破壊可能ブロック
    D_block_3,    // 破壊可能ブロック
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
    bool hoverflg;
    bool oldhover;
    bool leaveflg;

    std::shared_ptr<GeometricPrimitive> obj;

public:
    GroundBlock(std::shared_ptr<GeometricPrimitive>& primitive);
    ~GroundBlock() {}
    void Update();
    void Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& wvp, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& light_dir, const DirectX::XMFLOAT4& materialColor, bool wireframe);

public: // 各ブロックの処理関数
    void DestroyBlock();
    void SelectBlock();

public: // Set関数
    void SetType(int type) { this->type = type , this->count = type; }
    void SetBlockPosXZ(int x, int z) { this->pos.x = x, this->pos.z = z; }
    void   SetHover(bool flg) { oldhover = hoverflg, hoverflg = flg; }

public:
   FLOAT3 GetPosition() { return pos; }

};

// 地面ブロック管理
// マップ配置に関してもこのマネジャーがやる
class GroundBlockManager
{
private:
    static const int objMaX = 1000;
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
public:
    void SetBlockHover(int num, bool flg) { obj[num]->SetHover(flg); }

public:
    int  GetMapX() { return mapX; }
    int  GetMapY() { return mapY; }
    int  GetMea() { return mea; }
    FLOAT3 GetBlockPos(int num) { return obj[num]->GetPosition(); }

    static GroundBlockManager* getInstance()
    {
        static GroundBlockManager instance;
        return &instance;
    }

};

#define GBManager GroundBlockManager::getInstance()