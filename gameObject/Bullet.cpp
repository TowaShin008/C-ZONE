#include "Bullet.h"
using namespace Microsoft::WRL;

ID3D12GraphicsCommandList* Bullet::cmdList;
XMMATRIX Bullet::matView;
XMMATRIX Bullet::matProjection;
Camera* Bullet::camera = nullptr;

Bullet::Bullet(ID3D12GraphicsCommandList* arg_cmdList)
{
	cmdList = arg_cmdList;
	scale = { 0.2f,0.2f,0.2f };
	speed = { 1.0f,1.0f,1.0f };
	matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (float)WindowSize::window_width / (float)WindowSize::window_height, 0.1f, 1000.0f);
	collisionRadius = 0.25f;
}

Bullet::~Bullet()
{

}

void Bullet::CreateConstBuffer(ID3D12Device* device)
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

Bullet* Bullet::Create(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList,Vector3 position)
{
	Bullet* bullet = new Bullet(arg_cmdList);

	bullet->SetPosition(position);

	bullet->CreateConstBuffer(device);

	return bullet;
}

void Bullet::SetEye(const Vector3& eye)
{
	Bullet::camera->SetEye(eye);

	UpdateViewMatrix();
}

void Bullet::SetTarget(const Vector3& target)
{
	Bullet::camera->SetTarget(target);

	UpdateViewMatrix();
}

void Bullet::UpdateViewMatrix()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&camera->GetEye()), XMLoadFloat3(&camera->GetTarget()), XMLoadFloat3(&camera->GetUp()));
}

void Bullet::Initialize()
{
	hp = 1;
	invisibleTime = 0;
	isDeadFlag = false;
}

//プレイヤーの更新処理
void Bullet::Update(const Vector3& incrementValue)
{
	if (isDeadFlag) { return; }

	Move();

	SetScrollIncrement(incrementValue);

	TransferConstBuff();
}

void Bullet::Draw()
{
	if (isDeadFlag) { return; }

	Bullet::cmdList->SetPipelineState(PipelineState::simplePipelineState.Get());

	Bullet::cmdList->SetGraphicsRootSignature(RootSignature::simpleRootsignature.Get());

	Bullet::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Bullet::cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
	Bullet::cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

	objModel->Draw(Bullet::cmdList);
}

void Bullet::TransferConstBuff()
{
	HRESULT result;

	XMMATRIX matScale, matRot, matTrans;
	UpdateViewMatrix();
	// スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
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

void Bullet::Move()
{
	position = position + velocity * speed;
}

void Bullet::SetScrollIncrement(const Vector3& incrementValue)
{
	position += incrementValue;
}

bool Bullet::IsCollision(GameObject* arg_otherObject)
{
	GameObject* otherObject = arg_otherObject;

	CHARACTERTYPE otherCharacterType = otherObject->GetCharacterType();
	if (isDeadFlag || otherObject->GetIsDeadFlag() || otherCharacterType == characterType
		|| otherCharacterType == CHARACTERTYPE::BLOCK || (otherCharacterType == CHARACTERTYPE::PLAYER && characterType == CHARACTERTYPE::FRIEND)) {
		return false;
	}

	if (Collision::CheckSphereToSphere({ otherObject->GetPosition(),otherObject->GetCollisionRadius() },
		{ position,collisionRadius }))
	{//大きい弾の場合は貫通処理
		if (scale.x >= 2.0f)
		{//くらっても死なないオブジェクトの場合は貫通しない
			otherObject->CriticalDamage();
			if (otherObject->GetIsDeadFlag()==false)
			{
				Damage();
			}
		}
		else
		{
			Damage();
			otherObject->Damage();
		}
		return true;
	}

	return false;
}

