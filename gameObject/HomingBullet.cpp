#include "HomingBullet.h"
using namespace Microsoft::WRL;

ID3D12GraphicsCommandList* HomingBullet::cmdList;
XMMATRIX HomingBullet::matView;
XMMATRIX HomingBullet::matProjection;
Camera* HomingBullet::camera = nullptr;

HomingBullet::HomingBullet(ID3D12GraphicsCommandList* arg_cmdList)
{
	cmdList = arg_cmdList;
	scale = { 0.2f,0.2f,0.2f };
	rotation = { 0.0f,0.0f,0.0f };
	speed = { 1.0f,1.0f,1.0f };
	velocity = { 0.0f,0.0f,-90.0f };
	matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (float)WindowSize::window_width / (float)WindowSize::window_height, 0.1f, 1000.0f);
	collisionRadius = 0.25f;
}

HomingBullet::~HomingBullet()
{
}

void HomingBullet::CreateConstBuffer(ID3D12Device* device)
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

HomingBullet* HomingBullet::Create(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList, Vector3 position)
{
	HomingBullet* homingBullet = new HomingBullet(arg_cmdList);

	homingBullet->SetPosition(position);

	homingBullet->CreateConstBuffer(device);

	return homingBullet;
}

void HomingBullet::SetEye(const Vector3& eye)
{
	HomingBullet::camera->SetEye(eye);

	UpdateViewMatrix();
}

void HomingBullet::SetTarget(const Vector3& target)
{
	HomingBullet::camera->SetTarget(target);

	UpdateViewMatrix();
}

void HomingBullet::UpdateViewMatrix()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&camera->GetEye()), XMLoadFloat3(&camera->GetTarget()), XMLoadFloat3(&camera->GetUp()));
}

void HomingBullet::Initialize()
{
	hp = 1;
	invisibleTime = 0;
	isDeadFlag = false;
	velocity = { 0.0f,0.0f,0.0f };
	speed = { 0.1f,0.1f,0.5f };
	rotation = { 0.0f,0.0f,-90.0f };
}

//プレイヤーの更新処理
void HomingBullet::Update(const Vector3& incrementValue,const Vector3& arg_targetPosition)
{
	if (isDeadFlag) { return; }

	Move(arg_targetPosition);


	if (particleLugtime >= 1)
	{
		particleLugtime--;
	}

	SetScrollIncrement(incrementValue);

	TransferConstBuff();

	////マテリアルの転送
	//ConstBufferDataB1* constMap1 = nullptr;
	//result = constBuffB1->Map(0, nullptr, (void**)&constMap1);
	//constMap1->ambient = objModel->material.ambient;
	//constMap1->diffuse = objModel->material.diffuse;
	//constMap1->specular = objModel->material.specular;
	//constMap1->alpha = objModel->material.alpha;
	//constBuffB1->Unmap(0, nullptr);
}

void HomingBullet::Draw()
{
	if (isDeadFlag) { return; }

	HomingBullet::cmdList->SetPipelineState(PipelineState::bulletPipelineState.Get());

	HomingBullet::cmdList->SetGraphicsRootSignature(RootSignature::bulletRootsignature.Get());

	HomingBullet::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	HomingBullet::cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
	HomingBullet::cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

	objModel->Draw(HomingBullet::cmdList);
}

void HomingBullet::TransferConstBuff()
{
	HRESULT result;

	XMMATRIX matScale, matRot, matTrans;
	UpdateViewMatrix();
	// スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
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
}

void HomingBullet::Move(const Vector3& arg_targetPosition)
{
	Vector3 targetPosition;
	targetPosition = arg_targetPosition;

	Vector3 dir = { targetPosition.x - position.x,targetPosition.y - position.y, -1.0f };
	dir.normalize();
	velocity = { dir.x,dir.y,dir.z };


	position += velocity * speed;
}

void HomingBullet::SetScrollIncrement(const Vector3& incrementValue)
{
	position += incrementValue;
}

bool HomingBullet::IsCollision(GameObject* arg_otherObject)
{
	GameObject* otherObject = arg_otherObject;

	if (isDeadFlag || otherObject->GetIsDeadFlag() || otherObject->GetCharacterType() == characterType
		|| otherObject->GetCharacterType() == CHARACTERTYPE::BLOCK || (otherObject->GetCharacterType() == CHARACTERTYPE::PLAYER && characterType == CHARACTERTYPE::FRIEND)) {
		return false;
	}

	if (Collision::CheckSphereToSphere({ otherObject->GetPosition(),otherObject->GetCollisionRadius() },
		{ position,collisionRadius }))
	{
		if (scale.x >= 2.0f)
		{
			otherObject->CriticalDamage();
			if (otherObject->GetScale().x >= 2.0f)
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
