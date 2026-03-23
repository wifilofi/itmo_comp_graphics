#include "Headers/Game.h"
#include "Headers/DisplayWin32.h"
#include "Headers/InputDevice.h"
#include "Headers/GameComponent.h"
#include <iostream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

Game::Game(LPCWSTR appName, int screenWidth, int screenHeight)
	: name_(appName)
	, screenWidth_(screenWidth)
	, screenHeight_(screenHeight)
	, context_(nullptr)
	, swapChain_(nullptr)
	, backBuffer_(nullptr)
	, renderView_(nullptr)
	, totalTime_(0.0f)
	, frameCount_(0)
	, isExitRequested_(false)
{
}

Game::~Game()
{
	DestroyResources();
}

void Game::Init()
{
	HINSTANCE InstanceHandle = GetModuleHandle(nullptr);

	display_ = std::make_unique<DisplayWin32>(
		name_,
		InstanceHandle,
		screenWidth_,
		screenHeight_,
		[this](HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam) -> LRESULT
		{
			return this->MessageHandler(WindowHandle, Message, WParam, LParam);
		});

	input_ = std::make_unique<InputDevice>(this);

	D3D_FEATURE_LEVEL FeatureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

	DXGI_SWAP_CHAIN_DESC SwapDescription = {};
	SwapDescription.BufferCount = 2;
	SwapDescription.BufferDesc.Width = screenWidth_;
	SwapDescription.BufferDesc.Height = screenHeight_;
	SwapDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapDescription.BufferDesc.RefreshRate.Numerator = 60;
	SwapDescription.BufferDesc.RefreshRate.Denominator = 1;
	SwapDescription.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapDescription.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapDescription.OutputWindow = display_->GetWindowHandle();
	SwapDescription.Windowed = true;
	SwapDescription.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapDescription.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	SwapDescription.SampleDesc.Count = 1;
	SwapDescription.SampleDesc.Quality = 0;

	auto Result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		FeatureLevel,
		1,
		D3D11_SDK_VERSION,
		&SwapDescription,
		&swapChain_,
		&device_,
		nullptr,
		&context_);

	if (FAILED(Result))
	{
		std::cerr << "Failed to create D3D11 device and swap chain." << std::endl;
		return;
	}

	CreateBackBuffer();
	PrepareResources();

	for (auto& Component : components_)
	{
		Component->Initialize();
	}
}

void Game::Run()
{
	startTime_ = std::chrono::steady_clock::now();
	previousTime_ = startTime_;

	MSG Message = {};
	while (!isExitRequested_)
	{
		while (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		if (Message.message == WM_QUIT)
		{
			isExitRequested_ = true;
		}

		auto currentTime = std::chrono::steady_clock::now();
		float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - previousTime_).count() / 1000000.0f;
		previousTime_ = currentTime;

		totalTime_ += deltaTime;
		frameCount_++;

		if (totalTime_ > 1.0f)
		{
			float FramesPerSecond = frameCount_ / totalTime_;
			totalTime_ -= 1.0f;

			WCHAR TitleText[256];
			swprintf_s(TitleText, TEXT("FPS: %f"), FramesPerSecond);
			SetWindowText(display_->GetWindowHandle(), TitleText);

			frameCount_ = 0;
		}

		PrepareFrame();
		Update(deltaTime);
		Draw();
		swapChain_->Present(1, 0);
	}
}

void Game::PrepareFrame()
{
	context_->ClearState();
	RestoreTargets();

	D3D11_VIEWPORT Viewport = {};
	Viewport.Width = static_cast<float>(screenWidth_);
	Viewport.Height = static_cast<float>(screenHeight_);
	Viewport.TopLeftX = 0;
	Viewport.TopLeftY = 0;
	Viewport.MinDepth = 0;
	Viewport.MaxDepth = 1.0f;

	context_->RSSetViewports(1, &Viewport);
}

void Game::Update(float deltaTime)
{
	for (auto& Component : components_)
	{
		Component->Update(deltaTime);
	}
}

void Game::Draw()
{
	float ClearColor[] = { totalTime_, 0.1f, 0.1f, 1.0f };
	context_->ClearRenderTargetView(renderView_, ClearColor);

	for (auto& Component : components_)
	{
		Component->Draw();
	}

	context_->OMSetRenderTargets(0, nullptr, nullptr);
}

void Game::PrepareResources()
{
}

void Game::CreateBackBuffer()
{
	swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer_));
	device_->CreateRenderTargetView(backBuffer_, nullptr, &renderView_);
}

void Game::RestoreTargets()
{
	context_->OMSetRenderTargets(1, &renderView_, nullptr);
}

void Game::DestroyResources()
{
	for (auto& Component : components_)
	{
		Component->DestroyResources();
	}

	if (renderView_)
	{
		renderView_->Release();
		renderView_ = nullptr;
	}

	if (backBuffer_)
	{
		backBuffer_->Release();
		backBuffer_ = nullptr;
	}

	if (swapChain_)
	{
		swapChain_->Release();
		swapChain_ = nullptr;
	}

	if (context_)
	{
		context_->Release();
		context_ = nullptr;
	}
}

ID3D11Device* Game::GetDevice() const
{
	return device_.Get();
}

ID3D11DeviceContext* Game::GetDeviceContext() const
{
	return context_;
}

IDXGISwapChain* Game::GetSwapChain() const
{
	return swapChain_;
}

DisplayWin32* Game::GetDisplay() const
{
	return display_.get();
}

InputDevice* Game::GetInputDevice() const
{
	return input_.get();
}

int Game::GetScreenWidth() const
{
	return screenWidth_;
}

int Game::GetScreenHeight() const
{
	return screenHeight_;
}

float Game::GetTotalTime() const
{
	return totalTime_;
}

void Game::AddComponent(std::unique_ptr<GameComponent> Component)
{
	components_.push_back(std::move(Component));
}

LRESULT Game::MessageHandler(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam)
{
	switch (Message)
	{
	case WM_KEYDOWN:
	{
		unsigned int KeyCode = static_cast<unsigned int>(WParam);
		std::cout << "Key: " << KeyCode << std::endl;

		if (input_)
		{
			input_->OnKeyDown(KeyCode);
		}

		if (KeyCode == 27)
		{
			PostQuitMessage(0);
		}

		return 0;
	}
	case WM_KEYUP:
	{
		if (input_)
		{
			input_->OnKeyUp(static_cast<unsigned int>(WParam));
		}
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		if (input_)
		{
			int PositionX = LOWORD(LParam);
			int PositionY = HIWORD(LParam);
			input_->OnMouseMove(PositionX, PositionY);
		}
		return 0;
	}
	default:
	{
		return DefWindowProc(WindowHandle, Message, WParam, LParam);
	}
	}
}
