#pragma once
#include "geometric_primitive.h"
#include "SkinnedCube.h"
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
    G_block,    // ゴールブロック
    S_01_block, // ステージ1側ブロック
    S_10_block, // ステージ10側ブロック
    T_block,    // タイトルブロック

    Stage1 = 10,
    Stage2,
    Stage3,
    Stage4,
    Stage5,
    Stage6,
    Stage7,
    Stage8,
    Stage9,
    Stage10,
};

// 地面ブロック
class GroundBlock
{
private:
    FLOAT3 pos;
    bool hoverflg;
    bool oldhover;
    bool leaveflg;

    std::shared_ptr<SkinnedCube> obj;

public:
    int count; // 落ちるまでのカウント
    int type;
    GroundBlock(std::shared_ptr<SkinnedCube>& primitive);
    ~GroundBlock() {}
    void Update();
    void Render(ID3D11DeviceContext* context,const DirectX::XMFLOAT4X4& wvp, const DirectX::XMFLOAT4X4& world);

public: // 各ブロックの処理関数
    void DestroyBlock();
    void SelectBlock();

    void Select01Block();
    void Select10Block();
    void TitleBlock();

    void StartBlock();
    void GoalBlock() {}
    void StegeBlock(int num);

public: // Set関数
    void SetType(int type) { this->type = type; if (type > 0 && type < 4)this->count = type; }
    void SetBlockPosXZ(int x, int z) { this->pos.x = x, this->pos.z = z; }
    void SetHover(bool flg) { oldhover = hoverflg, hoverflg = flg; }

public:
   FLOAT3 GetPosition() { return pos; }
   int    GetType() {return type;}
   int    GetCount() { return count;}

public:
    bool checkExist();
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
    bool isSelect01;

    void Initialize(int x, int y, std::shared_ptr<SkinnedCube>& primitive);
    void Initialize();
    void Update();
    void Render(ID3D11DeviceContext* context, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);
    void Relese();

public:
    void SetPrimitive(std::shared_ptr<SkinnedCube> primitive);
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
    std::shared_ptr<GroundBlock> GetBlockObj(int num) { return obj[num]; }
    int GetCount(int num) { return obj[num]->GetCount(); }
    bool checkBlockExist();
    

    static GroundBlockManager* getInstance()
    {
        static GroundBlockManager instance;
        return &instance;
    }

};

#define GBManager GroundBlockManager::getInstance()