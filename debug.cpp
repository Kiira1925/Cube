#include "debug.h"
#include "framework.h"

Debug::Debug(ID3D11Device* device,
	const wchar_t* fileName)
    : Sprite(device, fileName), posY(0), screenHeight(SCREEN_HEIGHT)
{
}

Debug::~Debug()
{

}

void Debug::setString(const char* format,...)
{
	char str[256];
	vsprintf_s(str, format, (char*)(&format + 1));
	debugQueue.push(str);
}

void Debug::display(ID3D11DeviceContext* context,
	float r, float g, float b, float sx, float sy)
{
	posY = 0;
	while (!debugQueue.empty())
	{
		std::string t = debugQueue.front();
		posY += textoutD(context, t, 0, posY, sx, sy, r, g, b, 1);
		debugQueue.pop();

		if (posY > screenHeight)
		{
			while (!debugQueue.empty()) debugQueue.pop();
			break;
		}

	}
}
