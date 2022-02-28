#include "Block.h"
using namespace Microsoft::WRL;

ID3D12GraphicsCommandList* Block::cmdList;
XMMATRIX Block::matView;
XMMATRIX Block::matProjection;
Camera* Block::camera = nullptr;

Block::Block(ID3D12GraphicsCommandList* arg_cmdList)
{
	cmdList = arg_cmdList;
	scale = { 0.2f,0.2f,0.2f };
	characterType = CHARACTERTYPE::BLOCK;
	matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (float)WindowSize::window_width / (float)WindowSize::window_height, 0.1f, 1000.0f);
	collisionRadius = 0.5f;
}

Block::~Block()
{

}

void Block::CreateConstBuffer(ID3D12Device* arg_device)
{
	HRESULT result;

	result = arg_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0));

	result = arg_device->CreateCommittedResource(
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

Block* Block::Create(ID3D12Device* arg_device, ID3D12GraphicsCommandList* arg_cmdList,const Vector3& arg_position)
{
	Block* block = new Block(arg_cmdList);

	block->SetPosition({ arg_position.x,arg_position.y,arg_position.z });

	block->CreateConstBuffer(arg_device);

	return block;
}

void Block::SetEye(const Vector3& arg_eye)
{
	Block::camera->SetEye(arg_eye);

	UpdateViewMatrix();
}

void Block::SetTarget(const Vector3& arg_target)
{
	Block::camera->SetTarget(arg_target);

	UpdateViewMatrix();
}

void Block::UpdateViewMatrix()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&camera->GetEye()), XMLoadFloat3(&camera->GetTarget()), XMLoadFloat3(&camera->GetUp()));
}

void Block::SetOBJModel(OBJModel* arg_objModel)
{
	objModel = arg_objModel;
}

void Block::Initialize()
{
	isDeadFlag = false;
	velocity = { 0,0,0 };
	rotation = { 0.0f,270.0f,0.0f };
	const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
	color = white;
}
//プレイヤーの更新処理
void Block::Update(const Vector3& arg_incrementValue)
{
	if (isDeadFlag)
	{
		return;
	}

	centerPos += arg_incrementValue.x;

	position = position + velocity * speed;
	//定数バッファの転送
	TransferConstBuff();
}

void Block::Draw()
{
	if (isDeadFlag)
	{
		return;
	}

	const float fixScreenRightPos = 5.0f;

	if (stageblockFlag && centerPos + SCREENRIGHT + fixScreenRightPos < position.x && centerPos - SCREENLEFT < position.x)
	{
		return;
	}

	Block::cmdList->SetPipelineState(PipelineState::simplePipelineState.Get());

	Block::cmdList->SetGraphicsRootSignature(RootSignature::simpleRootsignature.Get());

	Block::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Block::cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
	Block::cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

	objModel->Draw(Block::cmdList);
}

void Block::TransferConstBuff()
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
	//ConstBufferDataB1* constMap1 = nullptr;
	//result = constBuffB1->Map(0, nullptr, (void**)&constMap1);
	//constMap1->ambient = objModel->material.ambient;
	//constMap1->diffuse = objModel->material.diffuse;
	//constMap1->specular = objModel->material.specular;
	//constMap1->alpha = objModel->material.alpha;
	//constBuffB1->Unmap(0, nullptr);
}

void Block::IsDead()
{
	isDeadFlag = true;
}

void Block::SetScrollIncrement(const Vector3& arg_incrementValue)
{
	position = arg_incrementValue;
}

bool Block::IsCollision(GameObject* arg_otherObject)
{
	GameObject* otherObject = arg_otherObject;

	if (isDeadFlag || otherObject->GetIsDeadFlag() || otherObject->GetCharacterType() != CHARACTERTYPE::PLAYER) {
		return false;
	}

	if (Collision::CheckSphereToSphere({ otherObject->GetPosition(),otherObject->GetCollisionRadius() },
		{ position,collisionRadius }))
	{
		otherObject->Damage();
		return true;
	}

	return false;
}
