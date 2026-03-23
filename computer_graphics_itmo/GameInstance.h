/*#pragma once

#include <windows.h>
#include <wrl.h>
#include <d3d11.h>
#include <vector>
#include <memory>
#include <chrono>


class GameInstance
{
public:
	GameInstance(int screenWidth, int screenHeight, LPCWSTR appName);

protected:
	virtual void Update(float deltaTime);
	virtual void Draw();
	void CreateBackBuffer();

	LPCWSTR _name;
	int _screenWidth;
	int _screenHeight;
	ID3D11DeviceContext* _context;
	IDXGISwapChain* _swapChain;
	ID3D11Texture2D* _backBuffer;
	ID3D11RenderTargetView* _renderView;
	float _totalTime;
	unsigned int _frameCount;
	bool _isExitRequested;

};*/