#pragma once

class Game;

class GameComponent
{
public:
	GameComponent(Game* game);
	virtual ~GameComponent();

	virtual void Initialize() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Draw() = 0;
	virtual void DestroyResources() = 0;

protected:
	Game* game;
};
