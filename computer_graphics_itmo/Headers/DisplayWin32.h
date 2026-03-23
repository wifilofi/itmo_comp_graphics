#pragma once

#include <windows.h>
#include <WinUser.h>
#include <functional>

class DisplayWin32
{
public:
	using WndProcCallback = std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>;

	DisplayWin32(LPCWSTR appName, HINSTANCE instanceHandle, int width, int height, WndProcCallback messageCallback);
	~DisplayWin32();

	HWND GetWindowHandle() const;
	HINSTANCE GetInstanceHandle() const;
	int GetClientWidth() const;
	int GetClientHeight() const;

private:
	static LRESULT CALLBACK WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

	HWND WindowHandle;
	HINSTANCE InstanceHandle;
	WNDCLASSEX WindowClass;
	int ClientWidth;
	int ClientHeight;
	WndProcCallback MessageCallback;
};
