#include"Triangle.h"
#include<assert.h>
#include "MyEngine.h"

void CreateTriangle::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;
	SettingVertex();
}

void CreateTriangle::Draw(const Vector4& a, const Vector4& b, const Vector4& c)
{
	//����
	vertexData_[0] = a;
	//��
	vertexData_[1] = b;
	//�E��
	vertexData_[2] = c;

	//VBV��ݒ�
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//�`���ݒ�BPS0�ɂ����Ă����Ă�����̂Ƃ͂܂��ʁB�������̂�ݒ肷��
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//�`��
	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void CreateTriangle::Finalize()
{
	vertexResource_->Release();
}

void CreateTriangle::SettingVertex()
{
	//���_���\�[�X�p�̃q�[�v�̐ݒ�
	D3D12_HEAP_PROPERTIES uplodeHeapProperties{};
	uplodeHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeap���g��
	//���_���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	//�o�b�t�@���\�[�X�B�e�N�X�`���̏ꍇ�͂܂��ʂ̐ݒ������
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeof(Vector4) * 3;//���\�[�X�T�C�Y�@
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