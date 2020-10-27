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
	if (SaveList.size() == 0)return  playerPos;
	auto iter = SaveList.rbegin();

	// Zが必ず - の値になるので先に + の値に変えないと違うブロックが対象になる
	int Z = playerPos.z * GBManager->GetMapX();
	int nowPos = playerPos.x + -Z;

	int pZ = iter->plPos.z * GBManager->GetMapX();
	int pastPos = iter->plPos.x + -pZ;
	
	if (pastPos < 0)pastPos *= -1;

	// 今いる位置のブロックのフラグ関連をリセットする( バグが治りました )
	GroundBlockManager::getInstance()->GetBlockObj(nowPos)->ReSetHover();
	// ひとつ前の場所のカウントを戻して置く
	GroundBlockManager::getInstance()->GetBlockObj(pastPos)->count = iter->thatCount;
	// フラグ関連を浮いているブロックと同じに戻す
	GroundBlockManager::getInstance()->GetBlockObj(pastPos)->ReSetHover();
	// 他の浮いているブロックと同じ位置まで上に戻す
	GroundBlockManager::getInstance()->GetBlockObj(pastPos)->SetBlockPosY(-1);
	// 自機の位置を変更するのが困難だったので返り値で渡すことにする ( getIntance で中身を変えたい )
	FLOAT3 pos = iter->plPos;
	SaveList.pop_back();
	return pos;
}

SaveData* SaveDataManager::add(FLOAT3 pos,int count)
{
	SaveData save;
	save.plPos = pos;
	save.thatCount = count;

	SaveList.push_back(save);

	return &(*SaveList.rbegin());

}

SaveDataManager::~SaveDataManager()
{
	SaveList.clear();
}
