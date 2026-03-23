#pragma once

#include <windows.h>
#include <wrl.h>
#include <d3d11.h>
#include <vector>
#include <memory>
#include <chrono>

class DisplayWin32;
class InputDevice;
class GameComponent;

class Game
{
public:
	Game(LPCWSTR appName, int screenWidth, int screenHeight);
	virtual ~Game();

	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;

	void Init();
	void Run();

	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetDeviceContext() const;
	IDXGISwapChain* GetSwapChain() const;
	DisplayWin32* GetDisplay() const;
	InputDevice* GetInputDevice() const;
	int GetScreenWidth() const;
	int GetScreenHeight() const;
	float GetTotalTime() const;

	void AddComponent(std::unique_ptr<GameComponent> component);

	LRESULT MessageHandler(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

protected:
	virtual void PrepareResources();
	virtual void Update(float deltaTime);
	virtual void Draw();
	void PrepareFrame();
	void CreateBackBuffer();
	void RestoreTargets();
	void DestroyResources();

	LPCWSTR name_;
	int screenWidth_;
	int screenHeight_;

	std::unique_ptr<DisplayWin32> display_;
	std::unique_ptr<InputDevice> input_;

	Microsoft::WRL::ComPtr<ID3D11Device> device_;
	ID3D11DeviceContext* context_;
	IDXGISwapChain* swapChain_;
	ID3D11Texture2D* backBuffer_;
	ID3D11RenderTargetView* renderView_;

	std::vector<std::unique_ptr<GameComponent>> components_;

	std::chrono::time_point<std::chrono::steady_clock> startTime_;
	std::chrono::time_point<std::chrono::steady_clock> previousTime_;
	float totalTime_;
	unsigned int frameCount_;
	bool isExitRequested_;
};
