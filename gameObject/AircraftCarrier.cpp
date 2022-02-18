#include "AircraftCarrier.h"
using namespace Microsoft::WRL;

ID3D12Device* AircraftCarrier::device;
ID3D12GraphicsCommandList* AircraftCarrier::cmdList;
XMMATRIX AircraftCarrier::matView;
XMMATRIX AircraftCarrier::matProjection;
Camera* AircraftCarrier::camera = nullptr;

AircraftCarrier::AircraftCarrier(ID3D12GraphicsCommandList* arg_cmdList)
{
	cmdList = arg_cmdList;
	const Vector3 bossScale = { 4.0f,4.0f,4.0f };
	scale = bossScale;
	collisionRadius = 2.0f;
	SetSpeed({ 0.1f,0.1f,0.1f });
	currentPhase = MOVEPHASE::PHASE1;
	currentType = MOVETYPE::STAY;
	hp = 1;
	invisibleTime = 0;
	characterType = CHARACTERTYPE::ENEMY;
	matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (float)WindowSize::window_width / (float)WindowSize::window_height, 0.1f, 1000.0f);
}

AircraftCarrier::~AircraftCarrier()
{
	//ゲームオブジェクトマネージャークラスに入れているためエイリアンの削除処理は、記述しなくてよい
	//for (int i = 0; i < aliens.size(); i++)
	//{
	//	delete aliens[i];
	//}
}

void AircraftCarrier::CreateConstBuffer(ID3D12Device* device)
{
	HRESULT result;
	//定数バッファの生成(位置)
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0));
	//定数バッファの生成(マテリアル)
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
	//ビュー行列の更新
	UpdateViewMatrix();
}

AircraftCarrier* AircraftCarrier::Create(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList,const Vector3& position)
{
	AircraftCarrier* aircraftCarrier = new AircraftCarrier(arg_cmdList);

	aircraftCarrier->Initialize();

	aircraftCarrier->SetPosition(position);

	aircraftCarrier->CreateConstBuffer(device);
	//弾丸の装備
	aircraftCarrier->AttachBullet(device);

	ParticleManager* deathParticle = ParticleManager::Create(device);
	//パーティクル色のセット
	deathParticle->SetSelectColor(2);

	aircraftCarrier->SetDeathParticleManager(deathParticle);

	return aircraftCarrier;
}

void AircraftCarrier::SetEye(const Vector3& eye)
{
	AircraftCarrier::camera->SetEye(eye);

	UpdateViewMatrix();
}

void AircraftCarrier::SetTarget(const Vector3& target)
{
	AircraftCarrier::camera->SetTarget(target);

	UpdateViewMatrix();
}

void AircraftCarrier::UpdateViewMatrix()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&camera->GetEye()), XMLoadFloat3(&camera->GetTarget()), XMLoadFloat3(&camera->GetUp()));
}

void AircraftCarrier::SetOBJModel(OBJHighModel* arg_objModel, OBJModel* alienModel, OBJModel* scoreModel)
{
	objModel = arg_objModel;
	SetAlienModel(alienModel,scoreModel);
}

void AircraftCarrier::SetAlienModel(OBJModel* alienModel, OBJModel* scoreModel)
{
	//先に敵機の生成とモデルセットとオブジェクトマネージャーへの追加をしておく
	for (int i = 0; i < aliens.size(); i++)
	{
		//モデル1のセット
		aliens[i]->SetOBJModel(alienModel,scoreModel);
	}
}

void AircraftCarrier::AttachBullet(ID3D12Device* device)
{
	aliens.resize(10);

	//先に敵機の生成とモデルセットとオブジェクトマネージャーへの追加をしておく
	for (int i = 0; i < aliens.size(); i++)
	{
		//キャラクター1の生成
		aliens[i] = Alien::Create(device, cmdList, { 5.0f + 25.0f,12.0f,0.0f });
		aliens[i]->SetIsDeadFlag(true);
	}
}

void AircraftCarrier::Initialize()
{
	isDeadFlag = false;
	shotLugTime = MAXENEMYLUGTIME;
	moveLugTime = 0;
	currentPhase = MOVEPHASE::PHASE1;
	currentType = MOVETYPE::STAY;
	moveEndFlag = false;
	shotFlag = true;
	velocity = { 0,0,0 };
	rotation = { 270.0f,0.0f,90.0f };
	scale = { 2.0f,2.0f,2.0f };
	color = { 1.0f,1.0f,1.0f,1.0f };
	lanchFlag = false;
	lanchLugTime = LANCHMAXTIME;
	hp = MAXHP;
	deathParticleFlag = false;
	invisibleTime = 0;
}
//プレイヤーの更新処理
void AircraftCarrier::Update(const Vector3& incrementValue)
{
	centerPosition += incrementValue.x;
	SetScrollIncrement(incrementValue);
	if (isDeadFlag == false)
	{
		HRESULT result;

		if (moveLugTime <= 0)
		{
			if (moveEndFlag == false)
			{
				Move(incrementValue);
			}
		}
		else
		{
			moveLugTime--;
		}

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

	//for (int i = 0; i < aliens.size(); i++)
	//{
	//	aliens[i]->Update(incrementValue);
	//}
}

void AircraftCarrier::Draw()
{
	if (isDeadFlag == false)
	{
		TransferConstBuff();

		AircraftCarrier::cmdList->SetPipelineState(PipelineState::simplePipelineState.Get());

		AircraftCarrier::cmdList->SetGraphicsRootSignature(RootSignature::simpleRootsignature.Get());

		AircraftCarrier::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		AircraftCarrier::cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
		AircraftCarrier::cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

		objModel->Draw(AircraftCarrier::cmdList);

		//for (int i = 0; i < aliens.size(); i++)
		//{
		//	aliens[i]->Draw(AircraftCarrier::cmdList);
		//}


		if (deathParticleFlag)
		{
			ParticleManager::PreDraw(cmdList);
			deathParticle->Draw();
			ParticleManager::PostDraw();
		}
	}
}

void AircraftCarrier::TransferConstBuff()
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
	matTrans = XMMatrixTranslation(position.x, position.y - 1.2f, position.z);

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

void AircraftCarrier::Move(const Vector3& incrementValue)
{
	//動きの種類
	switch (currentType)
	{
	case MOVETYPE::RECTANGLE:
		//四角形移動
		RectAngleMove();

		break;

	case MOVETYPE::TRIANGLE:
		//三角形移動
		TriAngleMove();

		break;

	case MOVETYPE::LEFTDIAGONALLY:
		//対角線移動
		LeftDiagonallyMove();

		break;

	case MOVETYPE::UPCURVE:
		//上カーブ移動
		UpCurveMove();

		break;

	case MOVETYPE::DOWNCURVE:
		//下カーブ移動
		DownCurveMove();

		break;

	case MOVETYPE::STAY:
		//停止
		StayMove(incrementValue);

		break;

	default:
		break;
	}

	velocity = velocity.normalize();

	position.x = position.x + velocity.x * speed.x;
	position.y = position.y + velocity.y * speed.y;
}

void AircraftCarrier::RectAngleMove()
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

void AircraftCarrier::TriAngleMove()
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

void AircraftCarrier::LeftDiagonallyMove()
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

void AircraftCarrier::UpCurveMove()
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

void AircraftCarrier::DownCurveMove()
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

void AircraftCarrier::StayMove(const Vector3& incrementValue)
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

	case MOVEPHASE::PHASE2://敵発射処理

		lanchLugTime++;
		if (lanchLugTime > LANCHMAXTIME)
		{
			lanchLugTime = 0;
			lanchFlag = true;
		}

		if (hp < 1)
		{
			for (int i = 0; i < aliens.size(); i++)
			{
				aliens[i]->SetIsDeadFlag(true);
			}
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,90.0f };
			deathParticleFlag = true;
			currentPhase = MOVEPHASE::PHASE3;
		}

		if (lanchFlag)
		{
			Vector3 upPosition = position;
			upPosition.y += 1.0f;
			for (int i = 0; i < 10 / 2; i++)
			{
				aliens[i]->Initialize();
				aliens[i]->SetMoveLugTime(i * 30);
				aliens[i]->SetPosition(upPosition);
				aliens[i]->SetMoveType(MOVETYPE::UPCURVE);
				aliens[i]->Update(incrementValue);
			}

			Vector3 downPosition = position;
			downPosition.y -= 1.0f;
			for (int i = 10 / 2; i < 10; i++)
			{
				aliens[i]->Initialize();
				aliens[i]->SetMoveLugTime((i - 10 / 2) * 30 + 240);
				aliens[i]->SetPosition(downPosition);
				aliens[i]->SetMoveType(MOVETYPE::DOWNCURVE);
				aliens[i]->Update(incrementValue);
			}

			lanchFlag = false;
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

void AircraftCarrier::IsDead()
{
	isDeadFlag = true;
}

void AircraftCarrier::SetMoveType(const MOVETYPE& moveType)
{
	currentType = moveType;

	if (currentType == MOVETYPE::UPCURVE)
	{
		curveTime = 1.0f;
		rotation.x = 270.0f;
	}
	if (currentType == MOVETYPE::DOWNCURVE)
	{
		curveTime = 3.0;
		rotation.x = 90.0f;
	}
}

void AircraftCarrier::SetScrollIncrement(const Vector3& incrementValue)
{
	position += incrementValue;
}

void AircraftCarrier::Damage()
{
	if (invisibleTime <= 0)
	{
		invisibleTime = INVISIBLEMAXTIME;
	}
	hp--;
}

void AircraftCarrier::CriticalDamage()
{
	if (invisibleTime <= 0)
	{
		invisibleTime = INVISIBLEMAXTIME;
	}
	hp -= 50;
}

void AircraftCarrier::DamageEffect()
{
	//無敵演出
	if (invisibleTime > 0)
	{
		invisibleTime--;
		if (invisibleTime % 20 <= 10)
		{
			const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
			color = white;
		}
		else
		{
			const Vector4 black = { 0.0f,0.0f,0.0f,1.0f };
			color = black;
		}
	}
	else
	{
		const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
		color = white;
	}
}

void AircraftCarrier::DeathParticleProcessing()
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


				deathParticle->Add(30, pos, vel, acc, 3.0f, 0.0f, { 1.0f,0.0f,0.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
			}
		}
	}

	deathParticle->Update();
}

bool AircraftCarrier::IsCollision(GameObject* arg_otherObject)
{
	GameObject* otherObject = arg_otherObject;

	if (isDeadFlag || otherObject->GetIsDeadFlag() || otherObject->GetCharacterType() == characterType
		|| otherObject->GetCharacterType() == CHARACTERTYPE::BLOCK || otherObject->GetCharacterType() == CHARACTERTYPE::PLAYER) {
		return false;
	}

	if (Collision::CheckSphereToSphere({ otherObject->GetPosition(),otherObject->GetCollisionRadius() },
		{ position,collisionRadius }))
	{
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

