#include "WinApp.h"

//�E�B���h�E�v���V�[�W��
LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	//���b�Z�[�W�ɉ����ăQ�[���ŗL�̏������s��
	switch (msg) {
		//�E�B���h�E���j�����ꂽ
	case WM_DESTROY:
		// OS�ɑ΂��āA�A�v���̏I����`����
		PostQuitMessage(0);
		return 0;
	}

	// �W���̃��b�Z�[�W�������s��
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WinApp::CreateWindowView(const wchar_t* title, int32_t clientWidth, int32_t clientheight) {
	//�E�B���h�E�v���V�[�W��
	wc_.lpfnWndProc = WindowProc;
	//�N���X��
	wc_.lpszClassName = L"CG2WindowClass";
	//�C���X�^���X�n���h��
	wc_.hInstance = GetModuleHandle(nullptr);
	//�J�[�\��
	wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	//�E�B���h�E�N���X�o�^
	RegisterClass(&wc_);

	//�E�B���h�E�T�C�Y�̍\���̂ɃN���C�A���g�̈������
	RECT wrc = { 0,0,kClientWidth,kClientHeight };

	//�N���C�A���g�̈�����Ɏ��ۂ̃T�C�Y��wrc��ύX���Ă��炤
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//�E�B���h�E�̐���
	hwnd_ = CreateWindow(
		wc_.lpszClassName,//�N���X��
		title,//�^�C�g���o�[�̖��O
		WS_OVERLAPPEDWINDOW,//�E�B���h�E�X�^�C��
		CW_USEDEFAULT,//�\��X���W
		CW_USEDEFAULT,//�\��Y���W
		wrc.right - wrc.left,//�E�B���h�E����
		wrc.bottom - wrc.top,//�E�B���h�E�c��
		nullptr,//�e�E�B���h�E�n���h��
		nullptr,//���j���[�n���h��
		wc_.hInstance,//�C���X�^���X�n���h��
		nullptr//�I�v�V����
	);

#ifdef _DEBUG//�f�o�b�O���C���[
	debugController_ = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {
		//�f�o�b�O���C���[��L����
		debugController_->EnableDebugLayer();
		//GPU���ł��`�F�b�N���s��
		debugController_->SetEnableGPUBasedValidation(TRUE);
	}
#endif // _DEBUG

	//�E�B���h�E�\��
	ShowWindow(hwnd_, SW_SHOW);
}

bool WinApp::Procesmessage() {
	MSG msg{};

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT) // �I�����b�Z�[�W�������烋�[�v�𔲂���
	{
		return true;
	}

	return false;
}

void WinApp::Finalize()
{
	debugController_->Release();
}


HWND WinApp::hwnd_;
UINT WinApp::windowStyle_;
ID3D12Debug1* WinApp::debugController_;
