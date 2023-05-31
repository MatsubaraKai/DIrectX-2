#pragma once
#include "DirectX.h"
#include <dxcapi.h>
#include"Vector4.h"
#include "Triangle.h"
#pragma comment(lib,"dxcompiler.lib")

class CreateEngine
{
public:
	void Initialize();

	void Initialization(WinApp* win, const wchar_t* title, int32_t width, int32_t height);

	void BeginFrame();

	void EndFrame();

	void Finalize();

	void Update();

	void DrawTriangle(const Vector4& a, const Vector4& b, const Vector4& c);

private:
	static WinApp* win_;
	static	DirectXCommon* dxCommon_;

	CreateTriangle* triangle_[16];

	int triangleCount_;

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

	ID3D12Resource* vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	D3D12_VIEWPORT viewport_{};
	D3D12_RECT scissorRect_{};

	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[1];

	//���_���\�[�X�Ƀf�[�^����������
	Vector4* vertexData_;
	Vector4 leftBottom[16];
	Vector4 top[16];
	Vector4 rightBottom[16];

	IDxcBlob* CompileShader(
		//CompileShader����Shader�t�@�C���ւ̃p�X
		const std::wstring& filePath,
		//Compieler�Ɏg�p����Profile
		const wchar_t* profile,
		//�������Ő����������̂�3��
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
	void VertexResource();
	void ViewPort();
	void ScissorRect();

};
