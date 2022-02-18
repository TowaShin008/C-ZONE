#include "Missile.h"
using namespace Microsoft::WRL;

ID3D12GraphicsCommandList* Missile::cmdList;
XMMATRIX Missile::matView;
XMMATRIX Missile::matProjection;
Camera* Missile::camera = nullptr;

Missile::Missile(ID3D12GraphicsCommandList* arg_cmdList)
{
	cmdList = arg_cmdList;
	scale = { 0.2f,0.2f,0.2f };
	rotation = { 0.0f,90.0f,0.0f };
	speed = { 1.0f,1.0f,1.0f };
	velocity = { 0.0f,0.0f,0.0f };
	matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (float)WindowSize::window_width / (float)WindowSize::window_height, 0.1f, 1000.0f);
	collisionRadius = 0.25f;
	upShotFlag = true;
	bossSceneFlag = false;
}

Missile::~Missile()
{
}

void Missile::CreateConstBuffer(ID3D12Device* device)
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

Missile* Missile::Create(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList, Vector3 position)
{
	Missile* missile = new Missile(arg_cmdList);

	missile->SetPosition(position);

	missile->CreateConstBuffer(device);

	ParticleManager* missileParticleMan;
	missileParticleMan = ParticleManager::Create(device);
	missile->SetMissilePerticleMan(missileParticleMan);

	return missile;
}

void Missile::SetEye(const Vector3& eye)
{
	Missile::camera->SetEye(eye);

	UpdateViewMatrix();
}

void Missile::SetTarget(const Vector3& target)
{
	Missile::camera->SetTarget(target);

	UpdateViewMatrix();
}

void Missile::UpdateViewMatrix()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&camera->GetEye()), XMLoadFloat3(&camera->GetTarget()), XMLoadFloat3(&camera->GetUp()));
}

void Missile::Initialize()
{
	hp = 1;
	particleLugtime = MAXMISSILELUGTIME;
	invisibleTime = 0;
	isDeadFlag = false;
	curveTime = 1.0f;
	velocity = { 0.0f,0.0f,0.0f };
	speed = { 0.0f,0.0f,0.0f };



	if (bossSceneFlag)
	{
		rotation = { 0.0f,0.0f,0.0f };

		if (upShotFlag)
			rotation.x = 270.0f;
		else
			rotation.x = 90.0f;
	}
	else
	{
		rotation = { 0.0f,90.0f,0.0f };

		if (upShotFlag)
			rotation.x = 270.0f;
		else
			rotation.x = 90.0f;
	}
}

//プレイヤーの更新処理
void Missile::Update(const Vector3& incrementValue, float centerPosition)
{
	if (isDeadFlag) { return; }

	Move();

	MissileParticleProcessing(centerPosition);

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

void Missile::Draw()
{
	missileParticleMan->Update();
	ParticleManager::PreDraw(cmdList);
	missileParticleMan->Draw();
	ParticleManager::PostDraw();

	if (isDeadFlag) { return; }

	Missile::cmdList->SetPipelineState(PipelineState::bulletPipelineState.Get());

	Missile::cmdList->SetGraphicsRootSignature(RootSignature::bulletRootsignature.Get());

	Missile::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Missile::cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
	Missile::cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

	objModel->Draw(Missile::cmdList);
}

void Missile::TransferConstBuff()
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

void Missile::Move()
{
	position += velocity * speed;

	if (curveTime > 0.0f)
	{
		curveTime -= 0.02f;
	}

	Vector3 missileEasing = easeIn({ 0.0f,0.0f,0.0f }, { 0.6f,0.6f,0.6f }, (1 - curveTime));
	speed = missileEasing;

	if (bossSceneFlag)
	{
		if (upShotFlag)
		{
			if (rotation.x < 360.0f)
			{
				rotation.x += 4.0f;
			}
			else
			{
				velocity.z = cosf((XM_PI / 2) * curveTime);
				velocity.y = sinf((XM_PI / 2) * curveTime);
			}
		}
		else
		{
			if (rotation.x > 0.0f)
			{
				rotation.x -= 4.0f;
			}
			else
			{
				velocity.z = cosf((XM_PI / 2) * curveTime);
				velocity.y = -sinf((XM_PI / 2) * curveTime);
			}
		}
	}
	else
	{
		if (upShotFlag)
		{
			if (rotation.x < 360.0f)
			{
				rotation.x += 4.0f;
			}
			else
			{
				velocity.x = cosf((XM_PI / 2) * curveTime);
				velocity.y = sinf((XM_PI / 2) * curveTime);
			}
		}
		else
		{
			if (rotation.x > 0.0f)
			{
				rotation.x -= 4.0f;
			}
			else
			{
				velocity.x = cosf((XM_PI / 2) * curveTime);
				velocity.y = -sinf((XM_PI / 2) * curveTime);
			}
		}
	}
}

void Missile::SetScrollIncrement(const Vector3& incrementValue)
{
	position += incrementValue;
}

void Missile::MissileParticleProcessing(float centerPosition)
{
	//ミサイルの後ろから出るパーティクル処理
	if (rotation.x >= 360.0f || rotation.x <= 0.0f)
		if (missileParticleMan->GetParticleLength() < 20 && particleLugtime <= 0)
		{
			const float rnd_pos = 1.5f;
			//変更ここまで
			Vector3 pos{};
			Vector3 dis{};
			const float rnd_vel = 0.2f;
			Vector3 vel{};
			Vector3 acc{};

			if (bossSceneFlag)
			{
				pos = { position.x + ((float)rand() / RAND_MAX * rnd_pos * 4.0f + 10.0f),position.y + ((float)rand() / RAND_MAX * rnd_pos * 8.0f - 5.0f),position.z };
				dis = { position.x - pos.x,position.y - pos.y ,position.z - pos.z };
				//X,Y,Z全て[-0.05f,+0.05f]でランダムに分布
				dis = dis.normalize();
				//X,Y,Z全て[-0.05f,+0.05f]でランダムに分布
				vel = dis * 0.05f;
				acc.z = -0.001f;
			}
			else
			{
				pos = { position.x + ((float)rand() / RAND_MAX * rnd_pos * 4.0f + 10.0f), position.y + ((float)rand() / RAND_MAX * rnd_pos * 8.0f - 5.0f),position.z };
				dis = { position.x - pos.x,position.y - pos.y ,0.0f };
				//X,Y,Z全て[-0.05f,+0.05f]でランダムに分布
				dis = dis.normalize();
				//X,Y,Z全て[-0.05f,+0.05f]でランダムに分布
				vel = dis * 0.05f;
				acc.x = -0.001f;
			}


			pos = position;
			pos.x = pos.x - centerPosition;
			pos.x += 0.5f;
			missileParticleMan->Add(40, pos, vel, acc, 0.0f, 0.5f, { 0.0f,0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f,1.0f });

			particleLugtime = MAXMISSILELUGTIME;
		}

	if (particleLugtime >= 1)
	{
		particleLugtime--;
	}
}

bool Missile::IsCollision(GameObject* arg_otherObject)
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
