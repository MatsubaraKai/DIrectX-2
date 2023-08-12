#include "Triangle.h"
#include <assert.h>
#include "MyEngine.h"

void Triangle::Initialize(DirectXCommon* dxCommon, MyEngine* engine)
{
	dxCommon_ = dxCommon;
	engine_ = engine;
	SettingVertex();
	SettingColor();
	TransformMatrix();
}

void Triangle::Draw(const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& material, const Matrix4x4& wvpdata)
{
	//左下
	vertexData_[0].position = a;
	vertexData_[0].texcoord = { 0.0f,1.0f };

	//上
	vertexData_[1].position = b;
	vertexData_[1].texcoord = { 0.5f,0.0f };

	//右下
	vertexData_[2].position = c;
	vertexData_[2].texcoord = { 1.0f,1.0f };

	*materialData_ = material;
	*wvpData_ = wvpdata;

	//VBVを設定
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);

	//形状を設定。PS0に設定しているものとはまた別。同じものを設定する
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//マテリアルCBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]のこと
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, engine_->textureSrvHandleGPU_);

	//描画
	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void Triangle::Finalize()
{
	materialResource_->Release();
	vertexResource_->Release();
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
	materialResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(Vector4));

	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}

void Triangle::TransformMatrix()
{
	wvpResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(Matrix4x4));
	wvpResource_->Map(0, NULL, reinterpret_cast<void**>(&wvpData_));
	*wvpData_ = MakeIdentity4x4();
}