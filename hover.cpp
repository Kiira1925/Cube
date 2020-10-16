#include "hover.h"

bool hover(FLOAT3 PlayerPos, FLOAT3 BlockPos)
{
    if (PlayerPos.x > BlockPos.x - 0.1f && PlayerPos.x < BlockPos.x + 0.1f
        && PlayerPos.z > BlockPos.z - 0.1f && PlayerPos.z < BlockPos.z + 0.1f)
    {
        return true;
    }
    return false;
}
