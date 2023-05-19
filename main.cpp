#include <Windows.h>
#include "WinApp.h"
#include "DirectX.h"

const wchar_t kWindowTitle[] = { L"CG2_WinMain" };

//Windows�A�v���ł̃G���g���[�|�C���g
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//������
	WinApp::CreateWindowView(kWindowTitle);
	DirectXCommon::DirectXInitialization();

	MSG msg{};
	//�E�B���h�E��x���������܂Ń��[�v
	while (msg.message != WM_QUIT) {
		//window�̃��b�Z�[�W���ŗD��ŏ���������
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			//�Q�[���̏���
		}
	}
	return 0;
}