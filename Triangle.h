#pragma once
#include"DirectX.h"
#include"Vector4.h"

class CreateEngine;

class CreateTriangle {
public:
	void Initialize(DirectXCommon* dxCommon);

	void Draw(const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& material);

	void Finalize();

private:
	void SettingVertex();

	void SettingColor();

private:
	CreateEngine* Engine;

	DirectXCommon* dxCommon_;

	Vector4* vertexData_;

	Vector4* materialData_;

	ID3D12Resource* vertexResource_;

	ID3D12Resource* materialResource_;

	ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBites);

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
};
