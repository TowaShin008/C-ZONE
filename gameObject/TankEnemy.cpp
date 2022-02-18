#include "TankEnemy.h"
using namespace Microsoft::WRL;

ID3D12GraphicsCommandList* TankEnemy::cmdList;
XMMATRIX TankEnemy::matView;
XMMATRIX TankEnemy::matProjection;
Camera* TankEnemy::camera = nullptr;

TankEnemy::TankEnemy(ID3D12GraphicsCommandList* arg_cmdList)
{
	cmdList = arg_cmdList;
	scale = { 0.5f,0.5f,0.5f };
	speed = { 0.1f,0.1f,0.1f };
	brakeCount = 0;
	collisionRadius = 1.0f;
	currentPhase = MOVEPHASE::PHASE1;
	currentType = MOVETYPE::TRIANGLE;
	characterType = CHARACTERTYPE::ENEMY;
	matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (float)WindowSize::window_width / (float)WindowSize::window_height, 0.1f, 1000.0f);
}

TankEnemy::~TankEnemy()
{
	for (int i = 0; i < bullets.size(); i++)
	{
		delete bullets[i];
	}
}

void TankEnemy::CreateConstBuffer(ID3D12Device* device)
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

	scoreCharacter.reset(OBJCharacter::Create(device));
	scoreCharacter->SetScale({ 2.0f,2.0f,2.0f });
	scoreCharacter->SetRotation({ 0,0,180.0f });

	UpdateViewMatrix();
}

TankEnemy* TankEnemy::Create(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList, Vector3 position)
{
	TankEnemy* tankEnemy = new TankEnemy(arg_cmdList);

	tankEnemy->Initialize();

	tankEnemy->SetPosition({ position.x - 19.25f,position.y + 9.8f,0.0f });

	tankEnemy->CreateConstBuffer(device);

	tankEnemy->AttachBullet(device);

	ParticleManager* deathParticle = ParticleManager::Create(device);

	deathParticle->SetSelectColor(2);

	tankEnemy->SetDeathParticleManager(deathParticle);

	return tankEnemy;
}

void TankEnemy::SetEye(const Vector3& eye)
{
	TankEnemy::camera->SetEye(eye);

	UpdateViewMatrix();
}

void TankEnemy::SetTarget(const Vector3& target)
{
	TankEnemy::camera->SetTarget(target);

	UpdateViewMatrix();
}

void TankEnemy::UpdateViewMatrix()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&camera->GetEye()), XMLoadFloat3(&camera->GetTarget()), XMLoadFloat3(&camera->GetUp()));
}

void TankEnemy::SetOBJModel(OBJHighModel* arg_objModel, OBJModel* bulletModel, OBJModel* scoreModel)
{
	objModel = arg_objModel;
	SetBulletModel(bulletModel);
	scoreCharacter->SetOBJModel(scoreModel);
}

void TankEnemy::SetBulletModel(OBJModel* bulletModel)
{
	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i]->SetOBJModel(bulletModel);
	}
}

void TankEnemy::AttachBullet(ID3D12Device* device)
{
	bullets.resize(BULLETMAXNUM);

	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i] = Bullet::Create(device, cmdList);
		bullets[i]->SetIsDeadFlag(true);
		bullets[i]->SetSpeed({ 0.2f,0.2f,0.2f });
		bullets[i]->SetCharacterType(CHARACTERTYPE::ENEMY);
		bullets[i]->SetVelocity({ -0.2f,0.0f,0.0f });
	}
}

void TankEnemy::Initialize()
{
	isDeadFlag = false;
	const int maxShotLugTime = 240;
	shotLugTime = maxShotLugTime;
	moveLugTime = 0;
	currentPhase = MOVEPHASE::PHASE1;
	currentType = MOVETYPE::TRIANGLE;
	moveEndFlag = false;
	velocity = { -1,-1,0 };
	rotation = { 270.0f,0.0f,90.0f };
	color = { 1.0f,0.0f,0.0f,1.0f };
	deathParticleFlag = false;
	hp = 1;
	invisibleTime = 0;
	brakeCount = 0.0f;
}
//プレイヤーの更新処理
void TankEnemy::Update(const Vector3& incrementValue)
{
	centerPosition += incrementValue.x;
	//SetScrollIncrement(incrementValue);
	if (moveLugTime <= 0)
	{
		if (isDeadFlag == false)
		{
			if (moveEndFlag == false)
			{
				//プレイヤーの一定範囲に入ったら動き始める
				if (position.x < playerPosition.x + 30.0f)
				{
					//移動処理
					Move();
				}
			}
			else
			{//移動が終わったら攻撃を始める
				//弾丸の発射処理
				ShotBullet();
				//ブレーキ処理
				Brake();
			}
			//定数バッファの転送
			TransferConstBuff();
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
	//死亡パーティクル演出
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

void TankEnemy::ShotBullet()
{
	//射撃処理
	if (shotLugTime < 1)
	{
		for (int i = 0; i < bullets.size(); i++)
		{
			if (bullets[i]->GetIsDeadFlag() == true)
			{
				Vector3 dirPosition = { playerPosition.x + 1.0f - position.x ,playerPosition.y - position.y ,playerPosition.z - position.z };
				dirPosition.normalize();
				bullets[i]->SetVelocity(dirPosition);
				bullets[i]->SetIsDeadFlag(false);
				bullets[i]->SetPosition({ position.x, position.y, position.z });
				const int maxShotLugTime = 240;
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

void TankEnemy::TransferConstBuff()
{
	if (rotation.y > 0.0f)
	{
		rotation.y -= 10.0f * speed.x;
	}
	else
	{
		rotation.y = 360.0f;
	}
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
	//ConstBufferDataB1* constMap1 = nullptr;
	//result = constBuffB1->Map(0, nullptr, (void**)&constMap1);
	//constMap1->ambient = objModel->material.ambient;
	//constMap1->diffuse = objModel->material.diffuse;
	//constMap1->specular = objModel->material.specular;
	//constMap1->alpha = objModel->material.alpha;
	//constBuffB1->Unmap(0, nullptr);
}

void TankEnemy::Move()
{
	//position.x = 0.15f;

	velocity = velocity.normalize();

	position.x = position.x + velocity.x * speed.x;
	position.y = position.y + velocity.y * speed.y;

	if (playerPosition.x + 5.0f > position.x)
	{
		moveEndFlag = true;
	}
}

void TankEnemy::Brake()
{
	if (brakeCount < 1.0f)
	{
		brakeCount += 0.01f;
		//ブレーキ時のイージング処理
		speed = easeOut({ 0.1f,0.1f,0.1f }, { 0.0f,0.0f,0.0f }, brakeCount);
		position.x = position.x + velocity.x * speed.x;
		position.y = position.y + velocity.y * speed.y;
	}
}

void TankEnemy::Draw()
{
	if (isDeadFlag == false)
	{
		TankEnemy::cmdList->SetPipelineState(PipelineState::basicPipelineState.Get());

		TankEnemy::cmdList->SetGraphicsRootSignature(RootSignature::basicRootsignature.Get());

		TankEnemy::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		TankEnemy::cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
		TankEnemy::cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

		objModel->Draw(TankEnemy::cmdList);

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
		if (bullets[i]->GetIsDeadFlag() == false)
			bullets[i]->Draw();
	}

}

void TankEnemy::RectAngleMove()
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

void TankEnemy::TriAngleMove()
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

		velocity.x = 0.2f;
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

void TankEnemy::LeftDiagonallyMove()
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

void TankEnemy::UpCurveMove()
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

void TankEnemy::DownCurveMove()
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

void TankEnemy::IsDead()
{
	isDeadFlag = true;
}

void TankEnemy::SetMoveType(const MOVETYPE& moveType)
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

void TankEnemy::SetScrollIncrement(const Vector3& incrementValue)
{
	position += incrementValue;
}

void TankEnemy::DeathParticleProcessing()
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
				const float rnd_acc = 0.02f;
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


bool TankEnemy::IsCollision(GameObject* arg_otherObject)
{
	GameObject* otherObject = arg_otherObject;

	if (isDeadFlag || otherObject->GetIsDeadFlag() || otherObject->GetCharacterType() == characterType)
	{
		return false;
	}

	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i]->IsCollision(otherObject);
	}

	Vector3 otherPosition = otherObject->GetPosition();
	if (otherObject->GetCharacterType() == CHARACTERTYPE::PLAYER)
	{
		playerPosition = otherPosition;
		return false;
	}

	if (otherObject->GetCharacterType() == CHARACTERTYPE::BLOCK)
	{//ブロックだった場合のブロックの上にのる為の当たり判定
		if (Collision::CheckSphereToSphere({ otherPosition,otherObject->GetCollisionRadius() },
			{ position,collisionRadius }))
		{
			position.y = otherPosition.y + 1.25f;
			return true;
		}
	}
	else
	{
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
	}

	return false;
}

void TankEnemy::Damage()
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
		deathParticleFlag = true;
	}
}

void TankEnemy::CriticalDamage()
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
		deathParticleFlag = true;
	}
}

void TankEnemy::ScoreProcessing(Vector3 incrementValue)
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
