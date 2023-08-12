#pragma once
#include <Windows.h>
#include <cstdint>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT
    ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class WinApp {
public:
	// クライアント領域サイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

	HINSTANCE GetHInstance() const { return wc_.hInstance; }

	static bool Procesmessage();
	static void Finalize();

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	static ID3D12Debug1* GetdebugController() { return debugController_; }

	static inline HWND GetHwnd() { return hwnd_; }

	static void CreateWindowView(const wchar_t* title, int32_t clientWidth, int32_t clientheight);

	static const int32_t GetKClientWidth() { return kClientWidth; }

	static const int32_t GetKClientHeight() { return kClientHeight; }

private:
	static UINT windowStyle_;

	static ID3D12Debug1* debugController_;

	static inline RECT wrc_ = {0, 0, kClientWidth, kClientHeight};

	static inline WNDCLASS wc_{}; // ウィンドウクラス

	static HWND hwnd_;
};