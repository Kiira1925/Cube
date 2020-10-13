#pragma once
#include "geometric_primitive.h"
#include "vector.h"
#include <string>
#include <memory>

// �e�u���b�N��enum�ŊǗ����邱�ƂŃv���O�������ł��킩��₷�����܂��B
enum Block
{
    N_block = -1, // �u���b�N��u���Ȃ��ꏊ
    D_block,    // �j��\�u���b�N
    I_block,    // �j��s�u���b�N
    C_block,    // �Z���N�g�u���b�N
    S_block,    // �X�^�[�g�u���b�N
};

// �n�ʃu���b�N
class GroundBlock
{
private:
    int count; // ������܂ł̃J�E���g
    int type;
    FLOAT3 pos;


    std::shared_ptr<GeometricPrimitive> obj;

public:
    GroundBlock(std::shared_ptr<GeometricPrimitive>& primitive);
    ~GroundBlock() {}
    //void Update();
    void Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& wvp, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& light_dir, const DirectX::XMFLOAT4& materialColor, bool wireframe);

    bool StartFlg(int type) {} // �X�^�[�g�u���b�N�ɂ��邩
};

// �n�ʃu���b�N�Ǘ�
// �}�b�v�z�u�Ɋւ��Ă����̃}�l�W���[�����
class GroundBlockManager
{
private:
    static const int objMaX = 100;
    std::shared_ptr<GroundBlock> obj[objMaX];

    // �}�b�v�f�[�^�擾
    char** mapData;
    uint32_t mapX, mapY;
    int mea; // �n���C��Ō�

    int stageNum;               //  �X�e�[�W�Ǘ�
    char fileNameStage[256];    // ���O�Ǎ��ݗp

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