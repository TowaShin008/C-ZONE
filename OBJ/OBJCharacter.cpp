#include "OBJCharacter.h"
using namespace Microsoft::WRL;

ID3D12GraphicsCommandList* OBJCharacter::cmdList;
XMMATRIX OBJCharacter::matView;
XMMATRIX OBJCharacter::matProjection;
Camera* OBJCharacter::camera = nullptr;

OBJCharacter::OBJCharacter()
{
	matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (float)WindowSize::window_width / (float)WindowSize::window_height, 0.1f, 1000.0f);
}

OBJCharacter::~OBJCharacter()
{

}

void OBJCharacter::CreateConstBuffer(ID3D12Device* device)
{
	HRESULT result;

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0));

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB1));

	if (FAILED(result))
	{
		assert(0);
	}

	UpdateViewMatrix();
}

OBJCharacter* OBJCharacter::Create(ID3D12Device* device,Vector3 position)
{
	OBJCharacter* objCharacter3d = new OBJCharacter();

	objCharacter3d->scale = { 1.0f,1.0f,1.0f };

	objCharacter3d->SetPosition(position);

	objCharacter3d->CreateConstBuffer(device);

	return objCharacter3d;
}

void OBJCharacter::SetEye(const Vector3& eye)
{
	OBJCharacter::camera->SetEye(eye);

	UpdateViewMatrix();
}

void OBJCharacter::SetTarget(const Vector3& target)
{
	OBJCharacter::camera->SetTarget(target);

	UpdateViewMatrix();
}

void OBJCharacter::UpdateViewMatrix()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&camera->GetEye()), XMLoadFloat3(&camera->GetTarget()), XMLoadFloat3(&camera->GetUp()));
}
//�v���C���[�̍X�V����
void OBJCharacter::Update(const Vector3& incrementValue)
{
	HRESULT result;
	XMMATRIX matScale, matRot, matTrans;

	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ���[���h�s��̍���
	matWorld = XMMatrixIdentity(); // �ό`�����Z�b�g
	matWorld *= matScale; // ���[���h�s��ɃX�P�[�����O�𔽉f
	matWorld *= matRot; // ���[���h�s��ɉ�]�𔽉f
	matWorld *= matTrans; // ���[���h�s��ɕ��s�ړ��𔽉f

		// �e�I�u�W�F�N�g�������
	if (parent != nullptr) {
		// �e�I�u�W�F�N�g�̃��[���h�s����|����
		matWorld *= parent->matWorld;
	}

	UpdateViewMatrix();
	//���[���h�s��̓]��
	ConstBufferDataB0* constMap0 = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap0);
	constMap0->color = color;
	constMap0->mat = matWorld * matView * matProjection;	// �s��̍���
	constBuffB0->Unmap(0, nullptr);

	//�}�e���A���̓]��
	ConstBufferDataB1* constMap1 = nullptr;
	result = constBuffB1->Map(0, nullptr, (void**)&constMap1);
	constMap1->ambient = objModel->material.ambient;
	constMap1->diffuse = objModel->material.diffuse;
	constMap1->specular = objModel->material.specular;
	constMap1->alpha = objModel->material.alpha;
	constBuffB1->Unmap(0, nullptr);

}

void OBJCharacter::Draw(ID3D12GraphicsCommandList* cmdList)
{
	OBJCharacter::cmdList = cmdList;
	OBJCharacter::cmdList->SetPipelineState(PipelineState::simplePipelineState.Get());

	OBJCharacter::cmdList->SetGraphicsRootSignature(RootSignature::simpleRootsignature.Get());

	OBJCharacter::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	OBJCharacter::cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
	OBJCharacter::cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

	objModel->Draw(OBJCharacter::cmdList);

	OBJCharacter::cmdList = nullptr;
}