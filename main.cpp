#include "MyEngine.h"
#include "Triangle.h"

const wchar_t kWindowTitle[] = { L"CG2" };

//Windows�A�v���ł̃G���g���[�|�C���g
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//������
	int Triangle = 1;
	WinApp* win_ = nullptr;
	MyEngine* Engine = new MyEngine;
	Engine->Initialization(win_, kWindowTitle, 1280, 720);

	Engine->Initialize();
	Vector4 triangleVertexData[10][3];
	Vector4 material[10] = {};
	Vector3 Rotate;
	float materialColor[4] = { material[10].x,material[10].y,material[10].w,material[10].z };

	

	while (true)
	{
		//window�̃��b�Z�[�W���ŗD��ŏ���������
		if (win_->Procesmessage()) {
			break;
		}

		//�Q�[���̏���
		Engine->BeginFrame();

		Engine->Update();

		for (int i = 0; i < Triangle; i++)
		{
			triangleVertexData[i][0] = { -1.0f,-1.0f + (i * 0.2f),0.0f,2.0f };
			triangleVertexData[i][1] = { 0.0f,1.5f + (i * 0.2f),0.0f,2.0f };
			triangleVertexData[i][2] = { 1.0f,-1.0f + (i * 0.2f),0.0f,2.0f };
			material[i] = { material[i].x,material[i].y,material[i].w,material[i].z };
		}

		ImGui::Begin("Material");
		ImGui::ColorEdit4("MaterialColor", materialColor);
		if (ImGui::TreeNode("Triangle")) {
			ImGui::SliderInt("Triangle", &Triangle,1,10,0);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Translate")) {
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Rotate")) {
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Scale")) {
			ImGui::TreePop();
		}
		ImGui::End();

		for (int i = 0; i < 10; i++)
		{
			material[i].x = materialColor[0];
			material[i].y = materialColor[1];
			material[i].z = materialColor[2];
			material[i].w = materialColor[3];
		}

		//�O�p�`�`��
		for (int i = 0; i < 10; i++)
		{
			Engine->DrawTriangle(triangleVertexData[i][0], triangleVertexData[i][1], triangleVertexData[i][2], material[i]);
		}

		

		Engine->EndFrame();
	}

	//�������
	Engine->Finalize();

	return 0;
}