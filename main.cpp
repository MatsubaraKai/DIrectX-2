#include "MyEngine.h"
#include "GameScene.h"

const wchar_t kWindowTitle[] = { L"CG2" };

//Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//COM初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	//初期化
	MyEngine* engine = new MyEngine;
    engine->Initialize(kWindowTitle, 1280, 720);

	GameScene* gameScene = new GameScene();
	gameScene->Initialize(engine, engine->GetDirectXCommon());

	while (true)
	{
		//windowのメッセージを最優先で処理させる
		if (WinApp::GetInstance()->Procesmessage())
		{
			break;
		}
		
		//ゲームの処理
		engine->BeginFrame();

		gameScene->Update();

		gameScene->Draw();

		engine->EndFrame();
	}

	//解放処理
	gameScene->Finalize();
	
	engine->Finalize();
	
	CoUninitialize();
	
	return 0;
}