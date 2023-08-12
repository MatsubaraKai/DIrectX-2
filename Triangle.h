#pragma once
#include"DirectXCommon.h"
#include "Vector3.h"
#include"Vector4.h"
#include "Matrix4x4.h"
#include "ConvertString.h"
#include "VertexData.h"

class MyEngine;

class Triangle
{
public:
	void Initialize(DirectXCommon* dxCommon, MyEngine* engine);

	void Draw(const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& material, const Matrix4x4& wvpdata);

	void Release();

private:
	void SettingVertex();

	void SettingColor();

	void MoveMatrix();

private:
	MyEngine* engine_;

	DirectXCommon* dxCommon_;

	VertexData* vertexData_;

	Vector4* materialData_;

	ID3D12Resource* vertexResource_;

	ID3D12Resource* materialResource_;

	ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	//WVP用のリソース
	ID3D12Resource* wvpResource_;

	Matrix4x4* wvpData_;
};