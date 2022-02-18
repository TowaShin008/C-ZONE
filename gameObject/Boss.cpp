#include "Boss.h"
using namespace Microsoft::WRL;

ID3D12GraphicsCommandList* Boss::cmdList;
XMMATRIX Boss::matView;
XMMATRIX Boss::matProjection;
Camera* Boss::camera = nullptr;

Boss::Boss(ID3D12GraphicsCommandList* arg_cmdList)
{
	cmdList = arg_cmdList;
	scale = { 4.0f,4.0f,4.0f };
	collisionRadius = 2.0f;
	rotation = { 270.0f,0.0f,0.0f };
	speed = { 0.1f,0.1f,0.0f };
	characterType = CHARACTERTYPE::ENEMY;
	matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (float)WindowSize::window_width / (float)WindowSize::window_height, 0.1f, 1000.0f);
}

Boss::~Boss()
{
	delete bigBullet;
}

void Boss::CreateConstBuffer(ID3D12Device* device)
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

Boss* Boss::Create(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList,Vector3 position)
{
	Boss* boss = new Boss(arg_cmdList);
	boss->Initialize();
	boss->SetPosition(position);
	boss->CreateConstBuffer(device);
	boss->AttachBullet(device);

	ParticleManager* deathParticle = ParticleManager::Create(device);
	deathParticle->SetSelectColor(2);
	boss->SetDeathParticleManager(deathParticle);

	return boss;
}

void Boss::SetEye(const Vector3& eye)
{
	Boss::camera->SetEye(eye);

	UpdateViewMatrix();
}

void Boss::SetTarget(const Vector3& target)
{
	Boss::camera->SetTarget(target);

	UpdateViewMatrix();
}

void Boss::UpdateViewMatrix()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&camera->GetEye()), XMLoadFloat3(&camera->GetTarget()), XMLoadFloat3(&camera->GetUp()));
}

void Boss::SetOBJModel(OBJHighModel* arg_objModel, OBJHighModel* bulletModel)
{
	objModel = arg_objModel;
	SetBulletModel(bulletModel);
}

void Boss::SetBulletModel(OBJHighModel* bulletModel)
{
	bigBullet->SetOBJModel(bulletModel);
}

void Boss::AttachBullet(ID3D12Device* device)
{
	bigBullet = HomingBullet::Create(device,cmdList);
	bigBullet->SetIsDeadFlag(true);
	bigBullet->SetCollisionRadius(2.0f);
	bigBullet->SetCharacterType(CHARACTERTYPE::ENEMY);
	//bigBullet->SetSpeed({0.4f,0.4f,0.4f});
	bigBullet->SetScale({ 2.5f,2.5f,2.5f });
	//bigBullet->SetVelocity({ 0.0f,0.0f,-0.5f });
}

void Boss::Initialize()
{
	isDeadFlag = false;
	shotLugTime = MAXENEMYLUGTIME;
	moveLugTime = 0;
	currentPhase = MOVEPHASE::PHASE1;
	currentType = MOVETYPE::STAY;
	moveEndFlag = false;
	shotFlag = true;
	velocity = { 0,0,0 };
	const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
	color = white;
	speed = { 0.1f,0.1f,0.0f };
	lanchFlag = false;
	lanchLugTime = LANCHMAXTIME;
	hp = 400;
	deathParticleFlag = false;
	invisibleTime = 0;
}
//プレイヤーの更新処理
void Boss::Update(const Vector3& incrementValue, const Vector3& playerPosition)
{
	centerPosition += incrementValue.x;
	if (isDeadFlag == false)
	{
		HRESULT result;
		SetScrollIncrement(incrementValue);
		if (moveLugTime <= 0)
		{
			if (moveEndFlag == false)
			{
				Move();
			}
		}
		else
		{
			moveLugTime--;
		}

		ShotBullet(incrementValue,playerPosition);

		DamageEffect();


		//マテリアルの転送
		//ConstBufferDataB1* constMap1 = nullptr;
		//result = constBuffB1->Map(0, nullptr, (void**)&constMap1);
		//constMap1->ambient = objModel->material.ambient;
		//constMap1->diffuse = objModel->material.diffuse;
		//constMap1->specular = objModel->material.specular;
		//constMap1->alpha = objModel->material.alpha;
		//constBuffB1->Unmap(0, nullptr);
	}

	DeathParticleProcessing();
}

void Boss::Draw()
{
	if (isDeadFlag == false)
	{
		TransferConstBuff();

		Boss::cmdList->SetPipelineState(PipelineState::simplePipelineState.Get());

		Boss::cmdList->SetGraphicsRootSignature(RootSignature::simpleRootsignature.Get());

		Boss::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Boss::cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
		Boss::cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

		objModel->Draw(Boss::cmdList);

		if (deathParticleFlag)
		{
			ParticleManager::PreDraw(cmdList);
			deathParticle->Draw();
			ParticleManager::PostDraw();
		}

		if (bigBullet->GetIsDeadFlag() == false)
			bigBullet->Draw();
	}
}

void Boss::TransferConstBuff()
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
	matTrans = XMMatrixTranslation(position.x, position.y - 4.0f, position.z);

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

void Boss::Move()
{
	//動きの種類
	switch (currentType)
	{
	case MOVETYPE::STAY:

		StayMove();

		break;

	default:
		break;
	}

	velocity = velocity.normalize();

	position.x = position.x + velocity.x * speed.x;
	position.y = position.y + velocity.y * speed.y;
}

void Boss::StayMove()
{
	switch (currentPhase)
	{
	case MOVEPHASE::PHASE1://左に進む処理

		velocity.x = -0.1f;
		if (position.x <= centerPosition + (SCREENRIGHT / 3.0f))
		{
			velocity = { 0,0,0 };
			currentPhase = MOVEPHASE::PHASE2;
		}

		break;

	case MOVEPHASE::PHASE2://下に進む処理

		lanchLugTime++;
		if (lanchLugTime > LANCHMAXTIME)
		{
			lanchLugTime = 0;
			lanchFlag = true;
		}

		if (hp < 1)
		{
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,0.0f };
			deathParticleFlag = true;
			currentPhase = MOVEPHASE::PHASE3;
		}

		break;

	case MOVEPHASE::PHASE3://下に進む処理

		velocity = { 1,-1,0 };

		if (position.x > centerPosition + 20.0f)
		{
			velocity = { 0,0,0 };
			rotation = { 0,270,0 };
			currentPhase = MOVEPHASE::PHASE1;
			IsDead();
			moveEndFlag = true;
		}

		break;


	default:
		break;
	}
}

void Boss::IsDead()
{
	isDeadFlag = true;
}

void Boss::SetMoveType(const MOVETYPE& moveType)
{
	currentType = moveType;

	if (currentType == MOVETYPE::UPCURVE)
	{
		curveTime = 1.0f;
		rotation.y = 90.0f;
	}
	if (currentType == MOVETYPE::DOWNCURVE)
	{
		curveTime = 3.0;
		rotation.y = 270.0f;
	}
}

void Boss::SetScrollIncrement(const Vector3& incrementValue)
{
	position += incrementValue;
}

void Boss::Damage()
{
	if (invisibleTime <= 0)
	{
		invisibleTime = INVISIBLEMAXTIME;
	}
	hp--;
}

void Boss::CriticalDamage()
{
	if (invisibleTime <= 0)
	{
		invisibleTime = INVISIBLEMAXTIME;
	}
	hp -= 50;
}

void Boss::DamageEffect()
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

void Boss::DeathParticleProcessing()
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
				//X,Y,Z全て[-5.0f,+5.0f]でランダムに分布

				const float rnd_vel = 0.1f;
				Vector3 vel{};

				//X,Y,Z全て[-0.05f,+0.05f]でランダムに分布
				vel.x = ((float)rand() / RAND_MAX * rnd_vel - rnd_vel / 4.0f) * 2;
				vel.y = (float)rand() / RAND_MAX * rnd_vel * 2;
				vel.z = 0.0f;
				//重力に見立ててYのみ[-0.001f,0]でランダムに分布
				const float rnd_acc = 0.01f;
				Vector3 acc{};
				acc.y = -(float)rand() / RAND_MAX * rnd_acc;

				const float startScale = 3.0f;
				const float endScale = 0.0f;
				const Vector4 startColor = { 1.0f,0.0f,0.0f,1.0f };
				const Vector4 endColor = { 1.0f,1.0f,1.0f,1.0f };
				deathParticle->Add(30, pos, vel, acc, startScale, endScale, startColor, endColor);
			}
		}
	}

	deathParticle->Update();
}

bool Boss::IsCollision(GameObject* arg_otherObject)
{
	GameObject* otherObject = arg_otherObject;

	if (isDeadFlag || otherObject->GetIsDeadFlag() || otherObject->GetCharacterType() == characterType
		|| otherObject->GetCharacterType() == CHARACTERTYPE::BLOCK) {
		return false;
	}

	bigBullet->IsCollision(otherObject);

	//if (Collision::CheckSphereToSphere({ otherObject->GetPosition(),otherObject->GetCollisionRadius() },
	//	{ position,collisionRadius }))
	//{
	//	if (otherObject->GetCharacterType() == CHARACTERTYPE::PLAYER)
	//	{
	//		otherObject->Damage();
	//		return true;
	//	}
	//	if (otherObject->GetScale().x >= 2.0f)
	//	{
	//		CriticalDamage();
	//	}
	//	else
	//	{
	//		Damage();
	//	}
	//	return true;
	//}

	return false;
}

void Boss::ShotBullet(const Vector3& incrementValue, const Vector3& playerPosition)
{
	//射撃処理
	if (shotFlag && bigBullet->GetIsDeadFlag())
	{
		bigBullet->Initialize();
		bigBullet->SetPosition({ position.x, position.y, position.z - 2.0f });
		shotLugTime = 240;
	}

	if (shotLugTime > 0)
	{//再発射するまでのラグタイム
		shotFlag = false;
		if (bigBullet->GetIsDeadFlag())
			shotLugTime--;
	}
	else
	{
		shotFlag = true;
	}

	if (bigBullet->GetIsDeadFlag() == false)
	{
		Vector3 targetPosition;
		targetPosition = playerPosition;

		Vector3 dir = { playerPosition.x - position.x,playerPosition.y - position.y, playerPosition.z - position.z };
		dir.normalize();
		bigBullet->SetVelocity({ dir.x,dir.y,dir.z });
		Vector3 rotation = bigBullet->GetRotasion();
		rotation.z += 2.0f;
		bigBullet->SetRotation(rotation);
		bigBullet->Update(incrementValue, targetPosition);

		if (bigBullet->GetPosition().z < position.z - SCREENBACK)
		{
			bigBullet->SetIsDeadFlag(true);
		}
	}
}

