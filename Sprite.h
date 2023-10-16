#pragma once
#include "DirectXCommon.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "VertexData.h"
#include "MyEngine.h"

class Sprite 
{
public:
	void Initialize(DirectXCommon* dxCommon, MyEngine* engine);

	void Draw(const Vector4& a, const Vector4& b, const Transform& transform, const Vector4& material, uint32_t index, const DirectionalLight& light);
	
	void Finalize();

private:
	void SettingVartex();

	void TransformMatrix();

	void SettingDictionalLight();

	void SettingColor();

private:
	DirectXCommon* dxCommon_;
	MyEngine* engine_;
	
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	ID3D12Resource* vertexResourceSprite_;
	VertexData* vertexData_;
	
	ID3D12Resource* transformationMatrixResource_;
	TransformationMatrix* transformationMatrixdata_;

	ID3D12Resource* materialResource_;
	Material* materialData_;

	DirectionalLight* directionalLight_;
	ID3D12Resource* directionalLightResource_;
};

