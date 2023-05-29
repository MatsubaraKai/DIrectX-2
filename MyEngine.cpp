#include "MyEngine.h"
#include <assert.h>

IDxcBlob* CreateEngine::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler)
{
	//���ꂩ��V�F�[�_�[���R���p�C������|�����O�ɏo��
	Log(ConvertString(std::format(L"Begin CompileShader, path:{},profile:{}\n", filePath, profile)));
	//hlsl�t�@�C����ǂ�
	IDxcBlobEncoding* shaderSource = nullptr;
	dxCommon_->SetHr(dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource));
	//�łȂ������猈�߂�
	assert(SUCCEEDED(dxCommon_->GetHr()));
	//�ǂݍ��񂾃t�@�C���̓��e��ݒ肷��
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;
	LPCWSTR arguments[] =
	{
		filePath.c_str(),//�R���p�C���Ώۂ�hlsl�t�@�C����
		L"-E",L"main",//�G���g���[�|�C���g�̎w��B��{�I��main�ȊO�ɂ͂��Ȃ�
		L"-T",profile,//ShaderProflie�̐ݒ�
		L"-Zi",L"-Qembed_debug",//�f�o�b�O�p�̏��𖄂ߍ���
		L"-Od", //�œK�����O���Ă���
		L"-Zpr",//���������C�A�E�g�͍s�D��
	};

	//���ۂ�Shader���R���p�C������
	IDxcResult* shaderResult = nullptr;
	dxCommon_->SetHr(dxcCompiler->Compile(
		&shaderSourceBuffer,//�ǂݍ��񂾃t�@�C��
		arguments,//�R���p�C���I�v�V����
		_countof(arguments),//�R���p�C���I�v�V�����̐�
		includeHandler, // include���܂܂ꂽ���X
		IID_PPV_ARGS(&shaderResult)//�R���p�C������
	));
	//�R���p�C���G���[�ł͂Ȃ�dxc���N���ł��Ȃ��Ȃǒv���I�ȏ�
	assert(SUCCEEDED(dxCommon_->GetHr()));

	//�x���E�G���[���o���烍�O�ɏo���Ď~�߂�
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0)
	{
		Log(shaderError->GetStringPointer());
		//�x���E�G���[�_�����
		assert(false);
	}

	//�R���p�C�����ʂ�����s�p�̃o�C�i���������擾
	IDxcBlob* shaderBlob = nullptr;
	dxCommon_->SetHr(shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr));
	assert(SUCCEEDED(dxCommon_->GetHr()));
	//�����������O���o��
	Log(ConvertString(std::format(L"Compile Succeeded, path:{},profile:{}\n", filePath, profile)));
	//�����g��Ȃ����\�[�X���J��
	shaderSource->Release();
	shaderResult->Release();
	//���s�p�̃o�C�i����ԋp
	return shaderBlob;
}

void CreateEngine::InitializeDxcCompiler()
{
	HRESULT hr;
	dxcUtils_ = nullptr;
	dxcCompiler_ = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));
	//�����_��include�͂��Ȃ����Ainclude�ɑΉ����邽�߂̐ݒ���s���Ă���
	includeHandler_ = nullptr;
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));
}

void CreateEngine::CreateRootSignature()
{
	//RootSignature�쐬
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//�V���A���C�Y���ăo�C�i���ɂ���
	signatureBlob_ = nullptr;
	errorBlob_ = nullptr;
	HRESULT hr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(dxCommon_->GetHr()))
	{
		Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}
	//�o�C�i�������ɐ���
	rootSignature_ = nullptr;
	hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob_->GetBufferPointer(),
		signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));
}

void CreateEngine::CreateInputlayOut()
{
	inputElementDescs_[0].SemanticName = "POSITION";
	inputElementDescs_[0].SemanticIndex = 0;
	inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs_);
}

void CreateEngine::BlendState()
{
	//���ׂĂ̐F�v�f����������
	blendDesc_.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
}

void CreateEngine::RasterizerState()
{
	//���ʁi���v���j��\�����Ȃ�
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
	//�O�p�`�̒���h��Ԃ�
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;

	//Shader���R���p�C������
	vertexShaderBlob_ = CompileShader(L"Object3d.VS.hlsl",
		L"vs_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	assert(vertexShaderBlob_ != nullptr);


	pixelShaderBlob_ = CompileShader(L"Object3d.PS.hlsl",
		L"ps_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	assert(pixelShaderBlob_ != nullptr);
}

void CreateEngine::InitializePSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_;//RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc_;//Inputlayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob_->GetBufferPointer(),
		vertexShaderBlob_->GetBufferSize() };//vertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob_->GetBufferPointer(),
		pixelShaderBlob_->GetBufferSize() };//pixcelShader
	graphicsPipelineStateDesc.BlendState = blendDesc_;//BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc_;//rasterizerState
	//��������RTV�̏��
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//���p����g�|���W�i�`��j�̃^�C�v�B�O�p�`
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//�ǂ̂悤�ɉ�ʂɐF��ł����ނ̂��̐ݒ�i�C�ɂ��Ȃ��ǂ��j
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//���ۂɐ���
	graphicsPipelineState_ = nullptr;
	HRESULT hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));
}

void CreateEngine::VertexResource()
{
	//���_���\�[�X�p�̃q�[�v�̐ݒ�
	D3D12_HEAP_PROPERTIES uplodeHeapProperties{};
	uplodeHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeap���g��
	//���_���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	//�o�b�t�@���\�[�X�B�e�N�X�`���̏ꍇ�͂܂��ʂ̐ݒ������
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeof(Vector4) * 3;//���\�[�X�T�C�Y�@�����vector4���l����
	//�o�b�t�@�̏ꍇ�͂����͂P�ɂ��錈�܂�
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	//�o�b�t�@�̏ꍇ�͂���ɂ��錈�܂�
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	HRESULT hr;

	//���ۂɒ��_���\�[�X�����
	hr = dxCommon_->GetDevice()->CreateCommittedResource(&uplodeHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertexResource_));
	assert(SUCCEEDED(hr));
	//���\�[�X�̐擪�̃A�h���X����g��
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//�g�p���郊�\�[�X�̃T�C�Y�͒��_3���̃T�C�Y
	vertexBufferView_.SizeInBytes = sizeof(Vector4) * 3;
	//1���_������̃T�C�Y
	vertexBufferView_.StrideInBytes = sizeof(Vector4);
	//�������ނ��߂̃A�h���X���擾
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

}

void CreateEngine::ViewPort()
{
	//�N���C�A���g�̈�̃T�C�Y�ƈꏏ�ɂ��ĉ�ʑS�̂ɕ\��
	viewport_.Width = WinApp::kClientWidth;
	viewport_.Height = WinApp::kClientHeight;
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;
}

void CreateEngine::ScissorRect()
{
	//�V�U�[�Z�`
	scissorRect_.left = 0;
	scissorRect_.right = WinApp::kClientWidth;
	scissorRect_.top = 0;
	scissorRect_.bottom = WinApp::kClientHeight;
}

void CreateEngine::Initialize()
{
	for (int i = 0; i < 10; i++)
	{
		triangle_[i] = new CreateTriangle();
		triangle_[i]->Initialize(dxCommon_);
	}
}

void CreateEngine::Initialization(WinApp* win, const wchar_t* title, int32_t width, int32_t height)
{
	dxCommon_->Initialization(win, title, win->kClientWidth, win->kClientHeight);

	InitializeDxcCompiler();

	CreateRootSignature();

	CreateInputlayOut();

	BlendState();

	RasterizerState();

	InitializePSO();

	ViewPort();

	ScissorRect();
}


void CreateEngine::BeginFrame()
{
	dxCommon_->PreDraw();
	//viewport��ݒ�
	dxCommon_->GetCommandList()->RSSetViewports(1, &viewport_);
	//scirssor��ݒ�
	dxCommon_->GetCommandList()->RSSetScissorRects(1, &scissorRect_);
	//RootSignature��ݒ�BPSO�Ƃ͕ʓr
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_);
	//PSO��ݒ�
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_);
}

void CreateEngine::EndFrame()
{
	dxCommon_->PostDraw();
}

void CreateEngine::Finalize()
{
	for (int i = 0; i < 10; i++)
	{
		triangle_[i]->Finalize();
	}
	graphicsPipelineState_->Release();
	signatureBlob_->Release();
	if (errorBlob_) {
		errorBlob_->Release();
	}
	rootSignature_->Release();
	pixelShaderBlob_->Release();
	vertexShaderBlob_->Release();
	dxCommon_->Finalize();
}

void CreateEngine::Update()
{
}

void CreateEngine::DrawTriangle(const Vector4& a, const Vector4& b, const Vector4& c)
{
	triangleCount_++;
	triangle_[triangleCount_]->Draw(a, b, c);
	if (triangleCount_ >= 9) {
		triangleCount_ = 0;
	}
}

WinApp* CreateEngine::win_;
DirectXCommon* CreateEngine::dxCommon_;