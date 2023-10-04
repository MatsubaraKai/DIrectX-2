#include "GameScene.h"

void GameScene::Initialize(MyEngine* engine, DirectXCommon* dxCommon) 
{
	engine_ = engine;
	dxCommon_ = dxCommon;

	triangleData_[0].position[0] = { -0.5f,-0.5f,0.5f,1.0f };
	triangleData_[0].position[1] = { 0.0f,0.0f,0.0f,1.0f };
	triangleData_[0].position[2] = { 0.5f,-0.5f,-0.5f,1.0f };
	triangleData_[0].material = { 1.0f,1.0f,1.0f,1.0f };

	triangleData_[1].position[0] = { -0.5f,-0.5f,0.0f,1.0f };
	triangleData_[1].position[1] = { 0.0f,0.5f,0.0f,1.0f };
	triangleData_[1].position[2] = { 0.5f,-0.5f,0.0f,1.0f };
	triangleData_[1].material = { 1.0f,1.0f,1.0f,1.0f };

	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	spriteData_.positionLeftTop[0] = { 0.0f,0.0f,0.0f,1.0f };
	spriteData_.positionRightDown[0] = { 640.0f,360.0f,0.0f,1.0f };
	spriteData_.positionLeftTop[1] = { 0.0f,0.0f,0.0f,1.0f };
	spriteData_.positionRightDown[1] = { 640.0f,360.0f,0.0f,1.0f };
	spriteData_.material = { 1.0f,1.0f,1.0f,1.0f };
	spriteTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	sphereTransform_ = { {0.4f,0.4f,0.4f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	sphereMaterial_ = { 1.0f,1.0f,1.0f,1.0f };

	texture = 0;
	uvResourceNum = 0;
	engine_->SettingTexture("resources/uvChecker.png", uvResourceNum);

	monsterBallResourceNum = 1;
	engine_->SettingTexture("resources/monsterBall.png", monsterBallResourceNum);

	for (int i = 0; i < 2; i++)
	{
		triangle_[i] = new Triangle();
		triangle_[i]->Initialize(dxCommon_, engine_);
	}

	for (int i = 0; i < 2; i++) 
	{
		sprite_[i] = new Sprite();
		sprite_[i]->Initialize(dxCommon_, engine_);
	}

	sphere_ = new Sphere();
	sphere_->Initialize(dxCommon_, engine_);

	cameraTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-5.0f} };
}

void GameScene::Update() 
{
	transform_.rotate.num[1] += 0.01f;
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

	sphereTransform_.rotate.num[1] += 0.01f;
	sphereMatrix_ = MakeAffineMatrix(sphereTransform_.scale, sphereTransform_.rotate, sphereTransform_.translate);

	Matrix4x4 sphereAffine = MakeAffineMatrix(sphereTransform_.scale, sphereTransform_.rotate, sphereTransform_.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform_.scale, cameraTransform_.rotate, cameraTransform_.translate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(dxCommon_->GetWin()->kClientWidth) / float(dxCommon_->GetWin()->kClientHeight), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix_, Multiply(viewMatrix, projectionMatrix));

	worldMatrix_ = worldViewProjectionMatrix;
	sphereMatrix_ = Multiply(sphereAffine, Multiply(viewMatrix, projectionMatrix));

	ImGui::Begin("OPTION");
	ImGui::Checkbox("useMonsterBall", &texture);
	ImGui::ColorEdit3("TriangleColor", triangleData_[0].material.num);
	ImGui::ColorEdit3("TriangleColor2", triangleData_[1].material.num);
	ImGui::ColorEdit3("SphereColor", sphereMaterial_.num);
	ImGui::DragFloat3("CameraTranslate", cameraTransform_.translate.num, 0.05f);
	ImGui::DragFloat3("SpriteTranslate", spriteTransform_.translate.num, 5.0f);
	ImGui::DragFloat3("SphereTranslate", sphereTransform_.translate.num, 0.05f);
	ImGui::DragFloat3("SphereRotate", sphereTransform_.rotate.num, 0.05f);
	ImGui::DragFloat3("SphereScale", sphereTransform_.scale.num, 0.05f);
	ImGui::End();
}

void GameScene::Draw() 
{
#pragma region 3Dオブジェクト描画
	for (int i = 0; i < 2; i++)
	{
		triangle_[i]->Draw(triangleData_[i].position[0], triangleData_[i].position[1], triangleData_[i].position[2], triangleData_[i].material, worldMatrix_, uvResourceNum);
	}

	sphere_->Draw(sphereMaterial_, sphereMatrix_, texture);
#pragma endregion

#pragma region 前景スプライト描画
	for (int i = 0; i < 1; i++)
	{
		sprite_[i]->Draw(spriteData_.positionLeftTop[i], spriteData_.positionRightDown[i], spriteTransform_, spriteData_.material, uvResourceNum);
	}
#pragma endregion
}

void GameScene::Finalize() 
{
	for (int i = 0; i < 2; i++)
	{
		triangle_[i]->Finalize();
	}

	for (int i = 0; i < 2; i++) 
	{
		sprite_[i]->Finalize();
	}

	sphere_->Finalize();
}
