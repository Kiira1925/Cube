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

	// Z���K�� - �̒l�ɂȂ�̂Ő�� + �̒l�ɕς��Ȃ��ƈႤ�u���b�N���ΏۂɂȂ�
	int Z = playerPos.z * GBManager->GetMapX();
	int nowPos = playerPos.x + -Z;

	int pZ = iter->plPos.z * GBManager->GetMapX();
	int pastPos = iter->plPos.x + -pZ;
	
	if (pastPos < 0)pastPos *= -1;

	// ������ʒu�̃u���b�N�̃t���O�֘A�����Z�b�g����( �o�O������܂��� )
	GroundBlockManager::getInstance()->GetBlockObj(nowPos)->ReSetHover();
	// �ЂƂO�̏ꏊ�̃J�E���g��߂��Ēu��
	GroundBlockManager::getInstance()->GetBlockObj(pastPos)->count = iter->thatCount;
	// �t���O�֘A�𕂂��Ă���u���b�N�Ɠ����ɖ߂�
	GroundBlockManager::getInstance()->GetBlockObj(pastPos)->ReSetHover();
	// ���̕����Ă���u���b�N�Ɠ����ʒu�܂ŏ�ɖ߂�
	GroundBlockManager::getInstance()->GetBlockObj(pastPos)->SetBlockPosY(-1);
	// ���@�̈ʒu��ύX����̂���������̂ŕԂ�l�œn�����Ƃɂ��� ( getIntance �Œ��g��ς����� )
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
