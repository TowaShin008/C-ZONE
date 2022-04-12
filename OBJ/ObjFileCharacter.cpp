#include "ObjFileCharacter.h"
using namespace Microsoft::WRL;

ID3D12GraphicsCommandList* ObjFileCharacter::cmdList;
XMMATRIX ObjFileCharacter::matView;
XMMATRIX ObjFileCharacter::matProjection;
Camera* ObjFileCharacter::camera = nullptr;
ID3D12Device* ObjFileCharacter::device = nullptr;

ObjFileCharacter::ObjFileCharacter(ID3D12GraphicsCommandList* arg_cmdList, ID3D12Device* arg_device)
{
	cmdList = arg_cmdList;
	device = arg_device;
	scale = { 5.0f,5.0f,1.0f };
	rotation = { 0,0,-90.0f };
	matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (float)WindowSize::window_width / (float)WindowSize::window_height, 0.1f, 1000.0f);
}

ObjFileCharacter::~ObjFileCharacter()
{

}

void ObjFileCharacter::CreateConstBuffer()
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

ObjFileCharacter* ObjFileCharacter::Create(ID3D12Device* arg_device, ID3D12GraphicsCommandList* arg_cmdList,const Vector3& arg_position)
{
	ObjFileCharacter* objFileCharacter = new ObjFileCharacter(arg_cmdList, arg_device);

	objFileCharacter->SetPosition(arg_position);

	objFileCharacter->CreateConstBuffer();

	return objFileCharacter;
}

void ObjFileCharacter::SetEye(const Vector3& arg_eye)
{
	ObjFileCharacter::camera->SetEye(arg_eye);

	UpdateViewMatrix();
}

void ObjFileCharacter::SetTarget(const Vector3& arg_target)
{
	ObjFileCharacter::camera->SetTarget(arg_target);

	UpdateViewMatrix();
}

void ObjFileCharacter::UpdateViewMatrix()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&camera->GetEye()), XMLoadFloat3(&camera->GetTarget()), XMLoadFloat3(&camera->GetUp()));
}

void ObjFileCharacter::Initialize()
{
	isDeadFlag = false;
}

//�v���C���[�̍X�V����
void ObjFileCharacter::Update(const Vector3& arg_incrementValue)
{
	HRESULT result;

	XMMATRIX matScale, matRot, matTrans;
	UpdateViewMatrix();
	SetScrollIncrement(arg_incrementValue);
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


	//���[���h�s��̓]��
	ConstBufferDataB0* constMap0 = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap0);
	constMap0->color = color;
	constMap0->mat = matWorld * matView * matProjection;	// �s��̍���
	constBuffB0->Unmap(0, nullptr);
}

void ObjFileCharacter::Draw()
{
	if (isDeadFlag == false)
	{
		ObjFileCharacter::cmdList->SetPipelineState(PipelineState::bulletPipelineState.Get());

		ObjFileCharacter::cmdList->SetGraphicsRootSignature(RootSignature::bulletRootsignature.Get());

		ObjFileCharacter::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		ObjFileCharacter::cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
		ObjFileCharacter::cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

		objModel->Draw(ObjFileCharacter::cmdList);
	}
}

void ObjFileCharacter::SetScrollIncrement(const Vector3& arg_incrementValue)
{
	position += arg_incrementValue;
}

bool ObjFileCharacter::IsCollision(GameObject* arg_otherObject)
{
	GameObject* otherObject = arg_otherObject;
	if (isDeadFlag || otherObject->GetIsDeadFlag() || otherObject->GetCharacterType() == characterType
		|| otherObject->GetCharacterType() == CHARACTERTYPE::BLOCK || (otherObject->GetCharacterType() == CHARACTERTYPE::PLAYER && characterType == CHARACTERTYPE::FRIEND)) {
		return false;
	}

	if (Collision::CheckLayToSphere({ position,{1,0,0} },
		{ otherObject->GetPosition(),otherObject->GetCollisionRadius() }))
	{
		otherObject->Damage();
		return true;
	}

	return false;
}
