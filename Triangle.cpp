#include "Triangle.h"
#include <assert.h>
#include "MyEngine.h"

void Triangle::Initialize(DirectXCommon* dxCommon, MyEngine* engine)
{
	dxCommon_ = dxCommon;
	engine_ = engine;
	SettingVertex();
	SettingColor();
	SettingDictionalLight();
	TransformMatrix();
}

void Triangle::Draw(const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& material, const Transform& transform, const Transform& cameraTransform, uint32_t index, const DirectionalLight& light)
{
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(dxCommon_->GetWin()->kClientWidth) / float(dxCommon_->GetWin()->kClientHeight), 0.1f, 100.0f);

	Matrix4x4 wvpMatrix_ = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

	//左下
	vertexData_[0].position = a;
	vertexData_[0].texcoord = { 0.0f,1.0f };

	//上
	vertexData_[1].position = b;
	vertexData_[1].texcoord = { 0.5f,0.0f };

	//右下
	vertexData_[2].position = c;
	vertexData_[2].texcoord = { 1.0f,1.0f };

	*materialData_ = { material,false };
	*wvpData_ = { wvpMatrix_,worldMatrix };
	*directionalLight_ = light;

	//VBVを設定
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);

	//形状を設定。PS0に設定しているものとはまた別。同じものを設定する
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//マテリアルCBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]のこと
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, engine_->textureSrvHandleGPU_[index]);

	//描画
	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void Triangle::Finalize()
{
	materialResource_->Release();
	vertexResource_->Release();
	directionalLightResource_->Release();
	wvpResource_->Release();
}

void Triangle::SettingVertex() 
{
	vertexResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * 3);
	
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 3;
	
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	
	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
}

void Triangle::SettingColor()
{
	//マテリアル用のリソースを作る　今回はcolor1つ分
	materialResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData));
	
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}

void Triangle::TransformMatrix()
{
	wvpResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(TransformationMatrix));
	wvpResource_->Map(0, NULL, reinterpret_cast<void**>(&wvpData_));
	wvpData_->WVP = MakeIdentity4x4();
}

void Triangle::SettingDictionalLight()
{
	directionalLightResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice(), sizeof(DirectionalLight));
	directionalLightResource_->Map(0, NULL, reinterpret_cast<void**>(&directionalLight_));
}