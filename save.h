#pragma once

#include <list>
#include "vector.h"

class SaveData
{
private:

public:
	FLOAT3 plPos; // その時の自機の位置
	int thatCount; // その時の自機の足元のブロックのカウント
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

	void init(); // リストの中身を初期化
	void update(FLOAT3 playerPos); // gameSceneに直接処理をかいているので使う必要がありません

	void uninit(); // リストの中身を初期化

	FLOAT3 SaveLoad(FLOAT3 playerPos); // リストの中身を現在に反映 一つ戻る
	SaveData* add(FLOAT3 pos, int count); // リストにデータを保存する
	std::list<SaveData>* getList() { return  &SaveList; }

};
