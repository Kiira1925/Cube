#include "pause.h"
#include "scene.h"

void PauseMenu::Initialize()
{
	//sprites[0] = std::make_unique<Sprite>(pFramework->getDevice(), L"./Data/Menu/eli.jpg");
	pause_flg = false;
	cursor = CONTINUE;
}

bool PauseMenu::Update()
{
	if (GetAsyncKeyState('P') & 1) { switch_pause(); }
	if (!pause_flg) return false;
	if (GetAsyncKeyState(VK_DOWN) & 1)
	{
		cursor++; 
		pFramework->soundSE[0]->Play(false);
	}
	if (GetAsyncKeyState(VK_UP) & 1)
	{
		cursor--;
		pFramework->soundSE[0]->Play(false);
	}

	if (cursor >= ITEM_MAX) { cursor = CONTINUE; }
	if (cursor < CONTINUE) { cursor = END; }

	switch (cursor)
	{
	case CONTINUE:
		if (GetAsyncKeyState(VK_RETURN) & 1) { switch_pause(); }
		break;
	case RESTART:
		// ステージリセット処理
		if (GetAsyncKeyState(VK_RETURN) & 1)
		{
			switch_pause();
			SceneGame::getInstance()->Reload(SceneManager::Instance().GetStageNum());
		}
		break;
	case RETURN_TO_STAGESELECT:
		if (GetAsyncKeyState(VK_RETURN) & 1)
		{
			switch_pause();
			SceneManager::Instance().ChangeScene(SceneSelect::getInstance());
		}
		break;
	case RETURN_TO_TITLE:
		if (GetAsyncKeyState(VK_RETURN) & 1)
		{
			switch_pause();
			SceneManager::Instance().ChangeScene(SceneTitle::getInstance());
		}
		break;
	case END:
		if (GetAsyncKeyState(VK_RETURN) & 1)
		{
			SendMessage(pFramework->hwnd, WM_CLOSE, NULL, NULL);
		}
		break;
	case ITEM_MAX:
		cursor = CONTINUE;
	}
	return true;
}

void PauseMenu::Draw()
{
	if (!pause_flg) return;

	blender::Set(blender::BS_ALPHA);
	pFramework->sprites[0]->render(pFramework->getDeviceContext(), 0, 0, 1920, 1080, 0, 0, 1920, 1080, 0, { 1,1,1,0.5f });
	pFramework->sprites[1]->render(pFramework->getDeviceContext(), 617, 137, 686, 806, 0, 0, 686, 806, 0, { 1,1,1,1.0f });
	pFramework->sprites[2]->render(pFramework->getDeviceContext(), 728, 340+(100*cursor), 501, 108, 0, 0, 590, 126, 0, { 1,1,1,1.0f });
	blender::Set(blender::BS_NONE);
	//sprites[0]->render(pFramework->getDeviceContext(), 0, 0, 0, 0, 0, 0, 400, 400, 0, { 1,1,1,1 });
}