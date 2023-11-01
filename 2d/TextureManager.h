#pragma once
#include"DirectXCommon.h"
#include"MyEngine.h"
#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"
#include<wrl.h>
#include<array>
class TextureManager
{
public:

	static TextureManager* GetInstance();

	struct Texture
	{
		D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
		Microsoft::WRL::ComPtr< ID3D12Resource> textureResource;
	};

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TextureManager();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directX"></param>
	/// <param name="engine"></param>
	void Initialize(int32_t width, int32_t height);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 画像読み込み
	/// </summary>
	/// <param name="index"></param>
	/// <param name="filePath"></param>
	uint32_t LoadTexture(const std::string& filePath);

	//ゲッター
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t index)
	{
		return textures_.at(index).textureSrvHandleGPU;
	}

private:

	DirectXCommon* dxCommon_;
	MyEngine* engine_;

	static const size_t kMaxTexture = 2056;	//最大テクスチャ数
	uint32_t TextureCount = 0;	//現在のテクスチャ数
	std::array<Texture, kMaxTexture> textures_;

	bool IsusedTexture[kMaxTexture];

	//中間リソース
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxTexture> intermediateResource;

	uint32_t descriptorSizeSRV;
	

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc_;
	Microsoft::WRL::ComPtr< ID3D12Resource> depthStencilResource_;

	static TextureManager* instance;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;

private:


	DirectX::ScratchImage ImageFileOpen(const std::string& filePath);
	Microsoft::WRL::ComPtr< ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);

	[[nodiscard]]
	Microsoft::WRL::ComPtr< ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);
	/// <summary>
	/// テクスチャデータを読む
	/// </summary>
	/// <param name="filepath"></param>
	/// <returns></returns>
	DirectX::ScratchImage LoadTexture(const std::string& filePath);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);
	
	/// <summary>
	Microsoft::WRL::ComPtr< ID3D12Resource> CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr< ID3D12Device> device, int32_t width, int32_t height);

	/// /// <param name="device"></param>
	/// <param name="width">ウィンドウの幅</param>
	/// <param name="height">ウィンドウの高さ</param>
	/// <returns></returns>
	ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);

	/// <summary>
	/// TextureResourceにデータを転送する
	/// </summary>
	/// <param name="texture"></param>
	/// <param name="mipImages"></param>
	void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);


	/// <summary>
	/// SRVの設定と生成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="srvDescriptorHeap"></param>
	void CreateShaderResourceView(ID3D12Device* device, ID3D12DescriptorHeap* srvDescriptorHeap);

	/// <summary>
	///	dsvの設定と設定
	/// </summary>
	void CreateDepthStencilView(ID3D12Device* device, ID3D12DescriptorHeap* dsvDescriptorHeap);


};