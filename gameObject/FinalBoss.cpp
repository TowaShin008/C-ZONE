#include "FinalBoss.h"
using namespace Microsoft::WRL;

ID3D12GraphicsCommandList* FinalBoss::cmdList;
XMMATRIX FinalBoss::matView;
XMMATRIX FinalBoss::matProjection;
Camera* FinalBoss::camera = nullptr;
ID3D12Device* FinalBoss::device = nullptr;

FinalBoss::FinalBoss(ID3D12GraphicsCommandList* arg_cmdList, ID3D12Device* arg_device)
{
	cmdList = arg_cmdList;
	device = arg_device;
	scale = { 1.0f,1.0f,1.0f };
	collisionRadius = 2.0f;
	rotation = { 0.0f,0.0f,0.0f };
	speed = { 0.1f,0.1f,0.0f };
	characterType = CHARACTERTYPE::ENEMY;
	matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (float)WindowSize::window_width / (float)WindowSize::window_height, 0.1f, 1000.0f);
}

FinalBoss::~FinalBoss()
{
	delete bodyBlock;

	for (int i = 0; i < bigBullets.size(); ++i)
	{
		delete bigBullets[i];
		bigBullets[i] = nullptr;
	}

	delete parentLeftWing;
	for (int i = 0; i < leftWing.size(); ++i)
	{
		delete leftWing[i];
		leftWing[i] = nullptr;
	}

	delete parentRightWing;
	for (int i = 0; i < rightWing.size(); ++i)
	{
		delete rightWing[i];
		rightWing[i] = nullptr;
	}
}

void FinalBoss::CreateConstBuffer()
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

FinalBoss* FinalBoss::Create(ID3D12Device* arg_device, ID3D12GraphicsCommandList* arg_cmdList,const Vector3& arg_position)
{
	FinalBoss* finalBoss = new FinalBoss(arg_cmdList, arg_device);
	finalBoss->SetPosition(arg_position);
	finalBoss->CreateConstBuffer();
	finalBoss->AttachBody(arg_device);
	finalBoss->AttachEye(arg_device);
	finalBoss->AttachLeftWing(arg_device);
	finalBoss->AttachRightWing(arg_device);
	finalBoss->AttachBullet(arg_device);
	finalBoss->Initialize();

	ParticleManager* deathParticle = ParticleManager::Create(arg_device);
	const int red = 2;
	deathParticle->SetSelectColor(red);
	finalBoss->SetDeathParticleManager(deathParticle);

	return finalBoss;
}

void FinalBoss::SetEye(const Vector3& arg_eye)
{
	FinalBoss::camera->SetEye(arg_eye);

	UpdateViewMatrix();
}

void FinalBoss::SetTarget(const Vector3& arg_target)
{
	FinalBoss::camera->SetTarget(arg_target);

	UpdateViewMatrix();
}

void FinalBoss::UpdateViewMatrix()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&camera->GetEye()), XMLoadFloat3(&camera->GetTarget()), XMLoadFloat3(&camera->GetUp()));
}

void FinalBoss::SetOBJModel(ObjFileModel* arg_eyeModel, ObjFileModel* arg_bodyModel, ObjFileModel* arg_wingModel, ObjFileModel* arg_bulletModel)
{
	SetEyeModel(arg_eyeModel);
	SetBodyModel(arg_bodyModel);
	SetWingModel(arg_wingModel);
	SetBulletModel(arg_bulletModel);
}

void FinalBoss::SetBodyModel(ObjFileModel* arg_bodyModel)
{
	bodyBlock->SetOBJModel(arg_bodyModel);
}


void FinalBoss::SetEyeModel(ObjFileModel* arg_eyeModel)
{
	finalBossEye->SetOBJModel(arg_eyeModel);
}

void FinalBoss::SetWingModel(ObjFileModel* arg_wingModel)
{
	for (int i = 0; i < leftWing.size(); ++i)
	{
		leftWing[i]->SetOBJModel(arg_wingModel);
	}

	for (int i = 0; i < rightWing.size(); ++i)
	{
		rightWing[i]->SetOBJModel(arg_wingModel);
	}
}

void FinalBoss::SetBulletModel(ObjFileModel* arg_bulletModel)
{
	for (int i = 0; i < bigBullets.size(); ++i)
	{
		bigBullets[i]->SetOBJModel(arg_bulletModel);
	}
}

void FinalBoss::AttachBody(ID3D12Device* arg_device)
{//体の初期化処理
	bodyBlock = Block::Create(arg_device, cmdList, { 0,0,position.z });
	bodyBlock->SetStageBlockFlag(false);
	bodyBlock->SetScale({ 1.0f,1.0f,1.0f });
	bodyBlock->Update({ 0,0,0 });
	const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
	bodyBlock->SetColor(white);
}

void FinalBoss::AttachEye(ID3D12Device* arg_device)
{//目の初期化処理
	const Vector3 finalBossEyeFixPosition = { 0.0f,2.0f,-13.5f };
	finalBossEye = FinalBossEye::Create(arg_device, cmdList, { finalBossEyeFixPosition.x,finalBossEyeFixPosition.y + position.y , position.z + finalBossEyeFixPosition.z });
	finalBossEye->SetScale({ 3.0f,3.0f,1.0f });
	finalBossEye->Update({ 0.0f,0.0f,0.0f });
	const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
	finalBossEye->SetColor(white);
}

void FinalBoss::AttachLeftWing(ID3D12Device* arg_device)
{//左の翼の初期化処理
	parentLeftWing = Block::Create(arg_device, cmdList, { -3.0f, position.y,position.z });
	parentLeftWing->SetScale({ 2.0f,2.0f,2.0f });
	parentLeftWing->Update({ 0,0,0 });
	const int leftWingNum = 6;
	leftWing.resize(leftWingNum);
	for (int i = 0; i < leftWing.size(); ++i)
	{
		if (i < leftWingNum / 2)
		{
			leftWing[i] = Block::Create(arg_device, cmdList, { - 9.0f, i * 2.0f + position.y,position.z });
			leftWing[i]->SetSpeed({ 0.01f,0.01f,0.01f });
		}
		else
		{
			leftWing[i] = Block::Create(arg_device, cmdList, { - 12.0f, (i - 3) * 4.0f + position.y - 2.0f,position.z });
			leftWing[i]->SetSpeed({ 0.05f,0.05f,0.05f });
		}
		leftWing[i]->SetStageBlockFlag(false);
		leftWing[i]->SetScale({ 1.0f,1.0f,1.0f });
		leftWing[i]->Update({ 0.0f,0,0 });
		leftWing[i]->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		leftWing[i]->SetParent(parentLeftWing);

		const int windNum = i % 3;
		if (windNum == 0)
		{
			leftWing[i]->SetRotation({ 0.0f,0.0f, 45.0f });
		}
		else if (windNum == 2)
		{
			leftWing[i]->SetRotation({ 0.0f,0.0f,-45.0f });
		}
	}
}

void FinalBoss::AttachRightWing(ID3D12Device* arg_device)
{//右の翼の初期化処理
	parentRightWing = Block::Create(arg_device, cmdList, { 3.0f, position.y,position.z });
	parentRightWing->SetScale({ 2.0f,2.0f,2.0f });
	parentRightWing->Update({ 0,0,0 });
	const int RightWingNum = 6;
	rightWing.resize(RightWingNum);
	for (int i = 0; i < rightWing.size(); ++i)
	{
		if (i < RightWingNum / 2)
		{
			rightWing[i] = Block::Create(arg_device, cmdList, { 9.0f, i * 2.0f + position.y,position.z });
			rightWing[i]->SetSpeed({ 0.01f,0.01f,0.01f });
		}
		else
		{
			rightWing[i] = Block::Create(arg_device, cmdList, { 12.0f, (i - 3) * 4.0f + position.y - 2.0f,position.z });
			rightWing[i]->SetSpeed({ 0.05f,0.05f,0.05f });
		}

		rightWing[i]->SetStageBlockFlag(false);
		rightWing[i]->SetScale({ 1.0f,1.0f,1.0f });

		rightWing[i]->Update({ 0.0f,0,0 });
		const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
		rightWing[i]->SetColor(white);
		rightWing[i]->SetParent(parentRightWing);

		const int windNum = i % 3;
		if (windNum == 0)
		{
			rightWing[i]->SetRotation({ 0.0f,0.0f,-45.0f });
		}
		else if (windNum == 2)
		{
			rightWing[i]->SetRotation({ 0.0f,0.0f, 45.0f });
		}
	}
}

void FinalBoss::AttachBullet(ID3D12Device* arg_device)
{
	bigBullets.resize(BIGBULLETMAXNUM);
	for (int i = 0; i < bigBullets.size(); ++i)
	{
		bigBullets[i] = HomingBullet::Create(arg_device, cmdList);
		bigBullets[i]->SetIsDeadFlag(true);
		bigBullets[i]->SetCollisionRadius(2.0f);
		bigBullets[i]->SetCharacterType(CHARACTERTYPE::ENEMY);
		bigBullets[i]->SetScale({ 2.5f,2.5f,2.5f });
	}

}

void FinalBoss::Initialize()
{
	isDeadFlag = false;
	shotLugTime = 60;
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
	entryFlag = true;
	InitialPosture();
}
//プレイヤーの更新処理
void FinalBoss::Update(const Vector3& arg_incrementValue, const Vector3& arg_playerPosition)
{
	centerPosition += arg_incrementValue.x;

	if (isDeadFlag == false)
	{
		SetScrollIncrement(arg_incrementValue);

		if (moveEndFlag == false)
		{//移動処理
			Move();
		}
		//ダメージ演出
		DamageEffect();


		if (finalBossEye->GetHp() <= 0)
		{
			hp = 0;
		}

		if (entryFlag)
		{//開始演出
			EntryPerformance();
		}
		else
		{
			if (bodyAttackFlag&&currentPhase!=MOVEPHASE::PHASE3)
			{//体当たり演出
				BodyAttack();
			}
			else
			{
				const int randBodyAttack = rand() % 400;
				//呼吸をしているときの上下の揺れ
				BreathMove();
				if (randBodyAttack == 0)
				{//体当たり以前の翼の回転具合を保存しておく
					previousParentLeftWingRotation = parentLeftWing->GetRotasion();
					previousParentRightWingRotation = parentRightWing->GetRotasion();
					bodyAttackFlag = true;
				}
			}
		}


		//射撃処理
		ShotBullet(arg_incrementValue, arg_playerPosition);

		if (blinkFlag)
		{//瞬き処理
			Blink();
		}
	}
	//死亡パーティクル処理
	DeathParticleProcessing();
}

void FinalBoss::Draw()
{
	if (isDeadFlag)
	{
		return;
	}

	TransferConstBuff();

	FinalBoss::cmdList->SetPipelineState(PipelineState::simplePipelineState.Get());

	FinalBoss::cmdList->SetGraphicsRootSignature(RootSignature::simpleRootsignature.Get());

	FinalBoss::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	FinalBoss::cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
	FinalBoss::cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

	finalBossEye->Draw();

	for (int i = 0; i < leftWing.size(); ++i)
	{
		leftWing[i]->Draw();
	}

	for (int i = 0; i < rightWing.size(); ++i)
	{
		rightWing[i]->Draw();
	}

	if (deathParticleFlag)
	{
		ParticleManager::PreDraw(cmdList);
		deathParticle->Draw();
		ParticleManager::PostDraw();
	}

	for (int i = 0; i < bigBullets.size(); i++)
	{
		if (bigBullets[i]->GetIsDeadFlag() == false)
		{
			bigBullets[i]->Draw();
		}
	}
}

void FinalBoss::TransferConstBuff()
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

	bodyBlock->Draw();

	//ワールド行列の転送
	ConstBufferDataB0* constMap0 = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap0);
	constMap0->color = color;
	constMap0->mat = matWorld * matView * matProjection;	// 行列の合成
	constBuffB0->Unmap(0, nullptr);
}

void FinalBoss::Move()
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

void FinalBoss::StayMove()
{
	switch (currentPhase)
	{
		float moveEndPositionY;
	case MOVEPHASE::PHASE1://左に進む処理

		if (position.y >= 0)
		{
			velocity = { 0,0,0 };
			currentPhase = MOVEPHASE::PHASE2;
		}

		break;

	case MOVEPHASE::PHASE2://下に進む処理


		if (hp < 1)
		{
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,0.0f };
			deathParticleFlag = true;
			currentPhase = MOVEPHASE::PHASE3;
		}

		break;

	case MOVEPHASE::PHASE3://下に進む処理

		velocity = { 0,-1,0 };
		shotFlag = false;
		EndPerformance();

		moveEndPositionY = -100.0f;
		if (finalBossEye->GetPosition().y < moveEndPositionY)
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

void FinalBoss::IsDead()
{
	isDeadFlag = true;
}

void FinalBoss::SetScrollIncrement(const Vector3& arg_incrementValue)
{
	position += arg_incrementValue;
}

void FinalBoss::Damage()
{
	if (invisibleTime <= 0)
	{
		invisibleTime = INVISIBLEMAXTIME;
	}
	hp--;
}

void FinalBoss::CriticalDamage()
{
	if (invisibleTime <= 0)
	{
		invisibleTime = INVISIBLEMAXTIME;
	}
	hp -= 50;
}

void FinalBoss::DamageEffect()
{
	//ダメージ演出
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

void FinalBoss::DeathParticleProcessing()
{
	//死亡時パーティクル
	if (deathParticleFlag)
	{
		if (deathParticle->GetParticleLength() < ParticleManager::vertexCount)
		{
			const float rnd_pos = 10.0f;
			Vector3 pos = bodyBlock->GetPosition();
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
				const Vector4 red = { 1.0f,0.0f,0.0f,1.0f };
				const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
				const Vector4 startColor = red;
				const Vector4 endColor = white;
				deathParticle->Add(60, pos, vel, acc, startScale, endScale, startColor, endColor);
			}
		}
	}

	deathParticle->Update();
}

bool FinalBoss::IsCollision(GameObject* arg_otherObject)
{
	GameObject* otherObject = arg_otherObject;

	if (isDeadFlag || otherObject->GetIsDeadFlag() || otherObject->GetCharacterType() == characterType
		|| otherObject->GetCharacterType() == CHARACTERTYPE::BLOCK) {
		return false;
	}

	for (int i = 0; i < bigBullets.size(); i++)
	{
		bigBullets[i]->IsCollision(otherObject);
	}
	

	if (Collision::CheckSphereToSphere({ otherObject->GetPosition(),otherObject->GetCollisionRadius() },
		{ finalBossEye->GetPosition(),collisionRadius }))
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

void FinalBoss::BreathMove()
{
	const float bodyAngleIncrementSize = 0.5f;
	bodyAngle += bodyAngleIncrementSize;
	if (bodyAngle > 360)
	{
		bodyAngle = 0.0f;
	}
	bodyBlock->SetPosition({  position.x,  position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });
	bodyBlock->Update({ 0,0,0 });


	//目の呼吸処理
	const Vector3 eyeFixPosition = { 0.0f,2.0f,-13.5f };
	finalBossEye->SetPosition({ position.x,eyeFixPosition.y + position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f , position.z + eyeFixPosition.z });
	//左の翼の呼吸処理
	const float leftWingPositionX = position.x - 3.0f;
	parentLeftWing->SetPosition({ leftWingPositionX,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });
	//左の翼の羽ばたき処理
	const Vector3 baseLeftWingRotation = { 0.0f,-10.0f,0.0f };
	parentLeftWing->SetRotation({ baseLeftWingRotation.x,cos((bodyAngle / 180.0f) * XM_PI) * 20.0f + baseLeftWingRotation.y,baseLeftWingRotation.z});

	parentLeftWing->Update({0,0,0});
	for (int i = 0; i < leftWing.size(); ++i)
	{
		leftWing[i]->Update({0,0,0});
	}
	//右の翼の呼吸処理
	const float rightWingPositionX = position.x + 3.0f;
	parentRightWing->SetPosition({ rightWingPositionX,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });
	//右の翼の羽ばたき処理
	const Vector3 baseRightWingRotation = { 0.0f,10.0f,0.0f };
	parentRightWing->SetRotation({ baseRightWingRotation.x, - cos((bodyAngle / 180.0f) * XM_PI) * 20.0f + baseRightWingRotation.y,baseRightWingRotation.z });

	parentRightWing->Update({ 0,0,0 });
	for (int i = 0; i < rightWing.size(); ++i)
	{
		rightWing[i]->Update({0,0,0});
	}
}

void FinalBoss::ShotBullet(const Vector3& arg_incrementValue, const Vector3& arg_playerPosition)
{
	if (shotFlag)
	{
		for (int i = 0; i < bigBullets.size(); i++)
		{
			//射撃処理
			if (bigBullets[i]->GetIsDeadFlag())
			{
				Vector3 shotPosition = finalBossEye->GetPosition();
				shotPosition.y -= 2.0f;
				bigBullets[i]->Initialize();
				bigBullets[i]->SetPosition(shotPosition);
				shotLugTime = 120;
				blinkFlag = true;
				break;
			}
		}
	}

	if (shotLugTime > 0)
	{//再発射するまでのラグタイム
		shotFlag = false;
		shotLugTime--;
	}
	else
	{
		shotFlag = true;
	}

	for (int i = 0; i < bigBullets.size(); i++)
	{
		if (bigBullets[i]->GetIsDeadFlag() == false)
		{
			Vector3 targetPosition;
			targetPosition = arg_playerPosition;

			Vector3 dir = { arg_playerPosition.x - position.x,arg_playerPosition.y - position.y, arg_playerPosition.z - position.z };
			dir.normalize();
			bigBullets[i]->SetVelocity({ dir.x,dir.y,dir.z });
			Vector3 rotation = bigBullets[i]->GetRotasion();
			rotation.z += 2.0f;
			bigBullets[i]->SetRotation(rotation);
			bigBullets[i]->Update(arg_incrementValue, targetPosition);

			if (bigBullets[i]->GetPosition().z < position.z - SCREENBACK)
			{
				bigBullets[i]->SetIsDeadFlag(true);
			}
		}
	}
}

void FinalBoss::Blink()
{
	Vector3 eyeScale = finalBossEye->GetScale();
	if (blinkAngle < 180.0f)
	{
		//目のY軸のスケールを小さくする
		blinkAngle += 8.0f;
		eyeScale.y = 3.0f * (1 - sin((blinkAngle / 180.0f) * XM_PI));
	}
	else
	{
		blinkFlag = false;
		blinkAngle = 0.0f;
		eyeScale.y = 3.0f;
	}

	finalBossEye->SetScale(eyeScale);
}

void FinalBoss::EntryPerformance()
{
	Vector3 leftWingRotation = parentLeftWing->GetRotasion();
	Vector3 rightWingRotation = parentRightWing->GetRotasion();
	Vector3 eyeScale = finalBossEye->GetScale();

	//両翼が開いて目が完全に開くまで処理を続ける
	if (leftWingRotation.y > 180.0f || rightWingRotation.y < -180.0f || eyeScale.y < 3.0f)
	{
		const float bodyAngleIncrementSize = 0.5f;
		bodyAngle += bodyAngleIncrementSize;
		const float bodyMaxAngle = 360;
		if (bodyAngle > bodyMaxAngle)
		{
			bodyAngle = 0.0f;
		}
		//体の呼吸処理
		bodyBlock->SetPosition({ position.x,  position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });
		bodyBlock->Update({ 0,0,0 });

		//目の呼吸処理
		finalBossEye->SetPosition({ position.x,2.0f + position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f , position.z - 13.5f });

		//目を開く処理
		const float maxEyeScale = 3.0f;
		if (eyeScale.y < maxEyeScale)
		{
			const float eyeScaleIncrementSize = 0.005f;
			eyeScale.y += eyeScaleIncrementSize;
		}
		finalBossEye->SetScale(eyeScale);

		//左翼を開く処理
		parentLeftWing->SetPosition({ -3.0f,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });
		const float maxLeftWingRotation = 180.0f;
		if (leftWingRotation.y > maxLeftWingRotation)
		{
			const float leftWingRotationIncrementSize = 0.1f;
			leftWingRotation.y -= leftWingRotationIncrementSize;
		}

		parentLeftWing->SetRotation(leftWingRotation);
		parentLeftWing->Update({ 0,0,0 });
		for (int i = 0; i < leftWing.size(); ++i)
		{
			leftWing[i]->Update({ 0,0,0 });
		}

		//右翼を開く処理
		parentRightWing->SetPosition({ 3.0f,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });
		const float maxRightWingRotation = -180.0f;
		if (rightWingRotation.y < maxRightWingRotation)
		{
			const float rightWingRotationIncrementSize = 0.1f;
			rightWingRotation.y += rightWingRotationIncrementSize;
		}

		parentRightWing->SetRotation(rightWingRotation);
		parentRightWing->Update({ 0,0,0 });
		for (int i = 0; i < rightWing.size(); ++i)
		{
			rightWing[i]->Update({ 0,0,0 });
		}
	}
	else
	{
		entryFlag = false;
	}
}

void FinalBoss::InitialPosture()
{
	//左翼の初期姿勢
	const Vector3 initialLeftWingRotation = { 0.0f,270.0f,0.0f };
	parentLeftWing->SetRotation(initialLeftWingRotation);
	//右翼の初期姿勢
	const Vector3 initialRightWingRotation = { 0.0f,-270.0f,0.0f };
	parentRightWing->SetRotation(initialRightWingRotation);
	//目の初期姿勢
	const Vector3 initialEyeScale = { 3.0f,0.0f,1.0f };
	finalBossEye->SetScale(initialEyeScale);
}

void FinalBoss::NormalPosture()
{
	//左翼の通常状態
	parentLeftWing->SetRotation(previousParentLeftWingRotation);
	//右翼の通常状態
	parentRightWing->SetRotation(previousParentRightWingRotation);
	//目の通常状態
	finalBossEye->SetScale({ 3.0f,3.0f,1.0f });
}

void FinalBoss::EndPerformance()
{
	bodyBlock->SetPosition({ position.x,  position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });
	bodyBlock->Update({ 0,0,0 });
	//親オブジェクトの解除
	parentLeftWing->IsDead();
	for (int i = 0; i < leftWing.size(); ++i)
	{
		//翼を向きによってバラバラに飛ばす
		const int windNum = i % 3;
		if (windNum == 0)
		{
			Vector3 velocity = { -1.0f,-1.0f,0.0f };
			velocity = velocity.normalize();
			leftWing[i]->SetVelocity(velocity);
		}
		else if (windNum == 1)
		{
			Vector3 velocity = { -1.0f,0.0f,0.0f };
			velocity = velocity.normalize();
			leftWing[i]->SetVelocity(velocity);
		}
		else if (windNum == 2)
		{
			Vector3 velocity = { -1.0f,1.0f,0.0f };
			velocity = velocity.normalize();
			leftWing[i]->SetVelocity(velocity);
		}
		leftWing[i]->Update({ 0,0,0 });
	}
	//親オブジェクトの解除
	parentRightWing->IsDead();
	for (int i = 0; i < rightWing.size(); ++i)
	{
		//翼を向きによってバラバラに飛ばす
		const int windNum = i % 3;
		if (windNum == 0)
		{
			Vector3 velocity = { 1.0f,-1.0f,0.0f };
			velocity = velocity.normalize();
			rightWing[i]->SetVelocity(velocity);
		}
		else if (windNum == 1)
		{
			Vector3 velocity = { 1.0f,0.0f,0.0f };
			velocity = velocity.normalize();
			rightWing[i]->SetVelocity(velocity);
		}
		else if (windNum == 2)
		{
			Vector3 velocity = { 1.0f,1.0f,0.0f };
			velocity = velocity.normalize();
			rightWing[i]->SetVelocity(velocity);
		}
		rightWing[i]->Update({ 0,0,0 });
	}


	const Vector3 basicEyePosition = { 0.0f,2.0f ,-13.5f };
	//落下処理
	finalBossEye->SetPosition({ basicEyePosition.x + position.x,basicEyePosition.y + position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f ,  basicEyePosition.z + position.z});

	Vector3 eyeScale = finalBossEye->GetScale();
	if (eyeScale.y > 0.0f)
	{//目を瞑る処理
		eyeScale.y -= 0.005f;
		finalBossEye->SetScale(eyeScale);
	}
	else
	{//目を瞑ったら死亡フラグをtrueにする
		finalBossEye->IsDead();
	}
}

void FinalBoss::BodyAttack()
{
	Vector3 bodyScale = bodyBlock->GetScale();
	Vector3 parentLeftWingRotation = parentLeftWing->GetRotasion();
	Vector3 parentRightWingRotation = parentRightWing->GetRotasion();
	const float incrementPosition = 1.0f;
	const float incrementScale = 0.005f;
	const float incrementLeftWingRotation = 5.0f;
	const float incrementRightWingRotation = 5.0f;
	const float leftBasePositionX = -3.0f;
	const float rightBasePositionX = 3.0f;
	const float basicEyePositionZ = -13.5f;
	switch (bodyAttackPhase)
	{
	case MOVEPHASE::PHASE1://上に上がる処理

		position.y += incrementPosition;


		finalBossEye->SetPosition({ position.x,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f ,position.z + basicEyePositionZ });

		if (bodyScale.x > 0.0f)
		{
			bodyScale.x -= incrementScale;
		}

		bodyBlock->SetScale(bodyScale);
		bodyBlock->SetPosition({ position.x,  position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });
		bodyBlock->Update({ 0,0,0 });


		if (parentLeftWingRotation.y < 270.0f)
		{
			parentLeftWingRotation.y -= incrementLeftWingRotation;
		}
		parentLeftWing->SetRotation(parentLeftWingRotation);


		if (parentRightWingRotation.y > -270.0f)
		{
			parentRightWingRotation.y += incrementRightWingRotation;
		}
		parentRightWing->SetRotation(parentRightWingRotation);

		parentLeftWing->SetPosition({ leftBasePositionX,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });

		parentLeftWing->Update({ 0,0,0 });
		for (int i = 0; i < leftWing.size(); ++i)
		{
			leftWing[i]->Update({ 0,0,0 });
		}

		parentRightWing->SetPosition({ rightBasePositionX,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });

		parentRightWing->Update({ 0,0,0 });
		for (int i = 0; i < rightWing.size(); ++i)
		{
			rightWing[i]->Update({ 0,0,0 });
		}

		if (position.y > 100.0f)
		{
			position.z = 0.0f;
			bodyAttackPhase = MOVEPHASE::PHASE2;
		}

		break;

	case MOVEPHASE::PHASE2://体当たり処理

		position.y -= incrementPosition;


		finalBossEye->SetPosition({ position.x, position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f , position.z + basicEyePositionZ });


		bodyBlock->SetPosition({ position.x,  position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });
		bodyBlock->Update({ 0,0,0 });


		parentLeftWing->SetPosition({ leftBasePositionX,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });

		parentLeftWing->Update({ 0,0,0 });
		for (int i = 0; i < leftWing.size(); ++i)
		{
			leftWing[i]->Update({ 0,0,0 });
		}


		parentRightWing->SetPosition({ rightBasePositionX,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });

		parentRightWing->Update({ 0,0,0 });
		for (int i = 0; i < rightWing.size(); ++i)
		{
			rightWing[i]->Update({ 0,0,0 });
		}

		if (position.y < -100.0f)
		{
			position.z = 50.0f;
			NormalPosture();
			bodyAttackPhase = MOVEPHASE::PHASE3;
		}

		break;

	case MOVEPHASE::PHASE3://戻る処理


		position.y += incrementPosition;


		finalBossEye->SetPosition({ position.x,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f ,position.z + basicEyePositionZ });


		bodyBlock->SetScale({ 1.0f,1.0f,1.0f });
		bodyBlock->SetPosition({ position.x,  position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });
		bodyBlock->Update({ 0,0,0 });



		parentLeftWing->SetPosition({ leftBasePositionX,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });

		parentLeftWing->Update({ 0,0,0 });
		for (int i = 0; i < leftWing.size(); ++i)
		{
			leftWing[i]->Update({ 0,0,0 });
		}


		parentRightWing->SetPosition({ rightBasePositionX,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });

		parentRightWing->Update({ 0,0,0 });
		for (int i = 0; i < rightWing.size(); ++i)
		{
			rightWing[i]->Update({ 0,0,0 });
		}

		if (position.y > 0.0f)
		{
			bodyAttackFlag = false;
			bodyAttackPhase = MOVEPHASE::PHASE1;
		}

		break;

	default:
		break;
	}
}
