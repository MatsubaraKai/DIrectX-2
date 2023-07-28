#include "MyEngine.h"
#include "Triangle.h"

const wchar_t kWindowTitle[] = { L"CG2" };

//Windows�A�v���ł̃G���g���[�|�C���g
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//������
	WinApp* win_ = nullptr;
	CreateEngine* Engine = new CreateEngine;

	Engine->Initialization(win_, kWindowTitle, 1280, 720);

	Engine->VariableInialize();

#pragma region �Q�[�����[�v
	MSG msg{};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		else {

			//�Q�[���̏���
			Engine->BeginFrame();

			Engine->Update();

			Engine->Draw();

			Engine->EndFrame();

		}
	}

#pragma endregion

	OutputDebugStringA("Hello,DirectX!\n");

	Engine->Finalize();

	return 0;
}