#include"Triangle.h"
#include<assert.h>
#include "MyEngine.h"

void CreateTriangle::Initialize(DirectXCommon* dxCommon, const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& material) {
	dxCommon_ = dxCommon;
	SettingVertex(a, b, c);
	SettingColor(material);
}

void CreateTriangle::Draw() {

	//VBVを設定
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//形状を設定。PS0に設定しているものとはまた別。同じものを設定する
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//マテリアルCBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//描画
	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);

}

void CreateTriangle::Finalize() {
	/************************************************
	Resourceとは動作の実行に必要な処理システムの要素や機器
	************************************************/
	materialResource_->Release();
	vertexResource_->Release();
}

void CreateTriangle::SettingVertex(const Vector4& a, const Vector4& b, const Vector4& c) {
	/************************************************
	Bufferはデータを一時的に保持する記憶領域
	Z-Bufferとは深度情報を格納したResource
	Vertexは頂点のこと
	VertexBufferView(VBV)
	ShaderResourceView(SRV)はTextureを読むのに必要なもの
	************************************************/
	vertexResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Vector4) * 3);
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(Vector4) * 3;
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(Vector4);
	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	//左下
	vertexData_[0] = a;
	//上
	vertexData_[1] = b;
	//右下
	vertexData_[2] = c;

}

void CreateTriangle::SettingColor(const Vector4& material) {
	//マテリアル用のリソースを作る　今回はcolor1つ分
	materialResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Vector4));
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

	*materialData_ = material;
}

ID3D12Resource* CreateTriangle::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {
	//頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uplodeHeapProperties{};
	uplodeHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
	//頂点リソースの設定
	D3D12_RESOURCE_DESC ResourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = sizeInBytes;//リソースサイズ
	//バッファの場合はこれらは１にする決まり
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	HRESULT hr;

	ID3D12Resource* Resource = nullptr;
	//実際に頂点リソースを作る
	hr = device->CreateCommittedResource(&uplodeHeapProperties, D3D12_HEAP_FLAG_NONE,
		&ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&Resource));
	assert(SUCCEEDED(hr));

	return Resource;
}