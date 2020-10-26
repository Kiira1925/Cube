#include "hover.h"

bool hover(FLOAT3 PlayerPos, FLOAT3 BlockPos)
{
    if (PlayerPos.x > BlockPos.x - 0.1f && PlayerPos.x < BlockPos.x + 0.1f
        && PlayerPos.z > BlockPos.z - 0.1f && PlayerPos.z < BlockPos.z + 0.1f && PlayerPos.y <=0.0f)
    {
        return true;
    }
    return false;
}

bool Front(FLOAT3 playerPos, GroundBlockManager* obj)
{
    for (int i = 0; i < obj->GetMea(); i++)
    {
        if ((playerPos.z + 1.0f) > (obj->GetBlockPos(i).z - 0.2f ))
        {
            if ((playerPos.z + 1.0f) < (obj->GetBlockPos(i).z + 0.2f))
            {
                int a = 0;
                if (playerPos.x > (obj->GetBlockPos(i).x - 0.2f))
                {
                    int c = 0;
                    if (playerPos.x < (obj->GetBlockPos(i).x + 0.2f))
                    {
                        if (obj->GetBlockObj(i)->GetType() == -1 || obj->GetBlockObj(i)->checkExist())
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool Back(FLOAT3 playerPos, GroundBlockManager* obj)
{
    for (int i = 0; i < obj->GetMea(); i++)
    {
        if ((playerPos.z - 1.0f) > (obj->GetBlockPos(i).z - 0.2f))
        {
            if ((playerPos.z - 1.0f) < (obj->GetBlockPos(i).z + 0.2f))
            {
                int a = 0;
                if (playerPos.x > (obj->GetBlockPos(i).x - 0.2f))
                {
                    int c = 0;
                    if (playerPos.x < (obj->GetBlockPos(i).x + 0.2f))
                    {
                        if (obj->GetBlockObj(i)->GetType() == -1 || obj->GetBlockObj(i)->checkExist())
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool Right(FLOAT3 playerPos, GroundBlockManager* obj)
{
    for (int i = 0; i < obj->GetMea(); i++)
    {
        if ((playerPos.x + 1.0f) > (obj->GetBlockPos(i).x - 0.2f))
        {
            if ((playerPos.x + 1.0f) < (obj->GetBlockPos(i).x + 0.2f))
            {
                int a = 0;
                if (playerPos.z > (obj->GetBlockPos(i).z - 0.2f))
                {
                    int c = 0;
                    if (playerPos.z < (obj->GetBlockPos(i).z + 0.2f))
                    {
                        if (obj->GetBlockObj(i)->GetType() == -1 || obj->GetBlockObj(i)->checkExist())
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool Left(FLOAT3 playerPos, GroundBlockManager* obj)
{
    for (int i = 0; i < obj->GetMea(); i++)
    {
        if ((playerPos.x - 1.0f) > (obj->GetBlockPos(i).x - 0.2f))
        {
            if ((playerPos.x - 1.0f) < (obj->GetBlockPos(i).x + 0.2f))
            {
                int a = 0;
                if (playerPos.z > (obj->GetBlockPos(i).z - 0.2f))
                {
                    int c = 0;
                    if (playerPos.z < (obj->GetBlockPos(i).z + 0.2f))
                    {
                        if (obj->GetBlockObj(i)->GetType() == -1 || obj->GetBlockObj(i)->checkExist())
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}