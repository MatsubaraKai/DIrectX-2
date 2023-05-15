#pragma once
#include <Windows.h>
#include <cstdint>

class WinApp
{
public:
	//�E�B���h�E�T�C�Y
	static const int32_t kClientWidth = 1280;//����
	static const int32_t kClientHeight = 720;//�c��

	//�E�B���h�E�v���V�[�W��
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	static void CreateWindowView(const wchar_t* title = L"CG2_WinMain");

private:
	// �E�B���h�E�N���X
	static inline WNDCLASS wc{};

	//�E�B���h�E�I�u�W�F�N�g
	static HWND hwnd;
};
