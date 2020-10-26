#include "framework.h"
#include "player.h"
#include "ground_blocks.h"

#include "save.h"
#include "debug.h"

void SaveDataManager::init()
{
	SaveList.clear();
}

void SaveDataManager::update(FLOAT3 playerPos)
{
	static bool flg, o_flg;
	flg = false;
	if (SaveList.empty())
	{
		framework::getInstance()->debug->setString("karakarakara!!!!!!!!!!!!!");
	}
	if (GetAsyncKeyState('I'))
	{
		flg = true;
		if (flg != o_flg)
		{
			SaveLoad(playerPos);
		}
	}
	o_flg = flg;

}

void SaveDataManager::uninit()
{
	SaveList.clear();
}

FLOAT3 SaveDataManager::SaveLoad(FLOAT3 playerPos)
{
	auto iter = SaveList.rbegin();

	int Z = playerPos.z * GBManager->GetMapX();
	int nowPos = playerPos.x + -Z;

	int pZ = iter->plPos.z * GBManager->GetMapX();
	int pastPos = iter->plPos.x + -pZ;
	
	if (pastPos < 0)pastPos *= -1;

	GroundBlockManager::getInstance()->GetBlockObj(nowPos)->count += 1;
	GroundBlockManager::getInstance()->GetBlockObj(pastPos)->count += 1;
	GroundBlockManager::getInstance()->GetBlockObj(pastPos)->ReSetHover();
	GroundBlockManager::getInstance()->GetBlockObj(pastPos)->SetBlockPosY(-1);
	FLOAT3 pos = iter->plPos;
	SaveList.pop_back();
	return pos;
}

SaveData* SaveDataManager::add(FLOAT3 pos)
{
	SaveData save;
	save.plPos = pos;

	SaveList.push_back(save);

	return &(*SaveList.rbegin());

}

SaveDataManager::~SaveDataManager()
{
	SaveList.clear();
}
