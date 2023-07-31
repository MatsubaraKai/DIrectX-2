#include "DirectXCommon.h"
#include <algorithm>
#include <cassert>
#include <thread>
#include <timeapi.h>
#include <vector>
#include <dxgidebug.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Winmm.lib")

WinApp* DirectXCommon::winApp_;

//DXGI�t�@�N�g���[�̐���
IDXGIFactory7* DirectXCommon::dxgiFactory_;

//�g�p����A�_�v�^�p�̕ϐ�
IDXGIAdapter4* DirectXCommon::useAdapter_;

//D3D12Device�̐���
ID3D12Device* DirectXCommon::device_;

//�R�}���h�L���[����
ID3D12CommandQueue* DirectXCommon::commandQueue_;

//�R�}���h�A���P�[�^�̐���
ID3D12CommandAllocator* DirectXCommon::commandAllocator_;

//�R�}���h���X�g�𐶐�����
ID3D12GraphicsCommandList* DirectXCommon::commandList_;

//�X���b�v�`�F�[��
IDXGISwapChain4* DirectXCommon::swapChain_;
DXGI_SWAP_CHAIN_DESC1 DirectXCommon::swapChainDesc_{};

//�f�B�X�N���v�^�q�[�v�̐���
ID3D12DescriptorHeap* DirectXCommon::rtvDescriptorHeap_;
D3D12_RENDER_TARGET_VIEW_DESC DirectXCommon::rtvDesc_{};

ID3D12DescriptorHeap* DirectXCommon::srvDescriptorHeap_;

//RTV���Q���̂Ńf�B�X�N���v�^���Q�p��
D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::rtvHandles_[2];
ID3D12Resource* DirectXCommon::swapChainResources_[2];

//Fence
ID3D12Fence* DirectXCommon::fence_;
UINT64 DirectXCommon::fenceValue_;
HANDLE DirectXCommon::fenceEvent_;

int32_t DirectXCommon::backBufferWidth_;
int32_t DirectXCommon::backBufferHeight_;

HRESULT DirectXCommon::hr_;

void DirectXCommon::Initialization(WinApp* win, const wchar_t* title, int32_t backBufferWidth, int32_t backBufferHeight)
{

	winApp_ = win;
	backBufferWidth_ = backBufferWidth;
	backBufferHeight_ = backBufferHeight;

	winApp_->CreateWindowView(title, 1280, 720);

	// DXGI�f�o�C�X������
	InitializeDXGIDevice();

	// �R�}���h�֘A������
	InitializeCommand();

	// �X���b�v�`�F�[���̐���
	CreateSwapChain();

	// �����_�[�^�[�Q�b�g����
	CreateFinalRenderTargets();

	// �t�F���X����
	CreateFence();

	ImGuiInitialize();
}

void DirectXCommon::ImGuiInitialize()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp_->GetHwnd());
	ImGui_ImplDX12_Init(device_,
		swapChainDesc_.BufferCount,
		rtvDesc_.Format,
		srvDescriptorHeap_,
		srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart());
}

void DirectXCommon::InitializeDXGIDevice()
{
	//DXGI�t�@�N�g���[�̐���
	dxgiFactory_ = nullptr;
	hr_ = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(hr_));

	//�g�p����A�_�v�^�p�̕ϐ�
	useAdapter_ = nullptr;
	//���ɃA�_�v�^�𗊂�
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) !=
		DXGI_ERROR_NOT_FOUND; i++) {
		//�A�_�v�^�[���̎擾
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr_ = useAdapter_->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr_));

		//�\�t�g�E�F�A�A�_�v�^�łȂ���΍̗p
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//�̗p�A�_�v�^�̏������O�ɏo��
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter_ = nullptr;//�\�t�g�E�F�A�A�_�v�^�̏ꍇ�͖���
	}
	//�K�؂ȃA�_�v�^���Ȃ����ߋN�����Ȃ�
	assert(useAdapter_ != nullptr);

	//D3D12Device�̐���
	device_ = nullptr;
	//�@�\���x���ƃ��O�o��
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelString[] = { "12.2","12.1","12.0" };
	//�������ɐ����ł��邩�m�F
	for (size_t i = 0; i < _countof(featureLevels); i++) {
		//�̗p�����A�_�v�^�[�Ńf�o�C�X����
		hr_ = D3D12CreateDevice(useAdapter_, featureLevels[i], IID_PPV_ARGS(&device_));
		//�w�肵���@�\���x���̃f�o�C�X�����ɐ����������m�F
		if (SUCCEEDED(hr_)) {
			//�����ł����̂Ń��O�o�͂����ă��[�v����
			Log(std::format("FeatureLevel : {}\n", featureLevelString[i]));
			break;
		}
	}

	//�f�o�C�X�������s�̈׋N�����Ȃ�
	assert(device_ != nullptr);
	Log("Complete create D3D12Device!!\n");//�������������O

#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//���o�C�G���[�Ŏ~�܂�
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//�G���[�Ŏ~�܂�
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//�x�����Ɏ~�܂�
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		//�}�����郁�b�Z�[�WID
		D3D12_MESSAGE_ID denyIds[] = {
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//�}�����郌�x��
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;

		//�w�肵�����b�Z�[�W�̕\����}������
		infoQueue->PushStorageFilter(&filter);

		//���
		infoQueue->Release();
	}
#endif // _DEBUG
}

void DirectXCommon::InitializeCommand() {
	commandQueue_ = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr_ = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	//�R�}���h�L���[�̐������s�ׁ̈A�N�����Ȃ�
	assert(SUCCEEDED(hr_));

	//�R�}���h�A���P�[�^�̐���
	commandAllocator_ = nullptr;
	hr_ = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	assert(SUCCEEDED(hr_));

	//�R�}���h���X�g�𐶐�����
	commandList_ = nullptr;
	hr_ = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_, nullptr, IID_PPV_ARGS(&commandList_));
	//�R�}���h���X�g�������s�ׁ̈A�N�����Ȃ�
	assert(SUCCEEDED(hr_));
}

void DirectXCommon::CreateSwapChain() {
	//�X���b�v�`�F�[��
	swapChain_ = nullptr;
	swapChainDesc_.Width = WinApp::kClientWidth;//��ʂ̕�
	swapChainDesc_.Height = WinApp::kClientHeight;//��ʂ̍���
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//�F�̌`��
	swapChainDesc_.SampleDesc.Count = 1;//�}���`�T���v�����Ȃ�
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//�`��̃^�[�Q�b�g�Ƃ��ė��p
	swapChainDesc_.BufferCount = 2;//�_�u���o�b�t�@
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//���j�^�Ɉڂ����璆�g��j��

	//�R�}���h�L���[�A�E�B���h�E�n���h���A�ݒ��n���Đ���
	hr_ = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_, winApp_->GetHwnd(), &swapChainDesc_, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain_));
	assert(SUCCEEDED(hr_));

	//RTV�p�f�B�X�N���v�^�q�[�v�̐���
	rtvDescriptorHeap_ = CreateDescriptorHeap(device_, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	//SwapChain����Resource�����������Ă���
	swapChainResources_[0] = { nullptr };
	swapChainResources_[1] = { nullptr };
	hr_ = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResources_[0]));
	//�擾�ł��Ȃ���΋N�����Ȃ�
	assert(SUCCEEDED(hr_));

	hr_ = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResources_[1]));
	assert(SUCCEEDED(hr_));

	//SRV�p�f�B�X�N���v�^�q�[�v�̐���
	srvDescriptorHeap_ = CreateDescriptorHeap(device_, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
}

ID3D12DescriptorHeap* DirectXCommon::CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
	//�f�B�X�N���v�^�q�[�v�̐���
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptionHeapDesc{};
	descriptionHeapDesc.Type = heapType;//�����_�[�^�[�Q�b�g�r���[�p
	descriptionHeapDesc.NumDescriptors = numDescriptors;//�_�u���o�b�t�@�p�ɓ�B�����Ă��ʂɂ��܂�Ȃ�
	descriptionHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device_->CreateDescriptorHeap(&descriptionHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	//�f�B�X�N���v�^�q�[�v���������s�ׁ̈A�N�����Ȃ�
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}

void DirectXCommon::CreateFinalRenderTargets() {
	//RTV�̐ݒ�
	rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//�o�͌��ʂ�SRGB�ɕϊ����ď�������
	rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2D�e�N�X�`���Ƃ��ď�������
	//�f�B�X�N���v�^�̐擪���擾����
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

	//�܂���ڂ����B��ڂ͍ŏ��̂Ƃ���ɍ��B���ꏊ��������Ŏw�肵�Ă�����K�v������
	rtvHandles_[0] = rtvStartHandle;
	device_->CreateRenderTargetView(swapChainResources_[0], &rtvDesc_, rtvHandles_[0]);
	//2�ڂ̃f�B�X�N���v�^�n���h���𓾂�i���͂Łj
	rtvHandles_[1].ptr = rtvHandles_[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//2�ڂ����
	device_->CreateRenderTargetView(swapChainResources_[1], &rtvDesc_, rtvHandles_[1]);
}

void DirectXCommon::CreateFence() {
	//�����l0��Fence�����
	fence_ = nullptr;
	fenceValue_ = 0;
	hr_ = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr_));

	//Fence��signal��҂��߂̃C�x���g���쐬����
	fenceEvent_ = CreateEvent(NULL, false, false, NULL);
	assert(fenceEvent_ != nullptr);
}

void DirectXCommon::PreDraw()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//�������ރo�b�N�o�b�t�@�̃C���f�b�N�X���擾
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();
	//�����barrier��Transition
	barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//None�ɂ���
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//barrier�Ώۂ̃��\�[�X�A�o�b�N�΂����ɑ΂��čs��
	barrier_.Transition.pResource = swapChainResources_[backBufferIndex];
	//�J�ڑO��resourcestate
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	//�J�ڌ��resourcestate
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrier�𒣂�
	commandList_->ResourceBarrier(1, &barrier_);
	//�`����RTV��ݒ肷��
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, nullptr);
	//�w�肵���F�ŉ�ʑS�̂��N���A����
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };//���ۂ��F�ARGBA��
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearColor, 0, nullptr);

	//�`��p��DescriptorHeap�̐ݒ�
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap_ };
	commandList_->SetDescriptorHeaps(1, descriptorHeaps);
}

void DirectXCommon::PostDraw() {
	hr_;
	//���ۂ�CommandList�̃R�}���h��ς�
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList_);
	//��ʕ`�揈���̏I���A��Ԃ�J��
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	//TransitionBarrier�𒣂�
	commandList_->ResourceBarrier(1, &barrier_);
	//�R�}���h���X�g�̓��e���m�肳����B�S�ẴR�}���h��ς�ł���close����
	hr_ = commandList_->Close();
	assert(SUCCEEDED(hr_));

	//GPU�ɃR�}���h���X�g�����s������
	ID3D12CommandList* commandLists[] = { commandList_ };
	commandQueue_->ExecuteCommandLists(1, commandLists);
	//GPU��OS�ɉ�ʂ̌������s���悤�ʒm����
	swapChain_->Present(1, 0);
	//Fence�̒l���X�V
	fenceValue_++;
	//GPU�������܂ŒH�蒅�������AFence�̒l���w�肵���l�ɑ������悤��signal�𑗂�
	commandQueue_->Signal(fence_, fenceValue_);

	if (fence_->GetCompletedValue() < fenceValue_) {
		//�w�肵��Signal�ɂ��ǂ蒅���Ă��Ȃ��̂ŁA���ǂ蒅���܂ő҂悤�ɃC�x���g��ݒ肷��
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		//�C�x���g�҂�
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	//���̃t���[���p�̃R�}���h���X�g������
	hr_ = commandAllocator_->Reset();
	assert(SUCCEEDED(hr_));
	hr_ = commandList_->Reset(commandAllocator_, nullptr);
	assert(SUCCEEDED(hr_));
}

void DirectXCommon::ClearRenderTarget() {
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();
	//�`����RTV��ݒ肷��
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, nullptr);
	//�w�肵���F�ŉ�ʑS�̂��N���A����
	float clearcolor[] = { 0.1f,0.25f,0.5f,1.0f };//���ۂ��F
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearcolor, 0, nullptr);

}

void DirectXCommon::Finalize() {
	CloseHandle(fenceEvent_);
	fence_->Release();
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	rtvDescriptorHeap_->Release();
	srvDescriptorHeap_->Release();
	swapChainResources_[0]->Release();
	swapChainResources_[1]->Release();
	swapChain_->Release();
	commandList_->Release();
	commandAllocator_->Release();
	commandQueue_->Release();
	device_->Release();
	useAdapter_->Release();
	dxgiFactory_->Release();
#ifdef DEBUG
	winApp_->GetdebugController()->Release();
#endif // DEBUG

	CloseWindow(winApp_->GetHwnd());
	////���\�[�X���[�N�`�F�b�N
	IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
	{
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}
}