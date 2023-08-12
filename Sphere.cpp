#include "Sphere.h"
#include "MyEngine.h"
#include <cmath>

void Sphere::Initialize(DirectXCommon* dxCommon, MyEngine* engine)
{
	dxCommon_ = dxCommon;
	engine_ = engine;
	kSubDivision = 16;
	vertexCount = kSubDivision * kSubDivision * 6;
	SettingVertex();
	SettingColor();
	TransformMatrix();
}

void Sphere::Draw(const Vector4& material, const Matrix4x4& wvpdata) 
{
	//経度分割一つ分の角度
	const float kLonEvery = pi * 2.0f / float(kSubDivision);
	const float kLatEvery = pi / float(kSubDivision);

	//緯度の方向に分割
	for (uint32_t latIndex = 0; latIndex < kSubDivision; ++latIndex)
	{
		float lat = -pi / 2.0f + kLatEvery * latIndex;
		//経度の方向に分割しながら線を描く
		for (uint32_t lonIndex = 0; lonIndex < kSubDivision; ++lonIndex)
		{
			uint32_t start = (latIndex * kSubDivision + lonIndex) * 6;
			float lon = lonIndex * kLonEvery;
			//頂点にデータを入力する　基準点a
			vertexData_[start].position = { cos(lat) * cos(lon),sin(lat),cos(lat) * sin(lon),1.0f };
			vertexData_[start].texcoord = { float(lonIndex) / float(kSubDivision),1.0f - float(latIndex) / kSubDivision };

			vertexData_[start + 1].position = { cos(lat + kLatEvery) * cos(lon),sin(lat + kLatEvery),cos(lat + kLatEvery) * sin(lon),1.0f };
			vertexData_[start + 1].texcoord = { vertexData_[start].texcoord.num[0],vertexData_[start].texcoord.num[1] - 1.0f / float(kSubDivision) };

			vertexData_[start + 2].position = { cos(lat) * cos(lon + kLonEvery),sin(lat),cos(lat) * sin(lon + kLonEvery),1.0f };
			vertexData_[start + 2].texcoord = { vertexData_[start].texcoord.num[0] + 1.0f / float(kSubDivision),vertexData_[start].texcoord.num[1] };

			vertexData_[start + 3].position = { cos(lat) * cos(lon + kLonEvery),sin(lat),cos(lat) * sin(lon + kLonEvery),1.0f };
			vertexData_[start + 3].texcoord = { vertexData_[start].texcoord.num[0] + 1.0f / float(kSubDivision),vertexData_[start].texcoord.num[1] };

			vertexData_[start + 4].position = { cos(lat + kLatEvery) * cos(lon),sin(lat + kLatEvery),cos(lat + kLatEvery) * sin(lon),1.0f };
			vertexData_[start + 4].texcoord = { vertexData_[start].texcoord.num[0],vertexData_[start].texcoord.num[1] - 1.0f / float(kSubDivision) };

			vertexData_[start + 5].position = { cos(lat + kLatEvery) * cos(lon + kLonEvery),sin(lat + kLatEvery), cos(lat + kLatEvery) * sin(lon + kLonEvery),1.0f };
			vertexData_[start + 5].texcoord = { vertexData_[start].texcoord.num[0] + 1.0f / float(kSubDivision),vertexData_[start].texcoord.num[1] - 1.0f / float(kSubDivision) };

			*materialData_ = material;
			*wvpData_ = wvpdata;

			//VBVを設定
			dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);

			//形状を設定。PS0に設定しているものとはまた別。同じものを設定する
			dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//マテリアルCBufferの場所を設定
			dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
			dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());

			//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]のこと
			dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, engine_->textureSrvHandleGPU_);

			//描画
			dxCommon_->GetCommandList()->DrawInstanced(vertexCount, 1, 0, 0);
		}
	}
}

void Sphere::Finalize()
{
	vertexResource->Release();
	materialResource_->Release();
	wvpResource_->Release();
}

void Sphere::SettingVertex()
{
	vertexResource = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * vertexCount);
	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();

	vertexBufferView.SizeInBytes = sizeof(VertexData) * vertexCount;

	vertexBufferView.StrideInBytes = sizeof(VertexData);

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
}

void Sphere::TransformMatrix() 
{
	wvpResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(Matrix4x4));
	wvpResource_->Map(0, NULL, reinterpret_cast<void**>(&wvpData_));
	*wvpData_ = MakeIdentity4x4();
}

void Sphere::SettingColor() 
{
	materialResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData));

	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}