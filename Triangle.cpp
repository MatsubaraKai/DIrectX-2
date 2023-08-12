#include "Triangle.h"
#include "MyEngine.h"
#include <assert.h>

void Triangle::Initialize(DirectXCommon* dxCommon, MyEngine* engine) {
	dxCommon_ = dxCommon;
	engine_ = engine;
	SettingVertex();
	SettingColor();
	MoveMatrix();
}

void Triangle::Draw(
    const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& material,
    const Matrix4x4& wvpdata) {
	// 左下
	vertexData_[0].position = a;
	vertexData_[0].texcoord = {0.0f, 1.0f};

	// 上
	vertexData_[1].position = b;
	vertexData_[1].texcoord = {0.5f, 0.0f};

	// 右下
	vertexData_[2].position = c;
	vertexData_[2].texcoord = {1.0f, 1.0f};

	// 追加
	vertexData_[3].position = {-0.5f, -0.3f, 0.3f, 1.0f};
	vertexData_[3].texcoord = {0.0f, 1.0f};

	vertexData_[4].position = {0.0f, 0.0f, 0.0f, 1.0f};
	vertexData_[4].texcoord = {0.5f, 0.0f};

	vertexData_[5].position = {0.5f, -0.3f, -0.3f, 1.0f};
	vertexData_[5].texcoord = {1.0f, 1.0f};

	// マテリアル
	*materialData_ = material;

	*wvpData_ = wvpdata;

	// VBVを設定
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);

	// 形状を設定。PS0に設定しているものとはまた別。同じものを設定する
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアルCBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(
	    0, materialResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(
	    1, wvpResource_->GetGPUVirtualAddress());

	// SRVのDescriptorTableの先頭を設定。2はrootPrameter[2]である。
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, engine_->GetTextureHandleGPU());

	// 描画
	dxCommon_->GetCommandList()->DrawInstanced(6, 1, 0, 0);
}

void Triangle::Release() {
	materialResource_->Release();
	vertexResource_->Release();
	wvpResource_->Release();
}

void Triangle::SettingVertex() {
	vertexResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * 6);
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 6;
	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
}

void Triangle::SettingColor() {
	// マテリアル用のリソースを作る　今回はcolor1つ分
	materialResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Vector4));
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}

void Triangle::MoveMatrix() {
	wvpResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Matrix4x4));
	wvpResource_->Map(0, NULL, reinterpret_cast<void**>(&wvpData_));
	*wvpData_ = MakeIdentity4x4();
}

ID3D12Resource* Triangle::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {
	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uplodeHeapProperties{};
	uplodeHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // UploadHeapを使う
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC ResourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = sizeInBytes; // リソースサイズ
	// バッファの場合はこれらは１にする決まり
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	HRESULT hr;

	ID3D12Resource* Resource = nullptr;
	// 実際に頂点リソースを作る
	hr = device->CreateCommittedResource(
	    &uplodeHeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc,
	    D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&Resource));
	assert(SUCCEEDED(hr));

	return Resource;
}