#pragma once
#include "DirectXCommon.h"
#include <dxcapi.h>
#include "Vector3.h"
#include"Vector4.h"
#include "Triangle.h"
#include "MatrixCalculation.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "ConvertString.h"
#include "VertexData.h"
#pragma comment(lib,"dxcompiler.lib")


class MyEngine 
{
public:
	void Initialize();

	void Initialization(WinApp* win, const wchar_t* title, int32_t width, int32_t height);

	void BeginFrame();

	void EndFrame();

	void Release();

	void Update();

	void DrawTriangle(const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& material);
	
	DirectX::ScratchImage LoadTexture(const std::string& filePath);

	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandleGPU() { return textureSrvHandleGPU_; }

private:
	static WinApp* winApp_;
	static	DirectXCommon* dxCommon_;

	Triangle* triangle_[11];

	int triangleCount_;

	const int kMaxTriangle = 5;

	IDxcUtils* dxcUtils_;
	IDxcCompiler3* dxcCompiler_;

	IDxcIncludeHandler* includeHandler_;

	ID3DBlob* signatureBlob_;
	ID3DBlob* errorBlob_;
	ID3D12RootSignature* rootSignature_;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};

	D3D12_BLEND_DESC blendDesc_{};

	IDxcBlob* vertexShaderBlob_;

	IDxcBlob* pixelShaderBlob_;

	D3D12_RASTERIZER_DESC rasterizerDesc_{};

	ID3D12PipelineState* graphicsPipelineState_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	D3D12_VIEWPORT viewport_{};
	D3D12_RECT scissorRect_{};

	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[2];

	//頂点リソースにデータを書き込む
	Vector4* vertexData_;

	Transform transform_;
	Matrix4x4 worldMatrix_;

	IDxcBlob* CompileShader(
		//CompileShaderするShaderファイルへのパス
		const std::wstring& filePath,
		//Compielerに使用するProfile
		const wchar_t* profile,
		//初期化で生成したものを3つ
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler
	);

	ID3D12Resource* textureResource_ = nullptr;

	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;

	DirectX::ScratchImage OpenImage(const std::string& filePath);

	ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);

	void UploadTexturData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

	void InitializeDxcCompiler();
	void CreateRootSignature();
	void CreateInputlayOut();
	void BlendState();
	void RasterizerState();
	void InitializePSO();
	void ViewPort();
	void ScissorRect();
};