#pragma once
#include <Windows.h>
#include <cstdint>
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class WinApp
{
public:
	//クライアント領域サイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

	bool Procesmessage();
	void Finalize();

	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	ID3D12Debug1* GetdebugController() { return debugController_; }

	inline HWND GetHwnd() { return hwnd_; }

	void CreateWindowView(const wchar_t* title, int32_t clientWidth, int32_t clientheight);

	WinApp(const WinApp& obj) = delete;

	WinApp& operator=(const WinApp& obj) = delete;

	static WinApp* GetInstance();

private:
	UINT windowStyle_;

	static ID3D12Debug1* debugController_;

	static inline WNDCLASS wc_{};//ウィンドウクラス

	static HWND hwnd_;

	WinApp() = default;

	~WinApp() = default;
};