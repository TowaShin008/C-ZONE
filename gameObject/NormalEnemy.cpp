#include "NormalEnemy.h"
using namespace Microsoft::WRL;

ID3D12GraphicsCommandList* NormalEnemy::cmdList;
XMMATRIX NormalEnemy::matView;
XMMATRIX NormalEnemy::matProjection;
Camera* NormalEnemy::camera = nullptr;

NormalEnemy::NormalEnemy(ID3D12GraphicsCommandList* arg_cmdList)
{
	cmdList = arg_cmdList;
	scale = { 0.2f,0.2f,0.2f };
	speed = { 0.1f,0.1f,0.1f };
	collisionRadius = 1.0f;
	currentPhase = MOVEPHASE::PHASE1;
	currentType = MOVETYPE::TRIANGLE;
	characterType = CHARACTERTYPE::ENEMY;
	matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (float)WindowSize::window_width / (float)WindowSize::window_height, 0.1f, 1000.0f);
}

NormalEnemy::~NormalEnemy()
{
	for (int i = 0; i < bullets.size(); i++)
	{
		delete bullets[i];
	}
}

void NormalEnemy::CreateConstBuffer(ID3D12Device* device)
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

	scoreCharacter.reset(OBJCharacter::Create(device));
	scoreCharacter->SetScale({2.0f,2.0f,2.0f});
	scoreCharacter->SetRotation({0,0,180.0f});

	if (FAILED(result))
	{
		assert(0);
	}

	UpdateViewMatrix();
}

NormalEnemy* NormalEnemy::Create(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList,Vector3 position)
{
	NormalEnemy* normalEnemy = new NormalEnemy(arg_cmdList);

	normalEnemy->Initialize();

	normalEnemy->SetPosition(position);

	normalEnemy->CreateConstBuffer(device);

	normalEnemy->AttachBullet(device);

	ParticleManager* deathParticle = ParticleManager::Create(device);

	deathParticle->SetSelectColor(2);

	normalEnemy->SetDeathParticleManager(deathParticle);

	return normalEnemy;
}

void NormalEnemy::SetEye(const Vector3& eye)
{
	NormalEnemy::camera->SetEye(eye);

	UpdateViewMatrix();
}

void NormalEnemy::SetTarget(const Vector3& target)
{
	NormalEnemy::camera->SetTarget(target);

	UpdateViewMatrix();
}

void NormalEnemy::UpdateViewMatrix()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&camera->GetEye()), XMLoadFloat3(&camera->GetTarget()), XMLoadFloat3(&camera->GetUp()));
}

void NormalEnemy::SetOBJModel(OBJHighModel* arg_objModel, OBJModel* bulletModel, OBJModel* arg_scoreModel)
{
	objModel = arg_objModel;
	SetBulletModel(bulletModel);
	scoreCharacter->SetOBJModel(arg_scoreModel);
}

void NormalEnemy::SetBulletModel(OBJModel* bulletModel)
{
	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i]->SetOBJModel(bulletModel);
	}
}

void NormalEnemy::AttachBullet(ID3D12Device* device)
{
	bullets.resize(BULLETMAXNUM);

	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i] = Bullet::Create(device,cmdList);
		bullets[i]->SetIsDeadFlag(true);
		bullets[i]->SetCharacterType(CHARACTERTYPE::ENEMY);
		bullets[i]->SetVelocity({-0.2f,0.0f,0.0f});
	}
}

void NormalEnemy::Initialize()
{
	isDeadFlag = false;
	const int maxShotLugTime = 60;
	shotLugTime = maxShotLugTime;
	moveLugTime = 0;
	currentPhase = MOVEPHASE::PHASE1;
	currentType = MOVETYPE::TRIANGLE;
	moveEndFlag = false;
	shotFlag = true;
	velocity = { 0,0,0 };
	rotation = { 270.0f,0.0f,90.0f };
	color = { 1.0f,0.0f,0.0f,1.0f };
	deathParticleFlag = false;
	hp = 1;
	invisibleTime = 0;
}
//プレイヤーの更新処理
void NormalEnemy::Update(const Vector3& incrementValue)
{
	centerPosition += incrementValue.x;
	SetScrollIncrement(incrementValue);
	if (moveLugTime <= 0)
	{
		if (isDeadFlag == false)
		{
			if (moveEndFlag == false)
			{
				//弾の射撃処理
				ShotBullet();
				//移動処理
				Move();

			}

				// 親オブジェクトがあれば
			if (parent != nullptr) {
				// 親オブジェクトのワールド行列を掛ける
				matWorld *= parent->matWorld;
			}

			//マテリアルの転送
			//ConstBufferDataB1* constMap1 = nullptr;
			//result = constBuffB1->Map(0, nullptr, (void**)&constMap1);
			//constMap1->ambient = objModel->material.ambient;
			//constMap1->diffuse = objModel->material.diffuse;
			//constMap1->specular = objModel->material.specular;
			//constMap1->alpha = objModel->material.alpha;
			//constBuffB1->Unmap(0, nullptr);
		}
		else
		{
			//スコア演出
			ScoreProcessing(incrementValue);
		}
	}
	else
	{
		moveLugTime--;

		if (moveLugTime > 0)
		{
			isDeadFlag = true;
		}
		else
		{
			isDeadFlag = false;
		}
	}

	DeathParticleProcessing();


	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i]->Update(incrementValue);
		if (bullets[i]->GetPosition().x < centerPosition + SCREENLEFT - 4.0f)
		{
			bullets[i]->SetIsDeadFlag(true);
		}
	}


}

void NormalEnemy::Draw()
{
	if (isDeadFlag == false)
	{
		TransferConstBuff();

		NormalEnemy::cmdList->SetPipelineState(PipelineState::basicPipelineState.Get());

		NormalEnemy::cmdList->SetGraphicsRootSignature(RootSignature::basicRootsignature.Get());

		NormalEnemy::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		NormalEnemy::cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
		NormalEnemy::cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

		objModel->Draw(NormalEnemy::cmdList);
	}
	else
	{
		ParticleManager::PreDraw(cmdList);
		deathParticle->Draw();
		ParticleManager::PostDraw();

		if (scoreCharacter->GetColor().w >= 0.1f)
		{
			scoreCharacter->Draw(cmdList);
		}
	}

	

	for (int i = 0; i < bullets.size(); i++)
	{
		if(bullets[i]->GetIsDeadFlag() == false)
		bullets[i]->Draw();
	}
}

void NormalEnemy::TransferConstBuff()
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
	matTrans = XMMatrixTranslation(position.x + 0.2f, position.y - 0.1f, position.z);

	// ワールド行列の合成
	matWorld = XMMatrixIdentity(); // 変形をリセット
	matWorld *= matScale; // ワールド行列にスケーリングを反映
	matWorld *= matRot; // ワールド行列に回転を反映
	matWorld *= matTrans; // ワールド行列に平行移動を反映

	//ワールド行列の転送
	ConstBufferDataB0* constMap0 = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap0);
	constMap0->color = color;
	constMap0->mat = matWorld * matView * matProjection;	// 行列の合成
	constBuffB0->Unmap(0, nullptr);
}

void NormalEnemy::Move()
{
	//動きの種類
	switch (currentType)
	{
	case MOVETYPE::RECTANGLE:

		RectAngleMove();

		break;

	case MOVETYPE::TRIANGLE:

		TriAngleMove();

		break;

	case MOVETYPE::LEFTDIAGONALLY:

		LeftDiagonallyMove();

		break;

	case MOVETYPE::UPCURVE:

		UpCurveMove();

		break;

	case MOVETYPE::DOWNCURVE:

		DownCurveMove();

		break;

	default:
		break;
	}

	velocity = velocity.normalize();

	position.x = position.x + velocity.x * speed.x;
	position.y = position.y + velocity.y * speed.y;

}

void NormalEnemy::RectAngleMove()
{
	switch (currentPhase)
	{
	case MOVEPHASE::PHASE1://左に進む処理

		velocity.x = -0.1f;
		if (position.x <= centerPosition + (SCREENRIGHT / 3.0f) * 2.0f)
		{
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,90.0f };
			currentPhase = MOVEPHASE::PHASE2;
		}

		break;

	case MOVEPHASE::PHASE2://下に進む処理

		velocity.y = -0.1f;
		rotation.x = 270.0f - 45.0f;

		if (position.y <= SCREENBUTTOM)
		{
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,90.0f };
			currentPhase = MOVEPHASE::PHASE3;
		}

		break;

	case MOVEPHASE::PHASE3://右に進む処理

		velocity.x = 0.1f;

		if (position.x >= centerPosition + SCREENRIGHT + 8.0f)
		{
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,90.0f };
			currentPhase = MOVEPHASE::PHASE4;
		}

		break;

	case MOVEPHASE::PHASE4://上に進む処理

		velocity.y = 0.1f;
		rotation.x = 270.0f + 45.0f;

		if (position.y >= SCREENTOP)
		{
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,90.0f };
			currentPhase = MOVEPHASE::PHASE1;
			IsDead();
			moveEndFlag = true;
		}

		break;


	default:
		break;
	}
}

void NormalEnemy::TriAngleMove()
{
	switch (currentPhase)
	{
	case MOVEPHASE::PHASE1://左に進む処理

		velocity.x = -0.2f;
		velocity.y = -0.1f;
		rotation.x = 270.0f - 45.0f;
		if (position.y <= 0.0f)
		{
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,90.0f };
			currentPhase = MOVEPHASE::PHASE2;
		}

		break;

	case MOVEPHASE::PHASE2://右に進む処理

		velocity.x =  0.2f;
		velocity.y = -0.1f;
		rotation.x = 270.0f - 45.0f;

		if (position.y <= SCREENBUTTOM)
		{
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,90.0f };
			currentPhase = MOVEPHASE::PHASE3;
		}

		break;

	case MOVEPHASE::PHASE3://上に進む処理

		velocity.y =   0.1f;
		rotation.x = 270.0f + 45.0f;

		if (position.y >= SCREENTOP)
		{
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,90.0f };
			currentPhase = MOVEPHASE::PHASE1;
			IsDead();
			moveEndFlag = true;
		}

		break;

	default:
		break;
	}
}

void NormalEnemy::LeftDiagonallyMove()
{
	if (currentPhase == MOVEPHASE::PHASE1)
	{
		velocity.x = -0.1f;
		velocity.y = -0.1f;
		rotation.x = 270.0f - 45.0f;

		if (position.y < SCREENBUTTOM - 5.0f)
		{
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,90.0f };
			currentPhase = MOVEPHASE::PHASE1;
			IsDead();
			moveEndFlag = true;
		}
	}
}

void NormalEnemy::UpCurveMove()
{
	if (currentPhase == MOVEPHASE::PHASE1)
	{
		if (curveTime < 2.0f)
		{
			curveTime += 0.01f;
		}

		if (rotation.y > 0.0f)
		{
			rotation.y -= 1.0f;
		}

		velocity.x = cosf((XM_PI / 2) * curveTime);
		velocity.y = sinf((XM_PI / 2) * curveTime);


		if (position.y < SCREENLEFT - 5.0f)
		{
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,90.0f };
			curveTime = 0.0f;
			currentPhase = MOVEPHASE::PHASE1;
			IsDead();
			moveEndFlag = true;
		}
	}
}

void NormalEnemy::DownCurveMove()
{
	if (currentPhase == MOVEPHASE::PHASE1)
	{
		if (curveTime > 2.0f)
		{
			curveTime -= 0.01f;
		}

		if (rotation.y < 360.0f)
		{
			rotation.y += 1.0f;
		}

		velocity.x = cosf((XM_PI / 2) * curveTime);
		velocity.y = sinf((XM_PI / 2) * curveTime);


		//rotation.z = 45.0f;

		if (position.y < SCREENLEFT - 5.0f)
		{
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,90.0f };
			curveTime = 0.0f;
			currentPhase = MOVEPHASE::PHASE1;
			IsDead();
			moveEndFlag = true;
		}
	}
}

void NormalEnemy::IsDead()
{
	isDeadFlag = true;
}

void NormalEnemy::SetMoveType(const MOVETYPE& moveType)
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

void NormalEnemy::SetScrollIncrement(const Vector3& incrementValue)
{
	position += incrementValue;
}

void NormalEnemy::Damage()
{
	if (invisibleTime < 1)
	{
		hp--;
		if (invisibleTime <= 0)
		{
			invisibleTime = INVISIBLEMAXTIME;
		}
	}
	if (hp == 0)
	{
		isDeadFlag = true;
		scoreCharacter->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		scoreCharacter->SetScale({ 2.0f,2.0f,2.0f });
		deathParticleFlag = true;
	}
}

void NormalEnemy::CriticalDamage()
{
	if (invisibleTime < 1)
	{
		hp--;
		if (invisibleTime <= 0)
		{
			invisibleTime = INVISIBLEMAXTIME;
		}
	}
	if (hp == 0)
	{
		isDeadFlag = true;
		scoreCharacter->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		scoreCharacter->SetScale({ 2.0f,2.0f,2.0f });
		deathParticleFlag = true;
	}
}

void NormalEnemy::DeathParticleProcessing()
{
	if (deathParticleFlag)
	{
		if (deathParticle->GetParticleLength() < 10)
		{
			for (int i = 0; i < 10; i++)
			{
				//X,Y,Z全て[-5.0f,+5.0f]でランダムに分布
				const float rnd_pos = 10.0f / 30;
				Vector3 pos = position;
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
				life = 40;
				pos.x = pos.x - centerPosition;
				deathParticle->Add(life, pos, vel, acc, 0.5f, 0.0f, { 1.0f,0.0f,0.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
			}
			deathParticleFlag = false;
		}
	}

	deathParticle->Update();
}

bool NormalEnemy::IsCollision(GameObject* arg_otherObject)
{
	GameObject* otherObject = arg_otherObject;

	if (isDeadFlag || otherObject->GetIsDeadFlag() || otherObject->GetCharacterType() == characterType 
		|| otherObject->GetCharacterType() == CHARACTERTYPE::BLOCK)
	{ return false; }

	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i]->IsCollision(otherObject);
	}

	if(otherObject->GetCharacterType() == CHARACTERTYPE::PLAYER)
	{
		return false;
	}

	if (Collision::CheckSphereToSphere({ otherObject->GetPosition(),otherObject->GetCollisionRadius() },
		{ position,collisionRadius }))
	{
		if (otherObject->GetScale().x >= 2.0f)
		{
			Damage();
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

void NormalEnemy::ShotBullet()
{
	if (shotFlag)
	{
		//射撃処理
		if (shotLugTime < 1)
		{
			for (int i = 0; i < bullets.size(); i++)
			{
				if (bullets[i]->GetIsDeadFlag() == true)
				{
					bullets[i]->SetIsDeadFlag(false);
					bullets[i]->SetPosition({ position.x - 2.0f, position.y, position.z });
					const int maxShotLugTime = 60;
					shotLugTime = maxShotLugTime;
					break;
				}
			}
		}
		else
		{//再発射するまでのラグタイム
			shotLugTime--;
		}
	}
}

void NormalEnemy::ScoreProcessing(Vector3 incrementValue)
{
	Vector4 scoreColor = scoreCharacter->GetColor();
	Vector3 scoreScale = scoreCharacter->GetScale();

	if (scoreScale.x <= 2.5f)
	{
		scoreScale.x += 0.05f;
		scoreScale.y += 0.05f;
		scoreScale.z += 0.05f;
	}

	if (scoreColor.w > 0.0f)
		scoreColor.w -= 0.01f;

	scoreCharacter->SetScale(scoreScale);
	scoreCharacter->SetColor(scoreColor);
	scoreCharacter->SetPosition({ position.x,position.y,position.z + 5.0f });
	scoreCharacter->Update(incrementValue);
}
