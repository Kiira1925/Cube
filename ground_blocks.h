#pragma once
#include "geometric_primitive.h"

// �n�ʃu���b�N
class GroundBlock
{
private:
    int count;
    int type;
    GeometricPrimitive obj;

public:
    void Initialize();
    void Update();
    void Render();


};

// �n�ʃu���b�N�Ǘ�
class GroundBlockManager
{
    GroundBlock obj[100];
};