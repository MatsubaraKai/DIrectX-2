#include "MyEngine.h"
#include <assert.h>

IDxcBlob* MyEngine::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler) {
	//これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader, path:{},profile:{}\n", filePath, profile)));
	
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	dxCommon_->SetHr(dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource));
	
	//読めなかったら決める
	assert(SUCCEEDED(dxCommon_->GetHr()));
	
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;
	LPCWSTR arguments[] = {
		filePath.c_str(),//コンパイル対象のhlslファイル名
		L"-E",L"main",//エントリーポイントの指定。基本的にmain以外にはしない
		L"-T",profile,//ShaderProflieの設定
		L"-Zi",L"-Qembed_debug",//デバッグ用の情報を埋め込む
		L"-Od", //最適化を外しておく
		L"-Zpr",//メモリレイアウトは行優先
	};
	
	//実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	dxCommon_->SetHr(dxcCompiler->Compile(
		&shaderSourceBuffer,//読み込んだファイル
		arguments,//コンパイルオプション
		_countof(arguments),//コンパイルオプションの数
		includeHandler, // includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)//コンパイル結果
	));
	
	//コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(dxCommon_->GetHr()));

	//警告・エラーが出たらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) 
	{
		Log(shaderError->GetStringPointer());
		//警告・エラーダメ絶対
		assert(false);
	}

	//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	dxCommon_->SetHr(shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr));
	assert(SUCCEEDED(dxCommon_->GetHr()));
	
	//成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded, path:{},profile:{}\n", filePath, profile)));
	
	//もう使わないリソースを開放
	shaderSource->Release();
	shaderResult->Release();
	
	//実行用のバイナリを返却
	return shaderBlob;
}

void MyEngine::InitializeDxcCompiler()
{
	HRESULT hr;
	dxcUtils_ = nullptr;
	dxcCompiler_ = nullptr;
	
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));
	
	//現時点でincludeはしないが、includeに対応するための設定を行っていく
	includeHandler_ = nullptr;
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));
}

void MyEngine::CreateRootSignature()
{
	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//RootParameter作成、複数設定可能な為、配列に
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド

	D3D12_DESCRIPTOR_RANGE descriptoraRange[1] = {};
	descriptoraRange[0].BaseShaderRegister = 0;//0から始まる
	descriptoraRange[0].NumDescriptors = 1;//数は1つ
	descriptoraRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptoraRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Descriptortableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixcelShaderを使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptoraRange;//tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptoraRange);//Tableで利用する数

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixcelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1;//レジスタ番号1を使う

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};//Samplerの設定
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//０～１の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのmipmapを使う
	staticSamplers[0].ShaderRegister = 0;//レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	descriptionRootSignature.pParameters = rootParameters;//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);//配列の長さ

	//シリアライズしてバイナリにする
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
	
	//バイナリを元に生成
	rootSignature_ = nullptr;
	hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob_->GetBufferPointer(),
		signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));
}

void MyEngine::CreateInputlayOut()
{
	inputElementDescs_[0].SemanticName = "POSITION";
	inputElementDescs_[0].SemanticIndex = 0;
	inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs_[1].SemanticName = "TEXCOORD";
	inputElementDescs_[1].SemanticIndex = 0;
	inputElementDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs_[2].SemanticName = "NORMAL";
	inputElementDescs_[2].SemanticIndex = 0;
	inputElementDescs_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs_);
}

void MyEngine::BlendState()
{
	//すべての色要素を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
}

void MyEngine::RasterizerState() 
{
	//裏面（時計回り）を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
	
	//三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;

	//Shaderをコンパイルする
	vertexShaderBlob_ = CompileShader(L"Object3d.VS.hlsl",
		L"vs_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	assert(vertexShaderBlob_ != nullptr);


	pixelShaderBlob_ = CompileShader(L"Object3d.PS.hlsl",
		L"ps_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	assert(pixelShaderBlob_ != nullptr);
}

void MyEngine::InitializePSO() 
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
	
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	
	//利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	
	//どのように画面に色を打ち込むのかの設定（気にしなく良い）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc_;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	
	//実際に生成
	graphicsPipelineState_ = nullptr;
	HRESULT hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));
}

void MyEngine::ViewPort()
{
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = WinApp::kClientWidth;
	viewport_.Height = WinApp::kClientHeight;
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;
}

void MyEngine::ScissorRect() 
{
	//シザー短形
	scissorRect_.left = 0;
	scissorRect_.right = WinApp::kClientWidth;
	scissorRect_.top = 0;
	scissorRect_.bottom = WinApp::kClientHeight;
}

void MyEngine::SettingDepth()
{
	//DepthStencilStateの設定
	depthStencilDesc_.DepthEnable = true;//有効化
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//書き込み
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;//比較関数、近ければ描画される
}

void MyEngine::Initialize(const wchar_t* title, int32_t width, int32_t height)
{
	dxCommon_ = new DirectXCommon();
	dxCommon_->Initialization(title, WinApp::GetInstance()->kClientWidth, WinApp::GetInstance()->kClientHeight);

	descriptorSizeDSV = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	descriptorSizeRTV = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	descriptorSizeSRV = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	InitializeDxcCompiler();

	CreateRootSignature();

	CreateInputlayOut();

	BlendState();

	RasterizerState();

	SettingDepth();

	InitializePSO();

	ViewPort();

	ScissorRect();
}


void MyEngine::BeginFrame()
{
	dxCommon_->PreDraw();
	
	//viewportを設定
	dxCommon_->GetCommandList()->RSSetViewports(1, &viewport_);
	
	//scirssorを設定
	dxCommon_->GetCommandList()->RSSetScissorRects(1, &scissorRect_);
	
	//RootSignatureを設定。PS0とは別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_);
	
	//PS0を設定
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_);
	
	//開発用UIの処理
	ImGui::ShowDemoWindow();
}

void MyEngine::EndFrame() 
{
	//内部コマンドを生成する
	ImGui::Render();

	dxCommon_->PostDraw();
}

void MyEngine::Finalize()
{
	for (int i = 0; i < 2; i++) 
	{
		textureResource_[i]->Release();
		intermediateResource_[i]->Release();
	}
	
	graphicsPipelineState_->Release();
	signatureBlob_->Release();
	
	if (errorBlob_)
	{
		errorBlob_->Release();
	}
	
	rootSignature_->Release();
	pixelShaderBlob_->Release();
	vertexShaderBlob_->Release();
	dxCommon_->Finalize();
}

void MyEngine::Update() {}

DirectX::ScratchImage MyEngine::LoadTexture(const std::string& filePath)
{
	//テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	//ミップマップ付きのデータを返す
	return mipImages;
}

ID3D12Resource* MyEngine::CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
{
	//metadataをもとにResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);//texturの幅
	resourceDesc.Height = UINT(metadata.height);//texturの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);//mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);//奥行or配列Textureの配列数
	resourceDesc.Format = metadata.format;//TextureのFormat
	resourceDesc.SampleDesc.Count = 1;//サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//Textureの次元数。普段は2次元

	//利用するheapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;//細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;//WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;//プロセッサの近くに配置

	//Resourceを生成する
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&resource));

	assert(SUCCEEDED(hr));
	
	return resource;
}


[[nodiscard]]
ID3D12Resource* MyEngine::UploadtextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, uint32_t index)
{
	std::vector<D3D12_SUBRESOURCE_DATA>subresource;
	DirectX::PrepareUpload(dxCommon_->GetDevice(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresource);
	uint64_t  intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresource.size()));
	intermediateResource_[index] = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), intermediateSize);
	UpdateSubresources(dxCommon_->GetCommandList(), texture, intermediateResource_[index], 0, 0, UINT(subresource.size()), subresource.data());

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	dxCommon_->GetCommandList()->ResourceBarrier(1, &barrier);
	return intermediateResource_[index];
}

void MyEngine::SettingTexture(const std::string& filePath, uint32_t index)
{
	DirectX::ScratchImage mipImage = LoadTexture(filePath);
	const DirectX::TexMetadata& metadata = mipImage.GetMetadata();
	textureResource_[index] = CreateTextureResource(dxCommon_->GetDevice(), metadata);
	intermediateResource_[index] = UploadtextureData(textureResource_[index], mipImage, index);

	//metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//SRVを作成するDescripterHeapの場所を決める
	textureSrvHandleGPU_[index] = GetGPUDescriptorHandle(dxCommon_->GetSrvDescriptiorHeap(), descriptorSizeSRV, index);
	textureSrvHandleCPU_[index] = GetCPUDescriptorHandle(dxCommon_->GetSrvDescriptiorHeap(), descriptorSizeSRV, index);

	//先頭はIMGUIが使ってるので、その次を使う
	textureSrvHandleCPU_[index].ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU_[index].ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//SRVの生成
	dxCommon_->GetDevice()->CreateShaderResourceView(textureResource_[index], &srvDesc, textureSrvHandleCPU_[index]);
}

D3D12_CPU_DESCRIPTOR_HANDLE MyEngine::GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorheap, uint32_t descriptorSize, uint32_t index) 
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorheap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE MyEngine::GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorheap, uint32_t descriptorSize, uint32_t index) 
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorheap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

DirectXCommon* MyEngine::dxCommon_;