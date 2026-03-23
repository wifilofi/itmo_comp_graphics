#pragma once

#include <unordered_set>

class Game;

class InputDevice
{
public:
	InputDevice(Game* game);

	void OnKeyDown(unsigned int keycode);
	void OnKeyUp(unsigned int keycode);
	void OnMouseMove(int positionX, int positionY);

	bool IsKeyDown(unsigned int keycode) const;
	int GetMousePositionX() const;
	int GetMousePositionY() const;

private:
	Game* game;
	std::unordered_set<unsigned int> pressedKeys;
	int mousePositionX;
	int mousePositionY;
};
