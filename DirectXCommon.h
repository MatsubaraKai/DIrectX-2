#pragma once
#include "ConvertString.h"
#include "WinApp.h"
#include <chrono>
#include <cstdlib>
#include <dxgi1_6.h>

class DirectXCommon {
public:
	void Initialization(
	    WinApp* win, const wchar_t* title, int32_t backBufferWidth = WinApp::kClientWidth,
	    int32_t backBufferHeight = WinApp::kClientHeight);

	void PreDraw();

	void PostDraw();

	static inline void ClearRenderTarget();

	static void Release();

	HRESULT GetHr() { return hr_; }

	void SetHr(HRESULT a) { this->hr_ = a; }

	ID3D12Device* GetDevice() { return device_; }

	ID3D12GraphicsCommandList* GetCommandList() { return commandList_; }

	static void ImGuiInitialize();

	ID3D12DescriptorHeap* GetSrvDescriptorHeap() { return srvDescriptorHeap_; }

private:
	void InitializeDXGIDevice();

	void InitializeCommand();

	void CreateSwapChain();

	void CreateFinalRenderTargets();

	void CreateFence();

private:
	static WinApp* winApp_;

	// DXGIファクトリーの生成
	static IDXGIFactory7* dxgiFactory_;

	// 使用するアダプタ用の変数
	static IDXGIAdapter4* useAdapter_;

	// D3D12Deviceの生成
	static ID3D12Device* device_;

	// コマンドキュー生成
	static ID3D12CommandQueue* commandQueue_;

	// コマンドアロケータの生成
	static ID3D12CommandAllocator* commandAllocator_;

	// コマンドリストを生成する
	static ID3D12GraphicsCommandList* commandList_;

	// スワップチェーン
	static IDXGISwapChain4* swapChain_;
	static DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;

	// ディスクリプタヒープの生成
	static ID3D12DescriptorHeap* rtvDescriptorHeap_;
	static D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_;

	// SRV
	static ID3D12DescriptorHeap* srvDescriptorHeap_;

	static inline ID3D12DescriptorHeap* dsvDescriptorHeap_;

	// RTVを２つ作るのでディスクリプタを２つ用意
	static D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	static ID3D12Resource* swapChainResources_[2];

	// Fence
	static ID3D12Fence* fence_;
	static UINT64 fenceValue_;
	static HANDLE fenceEvent_;

	static int32_t backBufferWidth_;
	static int32_t backBufferHeight_;

	static inline D3D12_RESOURCE_BARRIER barrier_{};

	static HRESULT hr_;

	ID3D12DescriptorHeap* CreateDescriptorHeap(
	    ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors,
	    bool shaderVisible);

	ID3D12Resource* CreateDepthStencilTextureResource(int32_t width, int32_t height);

	// 深度リソース
	static inline ID3D12Resource* depthStencilResource_;
};