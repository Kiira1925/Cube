#pragma once
#include "ground_blocks.h"
#include "vector.h"
#include <memory>

bool hover(FLOAT3 PlayerPos, FLOAT3 BlockPos);

bool Front(FLOAT3 playerPos, std::shared_ptr<GroundBlockManager> obj);
bool Back(FLOAT3 playerPos, std::shared_ptr<GroundBlockManager> obj);
bool Right(FLOAT3 playerPos, std::shared_ptr<GroundBlockManager> obj);
bool Left(FLOAT3 playerPos, std::shared_ptr<GroundBlockManager> obj);
