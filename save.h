#pragma once

#include <list>
#include "vector.h"

class SaveData
{
private:

public:
	FLOAT3 plPos;
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

	void init();
	void update(FLOAT3 playerPos); // I‚ð‰Ÿ‚µ‚½‚ç–ß‚é‚æ‚¤‚É‚µ‚Ü‚·

	void uninit();

	FLOAT3 SaveLoad(FLOAT3 playerPos);
	SaveData* add(FLOAT3 pos);
	std::list<SaveData>* getList() { return  &SaveList; }

};
