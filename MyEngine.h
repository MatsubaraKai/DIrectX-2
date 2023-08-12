#pragma once
#include "DirectXCommon.h"
#include <dxcapi.h>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "VertexData.h"
#include "Triangle.h"
#include "MatrixCalculation.h"
#include "ConvertString.h"
#pragma comment(lib,"dxcompiler.lib")

class MyEngine
{
public:
	void Initialize(const wchar_t* title, int32_t width, int32_t height);

	void BeginFrame();

	void EndFrame();

	void Finalize();

	void Update();

	void SettingTexture(const std::string& filePath);

	DirectXCommon* GetDirectXCommon() { return dxCommon_; }

	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;

private:
	static WinApp* win_;
	static	DirectXCommon* dxCommon_;

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

	ID3D12Resource* textureResource_;

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};

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

	void InitializeDxcCompiler();
	void CreateRootSignature();
	void CreateInputlayOut();
	void BlendState();
	void RasterizerState();
	void InitializePSO();
	void ViewPort();
	void ScissorRect();
	void SettingDepth();

	DirectX::ScratchImage LoadTexture(const std::string& filePath);
	ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);
	void UploadtextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);
};