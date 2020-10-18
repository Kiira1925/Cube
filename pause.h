#pragma once
#include "framework.h"
#include "sprite.h"
#include "blender.h"

enum MENU_ITEM { CONTINUE, RESTART, RETURN_TO_STAGESELECT, RETURN_TO_TITLE, END, ITEM_MAX };

class PauseMenu
{
private:
	//std::unique_ptr<Sprite> sprites[32];
	int cursor;
	int timer;
	
	bool pause_flg;

public:
	PauseMenu() = default;
	~PauseMenu() = default;

	void Initialize();
	bool Update();
	void Draw();
	void switch_pause() { pause_flg = !pause_flg; pFramework->soundSE[1]->Play(false); }

	static PauseMenu* getInstance()
	{
		static PauseMenu instance;
		return &instance;
	}
};
#define pPause PauseMenu::getInstance()