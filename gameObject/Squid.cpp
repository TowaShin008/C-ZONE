#include "Squid.h"
using namespace Microsoft::WRL;

ID3D12GraphicsCommandList* Squid::cmdList;
XMMATRIX Squid::matView;
XMMATRIX Squid::matProjection;
Camera* Squid::camera = nullptr;

Squid::Squid(ID3D12GraphicsCommandList* arg_cmdList)
{
	cmdList = arg_cmdList;
	scale = { 1.0f,1.0f,1.0f };
	collisionRadius = 2.0f;
	rotation = { 0.0f,0.0f,0.0f };
	speed = { 0.1f,0.1f,0.0f };
	characterType = CHARACTERTYPE::ENEMY;
	matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (float)WindowSize::window_width / (float)WindowSize::window_height, 0.1f, 1000.0f);
}

Squid::~Squid()
{
	for (int i = 0; i < map.size(); ++i)
	{
		for (int j = 0; j < map[i].size(); ++j)
		{
			if (map[i][j] == 1 || map[i][j] == 2)//iが縦でjが横
			{
				delete bodyBlocks[i][j];
				//モデル1のセット
				bodyBlocks[i][j] = nullptr;
			}
		}
	}

	//オブジェクトマネージャークラスに追加しているためこれらの削除は不要
	//delete leftEye;
	//delete rightEye;
}

void Squid::CreateConstBuffer(ID3D12Device* arg_device)
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

Squid* Squid::Create(ID3D12Device* arg_device, ID3D12GraphicsCommandList* arg_cmdList,const Vector3& arg_position)
{
	Squid* squid = new Squid(arg_cmdList);
	squid->Initialize();
	squid->SetPosition(arg_position);
	squid->CreateConstBuffer(arg_device);
	squid->AttachTentacles(arg_device);
	squid->AttachBody(arg_device);
	squid->AttachEye(arg_device);

	ParticleManager* deathParticle = ParticleManager::Create(arg_device);
	deathParticle->SetSelectColor(2);
	squid->SetDeathParticleManager(deathParticle);

	return squid;
}

void Squid::SetEye(const Vector3& arg_eye)
{
	Squid::camera->SetEye(arg_eye);

	UpdateViewMatrix();
}

void Squid::SetTarget(const Vector3& arg_target)
{
	Squid::camera->SetTarget(arg_target);

	UpdateViewMatrix();
}

void Squid::UpdateViewMatrix()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&camera->GetEye()), XMLoadFloat3(&camera->GetTarget()), XMLoadFloat3(&camera->GetUp()));
}

void Squid::SetOBJModel(OBJModel* arg_eyeModel, OBJModel* arg_bodyModel)
{
	SetEyeModel(arg_eyeModel);
	SetBodyModel(arg_bodyModel);
	SetTentaclesModel(arg_bodyModel);
}

void Squid::SetBodyModel(OBJModel* arg_bodyModel)
{
	for (int i = 0; i < map.size(); ++i)
	{
		for (int j = 0; j < map[i].size(); ++j)
		{
			if (map[i][j] == 1 || map[i][j] == 2)//iが縦でjが横
			{
				//モデル1のセット
				bodyBlocks[i][j]->SetOBJModel(arg_bodyModel);
			}
		}
	}
}

void Squid::SetTentaclesModel(OBJModel* arg_bodyModel)
{
	for (int i = 0; i < tentaclesBlocks.size(); ++i)
	{
		tentaclesBlocks[i]->SetOBJModel(arg_bodyModel);
	}
}


void Squid::SetEyeModel(OBJModel* arg_eyeModel)
{
	leftEye->SetOBJModel(arg_eyeModel);
	rightEye->SetOBJModel(arg_eyeModel);
}

void Squid::AttachBody(ID3D12Device* arg_device)
{
	map = {
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,1,1,1,0,1,1,1,0,1,1,1,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
	};

	const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };

	const Vector3 blockFixPos = { -100.0f,20.0f,0.0f };
	bodyBlocks.resize(map.size());
	for (int i = 0; i < map.size(); ++i)
	{
		bodyBlocks[i].resize(map[i].size());
		for (int j = 0; j < map[i].size(); ++j)
		{
			if (map[i][j] == 1 || map[i][j] == 2)//iが縦でjが横
			{//通常ブロックの生成
				bodyBlocks[i][j] = Block::Create(arg_device, cmdList, { j * 6.8f + blockFixPos.x + position.x, -i * 7.1f + blockFixPos.y + +position.y,position.z });
				bodyBlocks[i][j]->SetStageBlockFlag(false);
				bodyBlocks[i][j]->SetScale({1.0f,1.0f,1.0f});
				bodyBlocks[i][j]->Update({ 0.1f,0,0 });
				bodyBlocks[i][j]->SetColor(white);
			}
		}
	}
}

void Squid::AttachTentacles(ID3D12Device* arg_device)
{
	const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };

	const Vector3 blockFixPosition = { 20.0f,-2.0f,-45.0f };
	tentaclesBlocks.resize(TENTACLESNUM);
	for (int i = 0; i < tentaclesBlocks.size(); ++i)
	{//触手の初期化処理
		tentaclesBlocks[i] = Block::Create(arg_device, cmdList, { blockFixPosition.x + position.x , -i * 1.4f + blockFixPosition.y + position.y ,blockFixPosition.z + position.z });
		tentaclesBlocks[i]->SetStageBlockFlag(false);
		tentaclesBlocks[i]->SetScale({ 0.2f,0.2f,0.2f });
		tentaclesBlocks[i]->Update({ 0.1f,0,0 });
		tentaclesBlocks[i]->SetColor(white);
	}
}

void Squid::AttachEye(ID3D12Device* arg_device)
{
	const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
	//左目の初期化処理
	const Vector3 leftEyeFixPosition = { -18.5f,-12.0f,0.0f };
	leftEye = SquidEye::Create(arg_device, cmdList, { leftEyeFixPosition.x + position.x,leftEyeFixPosition.y + position.y , leftEyeFixPosition.z + position.z });
	leftEye->SetScale({ 1.0f,1.0f,1.0f });
	leftEye->Update({ 0.0f,0,0 });
	leftEye->SetColor(white);
	//右目の初期化処理
	const Vector3 rightEyeFixPosition = { 9.0f,-12.0f,0.0f };
	rightEye = SquidEye::Create(arg_device, cmdList, { rightEyeFixPosition.x + position.x,rightEyeFixPosition.y + position.y ,rightEyeFixPosition.z + position.z });
	rightEye->SetScale({ 1.0f,1.0f,1.0f });
	rightEye->Update({ 0.0f,0,0 });
	rightEye->SetColor(white);
}

void Squid::Initialize()
{
	isDeadFlag = false;
	moveLugTime = 0;
	currentPhase = MOVEPHASE::PHASE1;
	currentType = MOVETYPE::STAY;
	moveEndFlag = false;
	shotFlag = true;
	velocity = { 0,0,0 };
	const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
	color = white;
	speed = { 0.1f,0.1f,0.0f };
	hp = 400;
	deathParticleFlag = false;
	invisibleTime = 0;
}
//プレイヤーの更新処理
void Squid::Update(const Vector3& arg_incrementValue, const Vector3& arg_playerPosition)
{
	centerPosition += arg_incrementValue.x;

	leftEye->Update(arg_incrementValue);
	rightEye->Update(arg_incrementValue);
	if (isDeadFlag == false)
	{
		SetScrollIncrement(arg_incrementValue);

		if (moveLugTime <= 0)
		{
			if (moveEndFlag == false)
			{//移動処理
				Move(arg_playerPosition);
			}
		}
		else
		{
			moveLugTime--;
		}
		//ダメージ演出
		DamageEffect();

		if ((leftEye->GetHp() + rightEye->GetHp()) <= 0)
		{
			hp = 0;
		}
		//呼吸をしているときの上下の揺れ
		BreathMove();

		//水面から上がってくる処理
		FloatMove();

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

void Squid::Draw()
{
	if (isDeadFlag)
	{
		return;
	}

	// 親オブジェクトがあれば
	if (parent != nullptr) {
		// 親オブジェクトのワールド行列を掛ける
		matWorld *= parent->matWorld;
	}

	for (int i = 0; i < map.size(); ++i)
	{
		for (int j = 0; j < map[i].size(); ++j)
		{
			if (map[i][j] == 1 || map[i][j] == 2)//iが縦でjが横
			{
				//体の描画
				bodyBlocks[i][j]->Draw();
			}
		}
	}
	//定数バッファの転送
	TransferConstBuff();

	Squid::cmdList->SetPipelineState(PipelineState::simplePipelineState.Get());

	Squid::cmdList->SetGraphicsRootSignature(RootSignature::simpleRootsignature.Get());

	Squid::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Squid::cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
	Squid::cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

	//左目の描画
	leftEye->Draw();
	//右目の描画
	rightEye->Draw();

	if (deathParticleFlag)
	{//死亡パーティクル描画
		ParticleManager::PreDraw(cmdList);
		deathParticle->Draw();
		ParticleManager::PostDraw();
	} 
}

void Squid::TransferConstBuff()
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

	//ワールド行列の転送
	ConstBufferDataB0* constMap0 = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap0);
	constMap0->color = color;
	constMap0->mat = matWorld * matView * matProjection;	// 行列の合成
	constBuffB0->Unmap(0, nullptr);
}

void Squid::Move(const Vector3& arg_playerPosition)
{
	//動きの種類
	switch (currentType)
	{
	case MOVETYPE::STAY:

		StayMove(arg_playerPosition);

		break;

	default:
		break;
	}

	velocity = velocity.normalize();

	position.x = position.x + velocity.x * speed.x;
	position.y = position.y + velocity.y * speed.y;
}

void Squid::StayMove(const Vector3& arg_playerPosition)
{
	const float leftEyeMinPositionY = -100.0f;
	const float tentacleAngleIncrementSize = 4.0f;
	const float tentacleMaxAngle = 360.0f;
	switch (currentPhase)
	{
	case MOVEPHASE::PHASE1://定位置に来たら次のフェーズへ

		if (position.y >= 0)
		{
			velocity = { 0,0,0 };
			currentPhase = MOVEPHASE::PHASE2;
		}

		break;

	case MOVEPHASE::PHASE2://ゲームプレイ時の動作

		//触手をうねらせる処理
		tentacleAngle += tentacleAngleIncrementSize;
		if (tentacleAngle >= tentacleMaxAngle)
		{
			tentacleAngle = 0;
		}
		//触手のうねり処理
		TentacleMove();

		if (attackFlag)
		{
			if (upTentacleAttackFlag)
			{//上からの触手攻撃
				UpTentacleAttack();
			}
			else
			{//下からの触手攻撃
				DownTentacleAttack();
			}

			for (int i = 0; i < tentaclesBlocks.size(); ++i)
			{
				tentaclesBlocks[i]->Update({ 0,0,0 });
			}
		}
		else
		{
			rnd = rand() % 100;

			if (rnd == 0)
			{//プレイヤーのY軸のポジションによって攻撃方法を変える
				if (arg_playerPosition.y > 0.0f)
				{
					upTentacleAttackFlag = true;
				}
				else
				{
					upTentacleAttackFlag = false;
				}
				attackFlag = true;
			}
		}

		if (hp < 1)
		{//死亡演出の開始
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,0.0f };
			deathParticleFlag = true;
			currentPhase = MOVEPHASE::PHASE3;
		}

		break;

	case MOVEPHASE::PHASE3://死亡動作

		velocity = { 0,-1,0 };

		if (leftEye->GetPosition().y < leftEyeMinPositionY)
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

void Squid::IsDead()
{
	isDeadFlag = true;
}


void Squid::SetScrollIncrement(const Vector3& arg_incrementValue)
{
	position += arg_incrementValue;
}

void Squid::Damage()
{
	invisibleTime = INVISIBLEMAXTIME;
	hp--;
}

void Squid::CriticalDamage()
{
	invisibleTime = INVISIBLEMAXTIME;
	hp -= 50;
}

void Squid::DamageEffect()
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

void Squid::DeathParticleProcessing()
{
	//死亡時パーティクル
	if (deathParticleFlag)
	{
		if (deathParticle->GetParticleLength() < ParticleManager::vertexCount)
		{
			const float rnd_pos = 10.0f;
			//パーティクルを出すブロックのポジションを取得
			Vector3 pos = bodyBlocks[0][14]->GetPosition();
			pos.x = pos.x - centerPosition;

			pos.x += (float)rand() / RAND_MAX * rnd_pos;
			pos.y += (float)rand() / RAND_MAX * rnd_pos;
			for (int i = 0; i < 10; i++)
			{
				const float rnd_vel = 0.1f;
				Vector3 vel{};

				//X[-0.05f,+0.15f]でランダムに分布
				vel.x = ((float)rand() / RAND_MAX * rnd_vel - rnd_vel / 4.0f) * 2;
				//Y[0.0f,+0.2f]でランダムに分布
				vel.y = (float)rand() / RAND_MAX * rnd_vel * 2;
				vel.z = 0.0f;
				const float rnd_acc = 0.01f;
				Vector3 acc{};
				//重力に見立ててYのみ[-0.01f,0]でランダムに分布
				acc.y = -(float)rand() / RAND_MAX * rnd_acc;

				const float startScale = 3.0f;
				const float endScale = 0.0f;
				const Vector4 startColor = { 1.0f,0.0f,0.0f,1.0f };
				const Vector4 endColor = { 1.0f,1.0f,1.0f,1.0f };
				deathParticle->Add(60, pos, vel, acc, startScale, endScale, startColor, endColor);
			}
		}
	}

	deathParticle->Update();
}

bool Squid::IsCollision(GameObject* arg_otherObject)
{
	GameObject* otherObject = arg_otherObject;

	if (isDeadFlag || otherObject->GetIsDeadFlag() || otherObject->GetCharacterType() == characterType
		|| otherObject->GetCharacterType() == CHARACTERTYPE::BLOCK) {
		return false;
	}

	if (Collision::CheckSphereToSphere({ otherObject->GetPosition(),otherObject->GetCollisionRadius() },
		{ leftEye->GetPosition(),collisionRadius }))
	{
		if (otherObject->GetCharacterType() == CHARACTERTYPE::PLAYER)
		{
			otherObject->Damage();
			return true;
		}
		//敵の大きさによってダメージ量を変化させる
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

void Squid::UpTentacleAttack()
{
	const float positionXIncrementSize = 0.4f;
	const float positionYIncrementSize = 0.2f;
	switch (attackPhase)
	{
		float maxButtomPosition;
		float maxLeftPosition;
		float maxTopPosition;
	case MOVEPHASE::PHASE1://下に下がる処理

		for (int i = 0; i < tentaclesBlocks.size(); ++i)
		{
			Vector3 position = tentaclesBlocks[i]->GetPosition();
			position.y -= positionYIncrementSize;
			tentaclesBlocks[i]->SetPosition(position);
		}
		maxButtomPosition = -20.0f;
		//触手の先端が定位置に来たら次のフェーズへ
		if (tentaclesBlocks[0]->GetPosition().y <= maxButtomPosition)
		{
			for (int i = 0; i < tentaclesBlocks.size(); ++i)
			{
				Vector3 position = tentaclesBlocks[i]->GetPosition();
				position = { 20.0f, i * 1.4f + 12.0f,0.0f };
				tentaclesBlocks[i]->SetPosition(position);
			}
			attackPhase = MOVEPHASE::PHASE2;
		}

		break;

	case MOVEPHASE::PHASE2://上から下に下がる処理

		for (int i = 0; i < tentaclesBlocks.size(); ++i)
		{
			Vector3 position = tentaclesBlocks[i]->GetPosition();
			position.y -= positionYIncrementSize;
			tentaclesBlocks[i]->SetPosition(position);
		}

		maxButtomPosition = -2.0f;
		//触手の先端が定位置に来たら次のフェーズへ
		if (tentaclesBlocks[0]->GetPosition().y <= maxButtomPosition)
		{
			for (int i = 0; i < tentaclesBlocks.size(); ++i)
			{
				tentaclesBlocks[i]->SetPosition({ 20.0f, i * 1.4f - 2.0f,0.0f });
			}
			attackPhase = MOVEPHASE::PHASE3;
		}

		break;

	case MOVEPHASE::PHASE3://横に移動する処理

		
		homePositionX -= positionXIncrementSize;

		maxLeftPosition = -20.0f;
		//触手の先端が定位置に来たら次のフェーズへ
		if (tentaclesBlocks[0]->GetPosition().x <= maxLeftPosition)
		{
			const Vector3 basicTentaclePosition = { -20.0f,-2.0f,0.0f };
			for (int i = 0; i < tentaclesBlocks.size(); ++i)
			{
				tentaclesBlocks[i]->SetPosition({ basicTentaclePosition.x, i * 1.4f + basicTentaclePosition.y,basicTentaclePosition.z });
			}
			attackPhase = MOVEPHASE::PHASE4;
		}

		break;

	case MOVEPHASE::PHASE4://上に上がる処理

		for (int i = 0; i < tentaclesBlocks.size(); ++i)
		{
			Vector3 position = tentaclesBlocks[i]->GetPosition();
			position.y += positionYIncrementSize;
			tentaclesBlocks[i]->SetPosition(position);
		}

		maxTopPosition = 12.0f;
		//触手の先端が定位置に来たら次のフェーズへ
		if (tentaclesBlocks[0]->GetPosition().y >= maxTopPosition)
		{
			homePositionX = 20.0f;
			for (int i = 0; i < tentaclesBlocks.size(); ++i)
			{
				tentaclesBlocks[i]->SetPosition({ homePositionX , -i * 1.4f - 20.0f ,5.0f });
			}
			attackPhase = MOVEPHASE::PHASE5;
		}

		break;

	case MOVEPHASE::PHASE5://下から上に上がる処理

		for (int i = 0; i < tentaclesBlocks.size(); ++i)
		{
			Vector3 position = tentaclesBlocks[i]->GetPosition();
			position.y += positionYIncrementSize;
			tentaclesBlocks[i]->SetPosition(position);
		}

		maxTopPosition = -5.0f;
		//触手の先端が定位置に来たら次のフェーズへ
		if (tentaclesBlocks[0]->GetPosition().y >= maxTopPosition)
		{
			attackPhase = MOVEPHASE::PHASE1;
			attackFlag = false;
		}

		break;

	default:
		break;
	}
}

void Squid::DownTentacleAttack()
{
	const float positionYIncrementSize = 0.2f;
	const float homePositionIncrementSize = 0.4f;
	switch (attackPhase)
	{
		float maxButtomPosition;
		float maxTopPosition;
		float maxLeftPosition;
	case MOVEPHASE::PHASE1://下に下がる処理


		for (int i = 0; i < tentaclesBlocks.size(); ++i)
		{
			Vector3 position = tentaclesBlocks[i]->GetPosition();
			position.y -= positionYIncrementSize;
			tentaclesBlocks[i]->SetPosition(position);
		}

		maxButtomPosition = -20.0f;
		//触手の先端が定位置に来たら次のフェーズへ
		if (tentaclesBlocks[0]->GetPosition().y <= maxButtomPosition)
		{
			for (int i = 0; i < tentaclesBlocks.size(); ++i)
			{
				const Vector3 position = { 20.0f , -i * 1.4f - 5.0f ,0.0f};
				tentaclesBlocks[i]->SetPosition(position);
			}
			attackPhase = MOVEPHASE::PHASE2;
		}

		break;

	case MOVEPHASE::PHASE2://上に上がる処理

		for (int i = 0; i < tentaclesBlocks.size(); ++i)
		{
			Vector3 position = tentaclesBlocks[i]->GetPosition();
			position.y += positionYIncrementSize;
			tentaclesBlocks[i]->SetPosition(position);
		}

		maxTopPosition = 0.0f;
		//触手の先端が定位置に来たら次のフェーズへ
		if (tentaclesBlocks[0]->GetPosition().y >= maxTopPosition)
		{
			for (int i = 0; i < tentaclesBlocks.size(); ++i)
			{
				const Vector3 position = { 20.0f, -i * 1.4f - 0.0f,0.0f };
				tentaclesBlocks[i]->SetPosition(position);
			}
			attackPhase = MOVEPHASE::PHASE3;
		}

		break;

	case MOVEPHASE::PHASE3://横に移動する処理

		homePositionX -= homePositionIncrementSize;

		maxLeftPosition = -20.0f;
		//触手の先端が定位置に来たら次のフェーズへ
		if (tentaclesBlocks[0]->GetPosition().x <= maxLeftPosition)
		{
			for (int i = 0; i < tentaclesBlocks.size(); ++i)
			{
				const Vector3 position = { -20.0, -i * 1.4f - 0.0f,0.0f };
				tentaclesBlocks[i]->SetPosition(position);
			}
			attackPhase = MOVEPHASE::PHASE4;
		}

		break;

	case MOVEPHASE::PHASE4://下に下がる処理

		for (int i = 0; i < tentaclesBlocks.size(); ++i)
		{
			Vector3 position = tentaclesBlocks[i]->GetPosition();
			position.y -= positionYIncrementSize;
			tentaclesBlocks[i]->SetPosition(position);
		}

		maxButtomPosition = -20.0f;
		//触手の先端が定位置に来たら次のフェーズへ
		if (tentaclesBlocks[0]->GetPosition().y <= maxButtomPosition)
		{
			homePositionX = 20.0f;
			for (int i = 0; i < tentaclesBlocks.size(); ++i)
			{
				const Vector3 position = { homePositionX , -i * 1.4f - 20.0f ,5.0f };
				tentaclesBlocks[i]->SetPosition(position);
			}
			attackPhase = MOVEPHASE::PHASE5;
		}

		break;

	case MOVEPHASE::PHASE5://上に上がる処理

		for (int i = 0; i < tentaclesBlocks.size(); ++i)
		{
			Vector3 position = tentaclesBlocks[i]->GetPosition();
			position.y += positionYIncrementSize;
			tentaclesBlocks[i]->SetPosition(position);
		}

		maxTopPosition = -5.0f;
		//触手の先端が定位置に来たら次のフェーズへ
		if (tentaclesBlocks[0]->GetPosition().y >= maxTopPosition)
		{
			attackPhase = MOVEPHASE::PHASE1;
			attackFlag = false;
		}
		break;

	default:
		break;
	}
}

void Squid::TentacleMove()
{
	for (int i = 0; i < tentaclesBlocks.size(); ++i)
	{
		Vector3 position = tentaclesBlocks[i]->GetPosition();
		position.x = homePositionX + cos(tentacleAngle / 180.0f * XM_PI + (i * 0.5f)) * 2.0f;
		tentaclesBlocks[i]->SetPosition(position);
		tentaclesBlocks[i]->Update({ 0.0f,0,0 });
	}
}

void Squid::FloatMove()
{
	if (position.y < 0.0f)
	{
		const float tentacleAngleIncrementSize = 4.0f;
		const float tentacleMaxAngle = 360.0f;
		tentacleAngle += tentacleAngleIncrementSize;
		if (tentacleAngle >= tentacleMaxAngle)
		{
			tentacleAngle = 0;
		}

		for (int i = 0; i < tentaclesBlocks.size(); ++i)
		{//三角関数で計算した分を体のブロック全体に加算
			float positionX = tentaclesBlocks[i]->GetPosition().x;
			const float homePositionX = 20.0f;
			positionX = homePositionX + cos(tentacleAngle / 180.0f * XM_PI + (i * 0.5f)) * 2.0f;
			tentaclesBlocks[i]->SetPosition({ positionX , -i * 1.4f - 2.0f + position.y ,-45.0f + position.z });
			tentaclesBlocks[i]->Update({ 0.0f,0,0 });
		}
	}
}

void Squid::BreathMove()
{
	const Vector3 fixBodyPosition = { -100.0f,20.0f,0.0f };
	const float bodyIncrementAngle = 0.5f;
	bodyAngle += bodyIncrementAngle;
	const float bodyMaxAngle = 360.0f;
	if (bodyAngle > bodyMaxAngle)
	{
		bodyAngle = 0.0f;
	}

	const Vector3 blockScale = { 6.8f,7.1f,1.0f };
	for (int i = 0; i < map.size(); ++i)
	{
		for (int j = 0; j < map[i].size(); ++j)
		{
			if (map[i][j] == 1 || map[i][j] == 2)//iが縦でjが横
			{//三角関数で計算した分を体のブロック全体に加算
				bodyBlocks[i][j]->SetPosition({ j * blockScale.x + fixBodyPosition.x + position.x, -i * blockScale.y + fixBodyPosition.y + position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z + fixBodyPosition.z });
				bodyBlocks[i][j]->Update({ 0,0,0 });
			}
		}
	}


	const float eyeIncrementCount = 0.5f;
	eyeScaleCount += eyeIncrementCount;
	const float eyeScaleMaxCount = 180.0f;
	if (eyeScaleCount > eyeScaleMaxCount)
	{
		eyeScaleCount = 0.0f;
	}

	Vector3 eyeRotation = leftEye->GetRotasion();
	const float eyeRotationIncrementSize = 1.0f;
	const float eyeMaxRotation = 360.0f;
	if (eyeRotation.z < eyeMaxRotation)
	{
		eyeRotation.z += eyeRotationIncrementSize;
	}
	else
	{
		eyeRotation.z = 0.0f;
	}

	const Vector3 basicEyeScale = { 1.0f,1.0f,1.0f };
	const Vector3 eyeScale = { basicEyeScale.x + sin((eyeScaleCount / 180.0f) * XM_PI) * 0.2f,basicEyeScale.y + sin((eyeScaleCount / 180.0f) * XM_PI) * 0.2f,basicEyeScale.z + sin((eyeScaleCount / 180.0f) * XM_PI) * 0.2f };

	//左目の呼吸と回転処理
	const Vector3 leftEyeFixPosition = { -18.5f ,-12.0f,0.0f};
	leftEye->SetPosition({ leftEyeFixPosition.x + position.x,leftEyeFixPosition.y + position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f , leftEyeFixPosition.z + position.z });
	leftEye->SetRotation(eyeRotation);
	leftEye->SetScale(eyeScale);
	//右目の呼吸と回転処理
	const Vector3 rightEyeFixPosition = { 9.0f ,-12.0f,0.0f };
	rightEye->SetPosition({ rightEyeFixPosition.x + position.x,rightEyeFixPosition.y + position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f ,rightEyeFixPosition.z + position.z });
	rightEye->SetRotation(eyeRotation);
	rightEye->SetScale(eyeScale);
}

