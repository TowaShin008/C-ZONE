#include "Laser.h"
using namespace Microsoft::WRL;

ID3D12GraphicsCommandList* Laser::cmdList;
XMMATRIX Laser::matView;
XMMATRIX Laser::matProjection;
Camera* Laser::camera = nullptr;

Laser::Laser(ID3D12GraphicsCommandList* arg_cmdList)
{
	cmdList = arg_cmdList;
	scale = { 5.0f,5.0f,1.0f };
	const Vector4 lightBlue = { 0.0f,1.0f,1.0f,1.0f };
	color = lightBlue;
	rotation = {0,0,-90.0f};
	bossSceneFlag = false;
	matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (float)WindowSize::window_width / (float)WindowSize::window_height, 0.1f, 1000.0f);
}

Laser::~Laser()
{

}

void Laser::CreateConstBuffer(ID3D12Device* device)
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

Laser* Laser::Create(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList,Vector3 position)
{
	Laser* laser = new Laser(arg_cmdList);

	laser->SetPosition(position);

	laser->CreateConstBuffer(device);

	return laser;
}

void Laser::SetEye(const Vector3& eye)
{
	Laser::camera->SetEye(eye);

	UpdateViewMatrix();
}

void Laser::SetTarget(const Vector3& target)
{
	Laser::camera->SetTarget(target);

	UpdateViewMatrix();
}

void Laser::UpdateViewMatrix()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&camera->GetEye()), XMLoadFloat3(&camera->GetTarget()), XMLoadFloat3(&camera->GetUp()));
}

void Laser::Initialize()
{
	isDeadFlag = false;

	if (bossSceneFlag)
	{
		rotation = { 0,-90,-90.0f };
	}
	else
	{
		rotation = { 0,0,-90.0f };
	}
}

//プレイヤーの更新処理
void Laser::Update(const Vector3& incrementValue)
{
	SetScrollIncrement(incrementValue);
	//定数バッファの転送
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

void Laser::Draw()
{
	if (isDeadFlag)
	{
		return;
	}
	
	Laser::cmdList->SetPipelineState(PipelineState::bulletPipelineState.Get());

	Laser::cmdList->SetGraphicsRootSignature(RootSignature::bulletRootsignature.Get());

	Laser::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Laser::cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
	Laser::cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

	objModel->Draw(Laser::cmdList);
}

void Laser::TransferConstBuff()
{
	HRESULT result;

	XMMATRIX matScale, matRot, matTrans;
	UpdateViewMatrix();
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


	//ワールド行列の転送
	ConstBufferDataB0* constMap0 = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap0);
	constMap0->color = color;
	constMap0->mat = matWorld * matView * matProjection;	// 行列の合成
	constBuffB0->Unmap(0, nullptr);
}

void Laser::SetScrollIncrement(const Vector3& incrementValue)
{
	position += incrementValue;
}

bool Laser::IsCollision(GameObject* arg_otherObject)
{
	GameObject* otherObject = arg_otherObject;

	if (isDeadFlag || otherObject->GetIsDeadFlag() || otherObject->GetCharacterType() == characterType
		|| otherObject->GetCharacterType() == CHARACTERTYPE::BLOCK || (otherObject->GetCharacterType() == CHARACTERTYPE::PLAYER && characterType == CHARACTERTYPE::FRIEND)) {
		return false;
	}

	//ボスシーン化によってレイの向きを決める
	Vector3 laserDir = { 0,0,0 };
	if (bossSceneFlag)
	{
		laserDir = { 0,0,1 };
	}
	else
	{
		laserDir = { 1,0,0 };
	}

	if (Collision::CheckLayToSphere({ position,laserDir },
		{ otherObject->GetPosition(),otherObject->GetCollisionRadius() }))
	{
		otherObject->Damage();
		return true;
	}

	return false;
}
