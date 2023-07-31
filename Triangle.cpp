#include "Triangle.h"
#include <assert.h>
#include "MyEngine.h"

void Triangle::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;
	SettingVertex();
	SettingColor();
	MoveMatrix();
}

void Triangle::Draw(const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& material, const Matrix4x4& wvpdata)
{
	//����
	vertexData_[0] = a;
	//��
	vertexData_[1] = b;
	//�E��
	vertexData_[2] = c;

	*materialData_ = material;

	*wvpData_ = wvpdata;

	//VBV��ݒ�
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//�`���ݒ�BPS0�ɐݒ肵�Ă�����̂Ƃ͂܂��ʁB�������̂�ݒ肷��
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//�}�e���A��CBuffer�̏ꏊ��ݒ�
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	//�`��
	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);

}

void Triangle::Finalize()
{
	materialResource_->Release();
	vertexResource_->Release();
	wvpResource_->Release();
}

void Triangle::SettingVertex()
{
	vertexResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Vector4) * 3);
	//���\�[�X�̐擪�̃A�h���X����g��
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//�g�p���郊�\�[�X�̃T�C�Y�͒��_3���̃T�C�Y
	vertexBufferView_.SizeInBytes = sizeof(Vector4) * 3;
	//1���_������̃T�C�Y
	vertexBufferView_.StrideInBytes = sizeof(Vector4);
	//�������ނ��߂̃A�h���X���擾
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
}

void Triangle::SettingColor()
{
	//�}�e���A���p�̃��\�[�X�����@�����color1��
	materialResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Vector4));
	//�������ނ��߂̃A�h���X���擾
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}

void Triangle::MoveMatrix()
{
	wvpResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Matrix4x4));
	wvpResource_->Map(0, NULL, reinterpret_cast<void**>(&wvpData_));
	*wvpData_ = MakeIdentity4x4();
}

ID3D12Resource* Triangle::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes)
{
	//���_���\�[�X�p�̃q�[�v�̐ݒ�
	D3D12_HEAP_PROPERTIES uplodeHeapProperties{};
	uplodeHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeap���g��
	//���_���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC ResourceDesc{};
	//�o�b�t�@���\�[�X�B�e�N�X�`���̏ꍇ�͂܂��ʂ̐ݒ������
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = sizeInBytes;//���\�[�X�T�C�Y
	//�o�b�t�@�̏ꍇ�͂����͂P�ɂ��錈�܂�
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.SampleDesc.Count = 1;
	//�o�b�t�@�̏ꍇ�͂���ɂ��錈�܂�
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	HRESULT hr;

	ID3D12Resource* Resource = nullptr;
	//���ۂɒ��_���\�[�X�����
	hr = device->CreateCommittedResource(&uplodeHeapProperties, D3D12_HEAP_FLAG_NONE,
		&ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&Resource));
	assert(SUCCEEDED(hr));

	return Resource;
}