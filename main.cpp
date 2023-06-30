#include "MyEngine.h"
#include "Triangle.h"

const wchar_t kWindowTitle[] = { L"CG2_Title" };

//Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//初期化
	WinApp* win_ = nullptr;
	CreateEngine* Engine = new CreateEngine;
	Engine->Initialization(win_, kWindowTitle, 1280, 720);
	Engine->Initialize();

	Vector4 data1 = { -0.2f,0.4f,0.0f,1.0f };
	Vector4 data2 = { 0.0f,0.8f,0.0f,1.0f };
	Vector4 data3 = { 0.2f,0.4f,0.0f,1.0f };
	Vector4 material1 = { 1.0f,1.0f,0.0f,1.0f };

	Vector4 data4 = { -0.8f,-0.8f,0.0f,1.0f };
	Vector4 data5 = { -0.6f,-0.4f,0.0f,1.0f };
	Vector4 data6 = { -0.4f,-0.8f,0.0f,1.0f };
	Vector4 material2 = { 0.0f,1.0f,1.0f,1.0f };

	Vector4 data7 = { 0.4f,-0.8f,0.0f,1.0f };
	Vector4 data8 = { 0.6f,-0.4f,0.0f,1.0f };
	Vector4 data9 = { 0.8f,-0.8f,0.0f,1.0f };
	Vector4 material3 = { 1.0f,0.5f,1.0f,1.0f };

	Vector4 data10 = { 0.4f,-0.2f,0.0f,1.0f };
	Vector4 data11 = { 0.6f,0.2f,0.0f,1.0f };
	Vector4 data12 = { 0.8f,-0.2f,0.0f,1.0f };
	Vector4 material4 = { 1.0f,0.0f,1.0f,1.0f };

	Vector4 data13 = { -0.2f,-0.2f,0.0f,1.0f };
	Vector4 data14 = { 0.0f,0.2f,0.0f,1.0f };
	Vector4 data15 = { 0.2f,-0.2f,0.0f,1.0f };
	Vector4 material5 = { 0.0f,0.0f,0.0f,0.0f };

	Vector4 data16 = { 0.4f,0.4f,0.0f,1.0f };
	Vector4 data17 = { 0.6f,0.8f,0.0f,1.0f };
	Vector4 data18 = { 0.8f,0.4f,0.0f,1.0f };
	Vector4 material6 = { 1.0f,1.0f,1.0f,1.0f };

	Vector4 data19 = { -0.8f,-0.2f,0.0f,1.0f };
	Vector4 data20 = { -0.6f,0.2f,0.0f,1.0f };
	Vector4 data21 = { -0.4f,-0.2f,0.0f,1.0f };
	Vector4 material7 = { 1.0f,0.0f,0.0f,1.0f };

	Vector4 data22 = { -0.8f,0.4f,0.0f,1.0f };
	Vector4 data23 = { -0.6f,0.8f,0.0f,1.0f };
	Vector4 data24 = { -0.4f,0.4f,0.0f,1.0f };
	Vector4 material8 = { 0.0f,1.0f,0.0f,1.0f };


	Vector4 data25 = { -0.2f,-0.8f,0.0f,1.0f };
	Vector4 data26 = { 0.0f,-0.4f,0.0f,1.0f };
	Vector4 data27 = { 0.2f,-0.8f,0.0f,1.0f };
	Vector4 material9 = { 0.0f,0.0f,1.0f,1.0f };

	Vector4 data28 = { -0.4f,-0.8f,0.0f,1.0f };
	Vector4 data29 = { -0.3f,-0.4f,0.0f,1.0f };
	Vector4 data30 = { -0.2f,-0.8f,0.0f,1.0f };
	Vector4 material10 = { 1.0f,0.5f,0.0f,1.0f };

	Vector4 data31 = { -0.4f,-0.2f,0.0f,1.0f };
	Vector4 data32 = { -0.3f,0.2f,0.0f,1.0f };
	Vector4 data33 = { -0.2f,-0.2f,0.0f,1.0f };
	Vector4 material11 = { 1.0f,0.5f,0.0f,1.0f };

	Vector4 data34 = { -0.4f,0.4f,0.0f,1.0f };
	Vector4 data35 = { -0.3f,0.8f,0.0f,1.0f };
	Vector4 data36 = { -0.2f,0.4f,0.0f,1.0f };
	Vector4 material12 = { 1.0f,0.5f,0.0f,1.0f };

	Vector4 data37 = { 0.2f,0.4f,0.0f,1.0f };
	Vector4 data38 = { 0.3f,0.8f,0.0f,1.0f };
	Vector4 data39 = { 0.4f,0.4f,0.0f,1.0f };
	Vector4 material13 = { 1.0f,0.5f,0.0f,1.0f };

	Vector4 data40 = { 0.2f,-0.8f,0.0f,1.0f };
	Vector4 data41 = { 0.3f,-0.4f,0.0f,1.0f };
	Vector4 data42 = { 0.4f,-0.8f,0.0f,1.0f };
	Vector4 material14 = { 1.0f,0.5f,0.0f,1.0f };

	Vector4 data43 = { 0.2f,-0.2f,0.0f,1.0f };
	Vector4 data44 = { 0.3f,0.2f,0.0f,1.0f };
	Vector4 data45 = { 0.4f,-0.2f,0.0f,1.0f };
	Vector4 material15 = { 1.0f,0.5f,0.0f,1.0f };

	MSG msg{};
	//ウィンドウのxが押されるまでループ
	while (msg.message != WM_QUIT) {
		//windowのメッセージを最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			//ゲームの処理
			Engine->Update();
			Engine->BeginFrame();


			//三角形描画
			Engine->DrawTriangle(data1, data2, data3, material1);

			Engine->DrawTriangle(data4, data5, data6, material2);

			Engine->DrawTriangle(data7, data8, data9, material3);

			Engine->DrawTriangle(data10, data11, data12, material4);

			Engine->DrawTriangle(data13, data14, data15, material5);

			Engine->DrawTriangle(data16, data17, data18, material6);

			Engine->DrawTriangle(data19, data20, data21, material7);

			Engine->DrawTriangle(data22, data23, data24, material8);

			Engine->DrawTriangle(data25, data26, data27, material9);

			Engine->DrawTriangle(data28, data29, data30, material10);

			Engine->DrawTriangle(data31, data32, data33, material11);

			Engine->DrawTriangle(data34, data35, data36, material12);

			Engine->DrawTriangle(data37, data38, data39, material13);

			Engine->DrawTriangle(data40, data41, data42, material14);

			Engine->DrawTriangle(data43, data44, data45, material15);

			Engine->EndFrame();
		}
	}

	Engine->Finalize();

	return 0;
}
