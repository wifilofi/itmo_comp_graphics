#include "Headers/InputDevice.h"

InputDevice::InputDevice(Game* GameInstance)
	: game(GameInstance)
	, mousePositionX(0)
	, mousePositionY(0)
{
}

void InputDevice::OnKeyDown(unsigned int KeyCode)
{
	pressedKeys.insert(KeyCode);
}

void InputDevice::OnKeyUp(unsigned int KeyCode)
{
	pressedKeys.erase(KeyCode);
}

void InputDevice::OnMouseMove(int PositionX, int PositionY)
{
	mousePositionX = PositionX;
	mousePositionY = PositionY;
}

bool InputDevice::IsKeyDown(unsigned int KeyCode) const
{
	return pressedKeys.count(KeyCode) > 0;
}

int InputDevice::GetMousePositionX() const
{
	return mousePositionX;
}

int InputDevice::GetMousePositionY() const
{
	return mousePositionY;
}
