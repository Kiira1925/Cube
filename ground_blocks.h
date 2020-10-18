#pragma once
#include "geometric_primitive.h"
#include "SkinndeCube.h"
#include "vector.h"
#include <string>
#include <memory>

// �e�u���b�N��enum�ŊǗ����邱�ƂŃv���O�������ł��킩��₷�����܂��B
enum Block
{
    N_block = -1, // �u���b�N��u���Ȃ��ꏊ
    I_block,      // �j��s�u���b�N
    D_block_1,    // �j��\�u���b�N
    D_block_2,    // �j��\�u���b�N
    D_block_3,    // �j��\�u���b�N
    C_block,    // �Z���N�g�u���b�N
    S_block,    // �X�^�[�g�u���b�N
    G_block,    // �S�[���u���b�N

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

// �n�ʃu���b�N
class GroundBlock
{
private:
    int count; // ������܂ł̃J�E���g
    int type;
    FLOAT3 pos;
    bool hoverflg;
    bool oldhover;
    bool leaveflg;

    std::shared_ptr<SkinndeCube> obj;

public:
    GroundBlock(std::shared_ptr<SkinndeCube>& primitive);
    ~GroundBlock() {}
    void Update();
    void Render(ID3D11DeviceContext* context,const DirectX::XMFLOAT4X4& wvp, const DirectX::XMFLOAT4X4& world);

public: // �e�u���b�N�̏����֐�
    void DestroyBlock();
    void SelectBlock();
    void StartBlock();
    void GoalBlock() {}
    void StegeBlock(int num);

public: // Set�֐�
    void SetType(int type) { this->type = type; if (type > 0 && type < 4)this->count = type; }
    void SetBlockPosXZ(int x, int z) { this->pos.x = x, this->pos.z = z; }
    void   SetHover(bool flg) { oldhover = hoverflg, hoverflg = flg; }

public:
   FLOAT3 GetPosition() { return pos; }
   int    GetType() { return type; }
};

// �n�ʃu���b�N�Ǘ�
// �}�b�v�z�u�Ɋւ��Ă����̃}�l�W���[�����
class GroundBlockManager
{
private:
    static const int objMaX = 1000;
    std::shared_ptr<GroundBlock> obj[objMaX];

    // �}�b�v�f�[�^�擾
    char** mapData;
    uint32_t mapX, mapY;
    int mea; // �n���C��Ō�

    int stageNum;               //  �X�e�[�W�Ǘ�
    char fileNameStage[256];    // ���O�Ǎ��ݗp

public:
    void Initialize(int x, int y, std::shared_ptr<SkinndeCube>& primitive);
    void Initialize();
    void Update();
    void Render(ID3D11DeviceContext* context, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);
    void Relese();

public:
    void SetPrimitive(std::shared_ptr<SkinndeCube> primitive);
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

    static GroundBlockManager* getInstance()
    {
        static GroundBlockManager instance;
        return &instance;
    }

};

#define GBManager GroundBlockManager::getInstance()