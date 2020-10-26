#pragma once

#include <list>
#include "vector.h"

class SaveData
{
private:

public:
	FLOAT3 plPos; // ���̎��̎��@�̈ʒu
	int thatCount; // ���̎��̎��@�̑����̃u���b�N�̃J�E���g
};

class SaveDataManager
{
private:
	SaveDataManager() {};
public:
	static  SaveDataManager* getInstance()
	{
		static SaveDataManager instance;
		return &instance;
	}

	std::list<SaveData>SaveList;

	~SaveDataManager();

	void init(); // ���X�g�̒��g��������
	void update(FLOAT3 playerPos); // gameScene�ɒ��ڏ����������Ă���̂Ŏg���K�v������܂���

	void uninit(); // ���X�g�̒��g��������

	FLOAT3 SaveLoad(FLOAT3 playerPos); // ���X�g�̒��g�����݂ɔ��f ��߂�
	SaveData* add(FLOAT3 pos, int count); // ���X�g�Ƀf�[�^��ۑ�����
	std::list<SaveData>* getList() { return  &SaveList; }

};
