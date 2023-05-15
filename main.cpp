#include <Windows.h>
#include <cstdint>
#include <string>
#include <format>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>
#include <dxcapi.h>
#include "Vector4.h"
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

//�E�B���h�E�v���V�[�W��
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	//���b�Z�[�W�ɉ����ăQ�[���̌ŗL�̏������s��
	switch (msg) {
		//�E�B���h�E���j�����ꂽ
	case WM_DESTROY:
		//OS�ɑ΂��āA�A�v���̏I����`����
		PostQuitMessage(0);
		return 0;
	}
	//�W���̃��b�Z�[�W�������s��
	return DefWindowProc(hwnd, msg, wparam, lparam);
}


// wstring����string�֕ϊ�����֐�
// string->wstring
std::wstring ConvertString(const std::string& str)
{
	if (str.empty())
	{
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0)
	{
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

// wstring->string
std::string ConvertString(const std::wstring& str)
{
	if (str.empty())
	{
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0)
	{
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}


void Log(const std::string& message)
{
	OutputDebugStringA(message.c_str());
}


IDxcBlob* CompileShader(
	// Compiler����Shader�t�@�C���ւ̃p�X
	const std::wstring& filePath,
	// Compiler�Ɏg�p����Profile
	const wchar_t* profile,
	// �������Ő����������̂�3��
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler)
{
	// 1.hlsl�t�@�C����ǂ�
	// ���ꂩ��V�F�[�_�[���R���p�C������|�����O�ɏo��
	Log(ConvertString(std::format(L"Begin CompileShader,path{},profile{}\n", filePath, profile)));
	// hlsl�t�@�C����ǂݍ���
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	// �ǂݍ��߂Ȃ�������~�߂�
	assert(SUCCEEDED(hr));
	// �ǂݍ��񂾃t�@�C���̓��e��ݒ肷��
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8; // UTFB�̕����R�[�h�ł��邱�Ƃ�ʒm


	// 2.Compile����
	LPCWSTR arguments[] = {
		filePath.c_str(),
		L"-E",L"main",
		L"-T",profile,
		L"-Zi",L"-Qembed_debug",
		L"-Od",
		L"-Zpr",
	};
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,
		arguments,
		_countof(arguments),
		includeHandler,
		IID_PPV_ARGS(&shaderResult)
	);
	assert(SUCCEEDED(hr));


	// 3.�x���E�G���[���łĂ��Ȃ����m�F����
	// �x���E�G���[���o�Ă��烍�O�ɏo���Ď~�߂�
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0)
	{
		Log(shaderError->GetStringPointer());
		assert(false);
	}


	// 4.Compile���ʂ��󂯎���ĕԂ�
	// �R���p�C�����ʂ�����s�p�̃o�C�i���������擾
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// �����������O���o��
	Log(ConvertString(std::format(L"Compile Succeeded,path:{},profile:{}\n", filePath, profile)));
	// �����g��Ȃ����\�[�X�����
	shaderSource->Release();
	shaderResult->Release();
	// ���s�p�̃o�C�i����ԋp
	return shaderBlob;
}


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	OutputDebugStringA("Hello,DirectX!!\n");

	//�E�B���h�E�T�C�Y
	const int32_t kClientWidth = 1280;//����
	const int32_t kClientHeight = 720;//�c��

	//�E�B���h�E�N���X�̐ݒ�
	WNDCLASS wc{};
	wc.lpfnWndProc = WindowProc; //�E�B���h�E�v���V�[�W����ݒ�
	wc.lpszClassName = L"DirectXGame";         //�E�B���h�E�N���X��
	wc.hInstance = GetModuleHandle(nullptr);   //�E�B���h�E�n���h��
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);  //�J�[�\���w��

	//�E�B���h�E�N���X��OS�ɓo�^����
	RegisterClass(&wc);
	//�E�B���h�E�T�C�Y�o X���W Y���W ���� �c�� �p
	RECT wrc = { 0,0,kClientWidth,kClientHeight };
	//�����ŃT�C�Y��␳����
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//�E�B���h�E�I�u�W�F�N�g�̐���
	HWND hwnd = CreateWindow(wc.lpszClassName,//�N���X��
		L"DirectXGame",       //�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,  //�W���I�ȃE�B���h�E�X�^�C��
		CW_USEDEFAULT,        //�\��X���W (OS�ɔC����)
		CW_USEDEFAULT,        //�\��Y���W (OS�ɔC����)
		wrc.right - wrc.left, //�E�B���h�E����
		wrc.bottom - wrc.top, //�E�B���h�E�c��
		nullptr,              //�e�E�B���h�E�n���h��
		nullptr,              //���j���[�n���h��
		wc.hInstance,         //�Ăяo���A�v���P�[�V�����n���h��
		nullptr);	          //�I�v�V����

#ifdef _DEBUG
	ID3D12Debug1* debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		// �f�o�b�O���C���[��L��������
		debugController->EnableDebugLayer();
		// �����GPU���ł��`�F�b�N���s���悤�ɂ���
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif // _DEBUG

	ShowWindow(hwnd, SW_SHOW);

	//DXGI�t�@�N�g���[�̐���
	IDXGIFactory7* dxgiFactory = nullptr;
	// HRESULT��Window�n�̃G���[�R�[�h�ł���
	//�֐��������������ǂ�����SUCCEEDED�}�N���Ŕ���o����
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	//�������̍��{�I�ȕ����ŃG���[���o���ꍇ�̓v���O�������Ԉ���Ă��邩�A�ǂ��ɂ��o���Ȃ��ꍇ�������̂�assert�ɂ��Ă���
	assert(SUCCEEDED(hr));
	//�g�p����A�_�v�^�p�̕ϐ��B�ŏ���nullptr�����Ă���
	IDXGIAdapter4* useAdapter = nullptr;
	//�ǂ����ɃA�_�v�^�𗊂�
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; ++i)
	{
		//�A�_�v�^�[�̏����擾����
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));//�擾�o���Ȃ��͈̂�厖
		//�\�t�g�E�F�A�A�_�v�^�łȂ���΍̗p
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			//�̗p�����A�_�v�^�̏������O�ɏo�́Bwstring�̕��Ȃ̂Œ���
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr;//�\�t�g�E�F�A�A�_�v�^�̏ꍇ�͌��Ȃ��������ɂ���
	}
	//�K�؂ȃA�_�v�^��������Ȃ������̂ŋN���ł��Ȃ�
	assert(useAdapter != nullptr);


	ID3D12Device* device = nullptr;
	//�@�\���x���ƃ��O�o�͗p�̕�����
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	//�������ɐ����ł��邩�����Ă���
	for (size_t i = 0; i < _countof(featureLevels); ++i)
	{
		//�̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
		hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));
		//�w�肵���@�\���x���Ńf�o�C�X�������o���������m�F
		if (SUCCEEDED(hr))
		{
			//�����o�����̂Ń��O�o�͂��s���ă��[�v�𔲂���
			Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	//�f�o�C�X�̐��������܂������Ȃ������̂ŋN���o���Ȃ�
	assert(device != nullptr);
	Log("Complete create D3D12Device!!!\n");//�����������̃��O������

#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue))))
	{
		// ��΂��G���[��STOP
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// �G���[����STOP
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// �x������STOP
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		// ���
		infoQueue->Release();

		// �}�����郁�b�Z�[�W��ID
		D3D12_MESSAGE_ID denyIds[] = {
			// window11�ł�DXGI�f�o�b�O���C���[��DX12�f�o�b�O���C���[�̑��ݍ�p�o�O�ɂ��G���[���b�Z�[�W
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//�}�����郌�x��
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		// �w�肵�����b�Z�[�W��}��
		infoQueue->PushStorageFilter(&filter);
	}
#endif // _DEBUG


	MSG msg{};


	// �R�}���h�L���[�𐶐�����
	ID3D12CommandQueue* commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	// �R�}���h�L���[�̐��������܂������Ȃ������̂ŋN���ł��Ȃ�
	assert(SUCCEEDED(hr));

	// �R�}���h�A�v���P�[�^�𐶐�����
	ID3D12CommandAllocator* commandAllocator = nullptr;
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	// �R�}���h�A���P�[�^�̐��������܂������Ȃ������̂ŋN���ł��Ȃ�
	assert(SUCCEEDED(hr));

	// �R�}���h���X�g�̐���
	ID3D12GraphicsCommandList* commandList = nullptr;
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList));
	// �R�}���h���X�g�̐��������܂������Ȃ������̂ŋN���ł��Ȃ�
	assert(SUCCEEDED(hr));

	// �X���b�v�`�F�[���𐶐�����
	IDXGISwapChain4* swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = kClientWidth;   // ��ʂ̕��B�E�B���h�E�̃N���C�A���g�̈�𓯂����̂ɂ��Ă���
	swapChainDesc.Height = kClientHeight; // ��ʂ̍����B�E�B���h�E�̃N���C�A���g�̈�𓯂����̂ɂ��Ă���
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // �F�̌`��
	swapChainDesc.SampleDesc.Count = 1; // �}���`�T���v�����Ȃ�
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // �`��^�[�Q�b�g�Ƃ��ė��p
	swapChainDesc.BufferCount = 2; // �_�u���o�b�t�@
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // ���j�^�[�ɂ�������A���g��j��
	// �R�}���h�L���[�A�E�B���h�E�n���h���A�ݒ肵�ēn���Đ���
	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, hwnd, &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain));
	assert(SUCCEEDED(hr));

	// �f�B�X�N���v�^�q�[�v�̐���
	ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // �����_�[�^�[�Q�b�g�r���[�p
	rtvDescriptorHeapDesc.NumDescriptors = 2; // �_�u���o�b�t�@�p��2�B�����Ă���
	hr = device->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap));
	assert(SUCCEEDED(hr));

	// Swapchain����Resource�����������Ă���
	ID3D12Resource* swapChainResources[2] = { nullptr };
	hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
	assert(SUCCEEDED(hr));
	hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
	assert(SUCCEEDED(hr));


	//�����l0��Fence�����
	ID3D12Fence* fence = nullptr;
	uint64_t fenceValue = 0;
	hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));


	//Fence��Signal�������߂ɃC�x���g���쐬����
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);



	// dxpCompiler��������
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	// �����_��include�͂��Ȃ����Ainclude�ɑΉ����邽�߂̐ݒ���s���Ă���
	IDxcIncludeHandler* includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));


	// RootSignature�쐬
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	// �o�C�i�������Ƃɐ���
	ID3D12RootSignature* rootSignature = nullptr;
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);


	// BlendState�̐ݒ�
	D3D12_BLEND_DESC blendDesc{};
	// ���ׂĂ̐F�v�f����������
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;


	// RasterizerState�̐ݒ�
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// ���ʂ�\�����Ȃ�
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// �O�p�`�̒���h��Ԃ�
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shader���R���p�C��
	IDxcBlob* vertexShaderBlob = CompileShader(L"Object3d.VS.hlsl",
		L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);

	IDxcBlob* pixelShaderBlob = CompileShader(L"Object3d.PS.hlsl",
		L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);


	// PSO�𐶐�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature;
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	//��������RTV�̏��
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//���p����g�|���W(�`��)�̃^�C�v�B�O�p�`
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// �ǂ̂悤�ɉ�ʂɐF��ł����ނ��̐ݒ�
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// ���ۂɐ���
	ID3D12PipelineState* graphicsPipelineState = nullptr;
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));


	// ���_�����[�X�p�̃q�[�v�̐ݒ�
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	// ���_�����[�X�̓���
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	// �o�b�t�@���\�[�X�B�e�N�X�`���̏ꍇ�͂܂��ʂ̐ݒ������
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeof(Vector4) * 3;
	// �o�b�t�@�̏ꍇ�͂�����1�ɂ��錈�܂�
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	// �o�b�t�@�̏ꍇ����ɂ��錈�܂�
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// ���ۂɒ��_���\�[�X�����
	ID3D12Resource* vertexResource = nullptr;
	hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));


	// ���_�o�b�t�@�r���[���쐬����
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	// ���\�[�X��擪�̃A�h���X����g��
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	// ���_���\�[�X�̃T�C�Y�͒��_3���̃T�C�Y
	vertexBufferView.SizeInBytes = sizeof(Vector4) * 3;
	// �P���_������̃T�C�Y
	vertexBufferView.StrideInBytes = sizeof(Vector4);

	// ���_���\�[�X�Ƀf�[�^����������
	Vector4* vertexData = nullptr;
	// �������ނ��߂̃A�h���X���擾
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//����
	vertexData[0] = { -0.5f,-0.5f,0.0f,1.0f };
	//��
	vertexData[1] = { 0.0f,0.5f,0.0f,1.0f };
	//�E��
	vertexData[2] = { 0.5f,-0.5f,0.0f,1.0f };

	// �r���[�|�[�g
	D3D12_VIEWPORT viewport{};
	// �N���C�A���g�̈�̃T�C�Y�ƈꏏ�ɂ��ĉ�ʑS�̂ɕ\��
	viewport.Width = kClientWidth;
	viewport.Height = kClientHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// �V�U�[��`
	D3D12_RECT scissorRect{};
	// ��{�I�Ƀr���[�|�[�g�Ɠ�����`���\�������悤�ɂ���
	scissorRect.left = 0;
	scissorRect.right = kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = kClientHeight;


	//�E�B���h�E�́~�{�^�����������܂Ń��[�v
	while (msg.message != WM_QUIT) {
		//Window�Ƀ��b�Z�[�W�����Ă���ŗD��ŏ���������
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//�Q�[���̏���

			// RTV�̐ݒ�
			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
			rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // �o�͌��ʂ�SRGB�ɕϊ����ď�������
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2d�e�N�X�`���Ƃ��ď�������
			// �f�B�X�N���v�^�̐擪���擾����
			D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			// RTV��2���̂Ńf�B�X�N���v�^��2�p��
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
			// �܂�1�ڂ����B��ڂ͍ŏ��̍��B���ꏊ��������Ŏw�肵�Ă�����K�v������
			rtvHandles[0] = rtvStartHandle;
			device->CreateRenderTargetView(swapChainResources[0], &rtvDesc, rtvHandles[0]);
			// 2�ڂ̃f�B�X�N���v�^�n���h���𓾂�(����)
			rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			// 2�ڂ����
			device->CreateRenderTargetView(swapChainResources[1], &rtvDesc, rtvHandles[1]);


			// ���ꂩ�珑�����ރo�b�N�o�b�t�@�̃C���f�b�N�X���擾
			UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();
			// TransitionBarrier�̐ݒ�
			D3D12_RESOURCE_BARRIER barrier{};
			// ����̃o���A��Transition
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			//None�ɂ��Ă���
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			//�o���A�𒣂郊�\�[�X�B���݃o�b�t�@�ɑ΂��čs��
			barrier.Transition.pResource = swapChainResources[backBufferIndex];
			//�J�ڑO��ResourseState
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			//�J�ڌ�
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			// TransitionBarrier�𒣂�
			commandList->ResourceBarrier(1, &barrier);


			// �`����RTV��ݒ肷��
			commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, nullptr);
			// �w�肵���F�ŉ�ʑS�̂��N���A
			float clearColor[] = { 0.1f,0.25f,0.5f,1.0f }; // ���ۂ��F�BRGBA�̏�
			commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);


			// �R�}���h��ς�
			commandList->RSSetViewports(1, &viewport);
			commandList->RSSetScissorRects(1, &scissorRect);
			// RootSignature��ݒ�BPSO�ɂ��Ă��邯�Ǖʓr�ݒ肪�K�v
			commandList->SetGraphicsRootSignature(rootSignature);
			commandList->SetPipelineState(graphicsPipelineState);
			commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
			//�`���ݒ�BPSO��ݒ肵�Ă�����̂Ƃ͂܂��ʁB�������̂�ݒ肷��ƍl���Ă����Ηǂ�
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			// �`��!(DrawColl)�B3���_��1�̃C���X�^���X�B�C���X�^���X�ɂ��Ă͍���
			commandList->DrawInstanced(3, 1, 0, 0);


			//��ʂɕ`�������͏I���A��ʂɉf���̂ŁA��Ԃ�J��
			// �����RenderTarget����Present�ɂ���
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			// TransitionBarrier�𒣂�
			commandList->ResourceBarrier(1, &barrier);

			// �R�}���h���X�g�̓��e�m�肳����B���ׂẴR�}���h��ς�ł���close���邱�ƁB
			hr = commandList->Close();
			assert(SUCCEEDED(hr));


			// GPU�ɃR�}���h���X�g�̎��s���s�킹��
			ID3D12CommandList* commandLists[] = { commandList };
			commandQueue->ExecuteCommandLists(1, commandLists);
			// GPU��OS�̉�ʌ������s���悤�ʒm
			swapChain->Present(1, 0);

			// Fence�̒l���X�V
			fenceValue++;
			// GPU�����ɂ��ǂ蒅�������ɁAFence�̒l���w�肵���l�ɑ������悤�ɃV�O�i���𑗂�
			commandQueue->Signal(fence, fenceValue);

			// Fence�̒l���w�肵��signal�l�ɂ��ǂ蒅���Ă��邩�m�F����
			// GetConpletedValue�̏����l��Fence�쐬���ɓn���������l
			if (fence->GetCompletedValue() < fenceValue)
			{
				//�w�肵��signal�ɂ��ǂ蒅���Ă��Ȃ��̂ł��ǂ蒅���܂ő҂悤�ɃC�x���g��ݒ�
				fence->SetEventOnCompletion(fenceValue, fenceEvent);
				//�C�x���g��҂�
				WaitForSingleObject(fenceEvent, INFINITE);
			}

			// ���̃t���[���p�̃R�}���h���X�g������
			hr = commandAllocator->Reset();
			assert(SUCCEEDED(hr));
			hr = commandList->Reset(commandAllocator, nullptr);
			assert(SUCCEEDED(hr));
		}
	}

	CloseHandle(fenceEvent);
	fence->Release();
	rtvDescriptorHeap->Release();
	swapChainResources[0]->Release();
	swapChainResources[1]->Release();
	swapChain->Release();
	commandList->Release();
	commandAllocator->Release();
	commandQueue->Release();
	device->Release();
	useAdapter->Release();
	dxgiFactory->Release();

	// �������
	vertexResource->Release();
	graphicsPipelineState->Release();
	signatureBlob->Release();
	if (errorBlob)
	{
		errorBlob->Release();
	}
	rootSignature->Release();
	pixelShaderBlob->Release();
	vertexShaderBlob->Release();
#ifdef _DEBUG
	debugController->Release();
#endif // _DEBUG
	CloseWindow(hwnd);

	// ���\�[�X�`�F�b�N
	IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
	{
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}

	return 0;
}
