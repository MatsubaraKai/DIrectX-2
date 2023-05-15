#pragma once
#include <Windows.h>
#include <cstdint>

class WinApp
{
public:
	//ウィンドウサイズ
	static const int32_t kClientWidth = 1280;//横幅
	static const int32_t kClientHeight = 720;//縦幅

	//ウィンドウプロシージャ
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	static void CreateWindowView(const wchar_t* title = L"CG2_WinMain");

private:
	// ウィンドウクラス
	static inline WNDCLASS wc{};

	//ウィンドウオブジェクト
	static HWND hwnd;
};
