#pragma once
#include <chrono>
#include <cstdlib>
#include <dxgi1_6.h>
#include "WinApp.h"
#include "ConvertString.h"
#include "externals/DirectXTex/DirectXTex.h"

class DirectXCommon 
{
public:
	void Initialization(const wchar_t* title, int32_t backBufferWidth = WinApp::kClientWidth, int32_t backBufferHeight = WinApp::kClientHeight);

	void ImGuiInitialize();

	void PreDraw();
	void PostDraw();

	void ClearRenderTarget();
	void Finalize();

	static ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

	WinApp* GetWin() { return WinApp::GetInstance(); }

	HRESULT GetHr() { return  hr_; }
	void SetHr(HRESULT a) { this->hr_ = a; }
	
	ID3D12Device* GetDevice() { return device_; }
	ID3D12GraphicsCommandList* GetCommandList() { return commandList_; }

	ID3D12DescriptorHeap* GetSrvDescriptiorHeap() { return srvDescriptorHeap_; }
	ID3D12DescriptorHeap* GetDsvDescriptiorHeap() { return dsvDescriptorHeap_; }
	D3D12_RENDER_TARGET_VIEW_DESC getRtvDesc() { return rtvDesc_; }

private:
	void InitializeDXGIDevice();

	void CreateSwapChain();

	void InitializeCommand();

	void CreateFinalRenderTargets();

	void CreateFence();

	ID3D12Resource* CreateDepthStenciltextureResource(ID3D12Device* device, int32_t width, int32_t height);

	void CreateDepthStensil();

private:
	//DXGIファクトリーの生成
	IDXGIFactory7* dxgiFactory_;

	//使用するアダプタ用の変数
	IDXGIAdapter4* useAdapter_;

	//D3D12Deviceの生成
	ID3D12Device* device_;

	//コマンドキュー生成
	ID3D12CommandQueue* commandQueue_;

	//コマンドアロケータの生成
	ID3D12CommandAllocator* commandAllocator_;

	//コマンドリストを生成する
	ID3D12GraphicsCommandList* commandList_;

	//スワップチェーン
	IDXGISwapChain4* swapChain_;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;

	//ディスクリプタヒープの生成
	ID3D12DescriptorHeap* rtvDescriptorHeap_;//rtv用
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_;
	ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	ID3D12DescriptorHeap* srvDescriptorHeap_;//srv用

	//RTVを２つ作るのでディスクリプタを２つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	ID3D12Resource* swapChainResources_[2];

	//Fence
	ID3D12Fence* fence_;
	UINT64 fenceValue_;
	HANDLE fenceEvent_;

	int32_t backBufferWidth_;
	int32_t backBufferHeight_;

	D3D12_RESOURCE_BARRIER barrier_{};

	HRESULT hr_;

	ID3D12Resource* depthStencilResource_;
	ID3D12DescriptorHeap* dsvDescriptorHeap_;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvhandle_;
};