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

	Vector4 data[45];
	Vector4 material[15];

	 data[0] = {-0.2f,0.4f,0.0f,1.0f};
	 data[1] = {0.0f,0.8f,0.0f,1.0f};
	 data[2] = {0.2f,0.4f,0.0f,1.0f};
	 material[0] = {1.0f,1.0f,0.0f,1.0f};

	 data[3] = {-0.8f,-0.8f,0.0f,1.0f};
	 data[4] = {-0.6f,-0.4f,0.0f,1.0f};
	 data[5] = {-0.4f,-0.8f,0.0f,1.0f};
	 material[1] = {0.0f,1.0f,1.0f,1.0f};

	 data[6] = {0.4f,-0.8f,0.0f,1.0f};
	 data[7] = {0.6f,-0.4f,0.0f,1.0f};
	 data[8] = {0.8f,-0.8f,0.0f,1.0f};
	 material[2] = {1.0f,0.5f,1.0f,1.0f};

	 data[9] = {0.4f,-0.2f,0.0f,1.0f};
	 data[10] = {0.6f,0.2f,0.0f,1.0f};
	 data[11] = {0.8f,-0.2f,0.0f,1.0f};
	 material[3] = {1.0f,0.0f,1.0f,1.0f};

	 data[12] = {-0.2f,-0.2f,0.0f,1.0f};
	 data[13] = {0.0f,0.2f,0.0f,1.0f};
	 data[14] = {0.2f,-0.2f,0.0f,1.0f};
	 material[4] = {0.0f,0.0f,0.0f,0.0f};

	 data[15] = {0.4f,0.4f,0.0f,1.0f};
	 data[16] = {0.6f,0.8f,0.0f,1.0f};
	 data[17] = {0.8f,0.4f,0.0f,1.0f};
	 material[5] = {1.0f,1.0f,1.0f,1.0f};

	 data[18] = {-0.8f,-0.2f,0.0f,1.0f};
	 data[19] = {-0.6f,0.2f,0.0f,1.0f};
	 data[20] = {-0.4f,-0.2f,0.0f,1.0f};
	 material[6] = {1.0f,0.0f,0.0f,1.0f};

	 data[21] = {-0.8f,0.4f,0.0f,1.0f};
	 data[22] = {-0.6f,0.8f,0.0f,1.0f};
	 data[23] = {-0.4f,0.4f,0.0f,1.0f};
	 material[7] = {0.0f,1.0f,0.0f,1.0f};


	 data[24] = {-0.2f,-0.8f,0.0f,1.0f};
	 data[25] = {0.0f,-0.4f,0.0f,1.0f};
	 data[26] = {0.2f,-0.8f,0.0f,1.0f};
	 material[8] = {0.0f,0.0f,1.0f,1.0f};

	 data[27] = {-0.4f,-0.8f,0.0f,1.0f};
	 data[28] = {-0.3f,-0.4f,0.0f,1.0f};
	 data[29] = {-0.2f,-0.8f,0.0f,1.0f};
	 material[9] = {1.0f,0.5f,0.0f,1.0f};

	 data[30] = {-0.4f,-0.2f,0.0f,1.0f};
	 data[31] = {-0.3f,0.2f,0.0f,1.0f};
	 data[32] = {-0.2f,-0.2f,0.0f,1.0f};
	 material[10] = {1.0f,0.5f,0.0f,1.0f};

	 data[33] = {-0.4f,0.4f,0.0f,1.0f};
	 data[34] = {-0.3f,0.8f,0.0f,1.0f};
	 data[35] = {-0.2f,0.4f,0.0f,1.0f};
	 material[11] = {1.0f,0.5f,0.0f,1.0f};

	 data[36] = {0.2f,0.4f,0.0f,1.0f};
	 data[37] = {0.3f,0.8f,0.0f,1.0f};
	 data[38] = {0.4f,0.4f,0.0f,1.0f};
	 material[12] = {1.0f,0.5f,0.0f,1.0f};

	 data[39] = {0.2f,-0.8f,0.0f,1.0f};
	 data[40] = {0.3f,-0.4f,0.0f,1.0f};
	 data[41] = {0.4f,-0.8f,0.0f,1.0f};
	 material[13] = {1.0f,0.5f,0.0f,1.0f};

	 data[42] = {0.2f,-0.2f,0.0f,1.0f};
	 data[43] = {0.3f,0.2f,0.0f,1.0f};
	 data[44] = {0.4f,-0.2f,0.0f,1.0f};
	 material[14] = {1.0f,0.5f,0.0f,1.0f};

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
			Engine->DrawTriangle(data[0], data[1], data[2], material[0]);

			Engine->DrawTriangle(data[3], data[4], data[5], material[1]);

			Engine->DrawTriangle(data[6], data[7], data[8], material[2]);

			Engine->DrawTriangle(data[9], data[10], data[11], material[3]);

			Engine->DrawTriangle(data[12], data[13], data[14], material[4]);

			Engine->DrawTriangle(data[15], data[16], data[17], material[5]);

			Engine->DrawTriangle(data[18], data[19], data[20], material[6]);

			Engine->DrawTriangle(data[21], data[22], data[23], material[7]);

			Engine->DrawTriangle(data[24], data[25], data[26], material[8]);

			Engine->DrawTriangle(data[27], data[28], data[29], material[9]);

			Engine->DrawTriangle(data[30], data[31], data[32], material[10]);

			Engine->DrawTriangle(data[33], data[34], data[35], material[11]);

			Engine->DrawTriangle(data[36], data[37], data[38], material[12]);

			Engine->DrawTriangle(data[39], data[40], data[41], material[13]);

			Engine->DrawTriangle(data[42], data[43], data[44], material[14]);

			Engine->EndFrame();
		}
	}

	Engine->Finalize();

	return 0;
}
