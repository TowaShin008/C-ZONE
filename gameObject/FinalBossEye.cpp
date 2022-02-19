#include "FinalBossEye.h"
using namespace Microsoft::WRL;

ID3D12GraphicsCommandList* FinalBossEye::cmdList;
XMMATRIX FinalBossEye::matView;
XMMATRIX FinalBossEye::matProjection;
Camera* FinalBossEye::camera = nullptr;

FinalBossEye::FinalBossEye(ID3D12GraphicsCommandList* arg_cmdList)
{
	cmdList = arg_cmdList;
	scale = { 1.0f,1.0f,1.0f };
	collisionRadius = 2.0f;
	rotation = { 0.0f,0.0f,0.0f };
	speed = { 0.1f,0.1f,0.0f };
	characterType = CHARACTERTYPE::ENEMY;
	matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (float)WindowSize::window_width / (float)WindowSize::window_height, 0.1f, 1000.0f);
}

FinalBossEye::~FinalBossEye()
{
}

void FinalBossEye::CreateConstBuffer(ID3D12Device* device)
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

FinalBossEye* FinalBossEye::Create(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList, Vector3 position)
{
	FinalBossEye* finalBossEye = new FinalBossEye(arg_cmdList);
	finalBossEye->Initialize();
	finalBossEye->SetPosition(position);
	finalBossEye->CreateConstBuffer(device);

	ParticleManager* deathParticle = ParticleManager::Create(device);
	deathParticle->SetSelectColor(2);
	finalBossEye->SetDeathParticleManager(deathParticle);

	return finalBossEye;
}

void FinalBossEye::SetEye(const Vector3& eye)
{
	FinalBossEye::camera->SetEye(eye);

	UpdateViewMatrix();
}

void FinalBossEye::SetTarget(const Vector3& target)
{
	FinalBossEye::camera->SetTarget(target);

	UpdateViewMatrix();
}

void FinalBossEye::UpdateViewMatrix()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&camera->GetEye()), XMLoadFloat3(&camera->GetTarget()), XMLoadFloat3(&camera->GetUp()));
}

void FinalBossEye::SetOBJModel(OBJModel* arg_objModel)
{
	objModel = arg_objModel;
}

void FinalBossEye::Initialize()
{
	isDeadFlag = false;
	moveEndFlag = false;
	velocity = { 0,0,0 };
	const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
	color = white;
	speed = { 0.1f,0.1f,0.0f };
	hp = 400;
	deathParticleFlag = false;
	invisibleTime = 0;
}
//プレイヤーの更新処理
void FinalBossEye::Update(const Vector3& incrementValue)
{
	centerPosition += incrementValue.x;
	if (isDeadFlag == false)
	{
		SetScrollIncrement(incrementValue);
		if (moveEndFlag == false)
		{//移動処理
			Move();
		}
		//ダメージ演出
		DamageEffect();
		//定数バッファの転送
		TransferConstBuff();

		//マテリアルの転送
		//ConstBufferDataB1* constMap1 = nullptr;
		//result = constBuffB1->Map(0, nullptr, (void**)&constMap1);
		//constMap1->ambient = objModel->material.ambient;
		//constMap1->diffuse = objModel->material.diffuse;
		//constMap1->specular = objModel->material.specular;
		//constMap1->alpha = objModel->material.alpha;
		//constBuffB1->Unmap(0, nullptr);
	}
	//死亡パーティクル処理
	DeathParticleProcessing();
}

void FinalBossEye::Draw()
{
	if (isDeadFlag)
	{
		return;
	}
	
	FinalBossEye::cmdList->SetPipelineState(PipelineState::simplePipelineState.Get());

	FinalBossEye::cmdList->SetGraphicsRootSignature(RootSignature::simpleRootsignature.Get());

	FinalBossEye::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	FinalBossEye::cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
	FinalBossEye::cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

	objModel->Draw(FinalBossEye::cmdList);

	if (deathParticleFlag)
	{
		ParticleManager::PreDraw(cmdList);
		deathParticle->Draw();
		ParticleManager::PostDraw();
	}
}

void FinalBossEye::TransferConstBuff()
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

void FinalBossEye::Move()
{
	velocity = velocity.normalize();

	position.x = position.x + velocity.x * speed.x;
	position.y = position.y + velocity.y * speed.y;
}

void FinalBossEye::IsDead()
{
	isDeadFlag = true;
}


void FinalBossEye::SetScrollIncrement(const Vector3& incrementValue)
{
	position += incrementValue;
}

void FinalBossEye::Damage()
{
	if (invisibleTime <= 0)
	{
		invisibleTime = INVISIBLEMAXTIME;
	}
	hp--;
}

void FinalBossEye::CriticalDamage()
{
	if (invisibleTime <= 0)
	{
		invisibleTime = INVISIBLEMAXTIME;
	}
	hp -= 50;
}

void FinalBossEye::DamageEffect()
{
	//無敵演出
	if (invisibleTime > 0)
	{
		invisibleTime--;
		if (invisibleTime % 20 <= 10)
		{
			const Vector4 black = { 0.0f,0.0f,0.0f,1.0f };
			color = black;
		}
		else
		{
			const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
			color = white;
		}
	}
	else
	{
		const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
		color = white;
	}
}

void FinalBossEye::DeathParticleProcessing()
{
	if (deathParticleFlag)
	{
		if (deathParticle->GetParticleLength() < ParticleManager::vertexCount)
		{
			const float rnd_pos = 10.0f;
			Vector3 pos = position;
			pos.x = pos.x - centerPosition;

			pos.x += (float)rand() / RAND_MAX * rnd_pos;
			pos.y += (float)rand() / RAND_MAX * rnd_pos;
			for (int i = 0; i < 10; i++)
			{
				const float rnd_vel = 0.1f;
				Vector3 vel{};

				//X[-0.05f,+0.15f]でランダムに分布
				vel.x = ((float)rand() / RAND_MAX * rnd_vel - rnd_vel / 4.0f) * 2;
				//Y[-0.0f,+0.2f]でランダムに分布
				vel.y = (float)rand() / RAND_MAX * rnd_vel * 2;
				vel.z = 0.0f;

				const float rnd_acc = 0.01f;
				Vector3 acc{};
				//重力に見立ててYのみ[-0.01f,0]でランダムに分布
				acc.y = -(float)rand() / RAND_MAX * rnd_acc;

				const float startScale = 3.0f;
				const float endScale = 0.0f;
				const Vector4 red = { 1.0f,0.0f,0.0f,1.0f };
				const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
				const Vector4 startColor = red;
				const Vector4 endColor = white;
				deathParticle->Add(30, pos, vel, acc, startScale, endScale, startColor, endColor);
			}
		}
	}

	deathParticle->Update();
}

bool FinalBossEye::IsCollision(GameObject* arg_otherObject)
{
	GameObject* otherObject = arg_otherObject;

	if (isDeadFlag || otherObject->GetIsDeadFlag() || otherObject->GetCharacterType() == characterType
		|| otherObject->GetCharacterType() == CHARACTERTYPE::BLOCK) {
		return false;
	}

	if (Collision::CheckSphereToSphere({ otherObject->GetPosition(),otherObject->GetCollisionRadius() },
		{ position,collisionRadius }))
	{
		if (otherObject->GetCharacterType() == CHARACTERTYPE::PLAYER)
		{
			otherObject->Damage();
			return true;
		}
		if (otherObject->GetScale().x >= 2.0f)
		{
			CriticalDamage();
		}
		else
		{
			Damage();
		}
		return true;
	}

	return false;
}

