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
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0));

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
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
//プレイヤーの更新処理
void OBJCharacter::Update(const Vector3& incrementValue)
{
	HRESULT result;
	XMMATRIX matScale, matRot, matTrans;

	// スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ワールド行列の合成
	matWorld = XMMatrixIdentity(); // 変形をリセット
	matWorld *= matScale; // ワールド行列にスケーリングを反映
	matWorld *= matRot; // ワールド行列に回転を反映
	matWorld *= matTrans; // ワールド行列に平行移動を反映

		// 親オブジェクトがあれば
	if (parent != nullptr) {
		// 親オブジェクトのワールド行列を掛ける
		matWorld *= parent->matWorld;
	}

	UpdateViewMatrix();
	//ワールド行列の転送
	ConstBufferDataB0* constMap0 = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap0);
	constMap0->color = color;
	constMap0->mat = matWorld * matView * matProjection;	// 行列の合成
	constBuffB0->Unmap(0, nullptr);

	//マテリアルの転送
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