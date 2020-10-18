#pragma once
#include <queue>
#include "sprite.h"

class Debug :public Sprite
{
private:
	std::queue<std::string> debugQueue;
	float posY;
	int screenHeight;

public:
	// デバイス　画像ファイルのパス　最大表示可能文字数　スクリーンの高さ
	Debug(ID3D11Device*, const wchar_t*);
	~Debug();

	void Debug::setString(const char* format, ...);

	void display(ID3D11DeviceContext*, float r = (1), float g = (1), float b = (1), float sx = (1), float sy = (1));
};
