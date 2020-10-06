#pragma once
#include "geometric_primitive.h"

// 地面ブロック
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

// 地面ブロック管理
class GroundBlockManager
{
    GroundBlock obj[100];
};